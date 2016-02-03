#ifndef _POP3_STREAM_H_INCLUDED_
#define _POP3_STREAM_H_INCLUDED_

/*++
/* NAME
/*	pop3_stream 3h
/* SUMMARY
/*	pop3 stream I/O support
/* SYNOPSIS
/*	#include <pop3_stream.h>
/* DESCRIPTION
/* .nf

 /*
  * System library.
  */
#include <stdarg.h>
#include <setjmp.h>

 /*
  * Utility library.
  */
#include <vstring.h>
#include <vstream.h>

 /*
  * External interface.
  */
#define POP3_ERR_EOF	1		/* unexpected client disconnect */
#define POP3_ERR_TIME	2		/* time out */

extern void pop3_timeout_setup(VSTREAM *, int);
extern void PRINTFLIKE(2, 3) pop3_printf(VSTREAM *, const char *,...);
extern void pop3_flush(VSTREAM *);
extern int pop3_get(VSTRING *, VSTREAM *, int);
extern void pop3_fputs(const char *, int len, VSTREAM *);
extern void pop3_fwrite(const char *, int len, VSTREAM *);
extern void pop3_fputc(int, VSTREAM *);

extern void pop3_vprintf(VSTREAM *, const char *, va_list);

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

#endif
