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
SRCS	= cleanup.c cleanup_out.c cleanup_envelope.c cleanup_message.c \
	cleanup_extracted.c cleanup_state.c cleanup_rewrite.c \
	cleanup_map11.c cleanup_map1n.c cleanup_masquerade.c \
	cleanup_out_recipient.c cleanup_init.c cleanup_api.c \
	cleanup_addr.c
OBJS	= cleanup.o cleanup_out.o cleanup_envelope.o cleanup_message.o \
	cleanup_extracted.o cleanup_state.o cleanup_rewrite.o \
	cleanup_map11.o cleanup_map1n.o cleanup_masquerade.o \
	cleanup_out_recipient.o cleanup_init.o cleanup_api.o \
	cleanup_addr.o
HDRS	=
TESTSRC	= 
DEFS	= -I. -I$(INC_DIR) -D$(SYSTYPE)
CFLAGS	= $(DEBUG) $(OPT) $(DEFS)
TESTPROG= cleanup_masquerade
PROG	= cleanup
INC_DIR	= ../../include
LIBS	= ../../lib/libmaster.a ../../lib/libglobal.a ../../lib/libutil.a

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
	sed '1,/^# do not edit/!d' Makefile > printfck/Makefile
	set -e; for i in *.c; do printfck -f .printfck $$i >printfck/$$i; done
	cd printfck; make "INC_DIR=../../../include" `cd ..; ls *.o`

lint:
	lint $(DEFS) $(SRCS) $(LINTFIX)

clean:
	rm -f *.o *core $(PROG) $(TESTPROG) junk 
	rm -rf printfck

tidy:	clean

cleanup_masquerade: cleanup_masquerade.o 
	mv cleanup_masquerade.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIBS) $(SYSLIBS)
	mv junk cleanup_masquerade.o

tests:	cleanup_masquerade_test

cleanup_masquerade_test: cleanup_masquerade cleanup_masq.ref
	rm -f cleanup_masq.tmp
	./cleanup_masquerade '' a.b.c,b.c xxx@aa.a.b.c    >>cleanup_masq.tmp
	./cleanup_masquerade 'xxx' a.b.c,b.c xxx@aa.a.b.c >>cleanup_masq.tmp
	./cleanup_masquerade 'yyy' a.b.c,b.c xxx@aa.a.b.c >>cleanup_masq.tmp
	./cleanup_masquerade '' !a.b.c,b.c xxx@aa.a.b.c   >>cleanup_masq.tmp
	./cleanup_masquerade '' a.b.c,b.c xxx@a.b.c       >>cleanup_masq.tmp
	./cleanup_masquerade '' !a.b.c,b.c xxx@a.b.c      >>cleanup_masq.tmp
	./cleanup_masquerade '' a.b.c,b.c xxx@aaa.b.c     >>cleanup_masq.tmp
	./cleanup_masquerade '' a.b.c,b.c xxx@b.c         >>cleanup_masq.tmp
	diff cleanup_masq.ref cleanup_masq.tmp
	rm -f cleanup_masq.tmp

depend: $(MAKES)
	(sed '1,/^# do not edit/!d' Makefile.in; \
	set -e; for i in [a-z][a-z0-9]*.c; do \
	    $(CC) -E $(DEFS) $(INCL) $$i | sed -n -e '/^# *1 *"\([^"]*\)".*/{' \
	    -e 's//'`echo $$i|sed 's/c$$/o/'`': \1/' -e 'p' -e '}'; \
	done) | grep -v '[.][o][:][ ][/]' >$$$$ && mv $$$$ Makefile.in
	@$(EXPORT) make -f Makefile.in Makefile 1>&2

# do not edit below this line - it is generated by 'make depend'
cleanup.o: cleanup.c
cleanup.o: ../../include/sys_defs.h
cleanup.o: ../../include/msg.h
cleanup.o: ../../include/vstring.h
cleanup.o: ../../include/vbuf.h
cleanup.o: ../../include/dict.h
cleanup.o: ../../include/vstream.h
cleanup.o: ../../include/argv.h
cleanup.o: ../../include/mail_conf.h
cleanup.o: ../../include/cleanup_user.h
cleanup.o: ../../include/mail_proto.h
cleanup.o: ../../include/iostuff.h
cleanup.o: ../../include/attr.h
cleanup.o: ../../include/mail_params.h
cleanup.o: ../../include/record.h
cleanup.o: ../../include/rec_type.h
cleanup.o: ../../include/mail_server.h
cleanup.o: cleanup.h
cleanup.o: ../../include/nvtable.h
cleanup.o: ../../include/htable.h
cleanup.o: ../../include/mymalloc.h
cleanup.o: ../../include/maps.h
cleanup.o: ../../include/tok822.h
cleanup.o: ../../include/resolve_clnt.h
cleanup.o: ../../include/been_here.h
cleanup.o: ../../include/mail_stream.h
cleanup.o: ../../include/mime_state.h
cleanup.o: ../../include/header_opts.h
cleanup.o: ../../include/string_list.h
cleanup.o: ../../include/match_list.h
cleanup.o: ../../include/match_ops.h
cleanup_addr.o: cleanup_addr.c
cleanup_addr.o: ../../include/sys_defs.h
cleanup_addr.o: ../../include/msg.h
cleanup_addr.o: ../../include/vstring.h
cleanup_addr.o: ../../include/vbuf.h
cleanup_addr.o: ../../include/vstream.h
cleanup_addr.o: ../../include/mymalloc.h
cleanup_addr.o: ../../include/stringops.h
cleanup_addr.o: ../../include/rec_type.h
cleanup_addr.o: ../../include/cleanup_user.h
cleanup_addr.o: ../../include/mail_params.h
cleanup_addr.o: ../../include/ext_prop.h
cleanup_addr.o: ../../include/mail_addr.h
cleanup_addr.o: ../../include/canon_addr.h
cleanup_addr.o: ../../include/mail_addr_find.h
cleanup_addr.o: ../../include/maps.h
cleanup_addr.o: ../../include/dict.h
cleanup_addr.o: ../../include/argv.h
cleanup_addr.o: cleanup.h
cleanup_addr.o: ../../include/nvtable.h
cleanup_addr.o: ../../include/htable.h
cleanup_addr.o: ../../include/tok822.h
cleanup_addr.o: ../../include/resolve_clnt.h
cleanup_addr.o: ../../include/been_here.h
cleanup_addr.o: ../../include/mail_stream.h
cleanup_addr.o: ../../include/mail_conf.h
cleanup_addr.o: ../../include/mime_state.h
cleanup_addr.o: ../../include/header_opts.h
cleanup_addr.o: ../../include/string_list.h
cleanup_addr.o: ../../include/match_list.h
cleanup_addr.o: ../../include/match_ops.h
cleanup_api.o: cleanup_api.c
cleanup_api.o: ../../include/sys_defs.h
cleanup_api.o: ../../include/msg.h
cleanup_api.o: ../../include/vstring.h
cleanup_api.o: ../../include/vbuf.h
cleanup_api.o: ../../include/mymalloc.h
cleanup_api.o: ../../include/cleanup_user.h
cleanup_api.o: ../../include/mail_queue.h
cleanup_api.o: ../../include/vstream.h
cleanup_api.o: ../../include/mail_proto.h
cleanup_api.o: ../../include/iostuff.h
cleanup_api.o: ../../include/attr.h
cleanup_api.o: ../../include/bounce.h
cleanup_api.o: ../../include/deliver_request.h
cleanup_api.o: ../../include/recipient_list.h
cleanup_api.o: ../../include/mail_params.h
cleanup_api.o: ../../include/mail_stream.h
cleanup_api.o: ../../include/hold_message.h
cleanup_api.o: cleanup.h
cleanup_api.o: ../../include/argv.h
cleanup_api.o: ../../include/nvtable.h
cleanup_api.o: ../../include/htable.h
cleanup_api.o: ../../include/maps.h
cleanup_api.o: ../../include/dict.h
cleanup_api.o: ../../include/tok822.h
cleanup_api.o: ../../include/resolve_clnt.h
cleanup_api.o: ../../include/been_here.h
cleanup_api.o: ../../include/mail_conf.h
cleanup_api.o: ../../include/mime_state.h
cleanup_api.o: ../../include/header_opts.h
cleanup_api.o: ../../include/string_list.h
cleanup_api.o: ../../include/match_list.h
cleanup_api.o: ../../include/match_ops.h
cleanup_envelope.o: cleanup_envelope.c
cleanup_envelope.o: ../../include/sys_defs.h
cleanup_envelope.o: ../../include/msg.h
cleanup_envelope.o: ../../include/vstring.h
cleanup_envelope.o: ../../include/vbuf.h
cleanup_envelope.o: ../../include/vstream.h
cleanup_envelope.o: ../../include/mymalloc.h
cleanup_envelope.o: ../../include/stringops.h
cleanup_envelope.o: ../../include/nvtable.h
cleanup_envelope.o: ../../include/htable.h
cleanup_envelope.o: ../../include/record.h
cleanup_envelope.o: ../../include/rec_type.h
cleanup_envelope.o: ../../include/cleanup_user.h
cleanup_envelope.o: ../../include/qmgr_user.h
cleanup_envelope.o: ../../include/mail_params.h
cleanup_envelope.o: ../../include/verp_sender.h
cleanup_envelope.o: cleanup.h
cleanup_envelope.o: ../../include/argv.h
cleanup_envelope.o: ../../include/maps.h
cleanup_envelope.o: ../../include/dict.h
cleanup_envelope.o: ../../include/tok822.h
cleanup_envelope.o: ../../include/resolve_clnt.h
cleanup_envelope.o: ../../include/been_here.h
cleanup_envelope.o: ../../include/mail_stream.h
cleanup_envelope.o: ../../include/mail_conf.h
cleanup_envelope.o: ../../include/mime_state.h
cleanup_envelope.o: ../../include/header_opts.h
cleanup_envelope.o: ../../include/string_list.h
cleanup_envelope.o: ../../include/match_list.h
cleanup_envelope.o: ../../include/match_ops.h
cleanup_extracted.o: cleanup_extracted.c
cleanup_extracted.o: ../../include/sys_defs.h
cleanup_extracted.o: ../../include/msg.h
cleanup_extracted.o: ../../include/vstring.h
cleanup_extracted.o: ../../include/vbuf.h
cleanup_extracted.o: ../../include/vstream.h
cleanup_extracted.o: ../../include/mymalloc.h
cleanup_extracted.o: ../../include/nvtable.h
cleanup_extracted.o: ../../include/htable.h
cleanup_extracted.o: ../../include/cleanup_user.h
cleanup_extracted.o: ../../include/qmgr_user.h
cleanup_extracted.o: ../../include/record.h
cleanup_extracted.o: ../../include/rec_type.h
cleanup_extracted.o: ../../include/mail_params.h
cleanup_extracted.o: ../../include/mail_proto.h
cleanup_extracted.o: ../../include/iostuff.h
cleanup_extracted.o: ../../include/attr.h
cleanup_extracted.o: cleanup.h
cleanup_extracted.o: ../../include/argv.h
cleanup_extracted.o: ../../include/maps.h
cleanup_extracted.o: ../../include/dict.h
cleanup_extracted.o: ../../include/tok822.h
cleanup_extracted.o: ../../include/resolve_clnt.h
cleanup_extracted.o: ../../include/been_here.h
cleanup_extracted.o: ../../include/mail_stream.h
cleanup_extracted.o: ../../include/mail_conf.h
cleanup_extracted.o: ../../include/mime_state.h
cleanup_extracted.o: ../../include/header_opts.h
cleanup_extracted.o: ../../include/string_list.h
cleanup_extracted.o: ../../include/match_list.h
cleanup_extracted.o: ../../include/match_ops.h
cleanup_init.o: cleanup_init.c
cleanup_init.o: ../../include/sys_defs.h
cleanup_init.o: ../../include/msg.h
cleanup_init.o: ../../include/iostuff.h
cleanup_init.o: ../../include/name_mask.h
cleanup_init.o: ../../include/mail_addr.h
cleanup_init.o: ../../include/mail_params.h
cleanup_init.o: ../../include/ext_prop.h
cleanup_init.o: ../../include/flush_clnt.h
cleanup_init.o: cleanup.h
cleanup_init.o: ../../include/vstring.h
cleanup_init.o: ../../include/vbuf.h
cleanup_init.o: ../../include/vstream.h
cleanup_init.o: ../../include/argv.h
cleanup_init.o: ../../include/nvtable.h
cleanup_init.o: ../../include/htable.h
cleanup_init.o: ../../include/mymalloc.h
cleanup_init.o: ../../include/maps.h
cleanup_init.o: ../../include/dict.h
cleanup_init.o: ../../include/tok822.h
cleanup_init.o: ../../include/resolve_clnt.h
cleanup_init.o: ../../include/been_here.h
cleanup_init.o: ../../include/mail_stream.h
cleanup_init.o: ../../include/mail_conf.h
cleanup_init.o: ../../include/mime_state.h
cleanup_init.o: ../../include/header_opts.h
cleanup_init.o: ../../include/string_list.h
cleanup_init.o: ../../include/match_list.h
cleanup_init.o: ../../include/match_ops.h
cleanup_map11.o: cleanup_map11.c
cleanup_map11.o: ../../include/sys_defs.h
cleanup_map11.o: ../../include/msg.h
cleanup_map11.o: ../../include/vstring.h
cleanup_map11.o: ../../include/vbuf.h
cleanup_map11.o: ../../include/dict.h
cleanup_map11.o: ../../include/vstream.h
cleanup_map11.o: ../../include/argv.h
cleanup_map11.o: ../../include/mymalloc.h
cleanup_map11.o: ../../include/cleanup_user.h
cleanup_map11.o: ../../include/mail_addr_map.h
cleanup_map11.o: ../../include/maps.h
cleanup_map11.o: ../../include/quote_822_local.h
cleanup_map11.o: ../../include/quote_flags.h
cleanup_map11.o: cleanup.h
cleanup_map11.o: ../../include/nvtable.h
cleanup_map11.o: ../../include/htable.h
cleanup_map11.o: ../../include/tok822.h
cleanup_map11.o: ../../include/resolve_clnt.h
cleanup_map11.o: ../../include/been_here.h
cleanup_map11.o: ../../include/mail_stream.h
cleanup_map11.o: ../../include/mail_conf.h
cleanup_map11.o: ../../include/mime_state.h
cleanup_map11.o: ../../include/header_opts.h
cleanup_map11.o: ../../include/string_list.h
cleanup_map11.o: ../../include/match_list.h
cleanup_map11.o: ../../include/match_ops.h
cleanup_map1n.o: cleanup_map1n.c
cleanup_map1n.o: ../../include/sys_defs.h
cleanup_map1n.o: ../../include/mymalloc.h
cleanup_map1n.o: ../../include/msg.h
cleanup_map1n.o: ../../include/argv.h
cleanup_map1n.o: ../../include/vstring.h
cleanup_map1n.o: ../../include/vbuf.h
cleanup_map1n.o: ../../include/dict.h
cleanup_map1n.o: ../../include/vstream.h
cleanup_map1n.o: ../../include/mail_params.h
cleanup_map1n.o: ../../include/mail_addr_map.h
cleanup_map1n.o: ../../include/maps.h
cleanup_map1n.o: ../../include/cleanup_user.h
cleanup_map1n.o: ../../include/quote_822_local.h
cleanup_map1n.o: ../../include/quote_flags.h
cleanup_map1n.o: ../../include/been_here.h
cleanup_map1n.o: cleanup.h
cleanup_map1n.o: ../../include/nvtable.h
cleanup_map1n.o: ../../include/htable.h
cleanup_map1n.o: ../../include/tok822.h
cleanup_map1n.o: ../../include/resolve_clnt.h
cleanup_map1n.o: ../../include/mail_stream.h
cleanup_map1n.o: ../../include/mail_conf.h
cleanup_map1n.o: ../../include/mime_state.h
cleanup_map1n.o: ../../include/header_opts.h
cleanup_map1n.o: ../../include/string_list.h
cleanup_map1n.o: ../../include/match_list.h
cleanup_map1n.o: ../../include/match_ops.h
cleanup_masquerade.o: cleanup_masquerade.c
cleanup_masquerade.o: ../../include/sys_defs.h
cleanup_masquerade.o: ../../include/msg.h
cleanup_masquerade.o: ../../include/vstring.h
cleanup_masquerade.o: ../../include/vbuf.h
cleanup_masquerade.o: ../../include/argv.h
cleanup_masquerade.o: ../../include/htable.h
cleanup_masquerade.o: ../../include/mymalloc.h
cleanup_masquerade.o: ../../include/stringops.h
cleanup_masquerade.o: ../../include/mail_params.h
cleanup_masquerade.o: ../../include/tok822.h
cleanup_masquerade.o: ../../include/resolve_clnt.h
cleanup_masquerade.o: ../../include/quote_822_local.h
cleanup_masquerade.o: ../../include/quote_flags.h
cleanup_masquerade.o: cleanup.h
cleanup_masquerade.o: ../../include/vstream.h
cleanup_masquerade.o: ../../include/nvtable.h
cleanup_masquerade.o: ../../include/maps.h
cleanup_masquerade.o: ../../include/dict.h
cleanup_masquerade.o: ../../include/been_here.h
cleanup_masquerade.o: ../../include/mail_stream.h
cleanup_masquerade.o: ../../include/mail_conf.h
cleanup_masquerade.o: ../../include/mime_state.h
cleanup_masquerade.o: ../../include/header_opts.h
cleanup_masquerade.o: ../../include/string_list.h
cleanup_masquerade.o: ../../include/match_list.h
cleanup_masquerade.o: ../../include/match_ops.h
cleanup_message.o: cleanup_message.c
cleanup_message.o: ../../include/sys_defs.h
cleanup_message.o: ../../include/msg.h
cleanup_message.o: ../../include/vstring.h
cleanup_message.o: ../../include/vbuf.h
cleanup_message.o: ../../include/vstream.h
cleanup_message.o: ../../include/argv.h
cleanup_message.o: ../../include/split_at.h
cleanup_message.o: ../../include/mymalloc.h
cleanup_message.o: ../../include/stringops.h
cleanup_message.o: ../../include/nvtable.h
cleanup_message.o: ../../include/htable.h
cleanup_message.o: ../../include/record.h
cleanup_message.o: ../../include/rec_type.h
cleanup_message.o: ../../include/cleanup_user.h
cleanup_message.o: ../../include/tok822.h
cleanup_message.o: ../../include/resolve_clnt.h
cleanup_message.o: ../../include/header_opts.h
cleanup_message.o: ../../include/quote_822_local.h
cleanup_message.o: ../../include/quote_flags.h
cleanup_message.o: ../../include/mail_params.h
cleanup_message.o: ../../include/mail_date.h
cleanup_message.o: ../../include/mail_addr.h
cleanup_message.o: ../../include/is_header.h
cleanup_message.o: ../../include/ext_prop.h
cleanup_message.o: ../../include/mail_proto.h
cleanup_message.o: ../../include/iostuff.h
cleanup_message.o: ../../include/attr.h
cleanup_message.o: ../../include/mime_state.h
cleanup_message.o: ../../include/lex_822.h
cleanup_message.o: cleanup.h
cleanup_message.o: ../../include/maps.h
cleanup_message.o: ../../include/dict.h
cleanup_message.o: ../../include/been_here.h
cleanup_message.o: ../../include/mail_stream.h
cleanup_message.o: ../../include/mail_conf.h
cleanup_message.o: ../../include/string_list.h
cleanup_message.o: ../../include/match_list.h
cleanup_message.o: ../../include/match_ops.h
cleanup_out.o: cleanup_out.c
cleanup_out.o: ../../include/sys_defs.h
cleanup_out.o: ../../include/msg.h
cleanup_out.o: ../../include/vstring.h
cleanup_out.o: ../../include/vbuf.h
cleanup_out.o: ../../include/vstream.h
cleanup_out.o: ../../include/record.h
cleanup_out.o: ../../include/rec_type.h
cleanup_out.o: ../../include/cleanup_user.h
cleanup_out.o: ../../include/mail_params.h
cleanup_out.o: cleanup.h
cleanup_out.o: ../../include/argv.h
cleanup_out.o: ../../include/nvtable.h
cleanup_out.o: ../../include/htable.h
cleanup_out.o: ../../include/mymalloc.h
cleanup_out.o: ../../include/maps.h
cleanup_out.o: ../../include/dict.h
cleanup_out.o: ../../include/tok822.h
cleanup_out.o: ../../include/resolve_clnt.h
cleanup_out.o: ../../include/been_here.h
cleanup_out.o: ../../include/mail_stream.h
cleanup_out.o: ../../include/mail_conf.h
cleanup_out.o: ../../include/mime_state.h
cleanup_out.o: ../../include/header_opts.h
cleanup_out.o: ../../include/string_list.h
cleanup_out.o: ../../include/match_list.h
cleanup_out.o: ../../include/match_ops.h
cleanup_out_recipient.o: cleanup_out_recipient.c
cleanup_out_recipient.o: ../../include/sys_defs.h
cleanup_out_recipient.o: ../../include/argv.h
cleanup_out_recipient.o: ../../include/been_here.h
cleanup_out_recipient.o: ../../include/mail_params.h
cleanup_out_recipient.o: ../../include/rec_type.h
cleanup_out_recipient.o: ../../include/ext_prop.h
cleanup_out_recipient.o: ../../include/cleanup_user.h
cleanup_out_recipient.o: cleanup.h
cleanup_out_recipient.o: ../../include/vstring.h
cleanup_out_recipient.o: ../../include/vbuf.h
cleanup_out_recipient.o: ../../include/vstream.h
cleanup_out_recipient.o: ../../include/nvtable.h
cleanup_out_recipient.o: ../../include/htable.h
cleanup_out_recipient.o: ../../include/mymalloc.h
cleanup_out_recipient.o: ../../include/maps.h
cleanup_out_recipient.o: ../../include/dict.h
cleanup_out_recipient.o: ../../include/tok822.h
cleanup_out_recipient.o: ../../include/resolve_clnt.h
cleanup_out_recipient.o: ../../include/mail_stream.h
cleanup_out_recipient.o: ../../include/mail_conf.h
cleanup_out_recipient.o: ../../include/mime_state.h
cleanup_out_recipient.o: ../../include/header_opts.h
cleanup_out_recipient.o: ../../include/string_list.h
cleanup_out_recipient.o: ../../include/match_list.h
cleanup_out_recipient.o: ../../include/match_ops.h
cleanup_rewrite.o: cleanup_rewrite.c
cleanup_rewrite.o: ../../include/sys_defs.h
cleanup_rewrite.o: ../../include/msg.h
cleanup_rewrite.o: ../../include/vstring.h
cleanup_rewrite.o: ../../include/vbuf.h
cleanup_rewrite.o: ../../include/tok822.h
cleanup_rewrite.o: ../../include/resolve_clnt.h
cleanup_rewrite.o: ../../include/rewrite_clnt.h
cleanup_rewrite.o: ../../include/quote_822_local.h
cleanup_rewrite.o: ../../include/quote_flags.h
cleanup_rewrite.o: cleanup.h
cleanup_rewrite.o: ../../include/vstream.h
cleanup_rewrite.o: ../../include/argv.h
cleanup_rewrite.o: ../../include/nvtable.h
cleanup_rewrite.o: ../../include/htable.h
cleanup_rewrite.o: ../../include/mymalloc.h
cleanup_rewrite.o: ../../include/maps.h
cleanup_rewrite.o: ../../include/dict.h
cleanup_rewrite.o: ../../include/been_here.h
cleanup_rewrite.o: ../../include/mail_stream.h
cleanup_rewrite.o: ../../include/mail_conf.h
cleanup_rewrite.o: ../../include/mime_state.h
cleanup_rewrite.o: ../../include/header_opts.h
cleanup_rewrite.o: ../../include/string_list.h
cleanup_rewrite.o: ../../include/match_list.h
cleanup_rewrite.o: ../../include/match_ops.h
cleanup_state.o: cleanup_state.c
cleanup_state.o: ../../include/sys_defs.h
cleanup_state.o: ../../include/mymalloc.h
cleanup_state.o: ../../include/vstring.h
cleanup_state.o: ../../include/vbuf.h
cleanup_state.o: ../../include/htable.h
cleanup_state.o: ../../include/been_here.h
cleanup_state.o: ../../include/mail_params.h
cleanup_state.o: ../../include/mime_state.h
cleanup_state.o: ../../include/header_opts.h
cleanup_state.o: cleanup.h
cleanup_state.o: ../../include/vstream.h
cleanup_state.o: ../../include/argv.h
cleanup_state.o: ../../include/nvtable.h
cleanup_state.o: ../../include/maps.h
cleanup_state.o: ../../include/dict.h
cleanup_state.o: ../../include/tok822.h
cleanup_state.o: ../../include/resolve_clnt.h
cleanup_state.o: ../../include/mail_stream.h
cleanup_state.o: ../../include/mail_conf.h
cleanup_state.o: ../../include/string_list.h
cleanup_state.o: ../../include/match_list.h
cleanup_state.o: ../../include/match_ops.h
