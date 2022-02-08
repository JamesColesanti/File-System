#include "storage.h"
#include "inode.h"
#include "directory.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

// Get attributes for the file with the given path. Set field of
// given stat struct.
int storage_stat(const char *path, struct stat *st) {
	int rv = 0;
	// get root inode and search for file
	inode_t *dir = get_inode(0);
	int fileINum = directory_lookup(dir, path);
	// Return some metadata for the root directory...
	if (strcmp(path, "/") == 0) {
		st->st_mode = 040755; // directory
		st->st_size = 0;
		st->st_uid = getuid();
	}
	// ...and the simulated file...
	else if (fileINum >= 0) {
		inode_t *fileINode = get_inode(fileINum);
		st->st_mode = 0100644; // regular file
		st->st_size = fileINode->size;
		st->st_uid = getuid();
	}
	else { // ...other files do not exist on this filesystem
		rv = -ENOENT;
	}
	printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode,
			st->st_size);
	return rv;
}

// Find the file with the given path and read its data into the given buffer.
// Reading starts at byte offset and continues for size bytes. Return the number
// of bytes read.
int storage_read(const char *path, char *buf, size_t size, off_t offset) {
	int rv = size;

	// get inumber for file
	int inum = directory_lookup(get_inode(0), path);

	// if file is not found, return 0, indicating no bytes were read
	if (inum < 0) {
		return 0;
	}

	inode_t *inode = get_inode(inum);
	// copy data to buffer starting at the beginning of the data block 
	// + offset
	memcpy(buf, blocks_get_block(inode->block) + offset, size); 
	printf("read(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);

	// return number of bytes read
	return rv;

}

// Find the file with the given path and write to its data from the given buffer.
// Writing starts at byte offset and continues for size bytes. Return the number
// of bytes written.
int storage_write(const char *path, const char *buf, size_t size, off_t offset) {
	int rv = size;

	// get inumber for file
	int inum = directory_lookup(get_inode(0), path);

	// if file is not found, return 0, indicating no bytes were written
	if (inum < 0) {
		return 0;
	}

	inode_t *inode = get_inode(inum);
	// copy data from buffer to memory address (start of data block + offset)
	memcpy(blocks_get_block(inode->block) + offset, buf, size);

	// increment size of file by _size_
        inode->size += size;	
	printf("write(%s, %ld bytes, @+%ld) -> %d\n", path, size, offset, rv);

	// return number of bytes written
	return rv;
}

// Set the size field of the inode of the file with the given path to given size.
// Returns 0 on success and -1 when an error occurs.
int storage_truncate(const char *path, off_t size) {
	int rv = 0;
	
	// get inumber for file 
	int inum = directory_lookup(get_inode(0), path);
	
	if (inum < 0) {
		return -1;
	}

	// set file's inode size field to given size
	inode_t *inode = get_inode(inum);
	inode->size = size;

	printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
	return rv;
}

// Create a new directory entry for file _path with the given mode. Return 0
// on success and -1 when an error occurs.
int storage_mknod(const char *path, int mode) {
	int rv = 0;

	// allocate new inode
	int newINum = alloc_inode();
	inode_t *newINode = get_inode(newINum);

	char fileName[DIR_NAME_LENGTH];
	strcpy(fileName, path);

	// create new directory entry in root directory
	rv = directory_put(get_inode(0), fileName, mode, newINum);

	printf("mknod(%s, %04o) -> %d\n", path, mode, rv);
	return rv;
}

// Change the name of the file with the name _from_ to _to. Return 0 on success
// and -1 when an error occurs.
int storage_rename(const char *from, const char *to) {
	int rv = 0;

	// get root inode
	inode_t *dd = get_inode(0);
	dirent_t *dir = blocks_get_block(dd->block);
	int count = 0;
	int result;

	// loop through root directory entries
	while (count < dd->files) {
		result = strcmp(dir->name, from);
		// if _from_ file is found, change name to _to_
		// return 0 to indicate success
		if (result == 0) {
			strcpy(dir->name, to);
			return rv;
		}
		count++;
		dir = blocks_get_block(dd->block) + count * sizeof(dirent_t);
	}

	printf("rename(%s => %s) -> %d\n", from, to, rv);

	// return -1 to indicate error
	return -1;
}


