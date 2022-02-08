// Disk storage manipulation.
//
// Feel free to use as inspiration.

// based on cs3650 starter code

#ifndef NUFS_STORAGE_H
#define NUFS_STORAGE_H

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "slist.h"
#include <string.h>
#include <math.h>
#include "inode.h"

// Get attributes for the file with the given path. Set field of given
// stat struct.
int storage_stat(const char *path, struct stat *st);

// Find the file with the given path and read its data into the given buffer.
// Reading starts at byte offset and continues for size bytes. Return the number
// of bytes read.
int storage_read(const char *path, char *buf, size_t size, off_t offset);

// Find the file with the given path and write to its data from the given buffer.
// Writing starts at byte offset and continues for size bytes. Return the number 
// of bytes written.
int storage_write(const char *path, const char *buf, size_t size, off_t offset);

// Set the size field of the inode of the file with the given path to size.
// Returns 0 on success and -1 when an error occurs.
int storage_truncate(const char *path, off_t size);

// Create a new directory entry for file _path_ with the given mode. Return 0
// on success and -1 when an error occurs.
int storage_mknod(const char *path, int mode);

// Change the name of the file with name _from_ to _to_. Return 0 on success
// and -1 when an error occurs.
int storage_rename(const char *from, const char *to);

// not supported in this implementation
void storage_init(const char *path);
int storage_chmod(const char* path, mode_t mode); 
int storage_unlink(const char *path);
int storage_link(const char *from, const char *to);
int storage_set_time(const char *path, const struct timespec ts[2]);
slist_t *storage_list(const char *path);

#endif
