#include <sys_defs.h>
#include <events.h>
#include <mymalloc.h>
#include <vstream.h>
#include <name_mask.h>
#include <msg.h>

#include <cleanup_user.h>
#include <mail_params.h>
#include <mail_error.h>
#include <mail_proto.h>

#include "imapd.h"
// #include "imapd_chat.h"


void imapd_state_init(IMAPD_STATE *state, VSTREAM *stream) {

    state->err = IMAPD_STAT_OK;
    state->client = stream;
    state->buffer = vstring_alloc(100);

    state->time = time((time_t *) 0);
    state->error_count = 0;
    state->history = 0;
    state->domain = 0;
    state->authed = 0;
    state->retr_count = 0;
    state->retr_size = 0;
    state->dele_size = 0;
    state->peer_code = 0;

    state->userid = 0;
    state->passwd = 0;
    state->mdir = 0;
    state->reason = 0;
    state->where = 0;
    state->dele_size=0;
    state->firstmsg=NULL;
    state->firstfolder=NULL;

    imapd_peer_init(state);
//    imapd_chat_reset(state);

    
}

void imapd_state_reset(IMAPD_STATE *state) {
    if (state->buffer)
	vstring_free(state->buffer);
}
