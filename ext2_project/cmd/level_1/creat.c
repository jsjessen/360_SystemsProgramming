#include <cmd.h>

int my_creat(int argc, char* argv[])
{
    printf("creat: not yet implemented\n");
    return 0;
}

//creat_file()
//{
//  This is ALMOST THE SAME AS mkdir() except : 
//   (1). its inode's mode field is set to a REGULAR file, 
//        permission bits to (default) rw-r--r--, 
//   (2). No data block, so size = 0
//   (3). links_count = 1;
//   (4). Do NOT increment parent's links_count
//} 
//
//
//int my_creat(MINODE *pip; char *name)
//{
//  Same as mymkdir() except 
//    INODE's file type = 0x81A4 OR 0100644
//    links_count = 1
//    NO data block, so size = 0
//    do NOT inc parent's link count.
//}  
