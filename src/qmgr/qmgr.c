/*++
/* NAME
/*	qmgr 8
/* SUMMARY
/*	Postfix queue manager
/* SYNOPSIS
/*	\fBqmgr\fR [generic Postfix daemon options]
/* DESCRIPTION
/*	The \fBqmgr\fR daemon awaits the arrival of incoming mail
/*	and arranges for its delivery via Postfix delivery processes.
/*	The actual mail routing strategy is delegated to the
/*	\fBtrivial-rewrite\fR(8) daemon.
/*	This program expects to be run from the \fBmaster\fR(8) process
/*	manager.
/*
/*	Mail addressed to the local \fBdouble-bounce\fR address is
/*	logged and discarded.  This stops potential loops caused by
/*	undeliverable bounce notifications.
/* MAIL QUEUES
/* .ad
/* .fi
/*	The \fBqmgr\fR daemon maintains the following queues:
/* .IP \fBincoming\fR
/*	Inbound mail from the network, or mail picked up by the
/*	local \fBpickup\fR agent from the \fBmaildrop\fR directory.
/* .IP \fBactive\fR
/*	Messages that the queue manager has opened for delivery. Only
/*	a limited number of messages is allowed to enter the \fBactive\fR
/*	queue (leaky bucket strategy, for a fixed delivery rate).
/* .IP \fBdeferred\fR
/*	Mail that could not be delivered upon the first attempt. The queue
/*	manager implements exponential backoff by doubling the time between
/*	delivery attempts.
/* .IP \fBcorrupt\fR
/*	Unreadable or damaged queue files are moved here for inspection.
/* .IP \fBhold\fR
/*	Messages that are kept "on hold" are kept here until someone
/*	sets them free.
/* DELIVERY STATUS REPORTS
/* .ad
/* .fi
/*	The \fBqmgr\fR daemon keeps an eye on per-message delivery status
/*	reports in the following directories. Each status report file has
/*	the same name as the corresponding message file:
/* .IP \fBbounce\fR
/*	Per-recipient status information about why mail is bounced.
/*	These files are maintained by the \fBbounce\fR(8) daemon.
/* .IP \fBdefer\fR
/*	Per-recipient status information about why mail is delayed.
/*	These files are maintained by the \fBdefer\fR(8) daemon.
/* .IP \fBtrace\fR
/*	Per-recipient status information as requested with the
/*	Postfix "\fBsendmail -v\fR" or "\fBsendmail -bv\fR" command.
/*	These files are maintained by the \fBtrace\fR(8) daemon.
/* .PP
/*	The \fBqmgr\fR daemon is responsible for asking the
/*	\fBbounce\fR(8), \fBdefer\fR(8) or \fBtrace\fR(8) daemons to
/*	send delivery reports.
/* STRATEGIES
/* .ad
/* .fi
/*	The queue manager implements a variety of strategies for
/*	either opening queue files (input) or for message delivery (output).
/* .IP "\fBleaky bucket\fR"
/*	This strategy limits the number of messages in the \fBactive\fR queue
/*	and prevents the queue manager from running out of memory under
/*	heavy load.
/* .IP \fBfairness\fR
/*	When the \fBactive\fR queue has room, the queue manager takes one
/*	message from the \fBincoming\fR queue and one from the \fBdeferred\fR
/*	queue. This prevents a large mail backlog from blocking the delivery
/*	of new mail.
/* .IP "\fBslow start\fR"
/*	This strategy eliminates "thundering herd" problems by slowly
/*	adjusting the number of parallel deliveries to the same destination.
/* .IP "\fBround robin\fR
/*	The queue manager sorts delivery requests by destination.
/*	Round-robin selection prevents one destination from dominating
/*	deliveries to other destinations.
/* .IP "\fBexponential backoff\fR"
/*	Mail that cannot be delivered upon the first attempt is deferred.
/*	The time interval between delivery attempts is doubled after each
/*	attempt.
/* .IP "\fBdestination status cache\fR"
/*	The queue manager avoids unnecessary delivery attempts by
/*	maintaining a short-term, in-memory list of unreachable destinations.
/* .IP "\fBpreemptive message scheduling\fR"
/*	The queue manager attempts to minimize the average per-recipient delay
/*	while still preserving the correct per-message delays, using
/*	a sophisticated preemptive message scheduling.
/* TRIGGERS
/* .ad
/* .fi
/*	On an idle system, the queue manager waits for the arrival of
/*	trigger events, or it waits for a timer to go off. A trigger
/*	is a one-byte message.
/*	Depending on the message received, the queue manager performs
/*	one of the following actions (the message is followed by the
/*	symbolic constant used internally by the software):
/* .IP "\fBD (QMGR_REQ_SCAN_DEFERRED)\fR"
/*	Start a deferred queue scan.  If a deferred queue scan is already
/*	in progress, that scan will be restarted as soon as it finishes.
/* .IP "\fBI (QMGR_REQ_SCAN_INCOMING)\fR"
/*	Start an incoming queue scan. If an incoming queue scan is already
/*	in progress, that scan will be restarted as soon as it finishes.
/* .IP "\fBA (QMGR_REQ_SCAN_ALL)\fR"
/*	Ignore deferred queue file time stamps. The request affects
/*	the next deferred queue scan.
/* .IP "\fBF (QMGR_REQ_FLUSH_DEAD)\fR"
/*	Purge all information about dead transports and destinations.
/* .IP "\fBW (TRIGGER_REQ_WAKEUP)\fR"
/*	Wakeup call, This is used by the master server to instantiate
/*	servers that should not go away forever. The action is to start
/*	an incoming queue scan.
/* .PP
/*	The \fBqmgr\fR daemon reads an entire buffer worth of triggers.
/*	Multiple identical trigger requests are collapsed into one, and
/*	trigger requests are sorted so that \fBA\fR and \fBF\fR precede
/*	\fBD\fR and \fBI\fR. Thus, in order to force a deferred queue run,
/*	one would request \fBA F D\fR; in order to notify the queue manager
/*	of the arrival of new mail one would request \fBI\fR.
/* STANDARDS
/* .ad
/* .fi
/*	None. The \fBqmgr\fR daemon does not interact with the outside world.
/* SECURITY
/* .ad
/* .fi
/*	The \fBqmgr\fR daemon is not security sensitive. It reads
/*	single-character messages from untrusted local users, and thus may
/*	be susceptible to denial of service attacks. The \fBqmgr\fR daemon
/*	does not talk to the outside world, and it can be run at fixed low
/*	privilege in a chrooted environment.
/* DIAGNOSTICS
/*	Problems and transactions are logged to the syslog daemon.
/*	Corrupted message files are saved to the \fBcorrupt\fR queue
/*	for further inspection.
/*
/*	Depending on the setting of the \fBnotify_classes\fR parameter,
/*	the postmaster is notified of bounces and of other trouble.
/* BUGS
/*	A single queue manager process has to compete for disk access with
/*	multiple front-end processes such as \fBsmtpd\fR. A sudden burst of
/*	inbound mail can negatively impact outbound delivery rates.
/* CONFIGURATION PARAMETERS
/* .ad
/* .fi
/*	Changes to \fBmain.cf\fR are not picked up automatically as qmgr(8)
/*	processes are persistent. Use the \fBpostfix reload\fR command after
/*	a configuration change.
/*
/*	The text below provides only a parameter summary. See
/*	postconf(5) for more details including examples.
/*
/*	In the text below, \fItransport\fR is the first field in a
/*	\fBmaster.cf\fR entry.
/* COMPATIBILITY CONTROLS
/* .ad
/* .fi
/* .IP "\fBallow_min_user (no)\fR"
/*	Allow a recipient address to have `-' as the first character.
/* ACTIVE QUEUE CONTROLS
/* .ad
/* .fi
/* .IP "\fBqmgr_clog_warn_time (300s)\fR"
/*	The minimal delay between warnings that a specific destination is
/*	clogging up the Postfix active queue.
/* .IP "\fBqmgr_message_active_limit (20000)\fR"
/*	The maximal number of messages in the active queue.
/* .IP "\fBqmgr_message_recipient_limit (20000)\fR"
/*	The maximal number of recipients held in memory by the Postfix
/*	queue manager, and the maximal size of the size of the short-term,
/*	in-memory "dead" destination status cache.
/* .IP "\fBqmgr_message_recipient_minimum (10)\fR"
/*	The minimal number of in-memory recipients for any message.
/* .IP "\fBdefault_recipient_limit (10000)\fR"
/*	The default per-transport upper limit on the number of in-memory
/*	recipients.
/* .IP "\fItransport\fB_recipient_limit ($default_recipient_limit)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* .IP "\fBdefault_extra_recipient_limit (1000)\fR"
/*	The default value for the extra per-transport limit imposed on the
/*	number of in-memory recipients.
/* .IP "\fItransport\fB_extra_recipient_limit ($default_extra_recipient_limit)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* DELIVERY CONCURRENCY CONTROLS
/* .ad
/* .fi
/* .IP "\fBinitial_destination_concurrency (5)\fR"
/*	The initial per-destination concurrency level for parallel delivery
/*	to the same destination.
/* .IP "\fBdefault_destination_concurrency_limit (20)\fR"
/*	The default maximal number of parallel deliveries to the same
/*	destination.
/* .IP "\fItransport\fB_destination_concurrency_limit ($default_destination_concurrency_limit)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* RECIPIENT SCHEDULING CONTROLS
/* .ad
/* .fi
/* .IP "\fBdefault_destination_recipient_limit (50)\fR"
/*	The default maximal number of recipients per message delivery.
/* .IP "\fItransport\fB_destination_recipient_limit ($default_destination_recipient_limit)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* MESSAGE SCHEDULING CONTROLS
/* .ad
/* .fi
/* .IP "\fBdefault_delivery_slot_cost (5)\fR"
/*	How often the Postfix queue manager's scheduler is allowed to
/*	preempt delivery of one message with another.
/* .IP "\fItransport\fB_delivery_slot_cost ($default_delivery_slot_cost)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* .IP "\fBdefault_minimum_delivery_slots (3)\fR"
/*	How many recipients a message must have in order to invoke the
/*	Postfix queue manager's scheduling algorithm at all.
/* .IP "\fItransport\fB_minimum_delivery_slots ($default_minimum_delivery_slots)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* .IP "\fBdefault_delivery_slot_discount (50)\fR"
/*	The default value for transport-specific _delivery_slot_discount
/*	settings.
/* .IP "\fItransport\fB_delivery_slot_discount ($default_delivery_slot_discount)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* .IP "\fBdefault_delivery_slot_loan (3)\fR"
/*	The default value for transport-specific _delivery_slot_loan
/*	settings.
/* .IP "\fItransport\fB_delivery_slot_loan ($default_delivery_slot_loan)\fR"
/*	Idem, for delivery via the named message \fItransport\fR.
/* OTHER RESOURCE AND RATE CONTROLS
/* .ad
/* .fi
/* .IP "\fBminimal_backoff_time (1000s)\fR"
/*	The minimal time between attempts to deliver a deferred message.
/* .IP "\fBmaximal_backoff_time (4000s)\fR"
/*	The maximal time between attempts to deliver a deferred message.
/* .IP "\fBmaximal_queue_lifetime (5d)\fR"
/*	The maximal time a message is queued before it is sent back as
/*	undeliverable.
/* .IP "\fBqueue_run_delay (1000s)\fR"
/*	The time between deferred queue scans by the queue manager.
/* .IP "\fBtransport_retry_time (60s)\fR"
/*	The time between attempts by the Postfix queue manager to contact
/*	a malfunctioning message delivery transport.
/* .PP
/*	Available in Postfix version 2.1 and later:
/* .IP "\fBbounce_queue_lifetime (5d)\fR"
/*	The maximal time a bounce message is queued before it is considered
/*	undeliverable.
/* MISCELLANEOUS CONTROLS
/* .ad
/* .fi
/* .IP "\fBconfig_directory (see 'postconf -d' output)\fR"
/*	The default location of the Postfix main.cf and master.cf
/*	configuration files.
/* .IP "\fBdaemon_timeout (18000s)\fR"
/*	How much time a Postfix daemon process may take to handle a
/*	request before it is terminated by a built-in watchdog timer.
/* .IP "\fBdefer_transports (empty)\fR"
/*	The names of message delivery transports that should not be delivered
/*	to unless someone issues "\fBsendmail -q\fR" or equivalent.
/* .IP "\fBhelpful_warnings (yes)\fR"
/*	Log warnings about problematic configuration settings, and provide
/*	helpful suggestions.
/* .IP "\fBipc_timeout (3600s)\fR"
/*	The time limit for sending or receiving information over an internal
/*	communication channel.
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
/* FILES
/*	/var/spool/postfix/incoming, incoming queue
/*	/var/spool/postfix/active, active queue
/*	/var/spool/postfix/deferred, deferred queue
/*	/var/spool/postfix/bounce, non-delivery status
/*	/var/spool/postfix/defer, non-delivery status
/*	/var/spool/postfix/trace, delivery status
/* SEE ALSO
/*	trivial-rewrite(8), address routing
/*	bounce(8), delivery status reports
/*	postconf(5), configuration parameters
/*	master(8), process manager
/*	syslogd(8) system logging
/* README FILES
/* .ad
/* .fi
/*	Use "\fBpostconf readme_directory\fR" or
/*	"\fBpostconf html_directory\fR" to locate this information.
/* .na
/* .nf
/*	SCHEDULER_README, scheduling algorithm
/*	QSHAPE_README, Postfix queue analysis
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
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

/* Utility library. */

#include <msg.h>
#include <events.h>
#include <vstream.h>
#include <dict.h>

/* Global library. */

#include <mail_queue.h>
#include <recipient_list.h>
#include <mail_conf.h>
#include <mail_params.h>
#include <mail_proto.h>			/* QMGR_SCAN constants */
#include <mail_flow.h>
#include <flush_clnt.h>

/* Master process interface */

#include <master_proto.h>
#include <mail_server.h>

/* Rmail need mysql headers */
#include <mysql.h>

/* Application-specific. */

#include "qmgr.h"

 /*
  * Tunables.
  */
int     var_queue_run_delay;
int     var_min_backoff_time;
int     var_max_backoff_time;
int     var_max_queue_time;
int     var_dsn_queue_time;
int     var_qmgr_active_limit;
int     var_qmgr_rcpt_limit;
int     var_qmgr_msg_rcpt_limit;
int     var_xport_rcpt_limit;
int     var_stack_rcpt_limit;
int     var_delivery_slot_cost;
int     var_delivery_slot_loan;
int     var_delivery_slot_discount;
int     var_min_delivery_slots;
int     var_init_dest_concurrency;
int     var_transport_retry_time;
int     var_dest_con_limit;
int     var_dest_rcpt_limit;
char   *var_defer_xports;
bool    var_allow_min_user;
int     var_local_con_lim;
int     var_local_rcpt_lim;
int     var_proc_limit;
bool    var_verp_bounce_off;
bool    var_sender_routing;
int     var_qmgr_clog_warn_time;

/* Rmail variables start */
char	*var_rmail_mtadb_host;
char	*var_rmail_mtadb_user;
char	*var_rmail_mtadb_pass;
char	*var_rmail_mtadb_name;

char	*var_rmail_cacdb_host;
char	*var_rmail_cacdb_user;
char	*var_rmail_cacdb_pass;
char	*var_rmail_cacdb_name;

char	*var_rmail_transport_table;
char	*var_rmail_transport_idxfield;
char	*var_rmail_transport_domainfield;
char	*var_rmail_transport_basedirfield;

char	*var_rmail_hostmap_table;
char	*var_rmail_hostmap_domainfield;
char	*var_rmail_hostmap_nodenamefield;
char	*var_rmail_hostmap_hostnamefield;

char	*var_rmail_mailuser_table;
char	*var_rmail_mailuser_domainfield;
char	*var_rmail_mailuser_mailidfield;
char	*var_rmail_mailuser_mhostfield;
char	*var_rmail_mailuser_mboxfield;
char	*var_rmail_mailuser_statufield;
char	*var_rmail_mailuser_smtpfield;
char	*var_rmail_mailuser_pop3field;
char	*var_rmail_mailuser_webfield;

char	*var_rmail_mailcache_table;
char	*var_rmail_mailcache_domainfield;
char	*var_rmail_mailcache_mailidfield;
char	*var_rmail_mailcache_mhostfield;
char	*var_rmail_mailcache_mboxfield;
char	*var_rmail_mailcache_statufield;
char	*var_rmail_mailcache_smtpfield;
char	*var_rmail_mailcache_pop3field;
char	*var_rmail_mailcache_webfield;
char	*var_rmail_mailcache_timefield;

char	*var_rmail_dropdomain_table;
char	*var_rmail_dropdomain_domainfield;
char	*var_rmail_dropdomain_reasonfield;
char	*var_rmail_dropdomain_addtimefield;

char	*var_rmail_default_deliver;
char	*var_rmail_local_nexthop;



int   var_rmail_mtadb_port;
int   var_rmail_cacdb_port;
int   var_rmail_mailcache_expire;
int   var_rmail_license_userlimit;
int   var_rmail_license_domainlimit;
int   var_rmail_hostmapcache_expire;
int   var_rmail_domaincache_expire;



bool	var_rmail_mtadb_interactive;
bool	var_rmail_cacdb_interactive;
bool	var_rmail_mailcache_enable;
bool	var_rmail_allow_local;
bool	var_rmail_ismx;
bool	var_rmail_dropdomain_enable;
bool	var_rmail_returnmail_enable;
bool	var_rmail_hostmapcache_enable;
bool	var_rmail_domaincache_enable;


MYSQL mta_dbh;
MYSQL cac_dbh;

HOSTMAP_ENTRY *hmap_cache;
int hmap_last;
TRANSPORT_ENTRY *tran_cache;
int tran_last;

char myhostname[64];

/* Rmail variables end */

static QMGR_SCAN *qmgr_incoming;
static QMGR_SCAN *qmgr_deferred;

/* qmgr_deferred_run_event - queue manager heartbeat */

static void qmgr_deferred_run_event(int unused_event, char *dummy)
{

    /*
     * This routine runs when it is time for another deferred queue scan.
     * Make sure this routine gets called again in the future.
     */
    qmgr_scan_request(qmgr_deferred, QMGR_SCAN_START);
    event_request_timer(qmgr_deferred_run_event, dummy, var_queue_run_delay);
}

/* qmgr_trigger_event - respond to external trigger(s) */

static void qmgr_trigger_event(char *buf, int len,
			               char *unused_service, char **argv)
{
    int     incoming_flag = 0;
    int     deferred_flag = 0;
    int     i;

    /*
     * Sanity check. This service takes no command-line arguments.
     */
    if (argv[0])
	msg_fatal("unexpected command-line argument: %s", argv[0]);

    /*
     * Collapse identical requests that have arrived since we looked last
     * time. There is no client feedback so there is no need to process each
     * request in order. And as long as we don't have conflicting requests we
     * are free to sort them into the most suitable order.
     */
    for (i = 0; i < len; i++) {
	if (msg_verbose)
	    msg_info("request: %d (%c)",
		     buf[i], ISALNUM(buf[i]) ? buf[i] : '?');
	switch (buf[i]) {
	case TRIGGER_REQ_WAKEUP:
	case QMGR_REQ_SCAN_INCOMING:
	    incoming_flag |= QMGR_SCAN_START;
	    break;
	case QMGR_REQ_SCAN_DEFERRED:
	    deferred_flag |= QMGR_SCAN_START;
	    break;
	case QMGR_REQ_FLUSH_DEAD:
	    deferred_flag |= QMGR_FLUSH_DEAD;
	    incoming_flag |= QMGR_FLUSH_DEAD;
	    break;
	case QMGR_REQ_SCAN_ALL:
	    deferred_flag |= QMGR_SCAN_ALL;
	    incoming_flag |= QMGR_SCAN_ALL;
	    break;
	default:
	    if (msg_verbose)
		msg_info("request ignored");
	    break;
	}
    }

    /*
     * Process each request type at most once. Modifiers take effect upon the
     * next queue run. If no queue run is in progress, and a queue scan is
     * requested, the request takes effect immediately.
     */
    if (incoming_flag != 0)
	qmgr_scan_request(qmgr_incoming, incoming_flag);
    if (deferred_flag != 0)
	qmgr_scan_request(qmgr_deferred, deferred_flag);
}

/* qmgr_loop - queue manager main loop */

static int qmgr_loop(char *unused_name, char **unused_argv)
{
    char   *in_path = 0;
    char   *df_path = 0;
    int     token_count;
    int     in_feed = 0;

    const char *myname="qmgr_loop";
    char sql[512];
    MYSQL_RES *res;
    MYSQL_ROW row;
    HOSTMAP_ENTRY *cur_hostmap, *this_hostmap;
    TRANSPORT_ENTRY *cur_transport, *this_transport;
    int i=0;


    /* Check database handler first */

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


    /* Hostmap Cache */
    if (var_rmail_hostmapcache_enable && time(NULL) - hmap_last > var_rmail_hostmapcache_expire) {
      if (msg_debug)
	msg_info("%s: Attempt to refresh hostmap cache", myname);
      if (hmap_cache != NULL)
	free(hmap_cache);
      
      sprintf(sql, "SELECT %s, %s, %s FROM %s",
	  var_rmail_hostmap_domainfield, var_rmail_hostmap_hostnamefield,
	  var_rmail_hostmap_nodenamefield, var_rmail_hostmap_table);
      if (msg_debug)
	msg_info("%s: SQL[MTA] => %s", myname, sql);

      if (mysql_real_query(&mta_dbh, sql, strlen(sql)) != 0) { // query fail
	msg_warn("%s: MTA Database query fail: %s", myname, mysql_error(&mta_dbh));
	hmap_cache=NULL;
      } else {
	res = mysql_store_result(&mta_dbh);
	for (i=0; i< mysql_num_rows(res); i++) {
	  row = mysql_fetch_row(res);
	  cur_hostmap = (HOSTMAP_ENTRY *) malloc (sizeof(HOSTMAP_ENTRY));

	  cur_hostmap->idx = atoi(row[0]);
	  strcpy(cur_hostmap->hostname, row[1]);
	  strcpy(cur_hostmap->nodename, row[2]);
	  
	  if (msg_debug)
	    msg_info("%s: Insert hostmap cache %d:%s:%s",
		myname, cur_hostmap->idx, cur_hostmap->hostname, cur_hostmap->nodename);
	
	  if (i==0)
	    hmap_cache = cur_hostmap;
	  else
	    this_hostmap->next_entry = cur_hostmap;

	  this_hostmap = cur_hostmap;
	  this_hostmap->next_entry=NULL;
	}
	mysql_free_result(res);
      }
      hmap_last=time(NULL);
    } else if (var_rmail_hostmapcache_enable && msg_debug)
      msg_info("%s: Hostmap cache not expire (%d)",
	  myname, var_rmail_hostmapcache_expire + hmap_last - time(NULL));

    
    if (var_rmail_domaincache_enable && time(NULL) - tran_last > var_rmail_domaincache_expire) {
      if (msg_debug)
	msg_info("%s: Attempt to refresh domain cache", myname);
      if (tran_cache != NULL)
	free(tran_cache);

      sprintf(sql, "SELECT %s, %s, %s FROM %s",
	  var_rmail_transport_idxfield, var_rmail_transport_domainfield,
	  var_rmail_transport_basedirfield, var_rmail_transport_table);
      if (msg_debug)
	msg_info("%s: SQL[MTA] => %s", myname, sql);

      if (mysql_real_query(&mta_dbh, sql, strlen(sql)) != 0) { // query fail
	msg_warn("%s: MTA Database query fail: %s", myname, mysql_error(&mta_dbh));
	tran_cache=NULL;
      } else {
	res = mysql_store_result(&mta_dbh);
	for (i=0; i< mysql_num_rows(res); i++) {
	  row = mysql_fetch_row(res);
	  cur_transport = (TRANSPORT_ENTRY *) malloc (sizeof(TRANSPORT_ENTRY));

	  cur_transport->idx = atoi(row[0]);
	  strcpy(cur_transport->domain, row[1]);
	  strcpy(cur_transport->basedir, row[2]);

	  if (msg_debug)
	    msg_info("%s: Insert domain cache %d:%s:%s",
		myname, cur_transport->idx, cur_transport->domain, cur_transport->basedir);

	  if (i==0)
	    tran_cache = cur_transport;
	  else
	    this_transport->next_entry = cur_transport;

	  this_transport = cur_transport;
	  this_transport->next_entry = NULL;
	}
	mysql_free_result(res);
      }
      tran_last=time(NULL);
    } else if (var_rmail_domaincache_enable && msg_debug)
      msg_info("%s: Domain cache not expire (%d)",
	  myname, var_rmail_domaincache_expire + tran_last - time(NULL));
    

    /*
     * This routine runs as part of the event handling loop, after the event
     * manager has delivered a timer or I/O event (including the completion
     * of a connection to a delivery process), or after it has waited for a
     * specified amount of time. The result value of qmgr_loop() specifies
     * how long the event manager should wait for the next event.
     */
#define DONT_WAIT	0
#define WAIT_FOR_EVENT	(-1)

    /*
     * Attempt to drain the active queue by allocating a suitable delivery
     * process and by delivering mail via it. Delivery process allocation and
     * mail delivery are asynchronous.
     */
    qmgr_active_drain();

    /*
     * Let some new blood into the active queue when the queue size is
     * smaller than some configurable limit. When the system is under heavy
     * load, favor new mail over old mail.
     */
    if (qmgr_message_count < var_qmgr_active_limit)
	if ((in_path = qmgr_scan_next(qmgr_incoming)) != 0)
	    in_feed = qmgr_active_feed(qmgr_incoming, in_path);
    if (qmgr_message_count < var_qmgr_active_limit)
	if ((df_path = qmgr_scan_next(qmgr_deferred)) != 0)
	    qmgr_active_feed(qmgr_deferred, df_path);

    /*
     * Global flow control. If enabled, slow down receiving processes that
     * get ahead of the queue manager, but don't block them completely.
     */
    if (var_in_flow_delay > 0) {
	token_count = mail_flow_count();
	if (token_count < var_proc_limit) {
	    if (in_feed != 0)
		mail_flow_put(1);
	    else if (qmgr_incoming->handle == 0)
		mail_flow_put(var_proc_limit - token_count);
	} else if (token_count > var_proc_limit) {
	    mail_flow_get(token_count - var_proc_limit);
	}
    }
    if (in_path || df_path)
	return (DONT_WAIT);
    return (WAIT_FOR_EVENT);
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

/* qmgr_pre_init - pre-jail initialization */

static void qmgr_pre_init(char *unused_name, char **unused_argv)
{
    flush_init();
}

/* qmgr_post_init - post-jail initialization */

static void qmgr_post_init(char *name, char **unused_argv)
{
    const char *myname = "qmgr_post_init";
    char *buf, *buf1;
    int valid=0;
    /*
     * Backwards compatibility.
     */
    if (strcmp(var_procname, "nqmgr") == 0) {
	msg_warn("please update the %s/%s file; the new queue manager",
		 var_config_dir, MASTER_CONF_FILE);
	msg_warn("(old name: nqmgr) has become the standard queue manager (new name: qmgr)");
	msg_warn("support for the name old name (nqmgr) will be removed from Postfix");
    }

    /*
     * Sanity check.
     */
    if (var_qmgr_rcpt_limit < var_qmgr_active_limit) {
	msg_warn("%s is smaller than %s",
		 VAR_QMGR_RCPT_LIMIT, VAR_QMGR_ACT_LIMIT);
	var_qmgr_rcpt_limit = var_qmgr_active_limit;
    }

    /*
     * This routine runs after the skeleton code has entered the chroot jail.
     * Prevent automatic process suicide after a limited number of client
     * requests or after a limited amount of idle time. Move any left-over
     * entries from the active queue to the incoming queue, and give them a
     * time stamp into the future, in order to allow ongoing deliveries to
     * finish first. Start scanning the incoming and deferred queues.
     * Left-over active queue entries are moved to the incoming queue because
     * the incoming queue has priority; moving left-overs to the deferred
     * queue could cause anomalous delays when "postfix reload/start" are
     * issued often.
     */
    var_use_limit = 0;
    var_idle_limit = 0;
    qmgr_move(MAIL_QUEUE_ACTIVE, MAIL_QUEUE_INCOMING, event_time());
    qmgr_incoming = qmgr_scan_create(MAIL_QUEUE_INCOMING);
    qmgr_deferred = qmgr_scan_create(MAIL_QUEUE_DEFERRED);
    qmgr_scan_request(qmgr_incoming, QMGR_SCAN_START);
    qmgr_deferred_run_event(0, (char *) 0);


    /* Rmail here */
    mysql_init(&mta_dbh);
    if (mysql_real_connect(&mta_dbh, var_rmail_mtadb_host, var_rmail_mtadb_user,
	  var_rmail_mtadb_pass, var_rmail_mtadb_name, (unsigned int) var_rmail_mtadb_port,
	  NULL, var_rmail_mtadb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
      msg_fatal("%s: MTA database(%s) connection fail: %s",
	  myname, var_rmail_mtadb_host, mysql_error(&mta_dbh));
    } else if (msg_debug) {
      msg_info("%s: MTA database(%s) connect successfully, connection id: %lu",
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
	} else if (msg_debug)
	  msg_info("%s: CAC database(%s) connect succussfully, connection id: %lu",
	      myname, var_rmail_cacdb_host, mysql_thread_id(&cac_dbh));
      }
    }

    /* Init cache */
    hmap_cache=NULL;
    hmap_last=0;
    tran_cache=NULL;
    tran_last=0;

    /* Get myhostname */

		buf1 = (char *) malloc (strlen(var_myhostname));
    strcpy(buf1, var_myhostname);
    if ((buf = strchr(buf1, '.'))==0)  {
      strcpy(myhostname, var_myhostname); 
    } else {
      buf1[strlen(buf1)-strlen(buf)]='\0';
      strcpy(myhostname, buf1);
    }

    /* XXX check license */



    

    
}

/* main - the main program */

int     main(int argc, char **argv)
{
    static CONFIG_STR_TABLE str_table[] = {
	VAR_DEFER_XPORTS, DEF_DEFER_XPORTS, &var_defer_xports, 0, 0,
	/* Rmail start */
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

	VAR_RMAIL_HOSTMAP_TABLE, DEF_RMAIL_HOSTMAP_TABLE, &var_rmail_hostmap_table, 0, 0,
	VAR_RMAIL_HOSTMAP_DOMAINFIELD, DEF_RMAIL_HOSTMAP_DOMAINFIELD, &var_rmail_hostmap_domainfield, 0, 0,
	VAR_RMAIL_HOSTMAP_NODENAMEFIELD, DEF_RMAIL_HOSTMAP_NODENAMEFIELD, &var_rmail_hostmap_nodenamefield, 0, 0,
	VAR_RMAIL_HOSTMAP_HOSTNAMEFIELD, DEF_RMAIL_HOSTMAP_HOSTNAMEFIELD, &var_rmail_hostmap_hostnamefield, 0, 0,

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

	VAR_RMAIL_DROPDOMAIN_TABLE, DEF_RMAIL_DROPDOMAIN_TABLE, &var_rmail_dropdomain_table, 0, 0,
	VAR_RMAIL_DROPDOMAIN_DOMAINFIELD, DEF_RMAIL_DROPDOMAIN_DOMAINFIELD, &var_rmail_dropdomain_domainfield, 0, 0,
	VAR_RMAIL_DROPDOMAIN_REASONFIELD, DEF_RMAIL_DROPDOMAIN_REASONFIELD, &var_rmail_dropdomain_reasonfield, 0, 0, 
	VAR_RMAIL_DROPDOMAIN_ADDTIMEFIELD, DEF_RMAIL_DROPDOMAIN_ADDTIMEFIELD, &var_rmail_dropdomain_addtimefield, 0, 0,

	VAR_RMAIL_DEFAULT_DELIVER, DEF_RMAIL_DEFAULT_DELIVER, &var_rmail_default_deliver, 0, 0,
	VAR_RMAIL_LOCAL_NEXTHOP, DEF_RMAIL_LOCAL_NEXTHOP, &var_rmail_local_nexthop, 0, 0,
	/* Rmail end */
	0,
    };
    static CONFIG_TIME_TABLE time_table[] = {
	VAR_QUEUE_RUN_DELAY, DEF_QUEUE_RUN_DELAY, &var_queue_run_delay, 1, 0,
	VAR_MIN_BACKOFF_TIME, DEF_MIN_BACKOFF_TIME, &var_min_backoff_time, 1, 0,
	VAR_MAX_BACKOFF_TIME, DEF_MAX_BACKOFF_TIME, &var_max_backoff_time, 1, 0,
	VAR_MAX_QUEUE_TIME, DEF_MAX_QUEUE_TIME, &var_max_queue_time, 0, 8640000,
	VAR_DSN_QUEUE_TIME, DEF_DSN_QUEUE_TIME, &var_dsn_queue_time, 0, 8640000,
	VAR_XPORT_RETRY_TIME, DEF_XPORT_RETRY_TIME, &var_transport_retry_time, 1, 0,
	VAR_QMGR_CLOG_WARN_TIME, DEF_QMGR_CLOG_WARN_TIME, &var_qmgr_clog_warn_time, 0, 0,
	0,
    };
    static CONFIG_INT_TABLE int_table[] = {
	VAR_QMGR_ACT_LIMIT, DEF_QMGR_ACT_LIMIT, &var_qmgr_active_limit, 1, 0,
	VAR_QMGR_RCPT_LIMIT, DEF_QMGR_RCPT_LIMIT, &var_qmgr_rcpt_limit, 1, 0,
	VAR_QMGR_MSG_RCPT_LIMIT, DEF_QMGR_MSG_RCPT_LIMIT, &var_qmgr_msg_rcpt_limit, 1, 0,
	VAR_XPORT_RCPT_LIMIT, DEF_XPORT_RCPT_LIMIT, &var_xport_rcpt_limit, 0, 0,
	VAR_STACK_RCPT_LIMIT, DEF_STACK_RCPT_LIMIT, &var_stack_rcpt_limit, 0, 0,
	VAR_DELIVERY_SLOT_COST, DEF_DELIVERY_SLOT_COST, &var_delivery_slot_cost, 0, 0,
	VAR_DELIVERY_SLOT_LOAN, DEF_DELIVERY_SLOT_LOAN, &var_delivery_slot_loan, 0, 0,
	VAR_DELIVERY_SLOT_DISCOUNT, DEF_DELIVERY_SLOT_DISCOUNT, &var_delivery_slot_discount, 0, 100,
	VAR_MIN_DELIVERY_SLOTS, DEF_MIN_DELIVERY_SLOTS, &var_min_delivery_slots, 0, 0,
	VAR_INIT_DEST_CON, DEF_INIT_DEST_CON, &var_init_dest_concurrency, 1, 0,
	VAR_DEST_CON_LIMIT, DEF_DEST_CON_LIMIT, &var_dest_con_limit, 0, 0,
	VAR_DEST_RCPT_LIMIT, DEF_DEST_RCPT_LIMIT, &var_dest_rcpt_limit, 0, 0,
	VAR_LOCAL_RCPT_LIMIT, DEF_LOCAL_RCPT_LIMIT, &var_local_rcpt_lim, 0, 0,
	VAR_LOCAL_CON_LIMIT, DEF_LOCAL_CON_LIMIT, &var_local_con_lim, 0, 0,
	VAR_PROC_LIMIT, DEF_PROC_LIMIT, &var_proc_limit, 1, 0,
	/* Rmail start */
	VAR_RMAIL_MTADB_PORT, DEF_RMAIL_MTADB_PORT, &var_rmail_mtadb_port, 1, 0,
	VAR_RMAIL_CACDB_PORT, DEF_RMAIL_CACDB_PORT, &var_rmail_cacdb_port, 1, 0,
	VAR_RMAIL_MAILCACHE_EXPIRE, DEF_RMAIL_MAILCACHE_EXPIRE, &var_rmail_mailcache_expire, 1, 0, 

	VAR_RMAIL_HOSTMAPCACHE_EXPIRE, DEF_RMAIL_HOSTMAPCACHE_EXPIRE, &var_rmail_hostmapcache_expire, 1, 0,
	VAR_RMAIL_DOMAINCACHE_EXPIRE, DEF_RMAIL_DOMAINCACHE_EXPIRE, &var_rmail_domaincache_expire, 1, 0,
	
	VAR_RMAIL_LICENSE_USERLIMIT, DEF_RMAIL_LICENSE_USERLIMIT, &var_rmail_license_userlimit, 1, 0, 
	VAR_RMAIL_LICENSE_DOMAINLIMIT, DEF_RMAIL_LICENSE_DOMAINLIMIT, &var_rmail_license_domainlimit, 1, 0,
	/* Rmail end */
	0,
    };
    static CONFIG_BOOL_TABLE bool_table[] = {
	VAR_ALLOW_MIN_USER, DEF_ALLOW_MIN_USER, &var_allow_min_user,
	VAR_VERP_BOUNCE_OFF, DEF_VERP_BOUNCE_OFF, &var_verp_bounce_off,
	VAR_SENDER_ROUTING, DEF_SENDER_ROUTING, &var_sender_routing,
	/* Rmail start */
	VAR_RMAIL_MTADB_INTERACTIVE, DEF_RMAIL_MTADB_INTERACTIVE, &var_rmail_mtadb_interactive,
	VAR_RMAIL_CACDB_INTERACTIVE, DEF_RMAIL_CACDB_INTERACTIVE, &var_rmail_cacdb_interactive,
	VAR_RMAIL_MAILCACHE_ENABLE, DEF_RMAIL_MAILCACHE_ENABLE, &var_rmail_mailcache_enable,
	VAR_RMAIL_ALLOW_LOCAL, DEF_RMAIL_ALLOW_LOCAL, &var_rmail_allow_local,
	VAR_RMAIL_ISMX, DEF_RMAIL_ISMX, &var_rmail_ismx,
	VAR_RMAIL_DROPDOMAIN_ENABLE, DEF_RMAIL_DROPDOMAIN_ENABLE, &var_rmail_dropdomain_enable,
	VAR_RMAIL_RETURNMAIL_ENABLE, DEF_RMAIL_RETURNMAIL_ENABLE, &var_rmail_returnmail_enable,

	VAR_RMAIL_HOSTMAPCACHE_ENABLE, DEF_RMAIL_HOSTMAPCACHE_ENABLE, &var_rmail_hostmapcache_enable,
	VAR_RMAIL_DOMAINCACHE_ENABLE, DEF_RMAIL_DOMAINCACHE_ENABLE, &var_rmail_domaincache_enable,
	
	/* Rmail end */
	0,
    };

    /*
     * Use the trigger service skeleton, because no-one else should be
     * monitoring our service port while this process runs, and because we do
     * not talk back to the client.
     */
    trigger_server_main(argc, argv, qmgr_trigger_event,
			MAIL_SERVER_INT_TABLE, int_table,
			MAIL_SERVER_STR_TABLE, str_table,
			MAIL_SERVER_BOOL_TABLE, bool_table,
			MAIL_SERVER_TIME_TABLE, time_table,
			MAIL_SERVER_PRE_INIT, qmgr_pre_init,
			MAIL_SERVER_POST_INIT, qmgr_post_init,
			MAIL_SERVER_LOOP, qmgr_loop,
			MAIL_SERVER_PRE_ACCEPT, pre_accept,
			MAIL_SERVER_SOLITARY,
			0);
}
