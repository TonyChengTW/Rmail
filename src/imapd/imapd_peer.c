/*++
/* NAME
/*	imapd_peer 3
/* SUMMARY
/*	look up peer name/address information
/* SYNOPSIS
/*	#include "imapd.h"
/*
/*	void	imapd_peer_init(state)
/*	IMAPD_STATE *state;
/*
/*	void	imapd_peer_reset(state)
/*	IMAPD_STATE *state;
/* DESCRIPTION
/*	The imapd_peer_init() routine attempts to produce a printable
/*	version of the peer name and address of the specified socket.
/*	Where information is unavailable, the name and/or address
/*	are set to "unknown".
/*
/*	imapd_peer_init() updates the following fields:
/* .IP name
/*	The client hostname. An unknown name is represented by the
/*	string "unknown".
/* .IP addr
/*	Printable representation of the client address.
/* .IP namaddr
/*	String of the form: "name[addr]".
/* .IP peer_code
/*	The peer_code result field specifies how the client name
/*	information should be interpreted:
/* .RS
/* .IP 2
/*	Both name lookup and name verification succeeded.
/* .IP 4
/*	The name lookup or name verification failed with a recoverable
/*	error (no address->name mapping or no name->address mapping).
/* .IP 5
/*	The name lookup or verification failed with an unrecoverable
/*	error (no address->name mapping, bad hostname syntax, no
/*	name->address mapping, client address not listed for hostname).
/* .RE
/* .PP
/*	imapd_peer_reset() releases memory allocated by imapd_peer_init().
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>			/* strerror() */
#include <errno.h>
#include <netdb.h>
#include <string.h>

 /*
  * Older systems don't have h_errno. Even modern systems don't have
  * hstrerror().
  */
#ifdef NO_HERRNO

static int h_errno = TRY_AGAIN;

#define  HSTRERROR(err) "Host not found"

#else

#define  HSTRERROR(err) (\
	err == TRY_AGAIN ? "Host not found, try again" : \
	err == HOST_NOT_FOUND ? "Host not found" : \
	err == NO_DATA ? "Host name has no address" : \
	err == NO_RECOVERY ? "Name server failure" : \
	strerror(errno) \
    )
#endif

/* Utility library. */

#include <msg.h>
#include <mymalloc.h>
#include <valid_hostname.h>
#include <stringops.h>

/* Global library. */

#include <mail_proto.h>

/* Application-specific. */

#include "imapd.h"

/* imapd_peer_init - initialize peer information */

void    imapd_peer_init(IMAPD_STATE *state)
{
    struct sockaddr_in sin;
    SOCKADDR_SIZE len = sizeof(sin);
    struct hostent *hp;
    int     i;

    /*
     * Avoid suprious complaints from Purify on Solaris.
     */
    memset((char *) &sin, 0, len);

    /*
     * Look up the peer address information.
     */
    if (getpeername(vstream_fileno(state->client),
		    (struct sockaddr *) & sin, &len) >= 0) {
	errno = 0;
    }

    /*
     * If peer went away, give up.
     */
    if (errno == ECONNRESET || errno == ECONNABORTED) {
	state->name = mystrdup(CLIENT_NAME_UNKNOWN);
	state->addr = mystrdup(CLIENT_ADDR_UNKNOWN);
	state->peer_code = IMAPD_PEER_CODE_PERM;
    }

    /*
     * Look up and "verify" the client hostname.
     */
    else if (errno == 0 && sin.sin_family == AF_INET) {
	state->addr = mystrdup(inet_ntoa(sin.sin_addr));
	hp = gethostbyaddr((char *) &(sin.sin_addr),
			   sizeof(sin.sin_addr), AF_INET);
	if (hp == 0) {
	    state->name = mystrdup(CLIENT_NAME_UNKNOWN);
	    state->peer_code = (h_errno == TRY_AGAIN ?
				IMAPD_PEER_CODE_TEMP : IMAPD_PEER_CODE_PERM);
	} else if (valid_hostaddr(hp->h_name, DONT_GRIPE)) {
	    msg_warn("numeric result %s in address->name lookup for %s",
		     hp->h_name, state->addr);
	    state->name = mystrdup(CLIENT_NAME_UNKNOWN);
	    state->peer_code = IMAPD_PEER_CODE_PERM;
	} else if (!valid_hostname(hp->h_name, DONT_GRIPE)) {
	    state->name = mystrdup(CLIENT_NAME_UNKNOWN);
	    state->peer_code = IMAPD_PEER_CODE_PERM;
	} else {
	    state->name = mystrdup(hp->h_name);	/* hp->name is clobbered!! */
	    state->peer_code = IMAPD_PEER_CODE_OK;

	    /*
	     * Reject the hostname if it does not list the peer address.
	     */
#define REJECT_PEER_NAME(state, code) { \
	myfree(state->name); \
	state->name = mystrdup(CLIENT_NAME_UNKNOWN); \
	state->peer_code = code; \
    }

	    hp = gethostbyname(state->name);	/* clobbers hp->name!! */
	    if (hp == 0) {
		msg_warn("%s: hostname %s verification failed: %s",
			 state->addr, state->name, HSTRERROR(h_errno));
		REJECT_PEER_NAME(state, (h_errno == TRY_AGAIN ?
			      IMAPD_PEER_CODE_TEMP : IMAPD_PEER_CODE_PERM));
	    } else if (hp->h_length != sizeof(sin.sin_addr)) {
		msg_warn("%s: hostname %s verification failed: bad address size %d",
			 state->addr, state->name, hp->h_length);
		REJECT_PEER_NAME(state, IMAPD_PEER_CODE_PERM);
	    } else {
		for (i = 0; /* void */ ; i++) {
		    if (hp->h_addr_list[i] == 0) {
			msg_warn("%s: address not listed for hostname %s",
				 state->addr, state->name);
			REJECT_PEER_NAME(state, IMAPD_PEER_CODE_PERM);
			break;
		    }
		    if (memcmp(hp->h_addr_list[i],
			       (char *) &sin.sin_addr,
			       sizeof(sin.sin_addr)) == 0)
			break;			/* keep peer name */
		}
	    }
	}
    }

    /*
     * If it's not Internet, assume the client is local, and avoid using the
     * naming service because that can hang when the machine is disconnected.
     */
    else {
	state->name = mystrdup("localhost");
	state->addr = mystrdup("127.0.0.1");	/* XXX bogus. */
	state->peer_code = IMAPD_PEER_CODE_OK;
    }

    /*
     * Do the name[addr] formatting for pretty reports.
     */
    state->namaddr =
	concatenate(state->name, "[", state->addr, "]", (char *) 0);
}

/* imapd_peer_reset - destroy peer information */

void    imapd_peer_reset(IMAPD_STATE *state)
{
    myfree(state->name);
    myfree(state->addr);
    myfree(state->namaddr);
}
