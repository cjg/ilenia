if !DEBUG
DEBUG_FLAG = -DNDEBUG 
else
DEBUG_FLAG = -O0 -g -Wall -Wpointer-arith -Wstrict-prototypes -W
endif

if BAD_MAINTAINERS
BAD_MAINTAINERS_FLAG = -D_ENABLE_BAD_MAINTAINERS
endif

bin_PROGRAMS = ilenia
ilenia_SOURCES =\
	ilenia.c\
	memory.h\
	memory.c\
	str.h\
	str.c\
	list.h\
	list.c\
	dict.h\
	dict.c\
	repository.h\
	repository.c\
	port.h\
	port.c\
	dependencies.h\
	dependencies.c\
	package.h\
	package.c\
	ini.h\
	ini.c\
	conf.h\
	conf.c\
	cache.h\
	cache.c\
	exec.h\
	exec.c\
	driver.h\
	driver.c\
	rsync.h\
	rsync.c\
	httpup.h\
	httpup.c\
	cvs.h\
	cvs.c\
	local.h\
	local.c\
	output.h\
	output.c\
	info.h\
	info.c\
	update.h\
	update.c\
	file.h\
	file.c\
	job.h\
	job.c\
	remove.h\
	remove.c

AM_CFLAGS = -D_GNU_SOURCE $(BAD_MAINTAINERS_FLAG) $(DEBUG_FLAG)
ilenia_LDADD = -lpthread

