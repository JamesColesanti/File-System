// Inode manipulation routines.
//
// Feel free to use as inspiration.

// based on cs3650 starter code
#include "blocks.h"
#include "bitmap.h"

#ifndef INODE_H
#define INODE_H

// struct for representing an inode
typedef struct inode {
  int files;  // number of files (used when inode points to a directory entry)
  int mode;  // permission & type
  int size;  // bytes
  int block; // single block pointer (if max file size <= 4K)
} inode_t;

// return a pointer to the inode with given inum
inode_t *get_inode(int inum);

// return the inumber for the first free inode in the inode table
int alloc_inode();

// free the inode with the given inumber
void free_inode(int inum);

// not supported in this implementation
int grow_inode(inode_t *node, int size);
int shrink_inode(inode_t *node, int size);
int inode_get_bnum(inode_t *node, int fpn);

#endif
