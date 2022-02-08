#include "inode.h"

// return a pointer to the inode at the given index in the inode table
inode_t *get_inode(int inum) {
	// table starts after bitmaps in block 0, runs into block 1
	return blocks_get_block(0) + 2 * BLOCK_BITMAP_SIZE + (inum * sizeof(inode_t));
}

// return the first free inode in the inode table
int alloc_inode() {
	void* ibm = get_inode_bitmap();

	for (int i = 0; i < BLOCK_COUNT; i++) {
		if (!bitmap_get(ibm, i)) {
			// mark inode as taken
			bitmap_put(ibm, i, 1);
			printf("+ alloc_inode() -> %d\n", i);
			return i;
		}
	}
	return -1;
}

// free the inode with the given index in the inode table
void free_inode(int inum) {
	printf("+ free_inode(%d)\n", inum);
	void* ibm = get_inode_bitmap();
	bitmap_put(ibm, inum, 0);
}

