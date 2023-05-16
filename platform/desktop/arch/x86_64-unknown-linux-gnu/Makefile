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

DOCS:=
DOCS+=manager
DOCS+=player

default: $(BIN)

docroot:
	mkdir -p docroot
	for doc in $(DOCS); do \
		echo "#ifndef __FINWO_AS_DOC_$$(echo $${doc} | tr 'a-z' 'A-Z')_H__"          >  docroot/$${doc}.h ; \
		echo "#define __FINWO_AS_DOC_$$(echo $${doc} | tr 'a-z' 'A-Z')_H__"          >> docroot/$${doc}.h ; \
		xxd -i -n "doc_$${doc}" ../../../../services/$${doc}/dist/index.bundled.html >> docroot/$${doc}.h ; \
		echo "#endif // __FINWO_AS_DOC_$$(echo $${doc} | tr 'a-z' 'A-Z')_H__"        >> docroot/$${doc}.h ; \
	done

$(OBJ): docroot

$(BIN): $(OBJ)
	$(CPP) $(LDFLAGS) $(OBJ) -o $@
