CRUX_VERSION = $(shell crux | head -n 1 | sed -e 's/CRUX version //' -e 's/\./_/')
ileniadir = /etc/ports
ilenia_DATA = favoriterepo \
	      favoriteversion \
	      crux.cvsup \
	      crux-it.cvsup \
	      clc.cvsup

EXTRA_DIST = favoriterepo \
	     favoriteversion \
	     crux.cvsup.in \
	     crux-it.cvsup.in \
	     clc.cvsup.in

all-am: crux.cvsup crux-it.cvsup clc.cvsup

%: %.in
	sed -e "s/#VERSION#/$(VERSION)/" -e "s/#CRUX_VERSION#/$(CRUX_VERSION)/" $< > $@

clean-generic:
	-rm -f crux.cvsup crux-it.cvsup clc.cvsup	
