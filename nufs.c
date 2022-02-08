// based on cs3650 starter code

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "bitmap.h"
#include "inode.h"
#include "blocks.h"
#include "directory.h"
#include "storage.h"
#include "slist.h"

#define FUSE_USE_VERSION 26
#include <fuse.h>

/*
 * EXPLANATION OF DATA REPRESENTATION:
 * The entire data file is composed of 256 blocks. Block 0 holds the inode 
 * bitmap and the block bitmap, followed by the inode table. If necessary, 
 * block 1 is also set aside for the inode table. Whether or not this block 
 * is used is determined by the number of inodes needed (number of inode table 
 * blocks needed = ceil(255 * sizeof(inode_t) / 4096)). Block 2 is the data 
 * block for the root directory. This block consists of an array of dirent 
 * structs, each one representing an entry in the root directory. The limit 
 * on the number of files for this directory is 4096 / sizeof(dirent_t).
 * All directories would have this limit imposed on the number of files, 
 * although nested directories are not supported in this implementation. 
 * Block 3 is the first data block for files within the root directory. 
 * Inode 0 is the inode for the oot directory. Inode 1 is the inode for the 
 * first file within the root directory.
 */ 

// implementation for: man 2 access
// Checks if a file exists.
int nufs_access(const char *path, int mask) {
	int rv = 0;
	// get root inode and search for file
	inode_t *dirINode = get_inode(0);
	int fileINum = directory_lookup(dirINode, path);

	// if file is found in root, get its inode
	inode_t *file;
	if (fileINum >= 0) {
		file = get_inode(fileINum);
	}
	
	// set rv to 0 to indicate success if the given path is for the root
	// or a file in the root
	if (strcmp(path, "/") == 0) { 
		rv = 0;
	} else if (fileINum >= 0) {
		rv = 0;
	} else { // ...others do not exist
		rv = -ENOENT;
	}
	printf("access(%s, %04o) -> %d\n", path, mask, rv);
	return rv;
}

// Gets an object's attributes (type, permissions, size, etc).
// Implementation for: man 2 stat
// This is a crucial function.
int nufs_getattr(const char *path, struct stat *st) {
	return storage_stat(path, st);
}

// implementation for: man 2 readdir
// lists the contents of a directory
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi) {
	struct stat st;
	int rv;

	// call filler for "." entry
	rv = nufs_getattr("/", &st);
	assert(rv == 0);
	filler(buf, ".", &st, 0);

	// get root data block
	char fileName[strlen(path)];
	int count = 0;
	inode_t *inode = get_inode(0);
	dirent_t *dir = blocks_get_block(inode->block);

	// loop through files in root, calling filler for each one
	while (count < inode->files) {
		strcpy(fileName, dir->name);	

		rv = nufs_getattr(fileName, &st);
		assert(rv == 0);

		char fileNameNoSlash[20];
		strcpy(fileNameNoSlash, &fileName[1]);

		filler(buf, fileNameNoSlash, &st, 0);
		count++;
		dir = blocks_get_block(inode->block) + count * sizeof(dirent_t);
	}
	
	printf("readdir(%s) -> %d\n", path, rv);
	return 0;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
// Note, for this assignment, you can alternatively implement the create
// function.
int nufs_mknod(const char *path, mode_t mode, dev_t rdev) {
	return storage_mknod(path, mode);
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int nufs_mkdir(const char *path, mode_t mode) {
	int rv = nufs_mknod(path, mode | 040000, 0);
	printf("mkdir(%s) -> %d\n", path, rv);
	return rv;
}

int nufs_unlink(const char *path) {
	int rv = -1;
	printf("unlink(%s) -> %d\n", path, rv);
	return rv;
}

int nufs_link(const char *from, const char *to) {
	int rv = -1;
	printf("link(%s => %s) -> %d\n", from, to, rv);
	return rv;
}

int nufs_rmdir(const char *path) {
	int rv = -1;
	printf("rmdir(%s) -> %d\n", path, rv);
	return rv;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int nufs_rename(const char *from, const char *to) {
	return storage_rename(from, to);
}

int nufs_chmod(const char *path, mode_t mode) {
	int rv = -1;
	printf("chmod(%s) -> %d\n", path, mode);
	return rv;
}

// Find the inode for the file for the given path and sets its
// size field to size.
int nufs_truncate(const char *path, off_t size) {
	return storage_truncate(path, size);
}

// This is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
// You can just check whether the file is accessible.
int nufs_open(const char *path, struct fuse_file_info *fi) {
	int rv = nufs_access(path, 0);
	printf("open(%s) -> %d\n", path, rv);
	return rv;
}

// Actually read data
int nufs_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	return storage_read(path, buf, size, offset);
}

// Actually write data
int nufs_write(const char *path, const char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	return storage_write(path, buf, size, offset);	
}

// Update the timestamps on a file or directory.
int nufs_utimens(const char *path, const struct timespec ts[2]) {
	int rv = 0;
	printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n", path, ts[0].tv_sec,
			ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
	return rv;
}

// Extended operations
int nufs_ioctl(const char *path, int cmd, void *arg, struct fuse_file_info *fi,
		unsigned int flags, void *data) {
	int rv = -1;
	printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
	return rv;
}

void nufs_init_ops(struct fuse_operations *ops) {
	memset(ops, 0, sizeof(struct fuse_operations));
	ops->access = nufs_access;
	ops->getattr = nufs_getattr;
	ops->readdir = nufs_readdir;
	ops->mknod = nufs_mknod;
	// ops->create   = nufs_create; // alternative to mknod
	ops->mkdir = nufs_mkdir;
	ops->link = nufs_link;
	ops->unlink = nufs_unlink;
	ops->rmdir = nufs_rmdir;
	ops->rename = nufs_rename;
	ops->chmod = nufs_chmod;
	ops->truncate = nufs_truncate;
	ops->open = nufs_open;
	ops->read = nufs_read;
	ops->write = nufs_write;
	ops->utimens = nufs_utimens;
	ops->ioctl = nufs_ioctl;
};

struct fuse_operations nufs_ops;

int main(int argc, char *argv[]) {
	assert(argc > 2 && argc < 6);
	int temp = argc - 1;
	printf("TODO: mount %s as data file\n", argv[--argc]);

	// set up disk file if it has not been set up yet
	blocks_init(argv[temp]);
	directory_init();
	
	nufs_init_ops(&nufs_ops);

	return fuse_main(argc, argv, &nufs_ops, NULL);
}
