


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


typedef struct POP3D_MSG {
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
    struct POP3D_MSG *nextmsg;
} POP3D_MSG;

typedef struct POP3D_STATE {
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
    POP3D_MSG *firstmsg;
} POP3D_STATE;

#define POP3D_AFTER_CONNECT	"CONNECT"
#define POP3D_AFTER_DOT	    "END-OF-MESSAGE"

#define POP3D_STAT_OK	'K'
#define POP3D_STAT_FAIL	'F'

#define POP3D_STAND_ALONE(state) \
    (state->client == VSTREAM_IN && getuid() != var_owner_uid)

extern void pop3d_state_init(POP3D_STATE *, VSTREAM *);
extern void pop3d_state_reset(POP3D_STATE *);

void pop3d_peer_init(POP3D_STATE *state);
void pop3d_peer_reset(POP3D_STATE *state);

#define CLIENT_ATTR_UNKNOWN     "unknown"

#define CLIENT_NAME_UNKNOWN     CLIENT_ATTR_UNKNOWN
#define CLIENT_ADDR_UNKNOWN     CLIENT_ATTR_UNKNOWN
#define CLIENT_NAMADDR_UNKNOWN  CLIENT_ATTR_UNKNOWN
#define CLIENT_HELO_UNKNOWN     0
#define CLIENT_PROTO_UNKNOWN    CLIENT_ATTR_UNKNOWN
#define CLIENT_IDENT_UNKNOWN    0

#define POP3D_PEER_CODE_OK      2
#define POP3D_PEER_CODE_TEMP    4
#define POP3D_PEER_CODE_PERM    5

