/*++
/* NAME
/*	pop3d_chat 3
/* SUMMARY
/*	POP3 server request/response support
/* SYNOPSIS
/*	#include <pop3d.h>
/*	#include <pop3d_chat.h>
/*
/*	void	pop3d_chat_query(state)
/*	POP3D_STATE *state;
/*
/*	void	pop3d_chat_reply(state, format, ...)
/*	POP3D_STATE *state;
/*	char	*format;
/*
/*	void	pop3d_chat_notify(state)
/*	POP3D_STATE *state;
/*
/*	void	pop3d_chat_reset(state)
/*	POP3D_STATE *state;
/* DESCRIPTION
/*	This module implements POP3 server support for request/reply
/*	conversations, and maintains a limited POP3 transaction log.
/*
/*	pop3d_chat_query() receives a client request and appends a copy
/*	to the POP3 transaction log.
/*
/*	pop3d_chat_reply() formats a server reply, sends it to the
/*	client, and appends a copy to the POP3 transaction log.
/*	When soft_bounce is enabled, all 5xx (reject) reponses are
/*	replaced by 4xx (try again).
/*
/*	pop3d_chat_notify() sends a copy of the POP3 transaction log
/*	to the postmaster for review. The postmaster notice is sent only
/*	when delivery is possible immediately. It is an error to call
/*	pop3d_chat_notify() when no POP3 transaction log exists.
/*
/*	pop3d_chat_reset() resets the transaction log. This is
/*	typically done at the beginning of an POP3 session, or
/*	within a session to discard non-error information.
/* DIAGNOSTICS
/*	Panic: interface violations. Fatal errors: out of memory.
/*	internal protocol errors.
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
#include <setjmp.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>			/* 44BSD stdarg.h uses abort() */
#include <stdarg.h>

/* Utility library. */

#include <msg.h>
#include <argv.h>
#include <vstring.h>
#include <vstream.h>
#include <stringops.h>
#include <line_wrap.h>
#include <mymalloc.h>

/* Global library. */

#include <pop3_stream.h>
#include <record.h>
#include <rec_type.h>
#include <mail_proto.h>
#include <mail_params.h>
#include <mail_addr.h>
#include <post_mail.h>
#include <mail_error.h>

/* Application-specific. */

#include "pop3d.h"
#include "pop3d_chat.h"

#define STR	vstring_str
#define LEN	VSTRING_LEN

/* pop3_chat_reset - reset POP3 transaction log */

void    pop3d_chat_reset(POP3D_STATE *state)
{
    if (state->history) {
	argv_free(state->history);
	state->history = 0;
    }
}

/* pop3_chat_append - append record to POP3 transaction log */

static void pop3_chat_append(POP3D_STATE *state, char *direction)
{
    char   *line;

    /*
    if (state->notify_mask == 0)
	return;
    */

    if (state->history == 0)
	state->history = argv_alloc(10);
    line = concatenate(direction, STR(state->buffer), (char *) 0);
    argv_add(state->history, line, (char *) 0);
    myfree(line);
}

/* pop3d_chat_query - receive and record an POP3 request */

void    pop3d_chat_query(POP3D_STATE *state)
{
    int     last_char;

    last_char = pop3_get(state->buffer, state->client, var_line_limit);
    pop3_chat_append(state, "In:  ");
    if (last_char != '\n')
	msg_warn("%s[%s]: request longer than %d: %.30s...",
		 state->name, state->addr, var_line_limit,
		 printable(STR(state->buffer), '?'));

    if (msg_verbose)
	msg_info("< %s[%s]: %s", state->name, state->addr, STR(state->buffer));
}

/* pop3d_chat_reply - format, send and record an POP3 response */

void    pop3d_chat_reply(POP3D_STATE *state, char *format,...)
{
    va_list ap;
    int     delay = 0;

    va_start(ap, format);
    vstring_vsprintf(state->buffer, format, ap);
    va_end(ap);
    if (var_soft_bounce && STR(state->buffer)[0] == '5')
	STR(state->buffer)[0] = '4';
    pop3_chat_append(state, "Out: ");

    if (msg_verbose)
	msg_info("> %s[%s]: %s", state->name, state->addr, STR(state->buffer));

    /*
     * Slow down clients that make errors. Sleep-on-anything slows down
     * clients that make an excessive number of errors within a session.
     */
    if (state->error_count >= var_pop3d_soft_erlim)
	sleep(delay = var_pop3d_err_sleep);

    pop3_fputs(STR(state->buffer), LEN(state->buffer), state->client);

    /*
     * Flush unsent output if no I/O happened for a while. This avoids
     * timeouts with pipelined POP3 sessions that have lots of server-side
     * delays (tarpit delays or DNS lookups for UCE restrictions).
     */
    if (delay || time((time_t *) 0) - vstream_ftime(state->client) > 10)
	vstream_fflush(state->client);

    /*
     * Abort immediately if the connection is broken.
     */
    if (vstream_ftimeout(state->client))
	vstream_longjmp(state->client, POP3_ERR_TIME);
    if (vstream_ferror(state->client))
	vstream_longjmp(state->client, POP3_ERR_EOF);
}

/* print_line - line_wrap callback */

static void print_line(const char *str, int len, int indent, char *context)
{
    VSTREAM *notice = (VSTREAM *) context;

    post_mail_fprintf(notice, " %*s%.*s", indent, "", len, str);
}

/* pop3d_chat_notify - notify postmaster */

void    pop3d_chat_notify(POP3D_STATE *state)
{
  /*
    char   *myname = "pop3d_chat_notify";
    VSTREAM *notice;
    char  **cpp;
  */
    /*
     * Sanity checks.
     */
  /*
    if (state->history == 0)
	msg_panic("%s: no conversation history", myname);
    if (msg_verbose)
	msg_info("%s: notify postmaster", myname);
  */
    /*
     * Construct a message for the postmaster, explaining what this is all
     * about. This is junk mail: don't send it when the mail posting service
     * is unavailable, and use the double bounce sender address to prevent
     * mail bounce wars. Always prepend one space to message content that we
     * generate from untrusted data.
     */
#define NULL_TRACE_FLAGS	0
#define LENGTH	78
#define INDENT	4

  /*
    notice = post_mail_fopen_nowait(mail_addr_double_bounce(),
				    var_error_rcpt,
				    CLEANUP_FLAG_MASK_INTERNAL,
				    NULL_TRACE_FLAGS);
    if (notice == 0) {
	msg_warn("postmaster notify: %m");
	return;
    }
    post_mail_fprintf(notice, "From: %s (Mail Delivery System)",
		      mail_addr_mail_daemon());
    post_mail_fprintf(notice, "To: %s (Postmaster)", var_error_rcpt);
    post_mail_fprintf(notice, "Subject: %s POP3 server: errors from %s[%s]",
		      var_mail_name, state->name, state->addr);
    post_mail_fputs(notice, "");
    post_mail_fputs(notice, "Transcript of session follows.");
    post_mail_fputs(notice, "");
    argv_terminate(state->history);
    for (cpp = state->history->argv; *cpp; cpp++)
	line_wrap(printable(*cpp, '?'), LENGTH, INDENT, print_line,
		  (char *) notice);
    post_mail_fputs(notice, "");
    if (state->reason)
	post_mail_fprintf(notice, "Session aborted, reason: %s", state->reason);
    (void) post_mail_fclose(notice);
  */
}
