/*++
/* NAME
/*	imapd_token 3h
/* SUMMARY
/*	tokenize IMAPD command
/* SYNOPSIS
/*	#include <imapd_token.h>
/* DESCRIPTION
/* .nf

 /*
  * Utility library.
  */
#include <vstring.h>

 /*
  * External interface.
  */
typedef struct IMAPD_TOKEN {
    int     tokval;
    char   *strval;
    VSTRING *vstrval;
} IMAPD_TOKEN;

#define IMAPD_TOK_OTHER	0
#define IMAPD_TOK_ADDR	1
#define IMAPD_TOK_ERROR	2

extern int imapd_token(char *, IMAPD_TOKEN **);

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
