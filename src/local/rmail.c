#include <sys_defs.h>
#include <unistd.h>
#include <string.h>

/* Utility library. */

#include <msg.h>
#include <vstring.h>
#include <htable.h>

/* Global library. */

#include <mail_proto.h>
#include <resolve_clnt.h>
#include <rewrite_clnt.h>
#include <tok822.h>
#include <mail_params.h>
#include <defer.h>

/* Application-specific. */

#include "local.h"

#include <mysql.h>
#include <pwd.h>



extern char *rmail_mdir_lookup(const char *name) {
  const char *myname = "rmail_mdir_lookup";
  char *username, *domain, *mhost, *mbox, *basedir;
  int domain_idx;
  int mailcache_hitted=0;
  struct passwd *pw;
  char sql[512];
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *result;

  username = (char *) malloc (sizeof(char *) * strlen(name));
  username = strdup(name);

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
      return 0;
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
  } else {
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
      return 0;
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
	} else {
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
      return 0;
    }

    res = mysql_store_result(&mta_dbh);
    if (mysql_num_rows(res)!=1) {
      msg_warn("%s: No such user but want to deliver(%s)", myname, username);
      free(username);
      return 0;
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
      result = concatenate(pw->pw_dir, "/", (char *) 0);
      free(username);
      return result;
    } else {
      msg_warn("%s: Not allow unix local account but want to deliver(%s)", myname, username);
      free(username);
      return 0;
    }
  }

#define LAST_CHAR(s) (s[strlen(s) - 1])

  if (LAST_CHAR(mbox) != '/')
    result = concatenate(basedir, "/", mhost, "/", mbox, (char *) 0);
  else
    result = concatenate(basedir, "/", mhost, "/", mbox, "/", (char *) 0);


  free(username);
  return result;
  
}
