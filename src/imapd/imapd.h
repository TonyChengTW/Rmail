#include <unistd.h>
#include <vstream.h>
#include <vstring.h>
#include <argv.h>

#include <mail_stream.h>
#include <mysql.h>

/* Pre-connection dbh */
extern MYSQL mysql_dbh;
extern MYSQL pass_dbh;
extern int used_count;


typedef struct IMAPD_MSG {
    unsigned long size;
    unsigned long lines;
    int flags;
    char uidl[70];
    char file[1024];
    char subject[1024];
    char from[1024];
    time_t sendtime;
    
    char f_flags[9];
    char f_folder[3];
    char f_opop[3];
    int readed;
    
    /* gzipped? */
    int gzipped;
    
    /* For edm? */
    int is_bul;
    int bul_id;
    char *bulletin;
    struct IMAPD_MSG *nextmsg;
} IMAPD_MSG;

typedef struct IMAPD_FOLDER {
  char	flag[3];
  char	name[255];
  struct IMAPD_FOLDER *nextfolder;
} IMAPD_FOLDER;

typedef struct IMAPD_STATE {
    int	    err;
    VSTREAM *client;
    VSTRING *buffer;
    time_t  time;
    int	    peer_code;
    
    char    *reason;
    char    *where;

    
    char    *name;
    char    *addr;
    char    *namaddr;

    int	    error_count;
    ARGV    *history;

    char    *userid;
    int	    domain;
    char    *passwd;
    char    *mdir;
    int	    authed;
    unsigned long bulletin_record;
    
    unsigned int quota;
    unsigned int retr_count;
    unsigned long retr_size;
    unsigned long dele_size;
    IMAPD_MSG *firstmsg;
    IMAPD_FOLDER *firstfolder;
    char curflag[3];
} IMAPD_STATE;

#define IMAPD_AFTER_CONNECT	"CONNECT"
#define IMAPD_AFTER_DOT	    "END-OF-MESSAGE"

#define IMAPD_STAT_OK	'K'
#define IMAPD_STAT_FAIL	'F'

#define IMAPD_STAND_ALONE(state) \
    (state->client == VSTREAM_IN && getuid() != var_owner_uid)

extern void imapd_state_init(IMAPD_STATE *, VSTREAM *);
extern void imapd_state_reset(IMAPD_STATE *);

void imapd_peer_init(IMAPD_STATE *state);
void imapd_peer_reset(IMAPD_STATE *state);



// imapd_util.c
extern char *strip_quote(char *str);
extern char *strip_taildot(char *str);
extern char *strtoupper(char *str);
extern char *strtolower(char *str);
extern int wildcardfit(char *wildcard, char *test);
extern char *get_folderflag(IMAPD_STATE *state, char *name);
extern char *strip_leadspace(char *str);


#define CLIENT_ATTR_UNKNOWN     "unknown"

#define CLIENT_NAME_UNKNOWN     CLIENT_ATTR_UNKNOWN
#define CLIENT_ADDR_UNKNOWN     CLIENT_ATTR_UNKNOWN
#define CLIENT_NAMADDR_UNKNOWN  CLIENT_ATTR_UNKNOWN
#define CLIENT_HELO_UNKNOWN     0
#define CLIENT_PROTO_UNKNOWN    CLIENT_ATTR_UNKNOWN
#define CLIENT_IDENT_UNKNOWN    0

#define IMAPD_PEER_CODE_OK      2
#define IMAPD_PEER_CODE_TEMP    4
#define IMAPD_PEER_CODE_PERM    5

#define FLAG_SEEN   0
#define FLAG_RECENT 1
#define FLAG_FLAGGED  2
#define FLAG_ANSWERED 3
#define FLAG_DRAFT  4
#define FLAG_DELETED  5
#define FLAG_REV1 6
#define FLAG_REV2 7
