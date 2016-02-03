#ifndef _IMAP_STREAM_H_INCLUDED_
#define _IMAP_STREAM_H_INCLUDED_

/*++
/* NAME
/*	imap_stream 3h
/* SUMMARY
/*	imap stream I/O support
/* SYNOPSIS
/*	#include <imap_stream.h>
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
#define IMAP_ERR_EOF	1		/* unexpected client disconnect */
#define IMAP_ERR_TIME	2		/* time out */

extern void imap_timeout_setup(VSTREAM *, int);
extern void PRINTFLIKE(2, 3) imap_printf(VSTREAM *, const char *,...);
extern void imap_flush(VSTREAM *);
extern int imap_get(VSTRING *, VSTREAM *, int);
extern void imap_fputs(const char *, int len, VSTREAM *);
extern void imap_fwrite(const char *, int len, VSTREAM *);
extern void imap_fputc(int, VSTREAM *);

extern void imap_vprintf(VSTREAM *, const char *, va_list);

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
