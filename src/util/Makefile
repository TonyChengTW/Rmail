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
SRCS	= alldig.c argv.c argv_split.c attr_print0.c attr_print64.c \
	attr_scan0.c attr_scan64.c base64_code.c basename.c binhash.c \
	chroot_uid.c clean_env.c close_on_exec.c concatenate.c ctable.c \
	dict.c dict_alloc.c dict_db.c dict_dbm.c dict_debug.c dict_env.c \
	dict_cidr.c dict_ht.c dict_ni.c dict_nis.c \
	dict_nisplus.c dict_open.c dict_pcre.c dict_regexp.c \
	dict_static.c dict_tcp.c dict_unix.c dir_forest.c doze.c \
	duplex_pipe.c environ.c events.c exec_command.c fifo_listen.c \
	fifo_trigger.c file_limit.c find_inet.c fsspace.c fullname.c \
	get_domainname.c get_hostname.c hex_quote.c host_port.c htable.c \
	inet_addr_host.c inet_addr_list.c inet_addr_local.c inet_connect.c \
	inet_listen.c inet_trigger.c inet_util.c line_wrap.c \
	lowercase.c lstat_as.c mac_expand.c mac_parse.c make_dirs.c \
	match_list.c match_ops.c msg.c msg_output.c msg_syslog.c \
	msg_vstream.c mvect.c myflock.c mymalloc.c myrand.c mystrtok.c \
	name_mask.c netstring.c non_blocking.c nvtable.c open_as.c \
	open_limit.c open_lock.c peekfd.c percentm.c posix_signals.c \
	printable.c rand_sleep.c read_wait.c readable.c readlline.c \
	ring.c safe_getenv.c safe_open.c sane_accept.c sane_link.c \
	sane_rename.c sane_socketpair.c sane_time.c scan_dir.c \
	set_eugid.c set_ugid.c sigdelay.c skipblanks.c spawn_command.c \
	split_at.c split_nameval.c stat_as.c strcasecmp.c stream_connect.c \
	stream_listen.c stream_trigger.c sys_compat.c timed_connect.c \
	timed_read.c timed_wait.c timed_write.c translit.c trimblanks.c \
	unescape.c unix_connect.c unix_listen.c unix_trigger.c unsafe.c \
	username.c valid_hostname.c vbuf.c vbuf_print.c vstream.c \
	vstream_popen.c vstring.c vstring_vstream.c watchdog.c writable.c \
	write_buf.c write_wait.c auto_clnt.c attr_clnt.c attr_scan_plain.c \
	attr_print_plain.c sane_connect.c neuter.c name_code.c \
	uppercase.c
OBJS	= alldig.o argv.o argv_split.o attr_print0.o attr_print64.o \
	attr_scan0.o attr_scan64.o base64_code.o basename.o binhash.o \
	chroot_uid.o clean_env.o close_on_exec.o concatenate.o ctable.o \
	dict.o dict_alloc.o dict_db.o dict_dbm.o dict_debug.o dict_env.o \
	dict_cidr.o dict_ht.o dict_ni.o dict_nis.o \
	dict_nisplus.o dict_open.o dict_pcre.o dict_regexp.o \
	dict_static.o dict_tcp.o dict_unix.o dir_forest.o doze.o \
	duplex_pipe.o environ.o events.o exec_command.o fifo_listen.o \
	fifo_trigger.o file_limit.o find_inet.o fsspace.o fullname.o \
	get_domainname.o get_hostname.o hex_quote.o host_port.o htable.o \
	inet_addr_host.o inet_addr_list.o inet_addr_local.o inet_connect.o \
	inet_listen.o inet_trigger.o inet_util.o line_wrap.o \
	lowercase.o lstat_as.o mac_expand.o mac_parse.o make_dirs.o \
	match_list.o match_ops.o msg.o msg_output.o msg_syslog.o \
	msg_vstream.o mvect.o myflock.o mymalloc.o myrand.o mystrtok.o \
	name_mask.o netstring.o non_blocking.o nvtable.o open_as.o \
	open_limit.o open_lock.o peekfd.o percentm.o posix_signals.o \
	printable.o rand_sleep.o read_wait.o readable.o readlline.o \
	ring.o safe_getenv.o safe_open.o sane_accept.o sane_link.o \
	sane_rename.o sane_socketpair.o sane_time.o scan_dir.o \
	set_eugid.o set_ugid.o sigdelay.o skipblanks.o spawn_command.o \
	split_at.o split_nameval.o stat_as.o stream_connect.o \
	stream_listen.o stream_trigger.o sys_compat.o timed_connect.o \
	timed_read.o timed_wait.o timed_write.o translit.o trimblanks.o \
	unescape.o unix_connect.o unix_listen.o unix_trigger.o unsafe.o \
	username.o valid_hostname.o vbuf.o vbuf_print.o vstream.o \
	vstream_popen.o vstring.o vstring_vstream.o watchdog.o writable.o \
	write_buf.o write_wait.o auto_clnt.o attr_clnt.o attr_scan_plain.o \
	attr_print_plain.o sane_connect.o $(STRCASE) neuter.o name_code.o \
	uppercase.o
HDRS	= argv.h attr.h base64_code.h binhash.h chroot_uid.h clean_env.h \
	connect.h ctable.h dict.h dict_db.h dict_dbm.h dict_env.h \
	dict_cidr.h dict_ht.h dict_ni.h dict_nis.h \
	dict_nisplus.h dict_pcre.h dict_regexp.h \
	dict_static.h dict_tcp.h dict_unix.h dir_forest.h events.h \
	exec_command.h find_inet.h fsspace.h fullname.h get_domainname.h \
	get_hostname.h hex_quote.h host_port.h htable.h inet_addr_host.h \
	inet_addr_list.h inet_addr_local.h inet_util.h iostuff.h \
	line_wrap.h listen.h lstat_as.h mac_expand.h mac_parse.h \
	make_dirs.h match_list.h match_ops.h msg.h msg_output.h \
	msg_syslog.h msg_vstream.h mvect.h myflock.h mymalloc.h myrand.h \
	name_mask.h netstring.h nvtable.h open_as.h open_lock.h \
	percentm.h posix_signals.h readlline.h ring.h safe.h safe_open.h \
	sane_accept.h sane_fsops.h sane_socketpair.h sane_time.h \
	scan_dir.h set_eugid.h set_ugid.h sigdelay.h spawn_command.h \
	split_at.h stat_as.h stringops.h sys_defs.h timed_connect.h \
	timed_wait.h trigger.h username.h valid_hostname.h vbuf.h \
	vbuf_print.h vstream.h vstring.h vstring_vstream.h watchdog.h \
	auto_clnt.h attr_clnt.h sane_connect.h name_code.h
TESTSRC	= fifo_open.c fifo_rdwr_bug.c fifo_rdonly_bug.c select_bug.c \
	stream_test.c dup2_pass_on_exec.c
DEFS	= -I. -D$(SYSTYPE)
CFLAGS	= $(DEBUG) $(OPT) $(DEFS)
FILES	= Makefile $(SRCS) $(HDRS)
INCL	=
LIB	= libutil.a
TESTPROG= dict_open dup2_pass_on_exec events exec_command fifo_open \
	fifo_rdonly_bug fifo_rdwr_bug fifo_trigger fsspace fullname \
	inet_addr_host inet_addr_local mac_parse make_dirs msg_syslog \
	mystrtok sigdelay translit valid_hostname vstream_popen \
	vstring vstring_vstream doze select_bug stream_test mac_expand \
	watchdog unescape hex_quote name_mask rand_sleep sane_time ctable \
	inet_addr_list attr_print64 attr_scan64 base64_code attr_print0 \
	attr_scan0 host_port attr_scan_plain attr_print_plain htable

LIB_DIR	= ../../lib
INC_DIR	= ../../include

.c.o:;	$(CC) $(CFLAGS) -c $*.c

all: $(LIB)

Makefile: Makefile.in
	(set -e; echo "# DO NOT EDIT"; $(OPTS) $(SHELL) ../../makedefs && cat $?) >$@

test:	$(TESTPROG)

$(LIB):	$(OBJS)
	$(AR) $(ARFL) $(LIB) $?
	$(RANLIB) $(LIB)

$(LIB_DIR)/$(LIB): $(LIB)
	cp $(LIB) $(LIB_DIR)
	$(RANLIB) $(LIB_DIR)/$(LIB)

update: $(LIB_DIR)/$(LIB) $(HDRS)
	-for i in $(HDRS); \
	do \
	  cmp -s $$i $(INC_DIR)/$$i 2>/dev/null || cp $$i $(INC_DIR); \
	done
	cd $(INC_DIR); chmod 644 $(HDRS)

printfck: $(OBJS) $(PROG)
	rm -rf printfck
	mkdir printfck
	cp *.h printfck
	sed '1,/^# do not edit/!d' Makefile >printfck/Makefile
	set -e; for i in *.c; do printfck -f .printfck $$i >printfck/$$i; done
	cd printfck; make "INC_DIR=../../../include" `cd ..; ls *.o`

shar:
	@shar $(FILES)

lint:
	lint $(SRCS)

clean:
	rm -f *.o $(LIB) *core $(TESTPROG) junk $(MAKES) *.tmp
	rm -rf printfck

tidy:	clean

dup2_pass_on_exec: dup2_pass_on_exec.c
	$(CC) $(CFLAGS) -o $@ $@.c $(SYSLIBS)

vstring: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

msg_syslog: msg_syslog.c $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

vstring_vstream: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

valid_hostname: valid_hostname.c $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

events: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

dict_open: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

fullname: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

inet_addr_local: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

inet_addr_host: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

fifo_open: fifo_open.c
	$(CC) $(CFLAGS) -o $@ $@.c $(SYSLIBS)

sigdelay: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

mystrtok: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

fifo_rdwr_bug: fifo_rdwr_bug.c $(LIB)
	$(CC) $(CFLAGS)  -o $@ $@.c $(LIB) $(SYSLIBS)

fifo_rdonly_bug: fifo_rdonly_bug.c $(LIB)
	$(CC) $(CFLAGS)  -o $@ $@.c $(LIB) $(SYSLIBS)

select_bug: select_bug.c $(LIB)
	$(CC) $(CFLAGS)  -o $@ $@.c $(LIB) $(SYSLIBS)

translit: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

fsspace: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

exec_command: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

make_dirs: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

mac_parse: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

vstream_popen: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

fifo_trigger: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

doze: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

mac_expand: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

watchdog: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

unescape: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

hex_quote: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

name_mask: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

rand_sleep: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

sane_time: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

ctable: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

inet_addr_list: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

attr_print64: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

attr_scan64: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

base64_code: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

attr_print0: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

attr_scan0: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

host_port: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

attr_scan_plain: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

attr_print_plain: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

htable: $(LIB)
	mv $@.o junk
	$(CC) $(CFLAGS) -DTEST -o $@ $@.c $(LIB) $(SYSLIBS)
	mv junk $@.o

depend: $(MAKES)
	(sed '1,/^# do not edit/!d' Makefile.in; \
	set -e; for i in [a-z][a-z0-9]*.c; do \
	    $(CC) -E $(DEFS) $(INCL) $$i | sed -n -e '/^# *1 *"\([^"]*\)".*/{' \
	    -e 's//'`echo $$i|sed 's/c$$/o/'`': \1/' -e 'p' -e '}'; \
	done) | grep -v '[.][o][:][ ][/]' >$$$$ && mv $$$$ Makefile.in
	@$(EXPORT) make -f Makefile.in Makefile 1>&2

stream_test: stream_test.c $(LIB)
	$(CC) $(CFLAGS)  -o $@ $@.c $(LIB) $(SYSLIBS)

tests: valid_hostname_test mac_expand_test dict_test unescape_test \
	hex_quote_test ctable_test inet_addr_list_test base64_code_test \
	attr_scan64_test attr_scan0_test dict_pcre_test host_port_test \
	dict_cidr_test attr_scan_plain_test htable_test

valid_hostname_test: valid_hostname valid_hostname.in valid_hostname.ref
	./valid_hostname <valid_hostname.in 2>valid_hostname.tmp
	diff valid_hostname.ref valid_hostname.tmp
	rm -f valid_hostname.tmp

mac_expand_test: mac_expand mac_expand.in mac_expand.ref
	./mac_expand <mac_expand.in >mac_expand.tmp 2>&1
	diff mac_expand.ref mac_expand.tmp
	rm -f mac_expand.tmp

unescape_test: unescape unescape.in unescape.ref
	./unescape <unescape.in | od -cb >unescape.tmp
	diff -b unescape.ref unescape.tmp
	rm -f unescape.tmp

hex_quote_test: hex_quote
	./hex_quote <hex_quote.c | od -cb >hex_quote.tmp
	od -cb <hex_quote.c >hex_quote.ref
	cmp hex_quote.ref hex_quote.tmp
	rm -f hex_quote.ref hex_quote.tmp

ctable_test: ctable
	./ctable <ctable.in >ctable.tmp 2>&1
	diff ctable.ref ctable.tmp
	rm -f ctable.tmp

inet_addr_list_test: inet_addr_list
	./inet_addr_list `cat inet_addr_list.in` >inet_addr_list.tmp 2>&1
	diff inet_addr_list.ref inet_addr_list.tmp
	rm -f inet_addr_list.tmp

base64_code_test: base64_code
	./base64_code

attr_scan64_test: attr_print64 attr_scan64 attr_scan64.ref
	(./attr_print64 2>&3 | (sleep 1; ./attr_scan64)) >attr_scan64.tmp 2>&1 3>&1
	diff attr_scan64.ref attr_scan64.tmp
	rm -f attr_scan64.tmp

attr_scan0_test: attr_print0 attr_scan0 attr_scan0.ref
	(./attr_print0 2>&3 | (sleep 1; ./attr_scan0)) >attr_scan0.tmp 2>&1 3>&1
	diff attr_scan0.ref attr_scan0.tmp
	rm -f attr_scan0.tmp

dict_test: dict_open testdb dict_test.in dict_test.ref
	rm -f testdb.db testdb.dir testdb.pag
	../postmap/postmap -N hash:testdb
	./dict_open hash:testdb write < dict_test.in > dict_test.tmp 2>&1
	diff dict_test.ref dict_test.tmp
	../postmap/postmap -n hash:testdb
	./dict_open hash:testdb write < dict_test.in > dict_test.tmp 2>&1
	diff dict_test.ref dict_test.tmp
	rm -f testdb.db testdb.dir testdb.pag dict_test.tmp

dict_pcre_test: dict_open dict_pcre.in dict_pcre.map dict_pcre.ref
	./dict_open pcre:dict_pcre.map read <dict_pcre.in >dict_pcre.tmp 2>&1
	diff dict_pcre.ref dict_pcre.tmp
	rm -f dict_pcre.tmp

dict_regexp_test: dict_open dict_regexp.in dict_regexp.map dict_regexp.ref
	./dict_open regexp:dict_regexp.map read <dict_regexp.in >dict_regexp.tmp 2>&1
	diff dict_regexp.ref dict_regexp.tmp
	rm -f dict_regexp.tmp

dict_cidr_test: dict_open dict_cidr.in dict_cidr.map dict_cidr.ref
	./dict_open cidr:dict_cidr.map read <dict_cidr.in >dict_cidr.tmp 2>&1
	diff dict_cidr.ref dict_cidr.tmp
	rm -f dict_cidr.tmp

host_port_test: host_port host_port.in host_port.ref
	./host_port <host_port.in >host_port.tmp 2>&1
	diff host_port.ref host_port.tmp
	rm -f host_port.tmp

attr_scan_plain_test: attr_print_plain attr_scan_plain attr_scan_plain.ref
	(./attr_print_plain 2>&3 | (sleep 1; ./attr_scan_plain)) >attr_scan_plain.tmp 2>&1 3>&1
	diff attr_scan_plain.ref attr_scan_plain.tmp
	rm -f attr_scan_plain.tmp

htable_test: htable /usr/share/dict/words
	./htable < /usr/share/dict/words

# do not edit below this line - it is generated by 'make depend'
alldig.o: alldig.c
alldig.o: sys_defs.h
alldig.o: stringops.h
alldig.o: vstring.h
alldig.o: vbuf.h
argv.o: argv.c
argv.o: sys_defs.h
argv.o: mymalloc.h
argv.o: msg.h
argv.o: argv.h
argv_split.o: argv_split.c
argv_split.o: sys_defs.h
argv_split.o: mymalloc.h
argv_split.o: stringops.h
argv_split.o: vstring.h
argv_split.o: vbuf.h
argv_split.o: argv.h
attr_clnt.o: attr_clnt.c
attr_clnt.o: sys_defs.h
attr_clnt.o: msg.h
attr_clnt.o: mymalloc.h
attr_clnt.o: split_at.h
attr_clnt.o: vstream.h
attr_clnt.o: vbuf.h
attr_clnt.o: connect.h
attr_clnt.o: iostuff.h
attr_clnt.o: htable.h
attr_clnt.o: attr.h
attr_clnt.o: auto_clnt.h
attr_clnt.o: attr_clnt.h
attr_print0.o: attr_print0.c
attr_print0.o: sys_defs.h
attr_print0.o: msg.h
attr_print0.o: mymalloc.h
attr_print0.o: vstream.h
attr_print0.o: vbuf.h
attr_print0.o: htable.h
attr_print0.o: attr.h
attr_print64.o: attr_print64.c
attr_print64.o: sys_defs.h
attr_print64.o: msg.h
attr_print64.o: mymalloc.h
attr_print64.o: vstream.h
attr_print64.o: vbuf.h
attr_print64.o: htable.h
attr_print64.o: base64_code.h
attr_print64.o: vstring.h
attr_print64.o: attr.h
attr_print_plain.o: attr_print_plain.c
attr_print_plain.o: sys_defs.h
attr_print_plain.o: msg.h
attr_print_plain.o: mymalloc.h
attr_print_plain.o: vstream.h
attr_print_plain.o: vbuf.h
attr_print_plain.o: htable.h
attr_print_plain.o: attr.h
attr_scan0.o: attr_scan0.c
attr_scan0.o: sys_defs.h
attr_scan0.o: msg.h
attr_scan0.o: mymalloc.h
attr_scan0.o: vstream.h
attr_scan0.o: vbuf.h
attr_scan0.o: vstring.h
attr_scan0.o: vstring_vstream.h
attr_scan0.o: htable.h
attr_scan0.o: attr.h
attr_scan64.o: attr_scan64.c
attr_scan64.o: sys_defs.h
attr_scan64.o: msg.h
attr_scan64.o: mymalloc.h
attr_scan64.o: vstream.h
attr_scan64.o: vbuf.h
attr_scan64.o: vstring.h
attr_scan64.o: htable.h
attr_scan64.o: base64_code.h
attr_scan64.o: attr.h
attr_scan_plain.o: attr_scan_plain.c
attr_scan_plain.o: sys_defs.h
attr_scan_plain.o: msg.h
attr_scan_plain.o: mymalloc.h
attr_scan_plain.o: vstream.h
attr_scan_plain.o: vbuf.h
attr_scan_plain.o: vstring.h
attr_scan_plain.o: htable.h
attr_scan_plain.o: attr.h
auto_clnt.o: auto_clnt.c
auto_clnt.o: sys_defs.h
auto_clnt.o: msg.h
auto_clnt.o: mymalloc.h
auto_clnt.o: vstream.h
auto_clnt.o: vbuf.h
auto_clnt.o: events.h
auto_clnt.o: iostuff.h
auto_clnt.o: auto_clnt.h
base64_code.o: base64_code.c
base64_code.o: sys_defs.h
base64_code.o: msg.h
base64_code.o: mymalloc.h
base64_code.o: vstring.h
base64_code.o: vbuf.h
base64_code.o: base64_code.h
basename.o: basename.c
basename.o: sys_defs.h
basename.o: stringops.h
basename.o: vstring.h
basename.o: vbuf.h
binhash.o: binhash.c
binhash.o: sys_defs.h
binhash.o: mymalloc.h
binhash.o: msg.h
binhash.o: binhash.h
chroot_uid.o: chroot_uid.c
chroot_uid.o: sys_defs.h
chroot_uid.o: msg.h
chroot_uid.o: chroot_uid.h
clean_env.o: clean_env.c
clean_env.o: sys_defs.h
clean_env.o: msg.h
clean_env.o: argv.h
clean_env.o: safe.h
clean_env.o: clean_env.h
close_on_exec.o: close_on_exec.c
close_on_exec.o: sys_defs.h
close_on_exec.o: msg.h
close_on_exec.o: iostuff.h
concatenate.o: concatenate.c
concatenate.o: sys_defs.h
concatenate.o: mymalloc.h
concatenate.o: stringops.h
concatenate.o: vstring.h
concatenate.o: vbuf.h
ctable.o: ctable.c
ctable.o: sys_defs.h
ctable.o: msg.h
ctable.o: mymalloc.h
ctable.o: ring.h
ctable.o: htable.h
ctable.o: ctable.h
dict.o: dict.c
dict.o: sys_defs.h
dict.o: msg.h
dict.o: htable.h
dict.o: mymalloc.h
dict.o: vstream.h
dict.o: vbuf.h
dict.o: vstring.h
dict.o: readlline.h
dict.o: mac_parse.h
dict.o: stringops.h
dict.o: iostuff.h
dict.o: dict.h
dict.o: argv.h
dict.o: dict_ht.h
dict_alloc.o: dict_alloc.c
dict_alloc.o: sys_defs.h
dict_alloc.o: msg.h
dict_alloc.o: mymalloc.h
dict_alloc.o: dict.h
dict_alloc.o: vstream.h
dict_alloc.o: vbuf.h
dict_alloc.o: argv.h
dict_cidr.o: dict_cidr.c
dict_cidr.o: sys_defs.h
dict_cidr.o: mymalloc.h
dict_cidr.o: msg.h
dict_cidr.o: vstream.h
dict_cidr.o: vbuf.h
dict_cidr.o: vstring.h
dict_cidr.o: stringops.h
dict_cidr.o: readlline.h
dict_cidr.o: dict.h
dict_cidr.o: argv.h
dict_cidr.o: dict_cidr.h
dict_cidr.o: split_at.h
dict_db.o: dict_db.c
dict_db.o: sys_defs.h
dict_db.o: msg.h
dict_db.o: mymalloc.h
dict_db.o: vstring.h
dict_db.o: vbuf.h
dict_db.o: stringops.h
dict_db.o: iostuff.h
dict_db.o: myflock.h
dict_db.o: dict.h
dict_db.o: vstream.h
dict_db.o: argv.h
dict_db.o: dict_db.h
dict_dbm.o: dict_dbm.c
dict_dbm.o: sys_defs.h
dict_debug.o: dict_debug.c
dict_debug.o: sys_defs.h
dict_debug.o: msg.h
dict_debug.o: mymalloc.h
dict_debug.o: dict.h
dict_debug.o: vstream.h
dict_debug.o: vbuf.h
dict_debug.o: argv.h
dict_env.o: dict_env.c
dict_env.o: sys_defs.h
dict_env.o: mymalloc.h
dict_env.o: msg.h
dict_env.o: safe.h
dict_env.o: dict.h
dict_env.o: vstream.h
dict_env.o: vbuf.h
dict_env.o: argv.h
dict_env.o: dict_env.h
dict_ht.o: dict_ht.c
dict_ht.o: sys_defs.h
dict_ht.o: mymalloc.h
dict_ht.o: htable.h
dict_ht.o: dict.h
dict_ht.o: vstream.h
dict_ht.o: vbuf.h
dict_ht.o: argv.h
dict_ht.o: dict_ht.h
dict_ni.o: dict_ni.c
dict_ni.o: sys_defs.h
dict_nis.o: dict_nis.c
dict_nis.o: sys_defs.h
dict_nis.o: msg.h
dict_nis.o: mymalloc.h
dict_nis.o: vstring.h
dict_nis.o: vbuf.h
dict_nis.o: dict.h
dict_nis.o: vstream.h
dict_nis.o: argv.h
dict_nis.o: dict_nis.h
dict_nisplus.o: dict_nisplus.c
dict_nisplus.o: sys_defs.h
dict_nisplus.o: msg.h
dict_nisplus.o: mymalloc.h
dict_nisplus.o: htable.h
dict_nisplus.o: dict.h
dict_nisplus.o: vstream.h
dict_nisplus.o: vbuf.h
dict_nisplus.o: argv.h
dict_nisplus.o: dict_nisplus.h
dict_open.o: dict_open.c
dict_open.o: sys_defs.h
dict_open.o: argv.h
dict_open.o: mymalloc.h
dict_open.o: msg.h
dict_open.o: dict.h
dict_open.o: vstream.h
dict_open.o: vbuf.h
dict_open.o: dict_env.h
dict_open.o: dict_unix.h
dict_open.o: dict_tcp.h
dict_open.o: dict_dbm.h
dict_open.o: dict_db.h
dict_open.o: dict_nis.h
dict_open.o: dict_nisplus.h
dict_open.o: dict_ni.h
dict_open.o: dict_pcre.h
dict_open.o: dict_regexp.h
dict_open.o: dict_static.h
dict_open.o: dict_cidr.h
dict_open.o: stringops.h
dict_open.o: vstring.h
dict_open.o: split_at.h
dict_open.o: htable.h
dict_pcre.o: dict_pcre.c
dict_pcre.o: sys_defs.h
dict_pcre.o: mymalloc.h
dict_pcre.o: msg.h
dict_pcre.o: safe.h
dict_pcre.o: vstream.h
dict_pcre.o: vbuf.h
dict_pcre.o: vstring.h
dict_pcre.o: stringops.h
dict_pcre.o: readlline.h
dict_pcre.o: dict.h
dict_pcre.o: argv.h
dict_pcre.o: dict_pcre.h
dict_pcre.o: mac_parse.h
dict_regexp.o: dict_regexp.c
dict_regexp.o: sys_defs.h
dict_regexp.o: mymalloc.h
dict_regexp.o: msg.h
dict_regexp.o: safe.h
dict_regexp.o: vstream.h
dict_regexp.o: vbuf.h
dict_regexp.o: vstring.h
dict_regexp.o: stringops.h
dict_regexp.o: readlline.h
dict_regexp.o: dict.h
dict_regexp.o: argv.h
dict_regexp.o: dict_regexp.h
dict_regexp.o: mac_parse.h
dict_static.o: dict_static.c
dict_static.o: sys_defs.h
dict_static.o: mymalloc.h
dict_static.o: msg.h
dict_static.o: dict.h
dict_static.o: vstream.h
dict_static.o: vbuf.h
dict_static.o: argv.h
dict_static.o: dict_static.h
dict_tcp.o: dict_tcp.c
dict_tcp.o: sys_defs.h
dict_tcp.o: msg.h
dict_tcp.o: mymalloc.h
dict_tcp.o: vstring.h
dict_tcp.o: vbuf.h
dict_tcp.o: vstream.h
dict_tcp.o: vstring_vstream.h
dict_tcp.o: connect.h
dict_tcp.o: iostuff.h
dict_tcp.o: hex_quote.h
dict_tcp.o: dict.h
dict_tcp.o: argv.h
dict_tcp.o: stringops.h
dict_tcp.o: dict_tcp.h
dict_unix.o: dict_unix.c
dict_unix.o: sys_defs.h
dict_unix.o: msg.h
dict_unix.o: mymalloc.h
dict_unix.o: vstring.h
dict_unix.o: vbuf.h
dict_unix.o: dict.h
dict_unix.o: vstream.h
dict_unix.o: argv.h
dict_unix.o: dict_unix.h
dir_forest.o: dir_forest.c
dir_forest.o: sys_defs.h
dir_forest.o: msg.h
dir_forest.o: dir_forest.h
dir_forest.o: vstring.h
dir_forest.o: vbuf.h
doze.o: doze.c
doze.o: sys_defs.h
doze.o: msg.h
doze.o: iostuff.h
dup2_pass_on_exec.o: dup2_pass_on_exec.c
duplex_pipe.o: duplex_pipe.c
duplex_pipe.o: sys_defs.h
duplex_pipe.o: iostuff.h
duplex_pipe.o: sane_socketpair.h
environ.o: environ.c
environ.o: sys_defs.h
events.o: events.c
events.o: sys_defs.h
events.o: mymalloc.h
events.o: msg.h
events.o: iostuff.h
events.o: ring.h
events.o: events.h
exec_command.o: exec_command.c
exec_command.o: sys_defs.h
exec_command.o: msg.h
exec_command.o: argv.h
exec_command.o: exec_command.h
fifo_listen.o: fifo_listen.c
fifo_listen.o: sys_defs.h
fifo_listen.o: msg.h
fifo_listen.o: iostuff.h
fifo_listen.o: listen.h
fifo_open.o: fifo_open.c
fifo_rdonly_bug.o: fifo_rdonly_bug.c
fifo_rdonly_bug.o: sys_defs.h
fifo_rdwr_bug.o: fifo_rdwr_bug.c
fifo_rdwr_bug.o: sys_defs.h
fifo_trigger.o: fifo_trigger.c
fifo_trigger.o: sys_defs.h
fifo_trigger.o: msg.h
fifo_trigger.o: iostuff.h
fifo_trigger.o: safe_open.h
fifo_trigger.o: vstream.h
fifo_trigger.o: vbuf.h
fifo_trigger.o: vstring.h
fifo_trigger.o: trigger.h
file_limit.o: file_limit.c
file_limit.o: sys_defs.h
file_limit.o: msg.h
file_limit.o: iostuff.h
find_inet.o: find_inet.c
find_inet.o: sys_defs.h
find_inet.o: msg.h
find_inet.o: stringops.h
find_inet.o: vstring.h
find_inet.o: vbuf.h
find_inet.o: find_inet.h
fsspace.o: fsspace.c
fsspace.o: sys_defs.h
fsspace.o: msg.h
fsspace.o: fsspace.h
fullname.o: fullname.c
fullname.o: sys_defs.h
fullname.o: vstring.h
fullname.o: vbuf.h
fullname.o: safe.h
fullname.o: fullname.h
get_domainname.o: get_domainname.c
get_domainname.o: sys_defs.h
get_domainname.o: mymalloc.h
get_domainname.o: get_hostname.h
get_domainname.o: get_domainname.h
get_hostname.o: get_hostname.c
get_hostname.o: sys_defs.h
get_hostname.o: mymalloc.h
get_hostname.o: msg.h
get_hostname.o: valid_hostname.h
get_hostname.o: get_hostname.h
hex_quote.o: hex_quote.c
hex_quote.o: sys_defs.h
hex_quote.o: msg.h
hex_quote.o: vstring.h
hex_quote.o: vbuf.h
hex_quote.o: hex_quote.h
host_port.o: host_port.c
host_port.o: sys_defs.h
host_port.o: msg.h
host_port.o: split_at.h
host_port.o: stringops.h
host_port.o: vstring.h
host_port.o: vbuf.h
host_port.o: valid_hostname.h
host_port.o: host_port.h
htable.o: htable.c
htable.o: sys_defs.h
htable.o: mymalloc.h
htable.o: msg.h
htable.o: htable.h
inet_addr_host.o: inet_addr_host.c
inet_addr_host.o: sys_defs.h
inet_addr_host.o: inet_addr_list.h
inet_addr_host.o: inet_addr_host.h
inet_addr_list.o: inet_addr_list.c
inet_addr_list.o: sys_defs.h
inet_addr_list.o: msg.h
inet_addr_list.o: mymalloc.h
inet_addr_list.o: inet_addr_list.h
inet_addr_local.o: inet_addr_local.c
inet_addr_local.o: sys_defs.h
inet_addr_local.o: msg.h
inet_addr_local.o: mymalloc.h
inet_addr_local.o: vstring.h
inet_addr_local.o: vbuf.h
inet_addr_local.o: inet_addr_list.h
inet_addr_local.o: inet_addr_local.h
inet_connect.o: inet_connect.c
inet_connect.o: sys_defs.h
inet_connect.o: mymalloc.h
inet_connect.o: msg.h
inet_connect.o: find_inet.h
inet_connect.o: inet_util.h
inet_connect.o: iostuff.h
inet_connect.o: sane_connect.h
inet_connect.o: connect.h
inet_connect.o: timed_connect.h
inet_listen.o: inet_listen.c
inet_listen.o: sys_defs.h
inet_listen.o: mymalloc.h
inet_listen.o: msg.h
inet_listen.o: find_inet.h
inet_listen.o: inet_util.h
inet_listen.o: iostuff.h
inet_listen.o: listen.h
inet_listen.o: sane_accept.h
inet_trigger.o: inet_trigger.c
inet_trigger.o: sys_defs.h
inet_trigger.o: msg.h
inet_trigger.o: connect.h
inet_trigger.o: iostuff.h
inet_trigger.o: mymalloc.h
inet_trigger.o: events.h
inet_trigger.o: trigger.h
inet_util.o: inet_util.c
inet_util.o: sys_defs.h
inet_util.o: mymalloc.h
inet_util.o: split_at.h
inet_util.o: inet_util.h
line_wrap.o: line_wrap.c
line_wrap.o: sys_defs.h
line_wrap.o: line_wrap.h
lowercase.o: lowercase.c
lowercase.o: sys_defs.h
lowercase.o: stringops.h
lowercase.o: vstring.h
lowercase.o: vbuf.h
lstat_as.o: lstat_as.c
lstat_as.o: sys_defs.h
lstat_as.o: msg.h
lstat_as.o: set_eugid.h
lstat_as.o: lstat_as.h
mac_expand.o: mac_expand.c
mac_expand.o: sys_defs.h
mac_expand.o: msg.h
mac_expand.o: vstring.h
mac_expand.o: vbuf.h
mac_expand.o: mymalloc.h
mac_expand.o: mac_parse.h
mac_expand.o: mac_expand.h
mac_parse.o: mac_parse.c
mac_parse.o: sys_defs.h
mac_parse.o: msg.h
mac_parse.o: mac_parse.h
mac_parse.o: vstring.h
mac_parse.o: vbuf.h
make_dirs.o: make_dirs.c
make_dirs.o: sys_defs.h
make_dirs.o: msg.h
make_dirs.o: mymalloc.h
make_dirs.o: stringops.h
make_dirs.o: vstring.h
make_dirs.o: vbuf.h
make_dirs.o: make_dirs.h
match_list.o: match_list.c
match_list.o: sys_defs.h
match_list.o: msg.h
match_list.o: mymalloc.h
match_list.o: vstring.h
match_list.o: vbuf.h
match_list.o: vstream.h
match_list.o: vstring_vstream.h
match_list.o: stringops.h
match_list.o: argv.h
match_list.o: dict.h
match_list.o: match_ops.h
match_list.o: match_list.h
match_ops.o: match_ops.c
match_ops.o: sys_defs.h
match_ops.o: msg.h
match_ops.o: mymalloc.h
match_ops.o: split_at.h
match_ops.o: dict.h
match_ops.o: vstream.h
match_ops.o: vbuf.h
match_ops.o: argv.h
match_ops.o: match_ops.h
match_ops.o: stringops.h
match_ops.o: vstring.h
msg.o: msg.c
msg.o: sys_defs.h
msg.o: msg.h
msg.o: msg_output.h
msg_output.o: msg_output.c
msg_output.o: sys_defs.h
msg_output.o: mymalloc.h
msg_output.o: vstring.h
msg_output.o: vbuf.h
msg_output.o: vstream.h
msg_output.o: msg_vstream.h
msg_output.o: stringops.h
msg_output.o: percentm.h
msg_output.o: msg_output.h
msg_syslog.o: msg_syslog.c
msg_syslog.o: sys_defs.h
msg_syslog.o: vstring.h
msg_syslog.o: vbuf.h
msg_syslog.o: stringops.h
msg_syslog.o: msg.h
msg_syslog.o: msg_output.h
msg_syslog.o: msg_syslog.h
msg_syslog.o: safe.h
msg_vstream.o: msg_vstream.c
msg_vstream.o: sys_defs.h
msg_vstream.o: vstream.h
msg_vstream.o: vbuf.h
msg_vstream.o: msg.h
msg_vstream.o: msg_output.h
msg_vstream.o: msg_vstream.h
mvect.o: mvect.c
mvect.o: sys_defs.h
mvect.o: mymalloc.h
mvect.o: mvect.h
myflock.o: myflock.c
myflock.o: sys_defs.h
myflock.o: msg.h
myflock.o: myflock.h
mymalloc.o: mymalloc.c
mymalloc.o: sys_defs.h
mymalloc.o: msg.h
mymalloc.o: mymalloc.h
myrand.o: myrand.c
myrand.o: sys_defs.h
myrand.o: myrand.h
mystrtok.o: mystrtok.c
mystrtok.o: sys_defs.h
mystrtok.o: stringops.h
mystrtok.o: vstring.h
mystrtok.o: vbuf.h
name_code.o: name_code.c
name_code.o: sys_defs.h
name_code.o: name_code.h
name_mask.o: name_mask.c
name_mask.o: sys_defs.h
name_mask.o: msg.h
name_mask.o: mymalloc.h
name_mask.o: stringops.h
name_mask.o: vstring.h
name_mask.o: vbuf.h
name_mask.o: name_mask.h
netstring.o: netstring.c
netstring.o: sys_defs.h
netstring.o: msg.h
netstring.o: vstream.h
netstring.o: vbuf.h
netstring.o: vstring.h
netstring.o: netstring.h
neuter.o: neuter.c
neuter.o: sys_defs.h
neuter.o: stringops.h
neuter.o: vstring.h
neuter.o: vbuf.h
non_blocking.o: non_blocking.c
non_blocking.o: sys_defs.h
non_blocking.o: msg.h
non_blocking.o: iostuff.h
nvtable.o: nvtable.c
nvtable.o: sys_defs.h
nvtable.o: mymalloc.h
nvtable.o: htable.h
nvtable.o: nvtable.h
open_as.o: open_as.c
open_as.o: sys_defs.h
open_as.o: msg.h
open_as.o: set_eugid.h
open_as.o: open_as.h
open_limit.o: open_limit.c
open_limit.o: sys_defs.h
open_limit.o: iostuff.h
open_lock.o: open_lock.c
open_lock.o: sys_defs.h
open_lock.o: msg.h
open_lock.o: vstream.h
open_lock.o: vbuf.h
open_lock.o: vstring.h
open_lock.o: safe_open.h
open_lock.o: myflock.h
open_lock.o: open_lock.h
peekfd.o: peekfd.c
peekfd.o: sys_defs.h
peekfd.o: iostuff.h
percentm.o: percentm.c
percentm.o: sys_defs.h
percentm.o: vstring.h
percentm.o: vbuf.h
percentm.o: percentm.h
posix_signals.o: posix_signals.c
posix_signals.o: sys_defs.h
posix_signals.o: posix_signals.h
printable.o: printable.c
printable.o: sys_defs.h
printable.o: stringops.h
printable.o: vstring.h
printable.o: vbuf.h
rand_sleep.o: rand_sleep.c
rand_sleep.o: sys_defs.h
rand_sleep.o: msg.h
rand_sleep.o: myrand.h
rand_sleep.o: iostuff.h
read_wait.o: read_wait.c
read_wait.o: sys_defs.h
read_wait.o: msg.h
read_wait.o: iostuff.h
readable.o: readable.c
readable.o: sys_defs.h
readable.o: msg.h
readable.o: iostuff.h
readlline.o: readlline.c
readlline.o: sys_defs.h
readlline.o: msg.h
readlline.o: vstream.h
readlline.o: vbuf.h
readlline.o: vstring.h
readlline.o: readlline.h
ring.o: ring.c
ring.o: ring.h
safe_getenv.o: safe_getenv.c
safe_getenv.o: sys_defs.h
safe_getenv.o: safe.h
safe_open.o: safe_open.c
safe_open.o: sys_defs.h
safe_open.o: msg.h
safe_open.o: vstream.h
safe_open.o: vbuf.h
safe_open.o: vstring.h
safe_open.o: safe_open.h
sane_accept.o: sane_accept.c
sane_accept.o: sys_defs.h
sane_accept.o: msg.h
sane_accept.o: sane_accept.h
sane_connect.o: sane_connect.c
sane_connect.o: sys_defs.h
sane_connect.o: msg.h
sane_connect.o: sane_connect.h
sane_link.o: sane_link.c
sane_link.o: sys_defs.h
sane_link.o: msg.h
sane_link.o: sane_fsops.h
sane_rename.o: sane_rename.c
sane_rename.o: sys_defs.h
sane_rename.o: msg.h
sane_rename.o: sane_fsops.h
sane_socketpair.o: sane_socketpair.c
sane_socketpair.o: sys_defs.h
sane_socketpair.o: msg.h
sane_socketpair.o: sane_socketpair.h
sane_time.o: sane_time.c
sane_time.o: sys_defs.h
sane_time.o: msg.h
sane_time.o: sane_time.h
scan_dir.o: scan_dir.c
scan_dir.o: sys_defs.h
scan_dir.o: msg.h
scan_dir.o: mymalloc.h
scan_dir.o: stringops.h
scan_dir.o: vstring.h
scan_dir.o: vbuf.h
scan_dir.o: scan_dir.h
select_bug.o: select_bug.c
select_bug.o: sys_defs.h
select_bug.o: msg.h
select_bug.o: vstream.h
select_bug.o: vbuf.h
select_bug.o: msg_vstream.h
set_eugid.o: set_eugid.c
set_eugid.o: sys_defs.h
set_eugid.o: msg.h
set_eugid.o: set_eugid.h
set_ugid.o: set_ugid.c
set_ugid.o: sys_defs.h
set_ugid.o: msg.h
set_ugid.o: set_ugid.h
sigdelay.o: sigdelay.c
sigdelay.o: sys_defs.h
sigdelay.o: msg.h
sigdelay.o: posix_signals.h
sigdelay.o: sigdelay.h
skipblanks.o: skipblanks.c
skipblanks.o: sys_defs.h
skipblanks.o: stringops.h
skipblanks.o: vstring.h
skipblanks.o: vbuf.h
spawn_command.o: spawn_command.c
spawn_command.o: sys_defs.h
spawn_command.o: msg.h
spawn_command.o: timed_wait.h
spawn_command.o: set_ugid.h
spawn_command.o: argv.h
spawn_command.o: spawn_command.h
spawn_command.o: exec_command.h
spawn_command.o: clean_env.h
split_at.o: split_at.c
split_at.o: sys_defs.h
split_at.o: split_at.h
split_nameval.o: split_nameval.c
split_nameval.o: sys_defs.h
split_nameval.o: msg.h
split_nameval.o: stringops.h
split_nameval.o: vstring.h
split_nameval.o: vbuf.h
stat_as.o: stat_as.c
stat_as.o: sys_defs.h
stat_as.o: msg.h
stat_as.o: set_eugid.h
stat_as.o: stat_as.h
strcasecmp.o: strcasecmp.c
strcasecmp.o: sys_defs.h
stream_connect.o: stream_connect.c
stream_connect.o: sys_defs.h
stream_connect.o: msg.h
stream_connect.o: connect.h
stream_connect.o: iostuff.h
stream_listen.o: stream_listen.c
stream_listen.o: sys_defs.h
stream_listen.o: msg.h
stream_listen.o: listen.h
stream_listen.o: iostuff.h
stream_test.o: stream_test.c
stream_test.o: sys_defs.h
stream_test.o: stream_test.c
stream_test.o: iostuff.h
stream_test.o: msg.h
stream_test.o: msg_vstream.h
stream_test.o: vstream.h
stream_test.o: vbuf.h
stream_test.o: listen.h
stream_test.o: connect.h
stream_trigger.o: stream_trigger.c
stream_trigger.o: sys_defs.h
stream_trigger.o: msg.h
stream_trigger.o: connect.h
stream_trigger.o: iostuff.h
stream_trigger.o: mymalloc.h
stream_trigger.o: events.h
stream_trigger.o: trigger.h
sys_compat.o: sys_compat.c
sys_compat.o: sys_defs.h
timed_connect.o: timed_connect.c
timed_connect.o: sys_defs.h
timed_connect.o: msg.h
timed_connect.o: iostuff.h
timed_connect.o: sane_connect.h
timed_connect.o: timed_connect.h
timed_read.o: timed_read.c
timed_read.o: sys_defs.h
timed_read.o: msg.h
timed_read.o: iostuff.h
timed_wait.o: timed_wait.c
timed_wait.o: sys_defs.h
timed_wait.o: msg.h
timed_wait.o: posix_signals.h
timed_wait.o: timed_wait.h
timed_write.o: timed_write.c
timed_write.o: sys_defs.h
timed_write.o: msg.h
timed_write.o: iostuff.h
translit.o: translit.c
translit.o: sys_defs.h
translit.o: stringops.h
translit.o: vstring.h
translit.o: vbuf.h
trimblanks.o: trimblanks.c
trimblanks.o: sys_defs.h
trimblanks.o: stringops.h
trimblanks.o: vstring.h
trimblanks.o: vbuf.h
unescape.o: unescape.c
unescape.o: sys_defs.h
unescape.o: vstring.h
unescape.o: vbuf.h
unescape.o: stringops.h
unix_connect.o: unix_connect.c
unix_connect.o: sys_defs.h
unix_connect.o: msg.h
unix_connect.o: iostuff.h
unix_connect.o: sane_connect.h
unix_connect.o: connect.h
unix_connect.o: timed_connect.h
unix_listen.o: unix_listen.c
unix_listen.o: sys_defs.h
unix_listen.o: msg.h
unix_listen.o: iostuff.h
unix_listen.o: listen.h
unix_listen.o: sane_accept.h
unix_trigger.o: unix_trigger.c
unix_trigger.o: sys_defs.h
unix_trigger.o: msg.h
unix_trigger.o: connect.h
unix_trigger.o: iostuff.h
unix_trigger.o: mymalloc.h
unix_trigger.o: events.h
unix_trigger.o: trigger.h
unsafe.o: unsafe.c
unsafe.o: sys_defs.h
unsafe.o: safe.h
uppercase.o: uppercase.c
uppercase.o: sys_defs.h
uppercase.o: stringops.h
uppercase.o: vstring.h
uppercase.o: vbuf.h
username.o: username.c
username.o: sys_defs.h
username.o: username.h
valid_hostname.o: valid_hostname.c
valid_hostname.o: sys_defs.h
valid_hostname.o: msg.h
valid_hostname.o: mymalloc.h
valid_hostname.o: stringops.h
valid_hostname.o: vstring.h
valid_hostname.o: vbuf.h
valid_hostname.o: valid_hostname.h
vbuf.o: vbuf.c
vbuf.o: sys_defs.h
vbuf.o: vbuf.h
vbuf_print.o: vbuf_print.c
vbuf_print.o: sys_defs.h
vbuf_print.o: msg.h
vbuf_print.o: vbuf.h
vbuf_print.o: vstring.h
vbuf_print.o: vbuf_print.h
vstream.o: vstream.c
vstream.o: sys_defs.h
vstream.o: mymalloc.h
vstream.o: msg.h
vstream.o: vbuf_print.h
vstream.o: vbuf.h
vstream.o: iostuff.h
vstream.o: vstring.h
vstream.o: vstream.h
vstream_popen.o: vstream_popen.c
vstream_popen.o: sys_defs.h
vstream_popen.o: msg.h
vstream_popen.o: exec_command.h
vstream_popen.o: vstream.h
vstream_popen.o: vbuf.h
vstream_popen.o: argv.h
vstream_popen.o: set_ugid.h
vstream_popen.o: clean_env.h
vstream_popen.o: iostuff.h
vstring.o: vstring.c
vstring.o: sys_defs.h
vstring.o: mymalloc.h
vstring.o: msg.h
vstring.o: vbuf_print.h
vstring.o: vbuf.h
vstring.o: vstring.h
vstring_vstream.o: vstring_vstream.c
vstring_vstream.o: sys_defs.h
vstring_vstream.o: msg.h
vstring_vstream.o: vstring.h
vstring_vstream.o: vbuf.h
vstring_vstream.o: vstream.h
vstring_vstream.o: vstring_vstream.h
watchdog.o: watchdog.c
watchdog.o: sys_defs.h
watchdog.o: posix_signals.h
watchdog.o: msg.h
watchdog.o: mymalloc.h
watchdog.o: watchdog.h
writable.o: writable.c
writable.o: sys_defs.h
writable.o: msg.h
writable.o: iostuff.h
write_buf.o: write_buf.c
write_buf.o: sys_defs.h
write_buf.o: msg.h
write_buf.o: iostuff.h
write_wait.o: write_wait.c
write_wait.o: sys_defs.h
write_wait.o: msg.h
write_wait.o: iostuff.h
