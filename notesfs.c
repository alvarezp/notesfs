/* notesfs.c: FUSE-based filesystem for iPhone notes application.
 *
 * As of now, only the iPhone 2G database format is supported.
 *
 * Author: Octavio Alvarez <alvarezp@alvarezp.ods.org>
 *
 * License: GPL v3, but contact me if you need anything outside the GPL v3.
 *
 * Prerequisites:
 *    Development packages for:
 *       SQLite 3 (here using Debian libsqlite3-dev package 3.7.13-1)
 *       FUSE 2 (here using Debian libfuse-dev 2.9.0-5)
 *
 * Compilation (this command line requires pkg-config):
 *    gcc \
 *       `pkg-config --cflags fuse sqlite3` \
 *       -o notesfs notesfs.c -ggdb \
 *       `pkg-config --libs fuse sqlite3`
 *
 * Run (mounting):
 *    ./notesfs path/to/notes.db [FUSE options] MOUNT_POINT
 * 
 * In my phone, notes.db is under /var/mobile/Library/Notes, however, when you
 * connect the iPhone under a GNOME system, GVFS mounts /var/mobile/Media,
 * which means it won't be available. In my jailbroken phone, I hard linked the
 * file under /var/mobile/Media (soft links make GVFS-AFC lock up) using:
 *    cd /var/mobile/Media
 *    ln ../Library/Notes/notes.db
 *
 * As an alternative, if your iPhone is connected to the network, you can mount
 * it by using SSHFS. However, make you you have your iPhone connected, because
 * filesystem monitors will be constantly polling your iPhone FS and notesfs
 * and the battery may drain faster.
 *
 */

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sqlite3.h>

#define NOTES_EPOCH 978307200

sqlite3 *ppDb;

FILE *lf;

static int notesfs_getxattr(const char *path, const char *buf, char *fi, size_t size) {
	return -ENOTSUP;
}

static int notesfs_flush(const char *path, struct fuse_file_info *fi) {
	return 0;
}

static int notesfs_truncate(const char *path, off_t o) {

	if (path[1] != '0') {

		int r;

		sqlite3_stmt *ppStmt = NULL;
		const char *pzTail = NULL;
		sqlite3_prepare(ppDb, "UPDATE note_bodies SET data = ? WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);", -1, &ppStmt, &pzTail);
		sqlite3_bind_text(ppStmt, 1, &path[1], -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);

		r = sqlite3_step(ppStmt);
		if (r != SQLITE_DONE) {
			sqlite3_finalize(ppStmt);
			return -EINVAL;
		}

		return 0;


	} else {

		return -ENOTSUP;

	}

}

static int notesfs_ftruncate(const char * path, off_t o, struct fuse_file_info *fi) {
	return 0;
}

static int notesfs_utime(const char *path, struct utimbuf *u) {
	return 0;
}

static int notesfs_utimens (const char * path, const struct timespec tv[2]) {
	return 0;
}

static int notesfs_setattr (const char * path, const struct timespec tv[2]) {
	return 0;
}

static int notesfs_chown(const char *path, uid_t u, gid_t g) {
	return 0;
}

static int notesfs_chmod(const char *path, mode_t m) {
	return 0;
}

static int notesfs_getattr(const char *path, struct stat *stbuf)
{

	/* The 'st_dev' and 'st_blksize' fields are ignored. The 'st_ino'
	 * field is ignored except if the 'use_ino' mount option is given.
	 */

	/* struct stat *stbuf members:
	 * dev_t     st_dev     ID of device containing file
	 * ino_t     st_ino     file serial number
	 * mode_t    st_mode    mode of file (see below)
	 * nlink_t   st_nlink   number of links to the file
	 * uid_t     st_uid     user ID of file
	 * gid_t     st_gid     group ID of file
	 * dev_t     st_rdev    device ID (if file is character or block special)
	 * off_t     st_size    file size in bytes (if file is a regular file)
	 * time_t    st_atime   time of last access
	 * time_t    st_mtime   time of last data modification
	 * time_t    st_ctime   time of last status change
	 * blksize_t st_blksize a filesystem-specific preferred I/O block size for
	 * 			 this object.  In some filesystem types, this may
	 * 			 vary from file to file
	 * blkcnt_t  st_blocks  number of blocks allocated for this object
	 */

    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if(strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
	} else if (path[1] != '0') {

        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;

		sqlite3_stmt *ppStmt = NULL;
		const char *pzTail;
		const char *sql = "SELECT substr(replace(replace(replace(replace(data,'<div>','\n'),'</div>',''),'<br>',''),'&lt;','<'),?) FROM note_bodies WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);";
		sqlite3_prepare(ppDb, sql, -1, &ppStmt, &pzTail);
		sqlite3_bind_int(ppStmt, 1, strlen(path) + 1);
		sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);

		int r;	
		r = sqlite3_step(ppStmt);
		if (r == SQLITE_ROW) {
			int v;
			v =	sqlite3_column_bytes(ppStmt, 0);

	        stbuf->st_size = v;
		} 

		sqlite3_finalize(ppStmt);

		sqlite3_prepare(ppDb, "SELECT creation_date, modification_date FROM note WHERE title == ?;", -1, &ppStmt, &pzTail);
		sqlite3_bind_text(ppStmt, 1, &path[1], -1, SQLITE_TRANSIENT);

		r = sqlite3_step(ppStmt);
		if (r != SQLITE_ROW) {
			sqlite3_finalize(ppStmt);
			return -ENOENT;
		} 

        stbuf->st_ctime = sqlite3_column_int(ppStmt, 0) + NOTES_EPOCH;
        stbuf->st_mtime = sqlite3_column_int(ppStmt, 1) + NOTES_EPOCH;

		sqlite3_finalize(ppStmt);
    } else if (path[1] == '0') {

        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;

		char s[5];

		s[0] = path[2];
		s[1] = path[3];
		s[2] = path[4];
		s[3] = path[5];

		sqlite3_stmt *ppStmt = NULL;
		const char *pzTail;
		sqlite3_prepare(ppDb, "SELECT replace(replace(replace(replace(data,'<div>','\n'),'</div>',''),'<br>',''),'&lt;','<') FROM note_bodies WHERE note_id == ?;", -1, &ppStmt, &pzTail);
		sqlite3_bind_int(ppStmt, 1, atoi(s));

		int r;	
		r = sqlite3_step(ppStmt);
		if (r == SQLITE_ROW) {
			int v;
			v =	sqlite3_column_bytes(ppStmt, 0);

	        stbuf->st_size = v;
		} 

		sqlite3_finalize(ppStmt);

		sqlite3_prepare(ppDb, "SELECT creation_date, modification_date FROM note WHERE ROWID == ?;", -1, &ppStmt, &pzTail);
		sqlite3_bind_int(ppStmt, 1, atoi(s));

		r = sqlite3_step(ppStmt);
		if (r != SQLITE_ROW) {
			sqlite3_finalize(ppStmt);
			return -ENOENT;
		} 

        stbuf->st_ctime = sqlite3_column_int(ppStmt, 0) + NOTES_EPOCH;
        stbuf->st_mtime = sqlite3_column_int(ppStmt, 1) + NOTES_EPOCH;

		sqlite3_finalize(ppStmt);
    }

    return res;

}

/* Return the list of notes, using note_id + first line as file name. */
static int notesfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if(strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

	sqlite3_stmt *ppStmt = NULL;
	const char *pzTail;
	sqlite3_prepare(ppDb, "SELECT ROWID, title FROM note;", -1, &ppStmt, &pzTail);

	int r;
	for(r = sqlite3_step(ppStmt); r == SQLITE_ROW; r = sqlite3_step(ppStmt)) {
		int v;
		const char *s;

		v =	sqlite3_column_int(ppStmt, 0);
		s =	(const char *)sqlite3_column_text(ppStmt, 1);

		if (strchr(s,'/') == NULL) {
			filler(buf, s, NULL, 0);
			continue;
		}

		char fn[10];

		snprintf(fn, 10, "0%04d.txt", v);

		filler(buf, fn, NULL, 0);
	}

	sqlite3_finalize(ppStmt);
    return 0;
}

static int notesfs_open(const char *path, struct fuse_file_info *fi)
{

	return 0;
}

static int notesfs_create(const char * path, mode_t mode, struct fuse_file_info *fi)
{

	if (path[1] == '0') {
		return -ENOTSUP;
	}
	
	sqlite3_stmt *ppStmt = NULL;
	const char *pzTail;
	sqlite3_prepare(ppDb, "INSERT INTO note (creation_date,title,contains_cjk,modification_date,author,content_type) VALUES (?,?,?,?,?,?);", -1, &ppStmt, &pzTail);
	sqlite3_bind_int(ppStmt, 1, time(NULL) - NOTES_EPOCH);
	sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(ppStmt, 3, 0);
	sqlite3_bind_int(ppStmt, 4, time(NULL) - NOTES_EPOCH);
	sqlite3_bind_text(ppStmt, 5, "", 0, SQLITE_STATIC);
	sqlite3_bind_int(ppStmt, 6, 0);

	int r;	
	r = sqlite3_step(ppStmt);
	if (r != SQLITE_DONE) {
		sqlite3_finalize(ppStmt);
		return -EACCES;
	}

	sqlite3_finalize(ppStmt);

	int id;
	id = sqlite3_last_insert_rowid(ppDb);

	sqlite3_prepare(ppDb, "INSERT INTO note_bodies (note_id, data) VALUES (?,?);", -1, &ppStmt, &pzTail);
	sqlite3_bind_int(ppStmt, 1, id);
	sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);

	r = sqlite3_step(ppStmt);
	if (r != SQLITE_DONE) {
		sqlite3_finalize(ppStmt);
		return -ENOENT;
	}

	sqlite3_finalize(ppStmt);

	return 0;
}

static int notesfs_write(const char * path, const char * buf, size_t size, off_t offset, struct fuse_file_info *fi) {

	fprintf(lf, ":: WRITE: Should write %zu bytes from position %zd from path %s.\n", size, offset, path);
	fprintf(lf, "::-----   Contents: |");
	fwrite(buf, size, 1, lf);
	fprintf(lf, "|\n");

	if (path[1] != '0') {

		sqlite3_stmt *ppStmt = NULL;
		const char *pzTail = NULL;
		const char *sql = "SELECT substr(replace(replace(replace(replace(data,'<div>','\n'),'</div>',''),'<br>',''),'&lt;','<'),?) FROM note_bodies WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);";
		sqlite3_prepare(ppDb, sql, -1, &ppStmt, &pzTail);
		sqlite3_bind_int(ppStmt, 1, strlen(path) + 1);
		sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);

		int r;
		const char *s1;
		char *s = NULL;
		signed int b;
		r = sqlite3_step(ppStmt);
		if (r == SQLITE_ROW) {
			s1 =	(const char *) sqlite3_column_text(ppStmt, 0);
			b = sqlite3_column_bytes(ppStmt, 0);
			s = malloc(b+1);
			strncpy(s,s1,b);
		fprintf(lf, "::-----   b,s = %d|%s|\n", b,s);
		} else {
			sqlite3_finalize(ppStmt);
			return -EINVAL;
		} 

		sqlite3_finalize(ppStmt);

		char new[65535] = "";

		signed int p2 = 0;
		if (((b-(int)offset)-(int)size) > 0)
			p2 = ((b-offset)-size);

		strcpy(new,&path[1]);
		strcat(new,"&&&div>");
		strncat(new,s,offset);
		strncat(new,buf,size);
		strncat(new,&s[offset+size],p2);
		strcat(new,"&&&/div>");

		ppStmt = NULL;
		pzTail = NULL;
		sqlite3_prepare(ppDb, "UPDATE note_bodies SET data = replace(replace(replace(?,'<','&lt;'),'\n','</div><div>'),'&&&','<') WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);", -1, &ppStmt, &pzTail);
		/*sqlite3_prepare(ppDb, "UPDATE note_bodies SET data = ? WHERE substr(data,1,?) == ?;", -1, &ppStmt, &pzTail);*/
		sqlite3_bind_text(ppStmt, 1, new, -1, SQLITE_STATIC);
		sqlite3_bind_text(ppStmt, 2, &path[1], -1, SQLITE_TRANSIENT);
		fprintf(lf, "::----- ABOUT TO WRITE:\n");
		fprintf(lf, "::-----   old = |");
		fwrite(s, b, 1, lf);
		fprintf(lf, "|\n");
		fprintf(lf, "::-----   new = %s\n", new);

		r = sqlite3_step(ppStmt);
		if (r != SQLITE_DONE) {
			sqlite3_finalize(ppStmt);
			return -EINVAL;
		}

		return size;


	} else {

		return -ENOTSUP;

	}

}

/* Retrieve requested contents from database */
static int notesfs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
	fprintf(lf, ":: READ: Should to read %zu bytes from position %zd from path %s.\n", size, offset, path);

	if (path[1] != '0') {

		char id[5];

		id[0] = path[2];
		id[1] = path[3];
		id[2] = path[4];
		id[3] = path[5];

		sqlite3_stmt *ppStmt = NULL;
		const char *pzTail;
		const char *sql = "SELECT substr(replace(replace(replace(replace(data,'<div>','\n'),'</div>',''),'<br>',''),'&lt;','<'),?,?) FROM note_bodies WHERE note_id IN (SELECT ROWID FROM note WHERE note.title = ?);";
		sqlite3_prepare(ppDb, sql, -1, &ppStmt, &pzTail);
		sqlite3_bind_int(ppStmt, 1, offset + strlen(path) + 1);
		sqlite3_bind_int(ppStmt, 2, (int) size);
		sqlite3_bind_text(ppStmt, 3, &path[1], -1, SQLITE_TRANSIENT);

		int r;	
		int b;
		const char *s;
		r = sqlite3_step(ppStmt);
		if (r == SQLITE_ROW) {
			s =	sqlite3_column_text(ppStmt, 0);
			b = sqlite3_column_bytes(ppStmt, 0);
			fprintf(lf, "::----- Result OK\n");
			fprintf(lf, "::-----   strlen(s) = %zu\n", strlen(s));
			fprintf(lf, "::-----   s = %p\n", s);
			fprintf(lf, "::-----   s+offset = %p\n", s + offset);
			fprintf(lf, "::-----   b = %d\n", b);

			memcpy(buf, s + offset, b);
		} else {
			fprintf(lf, "::----- Result NOT OK\n");
			return -EINVAL;
		} 

		sqlite3_finalize(ppStmt);

		return b;
	} else {

		char id[5];

		id[0] = path[2];
		id[1] = path[3];
		id[2] = path[4];
		id[3] = path[5];

		sqlite3_stmt *ppStmt = NULL;
		const char *pzTail;
		sqlite3_prepare(ppDb, "SELECT substr(replace(replace(replace(replace(data,'<div>','\n'),'</div>',''),'<br>',''),'&lt;','<'),?,?) FROM note_bodies WHERE note_id == ?;", -1, &ppStmt, &pzTail);
		sqlite3_bind_int(ppStmt, 1, (int) offset);
		sqlite3_bind_int(ppStmt, 2, (int) size);
		sqlite3_bind_int(ppStmt, 3, atoi(id));

		int r;	
		const char *s;
		int b;
		r = sqlite3_step(ppStmt);
		if (r == SQLITE_ROW) {
			s =	sqlite3_column_text(ppStmt, 0);
			b = sqlite3_column_bytes(ppStmt, 0);

			memcpy(buf, s + offset, size);
		} else {
			b = 0;
		} 

		sqlite3_finalize(ppStmt);

		return b;
	}
}

static struct fuse_operations notesfs_oper = {
    .getattr	= notesfs_getattr,
    .readdir	= notesfs_readdir,
    .open	= notesfs_open,
    .read	= notesfs_read,
    .write	= notesfs_write,
    .chmod = notesfs_chmod,
    .chown = notesfs_chown,
    .utime = notesfs_utime,
    .truncate = notesfs_truncate,
    .ftruncate = notesfs_ftruncate,
    .utimens = notesfs_utimens,
    .flush = notesfs_flush,
    .getxattr = notesfs_getxattr,
    .create	= notesfs_create
};

int main(int argc, char *argv[])
{
	int r;

	printf("\nTHIS PROGRAM WILL PROBABLY MAKE YOU LOSE YOUR NOTES. BACK THEM UP!\n");
	printf("\n");
	printf("Welcome to notesfs! Only the original (2G) iPhone is supported so far.\n");

	r = sqlite3_open(argv[1], &ppDb);
	if (r != SQLITE_OK) {
		fprintf(stderr, "Error opening the database file: %s", sqlite3_errmsg(ppDb));
		return 1;
	};

	for (r = 2; r < argc; ++r) {
		argv[r-1] = argv[r];
	}
	argc--;

	lf = fopen("fuse.log", "w");
	r = fuse_main(argc, argv, &notesfs_oper, NULL);
	sqlite3_close(ppDb);
	return r;
}
