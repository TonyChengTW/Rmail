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
SRCS	= postcat.c
OBJS	= postcat.o
HDRS	= 
TESTSRC	= 
DEFS	= -I. -I$(INC_DIR) -D$(SYSTYPE)
CFLAGS	= $(DEBUG) $(OPT) $(DEFS)
TESTPROG= 
PROG	= postcat
INC_DIR	= ../../include
LIBS	= ../../lib/libglobal.a ../../lib/libutil.a

.c.o:;	$(CC) $(CFLAGS) -c $*.c

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(SYSLIBS)

Makefile: Makefile.in
	(set -e; echo "# DO NOT EDIT"; $(OPTS) $(SHELL) ../../makedefs && cat $?) >$@

test:	$(TESTPROG)

tests:	test

update: ../../bin/$(PROG)

../../bin/$(PROG): $(PROG)
	cp $(PROG) ../../bin

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
postcat.o: postcat.c
postcat.o: ../../include/sys_defs.h
postcat.o: ../../include/msg.h
postcat.o: ../../include/vstream.h
postcat.o: ../../include/vbuf.h
postcat.o: ../../include/vstring.h
postcat.o: ../../include/msg_vstream.h
postcat.o: ../../include/vstring_vstream.h
postcat.o: ../../include/record.h
postcat.o: ../../include/rec_type.h
postcat.o: ../../include/mail_queue.h
postcat.o: ../../include/mail_conf.h
postcat.o: ../../include/mail_params.h
