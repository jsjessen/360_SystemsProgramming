WSU CptS 360
============

------------------------
Ext2 File System Project
------------------------

OBJECTIVE:
    Implement FILE CONTENTS operations for EXT2 FS, which include:
    open / close
    lseek
    read / write (regular files)
    cat, cp, mv operations
    readdir
    mount / umount EXT2 file systems


Make Commands:

    'make'			    compiles everything
    
    'make run'		    runs the program	
    
    'make mem_run'	    runs the program through Valgrind to check for memory leaks
    
    'make test'	        tests the program	
    
    'make mem_test'	    tests the program through Valgrind to check for memory leaks
    
    'make disk'		    creates a fresh disk
    
    'make clean'		removes all object files, temp files, and the executable
    
    'make tar'		    adds everything needed to recreate executable into a tar
    
    'make submit'		uploads tar to KC's server
