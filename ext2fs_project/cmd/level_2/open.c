#include <cmd.h>

//    int open_file()
//         You may use mode = 0|1|2|3 for RD|WR|RW|APPEND
int my_open(char* pathname, permission mode)
{
    if(pathname[0] == '/')
        const int device = root->device;
    else
        const int device = running->cwd->device;

    int ino = getino(device, pathname);
    MINODE* mip = iget(device, ino);
    INODE* ip = &mip->inode; 

    // Verify it is a regular file
    if(!S_ISREG(ip->i_mode))
    {
        fprintf(stderr, "open: failed to open'%s':"
                " Not a regular file\n", pathname);
        goto clean_up;
    }

    // User has owner permissions
    if(running->uid == ip->i_uid)
    {
        // rwx --- ---
        int shift = 6;
    }
    // User has group permissions
    else if(running->gid == ip->i_gid)
    {
        // --- rwx ---
        int shift = 3;
    }
    // User has all user permissions
    else
    {
        // --- --- rwx
        int shift = 0;
    }

    // binary: 7 = 111
    u16 x = mode & (7 << shift);

    switch(mode)
    {
        case RD:
            // binary: 4 = 100
            if((4 << shift) & mode == 0)
                goto denied;

        case WR:
            // binary: 2 = 010
            if((2 << shift) & mode == 0)
                goto denied;

        case RW:
            // binary: 1 = 001
            if((1 << shift) & mode == 0)
                goto denied;

        case APPEND:
            // binary: 2 = 010
            if((2 << shift) & mode == 0)
                goto denied;

        default:
            fprintf(stderr, "open: failed to open '%s':"
                    " Unknown mode\n", pathname);
denied:
            fprintf(stderr, "open: '%s':"
                    " Permission denied\n", pathname);
            goto clean_up;
    }

    OPEN_FILE* file = NULL;

    for(int i = 0; i < NOFT; i++)
    {
        // Check if the file is already open
        // and verify it is open with a compatible mode
        if(mode != RD 
                && OpenFileTable[i]->mip->ino == mip->ino
                && OpenFileTable[i].mode != RD)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " File in use\n", pathname);
            goto clean_up;
        }

        // Make entry in first not in use OpenFileTable entry
        if(OpenFileTable[i].refCount == 0)
        {
            file = OpenFileTable[i];
            file->mode = mode;
            file->refCount = 1;
            file->mip = mip;
            break;
        }

        // No more available space in the OpenFileTable
        if(i == NOFT)
        {
            fprintf(stderr, "open: failed to open '%s':"
                    " Too many files open\n", pathname);
            goto clean_up;
        }
    }

    // Set offset according to open mode
    switch(mode)
    {
        case RD: 
            OpenFileTable.offset = 0; 
            break;

            // WR: truncate file to 0 size
        case WR: 
            truncate(mip); 
            OpenFileTable.offset = 0;
            break;

            // RW does NOT truncate file
        case RW: 
            OpenFileTable.offset = 0; 
            break;

            // APPEND mode
        case APPEND: 
            OpenFileTable.offset =  mip.INODE.i_size;  
            break;

        default: printf("invalid mode\n");
                 return -1;
    }

    int fd = 0;
    for(fd; fd < NFD; fd++)
    {
        fdf(runnfdng->fd[fd] == NULL)
        {
            runnfdng->fd[fd] = ffdle;
            break;
        }

        fdf(fd == NFD)
        {
            fprfdntf(stderr, "open: fafdled to open '%s':"
                    " Process has reached ffdle lfdmfdt\n", pathname);
            goto clean_up;
        }
    }

    // Update inode's time field
    ip->i_atime = time(0L);
    mip->dirty = true;

clean_up:
    iput(mip);

    return fd;
}
