############
# PJCHAT - A DEC112 chat client console
# (C) Wolfgang Kampichler 2020
# License: GNU GPL Version 3 or later, please see attached gpl-3.0.txt
# or http://www.gnu.org/copyleft/gpl.html
#############
PREFIX = /usr/local

LXML_CFLAGS=$(shell pkg-config --cflags libxml-2.0)
LXML_LDFLAGS=$(shell pkg-config --libs libxml-2.0)

LYML_CFLAGS=$(shell pkg-config --cflags yaml-0.1)
LYML_LDFLAGS=$(shell pkg-config --libs yaml-0.1)

PJ_CFLAGS=$(shell pkg-config --cflags libpjproject)
PJ_LDFLAGS=$(shell pkg-config --libs libpjproject)

CFLAGS  := -g -O0 -Wall -Werror=implicit-function-declaration -Werror=implicit-int $(LXML_CFLAGS) $(YML_CFLAGS) $(PJ_CFLAGS)
LDFLAGS := -Wl,--export-dynamic -lrt $(LDFLAGS)
LDLIBS  := $(LXML_LDFLAGS) $(LYML_LDFLAGS) $(PJ_LDFLAGS)

all: pjchat

pjchat.o: pjchat.c Makefile

functions.o: functions.c functions.h

pjchat: pjchat.o functions.o

clean:
	-rm *.o
	-rm pjchat

release: CFLAGS  := $(CFLAGS) -DRELEASE -O2
release: LDFLAGS := $(LDFLAGS) -Wl,--sort-common,-s
release: all

install: pjchat
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/pjchat

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/pjchat

.PHONY: all clean release install uninstall
