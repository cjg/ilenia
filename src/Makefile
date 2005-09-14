OWNER=root
GROUP=bin
CFLAGS= -O
LDFLAGS= -s
CC=cc
GET=co
SRCS=list.c list.h update-repos.c
OBJS=list.o update-repos.o 
BINDIR=/usr/bin
INSTALL=ginstall

all:	update-repos

$(SRCS):
	$(GET) $@
	$(CC) $(CFLAGS) -c $*.c

update-repos: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

install: update-repos
	$(INSTALL) -c -o $(OWNER) -g $(GROUP) -m 6755 update-repos $(BINDIR)

clean:
	-rm -rf update-repos *~ *.o 
