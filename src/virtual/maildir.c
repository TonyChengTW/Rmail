/*++
/* NAME
/*	maildir 3
/* SUMMARY
/*	delivery to maildir
/* SYNOPSIS
/*	#include "virtual.h"
/*
/*	int	deliver_maildir(state, usr_attr)
/*	LOCAL_STATE state;
/*	USER_ATTR usr_attr;
/* DESCRIPTION
/*	deliver_maildir() delivers a message to a qmail-style maildir.
/*
/*	Arguments:
/* .IP state
/*	The attributes that specify the message, recipient and more.
/* .IP usr_attr
/*	Attributes describing user rights and environment information.
/* DIAGNOSTICS
/*	deliver_maildir() always succeeds or it bounces the message.
/* SEE ALSO
/*	bounce(3)
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

#include "sys_defs.h"
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#ifndef EDQUOT
#define EDQUOT EFBIG
#endif

/* Utility library. */

#include <msg.h>
#include <mymalloc.h>
#include <stringops.h>
#include <vstream.h>
#include <vstring.h>
#include <make_dirs.h>
#include <set_eugid.h>
#include <get_hostname.h>
#include <sane_fsops.h>

/* Global library. */

#include <mail_copy.h>
#include <bounce.h>
#include <defer.h>
#include <sent.h>
#include <mail_params.h>

/* Application-specific. */

#include "virtual.h"

/* Rmail */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <zlib.h>

/* Rmail end */

/* deliver_maildir - delivery to maildir-style mailbox */

int     deliver_maildir(LOCAL_STATE state, USER_ATTR usr_attr)
{
    char   *myname = "deliver_maildir";
    char   *newdir;
    char   *tmpdir;
    char   *curdir;
    char   *tmpfile;
    char   *newfile;
    VSTRING *why;
    VSTRING *buf;
    VSTREAM *dst;
    int     mail_copy_status;
    int     deliver_status;
    int     copy_flags;
    struct stat st;
    time_t  starttime = time((time_t *) 0);
    /* Rmail */
    char *zipfile;
    int bulk_count=0;
    long cur_quota=1;
    long cur_sized=0;
    long tmp_sized=0;
    long bulk_size=0;
    bool update_tmp=1;
    bool quota_exceed=0;
    struct stat fptr;
    DIR *dirp;
    struct dirent *dire;
    char filename[128];
    FILE *fp;
    gzFile *gfp;
    char *qstr;
    char buff[1024];
    int fts, fpid;
    char loc[4], flags[9], fdr[3], opop[3], hn[128];
    char *gzinfo;
    /* Rmail end */

    /*
     * Make verbose logging easier to understand.
     */
    state.level++;
    if (msg_verbose)
	MSG_LOG_STATE(myname, state);

    /* Rmail */
    bulk_size = state.request->data_size;

    /* Get quota */
    qstr = concatenate(usr_attr.mailbox, "../", var_rmail_quota_file, (char *) 0);
    if (fp = fopen(qstr, "r")) {
      fscanf(fp, "%ld", &cur_quota);
      fclose(fp);
    }

    cur_quota = (cur_quota != 1) ? cur_quota : var_rmail_default_quota;
    if (cur_quota == 0)
      usr_attr.check_quota = 0;

    if (msg_debug)
      msg_info("%s: Got quota of %s: %ld, check_quota: %d",
	  myname, state.msg_attr.user, cur_quota, usr_attr.check_quota);

    /* check quota-tmp and determine if we have to update it */
    qstr = concatenate(usr_attr.mailbox, "../", var_rmail_quota_tmpfile, (char *) 0);
    if (fp = fopen(qstr, "r")) {
      fscanf(fp, "%lu", &tmp_sized);
      if (fstat(fileno(fp), &fptr)==0) {
	if (time(NULL) - fptr.st_mtime < var_rmail_quota_expire) {
	  update_tmp=0;
	} else {
	  update_tmp=1;
	}
      }
      fclose(fp);
    } else {
      update_tmp=1;
      tmp_sized=0;
    }

    if (usr_attr.check_quota == 0)
      update_tmp=0;
    
    if (update_tmp) {
      newdir = concatenate(usr_attr.mailbox, "new/", (char *) 0);
      
      if (dirp = opendir(newdir)) { // opendir 
	while (dire = readdir(dirp)) {
	  // skip . & ..
	  if (strcmp(dire->d_name, ".") == 0 || strcmp(dire->d_name, "..") == 0)
	    continue;

	  sprintf(filename, "%s/%s", newdir, dire->d_name);
	  if (dire->d_name[strlen(dire->d_name)-2] == 'g' &&
	      dire->d_name[strlen(dire->d_name)-1] == 'z') { // is gzipped file
	    
	    gzinfo = strrchr(filename, '.');
	    gzinfo++;
	    gzinfo[strlen(gzinfo)-2]='\0';

	    cur_sized = cur_sized + atoi(gzinfo);
	    
	    if (msg_debug)
	      msg_info("%s: Procrssing gzipped %s(%lu)", myname, dire->d_name, atoi(gzinfo));
	  } else { // is normal file
	    stat(filename, &fptr);
	    cur_sized = cur_sized + fptr.st_size;
	    if (msg_debug)
	      msg_info("%s: Processing plain %s %lu", myname, dire->d_name, fptr.st_size);
	  }

	} // readdir end
	if (msg_debug)
	  msg_info("%s: Got %s sized %lu", myname, newdir, cur_sized);
	closedir(dirp);
      } else { // opendir fail
	msg_warn("%s: Cannot open %s: %m", myname, newdir);
      }

      if (fp=fopen(qstr, "w")) {
	fprintf(fp, "%lu", cur_sized);
	fclose(fp);
      }
    } else { // no need to update tmp file
      cur_sized = tmp_sized;
    }

    // check if quota exceeded
    if (cur_sized + bulk_size > cur_quota)
      quota_exceed = 1;

    
    if (msg_verbose)
      msg_info("%s: update_tmp=%d, cur_sized=%ld, bulk_size=%ld, cur_quota=%ld, quota_exceed=%d",
	  myname, update_tmp, cur_sized, bulk_size, cur_quota, quota_exceed);

    

    /*
     * Don't deliver trace-only requests.
     */
    if (DEL_REQ_TRACE_ONLY(state.request->flags))
	return (sent(BOUNCE_FLAGS(state.request), SENT_ATTR(state.msg_attr),
		     "delivers to maildir"));

    /*
     * Initialize. Assume the operation will fail. Set the delivered
     * attribute to reflect the final recipient.
     */
    if (vstream_fseek(state.msg_attr.fp, state.msg_attr.offset, SEEK_SET) < 0)
	msg_fatal("seek message file %s: %m", VSTREAM_PATH(state.msg_attr.fp));
    state.msg_attr.delivered = state.msg_attr.recipient;
    mail_copy_status = MAIL_COPY_STAT_WRITE;
    buf = vstring_alloc(100);
    why = vstring_alloc(100);

    copy_flags = MAIL_COPY_TOFILE | MAIL_COPY_RETURN_PATH
	| MAIL_COPY_DELIVERED | MAIL_COPY_ORIG_RCPT;

    newdir = concatenate(usr_attr.mailbox, "new/", (char *) 0);
    tmpdir = concatenate(usr_attr.mailbox, "tmp/", (char *) 0);
    //curdir = concatenate(usr_attr.mailbox, "cur/", (char *) 0);
    // Dirty hack, we don't neew cur/
    curdir = concatenate(usr_attr.mailbox, "new/", (char *) 0);


    /*
     * Create and write the file as the recipient, so that file quota work.
     * Create any missing directories on the fly. The file name is chosen
     * according to ftp://koobera.math.uic.edu/www/proto/maildir.html:
     * 
     * "A unique name has three pieces, separated by dots. On the left is the
     * result of time(). On the right is the result of gethostname(). In the
     * middle is something that doesn't repeat within one second on a single
     * host. I fork a new process for each delivery, so I just use the
     * process ID. If you're delivering several messages from one process,
     * use starttime.pid_count.host, where starttime is the time that your
     * process started, and count is the number of messages you've
     * delivered."
     * 
     * Well, that stopped working on fast machines, and on operating systems
     * that randomize process ID values. When creating a file in tmp/ we use
     * the process ID because it still is an exclusive resource. When moving
     * the file to new/ we use the device number and inode number. I do not
     * care if this breaks on a remote AFS file system, because people should
     * know better.
     * 
     * On January 26, 2003, http://cr.yp.to/proto/maildir.html said:
     * 
     * A unique name has three pieces, separated by dots. On the left is the
     * result of time() or the second counter from gettimeofday(). On the
     * right is the result of gethostname(). (To deal with invalid host
     * names, replace / with \057 and : with \072.) In the middle is a
     * delivery identifier, discussed below.
     * 
     * [...]
     * 
     * Modern delivery identifiers are created by concatenating enough of the
     * following strings to guarantee uniqueness:
     * 
     * [...]
     * 
     * In, where n is (in hexadecimal) the UNIX inode number of this file.
     * Unfortunately, inode numbers aren't always available through NFS.
     * 
     * Vn, where n is (in hexadecimal) the UNIX device number of this file.
     * Unfortunately, device numbers aren't always available through NFS.
     * (Device numbers are also not helpful with the standard UNIX
     * filesystem: a maildir has to be within a single UNIX device for link()
     * and rename() to work.)
     * 
     * [...]
     * 
     * Pn, where n is (in decimal) the process ID.
     * 
     * [...]
     */
#define STR vstring_str

    set_eugid(usr_attr.uid, usr_attr.gid);
    /*
    vstring_sprintf(buf, "%lu.P%d.%s",
		    (unsigned long) starttime, var_pid, get_hostname());
		    */
    // New naming style
    vstring_sprintf(buf, "%lu.%05d%05d.%s.%s.%s.%s",
	(unsigned long) starttime, var_pid, bulk_count,
	"00000000", "00", "00", myhostname);
    
    tmpfile = concatenate(tmpdir, STR(buf), (char *) 0);
    newfile = 0;
    if ((dst = vstream_fopen(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0600)) == 0
	&& (errno != ENOENT
	    || make_dirs(tmpdir, 0700) < 0
	    || (dst = vstream_fopen(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0600)) == 0)) {
	vstring_sprintf(why, "create %s: %m", tmpfile);
    } else if (fstat(vstream_fileno(dst), &st) < 0) {
	vstring_sprintf(why, "create %s: %m", tmpfile);
    } else if (quota_exceed) {
      mail_copy_status = MAIL_COPY_STAT_WRITE;
      vstring_sprintf(why, "%c:%ld:%ld:%ld",
	  'Q', bulk_size, cur_sized, cur_quota);
      errno = EFBIG;
      if (unlink(tmpfile) < 0)
	msg_warn("%s: remove %s: %m", myname, tmpfile);
    } else {
	/*
	vstring_sprintf(buf, "%lu.V%lxI%lx.%s",
			(unsigned long) starttime, (unsigned long) st.st_dev,
			(unsigned long) st.st_ino, get_hostname());
			*/
	// New naming style
	vstring_sprintf(buf, "%lu.%05d%05d.%s.%s.%s.%s",
	  (unsigned long) starttime, var_pid, bulk_count,
	  "00000000", "00", "00", myhostname);
    
	newfile = concatenate(newdir, STR(buf), (char *) 0);
	if ((mail_copy_status = mail_copy(COPY_ATTR(state.msg_attr),
					dst, copy_flags, "\n", why)) == 0) {
	    if (sane_link(tmpfile, newfile) < 0
		&& (errno != ENOENT
		    || (make_dirs(curdir, 0700), make_dirs(newdir, 0700)) < 0
		    || sane_link(tmpfile, newfile) < 0)) {
		vstring_sprintf(why, "link to %s: %m", newfile);
		mail_copy_status = MAIL_COPY_STAT_WRITE;
	    }
	}

	if (var_rmail_zipmail_enable) {
	  vstring_sprintf(buf, "%lu.%05d.%05d.%s.%s.%s.%s.%lugz",
	      (unsigned long) starttime, var_pid, bulk_count,
	      "00000000", "00", "00", myhostname, bulk_size);
	  zipfile = concatenate(newdir, STR(buf), (char *) 0);

	  if (gfp = gzopen(zipfile, "wb")) {
	    if (fp = fopen(newfile, "rb")) {
	      while (fgets(buff, sizeof(buff), fp)) {
		gzputs(gfp, buff);
	      }
	      fclose(fp);

	      if (unlink(newfile) < 0)
		msg_warn("%s: remove %s: %m", myname, newfile);
	    } else { // fp open fail
	      msg_warn("%s: open %s for read: %m", myname, newfile);
	    }
	    gzclose(gfp);
	  } else { // gfp open fail
	    msg_warn("%s: open %s for write: %m", myname, zipfile);
	  }
	}

	if (unlink(tmpfile) < 0)
	    msg_warn("remove %s: %m", tmpfile);
    }
    set_eugid(var_owner_uid, var_owner_gid);

    /*
     * The maildir location is controlled by the mail administrator. If
     * delivery fails, try again later. We would just bounce when the maildir
     * location possibly under user control.
     */
    if (mail_copy_status & MAIL_COPY_STAT_CORRUPT) {
	deliver_status = DEL_STAT_DEFER;
    } else if (mail_copy_status != 0) {
	deliver_status = (errno == EDQUOT || errno == EFBIG ?
			  bounce_append : defer_append)
	    (BOUNCE_FLAGS(state.request), BOUNCE_ATTR(state.msg_attr),
//	     "maildir delivery failed: %s", vstring_str(why));
	    "%s", vstring_str(why));
	if (errno == EACCES) {
	    msg_warn("maildir access problem for UID/GID=%lu/%lu: %s",
		(long) usr_attr.uid, (long) usr_attr.gid, vstring_str(why));
	    msg_warn("perhaps you need to create the maildirs in advance");
	}
    } else {
	deliver_status = sent(BOUNCE_FLAGS(state.request),
			      SENT_ATTR(state.msg_attr),
			      "delivered to maildir");
    }
    vstring_free(buf);
    vstring_free(why);
    myfree(newdir);
    myfree(tmpdir);
    myfree(curdir);
    myfree(tmpfile);
    if (newfile)
	myfree(newfile);
    return (deliver_status);
}
