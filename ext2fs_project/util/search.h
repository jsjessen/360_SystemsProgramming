// James Jessen       
// 10918967          
// 
// CptS 360

// ----------------------------------
// Functions for searching through an
// ext2 filesystem and its components
// ----------------------------------

#ifndef __SEARCH_H_
#define __SEARCH_H_


#include <stdio.h>
#include <sys/stat.h>

#include <global.h>
#include <input.h>
#include <transfer.h>
#include <memory.h>
#include <print.h>


// Given an MINODE pointer this function finds its name
char* find_name(MINODE *mip);

// Given the parent DIR (MINODE pointer) and myino, this function finds 
// the name string of myino in the parent's data block. This is the SAME
// as SEARCH() by myino, then copy its name string into myname[ ].
int findmyname(MINODE *parent, int myino, char **myname);

// For a DIR Minode, extract the inumbers of . and .. 
// Read in 0th data block. The inumbers are in the first two dir entries.
// CALLER MUST iput(mip)
int findino(MINODE *mip, int *myino, int *parent);

// Assumes the device uses the ext2 filesystem 
// Given the file descriptor of an open device and target file's pathname
// Searches through the device along pathname for target file 
int getino(int device, char* pathname);
// Returns the inode number of target file if it exists
// Returns -1 if the target file does not exist

// Given a the inode number of a directory
// Searches through the directory for the target file
int search(MINODE *mip, char* target);
// Returns the target file's inode number if it exists
// Returns -1 if the target file does not exist


#endif
