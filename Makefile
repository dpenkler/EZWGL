# Generated automatically from Makefile.in by configure.
# Generated automatically from Makefile.in by configure.
# ./Makefile.in			-*- Makefile -*-

# ./Make.common.in	 -*- Makefile -*-

DATE="02 MARCH 2016"#
MAJORVERSION=1#
MINORVERSION=51#
VERSION=$(MAJORVERSION).$(MINORVERSION)#
VERNAME=EZWGL-$(VERSION)#
srcdir = .
SHELL = /bin/sh

# Installation target directories 
prefix = /usr/local
exec_prefix = ${prefix}
binprefix = /usr/local/bin
manprefix = /usr/local/man
destbindir= ${exec_prefix}/bin
destincludedir= /usr/local/include
destlibdir= /usr/local/lib
destmandir= ${prefix}/man/man1
manext = 1

# Tools & program stuff
CC = gcc 
CPP = gcc -E
RM = rm
RMF = rm -f
CP = cp
AR = ar
LN = ln
RANLIB = ranlib
INSTALL = /usr/bin/ginstall -c
INSTALL_PROGRAM = /usr/bin/ginstall -c -s -m 755
INSTALL_DATA = /usr/bin/ginstall -c -m 644
CFLAGS = -g
CPPFLAGS =  
LDFLAGS = 
DEFS = -DHAVE_CONFIG_H
LIBS =  -ljpeg -ltiff -lpng -lz  -lm
LINC = -I. -I./../ -I./../../
XINC =  
XLIB = -lXext  -lX11  -lSM -lICE -lX11 

# End of Make.common

#
basedir     = $(srcdir)
libdir      = $(basedir)/lib
includedir  = $(basedir)/include
demosdir    = $(basedir)/demos
examplesdir = $(basedir)/examples
docdir      = $(basedir)/doc
thisdir     =./
#

first_rule: all
dummy:

subdirs =    ${libdir} ${includedir} 
sampledirs = ${demosdir} ${examplesdir} 

DIST =	README Make.common.in Makefile.in config.h.in configure\
	configure.in install-sh

#-------------------------------------------------------------------------

all shared static:
	@for I in ${subdirs}; do (cd $$I; ${MAKE} $@ || exit 1); done
	@for I in ${sampledirs}; do (cd $$I; ${MAKE}); done


install install-shared install-static:
	@for I in $(subdirs); do (cd $$I; $(MAKE) DESTDIR=/usr/local \
		 $@ || exit 1); done

clean:
	- $(RMF) -f config.cache
	@for I in ${subdirs}; do (cd $$I; ${MAKE} $@ || exit 1); done
	@for I in ${sampledirs}; do (cd $$I; ${MAKE} $@); done

#-------------------------------------------------------------------------

configure: configure.in
	cd $(srcdir); autoconf --localdir=$(srcdir) $(srcdir)/configure.in \
		 > configure
	chmod 755 configure

config.h.in: configure.in
	cd $(srcdir); autoheader

Makefiles:
	$(SHELL) config.status

config.status:
	if test -x config.status; then config.status --recheck; \
	else $(SHELL) configure; fi

realclean: clean
	- $(RMF) *~ config.cache
	@for I in ${subdirs}; do (cd $$I; ${MAKE} $@ || exit 1); done

#-------------------------------------------------------------------------

# distclean goal is for making a clean source tree, but if you have run
# configure from a different directory, then doesn't destroy all your
# hardly compiled and linked stuff. That's why there is always $(srcdir)/
# In that case most of those commands do nothing, except cleaning *~
# and cleaning source links.
mrproper:
	(cd $(srcdir); $(RMF) *~ config.cache config.h config.log config.status)
	@for I in $(subdirs); do (cd $$I; $(MAKE) $@ || exit 1); done
	(cd $(srcdir); $(RMF) Makefile Make.common)

../$(VERNAME).tar.gz: 
	- $(RMF) -f ../$(VERNAME).tar.gz
	(cd ..; tar cvf - $(VERNAME) | gzip -f9 > $(VERNAME).tar.gz)

tar.gz:  ../$(VERNAME).tar.gz

uuencode: tar.gz
	uuencode ../$(VERNAME).tar.gz $(VERNAME).tar.gz > ../$(VERNAME).tgz.uu

distrib: mrproper configure config.h.in uuencode

# ------------------------------------------------------------------------








