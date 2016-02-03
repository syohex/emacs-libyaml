EMACS_ROOT ?= ../..
EMACS ?= emacs

CC      = gcc
LD      = gcc
CPPFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC $(CPPFLAGS)
#LDFLAGS = -lyaml
LDFLAGS =

.PHONY : test

all: libyaml-core.so

libyaml-core.so: libyaml-core.o
	$(LD) -shared $(LDFLAGS) -o $@ $^ -lyaml

libyaml-core.o: libyaml-core.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f libyaml-core.so libyaml-core.o

test:
	$(EMACS) -Q -batch -L . $(LOADPATH) \
		-l test/test-libyaml.el \
		-f ert-run-tests-batch-and-exit
