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
SRCS	= lmtp.c lmtp_connect.c lmtp_proto.c lmtp_chat.c lmtp_session.c \
	lmtp_addr.c lmtp_trouble.c lmtp_state.c lmtp_sasl_glue.c \
	lmtp_sasl_proto.c
OBJS	= lmtp.o lmtp_connect.o lmtp_proto.o lmtp_chat.o lmtp_session.o \
	lmtp_addr.o lmtp_trouble.o lmtp_state.o lmtp_sasl_glue.o \
	lmtp_sasl_proto.o
HDRS	= lmtp.h
TESTSRC	= 
DEFS	= -I. -I$(INC_DIR) -D$(SYSTYPE)
CFLAGS	= $(DEBUG) $(OPT) $(DEFS)
TESTPROG=
PROG	= lmtp
INC_DIR	= ../../include
LIBS	= ../../lib/libmaster.a ../../lib/libglobal.a ../../lib/libdns.a ../../lib/libutil.a

.c.o:;	$(CC) $(CFLAGS) -c $*.c

$(PROG):	$(OBJS) $(LIBS)
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
	cp *.h printfck
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
lmtp.o: lmtp.c
lmtp.o: ../../include/sys_defs.h
lmtp.o: ../../include/dict.h
lmtp.o: ../../include/vstream.h
lmtp.o: ../../include/vbuf.h
lmtp.o: ../../include/argv.h
lmtp.o: ../../include/msg.h
lmtp.o: ../../include/mymalloc.h
lmtp.o: ../../include/name_mask.h
lmtp.o: ../../include/split_at.h
lmtp.o: ../../include/deliver_request.h
lmtp.o: ../../include/vstring.h
lmtp.o: ../../include/recipient_list.h
lmtp.o: ../../include/mail_queue.h
lmtp.o: ../../include/mail_params.h
lmtp.o: ../../include/mail_conf.h
lmtp.o: ../../include/debug_peer.h
lmtp.o: ../../include/mail_error.h
lmtp.o: ../../include/flush_clnt.h
lmtp.o: ../../include/mail_server.h
lmtp.o: lmtp.h
lmtp.o: lmtp_sasl.h
lmtp_addr.o: lmtp_addr.c
lmtp_addr.o: ../../include/sys_defs.h
lmtp_addr.o: ../../include/msg.h
lmtp_addr.o: ../../include/vstring.h
lmtp_addr.o: ../../include/vbuf.h
lmtp_addr.o: ../../include/mymalloc.h
lmtp_addr.o: ../../include/inet_addr_list.h
lmtp_addr.o: ../../include/stringops.h
lmtp_addr.o: ../../include/mail_params.h
lmtp_addr.o: ../../include/own_inet_addr.h
lmtp_addr.o: ../../include/dns.h
lmtp_addr.o: lmtp.h
lmtp_addr.o: ../../include/vstream.h
lmtp_addr.o: ../../include/argv.h
lmtp_addr.o: ../../include/deliver_request.h
lmtp_addr.o: ../../include/recipient_list.h
lmtp_addr.o: lmtp_addr.h
lmtp_chat.o: lmtp_chat.c
lmtp_chat.o: ../../include/sys_defs.h
lmtp_chat.o: ../../include/msg.h
lmtp_chat.o: ../../include/vstring.h
lmtp_chat.o: ../../include/vbuf.h
lmtp_chat.o: ../../include/vstream.h
lmtp_chat.o: ../../include/argv.h
lmtp_chat.o: ../../include/stringops.h
lmtp_chat.o: ../../include/line_wrap.h
lmtp_chat.o: ../../include/mymalloc.h
lmtp_chat.o: ../../include/recipient_list.h
lmtp_chat.o: ../../include/deliver_request.h
lmtp_chat.o: ../../include/smtp_stream.h
lmtp_chat.o: ../../include/mail_params.h
lmtp_chat.o: ../../include/mail_addr.h
lmtp_chat.o: ../../include/post_mail.h
lmtp_chat.o: ../../include/cleanup_user.h
lmtp_chat.o: ../../include/mail_error.h
lmtp_chat.o: ../../include/name_mask.h
lmtp_chat.o: lmtp.h
lmtp_connect.o: lmtp_connect.c
lmtp_connect.o: ../../include/sys_defs.h
lmtp_connect.o: ../../include/msg.h
lmtp_connect.o: ../../include/vstream.h
lmtp_connect.o: ../../include/vbuf.h
lmtp_connect.o: ../../include/vstring.h
lmtp_connect.o: ../../include/split_at.h
lmtp_connect.o: ../../include/mymalloc.h
lmtp_connect.o: ../../include/iostuff.h
lmtp_connect.o: ../../include/timed_connect.h
lmtp_connect.o: ../../include/stringops.h
lmtp_connect.o: ../../include/host_port.h
lmtp_connect.o: ../../include/sane_connect.h
lmtp_connect.o: ../../include/mail_params.h
lmtp_connect.o: ../../include/mail_proto.h
lmtp_connect.o: ../../include/attr.h
lmtp_connect.o: ../../include/dns.h
lmtp_connect.o: lmtp.h
lmtp_connect.o: ../../include/argv.h
lmtp_connect.o: ../../include/deliver_request.h
lmtp_connect.o: ../../include/recipient_list.h
lmtp_connect.o: lmtp_addr.h
lmtp_proto.o: lmtp_proto.c
lmtp_proto.o: ../../include/sys_defs.h
lmtp_proto.o: ../../include/msg.h
lmtp_proto.o: ../../include/vstring.h
lmtp_proto.o: ../../include/vbuf.h
lmtp_proto.o: ../../include/vstream.h
lmtp_proto.o: ../../include/vstring_vstream.h
lmtp_proto.o: ../../include/stringops.h
lmtp_proto.o: ../../include/mymalloc.h
lmtp_proto.o: ../../include/name_code.h
lmtp_proto.o: ../../include/mail_params.h
lmtp_proto.o: ../../include/smtp_stream.h
lmtp_proto.o: ../../include/mail_queue.h
lmtp_proto.o: ../../include/recipient_list.h
lmtp_proto.o: ../../include/deliver_request.h
lmtp_proto.o: ../../include/deliver_completed.h
lmtp_proto.o: ../../include/defer.h
lmtp_proto.o: ../../include/bounce.h
lmtp_proto.o: ../../include/sent.h
lmtp_proto.o: ../../include/record.h
lmtp_proto.o: ../../include/rec_type.h
lmtp_proto.o: ../../include/off_cvt.h
lmtp_proto.o: ../../include/mark_corrupt.h
lmtp_proto.o: ../../include/quote_821_local.h
lmtp_proto.o: ../../include/quote_flags.h
lmtp_proto.o: ../../include/mail_proto.h
lmtp_proto.o: ../../include/iostuff.h
lmtp_proto.o: ../../include/attr.h
lmtp_proto.o: lmtp.h
lmtp_proto.o: ../../include/argv.h
lmtp_proto.o: lmtp_sasl.h
lmtp_sasl_glue.o: lmtp_sasl_glue.c
lmtp_sasl_glue.o: ../../include/sys_defs.h
lmtp_sasl_glue.o: ../../include/msg.h
lmtp_sasl_glue.o: ../../include/mymalloc.h
lmtp_sasl_glue.o: ../../include/stringops.h
lmtp_sasl_glue.o: ../../include/vstring.h
lmtp_sasl_glue.o: ../../include/vbuf.h
lmtp_sasl_glue.o: ../../include/split_at.h
lmtp_sasl_glue.o: ../../include/name_mask.h
lmtp_sasl_glue.o: ../../include/mail_params.h
lmtp_sasl_glue.o: ../../include/string_list.h
lmtp_sasl_glue.o: ../../include/match_list.h
lmtp_sasl_glue.o: ../../include/match_ops.h
lmtp_sasl_glue.o: ../../include/maps.h
lmtp_sasl_glue.o: ../../include/dict.h
lmtp_sasl_glue.o: ../../include/vstream.h
lmtp_sasl_glue.o: ../../include/argv.h
lmtp_sasl_glue.o: lmtp.h
lmtp_sasl_glue.o: ../../include/deliver_request.h
lmtp_sasl_glue.o: ../../include/recipient_list.h
lmtp_sasl_glue.o: lmtp_sasl.h
lmtp_sasl_proto.o: lmtp_sasl_proto.c
lmtp_sasl_proto.o: ../../include/sys_defs.h
lmtp_sasl_proto.o: ../../include/msg.h
lmtp_sasl_proto.o: ../../include/mymalloc.h
lmtp_sasl_proto.o: ../../include/mail_params.h
lmtp_sasl_proto.o: lmtp.h
lmtp_sasl_proto.o: ../../include/vstream.h
lmtp_sasl_proto.o: ../../include/vbuf.h
lmtp_sasl_proto.o: ../../include/vstring.h
lmtp_sasl_proto.o: ../../include/argv.h
lmtp_sasl_proto.o: ../../include/deliver_request.h
lmtp_sasl_proto.o: ../../include/recipient_list.h
lmtp_sasl_proto.o: lmtp_sasl.h
lmtp_session.o: lmtp_session.c
lmtp_session.o: ../../include/sys_defs.h
lmtp_session.o: ../../include/mymalloc.h
lmtp_session.o: ../../include/vstream.h
lmtp_session.o: ../../include/vbuf.h
lmtp_session.o: ../../include/stringops.h
lmtp_session.o: ../../include/vstring.h
lmtp_session.o: ../../include/debug_peer.h
lmtp_session.o: lmtp.h
lmtp_session.o: ../../include/argv.h
lmtp_session.o: ../../include/deliver_request.h
lmtp_session.o: ../../include/recipient_list.h
lmtp_state.o: lmtp_state.c
lmtp_state.o: ../../include/sys_defs.h
lmtp_state.o: ../../include/mymalloc.h
lmtp_state.o: ../../include/vstring.h
lmtp_state.o: ../../include/vbuf.h
lmtp_state.o: ../../include/vstream.h
lmtp_state.o: ../../include/mail_conf.h
lmtp_state.o: lmtp.h
lmtp_state.o: ../../include/argv.h
lmtp_state.o: ../../include/deliver_request.h
lmtp_state.o: ../../include/recipient_list.h
lmtp_state.o: lmtp_sasl.h
lmtp_trouble.o: lmtp_trouble.c
lmtp_trouble.o: ../../include/sys_defs.h
lmtp_trouble.o: ../../include/msg.h
lmtp_trouble.o: ../../include/vstring.h
lmtp_trouble.o: ../../include/vbuf.h
lmtp_trouble.o: ../../include/stringops.h
lmtp_trouble.o: ../../include/mymalloc.h
lmtp_trouble.o: ../../include/smtp_stream.h
lmtp_trouble.o: ../../include/vstream.h
lmtp_trouble.o: ../../include/deliver_request.h
lmtp_trouble.o: ../../include/recipient_list.h
lmtp_trouble.o: ../../include/deliver_completed.h
lmtp_trouble.o: ../../include/bounce.h
lmtp_trouble.o: ../../include/defer.h
lmtp_trouble.o: ../../include/mail_error.h
lmtp_trouble.o: ../../include/name_mask.h
lmtp_trouble.o: lmtp.h
lmtp_trouble.o: ../../include/argv.h
