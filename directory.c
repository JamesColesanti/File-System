#include "directory.h"
#include "slist.h"
#include "inode.h"
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"

// initialize root directory inode and data block
void directory_init() {
	// block 0 holds the bitmaps, with the inode table starting after them and
	// running into block 1
	// block 2 holds the root directory
	// if the root directory has not been initialized, we initialize it here
	void *ibm = get_inode_bitmap();
	if (!bitmap_get(ibm, 2)) {
		// inode 0
		int root_inum = alloc_inode();

		// block 2
		int root_block_num = alloc_block();

		// get root inode and set fields
		inode_t *root_inode = get_inode(root_inum);
		root_inode->files = 0;
		root_inode->mode = 040755;
		root_inode->block = root_block_num;

		// set inode 0 to taken
		bitmap_put(ibm, 0, 1);

		// set block 2 to tkane
		void *bbm = get_blocks_bitmap();
		bitmap_put(bbm, 2, 1);
	}
}

// return file inum if file is found, -1 otherwise
int directory_lookup(inode_t *dd, const char *name) {
	int count = 0;
	int result;

	// root directory is inode 0
	inode_t *inode = get_inode(0);
	// root directory data block - first entry is beginning of block
	dirent_t *dir = blocks_get_block(inode->block);
	
	// loop through directory entries, return entry inum if it is found
	while (count < dd->files) {
		result = strcmp(dir->name, name);
		if (result == 0) {
			return dir->inum;
		}
		count++;
		// next entry is count * the size of an entry struct
		dir = blocks_get_block(inode->block) + count * sizeof(dirent_t);
	}
	// return -1 if file does not have an entry
	return -1;
}

// create a new file entry for the given inode's directory
int directory_put(inode_t *dd, const char *name, mode_t mode, int inum) {
	// return 0 upon success
	int status = 0;

	// index of directory data block
	int dir_block_num = dd->block;

	// index of data block for new file
	int file_block_num = alloc_block();

	// initialize inode and fields for new file
	inode_t *file_inode = get_inode(inum);
	file_inode->files = 0;
	file_inode->mode = mode;
	file_inode->block = file_block_num;

	// if directory is full, return -1 
	if (dd->files >= (4096 / sizeof(dirent_t))) {
		status = -1;
	}

	// new file is given next available memory address in directory data block
	dirent_t *newFile = blocks_get_block(dir_block_num) + dd->files * sizeof(dirent_t);
	strcpy(newFile->name, name);
	newFile->inum = inum;

	// number of files in directory increases by 1
	dd->files++;

	return status;
}


