/*++
/* NAME
/*	qmgr_message 3
/* SUMMARY
/*	in-core message structures
/* SYNOPSIS
/*	#include "qmgr.h"
/*
/*	int	qmgr_message_count;
/*	int	qmgr_recipient_count;
/*
/*	QMGR_MESSAGE *qmgr_message_alloc(class, name, qflags)
/*	const char *class;
/*	const char *name;
/*	int	qflags;
/*
/*	QMGR_MESSAGE *qmgr_message_realloc(message)
/*	QMGR_MESSAGE *message;
/*
/*	void	qmgr_message_free(message)
/*	QMGR_MESSAGE *message;
/*
/*	void	qmgr_message_update_warn(message)
/*	QMGR_MESSAGE *message;
/*
/*	void	qmgr_message_kill_record(message, offset)
/*	QMGR_MESSAGE *message;
/*	long	offset;
/* DESCRIPTION
/*	This module performs en-gross operations on queue messages.
/*
/*	qmgr_message_count is a global counter for the total number
/*	of in-core message structures (i.e. the total size of the
/*	`active' message queue).
/*
/*	qmgr_recipient_count is a global counter for the total number
/*	of in-core recipient structures (i.e. the sum of all recipients
/*	in all in-core message structures).
/*
/*	qmgr_message_alloc() creates an in-core message structure
/*	with sender and recipient information taken from the named queue
/*	file. A null result means the queue file could not be read or
/*	that the queue file contained incorrect information. A result
/*	QMGR_MESSAGE_LOCKED means delivery must be deferred. The number
/*	of recipients read from a queue file is limited by the global
/*	var_qmgr_rcpt_limit configuration parameter. When the limit
/*	is reached, the \fIrcpt_offset\fR structure member is set to
/*	the position where the read was terminated. Recipients are
/*	run through the resolver, and are assigned to destination
/*	queues. Recipients that cannot be assigned are deferred or
/*	bounced. Mail that has bounced twice is silently absorbed.
/*
/*	qmgr_message_realloc() resumes reading recipients from the queue
/*	file, and updates the recipient list and \fIrcpt_offset\fR message
/*	structure members. A null result means that the file could not be
/*	read or that the file contained incorrect information. Recipient
/*	limit imposed this time is based on the position of the message
/*	job(s) on corresponding transport job list(s). It's considered
/*	an error to call this when the recipient slots can't be allocated.
/*
/*	qmgr_message_free() destroys an in-core message structure and makes
/*	the resources available for reuse. It is an error to destroy
/*	a message structure that is still referenced by queue entry structures.
/*
/*	qmgr_message_update_warn() takes a closed message, opens it, updates
/*	the warning field, and closes it again.
/*
/*	qmgr_message_kill_record() takes a closed message, opens it, updates
/*	the record type at the given offset to "killed", and closes the file.
/*	A killed envelope record is ignored. Killed records are not allowed
/*	inside the message content.
/* DIAGNOSTICS
/*	Warnings: malformed message file. Fatal errors: out of memory.
/* SEE ALSO
/*	envelope(3) message envelope parser
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*
/*	Scheduler enhancements:
/*	Patrik Rak
/*	Modra 6
/*	155 00, Prague, Czech Republic
/*--*/

/* System library. */

#include <sys_defs.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>			/* sscanf() */
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#ifdef STRCASECMP_IN_STRINGS_H
#include <strings.h>
#endif

/* Utility library. */

#include <msg.h>
#include <mymalloc.h>
#include <vstring.h>
#include <vstream.h>
#include <split_at.h>
#include <valid_hostname.h>
#include <argv.h>
#include <stringops.h>
#include <myflock.h>
#include <sane_time.h>

/* Global library. */

#include <dict.h>
#include <mail_queue.h>
#include <mail_params.h>
#include <canon_addr.h>
#include <record.h>
#include <rec_type.h>
#include <sent.h>
#include <deliver_completed.h>
#include <opened.h>
#include <verp_sender.h>
#include <mail_proto.h>
#include <qmgr_user.h>

/* Client stubs. */

#include <rewrite_clnt.h>
#include <resolve_clnt.h>

#include <mysql.h>

/* Application-specific. */

#include "qmgr.h"

int     qmgr_message_count;
int     qmgr_recipient_count;

static int rmail_resolve(RESOLVE_REPLY reply, QMGR_RCPT *recipient) {
  const char *myname = "rmail_resolve";
  MYSQL_RES *res;
  MYSQL_ROW row;
  char sql[512];
  char *username, *domain, *bare_addr;
  int domain_idx;
  char mhost[64];
  char rhost[64];
  struct passwd *pw;
  int mailcache_hitted = 0;
  int hostmapcache_hitted = 0;
  int domaincache_hitted = 0;
  HOSTMAP_ENTRY *cur_hostmap;
  TRANSPORT_ENTRY *cur_transport;

#define RET(re) {free(username); return re;}

  recipient->del =0;

  if (msg_debug)
    msg_info("%s: Rmail resolve => %s", myname, vstring_str(reply.recipient));

  // get my hostname
  if (msg_debug)
    msg_info("%s: My hostname is %s", myname, myhostname);
  

  // delim address
  if (*var_rcpt_delim == 0) {
    bare_addr = 0;
  } else {
    bare_addr = strip_addr(vstring_str(reply.recipient), (char **) 0, *var_rcpt_delim);
  }

  if (bare_addr == 0) {
    username = (char *) malloc (sizeof(char *) * strlen(vstring_str(reply.recipient)));
    username = strdup(vstring_str(reply.recipient));
  } else {
    username = (char *) malloc (sizeof(char *) * strlen(bare_addr));
    username = strdup(bare_addr);
  }
  

  // find domain_idx
  if ((domain = strrchr(username, '@')) == 0) {
    domain_idx = 1;
  } else {
    domain_idx = 0;
    username[strlen(username)-strlen(domain)]='\0';
    domain++;

    if (var_rmail_domaincache_enable) {
      cur_transport = tran_cache;
      while (cur_transport != NULL) {
	if (strcmp(cur_transport->domain, domain)==0) {
	  domain_idx = cur_transport->idx;
	  domaincache_hitted = 1;
	  if (msg_debug)
	    msg_info("%s: Domain cache hitted => %d", myname, domain_idx);
	  break;
	}
	cur_transport=cur_transport->next_entry;
      }
    }

    if (domaincache_hitted == 0) { // not hitted or disable domain cache
      sprintf(sql, "SELECT %s FROM %s WHERE %s='%s'",
	  var_rmail_transport_idxfield, var_rmail_transport_table,
	  var_rmail_transport_domainfield, domain);
      if (msg_debug)
	msg_info("%s: SQL[MTA] => %s", myname, sql);

      if (mysql_real_query(&mta_dbh, sql, strlen(sql)) != 0) {
	msg_warn("%s: MTA Database query fail(%s)", myname, mysql_error(&mta_dbh));
	vstring_strcpy(reply.transport, "bounce");
	vstring_strcpy(reply.reason, "Query fail");
	RET(0);
      } else {
	res = mysql_store_result(&mta_dbh);
	if (mysql_num_rows(res)==1) {
	  row = mysql_fetch_row(res);
	  domain_idx = atoi(row[0])? atoi(row[0]):0;
	} else {
	  domain_idx = 0;
	}
	mysql_free_result(res);
      }
    }
  }
  
  if (msg_debug)
    msg_info("%s: Got domain idx %d for %s", myname, domain_idx, vstring_str(reply.recipient));
  

  // get mhost 

  if (domain_idx == 0) { // remote user
    if (var_rmail_ismx) {
      msg_warn("%s: MX doesn't relay any remote bulks(%s)", myname, vstring_str(reply.recipient));
      vstring_strcpy(reply.transport, "bounce");
      vstring_strcpy(reply.reason, "MX doesn't relay any remote bulks");
    } else {
      /* Check if drop domain */
      if (var_rmail_dropdomain_enable && strlen(domain)!=0) {
	sprintf(sql, "SELECT %s FROM %s WHERE %s='%s'",
	    var_rmail_dropdomain_domainfield,
	    var_rmail_dropdomain_table,
	    var_rmail_dropdomain_domainfield, domain);
	if (msg_debug)
	  msg_info("%s: SQL[MTA] => %s", myname, sql);

	if (mysql_real_query(&mta_dbh, sql, strlen(sql)) != 0) {
	  msg_warn("%s: MTA Database query fail(%s)", myname, mysql_error(&mta_dbh));
	} else {
	  res = mysql_store_result(&mta_dbh);
	  if (mysql_num_rows(res) > 0) {
	    reply.del=1;
	    recipient->del=1;
	    vstring_strcpy(reply.transport, "bounce");
	    vstring_strcpy(reply.reason, "In DropDomain");
	  } else {
	    vstring_strcpy(reply.transport, "smtp");
	    vstring_strcpy(reply.nexthop, domain);
	  }
	  mysql_free_result(res);
	}
      } else { // dropdomain disable
	vstring_strcpy(reply.transport, "smtp");
	vstring_strcpy(reply.nexthop, domain);
      }
    }
    RET(0);
  } else { // local user
    pw = getpwnam((const char *) username);

    if (pw && var_rmail_allow_local) { // unix user
      if (strcasecmp(myhostname, var_rmail_local_nexthop)==0) {
	vstring_strcpy(reply.transport, var_rmail_default_deliver);
      } else {
	vstring_strcpy(reply.transport, "smtpx");
	vstring_strcpy(reply.nexthop, var_rmail_local_nexthop);
      }
      RET(0);
    } else { // normal
      mhost[0]='\0'; 
      // find mailuser records & get mhost
      if (var_rmail_mailcache_enable) {
	sprintf(sql, "SELECT %s, UNIX_TIMESTAMP(%s) FROM %s WHERE %s='%s' AND %s=%d",
	    var_rmail_mailcache_mhostfield, var_rmail_mailcache_timefield,
	    var_rmail_mailcache_table, var_rmail_mailcache_mailidfield,
	    username, var_rmail_mailcache_domainfield, domain_idx);
	if (msg_debug)
	  msg_info("%s: SQL[CAC] => %s", myname, sql);

	if (mysql_real_query(&cac_dbh, sql, strlen(sql)) != 0) { // query fail
	  // cache query fail, use mailuser to failover
	  msg_warn("%s: CAC Database query fail(%s)", myname, mysql_error(&cac_dbh));
	  mailcache_hitted = 0;
	} else {
	  res = mysql_store_result(&cac_dbh);
	  if (mysql_num_rows(res) == 1) {
	    row = mysql_fetch_row(res);
	    if (atoi(row[1]) + var_rmail_mailcache_expire < time(NULL)) { // expired
	      mailcache_hitted = 0;
	      sprintf(sql, "DELETE FROM %s WHERE %s='%s' AND %s=%d",
		  var_rmail_mailcache_table, var_rmail_mailcache_mailidfield,
		  username, var_rmail_mailcache_domainfield, domain_idx);
	      if (msg_debug)
		msg_info("%s: SQL[CAC] => %s", myname, sql);

	      if (mysql_real_query(&cac_dbh, sql, strlen(sql))!=0)
		msg_warn("%s: CAC Database query fail(%s)", myname, mysql_error(&cac_dbh));
	    } else { // not expired
	      strcpy(mhost, row[0]);
	      mailcache_hitted = 1;
	    }
	  } else {
	    mailcache_hitted = 0;
	  }
	  mysql_free_result(res);
	}
      }

      if (!var_rmail_mailcache_enable || mailcache_hitted == 0 || strlen(mhost)==0) {
	sprintf(sql, "SELECT %s, %s, %s, %s, %s, %s FROM %s WHERE %s='%s' AND %s=%d",
	    var_rmail_mailuser_mhostfield, var_rmail_mailuser_mboxfield, var_rmail_mailuser_statufield,
	    var_rmail_mailuser_smtpfield, var_rmail_mailuser_pop3field, var_rmail_mailuser_webfield,
	    var_rmail_mailuser_table, var_rmail_mailuser_mailidfield, username,
	    var_rmail_mailuser_domainfield, domain_idx);

	if (msg_debug)
	  msg_info("%s: SQL[MTA] => %s", myname, sql);

	if (mysql_real_query(&mta_dbh, sql, strlen(sql))!=0) {
	  msg_warn("%s: MTA Database query fail(%s)", myname, mysql_error(&mta_dbh));
	  vstring_strcpy(reply.transport, "bounce");
	  vstring_strcpy(reply.reason, "Database query fail");
	  RET(0);
	}

	res = mysql_store_result(&mta_dbh);
	if (mysql_num_rows(res) == 0) {
	  if (msg_debug)
	    msg_warn("%s: No such user(%s[%d]) but exist in queue", myname, username, domain_idx);
	  reply.del=1;
	  recipient->del=1;
	  vstring_strcpy(reply.transport, "bounce");
	  vstring_strcpy(reply.reason, "No such user");
	  mysql_free_result(res);
	  RET(0);
	} else {
	  row = mysql_fetch_row(res);
	  strcpy(mhost, row[0]);
	}

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

	  if (mysql_real_query(&cac_dbh, sql, strlen(sql))!=0) {
	    msg_warn("%s: CAC Database query fail(%s)", myname, mysql_error(&cac_dbh));
	  }
	}
	mysql_free_result(res);
      }

      if (strlen(mhost) == 0) {
	msg_warn("%s: Unknown error, no mhost(%s[%d])", myname, username, domain_idx);
	vstring_strcpy(reply.transport, "bounce");
	vstring_strcpy(reply.reason, "No any mhost to relay");
	reply.del=1;
	recipient->del=1;
	RET(0);
      }

      rhost[0]='\0';
      // check mhost
      if (var_rmail_hostmapcache_enable) {
	cur_hostmap = hmap_cache;
	while (cur_hostmap != NULL) {
	  if (strcmp(cur_hostmap->hostname, mhost)==0) {
	    // hitted!!
	    strcpy(rhost, cur_hostmap->nodename);
	    hostmapcache_hitted=1; 
	    if (msg_debug)
	      msg_info("%s: Hostmap cache hitted => %s", myname, rhost);
	    break;
	  }
	  cur_hostmap = cur_hostmap->next_entry;	  
	}
      }

      if (hostmapcache_hitted == 0) {
	sprintf(sql, "SELECT %s FROM %s WHERE %s='%s'",
	    var_rmail_hostmap_nodenamefield, var_rmail_hostmap_table,
	    var_rmail_hostmap_hostnamefield, mhost);

	if (msg_debug)
	  msg_info("%s: SQL[MTA] => %s", myname, sql);

	if (mysql_real_query(&mta_dbh, sql, strlen(sql))!=0) {
	  msg_warn("%s: MTA Database query fail(%s)", myname, mysql_error(&mta_dbh));
	  vstring_strcpy(reply.transport, "bounce");
	  vstring_strcpy(reply.reason, "Database query fail");
	  RET(0);
	}

	res = mysql_store_result(&mta_dbh);
	if (mysql_num_rows(res) != 1) {
	  msg_warn("%s: Hostmap fail, but try to deliver", myname);
	  strcpy(rhost, mhost);
	} else {
	  row = mysql_fetch_row(res);
	  strcpy(rhost, row[0]);
	}
	mysql_free_result(res);
      }

      if (strlen(rhost) == 0) {
	vstring_strcpy(reply.transport, "bounce");
	vstring_strcpy(reply.reason, "No relay host");
	RET(0);
      }
      
      if (strcasecmp(myhostname, rhost) == 0) {
	vstring_strcpy(reply.transport, var_rmail_default_deliver);
      } else {
	vstring_strcpy(reply.transport, "smtpx");
	vstring_strcpy(reply.nexthop, rhost);
      }
    }
  }
  RET(0);
}





/* qmgr_message_create - create in-core message structure */

static QMGR_MESSAGE *qmgr_message_create(const char *queue_name,
				           const char *queue_id, int qflags)
{
    QMGR_MESSAGE *message;

    message = (QMGR_MESSAGE *) mymalloc(sizeof(QMGR_MESSAGE));
    qmgr_message_count++;
    message->flags = 0;
    message->qflags = qflags;
    message->tflags = 0;
    message->tflags_offset = 0;
    message->rflags = QMGR_READ_FLAG_DEFAULT;
    message->fp = 0;
    message->refcount = 0;
    message->single_rcpt = 0;
    message->arrival_time = 0;
    message->queued_time = sane_time();
    message->data_offset = 0;
    message->queue_id = mystrdup(queue_id);
    message->queue_name = mystrdup(queue_name);
    message->encoding = 0;
    message->sender = 0;
    message->errors_to = 0;
    message->return_receipt = 0;
    message->filter_xport = 0;
    message->inspect_xport = 0;
    message->redirect_addr = 0;
    message->data_size = 0;
    message->warn_offset = 0;
    message->warn_time = 0;
    message->rcpt_offset = 0;
    message->verp_delims = 0;
    message->client_name = 0;
    message->client_addr = 0;
    message->client_proto = 0;
    message->client_helo = 0;
    qmgr_rcpt_list_init(&message->rcpt_list);
    message->rcpt_count = 0;
    message->rcpt_limit = var_qmgr_msg_rcpt_limit;
    message->rcpt_unread = 0;
    QMGR_LIST_INIT(message->job_list);
    return (message);
}

/* qmgr_message_close - close queue file */

static void qmgr_message_close(QMGR_MESSAGE *message)
{
    vstream_fclose(message->fp);
    message->fp = 0;
}

/* qmgr_message_open - open queue file */

static int qmgr_message_open(QMGR_MESSAGE *message)
{

    /*
     * Sanity check.
     */
    if (message->fp)
	msg_panic("%s: queue file is open", message->queue_id);

    /*
     * Open this queue file. Skip files that we cannot open. Back off when
     * the system appears to be running out of resources.
     */
    if ((message->fp = mail_queue_open(message->queue_name,
				       message->queue_id,
				       O_RDWR, 0)) == 0) {
	if (errno != ENOENT)
	    msg_fatal("open %s %s: %m", message->queue_name, message->queue_id);
	msg_warn("open %s %s: %m", message->queue_name, message->queue_id);
	return (-1);
    }
    return (0);
}

/* qmgr_message_oldstyle_scan - support for Postfix < 1.0 queue files */

static void qmgr_message_oldstyle_scan(QMGR_MESSAGE *message)
{
    VSTRING *buf;
    long    orig_offset,
            extra_offset;
    int     rec_type;
    char   *start;

    /*
     * Initialize. No early returns or we have a memory leak.
     */
    buf = vstring_alloc(100);
    if ((orig_offset = vstream_ftell(message->fp)) < 0)
	msg_fatal("vstream_ftell %s: %m", VSTREAM_PATH(message->fp));

    /*
     * Rewind to the very beginning to make sure we see all records.
     */
    if (vstream_fseek(message->fp, 0, SEEK_SET) < 0)
	msg_fatal("seek file %s: %m", VSTREAM_PATH(message->fp));

    /*
     * Scan through the old style queue file. Count the total number of
     * recipients and find the data/extra sections offsets. Note that the new
     * queue files require that data_size equals extra_offset - data_offset,
     * so we set data_size to this as well and ignore the size record itself
     * completely.
     */
    message->rcpt_unread = 0;
    for (;;) {
	rec_type = rec_get(message->fp, buf, 0);
	if (rec_type <= 0)
	    /* Report missing end record later. */
	    break;
	start = vstring_str(buf);
	if (msg_verbose > 1)
	    msg_info("old-style scan record %c %s", rec_type, start);
	if (rec_type == REC_TYPE_END)
	    break;
	if (rec_type == REC_TYPE_DONE || rec_type == REC_TYPE_RCPT) {
	    message->rcpt_unread++;
	    continue;
	}
	if (rec_type == REC_TYPE_MESG) {
	    if (message->data_offset == 0) {
		if ((message->data_offset = vstream_ftell(message->fp)) < 0)
		    msg_fatal("vstream_ftell %s: %m", VSTREAM_PATH(message->fp));
		if ((extra_offset = atol(start)) <= message->data_offset)
		    msg_fatal("bad extra offset %s file %s",
			      start, VSTREAM_PATH(message->fp));
		if (vstream_fseek(message->fp, extra_offset, SEEK_SET) < 0)
		    msg_fatal("seek file %s: %m", VSTREAM_PATH(message->fp));
		message->data_size = extra_offset - message->data_offset;
	    }
	    continue;
	}
    }

    /*
     * Clean up.
     */
    if (vstream_fseek(message->fp, orig_offset, SEEK_SET) < 0)
	msg_fatal("seek file %s: %m", VSTREAM_PATH(message->fp));
    vstring_free(buf);

    /*
     * Sanity checks. Verify that all required information was found,
     * including the queue file end marker.
     */
    if (message->data_offset == 0 || rec_type != REC_TYPE_END)
	msg_fatal("%s: envelope records out of order", message->queue_id);
}

/* qmgr_message_read - read envelope records */

static int qmgr_message_read(QMGR_MESSAGE *message)
{
    VSTRING *buf;
    int     rec_type;
    long    curr_offset;
    long    save_offset = message->rcpt_offset;	/* save a flag */
    int     save_unread = message->rcpt_unread;	/* save a count */
    char   *start;
    int     recipient_limit;
    const char *error_text;
    char   *name;
    char   *value;
    char   *orig_rcpt = 0;
    int     count;

    /*
     * Initialize. No early returns or we have a memory leak.
     */
    buf = vstring_alloc(100);

    /*
     * If we re-open this file, skip over on-file recipient records that we
     * already looked at, and refill the in-core recipient address list.
     * 
     * For the first time, the message recipient limit is calculated from the
     * global recipient limit. This is to avoid reading little recipients
     * when the active queue is near empty. When the queue becomes full, only
     * the necessary amount is read in core. Such priming is necessary
     * because there are no message jobs yet.
     * 
     * For the next time, the recipient limit is based solely on the message
     * jobs' positions in the job lists and/or job stacks.
     */
    if (message->rcpt_offset) {
	if (message->rcpt_list.len)
	    msg_panic("%s: recipient list not empty on recipient reload",
		      message->queue_id);
	if (vstream_fseek(message->fp, message->rcpt_offset, SEEK_SET) < 0)
	    msg_fatal("seek file %s: %m", VSTREAM_PATH(message->fp));
	message->rcpt_offset = 0;
	recipient_limit = message->rcpt_limit - message->rcpt_count;
    } else {
	recipient_limit = var_qmgr_rcpt_limit - qmgr_recipient_count;
	if (recipient_limit < message->rcpt_limit)
	    recipient_limit = message->rcpt_limit;
    }
    if (recipient_limit <= 0)
	msg_panic("%s: no recipient slots available", message->queue_id);

    /*
     * Read envelope records. XXX Rely on the front-end programs to enforce
     * record size limits. Read up to recipient_limit recipients from the
     * queue file, to protect against memory exhaustion. Recipient records
     * may appear before or after the message content, so we keep reading
     * from the queue file until we have enough recipients (rcpt_offset != 0)
     * and until we know all the non-recipient information.
     * 
     * Note that the total recipient count record is accurate only for fresh
     * queue files. After some of the recipients are marked as done and the
     * queue file is deferred, it can be used as upper bound estimate only.
     * Fortunately, this poses no major problem on the scheduling algorithm,
     * as the only impact is that the already deferred messages are not
     * chosen by qmgr_job_candidate() as often as they could.
     * 
     * On the first open, we must examine all non-recipient records.
     * 
     * Optimization: when we know that recipient records are not mixed with
     * non-recipient records, as is typical with mailing list mail, then we
     * can avoid having to examine all the queue file records before we can
     * start deliveries. This avoids some file system thrashing with huge
     * mailing lists.
     */
    for (;;) {
	if ((curr_offset = vstream_ftell(message->fp)) < 0)
	    msg_fatal("vstream_ftell %s: %m", VSTREAM_PATH(message->fp));
	if (curr_offset == message->data_offset && curr_offset > 0) {
	    if (vstream_fseek(message->fp, message->data_size, SEEK_CUR) < 0)
		msg_fatal("seek file %s: %m", VSTREAM_PATH(message->fp));
	    curr_offset += message->data_size;
	}
	rec_type = rec_get(message->fp, buf, 0);
	start = vstring_str(buf);
	if (msg_verbose > 1)
	    msg_info("record %c %s", rec_type, start);
	if (rec_type <= 0) {
	    msg_warn("%s: message rejected: missing end record",
		     message->queue_id);
	    break;
	}
	if (rec_type == REC_TYPE_END) {
	    message->rflags |= QMGR_READ_FLAG_SEEN_ALL_NON_RCPT;
	    break;
	}

	/*
	 * Process recipient records.
	 */
	if (rec_type == REC_TYPE_RCPT) {
	    /* See also below for code setting orig_rcpt. */
	    if (message->rcpt_offset == 0) {
		message->rcpt_unread--;
		qmgr_rcpt_list_add(&message->rcpt_list, curr_offset,
				   orig_rcpt ? orig_rcpt : "", start);
		if (orig_rcpt) {
		    myfree(orig_rcpt);
		    orig_rcpt = 0;
		}
		if (message->rcpt_list.len >= recipient_limit) {
		    if ((message->rcpt_offset = vstream_ftell(message->fp)) < 0)
			msg_fatal("vstream_ftell %s: %m",
				  VSTREAM_PATH(message->fp));
		    if (message->rflags & QMGR_READ_FLAG_SEEN_ALL_NON_RCPT)
			/* We already examined all non-recipient records. */
			break;
		    if (message->rflags & QMGR_READ_FLAG_MIXED_RCPT_OTHER)
			/* Examine all remaining non-recipient records. */
			continue;
		    /* Optimizations for "pure recipient" record sections. */
		    if (curr_offset > message->data_offset) {
			/* We already examined all non-recipient records. */
			message->rflags |= QMGR_READ_FLAG_SEEN_ALL_NON_RCPT;
			break;
		    }
		    /* Examine non-recipient records in extracted segment. */
		    if (vstream_fseek(message->fp, message->data_offset
				      + message->data_size, SEEK_SET) < 0)
			msg_fatal("seek file %s: %m", VSTREAM_PATH(message->fp));
		    continue;
		}
	    }
	    continue;
	}
	if (rec_type == REC_TYPE_DONE) {
	    if (message->rcpt_offset == 0) {
		message->rcpt_unread--;
		if (orig_rcpt) {
		    myfree(orig_rcpt);
		    orig_rcpt = 0;
		}
	    }
	    continue;
	}
	if (orig_rcpt != 0) {
	    /* REC_TYPE_ORCP must go before REC_TYPE_RCPT or REC_TYPE DONE. */
	    msg_warn("%s: ignoring out-of-order original recipient <%.200s>",
		     message->queue_id, orig_rcpt);
	    myfree(orig_rcpt);
	    orig_rcpt = 0;
	}
	if (rec_type == REC_TYPE_ORCP) {
	    /* See also above for code clearing orig_rcpt. */
	    if (message->rcpt_offset == 0)
		orig_rcpt = mystrdup(start);
	    continue;
	}

	/*
	 * Process non-recipient records.
	 */
	if (message->rflags & QMGR_READ_FLAG_SEEN_ALL_NON_RCPT)
	    /* We already examined all non-recipient records. */
	    continue;
	if (rec_type == REC_TYPE_SIZE) {
	    if (message->data_offset == 0) {
		if ((count = sscanf(start, "%ld %ld %d %d",
				 &message->data_size, &message->data_offset,
			   &message->rcpt_unread, &message->rflags)) >= 3) {
		    /* Postfix >= 1.0 (a.k.a. 20010228). */
		    if (message->data_offset <= 0 || message->data_size <= 0) {
			msg_warn("%s: invalid size record: %.100s",
				 message->queue_id, start);
			rec_type = REC_TYPE_ERROR;
			break;
		    }
		    if (message->rflags & ~QMGR_READ_FLAG_USER) {
			msg_warn("%s: invalid flags in size record: %.100s",
				 message->queue_id, start);
			rec_type = REC_TYPE_ERROR;
			break;
		    }
		} else if (count == 1) {
		    /* Postfix < 1.0 (a.k.a. 20010228). */
		    qmgr_message_oldstyle_scan(message);
		} else {
		    /* Can't happen. */
		    msg_warn("%s: message rejected: weird size record",
			     message->queue_id);
		    rec_type = REC_TYPE_ERROR;
		    break;
		}
	    }
	    continue;
	}
	if (rec_type == REC_TYPE_TIME) {
	    if (message->arrival_time == 0)
		message->arrival_time = atol(start);
	    continue;
	}
	if (rec_type == REC_TYPE_FILT) {
	    if (message->filter_xport != 0)
		myfree(message->filter_xport);
	    message->filter_xport = mystrdup(start);
	    continue;
	}
	if (rec_type == REC_TYPE_INSP) {
	    if (message->inspect_xport != 0)
		myfree(message->inspect_xport);
	    message->inspect_xport = mystrdup(start);
	    continue;
	}
	if (rec_type == REC_TYPE_RDR) {
	    if (message->redirect_addr != 0)
		myfree(message->redirect_addr);
	    message->redirect_addr = mystrdup(start);
	    continue;
	}
	if (rec_type == REC_TYPE_FROM) {
	    if (message->sender == 0) {
		message->sender = mystrdup(start);
		opened(message->queue_id, message->sender,
		       message->data_size, message->rcpt_unread,
		       "queue %s", message->queue_name);
	    }
	    continue;
	}
	if (rec_type == REC_TYPE_ATTR) {
	    if ((error_text = split_nameval(start, &name, &value)) != 0) {
		msg_warn("%s: bad attribute: %s: %.200s",
			 message->queue_id, error_text, start);
		rec_type = REC_TYPE_ERROR;
		break;
	    }
	    /* Allow extra segment to override envelope segment info. */
	    if (strcmp(name, MAIL_ATTR_ENCODING) == 0) {
		if (message->encoding != 0)
		    myfree(message->encoding);
		message->encoding = mystrdup(value);
	    }
	    /* Original client attributes. */
	    if (strcmp(name, MAIL_ATTR_CLIENT_NAME) == 0) {
		if (message->client_name != 0)
		    myfree(message->client_name);
		message->client_name = mystrdup(value);
	    }
	    if (strcmp(name, MAIL_ATTR_CLIENT_ADDR) == 0) {
		if (message->client_addr != 0)
		    myfree(message->client_addr);
		message->client_addr = mystrdup(value);
	    }
	    if (strcmp(name, MAIL_ATTR_PROTO_NAME) == 0) {
		if (message->client_proto != 0)
		    myfree(message->client_proto);
		message->client_proto = mystrdup(value);
	    }
	    if (strcmp(name, MAIL_ATTR_HELO_NAME) == 0) {
		if (message->client_helo != 0)
		    myfree(message->client_helo);
		message->client_helo = mystrdup(value);
	    }
	    /* Optional tracing flags. */
	    else if (strcmp(name, MAIL_ATTR_TRACE_FLAGS) == 0) {
		message->tflags = DEL_REQ_TRACE_FLAGS(atoi(value));
		if (message->tflags == DEL_REQ_FLAG_RECORD)
		    message->tflags_offset = curr_offset;
		else
		    message->tflags_offset = 0;
	    }
	    continue;
	}
	if (rec_type == REC_TYPE_ERTO) {
	    if (message->errors_to == 0)
		message->errors_to = mystrdup(start);
	    continue;
	}
	if (rec_type == REC_TYPE_RRTO) {
	    if (message->return_receipt == 0)
		message->return_receipt = mystrdup(start);
	    continue;
	}
	if (rec_type == REC_TYPE_WARN) {
	    if (message->warn_offset == 0) {
		message->warn_offset = curr_offset;
		message->warn_time = atol(start);
	    }
	    continue;
	}
	if (rec_type == REC_TYPE_VERP) {
	    if (message->verp_delims == 0) {
		if (message->sender == 0 || message->sender[0] == 0) {
		    msg_warn("%s: ignoring VERP request for null sender",
			     message->queue_id);
		} else if (verp_delims_verify(start) != 0) {
		    msg_warn("%s: ignoring bad VERP request: \"%.100s\"",
			     message->queue_id, start);
		} else {
		    message->single_rcpt = 1;
		    message->verp_delims = mystrdup(start);
		}
	    }
	    continue;
	}
    }

    /*
     * Grr.
     */
    if (orig_rcpt != 0) {
	if (rec_type > 0)
	    msg_warn("%s: ignoring out-of-order original recipient <%.200s>",
		     message->queue_id, orig_rcpt);
	myfree(orig_rcpt);
    }

    /*
     * Avoid clumsiness elsewhere in the program. When sending data across an
     * IPC channel, sending an empty string is more convenient than sending a
     * null pointer.
     */
    if (message->errors_to == 0 && message->sender)
	message->errors_to = mystrdup(message->sender);
    if (message->return_receipt == 0)
	message->return_receipt = mystrdup("");
    if (message->encoding == 0)
	message->encoding = mystrdup(MAIL_ATTR_ENC_NONE);
    if (message->client_name == 0)
	message->client_name = mystrdup("");
    if (message->client_addr == 0)
	message->client_addr = mystrdup("");
    if (message->client_proto == 0)
	message->client_proto = mystrdup("");
    if (message->client_helo == 0)
	message->client_helo = mystrdup("");

    /*
     * Clean up.
     */
    vstring_free(buf);

    /*
     * Sanity checks. Verify that all required information was found,
     * including the queue file end marker.
     */
    if (message->rcpt_unread < 0
	|| (message->rcpt_offset == 0 && message->rcpt_unread != 0)) {
	msg_warn("%s: rcpt count mismatch (%d)",
		 message->queue_id, message->rcpt_unread);
	message->rcpt_unread = 0;
    }
    if (rec_type <= 0) {
	/* Already logged warning. */
    } else if (message->arrival_time == 0) {
	msg_warn("%s: message rejected: missing arrival time record",
		 message->queue_id);
    } else if (message->sender == 0) {
	msg_warn("%s: message rejected: missing sender record",
		 message->queue_id);
    } else if (message->data_offset == 0) {
	msg_warn("%s: message rejected: missing size record",
		 message->queue_id);
    } else {
	return (0);
    }
    message->rcpt_offset = save_offset;		/* restore flag */
    message->rcpt_unread = save_unread;		/* restore count */
    qmgr_rcpt_list_free(&message->rcpt_list);
    qmgr_rcpt_list_init(&message->rcpt_list);
    return (-1);
}

/* qmgr_message_update_warn - update the time of next delay warning */

void    qmgr_message_update_warn(QMGR_MESSAGE *message)
{

    /*
     * XXX eventually this should let us schedule multiple warnings, right
     * now it just allows for one.
     */
    if (qmgr_message_open(message)
	|| vstream_fseek(message->fp, message->warn_offset, SEEK_SET) < 0
    || rec_fprintf(message->fp, REC_TYPE_WARN, REC_TYPE_WARN_FORMAT, 0L) < 0
	|| vstream_fflush(message->fp))
	msg_fatal("update queue file %s: %m", VSTREAM_PATH(message->fp));
    qmgr_message_close(message);
}

/* qmgr_message_kill_record - mark one message record as killed */

void    qmgr_message_kill_record(QMGR_MESSAGE *message, long offset)
{
    if (offset <= 0)
	msg_panic("qmgr_message_kill_record: bad offset 0x%lx", offset);
    if (qmgr_message_open(message)
	|| rec_put_type(message->fp, REC_TYPE_KILL, offset) < 0
	|| vstream_fflush(message->fp))
	msg_fatal("update queue file %s: %m", VSTREAM_PATH(message->fp));
    qmgr_message_close(message);
}

/* qmgr_message_sort_compare - compare recipient information */

static int qmgr_message_sort_compare(const void *p1, const void *p2)
{
    QMGR_RCPT *rcpt1 = (QMGR_RCPT *) p1;
    QMGR_RCPT *rcpt2 = (QMGR_RCPT *) p2;
    QMGR_QUEUE *queue1;
    QMGR_QUEUE *queue2;
    char   *at1;
    char   *at2;
    int     result;

    /*
     * Compare most significant to least significant recipient attributes.
     * The comparison function must be transitive, so NULL values need to be
     * assigned an ordinal (we set NULL last).
     */

    queue1 = rcpt1->queue;
    queue2 = rcpt2->queue;
    if (queue1 != 0 && queue2 == 0)
	return (-1);
    if (queue1 == 0 && queue2 != 0)
	return (1);
    if (queue1 != 0 && queue2 != 0) {

	/*
	 * Compare message transport.
	 */
	if ((result = strcmp(queue1->transport->name,
			     queue2->transport->name)) != 0)
	    return (result);

	/*
	 * Compare queue name (nexthop or recipient@nexthop).
	 */
	if ((result = strcmp(queue1->name, queue2->name)) != 0)
	    return (result);
    }

    /*
     * Compare recipient domain.
     */
    at1 = strrchr(rcpt1->address, '@');
    at2 = strrchr(rcpt2->address, '@');
    if (at1 == 0 && at2 != 0)
	return (1);
    if (at1 != 0 && at2 == 0)
	return (-1);
    if (at1 != 0 && at2 != 0
	&& (result = strcasecmp(at1, at2)) != 0)
	return (result);

    /*
     * Compare recipient address.
     */
    return (strcmp(rcpt1->address, rcpt2->address));
}

/* qmgr_message_sort - sort message recipient addresses by domain */

static void qmgr_message_sort(QMGR_MESSAGE *message)
{
    qsort((char *) message->rcpt_list.info, message->rcpt_list.len,
	  sizeof(message->rcpt_list.info[0]), qmgr_message_sort_compare);
    if (msg_verbose) {
	QMGR_RCPT_LIST list = message->rcpt_list;
	QMGR_RCPT *rcpt;

	msg_info("start sorted recipient list");
	for (rcpt = list.info; rcpt < list.info + list.len; rcpt++)
	    msg_info("qmgr_message_sort: %s", rcpt->address);
	msg_info("end sorted recipient list");
    }
}

/* qmgr_resolve_one - resolve or skip one recipient */

static int qmgr_resolve_one(QMGR_MESSAGE *message, QMGR_RCPT *recipient,
			            const char *addr, RESOLVE_REPLY *reply)
{
    if ((message->tflags & DEL_REQ_FLAG_VERIFY) == 0)
	resolve_clnt_query(addr, reply);
    else
	resolve_clnt_verify(addr, reply);
    if (reply->flags & RESOLVE_FLAG_FAIL) {
	qmgr_defer_recipient(message, recipient, "address resolver failure");
	return (-1);
    } else if (reply->flags & RESOLVE_FLAG_ERROR) {
	qmgr_bounce_recipient(message, recipient,
			      "bad address syntax: \"%s\"", addr);
	return (-1);
    } else {
	return (0);
    }
}

/* qmgr_message_resolve - resolve recipients */

static void qmgr_message_resolve(QMGR_MESSAGE *message)
{
    static ARGV *defer_xport_argv;
    QMGR_RCPT_LIST list = message->rcpt_list;
    QMGR_RCPT *recipient;
    QMGR_TRANSPORT *transport = 0;
    QMGR_QUEUE *queue = 0;
    RESOLVE_REPLY reply;
    VSTRING *queue_name;
    char   *at;
    char  **cpp;
    char   *nexthop;
    int     len;
    int     status;
    const char *myname="qmgr_message_resolve";
    int wrong_count = 0;

#define STREQ(x,y)	(strcmp(x,y) == 0)
#define STR		vstring_str
#define LEN		VSTRING_LEN
#define UPDATE(ptr,new)	{ myfree(ptr); ptr = mystrdup(new); }

    resolve_clnt_init(&reply);
    queue_name = vstring_alloc(1);
    for (recipient = list.info; recipient < list.info + list.len; recipient++) {

	/*
	 * Redirect overrides all else. But only once (per batch of
	 * recipients). For consistency with the remainder of Postfix,
	 * rewrite the address to canonical form before resolving it.
	 */
	if (message->redirect_addr) {
	    if (recipient > list.info) {
		recipient->queue = 0;
		continue;
	    }
	    rewrite_clnt_internal(REWRITE_CANON, message->redirect_addr,
				  reply.recipient);
	    UPDATE(recipient->address, STR(reply.recipient));
	    if (qmgr_resolve_one(message, recipient,
				 recipient->address, &reply) < 0)
		continue;
	    if (!STREQ(recipient->address, STR(reply.recipient)))
		UPDATE(recipient->address, STR(reply.recipient));
	}

	/*
	 * Content filtering overrides the address resolver.
	 */
	else if (message->filter_xport) {
	    vstring_strcpy(reply.transport, message->filter_xport);
	    if ((nexthop = split_at(STR(reply.transport), ':')) == 0
		|| *nexthop == 0)
		nexthop = var_myhostname;
	    vstring_strcpy(reply.nexthop, nexthop);
	    vstring_strcpy(reply.recipient, recipient->address);
	}

	/*
	 * Resolve the destination to (transport, nexthop, address). The
	 * result address may differ from the one specified by the sender.
	 */
	else if (var_sender_routing == 0) {
	    if (qmgr_resolve_one(message, recipient,
				 recipient->address, &reply) < 0)
		continue;
	    if (!STREQ(recipient->address, STR(reply.recipient)))
		UPDATE(recipient->address, STR(reply.recipient));
	}

	/*
	 * XXX Sender-based routing does not work very well, because it has
	 * problems with sending bounces.
	 */
	else {
	    if (qmgr_resolve_one(message, recipient,
				 message->sender, &reply) < 0)
		continue;
	    vstring_strcpy(reply.recipient, recipient->address);
	}

	/*
	 * Bounce null recipients. This should never happen, but is most
	 * likely the result of a fault in a different program, so aborting
	 * the queue manager process does not help.
	 */

	if (msg_debug)
	  msg_info("%s: Original transport=%s, nexthop=%s, recipient=%s",
	      myname, STR(reply.transport), STR(reply.nexthop), STR(reply.recipient));

	// Drop return mail if enable
	if (!var_rmail_returnmail_enable) {
	  if (strlen(message->sender) == 0) {
	    vstring_strcpy(reply.reason, "Disable return mail");
	    message->flags=0;
	    message->removed=1;
	    break;
	  }

	  if (strcasecmp(message->sender, "double-bounce")==0) {
	    vstring_strcpy(reply.reason, "Disable return mail");
	    message->flags=0;
	    message->removed=1;
	    break;
	  }

	  if (strcasecmp(message->sender, "MAILER-DAEMON")==0) {
	    vstring_strcpy(reply.reason, "Disable return mail");
	    message->flags=0;
	    message->removed=1;
	    break;
	  }

	}

	if (! message->filter_xport) {
	  rmail_resolve(reply, recipient);
	}

	if (recipient->del == 1 && list.len == 1) {
	  message->flags = 0;
	  message->removed=1;
	  if (msg_debug)
	    msg_info("%s: [%s] dropped, f=%s, r=%s (%s)", myname, message->queue_id, message->sender, recipient->address, STR(reply.reason));
	  break;
	} else if (recipient->del == 1 && wrong_count == list.len) {
	  message->flags=0;
	  message->removed=1;
	  if (msg_debug)
	    msg_info("%s: [%s] dropped, f=%s, r=%s (%s)", myname, message->queue_id, message->sender, recipient->address, STR(reply.reason));
	  break;
	} else if (recipient->del == 1) {
	  wrong_count++;
	  if (msg_debug)
	    msg_info("%s: [%s] want to drop, but there are still other recipients, f=%s, r=%s (%s)",
		myname, message->queue_id, message->sender, recipient->address, STR(reply.reason));
	  break;
	}

	if (msg_debug) 
	  msg_info("%s: Final transport=%s, nexthop=%s, recipient=%s",
	      myname, STR(reply.transport), STR(reply.nexthop), STR(reply.recipient));
	
	if (recipient->address[0] == 0) {
	    qmgr_bounce_recipient(message, recipient,
				  "null recipient address");
	    continue;
	}

	/*
	 * Bounce recipient addresses that start with `-'. External commands
	 * may misinterpret such addresses as command-line options.
	 * 
	 * In theory I could say people should always carefully set up their
	 * master.cf pipe mailer entries with `--' before the first
	 * non-option argument, but mistakes will happen regardless.
	 * 
	 * Therefore the protection is put in place here, in the queue manager,
	 * where it cannot be bypassed.
	 */
	if (var_allow_min_user == 0 && recipient->address[0] == '-') {
	    qmgr_bounce_recipient(message, recipient,
				  "invalid recipient syntax: \"%s\"",
				  recipient->address);
	    continue;
	}

	/*
	 * Discard mail to the local double bounce address here, so this
	 * system can run without a local delivery agent. They'd still have
	 * to configure something for mail directed to the local postmaster,
	 * though, but that is an RFC requirement anyway.
	 * 
	 * XXX This lookup should be done in the resolver, and the mail should
	 * be directed to a general-purpose null delivery agent.
	 */
	if (reply.flags & RESOLVE_CLASS_LOCAL) {
	    at = strrchr(STR(reply.recipient), '@');
	    len = (at ? (at - STR(reply.recipient))
		   : strlen(STR(reply.recipient)));
	    if (strncasecmp(STR(reply.recipient), var_double_bounce_sender,
			    len) == 0
		&& !var_double_bounce_sender[len]) {
		status = sent(message->tflags, message->queue_id,
			      recipient->orig_rcpt, recipient->address,
			   recipient->offset, "none", message->arrival_time,
			 "undeliverable postmaster notification discarded");
		if (status == 0) {
		    deliver_completed(message->fp, recipient->offset);
		    msg_warn("%s: undeliverable postmaster notification discarded",
			     message->queue_id);
		} else
		    message->flags |= status;
		continue;
	    }
	}

	/*
	 * Optionally defer deliveries over specific transports, unless the
	 * restriction is lifted temporarily.
	 */
	if (*var_defer_xports && (message->qflags & QMGR_FLUSH_DEAD) == 0) {
	    if (defer_xport_argv == 0)
		defer_xport_argv = argv_split(var_defer_xports, " \t\r\n,");
	    for (cpp = defer_xport_argv->argv; *cpp; cpp++)
		if (strcmp(*cpp, STR(reply.transport)) == 0)
		    break;
	    if (*cpp) {
		qmgr_defer_recipient(message, recipient, "deferred transport");
		continue;
	    }
	}

	/*
	 * Look up or instantiate the proper transport.
	 */
	if (transport == 0 || !STREQ(transport->name, STR(reply.transport))) {
	    if ((transport = qmgr_transport_find(STR(reply.transport))) == 0)
		transport = qmgr_transport_create(STR(reply.transport));
	    queue = 0;
	}

	/*
	 * This transport is dead. Defer delivery to this recipient.
	 */
	if ((transport->flags & QMGR_TRANSPORT_STAT_DEAD) != 0) {
	    qmgr_defer_recipient(message, recipient, transport->reason);
	    continue;
	}

	/*
	 * The nexthop destination provides the default name for the
	 * per-destination queue. When the delivery agent accepts only one
	 * recipient per delivery, give each recipient its own queue, so that
	 * deliveries to different recipients of the same message can happen
	 * in parallel, and so that we can enforce per-recipient concurrency
	 * limits and prevent one recipient from tying up all the delivery
	 * agent resources. We use recipient@nexthop as queue name rather
	 * than the actual recipient domain name, so that one recipient in
	 * multiple equivalent domains cannot evade the per-recipient
	 * concurrency limit. Split the address on the recipient delimiter if
	 * one is defined, so that extended addresses don't get extra
	 * delivery slots.
	 * 
	 * Fold the result to lower case so that we don't have multiple queues
	 * for the same name.
	 * 
	 * Important! All recipients in a queue must have the same nexthop
	 * value. It is OK to have multiple queues with the same nexthop
	 * value, but only when those queues are named after recipients.
	 * 
	 * The single-recipient code below was written for local(8) like
	 * delivery agents, and assumes that all domains that deliver to the
	 * same (transport + nexthop) are aliases for $nexthop. Delivery
	 * concurrency is changed from per-domain into per-recipient, by
	 * changing the queue name from nexthop into localpart@nexthop.
	 * 
	 * XXX This assumption is incorrect when different destinations share
	 * the same (transport + nexthop). In reality, such transports are
	 * rarely configured to use single-recipient deliveries. The fix is
	 * to decouple the per-destination recipient limit from the
	 * per-destination concurrency.
	 */
	vstring_strcpy(queue_name, STR(reply.nexthop));
	if (strcmp(transport->name, MAIL_SERVICE_ERROR) != 0
	    && transport->recipient_limit == 1) {
	    /* Copy the recipient localpart. */
	    at = strrchr(STR(reply.recipient), '@');
	    len = (at ? (at - STR(reply.recipient))
		   : strlen(STR(reply.recipient)));
	    vstring_strncpy(queue_name, STR(reply.recipient), len);
	    /* Remove the address extension from the recipient localpart. */
	    if (*var_rcpt_delim && split_addr(STR(queue_name), *var_rcpt_delim))
		vstring_truncate(queue_name, strlen(STR(queue_name)));
	    /* Assume the recipient domain is equivalent to nexthop. */
	    vstring_sprintf_append(queue_name, "@%s", STR(reply.nexthop));
	}
	lowercase(STR(queue_name));

	/*
	 * This transport is alive. Find or instantiate a queue for this
	 * recipient.
	 */
	if (queue == 0 || !STREQ(queue->name, STR(queue_name))) {
	    if ((queue = qmgr_queue_find(transport, STR(queue_name))) == 0)
		queue = qmgr_queue_create(transport, STR(queue_name),
					  STR(reply.nexthop));
	}

	/*
	 * This queue is dead. Defer delivery to this recipient.
	 */
	if (queue->window == 0) {
	    qmgr_defer_recipient(message, recipient, queue->reason);
	    continue;
	}

	/*
	 * This queue is alive. Bind this recipient to this queue instance.
	 */
	recipient->queue = queue;
    }
    resolve_clnt_free(&reply);
    vstring_free(queue_name);
}

/* qmgr_message_assign - assign recipients to specific delivery requests */

static void qmgr_message_assign(QMGR_MESSAGE *message)
{
    QMGR_RCPT_LIST list = message->rcpt_list;
    QMGR_RCPT *recipient;
    QMGR_ENTRY *entry = 0;
    QMGR_QUEUE *queue;
    QMGR_JOB *job = 0;
    QMGR_PEER *peer = 0;

    /*
     * Try to bundle as many recipients in a delivery request as we can. When
     * the recipient resolves to the same site and transport as the previous
     * recipient, do not create a new queue entry, just move that recipient
     * to the recipient list of the existing queue entry. All this provided
     * that we do not exceed the transport-specific limit on the number of
     * recipients per transaction. Skip recipients with a dead transport or
     * destination.
     */
#define LIMIT_OK(limit, count) ((limit) == 0 || ((count) < (limit)))

    for (recipient = list.info; recipient < list.info + list.len; recipient++) {
	if ((queue = recipient->queue) != 0) {
	    if (message->single_rcpt || entry == 0 || entry->queue != queue
		|| !LIMIT_OK(queue->transport->recipient_limit,
			     entry->rcpt_list.len)) {

		/*
		 * Lookup or instantiate the message job if necessary.
		 */
		if (job == 0 || queue->transport != job->transport) {
		    job = qmgr_job_obtain(message, queue->transport);
		    peer = 0;
		}

		/*
		 * Lookup or instantiate job peer if necessary.
		 */
		if (peer == 0 || queue != peer->queue) {
		    if ((peer = qmgr_peer_find(job, queue)) == 0)
			peer = qmgr_peer_create(job, queue);
		}

		/*
		 * Create new peer entry.
		 */
		entry = qmgr_entry_create(peer, message);
		job->read_entries++;
	    }

	    /*
	     * Add the recipient to the current entry and increase all those
	     * recipient counters accordingly.
	     */
	    qmgr_rcpt_list_add(&entry->rcpt_list, recipient->offset,
			       recipient->orig_rcpt, recipient->address);
	    job->rcpt_count++;
	    message->rcpt_count++;
	    qmgr_recipient_count++;
	}
    }

    /*
     * Release the message recipient list and reinitialize it for the next
     * time.
     */
    qmgr_rcpt_list_free(&message->rcpt_list);
    qmgr_rcpt_list_init(&message->rcpt_list);

    /*
     * Note that even if qmgr_job_obtain() reset the job candidate cache of
     * all transports to which we assigned new recipients, this message may
     * have other jobs which we didn't touch at all this time. But the number
     * of unread recipients affecting the candidate selection might have
     * changed considerably, so we must invalidate the caches if it might be
     * of some use.
     */
    for (job = message->job_list.next; job; job = job->message_peers.next)
	if (job->selected_entries < job->read_entries
	    && job->blocker_tag != job->transport->blocker_tag)
	    job->transport->candidate_cache_current = 0;
}

/* qmgr_message_move_limits - recycle unused recipient slots */

static void qmgr_message_move_limits(QMGR_MESSAGE *message)
{
    QMGR_JOB *job;

    for (job = message->job_list.next; job; job = job->message_peers.next)
	qmgr_job_move_limits(job);
}

/* qmgr_message_free - release memory for in-core message structure */

void    qmgr_message_free(QMGR_MESSAGE *message)
{
    QMGR_JOB *job;

    if (message->refcount != 0)
	msg_panic("qmgr_message_free: reference len: %d", message->refcount);
    if (message->fp)
	msg_panic("qmgr_message_free: queue file is open");
    while ((job = message->job_list.next) != 0)
	qmgr_job_free(job);
    myfree(message->queue_id);
    myfree(message->queue_name);
    if (message->encoding)
	myfree(message->encoding);
    if (message->sender)
	myfree(message->sender);
    if (message->verp_delims)
	myfree(message->verp_delims);
    if (message->errors_to)
	myfree(message->errors_to);
    if (message->return_receipt)
	myfree(message->return_receipt);
    if (message->filter_xport)
	myfree(message->filter_xport);
    if (message->inspect_xport)
	myfree(message->inspect_xport);
    if (message->redirect_addr)
	myfree(message->redirect_addr);
    if (message->client_name)
	myfree(message->client_name);
    if (message->client_addr)
	myfree(message->client_addr);
    if (message->client_proto)
	myfree(message->client_proto);
    if (message->client_helo)
	myfree(message->client_helo);
    qmgr_rcpt_list_free(&message->rcpt_list);
    qmgr_message_count--;
    myfree((char *) message);
}

/* qmgr_message_alloc - create in-core message structure */

QMGR_MESSAGE *qmgr_message_alloc(const char *queue_name, const char *queue_id,
				         int qflags)
{
    char   *myname = "qmgr_message_alloc";
    QMGR_MESSAGE *message;

    if (msg_verbose)
	msg_info("%s: %s %s", myname, queue_name, queue_id);

    /*
     * Create an in-core message structure.
     */
    message = qmgr_message_create(queue_name, queue_id, qflags);

    /*
     * Extract message envelope information: time of arrival, sender address,
     * recipient addresses. Skip files with malformed envelope information.
     */
#define QMGR_LOCK_MODE (MYFLOCK_OP_EXCLUSIVE | MYFLOCK_OP_NOWAIT)

    if (qmgr_message_open(message) < 0) {
	qmgr_message_free(message);
	return (0);
    }
    if (myflock(vstream_fileno(message->fp), INTERNAL_LOCK, QMGR_LOCK_MODE) < 0) {
	msg_info("%s: skipped, still being delivered", queue_id);
	qmgr_message_close(message);
	qmgr_message_free(message);
	return (QMGR_MESSAGE_LOCKED);
    }
    if (qmgr_message_read(message) < 0) {
	qmgr_message_close(message);
	qmgr_message_free(message);
	return (0);
    } else {

	/*
	 * Reset the defer log. This code should not be here, but we must
	 * reset the defer log *after* acquiring the exclusive lock on the
	 * queue file and *before* resolving new recipients. Since all those
	 * operations are encapsulated so nicely by this routine, the defer
	 * log reset has to be done here as well.
	 */
	if (mail_queue_remove(MAIL_QUEUE_DEFER, queue_id) && errno != ENOENT)
	    msg_fatal("%s: %s: remove %s %s: %m", myname,
		      queue_id, MAIL_QUEUE_DEFER, queue_id);
	qmgr_message_sort(message);
	qmgr_message_resolve(message);
	qmgr_message_sort(message);
	qmgr_message_assign(message);
	qmgr_message_close(message);
	if (message->rcpt_offset == 0)
	    qmgr_message_move_limits(message);
	return (message);
    }
}

/* qmgr_message_realloc - refresh in-core message structure */

QMGR_MESSAGE *qmgr_message_realloc(QMGR_MESSAGE *message)
{
    char   *myname = "qmgr_message_realloc";

    /*
     * Sanity checks.
     */
    if (message->rcpt_offset <= 0)
	msg_panic("%s: invalid offset: %ld", myname, message->rcpt_offset);
    if (msg_verbose)
	msg_info("%s: %s %s offset %ld", myname, message->queue_name,
		 message->queue_id, message->rcpt_offset);

    /*
     * Extract recipient addresses. Skip files with malformed envelope
     * information.
     */
    if (qmgr_message_open(message) < 0)
	return (0);
    if (qmgr_message_read(message) < 0) {
	qmgr_message_close(message);
	return (0);
    } else {
	qmgr_message_sort(message);
	qmgr_message_resolve(message);
	qmgr_message_sort(message);
	qmgr_message_assign(message);
	qmgr_message_close(message);
	if (message->rcpt_offset == 0)
	    qmgr_message_move_limits(message);
	return (message);
    }
}
