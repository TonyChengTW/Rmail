# DO NOT EDIT
SYSTYPE	= FREEBSD5
AR	= ar
ARFL	= rv
RANLIB	= ranlib
SYSLIBS	= -L/usr/local/lib/mysql -lmysqlclient -lz -lm -L/usr/local/lib -lpcre -lcrypt -L/usr/local/lib -lpcre 
CC	= gcc $(WARN) -DHAS_MYSQL -I/usr/local/include/mysql -DHAS_PCRE -I/usr/local/include
OPT	= -O
DEBUG	= -g
AWK	= awk
STRCASE = 
EXPORT	= AUXLIBS='-L/usr/local/lib/mysql -lmysqlclient -lz -lm -L/usr/local/lib -lpcre -lcrypt -L/usr/local/lib -lpcre' CCARGS='-DHAS_MYSQL -I/usr/local/include/mysql -DHAS_PCRE -I/usr/local/include' OPT='-O' DEBUG='-g'
WARN	= -W -Wformat -Wimplicit -Wmissing-prototypes \
	-Wparentheses -Wstrict-prototypes -Wswitch -Wuninitialized \
	-Wunused
SHELL	= /bin/sh
SRCS	= flush.c
OBJS	= flush.o
HDRS	= 
TESTSRC	=
DEFS	= -I. -I$(INC_DIR) -D$(SYSTYPE)
CFLAGS	= $(DEBUG) $(OPT) $(DEFS)
TESTPROG= 
PROG	= flush
INC_DIR = ../../include
LIBS	= ../../lib/libmaster.a ../../lib/libglobal.a ../../lib/libutil.a

.c.o:;	$(CC) $(CFLAGS) -c $*.c

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(SYSLIBS)

Makefile: Makefile.in
	(set -e; echo "# DO NOT EDIT"; $(OPTS) $(SHELL) ../../makedefs && cat $?) >$@

test:	$(TESTPROG)

tests:	test

update: ../../libexec/$(PROG)

../../libexec/$(PROG): $(PROG)
	cp $(PROG) ../../libexec

printfck: $(OBJS) $(PROG)
	rm -rf printfck
	mkdir printfck
	sed '1,/^# do not edit/!d' Makefile >printfck/Makefile
	set -e; for i in *.c; do printfck -f .printfck $$i >printfck/$$i; done
	cd printfck; make "INC_DIR=../../../include" `cd ..; ls *.o`

lint:
	lint $(DEFS) $(SRCS) $(LINTFIX)

clean:
	rm -f *.o *core $(PROG) $(TESTPROG) junk 
	rm -rf printfck

tidy:	clean

depend: $(MAKES)
	(sed '1,/^# do not edit/!d' Makefile.in; \
	set -e; for i in [a-z][a-z0-9]*.c; do \
	    $(CC) -E $(DEFS) $(INCL) $$i | sed -n -e '/^# *1 *"\([^"]*\)".*/{' \
	    -e 's//'`echo $$i|sed 's/c$$/o/'`': \1/' -e 'p' -e '}'; \
	done) | grep -v '[.][o][:][ ][/]' >$$$$ && mv $$$$ Makefile.in
	@$(EXPORT) make -f Makefile.in Makefile 1>&2

# do not edit below this line - it is generated by 'make depend'
flush.o: flush.c
flush.o: ../../include/sys_defs.h
flush.o: ../../include/msg.h
flush.o: ../../include/events.h
flush.o: ../../include/vstream.h
flush.o: ../../include/vbuf.h
flush.o: ../../include/vstring.h
flush.o: ../../include/vstring_vstream.h
flush.o: ../../include/myflock.h
flush.o: ../../include/htable.h
flush.o: ../../include/dict.h
flush.o: ../../include/argv.h
flush.o: ../../include/scan_dir.h
flush.o: ../../include/stringops.h
flush.o: ../../include/mail_params.h
flush.o: ../../include/mail_queue.h
flush.o: ../../include/mail_proto.h
flush.o: ../../include/iostuff.h
flush.o: ../../include/attr.h
flush.o: ../../include/mail_flush.h
flush.o: ../../include/flush_clnt.h
flush.o: ../../include/mail_conf.h
flush.o: ../../include/mail_scan_dir.h
flush.o: ../../include/maps.h
flush.o: ../../include/domain_list.h
flush.o: ../../include/match_list.h
flush.o: ../../include/match_ops.h
flush.o: ../../include/match_parent_style.h
flush.o: ../../include/mail_server.h
