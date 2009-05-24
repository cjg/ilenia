SUBDIRS = src conf doc
EXTRA_DIST = TODO

TODO:
	grep -nH -e 'TODO:' -e 'FIXME:' src/*.[ch] | awk '{ sub(/\/\//, ""); sub(/\/\*/,""); gsub (/:[\ \t]*TODO/, ": TODO"); gsub (/:[\ \t]*FIXME/, ": FIXME"); print}' > TODO
