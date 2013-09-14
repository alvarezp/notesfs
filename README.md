About
=====
notesfs is a FUSE-based filesystem for iPhone notes application.

Current limitations
===================
  * The iPhone must be jailbroken.
  * Only the iPhone 2G Notes database format is supported.

Author
======
  * Octavio Alvarez, alvarezp@alvarezp.ods.org

License
=======
 * GPL v3, but contact me if you need anything outside the GPL v3.

Required development packages
=============================
  * SQLite 3 (Debian: libsqlite3-dev 3.7.13-1, Ubuntu: libsqlite3-dev 3.7.7-2ubuntu2)
  * FUSE 2 (Debian: libfuse-dev 2.9.0-5, Ubuntu: 2.8.4-1.4ubuntu1)

Compilation
===========
The following command line requires pkg-config:
  * gcc ```pkg-config --cflags fuse sqlite3``` -o notesfs notesfs.c -ggdb ```pkg-config --libs fuse sqlite3```

Running (mounting)
==================
  * ./notesfs path/to/notes.db [FUSE options] MOUNT_POINT

In my phone, notes.db is under /var/mobile/Library/Notes, however, when you connect the iPhone under a GNOME system, GVFS mounts /var/mobile/Media, which means it won't be available. In my jailbroken phone, I hard linked the file under /var/mobile/Media (soft links make GVFS-AFC lock up) using:
  * cd /var/mobile/Media
  * ln ../Library/Notes/notes.db

As an alternative, if your iPhone is connected to the network, you can mount it by using SSHFS. However, make sure you have your iPhone connected, because filesystem monitors will be constantly polling your iPhone FS and notesfs and the battery may drain faster.
