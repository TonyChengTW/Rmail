/*++
/* NAME
/*	pop3d_chat 3h
/* SUMMARY
/*	SMTP server request/response support
/* SYNOPSIS
/*	#include <pop3d.h>
/*	#include <pop3d_chat.h>
/* DESCRIPTION
/* .nf

 /*
  * External interface.
  */
extern void pop3d_chat_reset(POP3D_STATE *);
extern void pop3d_chat_query(POP3D_STATE *);
extern void PRINTFLIKE(2, 3) pop3d_chat_reply(POP3D_STATE *, char *, ...);
extern void pop3d_chat_notify(POP3D_STATE *);

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

