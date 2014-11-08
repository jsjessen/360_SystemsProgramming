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

// Assumes the device uses the ext2 filesystem 
// Given the file descriptor of an open device and target file's pathname
// Searches through the device along pathname for target file 
int search_fs(int device, char* pathname);
// Returns the inode number of target file if it exists
// Returns -1 if the target file does not exist

// Given a the inode number of a directory
// Searches through the directory for the target file
int search_dir(int device, int dir, char* target);
// Returns the target file's inode number if it exists
// Returns -1 if the target file does not exist


#endif
