/*++
/* NAME
/*	imapd_chat 3h
/* SUMMARY
/*	SMTP server request/response support
/* SYNOPSIS
/*	#include <imapd.h>
/*	#include <imapd_chat.h>
/* DESCRIPTION
/* .nf

 /*
  * External interface.
  */
extern void imapd_chat_reset(IMAPD_STATE *);
extern void imapd_chat_query(IMAPD_STATE *);
extern void PRINTFLIKE(2, 3) imapd_chat_reply(IMAPD_STATE *, char *, ...);
extern void imapd_chat_notify(IMAPD_STATE *);

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

