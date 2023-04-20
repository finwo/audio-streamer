CPP=g++

LIBS:=
SRC:=

SRC+=$(wildcard src/*.c)
SRC+=$(wildcard src/*/*.c)

override CFLAGS?=-Wall -s -O2

INCLUDES:=
INCLUDES+=-I src

include lib/.dep/config.mk

OBJ:=$(SRC:.c=.o)
OBJ:=$(OBJ:.cc=.o)

override CFLAGS+=$(INCLUDES)
override CPPFLAGS=`pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0`
override LDFLAGS+=`pkg-config --libs gtk+-3.0 webkit2gtk-4.0`

default: $(BIN)

docroot:
	mkdir -p docroot
	../../util/bin2c < ../../../../services/manager/dist/index.bundled.html > docroot/manager.h

$(OBJ): docroot

$(BIN): $(OBJ)
	$(CPP) $(LDFLAGS) $(OBJ) -o $@
