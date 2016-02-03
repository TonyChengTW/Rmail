/*++
/* NAME
/*	pop3d_token 3h
/* SUMMARY
/*	tokenize POP3D command
/* SYNOPSIS
/*	#include <pop3d_token.h>
/* DESCRIPTION
/* .nf

 /*
  * Utility library.
  */
#include <vstring.h>

 /*
  * External interface.
  */
typedef struct POP3D_TOKEN {
    int     tokval;
    char   *strval;
    VSTRING *vstrval;
} POP3D_TOKEN;

#define POP3D_TOK_OTHER	0
#define POP3D_TOK_ADDR	1
#define POP3D_TOK_ERROR	2

extern int pop3d_token(char *, POP3D_TOKEN **);

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
