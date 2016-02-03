#include <sys_defs.h>
#include <events.h>
#include <mymalloc.h>
#include <vstream.h>
#include <name_mask.h>
#include <msg.h>
#include <stdio.h>

#include <cleanup_user.h>
#include <mail_params.h>
#include <mail_error.h>
#include <mail_proto.h>

#include "imapd.h"

char *strip_leadspace(char *str) {
  const char *myname="strip_leadspace";
  char *ret;
  int lead=1;
  int i=0, j=0;
  

  ret = (char *) malloc (sizeof(char *) * (strlen(str)+1));
  for (i=0; i<strlen(str); i++) {
    if (lead && (str[i]==' ' || str[i]=='(')) {
      // empty @.@
    } else if (str[i] != ' ' && str[i] != '(') {
      lead=0;
      ret[j++] = str[i];
    } else {
      ret[j++] = str[i];
    }
  }
  ret[j++]='\0';
  if (ret[strlen(ret)-1] == ')')
    ret[strlen(ret)-1] = '\0';
  return ret;
}

char *get_folderflag(IMAPD_STATE *state, char *name) {
  const char *myname="get_folderflag";
  IMAPD_FOLDER *curfolder;
  IMAPD_MSG *curmsg;
  char retflags[512];
  char *search;
  int haschildren=0;
  int marked=0;

  search = (char *) malloc (sizeof(char *) * (strlen(name)+2));
  sprintf(search, "%s*", name);
  
  curfolder = (IMAPD_FOLDER *) malloc (sizeof(IMAPD_FOLDER *));
  curfolder = state->firstfolder;

  while (curfolder) {
    if (strcmp(name, curfolder->name)!=0 && wildcardfit(search, curfolder->name)) {
      haschildren = 1;
      break;
    }
    curfolder = curfolder->nextfolder;
  }

  curmsg = (IMAPD_MSG *) malloc (sizeof(IMAPD_MSG *));
  curmsg = state->firstmsg;
  while (curmsg) {
    if (curmsg->f_flags[FLAG_SEEN] != '0') {
      marked=1;
      break;
    }
    curmsg = curmsg->nextmsg;
  }

  free(search);
  sprintf(retflags, "%s%s", 
      (haschildren==1) ? ((marked==1)? "\\Marked ": "\\Unmarked "): "",
      (haschildren==1)? "\\HasChildren": "\\HasNoChildren");
  return retflags;
}

char *strip_quote(char *str) {
  const char *myname="strip_quote";
  char *filtered;
  int i=0, j=0;

  if (strlen(str) == 0)
    return str;

  filtered = (char *) malloc (sizeof(char *) * (strlen(str)+1));
  for (i=0; i<strlen(str); i++) {
    if ((i != 0 || i != strlen(str)-1) && str[i]!='"')
      filtered[j++]=str[i];
  }
  filtered[j++]='\0';
  return filtered;
}

char *strip_taildot(char *str) {
  const char *myname="strip_taildot";
  static char *filtered;

  if (strlen(str) ==0)
    return str;
  filtered = (char *) malloc (sizeof(char *) * (strlen(str)+1));
  strcpy(filtered, str);
  if (filtered[strlen(filtered)-1] == '.')
    filtered[strlen(filtered)-1]='\0';
  return filtered;
}

char *strtoupper(char *str) {
  const char *myname="strtoupper";
  static char *filtered;
  static int i=0;
  filtered = (char *) malloc (sizeof(char *) * (strlen(str)+1));
  strcpy(filtered, str);
  for (i=0; i<strlen(str); i++) {
    filtered[i] = toupper(str[i]);
  }
  return filtered;
}

char *strtolower(char *str) {
  const char *myname="strtoupper";
  static char *filtered;
  static int i=0;
  filtered = (char *) malloc (sizeof(char *) * (strlen(str)+1));
  strcpy(filtered, str);
  for (i=0; i<strlen(str); i++) {
    filtered[i] = tolower(str[i]);
  }
  return filtered;
}


int asterisk(char **wildcard, char **test) {
  int fit=1;

  (*wildcard)++;
  while (('\000' != (**test))
      && ('*' == **wildcard)) {
    (*wildcard)++;
  }

  while ('*' == (**wildcard))
    (*wildcard)++;

  if (('\0' == (**test)) && ('\0' != (**wildcard)))
    return (fit=0);
  if (('\0' == (**test)) && ('\0' == (**wildcard)))
    return (fit=1);
  else {
    if (0 == wildcardfit(*wildcard, (*test))) {
      do {
	(*test)++;
	while (((**wildcard)!= (**test))
	    && ('\0' != (**test)))
	  (*test)++;
      } while ((('\0' != **test)) ? (0 == wildcardfit(*wildcard, (*test))) : ( 0 != (fit = 0)));
    }
    if (('\0' == **test) && ('\0' == **wildcard))
      fit=1;
    return (fit);
  }
}


int wildcardfit(char *wildcard, char *test) {
  int fit=1;
  for ( ; ('\000' != *wildcard) && (1 == fit) && ('\000' != *test); wildcard++) {
    switch (*wildcard) {
      case '*':
	fit = asterisk(&wildcard, &test);
	wildcard--;
	break;
      default:
	fit = (int) (*wildcard == *test);
	test++;
    }
  }
  while ((*wildcard == '*') && (1==fit))
    wildcard++;
  return (int)((1==fit) && ('\0' == *test) && ('\0' == *wildcard));
}

