#
# Creator  : gm 0.4
# Copyright: 2002, 2004, 2005 - Thomas Graf
# Date     : 2007.04.16 [YYYY.MM.DD]
#
# Authors  : Thomas Graf (original)
#            Udo Rader <udo.rader@bestsolution.at>

LEX      = flex
LFLAGS   = -i -8 -o
CC       = gcc
CXX      = c++
CPPFLAGS = -g3
INCLUDE  = -I.
WARNING  = -Wall #-std=c99 -pedantic 
CFLAGS   = $(CPPFLAGS) $(INCLUDE) $(WARNING)  -O2 -D_FORTIFY_SOURCE=0
CXXFLAGS = $(CPPFLAGS) $(INCLUDE) $(WARNING) -O2
LDFLAGS  = -s
LIBS     =

SRC      = ft.c
OBJ      = ft.o

TARGET   = ft

.SUFFIXES:
.SUFFIXES: .C .S .c .cc .cpp .cxx .l .o .s

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS) $(LIBS)

.S.o: 
	$(CC) $(CFLAGS) -c $<
.s.o: 
	$(CC) $(CFLAGS) -c $<
.l.c: 
	$(LEX) $(LFLAGS)$@ $<
.c.o: 
	$(CC) $(CFLAGS) -c $<
.C.o: 
	$(CXX) $(CXXFLAGS) -c $<
.cc.o:  
	$(CXX) $(CXXFLAGS) -c $<
.cxx.o:  
	$(CXX) $(CXXFLAGS) -c $<
.cpp.o:  
	$(CXX) $(CXXFLAGS) -c $<

clean: 
	rm -f $(OBJ) $(TARGET)

new: clean all

prefix   = /usr/local
install-strip: install
install : all
	install -c -d -m 755 $(prefix)/bin
	install -c -s -m 755 $(TARGET) $(prefix)/bin

MAKEDEP  = ( sed '/^\# DO NOT DELETE/q' < Makefile && \
	$(CC) -MM $(CFLAGS) $(INCLUDES) $(SRC) ) > .tmp && mv .tmp Makefile

depend: dep
dep:
	$(MAKEDEP)

tags:
	find -type f | \
	egrep "(*\.C|*\.H|*\.cpp|*\.c|*\.cc|*.cxx|*\.h|*\.hxx|*\.hpp)$$" | etags -

VERSION  = 
# VERSION = -0.1
# --> DISTFILE=NAME-0.1.tgz
PACKAGE  = $(TARGET)$(VERSION)
DISTFILE = $(PACKAGE).tar.gz
ZIPFILE  = $(PACKAGE).zip

dist-clean: distclean
distclean: clean
	rm -rf dist *~ ~* *\# \#* *.core core *,v .gdb* *.bak *.BAK *VS \
	$(DISTFILE) $(ZIPFILE) $(TARGET).tar.gz $(TARGET).zip

tgz: dist
dist: 
	rm -rf dist $(DISTFILE)
	mkdir -p dist/$(PACKAGE)
	find \( -type f -o -type l \) | egrep -v "^\./dist|RCS" > dist/filelist
	tar -c -T dist/filelist -f -| tar -xf - -C  dist/$(PACKAGE)
	make -C dist/$(PACKAGE) distclean
	tar -czf $(DISTFILE) -C dist $(PACKAGE)
	rm -rf dist

zip: 
	rm -rf dist $(ZIPFILE)
	mkdir -p dist/$(PACKAGE)
	find \( -type f -o -type l \) | egrep -v "^\./dist|RCS" > dist/filelist
	tar -c -T dist/filelist -f -| tar -xf - -C  dist/$(PACKAGE)
	make -C dist/$(PACKAGE) distclean
	cd dist ; zip -r9q ../$(ZIPFILE) $(PACKAGE)
	rm -rf dist

nice: indent
indent: 
	indent -kr -i8 -pmt $(SRC)

x: ${TARGET}
	./${TARGET}

.PHONY: all clean dep depend dist distclean dist-clean \
	indent install install-strip new nice tags tgz zip x

-include Makefile.inc # '-' don't stop and suppress error messages 
-include Makefile.dep # '-' don't stop and suppress error messages 
-include makefile.inc # '-' don't stop and suppress error messages 
-include makefile.dep # '-' don't stop and suppress error messages 

# DO NOT DELETE
