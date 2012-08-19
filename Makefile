# This Makefile requires a GNU system in two places:
#
# 1. It is assumed to be written for GNU make.
# 2. xargs uses -r.

notesfs: notesfs.c
	gcc `pkg-config --cflags fuse sqlite3` -o notesfs notesfs.c `pkg-config --libs fuse sqlite3`

.PHONY : kill
kill:
	pgrep notesfs | xargs -r kill

.PHONY : forcekill
forcekill:
	pgrep notesfs | xargs -r kill -9

