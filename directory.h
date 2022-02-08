// Directory manipulation functions.
//
// Feel free to use as inspiration.

// based on cs3650 starter code

#ifndef DIRECTORY_H
#define DIRECTORY_H

#define DIR_NAME_LENGTH 20

#include "blocks.h"
#include "inode.h"
#include "slist.h"
#include <sys/stat.h>

// used to keep track of name and inumber of files
// directories do not get entries in this implementation (they hold the
// entries of their files)
typedef struct dirent {
  char name[DIR_NAME_LENGTH];
  int inum;
} dirent_t;

// Initalize the root directory data block (the only directory supported 
// by this implementation).
void directory_init();

// Determine if the given inode's directory has an try with the given name. Return the
// inumber of the file if true, return -1 if false.
int directory_lookup(inode_t *dd, const char *name);

// Create a new directory entry with the given name and inum. Place its dirent_t struct
// in the given inode's directory data block.
int directory_put(inode_t *dd, const char *name, mode_t mode, int inum);

#endif
