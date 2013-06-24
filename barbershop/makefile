CFLAGS:=-02 -Wall
CFILES:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp,objs/%.o,$(CFILES))
HEADERS:=$(wildcard *.h)
LIBS:=$(wildcard libs/*.a $(OS)/*.a)
LINK_FLAGS:=
CC:=g++

OS:=$(shell uname)
ifeq ($(OS),Darwin)
	LINK_FLAGS:=$(LINK_FLAGS) -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -lglfw -framework Cocoa -framework OpenGL -framework IOKit
endif

all: barbershop

barbershop: $(HEADERS) $(OBJS) $(LIBS)
	$(CC) $(LINK_FLAGS) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

objs:
	-rm objs
	mkdir objs

objs/%.o: %.cpp $(HEADERS) objs
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm tuner
	-rm -r objs
