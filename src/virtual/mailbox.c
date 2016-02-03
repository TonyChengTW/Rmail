/*++
/* NAME
/*	mailbox 3
/* SUMMARY
/*	mailbox delivery
/* SYNOPSIS
/*	#include "virtual.h"
/*
/*	int	deliver_mailbox(state, usr_attr, statusp)
/*	LOCAL_STATE state;
/*	USER_ATTR usr_attr;
/*	int	*statusp;
/* DESCRIPTION
/*	deliver_mailbox() delivers to UNIX-style mailbox or to maildir.
/*
/*	A zero result means that the named user was not found.
/*
/*	Arguments:
/* .IP state
/*	The attributes that specify the message, recipient and more.
/* .IP usr_attr
/*	Attributes describing user rights and mailbox location.
/* .IP statusp
/*	Delivery status: see below.
/* DIAGNOSTICS
/*	The message delivery status is non-zero when delivery should be tried
/*	again.
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/* Utility library. */

#include <msg.h>
#include <vstring.h>
#include <vstream.h>
#include <mymalloc.h>
#include <stringops.h>
#include <set_eugid.h>

/* Global library. */

#include <mail_copy.h>
#include <mbox_open.h>
#include <defer.h>
#include <sent.h>
#include <mail_params.h>
#include <mail_addr_find.h>


#include <mysql.h>
#include <pwd.h>

#ifndef EDQUOT
#define EDQUOT EFBIG
#endif

/* Application-specific. */

#include "virtual.h"

#define YES	1
#define NO	0


MYSQL mta_dbh;
MYSQL cac_dbh;

/* deliver_mailbox_file - deliver to recipient mailbox */

static int deliver_mailbox_file(LOCAL_STATE state, USER_ATTR usr_attr)
{
    char   *myname = "deliver_mailbox_file";
    VSTRING *why;
    MBOX   *mp;
    int     mail_copy_status;
    int     deliver_status;
    int     copy_flags;
    long    end;
    struct stat st;

    /*
     * Make verbose logging easier to understand.
     */
    state.level++;
    if (msg_verbose)
	MSG_LOG_STATE(myname, state);

    /*
     * Don't deliver trace-only requests.
     */
    if (DEL_REQ_TRACE_ONLY(state.request->flags))
	return (sent(BOUNCE_FLAGS(state.request), SENT_ATTR(state.msg_attr),
		     "delivers to mailbox"));

    /*
     * Initialize. Assume the operation will fail. Set the delivered
     * attribute to reflect the final recipient.
     */
    if (vstream_fseek(state.msg_attr.fp, state.msg_attr.offset, SEEK_SET) < 0)
	msg_fatal("seek message file %s: %m", VSTREAM_PATH(state.msg_attr.fp));
    state.msg_attr.delivered = state.msg_attr.recipient;
    mail_copy_status = MAIL_COPY_STAT_WRITE;
    why = vstring_alloc(100);

    /*
     * Lock the mailbox and open/create the mailbox file.
     * 
     * Write the file as the recipient, so that file quota work.
     */
    copy_flags = MAIL_COPY_MBOX;

    set_eugid(usr_attr.uid, usr_attr.gid);
    mp = mbox_open(usr_attr.mailbox, O_APPEND | O_WRONLY | O_CREAT,
		   S_IRUSR | S_IWUSR, &st, -1, -1,
		   virtual_mbox_lock_mask, why);
    if (mp != 0) {
	if (S_ISREG(st.st_mode) == 0) {
	    vstream_fclose(mp->fp);
	    vstring_sprintf(why, "destination is not a regular file");
	    errno = 0;
	} else {
	    end = vstream_fseek(mp->fp, (off_t) 0, SEEK_END);
	    mail_copy_status = mail_copy(COPY_ATTR(state.msg_attr), mp->fp,
					 copy_flags, "\n", why);
	}
	mbox_release(mp);
    }
    set_eugid(var_owner_uid, var_owner_gid);

    /*
     * As the mail system, bounce, defer delivery, or report success.
     */
    if (mail_copy_status & MAIL_COPY_STAT_CORRUPT) {
	deliver_status = DEL_STAT_DEFER;
    } else if (mail_copy_status != 0) {
	deliver_status = (errno == EDQUOT || errno == EFBIG ?
			  bounce_append : defer_append)
	    (BOUNCE_FLAGS(state.request), BOUNCE_ATTR(state.msg_attr),
	     "mailbox %s: %s", usr_attr.mailbox, vstring_str(why));
    } else {
	deliver_status = sent(BOUNCE_FLAGS(state.request),
			      SENT_ATTR(state.msg_attr),
			      "delivered to mailbox");
    }
    vstring_free(why);
    return (deliver_status);
}

/* deliver_mailbox - deliver to recipient mailbox */

int     deliver_mailbox(LOCAL_STATE state, USER_ATTR usr_attr, int *statusp)
{
    char   *myname = "deliver_mailbox";
    const char *mailbox_res;
    const char *uid_res;
    const char *gid_res;
    long    n;

    /* Rmail */
    char *username, *domain, *mhost, *mbox, *basedir, *bare_addr;
    int domain_idx;
    int mailcache_hitted=0;
    struct passwd *pw;
    char sql[512];
    MYSQL_RES *res;
    MYSQL_ROW row;

    /*
     * Make verbose logging easier to understand.
     */
    state.level++;
    if (msg_verbose)
	MSG_LOG_STATE(myname, state);

    /*
     * Sanity check.
     */
    
    /* Rmail: Disable trandictional virtual mailbox */
    /*
    if (*var_virt_mailbox_base != '/')
	msg_fatal("do not specify relative pathname: %s = %s",
		  VAR_VIRT_MAILBOX_BASE, var_virt_mailbox_base);
    */

    /*
     * Look up the mailbox location. Bounce if not found, defer in case of
     * trouble.
     */
#define IGNORE_EXTENSION ((char **) 0)

    /* Rmail: Disable any default map lookup */
    /*
    mailbox_res = mail_addr_find(virtual_mailbox_maps, state.msg_attr.user,
				 IGNORE_EXTENSION);
    if (mailbox_res == 0) {
	if (dict_errno == 0)
	    return (NO);

	*statusp = defer_append(BOUNCE_FLAGS(state.request),
				BOUNCE_ATTR(state.msg_attr),
				"%s: lookup %s: %m",
			  virtual_mailbox_maps->title, state.msg_attr.user);
	return (YES);
    }
    usr_attr.mailbox = concatenate(var_virt_mailbox_base, "/",
				   mailbox_res, (char *) 0);
    */

#define RETURN(res) { myfree(usr_attr.mailbox); return (res); }

    /*
     * Look up the mailbox owner rights. Defer in case of trouble.
     */
    /* Rmail: Disable any default map lookup */
    /*
    uid_res = mail_addr_find(virtual_uid_maps, state.msg_attr.user,
			     IGNORE_EXTENSION);
    if (uid_res == 0) {
	*statusp = defer_append(BOUNCE_FLAGS(state.request),
				BOUNCE_ATTR(state.msg_attr),
				"recipient %s: uid not found in %s",
			      state.msg_attr.user, virtual_uid_maps->title);
	RETURN(YES);
    }
    if ((n = atol(uid_res)) < var_virt_minimum_uid) {
	*statusp = defer_append(BOUNCE_FLAGS(state.request),
				BOUNCE_ATTR(state.msg_attr),
				"recipient %s: bad uid %s in %s",
		     state.msg_attr.user, uid_res, virtual_uid_maps->title);
	RETURN(YES);
    }
    usr_attr.uid = (uid_t) n;
    */

    /*
     * Look up the mailbox group rights. Defer in case of trouble.
     */
    /* Rmail: Disable any default map lookup */
    /*
    gid_res = mail_addr_find(virtual_gid_maps, state.msg_attr.user,
			     IGNORE_EXTENSION);
    if (gid_res == 0) {
	*statusp = defer_append(BOUNCE_FLAGS(state.request),
				BOUNCE_ATTR(state.msg_attr),
				"recipient %s: gid not found in %s",
			      state.msg_attr.user, virtual_gid_maps->title);
	RETURN(YES);
    }
    if ((n = atol(gid_res)) <= 0) {
	*statusp = defer_append(BOUNCE_FLAGS(state.request),
				BOUNCE_ATTR(state.msg_attr),
				"recipient %s: bad gid %s in %s",
		     state.msg_attr.user, gid_res, virtual_gid_maps->title);
	RETURN(YES);
    }
    usr_attr.gid = (gid_t) n;
    */


    /* Rmail: Lookup maildir, uid, gid */
    username = (char *) malloc (sizeof(char *) * sizeof(state.msg_attr.user));
    username = strdup(state.msg_attr.user);

    if ((domain = strrchr(username, '@')) == 0) { // no @
      domain_idx = 1;
      sprintf(sql, "SELECT %s, %s FROM %s WHERE %s=%d",
	  var_rmail_transport_basedirfield, var_rmail_transport_domainfield,
	  var_rmail_transport_table, var_rmail_transport_idxfield, domain_idx);
      if (msg_debug)
	msg_info("%s: SQL[MTA] => %s", myname, sql);

      if (mysql_real_query(&mta_dbh, sql, strlen(sql)) != 0) {
	msg_warn("%s: MTA Database query fail: %s", myname, mysql_error(&mta_dbh));
	free(username);
	return(NO);
      }
      res = mysql_store_result(&mta_dbh);
      if (mysql_num_rows(res) != 1) {
	msg_warn("%s: No default domain but want to deliver(%s)", myname, username);
	domain = strdup(var_mydomain);
	basedir = strdup(var_rmail_default_basedir);
      } else {
        row = mysql_fetch_row(res);
        basedir = strdup(row[0]);
        domain = strdup(row[1]);
      }
      mysql_free_result(res);
    } else { // had @
      username[strlen(username)-strlen(domain)]='\0';
      domain++;

      sprintf(sql, "SELECT %s, %s FROM %s WHERE %s='%s'",
	  var_rmail_transport_basedirfield, var_rmail_transport_idxfield,
	  var_rmail_transport_table, var_rmail_transport_domainfield,
	  domain);

      if (msg_debug)
	msg_info("%s: SQL[MTA] => %s", myname, sql);

      if (mysql_real_query(&mta_dbh, sql, strlen(sql)) != 0) {
	msg_warn("%s: MTA Database query fail: %s", myname, mysql_error(&mta_dbh));
	free(username);
	return(NO);
      }

      res = mysql_store_result(&mta_dbh);
      if (mysql_num_rows(res) != 1) {
	msg_warn("%s: No such domain but want to deliver(%s)", myname, domain);
	basedir = strdup(var_rmail_default_basedir);
	domain_idx = 1;
      } else {
	row = mysql_fetch_row(res);
	basedir = strdup(row[0]);
	domain_idx = atoi(row[1]);
  	mysql_free_result(res);
      }
    }

    if (msg_debug)
      msg_info("%s: Get domain_idx=%d, basedir=%s", myname, domain_idx, basedir);

    /* Find mailuser/mailcache first */
    if (var_rmail_mailcache_enable) {
      sprintf(sql, "SELECT %s, %s, UNIX_TIMESTAMP(%s) FROM %s WHERE %s='%s' AND %s='%d'",
	  var_rmail_mailcache_mhostfield, var_rmail_mailcache_mboxfield,
	  var_rmail_mailcache_timefield, var_rmail_mailcache_table,
	  var_rmail_mailcache_mailidfield, username,
	  var_rmail_mailcache_domainfield, domain_idx);

      if (msg_debug)
	msg_info("%s: SQL[CAC] => %s", myname, sql);

      if (mysql_real_query(&cac_dbh, sql, strlen(sql))!=0) { // query fail
	msg_warn("%s: CAC Database query fail: %s", myname, mysql_error(&cac_dbh));
	mailcache_hitted = 0;
      } else {
	res = mysql_store_result(&cac_dbh);
	if (mysql_num_rows(res)!=1) { // no cache hitted
	  mailcache_hitted = 0;
	} else { // check if expire
	  row = mysql_fetch_row(res);
	  if (atoi(row[2]) + var_rmail_mailcache_expire < time(NULL)) { // cache expire, delete it
	    sprintf(sql, "DELETE FROM %s WHERE %s='%s' AND %s=%d",
		var_rmail_mailcache_table, var_rmail_mailcache_mailidfield, username, 
		var_rmail_mailcache_domainfield, domain_idx);
	    if (msg_debug)
	      msg_info("%s: SQL[CAC] => %s", myname, sql);

	    if (mysql_real_query(&cac_dbh, sql, strlen(sql))!=0)
	      msg_warn("%s: CAC Database query fail: %s", myname, mysql_error(&cac_dbh));
	    
	    mailcache_hitted = 0;
	  } else { // cache not expire
	    mhost = strdup(row[0]);
	    mbox = strdup(row[1]);
	    mailcache_hitted = 1;
	  }
	}
	mysql_free_result(res);
      }
    } else
      mailcache_hitted = 0;
    
    // mailcache disable or no cache hitted 

    if (!mailcache_hitted) {
      sprintf(sql, "SELECT %s, %s, %s, %s, %s, %s FROM %s WHERE %s='%s' AND %s=%d",
	  var_rmail_mailuser_mhostfield, var_rmail_mailuser_mboxfield, var_rmail_mailuser_statufield,
	  var_rmail_mailuser_smtpfield, var_rmail_mailuser_pop3field, var_rmail_mailuser_webfield,
	  var_rmail_mailuser_table, var_rmail_mailuser_mailidfield, username,
	  var_rmail_mailuser_domainfield, domain_idx);
      if (msg_debug)
	msg_info("%s: SQL[MTA] => %s", myname, sql);
      if (mysql_real_query(&mta_dbh, sql, strlen(sql))!=0) {
	msg_warn("%s: MTA Database query fail(%s)", myname, mysql_error(&mta_dbh));
	free(username);
	return(NO);
      }

      res = mysql_store_result(&mta_dbh);
      if (mysql_num_rows(res)!=1) {
	msg_warn("%s: No such user but want to deliver(%s)", myname, username);
	free(username);
	return(NO);
      }
      row = mysql_fetch_row(res);
      mhost = strdup(row[0]);
      mbox = strdup(row[1]);
      mailcache_hitted = 1;

      if (var_rmail_mailcache_enable) {
	sprintf(sql, "INSERT INTO %s SET %s='%s', %s=%d, %s='%s', %s='%s', %s=%d, %s=NOW(), %s=%d, %s=%d, %s=%d",
	    var_rmail_mailcache_table, var_rmail_mailcache_mailidfield, username,
	    var_rmail_mailcache_domainfield, domain_idx, var_rmail_mailcache_mhostfield, mhost,
	    var_rmail_mailcache_mboxfield, row[1],
	    var_rmail_mailcache_statufield, atoi(row[2]), var_rmail_mailcache_timefield,
	    var_rmail_mailcache_smtpfield, atoi(row[3]),
	    var_rmail_mailcache_pop3field, atoi(row[4]),
	    var_rmail_mailcache_webfield, atoi(row[5]));
	if (msg_debug)
	  msg_info("%s: SQL[CAC] => %s", myname, sql);

	if (mysql_real_query(&cac_dbh, sql, strlen(sql))!=0)
	  msg_warn("%s: CAC Database query fail(%s)", myname, mysql_error(&cac_dbh));
      }
      mysql_free_result(res);
    }

    if (!mailcache_hitted) { // no any record? check unix local account
      pw = getpwnam(username);
      if (pw && var_rmail_allow_local) { // enable local account
	usr_attr.check_quota=0;
	usr_attr.uid = pw->pw_uid;
	usr_attr.gid = pw->pw_gid;
	usr_attr.mailbox = concatenate(pw->pw_dir, "/", var_rmail_maildir_sufix, "/", (char *) 0);
      } else { // disable local account
	msg_warn("%s: Not allow unix local account but want to deliver(%s)", myname, username);
	free(username);
	return(NO);
      }
    } else {
      usr_attr.uid = var_owner_uid;
      usr_attr.gid = var_owner_gid;
      usr_attr.check_quota = 1;
    }

#define LAST_CHAR(s) (s[strlen(s) - 1])


    if (LAST_CHAR(var_rmail_maildir_sufix) != '/') {
      usr_attr.mailbox = concatenate(basedir, "/", mhost, "/", 
	  mbox, "/", var_rmail_maildir_sufix, "/", (char *) 0);
    } else {
      usr_attr.mailbox = concatenate(basedir, "/", mhost, "/", 
	  mbox, "/", var_rmail_maildir_sufix, (char *) 0);
    }



    /* Rmail end */

    if (msg_verbose)
	msg_info("%s[%d]: set user_attr: %s, uid = %u, gid = %u",
		 myname, state.level, usr_attr.mailbox,
		 (unsigned) usr_attr.uid, (unsigned) usr_attr.gid);

    /*
     * Deliver to mailbox or to maildir.
     */

    /* Rmail: disable mailbox/maildir check */
    /*
    if (LAST_CHAR(usr_attr.mailbox) == '/')
	*statusp = deliver_maildir(state, usr_attr);
    else
	*statusp = deliver_mailbox_file(state, usr_attr);
    */
    
    /*
     * Cleanup.
     */
    *statusp = deliver_maildir(state, usr_attr);
    free(username);
    RETURN(YES);
}
