#! /bin/sh

LD_LIBRARY_PATH_BACKUP=${LD_LIBRARY_PATH}; export LD_LIBRARY_PATH_BACKUP
LD_LIBRARY_PATH=""; export LD_LIBRARY_PATH
make -f Makefile.in makefiles \
	AUXLIBS='-L/usr/local/lib/mysql -lmysqlclient -lz -lm -L/usr/local/lib -lpcre -lcrypt' \
	CCARGS='-DHAS_MYSQL -I/usr/local/include/mysql'

LD_LIBRARY_PATH=${LD_LIBRARY_PATH_BACKUP}; export LD_LIBRARY_PATH
LD_LIBRARY_PATH_BACKUP=""; export LD_LIBRARY_PATH_BACKUP
