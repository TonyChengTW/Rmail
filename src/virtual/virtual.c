/*++
/* NAME
/*	virtual 8
/* SUMMARY
/*	Postfix virtual domain mail delivery agent
/* SYNOPSIS
/*	\fBvirtual\fR [generic Postfix daemon options]
/* DESCRIPTION
/*	The \fBvirtual\fR(8) delivery agent is designed for virtual mail
/*	hosting services. Originally based on the Postfix local(8) delivery
/*	agent, this agent looks up recipients with map lookups of their
/*	full recipient address, instead of using hard-coded unix password
/*	file lookups of the address local part only.
/*
/*	This delivery agent only delivers mail.  Other features such as
/*	mail forwarding, out-of-office notifications, etc., must be
/*	configured via virtual_alias maps or via similar lookup mechanisms.
/* MAILBOX LOCATION
/* .ad
/* .fi
/*	The mailbox location is controlled by the \fBvirtual_mailbox_base\fR
/*	and \fBvirtual_mailbox_maps\fR configuration parameters (see below).
/*	The \fBvirtual_mailbox_maps\fR table is indexed by the recipient
/*	address as described under TABLE SEARCH ORDER below.
/*
/*	The mailbox pathname is constructed as follows:
/*
/* .ti +2
/*	\fB$virtual_mailbox_base/$virtual_mailbox_maps(\fIrecipient\fB)\fR
/*
/*	where \fIrecipient\fR is the full recipient address.
/* UNIX MAILBOX FORMAT
/* .ad
/* .fi
/*	When the mailbox location does not end in \fB/\fR, the message
/*	is delivered in UNIX mailbox format.   This format stores multiple
/*	messages in one textfile.
/*
/*	The \fBvirtual\fR delivery agent prepends a "\fBFrom \fIsender
/*	time_stamp\fR" envelope header to each message, prepends a
/*	\fBDelivered-To:\fR message header with the envelope recipient
/*	address,
/*	prepends an \fBX-Original-To:\fR header with the recipient address as
/*	given to Postfix,
/*	prepends a \fBReturn-Path:\fR message header with the
/*	envelope sender address, prepends a \fB>\fR character to lines
/*	beginning with "\fBFrom \fR", and appends an empty line.
/*
/*	The mailbox is locked for exclusive access while delivery is in
/*	progress. In case of problems, an attempt is made to truncate the
/*	mailbox to its original length.
/* QMAIL MAILDIR FORMAT
/* .ad
/* .fi
/*	When the mailbox location ends in \fB/\fR, the message is delivered
/*	in qmail \fBmaildir\fR format. This format stores one message per file.
/*
/*	The \fBvirtual\fR delivery agent daemon prepends a \fBDelivered-To:\fR
/*	message header with the final envelope recipient address,
/*	prepends an \fBX-Original-To:\fR header with the recipient address as
/*	given to Postfix, and prepends a
/*	\fBReturn-Path:\fR message header with the envelope sender address.
/*
/*	By definition, \fBmaildir\fR format does not require application-level
/*	file locking during mail delivery or retrieval.
/* MAILBOX OWNERSHIP
/* .ad
/* .fi
/*	Mailbox ownership is controlled by the \fBvirtual_uid_maps\fR
/*	and \fBvirtual_gid_maps\fR lookup tables, which are indexed
/*	with the full recipient address. Each table provides
/*	a string with the numerical user and group ID, respectively.
/*
/*	The \fBvirtual_minimum_uid\fR parameter imposes a lower bound on
/*	numerical user ID values that may be specified in any
/*	\fBvirtual_uid_maps\fR.
/* TABLE SEARCH ORDER
/* .ad
/* .fi
/*	Normally, a lookup table is specified as a text file that
/*	serves as input to the \fBpostmap\fR(1) command. The result, an
/*	indexed file in \fBdbm\fR or \fBdb\fR format, is used for fast
/*	searching by the mail system.
/*
/*	The search order is as follows. The search stops
/*	upon the first successful lookup.
/* .IP \(bu
/*	When the recipient has an optional address extension the
/*	\fIuser+extension@domain.tld\fR address is looked up first.
/* .sp
/*	With Postfix versions before 2.1, the optional address extension
/*	is always ignored.
/* .IP \(bu
/*	The \fIuser@domain.tld\fR address, without address extension,
/*	is looked up next.
/* .IP \(bu
/*	Finally, the recipient \fI@domain\fR is looked up.
/* .PP
/*	When the table is provided via other means such as NIS, LDAP
/*	or SQL, the same lookups are done as for ordinary indexed files.
/*
/*	Alternatively, a table can be provided as a regular-expression
/*	map where patterns are given as regular expressions. In that case,
/*	only the full recipient address is given to the regular-expression
/*	map.
/* SECURITY
/* .ad
/* .fi
/*	The virtual delivery agent is not security sensitive, provided
/*	that the lookup tables with recipient user/group ID information are
/*	adequately protected. This program is not designed to run chrooted.
/* STANDARDS
/*	RFC 822 (ARPA Internet Text Messages)
/* DIAGNOSTICS
/*	Mail bounces when the recipient has no mailbox or when the
/*	recipient is over disk quota. In all other cases, mail for
/*	an existing recipient is deferred and a warning is logged.
/*
/*	Problems and transactions are logged to \fBsyslogd\fR(8).
/*	Corrupted message files are marked so that the queue
/*	manager can move them to the \fBcorrupt\fR queue afterwards.
/*
/*	Depending on the setting of the \fBnotify_classes\fR parameter,
/*	the postmaster is notified of bounces and of other trouble.
/* BUGS
/*	This delivery agent supports address extensions in email
/*	addresses and in lookup table keys, but does not propagate
/*	address extension information to the result of table lookup.
/*
/*	Postfix should have lookup tables that can return multiple result
/*	attributes. In order to avoid the inconvenience of maintaining
/*	three tables, use an LDAP or MYSQL database.
/* CONFIGURATION PARAMETERS
/* .ad
/* .fi
/*	Changes to \fBmain.cf\fR are picked up automatically, as virtual(8)
/*	processes run for only a limited amount of time. Use the command
/*	"\fBpostfix reload\fR" to speed up a change.
/*
/*	The text below provides only a parameter summary. See
/*	postconf(5) for more details including examples.
/* MAILBOX DELIVERY CONTROLS
/* .ad
/* .fi
/* .IP "\fBvirtual_mailbox_base (empty)\fR"
/*	A prefix that the virtual(8) delivery agent prepends to all pathname
/*	results from $virtual_mailbox_maps table lookups.
/* .IP "\fBvirtual_mailbox_maps (empty)\fR"
/*	Optional lookup tables with all valid addresses in the domains that
/*	match $virtual_mailbox_domains.
/* .IP "\fBvirtual_minimum_uid (100)\fR"
/*	The minimum user ID value that the virtual(8) delivery agent accepts
/*	as a result from \fB$virtual_uid_maps\fR table lookup.
/* .IP "\fBvirtual_uid_maps (empty)\fR"
/*	Lookup tables with the per-recipient user ID that the virtual(8)
/*	delivery agent uses while writing to the recipient's mailbox.
/* .IP "\fBvirtual_gid_maps (empty)\fR"
/*	Lookup tables with the per-recipient group ID for virtual(8) mailbox
/*	delivery.
/* .PP
/*	Available in Postfix version 2.0 and later:
/* .IP "\fBvirtual_mailbox_domains ($virtual_mailbox_maps)\fR"
/*	The list of domains that are delivered via the $virtual_transport
/*	mail delivery transport.
/* .IP "\fBvirtual_transport (virtual)\fR"
/*	The default mail delivery transport for domains that match the
/*	$virtual_mailbox_domains parameter value.
/* LOCKING CONTROLS
/* .ad
/* .fi
/* .IP "\fBvirtual_mailbox_lock (see 'postconf -d' output)\fR"
/*	How to lock a UNIX-style virtual(8) mailbox before attempting
/*	delivery.
/* .IP "\fBdeliver_lock_attempts (20)\fR"
/*	The maximal number of attempts to acquire an exclusive lock on a
/*	mailbox file or bounce(8) logfile.
/* .IP "\fBdeliver_lock_delay (1s)\fR"
/*	The time between attempts to acquire an exclusive lock on a mailbox
/*	file or bounce(8) logfile.
/* .IP "\fBstale_lock_time (500s)\fR"
/*	The time after which a stale exclusive mailbox lockfile is removed.
/* RESOURCE AND RATE CONTROLS
/* .ad
/* .fi
/* .IP "\fBvirtual_destination_concurrency_limit ($default_destination_concurrency_limit)\fR"
/*	The maximal number of parallel deliveries to the same destination
/*	via the virtual message delivery transport.
/* .IP "\fBvirtual_destination_recipient_limit ($default_destination_recipient_limit)\fR"
/*	The maximal number of recipients per delivery via the virtual
/*	message delivery transport.
/* .IP "\fBvirtual_mailbox_limit (51200000)\fR"
/*	The maximal size in bytes of an individual mailbox or maildir file,
/*	or zero (no limit).
/* MISCELLANEOUS CONTROLS
/* .ad
/* .fi
/* .IP "\fBconfig_directory (see 'postconf -d' output)\fR"
/*	The default location of the Postfix main.cf and master.cf
/*	configuration files.
/* .IP "\fBdaemon_timeout (18000s)\fR"
/*	How much time a Postfix daemon process may take to handle a
/*	request before it is terminated by a built-in watchdog timer.
/* .IP "\fBipc_timeout (3600s)\fR"
/*	The time limit for sending or receiving information over an internal
/*	communication channel.
/* .IP "\fBmax_idle (100s)\fR"
/*	The maximum amount of time that an idle Postfix daemon process
/*	waits for the next service request before exiting.
/* .IP "\fBmax_use (100)\fR"
/*	The maximal number of connection requests before a Postfix daemon
/*	process terminates.
/* .IP "\fBprocess_id (read-only)\fR"
/*	The process ID of a Postfix command or daemon process.
/* .IP "\fBprocess_name (read-only)\fR"
/*	The process name of a Postfix command or daemon process.
/* .IP "\fBqueue_directory (see 'postconf -d' output)\fR"
/*	The location of the Postfix top-level queue directory.
/* .IP "\fBsyslog_facility (mail)\fR"
/*	The syslog facility of Postfix logging.
/* .IP "\fBsyslog_name (postfix)\fR"
/*	The mail system name that is prepended to the process name in syslog
/*	records, so that "smtpd" becomes, for example, "postfix/smtpd".
/* SEE ALSO
/*	qmgr(8), queue manager
/*	bounce(8), delivery status reports
/*	postconf(5), configuration parameters
/*	syslogd(8), system logging
/* README_FILES
/*	Use "\fBpostconf readme_directory\fR" or
/*	"\fBpostconf html_directory\fR" to locate this information.
/*	VIRTUAL_README, domain hosting howto
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* HISTORY
/* .ad
/* .fi
/*	This delivery agent was originally based on the Postfix local delivery
/*	agent. Modifications mainly consisted of removing code that either
/*	was not applicable or that was not safe in this context: aliases,
/*	~user/.forward files, delivery to "|command" or to /file/name.
/*
/*	The \fBDelivered-To:\fR message header appears in the \fBqmail\fR
/*	system by Daniel Bernstein.
/*
/*	The \fBmaildir\fR structure appears in the \fBqmail\fR system
/*	by Daniel Bernstein.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*
/*	Andrew McNamara
/*	andrewm@connect.com.au
/*	connect.com.au Pty. Ltd.
/*	Level 3, 213 Miller St
/*	North Sydney 2060, NSW, Australia
/*--*/

/* System library. */

#include <sys_defs.h>
#include <stdlib.h>
#ifdef USE_PATHS_H
#include <paths.h>			/* XXX mail_spool_dir dependency */
#endif

/* Utility library. */

#include <msg.h>
#include <vstring.h>
#include <vstream.h>
#include <iostuff.h>
#include <set_eugid.h>
#include <dict.h>

/* Global library. */

#include <mail_queue.h>
#include <recipient_list.h>
#include <deliver_request.h>
#include <deliver_completed.h>
#include <mail_params.h>
#include <mail_conf.h>
#include <mail_params.h>
#include <mail_addr_find.h>
#include <flush_clnt.h>

/* Single server skeleton. */

#include <mail_server.h>

/* Application-specific. */

#include "virtual.h"

/* Rmail need mysql.h */
#include <mysql.h>

 /*
  * Tunable parameters.
  */
char   *var_virt_mailbox_maps;
char   *var_virt_uid_maps;
char   *var_virt_gid_maps;
int     var_virt_minimum_uid;
char   *var_virt_mailbox_base;
char   *var_virt_mailbox_lock;
int     var_virt_mailbox_limit;
char   *var_mail_spool_dir;		/* XXX dependency fix */

/* Rmail variables */
int   var_rmail_mtadb_port;
int   var_rmail_cacdb_port;
int   var_rmail_mailcache_expire;
int   var_rmail_default_quota;
int   var_rmail_quota_expire;

bool  var_rmail_mtadb_interactive;
bool  var_rmail_cacdb_interactive;
bool  var_rmail_mailcache_enable;
bool  var_rmail_allow_local;
bool  var_rmail_zipmail_enable;

char  *var_rmail_mtadb_host;
char  *var_rmail_mtadb_user;
char  *var_rmail_mtadb_pass;
char  *var_rmail_mtadb_name;
char  *var_rmail_cacdb_host;
char  *var_rmail_cacdb_user;
char  *var_rmail_cacdb_pass;
char  *var_rmail_cacdb_name;

char  *var_rmail_transport_table;
char  *var_rmail_transport_idxfield;
char  *var_rmail_transport_domainfield;
char  *var_rmail_transport_basedirfield;

char  *var_rmail_mailuser_table;
char  *var_rmail_mailuser_domainfield;
char  *var_rmail_mailuser_mailidfield;
char  *var_rmail_mailuser_mhostfield;
char  *var_rmail_mailuser_mboxfield;
char  *var_rmail_mailuser_statufield;
char  *var_rmail_mailuser_smtpfield;
char  *var_rmail_mailuser_pop3field;
char  *var_rmail_mailuser_webfield;

char  *var_rmail_mailcache_table;
char  *var_rmail_mailcache_domainfield;
char  *var_rmail_mailcache_mailidfield;
char  *var_rmail_mailcache_mhostfield;
char  *var_rmail_mailcache_mboxfield;
char  *var_rmail_mailcache_statufield;
char  *var_rmail_mailcache_smtpfield;
char  *var_rmail_mailcache_pop3field;
char  *var_rmail_mailcache_webfield;
char  *var_rmail_mailcache_timefield;
char  *var_rmail_maildir_sufix;
char  *var_rmail_quota_file;
char  *var_rmail_quota_tmpfile;
char  *var_rmail_default_basedir;

MYSQL mta_dbh;
MYSQL cac_dbh;

char myhostname[64];
 /*
  * Mappings.
  */
MAPS   *virtual_mailbox_maps;
MAPS   *virtual_uid_maps;
MAPS   *virtual_gid_maps;

 /*
  * Bit masks.
  */
int     virtual_mbox_lock_mask;

/* local_deliver - deliver message with extreme prejudice */

static int local_deliver(DELIVER_REQUEST *rqst, char *service)
{
    char   *myname = "local_deliver";
    RECIPIENT *rcpt_end = rqst->rcpt_list.info + rqst->rcpt_list.len;
    RECIPIENT *rcpt;
    int     rcpt_stat;
    int     msg_stat;
    LOCAL_STATE state;
    USER_ATTR usr_attr;

    if (msg_verbose)
	msg_info("local_deliver: %s from %s", rqst->queue_id, rqst->sender);

    /*
     * Initialize the delivery attributes that are not recipient specific.
     */
    state.level = 0;
    deliver_attr_init(&state.msg_attr);
    state.msg_attr.queue_name = rqst->queue_name;
    state.msg_attr.queue_id = rqst->queue_id;
    state.msg_attr.fp = rqst->fp;
    state.msg_attr.offset = rqst->data_offset;
    state.msg_attr.sender = rqst->sender;
    state.msg_attr.relay = service;
    state.msg_attr.arrival_time = rqst->arrival_time;
    RESET_USER_ATTR(usr_attr, state.level);
    state.request = rqst;

    /*
     * Iterate over each recipient named in the delivery request. When the
     * mail delivery status for a given recipient is definite (i.e. bounced
     * or delivered), update the message queue file and cross off the
     * recipient. Update the per-message delivery status.
     */
    for (msg_stat = 0, rcpt = rqst->rcpt_list.info; rcpt < rcpt_end; rcpt++) {
	state.msg_attr.orig_rcpt = rcpt->orig_addr;
	state.msg_attr.recipient = rcpt->address;
	state.msg_attr.rcpt_offset = rcpt->offset;
	rcpt_stat = deliver_recipient(state, usr_attr);
	if (rcpt_stat == 0)
	    deliver_completed(state.msg_attr.fp, rcpt->offset);
	msg_stat |= rcpt_stat;
    }

    return (msg_stat);
}

/* local_service - perform service for client */

static void local_service(VSTREAM *stream, char *service, char **argv)
{
    DELIVER_REQUEST *request;
    int     status;
    const char *myname="local_service";

    if (mysql_ping(&mta_dbh) != 0) {
      msg_warn("%s: MTA database(%s) had gone(%s), attempted to reconnect",
	  myname, var_rmail_mtadb_host, mysql_error(&mta_dbh));
      if (msg_debug)
	msg_info("%s: MTA database(%s) reconnectted, id %lu",
	    myname, var_rmail_mtadb_host, mysql_thread_id(&mta_dbh));
    } else if (msg_debug)
      msg_info("%s: MTA database(%s) still alive, id: %lu", myname, var_rmail_mtadb_host, mysql_thread_id(&mta_dbh));

    if (var_rmail_mailcache_enable) {
      if (mysql_ping(&cac_dbh) != 0) {
	msg_warn("%s: CAC database(%s) had gone(%s), attempted to reconnect",
	    myname, var_rmail_cacdb_host, mysql_error(&cac_dbh));
	if (msg_debug)
	  msg_info("%s: CAC database(%s) reconnectted, id %lu",
	      myname, var_rmail_cacdb_host, mysql_thread_id(&cac_dbh));
      } else if (msg_debug)
	msg_info("%s: CAC database(%s) still alive, id: %lu", myname, var_rmail_cacdb_host, mysql_thread_id(&cac_dbh));
    }

    /*
     * Sanity check. This service takes no command-line arguments.
     */
    if (argv[0])
	msg_fatal("unexpected command-line argument: %s", argv[0]);

    /*
     * This routine runs whenever a client connects to the UNIX-domain socket
     * that is dedicated to local mail delivery service. What we see below is
     * a little protocol to (1) tell the client that we are ready, (2) read a
     * delivery request from the client, and (3) report the completion status
     * of that request.
     */
    if ((request = deliver_request_read(stream)) != 0) {
	status = local_deliver(request, service);
	deliver_request_done(stream, request, status);
    }
}

/* pre_accept - see if tables have changed */

static void pre_accept(char *unused_name, char **unused_argv)
{
    const char *table;

    if ((table = dict_changed_name()) != 0) {
	msg_info("table %s has changed -- restarting", table);
	exit(0);
    }
}

/* post_init - post-jail initialization */

static void post_init(char *unused_name, char **unused_argv)
{

    /*
     * Drop privileges most of the time.
     */
    set_eugid(var_owner_uid, var_owner_gid);

    virtual_mailbox_maps =
	maps_create(VAR_VIRT_MAILBOX_MAPS, var_virt_mailbox_maps,
		    DICT_FLAG_LOCK | DICT_FLAG_PARANOID);

    virtual_uid_maps =
	maps_create(VAR_VIRT_UID_MAPS, var_virt_uid_maps,
		    DICT_FLAG_LOCK | DICT_FLAG_PARANOID);

    virtual_gid_maps =
	maps_create(VAR_VIRT_GID_MAPS, var_virt_gid_maps,
		    DICT_FLAG_LOCK | DICT_FLAG_PARANOID);

    virtual_mbox_lock_mask = mbox_lock_mask(var_virt_mailbox_lock);
}

/* pre_init - pre-jail initialization */

static void pre_init(char *unused_name, char **unused_argv)
{
    const char *myname = "pre_init";
    char *buf, *buf1;

    if (msg_verbose)
      msg_debug=1;

    mysql_init(&mta_dbh);
    if (mysql_real_connect(&mta_dbh, var_rmail_mtadb_host, var_rmail_mtadb_user,
	  var_rmail_mtadb_pass, var_rmail_mtadb_name, (unsigned int) var_rmail_mtadb_port,
	  NULL, var_rmail_mtadb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
      msg_fatal("%s: MTA database(%s) connection fail: %s",
	  myname, var_rmail_mtadb_host, mysql_error(&mta_dbh));
    } else if (msg_debug) {
      msg_info("%s: MTA database(%s) connect succussfully, connection id: %lu",
	  myname, var_rmail_mtadb_host, mysql_thread_id(&mta_dbh));
    }


    if (var_rmail_mailcache_enable) {
      if (strcmp(var_rmail_mtadb_host, var_rmail_cacdb_host)==0 &&
	  strcmp(var_rmail_mtadb_user, var_rmail_cacdb_user)==0 &&
	  strcmp(var_rmail_mtadb_pass, var_rmail_cacdb_pass)==0 &&
	  strcmp(var_rmail_mtadb_name, var_rmail_cacdb_name)==0 &&
	  var_rmail_mtadb_port == var_rmail_cacdb_port &&
	  var_rmail_mtadb_interactive == var_rmail_cacdb_interactive) {
	 cac_dbh = mta_dbh;
	 if (msg_debug)
	   msg_info("%s: CAC & MTA database are the same, use one handler",
	       myname);
      } else {
	mysql_init(&cac_dbh);
	if (mysql_real_connect(&cac_dbh, var_rmail_cacdb_host, var_rmail_cacdb_user,
	      var_rmail_cacdb_pass, var_rmail_cacdb_name, (unsigned int) var_rmail_cacdb_port,
	      NULL, var_rmail_cacdb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
	  msg_fatal("%s: CAC database(%s) connection fail: %s",
	      myname, var_rmail_cacdb_host, mysql_error(&cac_dbh));
	} else if (msg_debug) {
	  msg_info("%s: CAC database(%s) connect succussfully, connection id: %lu",
	      myname, var_rmail_cacdb_host, mysql_thread_id(&cac_dbh));
	}
      }
    }

    /* Get myhostname */
    strcpy(buf1, var_myhostname);
    if ((buf = strchr(buf1, '.'))==0)  {
      strcpy(myhostname, var_myhostname); 
    } else {
      buf1[strlen(buf1)-strlen(buf)]='\0';
      strcpy(myhostname, buf1);
    }
    



    /*
     * Reset the file size limit from the message size limit to the mailbox
     * size limit.
     * 
     * We can't have mailbox size limit smaller than the message size limit,
     * because that prohibits the delivery agent from updating the queue
     * file.
     */
    if (var_virt_mailbox_limit) {
	if (var_virt_mailbox_limit < var_message_limit)
	    msg_fatal("main.cf configuration error: %s is smaller than %s",
		      VAR_VIRT_MAILBOX_LIMIT, VAR_MESSAGE_LIMIT);
	set_file_limit(var_virt_mailbox_limit);
    }

    /*
     * flush client.
     */
    flush_init();
}

/* main - pass control to the single-threaded skeleton */

int     main(int argc, char **argv)
{
    static CONFIG_INT_TABLE int_table[] = {
	VAR_VIRT_MINUID, DEF_VIRT_MINUID, &var_virt_minimum_uid, 1, 0,
	VAR_VIRT_MAILBOX_LIMIT, DEF_VIRT_MAILBOX_LIMIT, &var_virt_mailbox_limit, 0, 0,
	/* Rmail */
	VAR_RMAIL_MTADB_PORT, DEF_RMAIL_MTADB_PORT, &var_rmail_mtadb_port, 0, 0,
	VAR_RMAIL_CACDB_PORT, DEF_RMAIL_CACDB_PORT, &var_rmail_cacdb_port, 0, 0,
	VAR_RMAIL_MAILCACHE_EXPIRE, DEF_RMAIL_MAILCACHE_EXPIRE, &var_rmail_mailcache_expire, 0, 0, 
	VAR_RMAIL_DEFAULT_QUOTA, DEF_RMAIL_DEFAULT_QUOTA, &var_rmail_default_quota, 0, 0,
	VAR_RMAIL_QUOTA_EXPIRE, DEF_RMAIL_QUOTA_EXPIRE, &var_rmail_quota_expire, 0, 0,
	/* Rmail end */
	0,
    };

    static CONFIG_BOOL_TABLE bool_table[] = {
	VAR_RMAIL_MTADB_INTERACTIVE, DEF_RMAIL_MTADB_INTERACTIVE, &var_rmail_mtadb_interactive,
	VAR_RMAIL_CACDB_INTERACTIVE, DEF_RMAIL_CACDB_INTERACTIVE, &var_rmail_cacdb_interactive,
	VAR_RMAIL_MAILCACHE_ENABLE, DEF_RMAIL_MAILCACHE_ENABLE, &var_rmail_mailcache_enable,
	VAR_RMAIL_ALLOW_LOCAL, DEF_RMAIL_ALLOW_LOCAL, &var_rmail_allow_local,
	VAR_RMAIL_ZIPMAIL_ENABLE, DEF_RMAIL_ZIPMAIL_ENABLE, &var_rmail_zipmail_enable,
	0,
    };

    static CONFIG_STR_TABLE str_table[] = {
	VAR_MAIL_SPOOL_DIR, DEF_MAIL_SPOOL_DIR, &var_mail_spool_dir, 0, 0,
	VAR_VIRT_MAILBOX_MAPS, DEF_VIRT_MAILBOX_MAPS, &var_virt_mailbox_maps, 0, 0,
	VAR_VIRT_UID_MAPS, DEF_VIRT_UID_MAPS, &var_virt_uid_maps, 0, 0,
	VAR_VIRT_GID_MAPS, DEF_VIRT_GID_MAPS, &var_virt_gid_maps, 0, 0,
	VAR_VIRT_MAILBOX_BASE, DEF_VIRT_MAILBOX_BASE, &var_virt_mailbox_base, 1, 0,
	VAR_VIRT_MAILBOX_LOCK, DEF_VIRT_MAILBOX_LOCK, &var_virt_mailbox_lock, 1, 0,
	/* Rmail */
	VAR_RMAIL_MTADB_HOST, DEF_RMAIL_MTADB_HOST, &var_rmail_mtadb_host, 0, 0,
	VAR_RMAIL_MTADB_USER, DEF_RMAIL_MTADB_USER, &var_rmail_mtadb_user, 0, 0,
	VAR_RMAIL_MTADB_PASS, DEF_RMAIL_MTADB_PASS, &var_rmail_mtadb_pass, 0, 0,
	VAR_RMAIL_MTADB_NAME, DEF_RMAIL_MTADB_NAME, &var_rmail_mtadb_name, 0, 0,

	VAR_RMAIL_CACDB_HOST, DEF_RMAIL_CACDB_HOST, &var_rmail_cacdb_host, 0, 0,
	VAR_RMAIL_CACDB_USER, DEF_RMAIL_CACDB_USER, &var_rmail_cacdb_user, 0, 0,
	VAR_RMAIL_CACDB_PASS, DEF_RMAIL_CACDB_PASS, &var_rmail_cacdb_pass, 0, 0,
	VAR_RMAIL_CACDB_NAME, DEF_RMAIL_CACDB_NAME, &var_rmail_cacdb_name, 0, 0,

	VAR_RMAIL_TRANSPORT_TABLE, DEF_RMAIL_TRANSPORT_TABLE, &var_rmail_transport_table, 0, 0,
	VAR_RMAIL_TRANSPORT_IDXFIELD, DEF_RMAIL_TRANSPORT_IDXFIELD, &var_rmail_transport_idxfield, 0, 0,
	VAR_RMAIL_TRANSPORT_DOMAINFIELD, DEF_RMAIL_TRANSPORT_DOMAINFIELD, &var_rmail_transport_domainfield, 0, 0,
	VAR_RMAIL_TRANSPORT_BASEDIRFIELD, DEF_RMAIL_TRANSPORT_BASEDIRFIELD, &var_rmail_transport_basedirfield, 0, 0,

	VAR_RMAIL_MAILUSER_TABLE, DEF_RMAIL_MAILUSER_TABLE, &var_rmail_mailuser_table, 0, 0,
	VAR_RMAIL_MAILUSER_DOMAINFIELD, DEF_RMAIL_MAILUSER_DOMAINFIELD, &var_rmail_mailuser_domainfield, 0, 0,
	VAR_RMAIL_MAILUSER_MAILIDFIELD, DEF_RMAIL_MAILUSER_MAILIDFIELD, &var_rmail_mailuser_mailidfield, 0, 0,
	VAR_RMAIL_MAILUSER_MHOSTFIELD, DEF_RMAIL_MAILUSER_MHOSTFIELD, &var_rmail_mailuser_mhostfield, 0, 0,
	VAR_RMAIL_MAILUSER_MBOXFIELD, DEF_RMAIL_MAILUSER_MBOXFIELD, &var_rmail_mailuser_mboxfield, 0, 0,
	VAR_RMAIL_MAILUSER_STATUFIELD, DEF_RMAIL_MAILUSER_STATUFIELD, &var_rmail_mailuser_statufield, 0, 0,
	VAR_RMAIL_MAILUSER_SMTPFIELD, DEF_RMAIL_MAILUSER_SMTPFIELD, &var_rmail_mailuser_smtpfield, 0, 0,
	VAR_RMAIL_MAILUSER_POP3FIELD, DEF_RMAIL_MAILUSER_POP3FIELD, &var_rmail_mailuser_pop3field, 0, 0,
	VAR_RMAIL_MAILUSER_WEBFIELD, DEF_RMAIL_MAILUSER_WEBFIELD, &var_rmail_mailuser_webfield, 0, 0,
	
	VAR_RMAIL_MAILCACHE_TABLE, DEF_RMAIL_MAILCACHE_TABLE, &var_rmail_mailcache_table, 0, 0,
	VAR_RMAIL_MAILCACHE_DOMAINFIELD, DEF_RMAIL_MAILCACHE_DOMAINFIELD, &var_rmail_mailcache_domainfield, 0, 0,
	VAR_RMAIL_MAILCACHE_MAILIDFIELD, DEF_RMAIL_MAILCACHE_MAILIDFIELD, &var_rmail_mailcache_mailidfield, 0, 0,
	VAR_RMAIL_MAILCACHE_MHOSTFIELD, DEF_RMAIL_MAILCACHE_MHOSTFIELD, &var_rmail_mailcache_mhostfield, 0, 0,
	VAR_RMAIL_MAILCACHE_MBOXFIELD, DEF_RMAIL_MAILCACHE_MBOXFIELD, &var_rmail_mailcache_mboxfield, 0, 0,
	VAR_RMAIL_MAILCACHE_STATUFIELD, DEF_RMAIL_MAILCACHE_STATUFIELD, &var_rmail_mailcache_statufield, 0, 0,
	VAR_RMAIL_MAILCACHE_SMTPFIELD, DEF_RMAIL_MAILCACHE_SMTPFIELD, &var_rmail_mailcache_smtpfield, 0, 0,
	VAR_RMAIL_MAILCACHE_POP3FIELD, DEF_RMAIL_MAILCACHE_POP3FIELD, &var_rmail_mailcache_pop3field, 0, 0,
	VAR_RMAIL_MAILCACHE_WEBFIELD, DEF_RMAIL_MAILCACHE_WEBFIELD, &var_rmail_mailcache_webfield, 0, 0,
	VAR_RMAIL_MAILCACHE_TIMEFIELD, DEF_RMAIL_MAILCACHE_TIMEFIELD, &var_rmail_mailcache_timefield, 0, 0,

	VAR_RMAIL_MAILDIR_SUFIX, DEF_RMAIL_MAILDIR_SUFIX, &var_rmail_maildir_sufix, 0, 0,
	VAR_RMAIL_QUOTA_FILE, DEF_RMAIL_QUOTA_FILE, &var_rmail_quota_file, 0, 0,
	VAR_RMAIL_QUOTA_TMPFILE, DEF_RMAIL_QUOTA_TMPFILE, &var_rmail_quota_tmpfile, 0, 0,
	VAR_RMAIL_DEFAULT_BASEDIR, DEF_RMAIL_DEFAULT_BASEDIR, &var_rmail_default_basedir, 0, 0,

	/* Rmail End */
	0,
    };

    single_server_main(argc, argv, local_service,
		       MAIL_SERVER_INT_TABLE, int_table,
		       MAIL_SERVER_STR_TABLE, str_table,
		       MAIL_SERVER_PRE_INIT, pre_init,
		       MAIL_SERVER_BOOL_TABLE, bool_table,
		       MAIL_SERVER_POST_INIT, post_init,
		       MAIL_SERVER_PRE_ACCEPT, pre_accept,
		       0);
}
