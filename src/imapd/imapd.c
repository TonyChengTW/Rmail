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
#include <imap_stream.h>
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
#include <math.h>

/* Application-specific */

#include "imapd.h"
#include "imapd_chat.h"
#include "imapd_token.h"

int var_rmail_imapd_tmout;
int var_imapd_soft_erlim;
int var_imapd_err_sleep;

int var_rmail_mtadb_port;
int var_rmail_autdb_port;
int var_rmail_recdb_port;
int var_rmail_cacdb_port;
int var_rmail_buldb_port;
int var_imapd_soft_erlim;
int var_rmail_mailcache_expire;

bool  var_rmail_mtadb_interactive;
bool  var_rmail_autdb_interactive;
bool  var_rmail_recdb_interactive;
bool  var_rmail_cacdb_interactive;
bool  var_rmail_buldb_interactive;
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

char *var_rmail_buldb_host;
char *var_rmail_buldb_user;
char *var_rmail_buldb_pass;
char *var_rmail_buldb_name;

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

bool var_rmail_bulletin_enable;
char *var_rmail_bulrecord_table;
char *var_rmail_bulrecord_mailidfield;
char *var_rmail_bulrecord_domainfield;
char *var_rmail_bulrecord_recordfield;

char *var_rmail_bulcontent_table;
char *var_rmail_bulcontent_idfield;
char *var_rmail_bulcontent_activefield;
char *var_rmail_bulcontent_timefield;
char *var_rmail_bulcontent_uidlfield;
char *var_rmail_bulcontent_contentfield;

char *var_rmail_folder_table;
char *var_rmail_folder_domainfield;
char *var_rmail_folder_mailidfield;
char *var_rmail_folder_namefield;
char *var_rmail_folder_flagfield;

/* extern variables */
int used_count;
MYSQL mta_dbh;
MYSQL rec_dbh;
MYSQL aut_dbh;
MYSQL cac_dbh;
MYSQL bul_dbh;

static int parse_mdir(IMAPD_STATE *state);
static int parse_folder(IMAPD_STATE *state);

typedef struct IMAPD_CMD {
    char *name;
    int (*action) (IMAPD_STATE *, int, IMAPD_TOKEN *);
    int flags;
} IMAPD_CMD;

#define IMAPD_CMD_FLAG_LIMIT	    (1<<0)  /* limit usage */

static int append_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int authenticate_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int capability_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname = "capability_cmd";
  imapd_chat_reply(state, "* CAPABILITY IMAP4rev1");
  imapd_chat_reply(state, "%s OK CAPABILITY completed", argv[0].strval);
  return 0;
}

static int check_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int close_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int copy_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int create_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int delete_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int deleteacl_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int examine_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname="examine_cmd";
  char *fld;
  char *tmp;
  char *tmp2;
  char fld_flag[3]="";
  int i;
  int cnt_messages=0, cnt_unseen=0, cnt_recent=0, cnt_uidnext=0, cnt_uidvalidity=0;
  IMAPD_MSG *curmsg;
  IMAPD_FOLDER *curfolder;
  int maxuid;
  char *word, *brkt;
  char ret[512]="";

  if (state->authed == 0) {
    imapd_chat_reply(state, "%s NO Please, auth first.", argv[0].strval);
    return 1;
  }

  fld = strip_quote(argv[2].strval);
  
  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER *));
  curfolder = state->firstfolder;
  while (curfolder) {
    if (strcasecmp(curfolder->name, fld)==0) {
      strcpy(fld_flag, curfolder->flag);
      break;
    }
    curfolder = curfolder->nextfolder;
  }

  if (strlen(fld_flag)==0) {
    imapd_chat_reply(state, "%s NO Mailbox does not exist.", argv[0].strval);
    return 1;
  }

  curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG *));
  curmsg = state->firstmsg;
  while (curmsg) {
    if (strcasecmp(curmsg->f_folder, fld_flag)==0) {
      cnt_messages++;
      if (curmsg->f_flags[FLAG_RECENT] == '0')
	cnt_recent++;
      if (curmsg->f_flags[FLAG_SEEN] == '0') 
	cnt_unseen++;
      
      if (atoi(curmsg->uidl) > cnt_uidnext) 
	cnt_uidnext = atoi(curmsg->uidl)+1;

      if (atoi(curmsg->uidl) > cnt_uidvalidity)
	cnt_uidvalidity = atoi(curmsg->uidl);
    }
    curmsg=curmsg->nextmsg;
  }
  imapd_chat_reply(state, "* %d EXISTS", cnt_messages);
  imapd_chat_reply(state, "* %d RECENT", cnt_recent);
  imapd_chat_reply(state, "* OK [UNSEEN %d] Message %d is first unseen", cnt_unseen, cnt_unseen);
  imapd_chat_reply(state, "* OK [UIDVALIDITY %d] UIDs valid", cnt_uidvalidity);
  imapd_chat_reply(state, "* OK [UIDNEXT %d] Predicted next UID", cnt_uidnext);
  imapd_chat_reply(state, "* FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft)");
  imapd_chat_reply(state, "* OK [PERMANENTFLAGS ()] No permanent flags permitted");
  imapd_chat_reply(state, "%s OK [READ-ONLY] EXAMINE completed.", argv[0].strval);

  return 0;
}

static int expunge_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname="expunge_cmd";
  IMAPD_MSG *curmsg;
  

  if (state->authed == 0) {
    imapd_chat_reply(state, "%s NO Please, auth first.", argv[0].strval);
    return 1;
  }

  if (strlen(state->curflag)==0) {
    imapd_chat_reply(state, "%s NO Please select mailbox first.", argv[0].strval);
    return 1;
  }

  curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG));
  curmsg = state->firstmsg;

  while (curmsg) {
    if (strcasecmp(curmsg->f_folder, state->curflag)==0 &&
	curmsg->f_flags[FLAG_DELETED] != '0') {
      unlink(curmsg->file);
      imapd_chat_reply(state, "* %s EXPUNGE", curmsg->uidl);
    }
    curmsg = curmsg->nextmsg;
  }
  // repsrse_mdir again
  if (parse_mdir(state)) {
    imapd_chat_reply(state, "%s NO Expunge fail.", argv[0].strval);
    return 1;
  }
  imapd_chat_reply(state, "%s OK EXPUNGE completed.", argv[0].strval);
  return 0;
}

static int fetch_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int getacl_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int getqouta_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int getquotaroot_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int list_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname="list_cmd";
  IMAPD_FOLDER *curfolder;
  char *ref, *wld;
  char *patten;
  int show =0;

  if (state->authed==0) {
    imapd_chat_reply(state, "%s NO Please, auth first.", argv[0].strval);
    return 1;
  }

  ref = strip_taildot(strip_quote(argv[2].strval));
  wld = strip_quote(argv[3].strval);
  patten = (char *) malloc (sizeof(char *) * (strlen(ref) + strlen(wld)+1));

  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER *));
  curfolder = state->firstfolder;
  while (curfolder) {
    if (strlen(wld) == 0 && strlen(ref)==0) {
      // all empty
      imapd_chat_reply(state, "* LIST (\\Noselect) \".\" \"\"");
      break;
    } else if (strlen(wld) == 0) {
      //wld is empty, match ref only and strip name
      if (strcasecmp(curfolder->name, ref)==0) {
	imapd_chat_reply(state, "* LIST (%s) \".\" \"\"", get_folderflag(state, curfolder->name));
      }
    } else if (strlen(ref)==0) {
      // ref is empty, match only wildcard
      if (wildcardfit(wld, curfolder->name)) {
	imapd_chat_reply(state, "* LIST (%s) \".\" \"%s\"", get_folderflag(state, curfolder->name), curfolder->name);
      }
    } else {
      // wld is not empty, wildcardfit it
      sprintf(patten, "%s.%s", ref, wld);
      if (wildcardfit(patten, curfolder->name)) {
	imapd_chat_reply(state, "* LIST (%s) \".\" \"%s\"", get_folderflag(state, curfolder->name), curfolder->name);
      }
    }
    curfolder=curfolder->nextfolder;
  }
  
  imapd_chat_reply(state, "%s OK LIST completed", argv[0].strval);
  free(patten);
  return 0;
}

static int listrights_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int login_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname="login_cmd";
  char sql[512];
  MYSQL_RES *res;
  MYSQL_ROW row;
  struct passwd *pw;
  char basedir[128], mhost[128], mdir[128];
  int mailcache_hitted=0;
  static char *username, *domain;
  static int domain_idx;
  IMAPD_FOLDER *curfolder;

  if (state->authed==1) {
    imapd_chat_reply(state, "%s NO You've logged in as %s.",
      argv[0].strval, state->userid);
    return 1;
  }

  if (argc != 4) {
    imapd_chat_reply(state, "%s NO Error in IMAP syntax.", argv[0].strval);
    return 1;
  }

  state->userid = strip_quote(argv[2].strval);
  state->passwd = strip_quote(argv[3].strval);

  pw = getpwnam(argv[1].strval);

  if (pw && var_rmail_allow_local) {
    // local account
    if (var_rmail_allow_local) {
      if (strcmp(pw->pw_passwd, crypt(state->passwd, pw->pw_passwd))==0) {
        //auth ok
        state->mdir = concatenate(pw->pw_dir, "/", var_rmail_maildir_sufix, "/", (char *) 0);
        set_eugid(pw->pw_uid, pw->pw_gid);
        state->userid=strdup(argv[1].strval);
      } else {
        //auth fail
        state->userid=0;
        state->passwd=0;
        imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
        return 1;
      }
    } else {
      state->userid=0;
      state->passwd=0;
      imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
      return 1;
    }
  } else {
    username = (char *) malloc((strlen(state->userid)+1) * sizeof(char *));
    username = strdup(state->userid);


    if ((domain = strrchr(state->userid, '@')) != 0) {
      /* there's a @ */
      username[strlen(state->userid) - strlen(domain)]='\0';
      domain++;
      // Get domain_idx;
      sprintf(sql, "SELECT %s FROM %s WHERE %s='%s'",
          var_rmail_transport_idxfield, var_rmail_transport_table,
          var_rmail_transport_domainfield, domain);
      if (msg_verbose)
        msg_info("%s: SQL => %s", myname, sql);
      if (mysql_real_query(&mta_dbh, sql, strlen(sql))!=0) {
        msg_warn("%s: Query fail(%s)", myname, mysql_error(&mta_dbh));
        imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
        state->userid=0;
        state->passwd=0;
        free(username);
        return 1;
      }

      res = mysql_store_result(&mta_dbh);
      if (mysql_num_rows(res)==0) {
        // no such domain?
        imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
        state->userid=0;
        state->passwd=0;
        mysql_free_result(res);
        free(username);
        return 1;
      } else if (mysql_num_rows(res)!=1) {
        imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
        state->userid=0;
        state->passwd=0;
        mysql_free_result(res);
        free(username);
        return 1;
      } else {
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        domain_idx = (row[0])? atoi(row[0]) : 0;
        if (msg_debug)
          msg_info("%s: Pre decide domain_idx=%d(%s)", myname, domain_idx, domain);
        state->userid=strdup(username);
      }
      free(username);
    } else {
      domain_idx=-1;
    }

    // Do Auth
    sprintf(sql, "SELECT %s FROM %s WHERE %s='%s' AND %s=ENCRYPT('%s', LEFT(%s, 2))",
        var_rmail_auth_domainfield, var_rmail_auth_table,
        var_rmail_auth_mailidfield, state->userid,
        var_rmail_auth_encfield, state->passwd, var_rmail_auth_encfield);
    if (msg_verbose)
      msg_info("%s: SQL => %s", myname, sql);
    if (mysql_real_query(&aut_dbh, sql, strlen(sql))!=0) {
      msg_warn("%s: Query fail(%s)", myname, mysql_error(&aut_dbh));
      imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
      state->userid=0;
      state->passwd=0;
      return 1;
    }

    res = mysql_store_result(&aut_dbh);
    if (mysql_num_rows(res)==0) {
      imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
      state->userid=0;
      state->passwd=0;
      mysql_free_result(res);
      return 1;
    } else if (mysql_num_rows(res)!=1) {
      imapd_chat_reply(state, "%s NO Login fail(multi-row).", argv[0].strval);
      state->userid=0;
      state->passwd=0;
      mysql_free_result(res);
      return 1;
    }
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    state->domain = (row[0])? atoi(row[0]) : 0;

    if (state->domain == 0) {
      imapd_chat_reply(state, "%s NO Login fail(no domain).", argv[0].strval);
      state->userid=0;
      state->passwd=0;
      return 1;
    } else if (domain_idx != -1 && state->domain != domain_idx) {
      imapd_chat_reply(state, "%s NO Login fail(password and transport not fit).", argv[0].strval);
      state->userid=0;
      state->passwd=0;
      return 1;
    }

    // get basedir
    sprintf(sql, "SELECT %s FROM %s WHERE %s=%d",
        var_rmail_transport_basedirfield, var_rmail_transport_table,
        var_rmail_transport_idxfield, state->domain);
    if (msg_verbose)
      msg_info("%s: SQL => %s", myname, sql);
    if (mysql_real_query(&mta_dbh, sql, strlen(sql))!=0) {
      imapd_chat_reply(state, "%s NO Login fail(wrong basedir).", argv[0].strval);
      state->userid=0;
      state->passwd=0;
      state->domain=0;
      return 1;
    }
    res = mysql_store_result(&mta_dbh);
    if (mysql_num_rows(res)!=1) {
      imapd_chat_reply(state, "%s NO Login fail(No basedir record).", argv[0].strval);
      state->userid=0;
      state->passwd=0;
      state->domain=0;
      mysql_free_result(res);
      return 1;
    }

    row = mysql_fetch_row(res);
    mysql_free_result(res);
    strcpy(basedir, row[0]);


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
        imapd_chat_reply(state, "%s NO Login fail(Query fail).", argv[0].strval);
        state->userid=0;
        state->passwd=0;
        state->domain=0;
        return 1;
      }

      res = mysql_store_result(&mta_dbh);
      if (mysql_num_rows(res)==0) {
        msg_warn("%s: No such user by exist in queue(%s)", myname, state->userid);
        imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
        state->userid=0;
        state->passwd=0;
        state->domain=0;
        mysql_free_result(res);
        return 1;
      } else if (mysql_num_rows(res)!=1) {
        msg_warn("%s: Double record in %s (%s)", myname, var_rmail_mailuser_table, state->userid);
        imapd_chat_reply(state, "%s NO Login fail.", argv[0].strval);
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

    state->mdir=concatenate(
        (char *) basedir, "/", (char *) mhost, "/", (char *) mdir, "/", var_rmail_maildir_sufix, (char *) 0);
  }

  if (msg_verbose)
    msg_info("%s: %s Authed ok, maildir=%s", myname, state->userid, state->mdir);

  //parse folder
  if (parse_folder(state)) {
    imapd_chat_reply(state, "%s NO Error in parsing folder.", argv[0].strval);
    state->userid=0;
    state->passwd=0;
    state->domain=0;
    return 1;
  }
  // debug folder
  curfolder = state->firstfolder;
  while (curfolder != NULL) {
    curfolder=curfolder->nextfolder;
  }
  
  //parse mdir
  if (parse_mdir(state)) {
    imapd_chat_reply(state, "%s NO Error in parsing maildir.", argv[0].strval);
    state->userid=0;
    state->passwd=0;
    state->domain=0;
    return 1;
  }

  imapd_chat_reply(state, "%s OK LOGIN Ok.", argv[0].strval);
  state->authed=1;

  return 0;
}

static int logout_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname = "logout_cmd";

  imapd_chat_reply(state, "* BYE Rmail-IMAPD Server shutting down.");
  imapd_chat_reply(state, "%s OK LOGOUT completed.", argv[0].strval);
  //TODO clear something

  return 0;
}

static int lsub_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname="lsub_cmd";
  IMAPD_FOLDER *curfolder;
  char *ref, *wld;
  char *patten;
  int show =0;

  if (state->authed==0) {
    imapd_chat_reply(state, "%s NO Please, auth first.", argv[0].strval);
    return 1;
  }

  ref = strip_taildot(strip_quote(argv[2].strval));
  wld = strip_quote(argv[3].strval);
  patten = (char *) malloc (sizeof(char *) * (strlen(ref) + strlen(wld)+1));

  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER *));
  curfolder = state->firstfolder;
  while (curfolder) {
    if (strlen(wld) == 0 && strlen(ref)==0) {
      // all empty
      imapd_chat_reply(state, "* LSUB (\\Noselect) \".\" \"\"");
      break;
    } else if (strlen(wld) == 0) {
      //wld is empty, match ref only and strip name
      if (strcasecmp(curfolder->name, ref)==0) {
	imapd_chat_reply(state, "* LSUB (%s) \".\" \"\"", get_folderflag(state, curfolder->name));
      }
    } else if (strlen(ref)==0) {
      // ref is empty, match only wildcard
      if (wildcardfit(wld, curfolder->name)) {
	imapd_chat_reply(state, "* LSUB (%s) \".\" \"%s\"", get_folderflag(state, curfolder->name), curfolder->name);
      }
    } else {
      // wld is not empty, wildcardfit it
      sprintf(patten, "%s.%s", ref, wld);
      if (wildcardfit(patten, curfolder->name)) {
	imapd_chat_reply(state, "* LSUB (%s) \".\" \"%s\"", get_folderflag(state, curfolder->name), curfolder->name);
      }
    }
    curfolder=curfolder->nextfolder;
  }
  
  imapd_chat_reply(state, "%s OK LSUB completed", argv[0].strval);
  free(patten);
  return 0;
}

static int myrights_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int noop_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int rename_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int search_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int select_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname="select_cmd";
  char *fld;
  char *tmp;
  char *tmp2;
  char fld_flag[3]="";
  int i;
  int cnt_messages=0, cnt_unseen=0, cnt_recent=0, cnt_uidnext=0, cnt_uidvalidity=0;
  IMAPD_MSG *curmsg;
  IMAPD_FOLDER *curfolder;
  int maxuid;
  char *word, *brkt;
  char ret[512]="";

  if (state->authed == 0) {
    imapd_chat_reply(state, "%s NO Please, auth first.", argv[0].strval);
    return 1;
  }

  fld = strip_quote(argv[2].strval);
  
  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER *));
  curfolder = state->firstfolder;
  while (curfolder) {
    if (strcasecmp(curfolder->name, fld)==0) {
      strcpy(fld_flag, curfolder->flag);
      break;
    }
    curfolder = curfolder->nextfolder;
  }

  if (strlen(fld_flag)==0) {
    imapd_chat_reply(state, "%s NO Mailbox does not exist.", argv[0].strval);
    return 1;
  }

  strcpy(state->curflag, fld_flag);

  curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG *));
  curmsg = state->firstmsg;
  while (curmsg) {
    if (strcasecmp(curmsg->f_folder, fld_flag)==0) {
      cnt_messages++;
      if (curmsg->f_flags[FLAG_RECENT] == '0')
	cnt_recent++;
      if (curmsg->f_flags[FLAG_SEEN] == '0') 
	cnt_unseen++;
      
      if (atoi(curmsg->uidl) > cnt_uidnext) 
	cnt_uidnext = atoi(curmsg->uidl)+1;

      if (atoi(curmsg->uidl) > cnt_uidvalidity)
	cnt_uidvalidity = atoi(curmsg->uidl);
    }
    curmsg=curmsg->nextmsg;
  }
  imapd_chat_reply(state, "* %d EXISTS", cnt_messages);
  imapd_chat_reply(state, "* %d RECENT", cnt_recent);
  imapd_chat_reply(state, "* OK [UNSEEN %d] Message %d is first unseen", cnt_unseen, cnt_unseen);
  imapd_chat_reply(state, "* OK [UIDVALIDITY %d] UIDs valid", cnt_uidvalidity);
  imapd_chat_reply(state, "* OK [UIDNEXT %d] Predicted next UID", cnt_uidnext);
  imapd_chat_reply(state, "* FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft)");
  imapd_chat_reply(state, "* OK [PERMANENTFLAGS ()] No permanent flags permitted");
  imapd_chat_reply(state, "%s OK [READ-ONLY] SELECT completed.", argv[0].strval);
  return 0;
}

static int setacl_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int setquota_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int status_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname="status_cmd";
  char *fld;
  char *tmp;
  char *tmp2;
  char fld_flag[3]="";
  int i;
  int cnt_messages=0, cnt_unseen=0, cnt_recent=0, cnt_uidnext=0, cnt_uidvalidity=0;
  IMAPD_MSG *curmsg;
  IMAPD_FOLDER *curfolder;
  int maxuid;
  char *word, *brkt;
  char ret[512]="";

  if (state->authed == 0) {
    imapd_chat_reply(state, "%s NO Please, auth first.", argv[0].strval);
    return 1;
  }

  if (argc < 4) {
    imapd_chat_reply(state, "%s NO Error syntax.", argv[0].strval);
    return 1;
  }

  fld = strip_quote(argv[2].strval);
  
  tmp = (char *) malloc (sizeof(char *) * 512);
  tmp[0]='\0';
  
  for (i=3; i<argc; i++) {
    sprintf(tmp, "%s %s", tmp, argv[i].strval);
  }
  tmp2 = strip_leadspace(tmp);
  free(tmp);
  // find flags;

  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER *));
  curfolder = state->firstfolder;
  while (curfolder) {
    if (strcasecmp(curfolder->name, fld)==0) {
      strcpy(fld_flag, curfolder->flag);
      break;
    }
    curfolder = curfolder->nextfolder;
  }

  if (strlen(fld_flag)==0) {
    imapd_chat_reply(state, "%s NO Mailbox does not exist.", argv[0].strval);
    return 1;
  }

  curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG *));
  curmsg = state->firstmsg;
  while (curmsg) {
    if (strcasecmp(curmsg->f_folder, fld_flag)==0) {
      cnt_messages++;
      if (curmsg->f_flags[FLAG_RECENT] == '0')
	cnt_recent++;
      if (curmsg->f_flags[FLAG_SEEN] == '0') 
	cnt_unseen++;
      
      if (atoi(curmsg->uidl) > cnt_uidnext) 
	cnt_uidnext = atoi(curmsg->uidl)+1;

      if (atoi(curmsg->uidl) > cnt_uidvalidity)
	cnt_uidvalidity = atoi(curmsg->uidl);
    }
    curmsg=curmsg->nextmsg;
  }

  for (word=strtok_r(tmp2, " ", &brkt); word; word=strtok_r(NULL, " ", &brkt)) {
    if (strcmp(word, "MESSAGES")==0)
      sprintf(ret, "%s%sMESSAGES %d", ret, strlen(ret)==0 ? "":" ", cnt_messages);
    if (strcmp(word, "UNSEEN")==0)
      sprintf(ret, "%s%sUNSEEN %d", ret, strlen(ret)==0 ? "":" ", cnt_unseen);
    if (strcmp(word, "RECENT")==0)
      sprintf(ret, "%s%sRECENT %d", ret,  strlen(ret)==0 ? "":" ", cnt_recent);
    if (strcmp(word, "UIDNEXT")==0)
      sprintf(ret, "%s%sUIDNEXT %d", ret, strlen(ret)==0 ? "":" ", cnt_uidnext);
    if (strcmp(word, "UIDVALIDITY")==0)
      sprintf(ret, "%s%sUIDVALIDITY %s", ret, strlen(ret)==0 ? "":" ", cnt_uidvalidity);
  }
  imapd_chat_reply(state, "* STATUS \"%s\" (%s)", fld, ret);
  imapd_chat_reply(state, "%s OK STATUS Completed.", argv[0].strval);
  return 0;
}

static int store_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int subscribe_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int uid_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
  const char *myname = "uid_cmd";
  IMAPD_MSG *curmsg;
  char *word, *brkt;
  int seq_from=0, seq_to=0, uid_from=0, uid_to=0;
  char tmp[4096]="";

  if (state->authed == 0) {
    imapd_chat_reply(state, "%s NO Please, auth first.", argv[0].strval);
    return 1;
  }

  if (strlen(state->curflag)==0) {
    imapd_chat_reply(state, "%s NO Please select mailbox first.", argv[0].strval);
    return 1;
  }

  if (strcasecmp(argv[2].strval, "SEARCH")==0) {
    if (argc==4) {
      // seq search
      for (word = strtok_r(argv[3].strval, ":", &brkt); word; word=strtok_r(NULL, ":", &brkt)) {
	if (seq_from ==0) {
	  seq_from = atoi(word);
	} else {
	  seq_to = atoi(word);
	}
      }
    } else if (argc==5) {
      // seq + uid search
      for (word = strtok_r(argv[4].strval, ":", &brkt); word; word=strtok_r(NULL, ":", &brkt)) {
	if (uid_from ==0) {
	  uid_from = atoi(word);
	} else {
	  uid_to = atoi(word);
	}
      }
    } else if (argc==6) {
      // uid search
      for (word = strtok_r(argv[5].strval, ":", &brkt); word; word=strtok_r(NULL, ":", &brkt)) {
	if (uid_from ==0) {
	  uid_from = atoi(word);
	} else {
	  uid_to = atoi(word);
	}
      }

      for (word = strtok_r(argv[2].strval, ":", &brkt); word; word=strtok_r(NULL, ":", &brkt)) {
	if (seq_from ==0) {
	  seq_from = atoi(word);
	} else {
	  seq_to = atoi(word);
	}
      }
    }
    curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG *));
    curmsg = state->firstmsg;
    while (curmsg) {
      if (strcasecmp(curmsg->f_folder, state->curflag)==0) {
	if (seq_from==0 ||
	    seq_to==0 ||
	    (seq_from <= atoi(curmsg->uidl) && seq_to >= atoi(curmsg->uidl))) {
	  
	  if (uid_from==0 ||
	      uid_to==0 ||
	      (uid_from <= atoi(curmsg->uidl) && uid_to >= atoi(curmsg->uidl))) {
	    sprintf(tmp, "%s%s%s", tmp, strlen(tmp)==0 ? "": " ", curmsg->uidl);
	  }
	}
      }
      curmsg=curmsg->nextmsg;
    }
    imapd_chat_reply(state, "* SEARCH %s", tmp);
  } else if (strcasecmp(argv[2].strval, "COPY")==0) {


  } else if (strcasecmp(argv[2].strval, "FETCH")==0) {


  } else if (strcasecmp(argv[2].strval, "STORE") ==0) {


  } else {
    imapd_chat_reply(state, "%s NO Wrong UID syntax.", argv[0].strval);
    return 1;
  }
  
  imapd_chat_reply(state, "%s OK UID completed.", argv[0].strval);
  return 0;
}

static int unselect_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}

static int unsubscribe_cmd(IMAPD_STATE *state, int argc, IMAPD_TOKEN *argv) {
}



static IMAPD_CMD imapd_cmd_table[] = {
  "APPEND", append_cmd, IMAPD_CMD_FLAG_LIMIT,
  "AUTHENTICATE", authenticate_cmd, IMAPD_CMD_FLAG_LIMIT,
  "CAPABILITY", capability_cmd, IMAPD_CMD_FLAG_LIMIT,
  "CHECK", check_cmd, IMAPD_CMD_FLAG_LIMIT,
  "CLOSE", close_cmd, IMAPD_CMD_FLAG_LIMIT,
  "COPY", copy_cmd, IMAPD_CMD_FLAG_LIMIT,
  "CREATE", create_cmd, IMAPD_CMD_FLAG_LIMIT,
  "DELETE", delete_cmd, IMAPD_CMD_FLAG_LIMIT,
  "DELETEACL", deleteacl_cmd, IMAPD_CMD_FLAG_LIMIT,
  "EXAMINE", examine_cmd, IMAPD_CMD_FLAG_LIMIT,
  "EXPUNGE", expunge_cmd, IMAPD_CMD_FLAG_LIMIT,
  "FETCH", fetch_cmd, IMAPD_CMD_FLAG_LIMIT,
  "GETACL", getacl_cmd, IMAPD_CMD_FLAG_LIMIT,
  "GETQUOTA", getqouta_cmd, IMAPD_CMD_FLAG_LIMIT,
  "GETQUOTAROOT", getquotaroot_cmd, IMAPD_CMD_FLAG_LIMIT,
  "LIST", list_cmd, IMAPD_CMD_FLAG_LIMIT,
  "LISTRIGHTS", listrights_cmd, IMAPD_CMD_FLAG_LIMIT,
  "LOGIN", login_cmd, 0,
  "LOGOUT", logout_cmd, 0,
  "LSUB", lsub_cmd, IMAPD_CMD_FLAG_LIMIT,
  "MYRIGHTS", myrights_cmd, IMAPD_CMD_FLAG_LIMIT,
  "NOOP", noop_cmd, IMAPD_CMD_FLAG_LIMIT,
  "RENAME", rename_cmd, IMAPD_CMD_FLAG_LIMIT,
  "SEARCH", search_cmd, IMAPD_CMD_FLAG_LIMIT,
  "SELECT", select_cmd, IMAPD_CMD_FLAG_LIMIT,
  "SETACL", setacl_cmd, IMAPD_CMD_FLAG_LIMIT,
  "SETQUOTA", setquota_cmd, IMAPD_CMD_FLAG_LIMIT,
  "STATUS", status_cmd, IMAPD_CMD_FLAG_LIMIT,
  "STORE", store_cmd, IMAPD_CMD_FLAG_LIMIT,
  "SUBSCRIBE", subscribe_cmd, IMAPD_CMD_FLAG_LIMIT,
  "UID", uid_cmd, IMAPD_CMD_FLAG_LIMIT,
  "UNSELECT", unselect_cmd, IMAPD_CMD_FLAG_LIMIT,
  "UNSUBSCRIBE", unsubscribe_cmd, IMAPD_CMD_FLAG_LIMIT,
  // IMAP4 Capabilities
  /*
  "ACL", acl_cmd, IMAPD_CMD_FLAG_LIMIT,
  "CATENATE", catenate_cmd, IMAPD_CMD_FLAG_LIMIT,
  "CONDSTORE", condstore_cmd, IMAPD_CMD_FLAG_LIMIT,
  "ID", id_cmd, IMAPD_CMD_FLAG_LIMIT,
  "IDLE", idle_cmd, IMAPD_CMD_FLAG_LIMIT,
  "LITERAL+", literalplus_cmd, IMAPD_CMD_FLAG_LIMIT,
  "LOGINDISABLED", logindisabled_cmd, IMAPD_CMD_FLAG_LIMIT,
  "LOGIN-REFERRALS", loginreferrals_cmd, IMAPD_CMD_FLAG_LIMIT,
  "MAILBOX-REFERRALS", mailboxreferrals_cmd, IMAPD_CMD_FLAG_LIMIT,
  "MULTIAPPEND", multiappend_cmd, IMAPD_CMD_FLAG_LIMIT,
  "NAMESPACE", namespace_cmd, IMAPD_CMD_FLAG_LIMIT,
  "QUOTA", quota_cmd, IMAPD_CMD_FLAG_LIMIT,
  "RIGHTS=", rightequal_cmd, IMAPD_CMD_FLAG_LIMIT,
  "SORT=MODSEQ", sortmodseq_cmd, IMAPD_CMD_FLAG_LIMIT,
  "STARTTLS", starttls_cmd, 0,
  "UIDPLUS", uidplus_cmd, IMAPD_CMD_FLAG_LIMIT,
  "UNSELECT", unselect_cmd, IMAPD_CMD_FLAG_LIMIT,
  "URLAUTH", urlauth_cmd, IMAPD_CMD_FLAG_LIMIT,
  */
  0,
};

static void imapd_proto(IMAPD_STATE *state) {
    const char *myname="imapd_proto";
    static time_t var_now;
    int argc;
    IMAPD_CMD *cmdp;
    IMAPD_TOKEN *argv;
    
    time(&var_now);
    
    imap_timeout_setup(state->client, var_rmail_imapd_tmout);
    
    switch (vstream_setjmp(state->client)) {
	default:
	    msg_panic("%s: unknown error reading from %s[%s]",
		myname, state->name, state->addr);
	    break;
	
	case IMAP_ERR_TIME:
	    state->reason = "timeout";
	    //imapd_chat_reply(state, " timeout exceeded.");
	    break;
	
	case IMAP_ERR_EOF:
	    state->reason = "lost connection";
	    break;
	
	case 0:

	    imapd_chat_reply(state, "* OK [CAPABILITY IMAP4rev1] %s Rmail-IMAP ready. Copyright 1998-2005 Watcher Information, Ltd. <%d-%d-%d@%s>.",
		var_myhostname, (int) var_now, used_count, (int) mysql_thread_id(&mta_dbh),
		var_myhostname);

	    for (;;) {
		watchdog_pat();
		imapd_chat_query(state);

		if ((argc = imapd_token(vstring_str(state->buffer), &argv)) == 0 ) {
		    imapd_chat_reply(state, "-ERR bad syntax.");
		    state->error_count++;
		    continue;
		}

		
		if (argc < 2) {
			imapd_chat_reply(state, "%s NO Error in IMAP command received by server.", argv[0].strval);
			continue;
		}
		for (cmdp = imapd_cmd_table; cmdp->name !=0; cmdp++) 
		    if (strcasecmp(argv[1].strval, cmdp->name) ==0 )
			break;

		if (cmdp->name == 0) {
		    imapd_chat_reply(state, "%s NO Error in IMAP command received by server.", argv[0].strval);
		    continue;
		}

		state->where = cmdp->name;
		if (cmdp->action(state, argc, argv)!=0)
		    state->error_count++;

		if ((cmdp->flags & IMAPD_CMD_FLAG_LIMIT))
		    state->error_count++;
		if (cmdp->action == logout_cmd)
		    break;
	    }
	    break;
    }

    if (state->reason && state->where
        && (strcmp(state->where, IMAPD_AFTER_DOT)
            || strcmp(state->reason, "lost connection")))
        msg_info("%s after %s from %s[%s]",
                 state->reason, state->where, state->name, state->addr);

}


static void imapd_service(VSTREAM *stream, char *unused_service, char **argv) {
    const char *myname = "imapd_service";
    IMAPD_STATE state;
    IMAPD_FOLDER *folder;

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

    if (var_rmail_bulletin_enable) {
      if (mysql_ping(&bul_dbh) != 0) {
	msg_warn("%s: BUL database(%s) had gone(%s), attempted to reconnect",
	    myname, var_rmail_buldb_host, mysql_error(&bul_dbh));
	if (msg_verbose)
	  msg_info("%s: BUL database(%s) reconnectted, id %lu",
	      myname, var_rmail_buldb_host, mysql_thread_id(&bul_dbh));
      } else if (msg_verbose)
	msg_info("%s: BUL database(%s) still alive", myname, var_rmail_buldb_host);
    }
    



    
    
    used_count++;
    if (argv[0])
	msg_fatal("unexpected command-line argument: %s", argv[0]);

    imapd_state_init(&state, stream);
    msg_info("connect from %s[%s]", state.name, state.addr);

    

    imapd_proto(&state);



    
    if (state.domain)
	msg_info("disconnect from %s[%s] %s:%d:%d:%ld:%s",
	    state.name, state.addr, state.userid, state.domain, state.retr_count, state.retr_size, state.addr);
    imapd_state_reset(&state);

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

    if (var_rmail_bulletin_enable) {
      mysql_init(&bul_dbh);
      if (mysql_real_connect(&bul_dbh, var_rmail_buldb_host, var_rmail_buldb_user,
	    var_rmail_buldb_pass, var_rmail_buldb_name, (unsigned int) var_rmail_buldb_port,
	    NULL, var_rmail_buldb_interactive ? CLIENT_INTERACTIVE:0) == NULL) {
	msg_fatal("%s: BUL database(%s) connection fail: %s",
	    myname, var_rmail_buldb_host, mysql_error(&bul_dbh));
      } else if (msg_verbose) {
	msg_info("%s: BUL database(%s) connectted, id %lu",
	    myname, var_rmail_buldb_host, mysql_thread_id(&bul_dbh));
      }
    }

    used_count=0;
    /* Database connection */
    if (msg_verbose)
      msg_info("%s: pre_jail_init", myname);

}

static int parse_folder(IMAPD_STATE *state) {
  const char *myname = "parse_folder";
  char sql[512];
  IMAPD_FOLDER *curfolder, *thisfolder;
  MYSQL_RES *res;
  MYSQL_ROW row;

  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER));
  strcpy(curfolder->name, "INBOX");
  strcpy(curfolder->flag, "00");
  state->firstfolder = curfolder;
  thisfolder = curfolder;
  thisfolder->nextfolder = NULL;

  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER));
  strcpy(curfolder->name, "INBOX.Trash");
  strcpy(curfolder->flag, "01");
  thisfolder->nextfolder = curfolder;
  thisfolder = curfolder;
  thisfolder->nextfolder = NULL;
  
  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER));
  strcpy(curfolder->name, "INBOX.Drafts");
  strcpy(curfolder->flag, "02");
  thisfolder->nextfolder = curfolder;
  thisfolder = curfolder;
  thisfolder->nextfolder = NULL;

  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER));
  strcpy(curfolder->name, "INBOX.Sent");
  strcpy(curfolder->flag, "03");
  thisfolder->nextfolder = curfolder;
  thisfolder = curfolder;
  thisfolder->nextfolder = NULL;

  // query from databases
  sprintf(sql, "SELECT %s, %s FROM %s WHERE %s=%d AND %s='%s'",
      var_rmail_folder_namefield, var_rmail_folder_flagfield, var_rmail_folder_table,
      var_rmail_folder_domainfield, state->domain, var_rmail_folder_mailidfield, state->userid);
  if (msg_verbose)
    msg_info("%s: SQL => %s", myname, sql);
  if (mysql_real_query(&mta_dbh, sql, strlen(sql))!=0) {
    msg_warn("%s: Query fail(%s)", myname, mysql_error(&cac_dbh));
    mysql_free_result(res);
    return 1;
  }

  res = mysql_store_result(&mta_dbh);
  if (mysql_num_rows(res)!=0) {
    while (row = mysql_fetch_row(res)) {
      curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER));
      strcpy(curfolder->name, row[0]);
      strcpy(curfolder->flag, row[1]);
      thisfolder->nextfolder = curfolder;
      thisfolder = curfolder;
      thisfolder->nextfolder=NULL;
    }
  }

  
  return 0;
}

static int parse_mdir(IMAPD_STATE *state) {
  const char *myname = "parse_mdir";
  char *newdir;
  DIR *dirp, *dp;
  struct dirent *dire;
  struct stat fptr;
  IMAPD_MSG *curmsg, *thismsg;
  unsigned long fts, fpid;
  char flag[9], fdr[3], opop[3], hn[128];
  char buf[1024];
  char *gzinfo;
  char *filename;
  char sql[512];
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned long bulletin_record=0;
  unsigned int bulletin_id=0;
  unsigned long current_mask=0;
  int i;
  int n;
  struct dirent **namelist;

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

  n = scandir(newdir, &namelist, 0, alphasort);
  if (n < 0)
    return 1;

//  while (dire = readdir(dirp)) {
  while (n--) {
    dire = namelist[n];
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
    curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG));
    strcpy(curmsg->file, filename);
    curmsg->flags = 0;
    curmsg->is_bul=0;

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

    sprintf(curmsg->uidl, "%lu", fts, fpid);
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
  
  // bulletins
  if (var_rmail_bulletin_enable) {
    sprintf(sql, "SELECT %s FROM %s WHERE %s='%s' AND %s=%d",
	var_rmail_bulrecord_recordfield, var_rmail_bulrecord_table,
	var_rmail_bulrecord_mailidfield, state->userid,
	var_rmail_bulrecord_domainfield, state->domain);
    if (msg_verbose)
      msg_info("%s: SQL => %s", myname, sql);
    if (mysql_real_query(&bul_dbh, sql, strlen(sql))!=0) {
      msg_warn("%s: Query fail(%s)", myname, mysql_error(&bul_dbh));
      return 0;
    }
    res = mysql_store_result(&bul_dbh);
    if (mysql_num_rows(res)==0) {
      mysql_free_result(res);
      return 0;
    }

    row = mysql_fetch_row(res);
    mysql_free_result(res);
    bulletin_record = atol(row[0]);
    state->bulletin_record= bulletin_record;
    
    if (msg_verbose)
      msg_info("%s: Got bulletin record %lu for %s(%d)", myname, bulletin_record, state->userid, state->domain);
    
    if (bulletin_record == 0) {
      // no bulletin unreaded
      return 0;
    }

    for (i=1; i<63; i++) {
      // if more than records
      current_mask = pow(2, (i-1));
      if (bulletin_record< current_mask) {
	break;
      }

      if ((bulletin_record & current_mask) == 0) {
	// no this bit
	continue;
      }

      // get uidl & content
      sprintf(sql, "SELECT %s, %s, LENGTH(%s) FROM %s WHERE %s=%d AND %s='Y' AND %s>FROM_UNIXTIME(%d)",
	  var_rmail_bulcontent_uidlfield, var_rmail_bulcontent_contentfield, var_rmail_bulcontent_contentfield,
	  var_rmail_bulcontent_table, var_rmail_bulcontent_idfield, i,
	  var_rmail_bulcontent_activefield, var_rmail_bulcontent_timefield, 
	  time(NULL));
      if (msg_verbose)
	msg_info("%s: SQL => %s", myname, sql);
      if (mysql_real_query(&bul_dbh, sql, strlen(sql))!=0) {
	msg_warn("%s: Query fail(%s)", myname, mysql_error(&bul_dbh));
	continue;
      }
      res = mysql_store_result(&bul_dbh);
      if (mysql_num_rows(res)==0) {
	mysql_free_result(res);
	continue;
      }
      row = mysql_fetch_row(res);
      mysql_free_result(res);
      // init msg

      msg_info("msg malloc");
      curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG));
      msg_info("msg malloc end");
      strcpy(curmsg->file, "");
      curmsg->flags = 0;

      curmsg->is_bul=1;
      curmsg->bul_id=i;
      curmsg->flags=0;
      strcpy(curmsg->uidl, row[0]);
      strcpy(curmsg->f_flags, "00000000");
      strcpy(curmsg->f_folder, "00");
      strcpy(curmsg->f_opop, "00");
      curmsg->size = atol(row[2]);
      msg_info("bul malloc");
      curmsg->bulletin = (char *) malloc (sizeof(char *) * strlen(row[1])+1);
      msg_info("bul malloc end");
      strcpy(curmsg->bulletin, row[1]);
      
      curmsg->gzipped=0;
      curmsg->readed=0;

      // follow link-list
      if (state->firstmsg == NULL)
          state->firstmsg = curmsg;
      else
          thismsg->nextmsg = curmsg;

      thismsg = curmsg;
      thismsg->nextmsg=NULL;
    }
    
  }
  return 0;
}

int main(int argc, char **argv) {
    static CONFIG_INT_TABLE int_table[] = {
	VAR_RMAIL_MTADB_PORT, DEF_RMAIL_MTADB_PORT, &var_rmail_mtadb_port, 1, 0,
	VAR_RMAIL_AUTDB_PORT, DEF_RMAIL_AUTDB_PORT, &var_rmail_autdb_port, 1, 0,
	VAR_RMAIL_RECDB_PORT, DEF_RMAIL_RECDB_PORT, &var_rmail_recdb_port, 1, 0,
	VAR_RMAIL_CACDB_PORT, DEF_RMAIL_CACDB_PORT, &var_rmail_cacdb_port, 1, 0,
	VAR_RMAIL_BULDB_PORT, DEF_RMAIL_BULDB_PORT, &var_rmail_buldb_port, 1, 0,
	VAR_IMAPD_SOFT_ERLIM, DEF_IMAPD_SOFT_ERLIM, &var_imapd_soft_erlim, 1, 0,
	VAR_RMAIL_MAILCACHE_EXPIRE, DEF_RMAIL_MAILCACHE_EXPIRE, &var_rmail_mailcache_expire, 1, 0,
	0,
    };

    static CONFIG_BOOL_TABLE bool_table[] = {
	VAR_RMAIL_MTADB_INTERACTIVE, DEF_RMAIL_MTADB_INTERACTIVE, &var_rmail_mtadb_interactive,
	VAR_RMAIL_RECDB_INTERACTIVE, DEF_RMAIL_RECDB_INTERACTIVE, &var_rmail_recdb_interactive,
	VAR_RMAIL_AUTDB_INTERACTIVE, DEF_RMAIL_AUTDB_INTERACTIVE, &var_rmail_autdb_interactive,
	VAR_RMAIL_CACDB_INTERACTIVE, DEF_RMAIL_CACDB_INTERACTIVE, &var_rmail_cacdb_interactive,
	VAR_RMAIL_BULDB_INTERACTIVE, DEF_RMAIL_BULDB_INTERACTIVE, &var_rmail_buldb_interactive,
	VAR_RMAIL_MAILLOG_ENABLE, DEF_RMAIL_MAILLOG_ENABLE, &var_rmail_maillog_enable,
	VAR_RMAIL_MAILRECORD_ENABLE, DEF_RMAIL_MAILRECORD_ENABLE, &var_rmail_mailrecord_enable,
	VAR_RMAIL_RCPOP3_ENABLE, DEF_RMAIL_RCPOP3_ENABLE, &var_rmail_rcpop3_enable,
	VAR_RMAIL_MAILCACHE_ENABLE, DEF_RMAIL_MAILCACHE_ENABLE, &var_rmail_mailcache_enable,
	VAR_RMAIL_ALLOW_LOCAL, DEF_RMAIL_ALLOW_LOCAL, &var_rmail_allow_local,
	VAR_RMAIL_POP3PD_ENABLE, DEF_RMAIL_POP3PD_ENABLE, &var_rmail_pop3pd_enable,
	VAR_RMAIL_BULLETIN_ENABLE, DEF_RMAIL_BULLETIN_ENABLE, &var_rmail_bulletin_enable,
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
	
	VAR_RMAIL_BULDB_HOST, DEF_RMAIL_BULDB_HOST, &var_rmail_buldb_host, 1, 0,
	VAR_RMAIL_BULDB_USER, DEF_RMAIL_BULDB_USER, &var_rmail_buldb_user, 1, 0,
	VAR_RMAIL_BULDB_PASS, DEF_RMAIL_BULDB_PASS, &var_rmail_buldb_pass, 1, 0,
	VAR_RMAIL_BULDB_NAME, DEF_RMAIL_BULDB_NAME, &var_rmail_buldb_name, 1, 0,
	
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

	VAR_RMAIL_BULRECORD_TABLE, DEF_RMAIL_BULRECORD_TABLE, &var_rmail_bulrecord_table, 1, 0,
	VAR_RMAIL_BULRECORD_MAILIDFIELD, DEF_RMAIL_BULRECORD_MAILIDFIELD, &var_rmail_bulrecord_mailidfield, 1, 0,
	VAR_RMAIL_BULRECORD_DOMAINFIELD, DEF_RMAIL_BULRECORD_DOMAINFIELD, &var_rmail_bulrecord_domainfield, 1, 0,
	VAR_RMAIL_BULRECORD_RECORDFIELD, DEF_RMAIL_BULRECORD_RECORDFIELD, &var_rmail_bulrecord_recordfield, 1, 0,

	VAR_RMAIL_BULCONTENT_TABLE, DEF_RMAIL_BULCONTENT_TABLE, &var_rmail_bulcontent_table, 1, 0,
	VAR_RMAIL_BULCONTENT_IDFIELD, DEF_RMAIL_BULCONTENT_IDFIELD, &var_rmail_bulcontent_idfield, 1, 0,
	VAR_RMAIL_BULCONTENT_ACTIVEFIELD, DEF_RMAIL_BULCONTENT_ACTIVEFIELD, &var_rmail_bulcontent_activefield, 1, 0,
	VAR_RMAIL_BULCONTENT_UIDLFIELD, DEF_RMAIL_BULCONTENT_UIDLFIELD, &var_rmail_bulcontent_uidlfield, 1, 0,
	VAR_RMAIL_BULCONTENT_TIMEFIELD, DEF_RMAIL_BULCONTENT_TIMEFIELD, &var_rmail_bulcontent_timefield, 1, 0,
	VAR_RMAIL_BULCONTENT_CONTENTFIELD, DEF_RMAIL_BULCONTENT_CONTENTFIELD, &var_rmail_bulcontent_contentfield, 1, 0,
	VAR_RMAIL_FOLDER_TABLE, DEF_RMAIL_FOLDER_TABLE, &var_rmail_folder_table, 1, 0,
	VAR_RMAIL_FOLDER_DOMAINFIELD, DEF_RMAIL_FOLDER_DOMAINFIELD, &var_rmail_folder_domainfield, 1, 0,
	VAR_RMAIL_FOLDER_MAILIDFIELD, DEF_RMAIL_FOLDER_MAILIDFIELD, &var_rmail_folder_mailidfield, 1, 0,
	VAR_RMAIL_FOLDER_NAMEFIELD, DEF_RMAIL_FOLDER_NAMEFIELD, &var_rmail_folder_namefield, 1, 0,
	VAR_RMAIL_FOLDER_FLAGFIELD, DEF_RMAIL_FOLDER_FLAGFIELD, &var_rmail_folder_flagfield, 1, 0,
	
	0,
    };

    static CONFIG_TIME_TABLE time_table[] = {
	VAR_RMAIL_IMAPD_TMOUT, DEF_RMAIL_IMAPD_TMOUT, &var_rmail_imapd_tmout, 1, 0,
	VAR_IMAPD_ERR_SLEEP, DEF_IMAPD_ERR_SLEEP, &var_imapd_err_sleep, 0, 0,
	0,
    };

    single_server_main(argc, argv, imapd_service,
	    MAIL_SERVER_INT_TABLE, int_table,
	    MAIL_SERVER_STR_TABLE, str_table,
	    MAIL_SERVER_BOOL_TABLE, bool_table,
	    MAIL_SERVER_TIME_TABLE, time_table,
	    MAIL_SERVER_PRE_INIT, pre_jail_init,
	    MAIL_SERVER_PRE_ACCEPT, pre_accept,
	    0);
}
