man_MANS = ilenia.8

EXTRA_DIST = ilenia.8.in

%: %.in
	sed -e "s/#VERSION#/$(VERSION)/" $< > $@
	
clean-generic:
	-rm -f ilenia.8
