/*++
/* NAME
/*	pop3_stream 3
/* SUMMARY
/*	pop3 stream I/O support
/* SYNOPSIS
/*	#include <pop3_stream.h>
/*
/*	void	pop3_timeout_setup(stream, timeout)
/*	VSTREAM *stream;
/*	int	timeout;
/*
/*	void	pop3_printf(stream, format, ...)
/*	VSTREAM *stream;
/*	const char *format;
/*
/*	void	pop3_flush(stream)
/*	VSTREAM *stream;
/*
/*	int	pop3_get(vp, stream, maxlen)
/*	VSTRING	*vp;
/*	VSTREAM *stream;
/*	int	maxlen;
/*
/*	void	pop3_fputs(str, len, stream)
/*	const char *str;
/*	int	len;
/*	VSTREAM *stream;
/*
/*	void	pop3_fwrite(str, len, stream)
/*	const char *str;
/*	int	len;
/*	VSTREAM *stream;
/*
/*	void	pop3_fputc(ch, stream)
/*	int	ch;
/*	VSTREAM *stream;
/*
/*	void	pop3_vprintf(stream, format, ap)
/*	VSTREAM *stream;
/*	char	*format;
/*	va_list	ap;
/* DESCRIPTION
/*	This module reads and writes text records delimited by CR LF,
/*	with error detection: timeouts or unexpected end-of-file.
/*	A trailing CR LF is added upon writing and removed upon reading.
/*
/*	pop3_timeout_setup() arranges for a time limit on the pop3 read
/*	and write operations described below.
/*	This routine alters the behavior of streams as follows:
/* .IP \(bu
/*	The read/write timeout is set to the specified value.
/* .IP \f(bu
/*	The stream is configured to use double buffering.
/* .IP \f(bu
/*	The stream is configured to enable exception handling.
/* .PP
/*	pop3_printf() formats its arguments and writes the result to
/*	the named stream, followed by a CR LF pair. The stream is NOT flushed.
/*	Long lines of text are not broken.
/*
/*	pop3_flush() flushes the named stream.
/*
/*	pop3_get() reads the named stream up to and including
/*	the next LF character and strips the trailing CR LF. The
/*	\fImaxlen\fR argument limits the length of a line of text,
/*	and protects the program against running out of memory.
/*	Specify a zero bound to turn off bounds checking.
/*	The result is the last character read, or VSTREAM_EOF.
/*
/*	pop3_fputs() writes its string argument to the named stream.
/*	Long strings are not broken. Each string is followed by a
/*	CR LF pair. The stream is not flushed.
/*
/*	pop3_fwrite() writes its string argument to the named stream.
/*	Long strings are not broken. No CR LF is appended. The stream
/*	is not flushed.
/*
/*	pop3_fputc() writes one character to the named stream.
/*	The stream is not flushed.
/*
/*	pop3_vprintf() is the machine underneath pop3_printf().
/* DIAGNOSTICS
/* .fi
/* .ad
/*	In case of error, a vstream_longjmp() call is performed to the
/*	context specified with vstream_setjmp().
/*	Error codes passed along with vstream_longjmp() are:
/* .IP POP3_ERR_EOF
/*	An I/O error happened, or the peer has disconnected unexpectedly.
/* .IP POP3_ERR_TIME
/*	The time limit specified to pop3_timeout_setup() was exceeded.
/* BUGS
/*	The timeout deadline affects all I/O on the named stream, not
/*	just the I/O done on behalf of this module.
/*
/*	The timeout deadline overwrites any previously set up state on
/*	the named stream.
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
#include <sys/socket.h>
#include <sys/time.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>			/* FD_ZERO() needs bzero() prototype */
#include <errno.h>

/* Utility library. */

#include <vstring.h>
#include <vstream.h>
#include <vstring_vstream.h>
#include <msg.h>
#include <iostuff.h>

/* Application-specific. */

#include "pop3_stream.h"

/* pop3_timeout_reset - reset per-stream timeout flag */

static void pop3_timeout_reset(VSTREAM *stream)
{
    vstream_clearerr(stream);
}

/* pop3_timeout_detect - test the per-stream timeout flag */

static void pop3_timeout_detect(VSTREAM *stream)
{
    if (vstream_ftimeout(stream))
	vstream_longjmp(stream, POP3_ERR_TIME);
}

/* pop3_timeout_setup - configure timeout trap */

void    pop3_timeout_setup(VSTREAM *stream, int maxtime)
{
    vstream_control(stream,
		    VSTREAM_CTL_DOUBLE,
		    VSTREAM_CTL_TIMEOUT, maxtime,
		    VSTREAM_CTL_EXCEPT,
		    VSTREAM_CTL_END);
}

/* pop3_flush - flush stream */

void    pop3_flush(VSTREAM *stream)
{
    int     err;

    /*
     * Do the I/O, protected against timeout.
     */
    pop3_timeout_reset(stream);
    err = vstream_fflush(stream);
    pop3_timeout_detect(stream);

    /*
     * See if there was a problem.
     */
    if (err != 0) {
	if (msg_verbose)
	    msg_info("pop3_flush: EOF");
	vstream_longjmp(stream, POP3_ERR_EOF);
    }
}

/* pop3_vprintf - write one line to POP3 peer */

void    pop3_vprintf(VSTREAM *stream, const char *fmt, va_list ap)
{
    int     err;

    /*
     * Do the I/O, protected against timeout.
     */
    pop3_timeout_reset(stream);
    vstream_vfprintf(stream, fmt, ap);
    vstream_fputs("\r\n", stream);
    err = vstream_ferror(stream);
    pop3_timeout_detect(stream);

    /*
     * See if there was a problem.
     */
    if (err != 0) {
	if (msg_verbose)
	    msg_info("pop3_vprintf: EOF");
	vstream_longjmp(stream, POP3_ERR_EOF);
    }
}

/* pop3_printf - write one line to POP3 peer */

void    pop3_printf(VSTREAM *stream, const char *fmt,...)
{
    va_list ap;

    va_start(ap, fmt);
    pop3_vprintf(stream, fmt, ap);
    va_end(ap);
}

/* pop3_get - read one line from POP3 peer */

int     pop3_get(VSTRING *vp, VSTREAM *stream, int bound)
{
    int     last_char;
    int     next_char;

    /*
     * It's painful to do I/O with records that may span multiple buffers.
     * Allow for partial long lines (we will read the remainder later) and
     * allow for lines ending in bare LF. The idea is to be liberal in what
     * we accept, strict in what we send.
     * 
     * XXX 2821: Section 4.1.1.4 says that an POP3 server must not recognize
     * bare LF as record terminator.
     */
    pop3_timeout_reset(stream);
    last_char = (bound == 0 ? vstring_get(vp, stream) :
		 vstring_get_bound(vp, stream, bound));

    switch (last_char) {

	/*
	 * Do some repair in the rare case that we stopped reading in the
	 * middle of the CRLF record terminator.
	 */
    case '\r':
	if ((next_char = VSTREAM_GETC(stream)) == '\n') {
	    VSTRING_ADDCH(vp, '\n');
	    last_char = '\n';
	    /* FALLTRHOUGH */
	} else {
	    if (next_char != VSTREAM_EOF)
		vstream_ungetc(stream, next_char);
	    break;
	}

	/*
	 * Strip off the record terminator: either CRLF or just bare LF.
	 * 
	 * XXX RFC 2821 disallows sending bare CR everywhere. We remove bare CR
	 * if received before CRLF, and leave it alone otherwise.
	 */
    case '\n':
	vstring_truncate(vp, VSTRING_LEN(vp) - 1);
	while (VSTRING_LEN(vp) > 0 && vstring_end(vp)[-1] == '\r')
	    vstring_truncate(vp, VSTRING_LEN(vp) - 1);
	VSTRING_TERMINATE(vp);

	/*
	 * Partial line: just read the remainder later. If we ran into EOF,
	 * the next test will deal with it.
	 */
    default:
	break;
    }
    pop3_timeout_detect(stream);

    /*
     * EOF is bad, whether or not it happens in the middle of a record. Don't
     * allow data that was truncated because of EOF.
     */
    if (vstream_feof(stream) || vstream_ferror(stream)) {
	if (msg_verbose)
	    msg_info("pop3_get: EOF");
	vstream_longjmp(stream, POP3_ERR_EOF);
    }
    return (last_char);
}

/* pop3_fputs - write one line to POP3 peer */

void    pop3_fputs(const char *cp, int todo, VSTREAM *stream)
{
    unsigned err;

    if (todo < 0)
	msg_panic("pop3_fputs: negative todo %d", todo);

    /*
     * Do the I/O, protected against timeout.
     */
    pop3_timeout_reset(stream);
    err = (vstream_fwrite(stream, cp, todo) != todo
	   || vstream_fputs("\r\n", stream) == VSTREAM_EOF);
    pop3_timeout_detect(stream);

    /*
     * See if there was a problem.
     */
    if (err != 0) {
	if (msg_verbose)
	    msg_info("pop3_fputs: EOF");
	vstream_longjmp(stream, POP3_ERR_EOF);
    }
}

/* pop3_fwrite - write one string to POP3 peer */

void    pop3_fwrite(const char *cp, int todo, VSTREAM *stream)
{
    unsigned err;

    if (todo < 0)
	msg_panic("pop3_fwrite: negative todo %d", todo);

    /*
     * Do the I/O, protected against timeout.
     */
    pop3_timeout_reset(stream);
    err = (vstream_fwrite(stream, cp, todo) != todo);
    pop3_timeout_detect(stream);

    /*
     * See if there was a problem.
     */
    if (err != 0) {
	if (msg_verbose)
	    msg_info("pop3_fwrite: EOF");
	vstream_longjmp(stream, POP3_ERR_EOF);
    }
}

/* pop3_fputc - write to POP3 peer */

void    pop3_fputc(int ch, VSTREAM *stream)
{
    int     stat;

    /*
     * Do the I/O, protected against timeout.
     */
    pop3_timeout_reset(stream);
    stat = VSTREAM_PUTC(ch, stream);
    pop3_timeout_detect(stream);

    /*
     * See if there was a problem.
     */
    if (stat == VSTREAM_EOF) {
	if (msg_verbose)
	    msg_info("pop3_fputc: EOF");
	vstream_longjmp(stream, POP3_ERR_EOF);
    }
}
