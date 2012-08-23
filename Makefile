# TDD-oriented Makefile for Notesfs.

# This Makefile requires a GNU system.

# Automatic dependency generation adapted from
# http://www.scottmcpeak.com/autodepend/autodepend.html

# This file is Copyright 2010 (C) Octavio Alvarez <alvarezp@alvarezp.ods.org>
# License: the one that reigns the rest of the project.

CFLAGS = -Wall -Werror -ansi -std=c99 -pedantic-errors -ggdb `pkg-config --cflags fuse sqlite3`

.SECONDARY:

APP = notesfs
OBJS_TDD =
OBJS_NO_TDD = notesfs.o
ALL_OBJS = $(OBJS_TDD) $(OBJS_NO_TDD)
TESTS = $(OBJS_TDD:.o=.ts)
LIBS = `pkg-config --libs fuse sqlite3`

.PHONY : all
all: $(APP)

# Pull in dependency info for existing .o and .t files.
-include $(ALL_OBJS:.o=.d)
-include $(ALL_OBJS:.o=.t.d)

# All lower targets depend on Makefile so everything rebuilds if Makefile
# changes.
Makefile:

$(APP): Makefile $(ALL_OBJS) $(TESTS)
	@echo -e '\n'== $@, building app...
	gcc -o $(APP) $(LIBS) $(ALL_OBJS)

# Compile plus generate dependency information.
%.o: Makefile %.c
	@echo -e '\n'== $@, building module...
	gcc $(CFLAGS) -o $*.o -c $*.c
	@echo -e '\n'== $@, generating dependency information...
	gcc $(CFLAGS) -MM -MP -MT $*.o $*.c > $*.d

%.ts: Makefile %.t
	@echo -e '\n'== $@, testing...
	./$*.t && touch $*.ts;

%.t.c:
	@echo -e '\n'== $@ doesn\'t exist! Please create one.
	@false

%.to: Makefile %.t.c
	@echo -e '\n'== $@, building test module...
	gcc $(CFLAGS) -o $*.to -c $*.t.c
	@echo -e '\n'== $@, generating dependency information...
	gcc $(CFLAGS) -MM -MP -MT $*.to $*.t.c > $*.t.d

%.t: %.to %.o
	@echo -e '\n'== $@, building test...
	gcc -o $*.t $^

.PHONY : clean
clean:
	@echo -e '\n'== Cleaning...
	rm -f $(APP)
	rm -f *.o
	rm -f *.d
	rm -f *.t
	rm -f *.to
	rm -f *.ts

.PHONY : kill
kill:
	pgrep notesfs | xargs -r kill

.PHONY : forcekill
forcekill:
	pgrep notesfs | xargs -r kill -9

