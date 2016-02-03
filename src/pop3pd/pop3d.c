#include <sys_defs.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>                      /* remove() */
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#ifdef STRCASECMP_IN_STRINGS_H
#include <strings.h>
#endif

/* Utility library. */

#include <msg.h>
#include <mymalloc.h>
#include <vstring.h>
#include <vstream.h>
#include <vstring_vstream.h>
#include <stringops.h>
#include <events.h>
#include <pop3_stream.h>
#include <valid_hostname.h>
#include <dict.h>
#include <watchdog.h>

/* Global library. */

#include <mail_params.h>
#include <record.h>
#include <rec_type.h>
#include <mail_proto.h>
#include <cleanup_user.h>
#include <mail_date.h>
#include <mail_conf.h>
#include <off_cvt.h>
#include <debug_peer.h>
#include <mail_error.h>
#include <flush_clnt.h>
#include <mail_stream.h>
#include <mail_queue.h>
#include <tok822.h>
#include <verp_sender.h>
#include <string_list.h>
#include <quote_822_local.h>
#include <lex_822.h>
#include <namadr_list.h>

#include <mail_server.h>
/* for opendir */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>

/* for zlib */
#include <zlib.h>

/* for MD5 */
#include <md5.h>

#include <pwd.h>
#include <sys/types.h>

/* Application-specific */

#include "pop3d.h"
#include "pop3d_chat.h"
#include "pop3d_token.h"

int var_rmail_pop3d_tmout;
int var_pop3d_soft_erlim;
int var_pop3d_err_sleep;

int var_rmail_mtadb_port;
int var_rmail_autdb_port;
int var_rmail_recdb_port;
int var_rmail_cacdb_port;
int var_pop3d_soft_erlim;
int var_rmail_mailcache_expire;
int var_rmail_pop3pd_targetport;

bool  var_rmail_mtadb_interactive;
bool  var_rmail_autdb_interactive;
bool  var_rmail_recdb_interactive;
bool  var_rmail_cacdb_interactive;
bool  var_rmail_maillog_enable;
bool  var_rmail_mailrecord_enable;
bool  var_rmail_rcpop3_enable;
bool  var_rmail_mailcache_enable;
bool  var_rmail_allow_local;
bool  var_rmail_pop3pd_enable;

char *var_rmail_mtadb_host;
char *var_rmail_mtadb_user;
char *var_rmail_mtadb_pass;
char *var_rmail_mtadb_name;

char *var_rmail_recdb_host;
char *var_rmail_recdb_user;
char *var_rmail_recdb_pass;
char *var_rmail_recdb_name;

char *var_rmail_autdb_host;
char *var_rmail_autdb_user;
char *var_rmail_autdb_pass;
char *var_rmail_autdb_name;

char *var_rmail_cacdb_host;
char *var_rmail_cacdb_user;
char *var_rmail_cacdb_pass;
char *var_rmail_cacdb_name;

char *var_rmail_transport_table;
char *var_rmail_transport_idxfield;
char *var_rmail_transport_domainfield;
char *var_rmail_transport_basedirfield;

char *var_rmail_mailuser_table;
char *var_rmail_mailuser_domainfield;
char *var_rmail_mailuser_mailidfield;
char *var_rmail_mailuser_mhostfield;
char *var_rmail_mailuser_mboxfield;
char *var_rmail_mailuser_statufield;

char *var_rmail_maillog_table;
char *var_rmail_maillog_domainfield;
char *var_rmail_maillog_mailidfield;
char *var_rmail_maillog_ipfield;
char *var_rmail_maillog_typefield;
char *var_rmail_maillog_timefield;

char *var_rmail_mailcache_table;
char *var_rmail_mailcache_table;
char *var_rmail_mailcache_domainfield;
char *var_rmail_mailcache_mailidfield;
char *var_rmail_mailcache_mhostfield;
char *var_rmail_mailcache_mboxfield;
char *var_rmail_mailcache_statufield;
char *var_rmail_mailcache_timefield;

char *var_rmail_mailrecord_table;
char *var_rmail_mailrecord_domainfield;
char *var_rmail_mailrecord_mailidfield;
char *var_rmail_mailrecord_smtpipfield;
char *var_rmail_mailrecord_pop3ipfield;
char *var_rmail_mailrecord_smtptimefield;
char *var_rmail_mailrecord_pop3timefield;

char *var_rmail_auth_table;
char *var_rmail_auth_mailidfield;
char *var_rmail_auth_domainfield;
char *var_rmail_auth_encfield;

char *var_rmail_maildir_sufix;
char *var_rmail_quota_tmpfile;

char *var_rmail_local_nexthop;

/* extern variables */
int used_count;
MYSQL mta_dbh;
MYSQL rec_dbh;
MYSQL aut_dbh;
MYSQL cac_dbh;

static ssize_t readline_n(int fd, void *vptr, size_t maxlen) {
  ssize_t n, rc;
  char c, *ptr;

  ptr = vptr;
  for (n=1; n< maxlen; n++) {
    again:
      if ( (rc = read(fd, &c, 1)) ==1 ) {
	if (c == '\r') {
	  continue;
	}
	if (c == '\n') {
	  break;
	}
	*ptr++=c;
      } else if (rc == 0) {
	if (n == 1)
	  return 0;
	else
	  break;
      } else {
	if (errno == EINTR)
	  goto again;
	return(-1);
      }
  }
  *ptr=0;
  
  return n;
}

static int user_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname="user_cmd";
    int i;

    if (state->authed) {
      pop3d_chat_reply(state, "-ERR You have been authed, are you really %s?",
	  state->userid);
      return 1;
    }

    if (argc == 1) {
      pop3d_chat_reply(state, "-ERR You need to tell me who you are, hmmm...");
      return 1;
    }

    if (state->userid != 0) {
      pop3d_chat_reply(state, "-ERR You've told me who you are");
      return 1;
    }

    for (i=0; i<strlen(argv[1].strval); i++) {
      argv[1].strval[i]=tolower(argv[1].strval[i]);
    }

    state->userid = strdup(argv[1].strval);
    pop3d_chat_reply(state, "+OK Welcome %s! Please show me your password", state->userid);
    return 0;
}

static int pass_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname="pass_cmd";
    char sql[512];
    MYSQL_RES *res;
    MYSQL_ROW row;
    DIR *dirp;
    struct dirent *dire, *dp;
    struct stat fptr;
    struct passwd *pw;
    POP3D_MSG *curmsg, *thismsg;
    char basedir[128], mhost[128], mdir[128];
    int mailcache_hitted=0;
    char cmdbuf[512];
    char revbuf[512];

    struct sockaddr_in servaddr;
    struct hostent *he;
    

    if (state->authed==1) {
      pop3d_chat_reply(state, "-ERR You have been authed, are you really %s?",
	  state->userid);
      return 1;
    }
    
    if (state->userid==0) {
      pop3d_chat_reply(state, "-ERR Please tell me who you are first.");
      return 1;
    }

    if (state->passwd!=0) {
      pop3d_chat_reply(state, "-ERR You've give me your password");
      return 1;
    }

    state->passwd=strdup(argv[1].strval);


// not unix account start
    pw = getpwnam(state->userid);

    if (pw && var_rmail_allow_local) {
      //local account
      if (var_rmail_allow_local) {
	if (strcmp(pw->pw_passwd, crypt(state->passwd, pw->pw_passwd))==0) {
	  // auth ok
	  strcpy(mhost, var_rmail_local_nexthop);
	} else {
	  // auth fail;
	  state->userid=0;
	  state->passwd=0;
	  pop3d_chat_reply(state, "-ERR Auth fail");
	  return 1;
	}
      } else {
	state->userid=0;
	state->passwd=0;
	pop3d_chat_reply(state, "-ERR Auth fail");
	return 1;
      }
    } else {
      // Do Auth
      sprintf(sql, "SELECT %s FROM %s WHERE %s='%s' AND %s=ENCRYPT('%s', LEFT(%s, 2))",
	  var_rmail_auth_domainfield, var_rmail_auth_table,
	  var_rmail_auth_mailidfield, state->userid,
	  var_rmail_auth_encfield, state->passwd, var_rmail_auth_encfield); 

      if (msg_verbose)
	msg_info("%s: SQL => %s", myname, sql);
    
      if (mysql_real_query(&aut_dbh, sql, strlen(sql))!=0) {
	msg_warn("%s: Query fail(%s)", myname, mysql_error(&aut_dbh));
	pop3d_chat_reply(state, "-ERR Auth fail");
	state->userid=0;
	state->passwd=0;
	return 1;
      }

      res = mysql_store_result(&aut_dbh);
      if (mysql_num_rows(res)==0) {
	pop3d_chat_reply(state, "-ERR Auth fail");
	state->userid=0;
	state->passwd=0;
	mysql_free_result(res);
	return 1;
      } else if (mysql_num_rows(res)!=1) {
	pop3d_chat_reply(state, "-ERR Auth fail: multi-rows");
	state->userid=0;
	state->passwd=0;
	mysql_free_result(res);
	return 1;
      }
      row = mysql_fetch_row(res);
      mysql_free_result(res);
      state->domain = (row[0])? atoi(row[0]) : 0;

      if (state->domain == 0) {
	pop3d_chat_reply(state, "-ERR Unknown error(no domain)");
	state->userid=0;
	state->passwd=0;
	return 1;
      }

      // get mdir & mhost
      if (var_rmail_mailcache_enable) {
	sprintf(sql, "SELECT %s, %s, UNIX_TIMESTAMP(%s) FROM %s WHERE %s='%s' AND %s=%d",
	    var_rmail_mailcache_mhostfield, var_rmail_mailcache_mboxfield, 
	    var_rmail_mailcache_timefield, var_rmail_mailcache_table,
	    var_rmail_mailcache_mailidfield, state->userid,
	    var_rmail_mailcache_domainfield, state->domain);

	if (msg_verbose)
	  msg_info("%s: SQL => %s", myname, sql);

	if (mysql_real_query(&cac_dbh, sql, strlen(sql))==0) {
	  res = mysql_store_result(&cac_dbh);
	  if (mysql_num_rows(res)==1) {
	    //cache hitted, check expire
	    row = mysql_fetch_row(res);
	    mysql_free_result(res);
	    if ((int) time(NULL) - (int) atoi(row[1]) < var_rmail_mailcache_expire) {
	      // not expire, use it directly
	      strcpy(mhost, row[0]);
	      strcpy(mdir, row[1]);
	      if (msg_verbose)
		msg_info("%s: Got cache data %s", myname, mhost);
	      mailcache_hitted=1;
	    } else {
	      // expire
	      mailcache_hitted =0;
	      sprintf(sql, "DELETE FROM %s WHERE %s='%s' AND %s=%d",
		  var_rmail_mailcache_table, var_rmail_mailcache_mailidfield,
		  state->userid, var_rmail_mailcache_domainfield, state->domain);
	      if (msg_verbose)
		msg_info("%s: SQL => %s", myname, sql);

	      if (mysql_real_query(&cac_dbh, sql, strlen(sql))!=0) {
		msg_warn("%s: Query fail: %s", myname, mysql_error(&cac_dbh));
	      }
	    }
	  } else {
	    mailcache_hitted=0;
	  }
	} else {
	  msg_warn("%s: Query fail: %s", myname, mysql_error(&cac_dbh));
	}
      }


      if (!mailcache_hitted) {
	sprintf(sql, "SELECT %s, %s, %s FROM %s WHERE %s='%s' AND %s=%d",
	    var_rmail_mailuser_mhostfield, var_rmail_mailuser_mboxfield,
	    var_rmail_mailuser_statufield, var_rmail_mailuser_table,
	    var_rmail_mailuser_mailidfield, state->userid,
	    var_rmail_mailuser_domainfield, state->domain);
	if (msg_verbose)
	  msg_info("%s: SQL => %s", myname, sql);

	if (mysql_real_query(&mta_dbh, sql, strlen(sql))!=0) {
	  msg_warn("%s: Query fail(%s)", myname, mysql_error(&mta_dbh));
	  pop3d_chat_reply(state, "-ERR Auth fail(Query fail)");
	  state->userid=0;
	  state->passwd=0;
	  state->domain=0;
	  return 1;
	}

	res = mysql_store_result(&mta_dbh);
	if (mysql_num_rows(res)==0) {
	  msg_warn("%s: No such user by exist in queue(%s)", myname, state->userid);
	  pop3d_chat_reply(state, "-ERR Auth fail");
	  state->userid=0;
	  state->passwd=0;
	  state->domain=0;
	  mysql_free_result(res);
	  return 1;
	} else if (mysql_num_rows(res)!=1) {
	  msg_warn("%s: Double record in %s (%s)", myname, var_rmail_mailuser_table, state->userid);
	  pop3d_chat_reply(state, "-ERR Auth fail");
	  state->userid=0;
	  state->passwd=0;
	  state->domain=0;
	  mysql_free_result(res);
	  return 1;
	} else {
	  row = mysql_fetch_row(res);
	  mysql_free_result(res);
	  strcpy(mhost, row[0]);
	  strcpy(mdir, row[1]);
	}
      
	if (!var_rmail_mailcache_enable) {
	  sprintf(sql, "INSERT INTO %s SET %s='%s', %s=%d, %s='%s', %s='%s', %s=%d, %s=FROM_UNIXTIME(%d)",
	      var_rmail_mailcache_table,
	      var_rmail_mailcache_mailidfield, state->userid,
	      var_rmail_mailcache_domainfield, state->domain,
	      var_rmail_mailcache_mhostfield, row[0],
	      var_rmail_mailcache_mboxfield, row[1],
	      var_rmail_mailcache_statufield, atoi(row[2]),
	      var_rmail_mailcache_timefield, (int) time(NULL));
	  if (msg_verbose)
	    msg_info("%s: SQL => %s", myname, sql);
	  if (mysql_real_query(&cac_dbh, sql, strlen(sql))!=0) {
	    msg_warn("%s: Query fail(%s)", myname, mysql_error(&cac_dbh));
	  }
	}
      }
    
    }
    /* Create socket to mhost & send addr_rewrite, user, pass */
    state->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(var_rmail_pop3pd_targetport);
    he = gethostbyname(mhost);

    msg_info("%s: Get %s ==> %s", myname, mhost, inet_ntoa(*(struct in_addr *)he->h_addr));

    inet_pton(AF_INET, inet_ntoa(*(struct in_addr *)he->h_addr), &servaddr.sin_addr);
    connect(state->sockfd, &servaddr, sizeof(servaddr));
    // read banner
    readline_n(state->sockfd, revbuf, sizeof(revbuf));
    
    // rewrite addr first
    sprintf(cmdbuf, "ADDR %s %s\n", state->addr, state->name);
    write(state->sockfd, cmdbuf, strlen(cmdbuf));
    readline_n(state->sockfd, revbuf, sizeof(revbuf));

    // send user
    sprintf(cmdbuf, "USER %s\n", state->userid);
    write(state->sockfd, cmdbuf, strlen(cmdbuf));
    readline_n(state->sockfd, revbuf, sizeof(revbuf));

    // send pass
    sprintf(cmdbuf, "PASS %s\n", state->passwd);
    write(state->sockfd, cmdbuf, strlen(cmdbuf));
    readline_n(state->sockfd, revbuf, sizeof(revbuf));
    
    pop3d_chat_reply(state, "+OK You've been authed, %s", state->userid);
    state->authed=1;
    return 0;
}

static int quit_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname="quit_cmd";
    char cmdbuf[512], revbuf[512];

    if (state->sockfd && state->authed!=0) {
      sprintf(cmdbuf, "QUIT\n");
      write(state->sockfd, cmdbuf, strlen(cmdbuf));
      readline_n(state->sockfd, revbuf, sizeof(revbuf));
      pop3d_chat_reply(state, "%s", revbuf);
      close(state->sockfd);
    } else {
      pop3d_chat_reply(state, "+OK bye bye");
    }
    return(0);
}

static int stat_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "stat_cmd";
    char cmdbuf[512], revbuf[512];

    if (state->authed==0) {
      pop3d_chat_reply(state, "-ERR Please, auth first");
      return 1;
    }

    if (argc!=1) {
      pop3d_chat_reply(state, "-ERR bad syntax");
      return 1;
    }

    sprintf(cmdbuf, "STAT\n");
    write(state->sockfd,  cmdbuf, strlen(cmdbuf));

    readline_n(state->sockfd, revbuf, sizeof(revbuf));
    pop3d_chat_reply(state, "%s", revbuf);
    
    return 0;
}

static int list_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "list_cmd";
    char cmdbuf[512], revbuf[512];

    if (state->authed==0) {
      pop3d_chat_reply(state, "-ERR Please, auth first");
      return 1;
    }

    if (argc!=1) {
      sprintf(cmdbuf, "LIST %s\n", argv[1].strval);
      write(state->sockfd, cmdbuf, strlen(cmdbuf));
      readline_n(state->sockfd, revbuf, sizeof(revbuf));
      pop3d_chat_reply(state, "%s", revbuf);
    } else {
      sprintf(cmdbuf, "LIST\n");
      write(state->sockfd, cmdbuf, strlen(cmdbuf));
      while (1) {
	if (readline_n(state->sockfd, revbuf, sizeof(revbuf))>1)
	  pop3d_chat_reply(state, "%s", revbuf);
	if (strcmp(revbuf, ".")==0)
	  break;
      }
    }
    return 0;
}

static int retr_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "retr_cmd";
    char cmdbuf[512], revbuf[512];
    
    if (state->authed==0) {
      pop3d_chat_reply(state, "-ERR Please, auth first");
      return 1;
    }

    if (argc!=2) {
      pop3d_chat_reply(state, "-ERR bad syntax");
      return 1;
    }

    sprintf(cmdbuf, "RETR %s\n", argv[1].strval);
    write(state->sockfd, cmdbuf, strlen(cmdbuf));
    while (1) {
      if (readline_n(state->sockfd, revbuf, sizeof(revbuf))>1)
	pop3d_chat_reply(state, "%s", revbuf);
      if (strcmp(revbuf, ".")==0)
	break;
    }
    
    return 1;
    
}

static int dele_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "dele_cmd";
    char cmdbuf[512], revbuf[512];

    if (state->authed==0) {
      pop3d_chat_reply(state, "-ERR Please, auth first");
      return 1;
    }

    if (argc!=2) {
      pop3d_chat_reply(state, "-ERR bad syntax");
      return 1;
    }

    sprintf(cmdbuf, "DELE %s\n", argv[1].strval);
    write(state->sockfd, cmdbuf, strlen(cmdbuf));
    readline_n(state->sockfd, revbuf, sizeof(revbuf));
    pop3d_chat_reply(state, "%s", revbuf);
    return 1;
}

static int noop_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "noop_cmd";
    pop3d_chat_reply(state, "+OK But don't let me wait for so long");
    return 0;
}

static int rset_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "rset_cmd";
    if (state->authed==0) {
      pop3d_chat_reply(state, "-ERR Please, auth first");
      return 1;
    }
    pop3d_chat_reply(state, "+OK Bye bye flags");
    return 0;
}

static int apop_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "apop_cmd";
    pop3d_chat_reply(state, "-ERR Not support APOP now, maybe later");
    return 0;
}

static int top_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "top_cmd";
    char cmdbuf[512], revbuf[512];

    if (state->authed==0) {
      pop3d_chat_reply(state, "-ERR Please, auth first");
      return 1;
    }

    if (argc!=3) {
      pop3d_chat_reply(state, "-ERR bad syntax");
      return 1;
    }

    sprintf(cmdbuf, "TOP %s %s\n", argv[1].strval, argv[2].strval);
    write(state->sockfd, cmdbuf, strlen(cmdbuf));
    while (1) {
      if (readline_n(state->sockfd, revbuf, sizeof(revbuf))>1)
	pop3d_chat_reply(state, "%s", revbuf);
      if (strcmp(revbuf, ".")==0)
	break;
    }
    
    return 1;
}

static int uidl_cmd(POP3D_STATE *state, int argc, POP3D_TOKEN *argv) {
    const char *myname = "uidl_cmd";
    char cmdbuf[512], revbuf[512];

    if (state->authed==0) {
      pop3d_chat_reply(state, "-ERR Please, auth first");
      return 1;
    }

    if (argc!=1) {
      sprintf(cmdbuf, "UIDL %s\n", argv[1].strval);
      write(state->sockfd, cmdbuf, strlen(cmdbuf));
      readline_n(state->sockfd, revbuf, sizeof(revbuf));
      pop3d_chat_reply(state, "%s", revbuf);
    } else {
      sprintf(cmdbuf, "UIDL\n");
      write(state->sockfd, cmdbuf, strlen(cmdbuf));
      while (1) {
	if (readline_n(state->sockfd, revbuf, sizeof(revbuf))>1)
	  pop3d_chat_reply(state, "%s", revbuf);
	if (strcmp(revbuf, ".")==0)
	  break;
      }
    }
    return 1;
}

typedef struct POP3D_CMD {
    char *name;
    int (*action) (POP3D_STATE *, int, POP3D_TOKEN *);
    int flags;
} POP3D_CMD;

#define POP3D_CMD_FLAG_LIMIT	    (1<<0)  /* limit usage */

static POP3D_CMD pop3d_cmd_table[] = {
    "USER", user_cmd, 0,
    "PASS", pass_cmd, 0,
    "QUIT", quit_cmd, 0,
    "STAT", stat_cmd, POP3D_CMD_FLAG_LIMIT,
    "LIST", list_cmd, POP3D_CMD_FLAG_LIMIT,
    "RETR", retr_cmd, POP3D_CMD_FLAG_LIMIT,
    "DELE", dele_cmd, POP3D_CMD_FLAG_LIMIT,
    "NOOP", noop_cmd, POP3D_CMD_FLAG_LIMIT,
    "RSET", rset_cmd, POP3D_CMD_FLAG_LIMIT,
    "APOP", apop_cmd, 0,
    "TOP", top_cmd, POP3D_CMD_FLAG_LIMIT,
    "UIDL", uidl_cmd, POP3D_CMD_FLAG_LIMIT,
    0,
};


static void pop3d_proto(POP3D_STATE *state) {
    const char *myname="pop3d_proto";
    static time_t var_now;
    int argc;
    POP3D_CMD *cmdp;
    POP3D_TOKEN *argv;
    
    time(&var_now);
    
    pop3_timeout_setup(state->client, var_rmail_pop3d_tmout);
    
    switch (vstream_setjmp(state->client)) {
	default:
	    msg_panic("%s: unknown error reading from %s[%s]",
		myname, state->name, state->addr);
	    break;
	
	case POP3_ERR_TIME:
	    state->reason = "timeout";
	    pop3d_chat_reply(state, "-ERR timeout exceeded");
	    break;
	
	case POP3_ERR_EOF:
	    state->reason = "lost connection";
	    break;
	
	case 0:

	    pop3d_chat_reply(state, "+OK Welcome to %s POP3 Service <%d-%d-%d@%s>",
		var_myhostname, (int) var_now, used_count, (int) mysql_thread_id(&mta_dbh),
		var_myhostname);

	    for (;;) {
		watchdog_pat();
		pop3d_chat_query(state);

		if ((argc = pop3d_token(vstring_str(state->buffer), &argv)) == 0 ) {
		    pop3d_chat_reply(state, "-ERR bad syntax");
		    state->error_count++;
		    continue;
		}

		

		for (cmdp = pop3d_cmd_table; cmdp->name !=0; cmdp++) 
		    if (strcasecmp(argv[0].strval, cmdp->name) ==0 )
			break;

		if (cmdp->name == 0) {
		    pop3d_chat_reply(state, "-ERR %s? I don't understand, Sir!", argv[0].strval);
		    continue;
		}

		state->where = cmdp->name;
		if (cmdp->action(state, argc, argv)!=0)
		    state->error_count++;

		if ((cmdp->flags & POP3D_CMD_FLAG_LIMIT))
		    state->error_count++;
		if (cmdp->action == quit_cmd)
		    break;
	    }
	    break;
    }

    if (state->reason && state->where
        && (strcmp(state->where, POP3D_AFTER_DOT)
            || strcmp(state->reason, "lost connection")))
        msg_info("%s after %s from %s[%s]",
                 state->reason, state->where, state->name, state->addr);

}


static void pop3d_service(VSTREAM *stream, char *unused_service, char **argv) {
    const char *myname = "pop3d_service";
    POP3D_STATE state;

    if (mysql_ping(&mta_dbh) != 0) {
      msg_warn("%s: MTA database(%s) had gone(%s), attempted to reconnect",
	  myname, var_rmail_mtadb_host, mysql_error(&mta_dbh));
      if (msg_verbose)
	msg_info("%s: MTA database(%s) reconnectted, id %lu",
	    myname, var_rmail_mtadb_host, mysql_thread_id(&mta_dbh));
    } else if (msg_verbose)
      msg_info("%s: MTA database(%s) still alive", myname, var_rmail_mtadb_host);

    if (mysql_ping(&aut_dbh) != 0) {
      msg_warn("%s: AUT database(%s) had gone(%s), attempted to reconnect",
	  myname, var_rmail_autdb_host, mysql_error(&aut_dbh));
      if (msg_verbose)
	msg_info("%s: AUT database(%s) reconnectted, id %lu",
	    myname, var_rmail_autdb_host, mysql_thread_id(&aut_dbh));
    } else if (msg_verbose)
      msg_info("%s: AUT database(%s) still alive", myname, var_rmail_autdb_host);

    if (var_rmail_maillog_enable || var_rmail_mailrecord_enable) {
      if (mysql_ping(&rec_dbh) != 0) {
	msg_warn("%s: REC database(%s) had gone(%s), attempted to reconnect",
	    myname, var_rmail_recdb_host, mysql_error(&rec_dbh));
	if (msg_verbose)
	  msg_info("%s: REC database(%s) reconnectted, id %lu",
	      myname, var_rmail_recdb_host, mysql_thread_id(&rec_dbh));
      } else if (msg_verbose)
	msg_info("%s: REC database(%s) still alive", myname, var_rmail_recdb_host);
    }    

    if (var_rmail_mailcache_enable) {
      if (mysql_ping(&cac_dbh) != 0) {
	msg_warn("%s: CAC database(%s) had gone(%s), attempted to reconnect",
	    myname, var_rmail_cacdb_host, mysql_error(&cac_dbh));
	if (msg_verbose)
	  msg_info("%s: CAC database(%s) reconnectted, id %lu",
	      myname, var_rmail_cacdb_host, mysql_thread_id(&cac_dbh));
      } else if (msg_verbose)
	msg_info("%s: CAC database(%s) still alive", myname, var_rmail_cacdb_host);
    }
    



    
    
    used_count++;
    if (argv[0])
	msg_fatal("unexpected command-line argument: %s", argv[0]);

    pop3d_state_init(&state, stream);
    msg_info("connect from %s[%s]", state.name, state.addr);

    

    pop3d_proto(&state);



    
    if (state.domain)
	msg_info("disconnect from %s[%s] %s:%d:%d:%ld:%s",
	    state.name, state.addr, state.userid, state.domain, state.retr_count, state.retr_size, state.addr);
    pop3d_state_reset(&state);

}



static void pre_accept(char *unused_name, char **unused_argv) {
    const char *myname = "pre_accept";

    if (msg_verbose)
      msg_info("%s: pre_accept", myname);

}

static void pre_jail_init(char *unused_name, char **unused_argv) {
    const char *myname = "pre_jail_init";

    /* some config overwrite */
    if (!var_rmail_mailrecord_enable) {
      if (var_rmail_rcpop3_enable) {
	msg_warn("%s: Record disable, but RC pop3 enable, overwrite Record setting", myname);
	var_rmail_mailrecord_enable=1;
      }
    }

    mysql_init(&mta_dbh);
    if (mysql_real_connect(&mta_dbh, var_rmail_mtadb_host, var_rmail_mtadb_user,
	  var_rmail_mtadb_pass, var_rmail_mtadb_name, (unsigned int) var_rmail_mtadb_port,
	  NULL, var_rmail_mtadb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
      /* Connection fail */
      msg_fatal("%s: MTA database(%s) connection fail: %s",
	  myname, var_rmail_mtadb_host, mysql_error(&mta_dbh));
    } else if (msg_verbose) {
      msg_info("%s: MTA database(%s) connectted, id %lu",
	  myname, var_rmail_mtadb_host, mysql_thread_id(&mta_dbh));
    }

    mysql_init(&aut_dbh);
    if (mysql_real_connect(&aut_dbh, var_rmail_autdb_host, var_rmail_autdb_user,
	  var_rmail_autdb_pass, var_rmail_autdb_name, (unsigned int) var_rmail_autdb_port,
	  NULL, var_rmail_autdb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
      msg_fatal("%s: AUT database(%s) connection fail: %s",
	  myname, var_rmail_autdb_host, mysql_error(&aut_dbh));
    } else if (msg_verbose) {
      msg_info("%s: AUT database(%s) connectted, id %lu",
	  myname, var_rmail_autdb_host, mysql_thread_id(&aut_dbh));
    }

    if (var_rmail_maillog_enable || var_rmail_mailrecord_enable) {
      mysql_init(&rec_dbh);
      if (mysql_real_connect(&rec_dbh, var_rmail_recdb_host, var_rmail_recdb_user,
	    var_rmail_recdb_pass, var_rmail_recdb_name, (unsigned int) var_rmail_recdb_port,
	    NULL, var_rmail_recdb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
	msg_fatal("%s: REC database(%s) connection fail: %s",
	    myname, var_rmail_recdb_host, mysql_error(&rec_dbh));
      } else if (msg_verbose) {
	msg_info("%s: REC database(%s) connectted, id %lu",
	    myname, var_rmail_recdb_host, mysql_thread_id(&rec_dbh));
      }
    }

    if (var_rmail_mailcache_enable) {
      mysql_init(&cac_dbh);
      if (mysql_real_connect(&cac_dbh, var_rmail_cacdb_host, var_rmail_cacdb_user,
	    var_rmail_cacdb_pass, var_rmail_cacdb_name, (unsigned int) var_rmail_cacdb_port,
	    NULL, var_rmail_cacdb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
	msg_fatal("%s: CAC database(%s) connection fail: %s",
	    myname, var_rmail_cacdb_host, mysql_error(&cac_dbh));
      } else if (msg_verbose) {
	msg_info("%s: CAC database(%s) connectted, id %lu",
	    myname, var_rmail_cacdb_host, mysql_thread_id(&cac_dbh));
      }
    }

    used_count=0;
    /* Database connection */
    if (msg_verbose)
      msg_info("%s: pre_jail_init", myname);

}

static int parse_mdir(POP3D_STATE *state) {
  const char *myname = "parse_mdir";
  char *newdir;
  DIR *dirp, *dp;
  struct dirent *dire;
  struct stat fptr;
  POP3D_MSG *curmsg, *thismsg;
  unsigned long fts, fpid;
  char flag[9], fdr[3], opop[3], hn[128];
  char buf[1024];
  char *gzinfo;
  char *filename;

  if (msg_verbose)
    msg_info("%s: Start to parse %s", myname, state->mdir);
  
  // chdir to mdir first
  if (chdir(state->mdir)==-1) {
    msg_warn("%s: cannot chdir to %s", myname, state->mdir);
    return 1;
  }

  newdir = concatenate("new/", (char *) 0);
  
  if ((dirp = opendir(newdir))==NULL) {
    msg_warn("%s: cannot opendir %s/%s", myname, state->mdir, newdir);
    return 1;
  }

  while (dire = readdir(dirp)) {
    // skip . & ..
    if (strcmp(dire->d_name, ".")==0 || strcmp(dire->d_name, "..")==0)
      continue;

    filename =concatenate(newdir, dire->d_name, (char *) 0);

    // skip any directory
    if (dp=opendir(filename)) {
      closedir(dp);
      continue;
    }

    // init msg
    curmsg = (POP3D_MSG *) malloc (sizeof(POP3D_MSG));
    strcpy(curmsg->file, filename);
    curmsg->flags = 0;

    // check if gzipped
    if (dire->d_name[strlen(dire->d_name)-2]=='g' &&
	dire->d_name[strlen(dire->d_name)-1]=='z')
      curmsg->gzipped = 1;
    else
      curmsg->gzipped = 0;


    // get flags 
    sscanf(dire->d_name, "%lu.%lu.%8s.%2s.%2s.%s",
	&fts, &fpid, flag, fdr, opop, hn);
    if (flag[0] != '0')
      curmsg->readed = 1;
    else
      curmsg->readed = 0;

    sprintf(curmsg->uidl, "%lu.%lu", fts, fpid);
    strcpy(curmsg->f_flags, flag);
    strcpy(curmsg->f_folder, fdr);
    strcpy(curmsg->f_opop, opop);

    // get size
    if (curmsg->gzipped) {
      // gzipped file, get size from gzinfo
      gzinfo = strrchr(dire->d_name, '.');
      gzinfo++;

      gzinfo[strlen(gzinfo)-2]='\0';
      curmsg->size=atoi(gzinfo);
    } else {
      // non-gzipped, use stat to get size
      stat(filename, &fptr);
      curmsg->size= fptr.st_size;
    }


    if (msg_verbose)
      msg_info("%s: File => %s, Size= %lu, Flag => %s, Folder => %s, Opop => %s",
	  myname, curmsg->file, curmsg->size, curmsg->f_flags, curmsg->f_folder,
	  curmsg->f_opop);
    
    // follow link-list
    if (state->firstmsg == NULL)
      state->firstmsg = curmsg;
    else
      thismsg->nextmsg = curmsg;

    thismsg = curmsg;
    thismsg->nextmsg=NULL;
  }
  closedir(dirp);
  
  

  return 0;
}

int main(int argc, char **argv) {
    static CONFIG_INT_TABLE int_table[] = {
	VAR_RMAIL_MTADB_PORT, DEF_RMAIL_MTADB_PORT, &var_rmail_mtadb_port, 1, 0,
	VAR_RMAIL_AUTDB_PORT, DEF_RMAIL_AUTDB_PORT, &var_rmail_autdb_port, 1, 0,
	VAR_RMAIL_RECDB_PORT, DEF_RMAIL_RECDB_PORT, &var_rmail_recdb_port, 1, 0,
	VAR_RMAIL_CACDB_PORT, DEF_RMAIL_CACDB_PORT, &var_rmail_cacdb_port, 1, 0,
	VAR_POP3D_SOFT_ERLIM, DEF_POP3D_SOFT_ERLIM, &var_pop3d_soft_erlim, 1, 0,
	VAR_RMAIL_MAILCACHE_EXPIRE, DEF_RMAIL_MAILCACHE_EXPIRE, &var_rmail_mailcache_expire, 1, 0,
	VAR_RMAIL_POP3PD_TARGETPORT, DEF_RMAIL_POP3PD_TARGETPORT, &var_rmail_pop3pd_targetport, 1, 0,
	0,
    };

    static CONFIG_BOOL_TABLE bool_table[] = {
	VAR_RMAIL_MTADB_INTERACTIVE, DEF_RMAIL_MTADB_INTERACTIVE, &var_rmail_mtadb_interactive,
	VAR_RMAIL_RECDB_INTERACTIVE, DEF_RMAIL_RECDB_INTERACTIVE, &var_rmail_recdb_interactive,
	VAR_RMAIL_AUTDB_INTERACTIVE, DEF_RMAIL_AUTDB_INTERACTIVE, &var_rmail_autdb_interactive,
	VAR_RMAIL_CACDB_INTERACTIVE, DEF_RMAIL_CACDB_INTERACTIVE, &var_rmail_cacdb_interactive,
	VAR_RMAIL_MAILLOG_ENABLE, DEF_RMAIL_MAILLOG_ENABLE, &var_rmail_maillog_enable,
	VAR_RMAIL_MAILRECORD_ENABLE, DEF_RMAIL_MAILRECORD_ENABLE, &var_rmail_mailrecord_enable,
	VAR_RMAIL_RCPOP3_ENABLE, DEF_RMAIL_RCPOP3_ENABLE, &var_rmail_rcpop3_enable,
	VAR_RMAIL_MAILCACHE_ENABLE, DEF_RMAIL_MAILCACHE_ENABLE, &var_rmail_mailcache_enable,
	VAR_RMAIL_ALLOW_LOCAL, DEF_RMAIL_ALLOW_LOCAL, &var_rmail_allow_local,
	VAR_RMAIL_POP3PD_ENABLE, DEF_RMAIL_POP3PD_ENABLE, &var_rmail_pop3pd_enable,
	0,
    };

    static CONFIG_STR_TABLE str_table[] = {
        VAR_RMAIL_MTADB_HOST, DEF_RMAIL_MTADB_HOST, &var_rmail_mtadb_host, 1, 0,
        VAR_RMAIL_MTADB_USER, DEF_RMAIL_MTADB_USER, &var_rmail_mtadb_user, 1, 0,
        VAR_RMAIL_MTADB_PASS, DEF_RMAIL_MTADB_PASS, &var_rmail_mtadb_pass, 1, 0,
        VAR_RMAIL_MTADB_NAME, DEF_RMAIL_MTADB_NAME, &var_rmail_mtadb_name, 1, 0,
	
        VAR_RMAIL_RECDB_HOST, DEF_RMAIL_RECDB_HOST, &var_rmail_recdb_host, 1, 0,
        VAR_RMAIL_RECDB_USER, DEF_RMAIL_RECDB_USER, &var_rmail_recdb_user, 1, 0,
        VAR_RMAIL_RECDB_PASS, DEF_RMAIL_RECDB_PASS, &var_rmail_recdb_pass, 1, 0,
        VAR_RMAIL_RECDB_NAME, DEF_RMAIL_RECDB_NAME, &var_rmail_recdb_name, 1, 0,
	
        VAR_RMAIL_AUTDB_HOST, DEF_RMAIL_AUTDB_HOST, &var_rmail_autdb_host, 1, 0,
        VAR_RMAIL_AUTDB_USER, DEF_RMAIL_AUTDB_USER, &var_rmail_autdb_user, 1, 0,
        VAR_RMAIL_AUTDB_PASS, DEF_RMAIL_AUTDB_PASS, &var_rmail_autdb_pass, 1, 0,
        VAR_RMAIL_AUTDB_NAME, DEF_RMAIL_AUTDB_NAME, &var_rmail_autdb_name, 1, 0,

	VAR_RMAIL_CACDB_HOST, DEF_RMAIL_CACDB_HOST, &var_rmail_cacdb_host, 1, 0,
	VAR_RMAIL_CACDB_USER, DEF_RMAIL_CACDB_USER, &var_rmail_cacdb_user, 1, 0,
	VAR_RMAIL_CACDB_PASS, DEF_RMAIL_CACDB_PASS, &var_rmail_cacdb_pass, 1, 0,
	VAR_RMAIL_CACDB_NAME, DEF_RMAIL_CACDB_NAME, &var_rmail_cacdb_name, 1, 0,
	
	VAR_RMAIL_AUTH_TABLE, DEF_RMAIL_AUTH_TABLE, &var_rmail_auth_table, 1, 0,
	VAR_RMAIL_AUTH_MAILIDFIELD, DEF_RMAIL_AUTH_MAILIDFIELD, &var_rmail_auth_mailidfield, 1, 0,
	VAR_RMAIL_AUTH_DOMAINFIELD, DEF_RMAIL_AUTH_DOMAINFIELD, &var_rmail_auth_domainfield, 1, 0,
	VAR_RMAIL_AUTH_ENCFIELD, DEF_RMAIL_AUTH_ENCFIELD, &var_rmail_auth_encfield, 1, 0,
	
        VAR_RMAIL_TRANSPORT_TABLE, DEF_RMAIL_TRANSPORT_TABLE, &var_rmail_transport_table, 1, 0,
        VAR_RMAIL_TRANSPORT_DOMAINFIELD, DEF_RMAIL_TRANSPORT_DOMAINFIELD, &var_rmail_transport_domainfield, 1, 0,
        VAR_RMAIL_TRANSPORT_IDXFIELD, DEF_RMAIL_TRANSPORT_IDXFIELD, &var_rmail_transport_idxfield, 1, 0,
        VAR_RMAIL_TRANSPORT_BASEDIRFIELD, DEF_RMAIL_TRANSPORT_BASEDIRFIELD, &var_rmail_transport_basedirfield, 1, 0,
	
        VAR_RMAIL_MAILUSER_TABLE, DEF_RMAIL_MAILUSER_TABLE, &var_rmail_mailuser_table, 1, 0,
        VAR_RMAIL_MAILUSER_MAILIDFIELD, DEF_RMAIL_MAILUSER_MAILIDFIELD, &var_rmail_mailuser_mailidfield, 1, 0,
        VAR_RMAIL_MAILUSER_DOMAINFIELD, DEF_RMAIL_MAILUSER_DOMAINFIELD, &var_rmail_mailuser_domainfield, 1, 0,
        VAR_RMAIL_MAILUSER_MHOSTFIELD, DEF_RMAIL_MAILUSER_MHOSTFIELD, &var_rmail_mailuser_mhostfield, 1, 0, 
	VAR_RMAIL_MAILUSER_MBOXFIELD, DEF_RMAIL_MAILUSER_MBOXFIELD, &var_rmail_mailuser_mboxfield, 1, 0,
        VAR_RMAIL_MAILUSER_STATUFIELD, DEF_RMAIL_MAILUSER_STATUFIELD, &var_rmail_mailuser_statufield, 1, 0,
	
        VAR_RMAIL_MAILRECORD_TABLE, DEF_RMAIL_MAILRECORD_TABLE, &var_rmail_mailrecord_table, 1, 0,
        VAR_RMAIL_MAILRECORD_MAILIDFIELD, DEF_RMAIL_MAILRECORD_MAILIDFIELD, &var_rmail_mailrecord_mailidfield, 1, 0,
        VAR_RMAIL_MAILRECORD_DOMAINFIELD, DEF_RMAIL_MAILRECORD_DOMAINFIELD, &var_rmail_mailrecord_domainfield, 1, 0,
        VAR_RMAIL_MAILRECORD_POP3IPFIELD, DEF_RMAIL_MAILRECORD_POP3IPFIELD, &var_rmail_mailrecord_pop3ipfield, 1, 0, 
        VAR_RMAIL_MAILRECORD_POP3TIMEFIELD, DEF_RMAIL_MAILRECORD_POP3TIMEFIELD, &var_rmail_mailrecord_pop3timefield, 1, 0,
	VAR_RMAIL_MAILRECORD_SMTPTIMEFIELD, DEF_RMAIL_MAILRECORD_SMTPTIMEFIELD, &var_rmail_mailrecord_smtptimefield, 1, 0,
	
	VAR_RMAIL_MAILDIR_SUFIX, DEF_RMAIL_MAILDIR_SUFIX, &var_rmail_maildir_sufix, 1, 0,
	
        VAR_RMAIL_MAILLOG_TABLE, DEF_RMAIL_MAILLOG_TABLE, &var_rmail_maillog_table, 1, 0,
        VAR_RMAIL_MAILLOG_MAILIDFIELD, DEF_RMAIL_MAILLOG_MAILIDFIELD, &var_rmail_maillog_mailidfield, 1, 0,
        VAR_RMAIL_MAILLOG_DOMAINFIELD, DEF_RMAIL_MAILLOG_DOMAINFIELD, &var_rmail_maillog_domainfield, 1, 0,
        VAR_RMAIL_MAILLOG_IPFIELD, DEF_RMAIL_MAILLOG_IPFIELD, &var_rmail_maillog_ipfield, 1, 0,
        VAR_RMAIL_MAILLOG_TYPEFIELD, DEF_RMAIL_MAILLOG_TYPEFIELD, &var_rmail_maillog_typefield, 1, 0,
        VAR_RMAIL_MAILLOG_TIMEFIELD, DEF_RMAIL_MAILLOG_TIMEFIELD, &var_rmail_maillog_timefield, 1, 0,

	VAR_RMAIL_MAILCACHE_TABLE, DEF_RMAIL_MAILCACHE_TABLE, &var_rmail_mailcache_table, 1, 0,
	VAR_RMAIL_MAILCACHE_DOMAINFIELD, DEF_RMAIL_MAILCACHE_DOMAINFIELD, &var_rmail_mailcache_domainfield, 1, 0,
	VAR_RMAIL_MAILCACHE_MAILIDFIELD, DEF_RMAIL_MAILCACHE_MAILIDFIELD, &var_rmail_mailcache_mailidfield, 1, 0,
	VAR_RMAIL_MAILCACHE_MHOSTFIELD, DEF_RMAIL_MAILCACHE_MHOSTFIELD, &var_rmail_mailcache_mhostfield, 1, 0,
	VAR_RMAIL_MAILCACHE_MBOXFIELD, DEF_RMAIL_MAILCACHE_MBOXFIELD, &var_rmail_mailcache_mboxfield, 1, 0,
	VAR_RMAIL_MAILCACHE_STATUFIELD, DEF_RMAIL_MAILCACHE_STATUFIELD, &var_rmail_mailcache_statufield, 1, 0,
	VAR_RMAIL_MAILCACHE_TIMEFIELD, DEF_RMAIL_MAILCACHE_TIMEFIELD, &var_rmail_mailcache_timefield, 1, 0,
	
	VAR_RMAIL_QUOTA_TMPFILE, DEF_RMAIL_QUOTA_TMPFILE, &var_rmail_quota_tmpfile, 1, 0,

	VAR_RMAIL_LOCAL_NEXTHOP, DEF_RMAIL_LOCAL_NEXTHOP, &var_rmail_local_nexthop, 1, 0,
	0,
    };

    static CONFIG_TIME_TABLE time_table[] = {
	VAR_RMAIL_POP3D_TMOUT, DEF_RMAIL_POP3D_TMOUT, &var_rmail_pop3d_tmout, 1, 0,
	VAR_POP3D_ERR_SLEEP, DEF_POP3D_ERR_SLEEP, &var_pop3d_err_sleep, 0, 0,
	0,
    };

    single_server_main(argc, argv, pop3d_service,
	    MAIL_SERVER_INT_TABLE, int_table,
	    MAIL_SERVER_STR_TABLE, str_table,
	    MAIL_SERVER_BOOL_TABLE, bool_table,
	    MAIL_SERVER_TIME_TABLE, time_table,
	    MAIL_SERVER_PRE_INIT, pre_jail_init,
	    MAIL_SERVER_PRE_ACCEPT, pre_accept,
	    0);
}
