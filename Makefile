# crashdetect makefile
#
# To compile crahdetect on Linux run make as follows:
#   make os=linux config=release
# Similarly, to build on Windows with MinGW/Cygwin:
#   make os=windows config=release
#
# Build prerequisites:
#  1) Programs
#     - gcc
#     - g++
#     - GNU make
#     - sed
#     - windres (Windows only)
#  1) Libraries
#     - Boost.Filesystem
#     - Boost.System 

CC  = gcc
CXX = g++

ifndef config
config = release
endif

ifeq ($(config),debug)
CFLAGS += -g
CFLAGS += -Wall
endif

ifeq ($(config),release)
CFLAGS += -O2
CFLAGS += -Wno-attributes
endif

CFLAGS  += -m32
LDFLAGS += -m32

ifeq ($(os),windows)
CPPFLAGS += -D_WIN32 
LDFLAGS  += -Wl,--kill-at --def crashdetect.def
endif

ifeq ($(os),linux)
CPPFLAGS += -DLINUX
CFLAGS   += -fPIC
LDFLAGS  += -Wl,--no-undefined 
LDFLAGS  += -fPIC
endif

CPPFLAGS += -Ilib 
CPPFLAGS += -Ilib/amx 
CPPFLAGS += -I$(BOOST_ROOT)
CPPFLAGS += -DHAVE_STDINT_H 
CPPFLAGS += -include stddef.h

CXXFLAGS += -std=c++98
CXXFLAGS += $(CFLAGS)

LDFLAGS  += -L$(BOOST_ROOT)/stage/lib 
LDFLAGS  += -lboost_filesystem 
LDFLAGS  += -lboost_system 
LDFLAGS  += -ldl
LDFLAGS  += -shared

objects = lib/amx/amx.c.o \
	  lib/amx/amxaux.c.o \
	  lib/amx/amxdbg.c.o \
          src/amxcallstack.cpp.o \
          src/amxdebuginfo.cpp.o \
          src/amxpathfinder.cpp.o \
          src/crash.cpp.o \
          src/crashdetect.cpp.o \
          src/interrupt.cpp.o \
          src/jump-x86.cpp.o

ifeq ($(os),windows)
objects += src/crashdetect.rc.o
endif

ifeq ($(os),windows)
binary = crashdetect.dll
endif

ifeq ($(os),linux)
binary = crashdetect.so
endif

all: $(binary)

$(binary): $(objects)
	$(CXX) $(LDFLAGS) -o $@ $?

lib/amx/amx.c.o: lib/amx/amx.c lib/amx/amx.h lib/amx/sclinux.h lib/amx/getch.h lib/amx/osdefs.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

lib/amx/amxaux.c.o: lib/amx/amxaux.c lib/amx/amxaux.h lib/amx/amx.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

lib/amx/amxdbg.c.o: lib/amx/amxdbg.c lib/amx/amxdbg.h lib/amx/amx.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/amxcallstack.cpp.o: src/amxcallstack.cpp src/amxcallstack.h src/amxdebuginfo.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

src/amxdebuginfo.cpp.o: src/amxdebuginfo.cpp src/amxdebuginfo.h lib/amx/amx.h lib/amx/amxdbg.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

src/amxpathfinder.cpp.o: src/amxpathfinder.cpp src/amxpathfinder.h lib/amx/amx.h lib/amx/amxaux.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

src/crash.cpp.o: src/crash.cpp src/crash.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	
src/crashdetect.cpp.o: src/crashdetect.cpp src/crashdetect.h src/amxcallstack.h src/amxdebuginfo.h\
                       src/amxpathfinder.h src/jump-x86.h src/version.h lib/plugincommon.h lib/amx/amx.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

ifeq ($(os),windows)
src/crashdetect.rc.o: src/crashdetect.rc src/version.h
	windres $< $@
endif

src/interrupt.cpp.o: src/interrupt.cpp src/interrupt.h
	$(CXX) $(CPPFALGS) $(CXXFLAGS) -c $< -o $@

src/jump-x86.cpp.o: src/jump-x86.cpp src/jump-x86.h
	$(CXX) $(CPPFALGS) $(CXXFLAGS) -c $< -o $@

src/version.h:
	echo "Generating $@..."
	sh ./version.h.sh > $@

clean:
	rm -vf $(objects) $(binary)

.PHONY: all version clean
