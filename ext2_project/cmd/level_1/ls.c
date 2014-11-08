#include <cmd.h>

// ls a/b/c c/d/e
int my_ls(int argc, char* argv[])
{
//    int dev = running->cwd->dev;
//    int ino = 0;
//    MINODE *mip = NULL;
//
//    // ls each path given by user
//    int i = 1;
//    while(argv[i])
//    {
//        printf("%s:\n", argv[i]);
//
//        ino = getino(dev, argv[i]);
//        mip = iget(dev,ino);
//
//        if(mip->inode->i_mode == EXT2_FS_IFREG)
//        {
//            list_file(mip, basename(pathname));
//        }
//        else if(mip->inode->i_mode  == EXT2_FS_IFDIR)
//        {
//            list_dir(mip);
//        }
//        else
//        {
//            //0x1234
//            printf("%s: unknown file type (%#6x)", argv[i], mip->inode->i_mode);
//
//        }
//        //iput(mip); // ???
//    }
    printf("you have reached ls!\n");
    return 0;
}
