# Generated automatically from Makefile.in by configure.
#

# ./Make.common.in	 -*- Makefile -*-

DATE="20 APRIL 1999"#
MAJORVERSION=1#
MINORVERSION=50#
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
CFLAGS = -O2 -fpic
CPPFLAGS =  
LDFLAGS = 
DEFS = -DHAVE_CONFIG_H
LIBS =  -ljpeg -ltiff -lpng -lz  -lm
LINC = -I. -I./../ -I./../../
XINC =  
XLIB = -lXext  -lX11  -lSM -lICE -lX11 

# End of Make.common



all static shared:


install-shared install-static: install

install:
	$(CP) EZ.h $(destincludedir)

clean realclean:

mrproper:
	- $(RMF) Makefile
