//********** main.c **********
//
// James Jessen       Group #1
// 10918967            Seq. #4
// 
// CptS 360
// PreLab #5
//
// --------------------------------------------------------------
// Displays the disk blocks of a file in an EXT2 file system.
// The showblock program runs as follows:
//
//        showblock   DEVICE    PATHNAME
//
// e.g.   showblock   mydisk    /a/b/c/d
//
// It locates the file named PATHNAME and prints the 
// disk blocks (direct, indirect, double-indirect) of the file.
// --------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ext2fs/ext2_fs.h>
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

#define BLKSIZE 1024
#define MAX_NUM_ARGS 32
#define TITLE_WIDTH 50

SUPER   *sp;
GD      *gp;
INODE   *ip;
DIR     *dp;

INODE myinode;

char buf[1024];
int fd;
int inodeBegin;
int inodesPerBlock;
int blockSize;


int get_block(int fd, int blk, char *buf)
{
    lseek(fd, (long)(blk * BLKSIZE), 0);
    read(fd, buf, BLKSIZE);
}


INODE *iget(int ino)
{
    char buf[BLKSIZE];
    int blk, offset;

    // Inode count starts from 1 not 0, so need -1
    blk = (ino - 1) / inodesPerBlock + inodeBegin;
    offset = (ino - 1) % inodesPerBlock;
    printf("\niget(): ino=%d blk=%d offset=%d\n", ino, blk, offset);

    get_block(fd, blk, buf);
    ip = (INODE *)buf + offset;
    myinode = *ip;

    return &myinode;
}


void print(INODE *ip)
{
    int i;
    char *cp;
    DIR *dp;
    char buf[BLKSIZE];
    char temp[256];

    for (i = 0; i < 12; i++)
    {
        if (ip->i_block[i] == 0)
            break;

        printf("block = %d\n", ip->i_block[i]); 
        printf("\n Inode #   Record Length   Name Length   Name \n");

        get_block(fd, ip->i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;

        while (cp < buf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            printf(" %4d          %4d          %4d        %s\n",
                    dp->inode, dp->rec_len, dp->name_len, temp);

            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
}


int search(INODE *ip, char *name)
{
    int i;
    char *cp;
    DIR *dp;
    char buf[BLKSIZE];
    char temp[256];

    // For DIR inodes, assume that (the number of entries is small so that) only has
    // 12 DIRECT data blocks. Therefore, search only the direct blocks for name[0].
    for (i = 0; i < 12; i++)
    {
        if (ip->i_block[i] == 0)
            break;

        get_block(fd, ip->i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;

        while (cp < buf + BLKSIZE)
        {
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;

            if (strcmp(name, temp) == 0)
                return dp->inode;

            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }

    return 0;    
}


void printTitle(char* title, char symbol)
{
    int length = strlen(title) + 2;
    int width = TITLE_WIDTH;
    int i;

    printf("\n\n");
    for (i = 0; i < width - length; i++)
    {
        if (i != (width - length) / 2)
            putchar(symbol);
        else
            printf(" %s ", title);
    }
    putchar('\n');
}
void printDivider(char symbol)
{
    int width = TITLE_WIDTH;
    int i;

    for (i = 0; i < width - 1; i++)
        putchar(symbol);
    putchar('\n');
}
void printSuper()
{
    printTitle("Super Block Info", '*');
    printf("inodes_count            =                    %4u\n", sp->s_inodes_count);
    printf("blocks_count            =                    %4u\n", sp->s_blocks_count);
    printf("free_inodes_count       =                    %4u\n", sp->s_free_inodes_count);
    printf("free_blocks_count       =                    %4u\n", sp->s_free_blocks_count);
    printf("log_block_size          =                    %4u\n", sp->s_log_block_size);
    printf("blocks_per_group        =                    %4u\n", sp->s_blocks_per_group);
    printf("inodes_per_group        =                    %4u\n", sp->s_inodes_per_group);
    printf("mnt_count               =                    %4u\n", sp->s_mnt_count);
    printf("max_mnt_count           =                    %4u\n", sp->s_max_mnt_count);
    printf("magic                   =                    %4x\n", sp->s_magic);
    printf("inode_size              =                    %4u\n", sp->s_inode_size);
}
void printGD()
{
    printTitle("Group Descriptor Info", '*');
    printf("block_bitmap            =                    %4u\n", gp->bg_block_bitmap);
    printf("inode_bitmap            =                    %4u\n", gp->bg_inode_bitmap);
    printf("inode_table             =                    %4u\n", gp->bg_inode_table);
    printf("free_blocks_count       =                    %4u\n", gp->bg_free_blocks_count);
    printf("free_inodes_count       =                    %4u\n", gp->bg_free_inodes_count);
    printf("used_dirs_count         =                    %4u\n", gp->bg_used_dirs_count);
}
void printInode()
{
    printTitle("Root Inode Info", '*');
    printf("inode_block             =                    %4u\n", 5);
    printf("mode                    =                    %4x\n", ip->i_mode);
    printf("uid                     =                    %4u\n", ip->i_uid);
    printf("gid                     =                    %4u\n", ip->i_gid);
    printf("size                    =                    %4u\n", ip->i_size);
    printf("links_count             =                    %4u\n", ip->i_links_count);
    printf("i_block[0]              =                    %4u\n", ip->i_block[0]);
    printDivider('-');
}

// MAIN
int main(int argc, char *argv[])
{
    char *tok_ptr;
    char *dev, *pathname; 
    char *name[MAX_NUM_ARGS];
    int n;
    int ino;
    int i, j, k;
    int dbuf[3][256];

    if (argc != 3)
    {
        printf("Usage:\n");
        printf("         showblock   DEVICE    PATHNAME\n");
        printf("  e.g.   showblock   mydisk    /a/b/c/d\n\n");
        exit(1);
    }

    dev = argv[1];
    pathname = argv[2];

    fd = open(dev, O_RDONLY); //Note: add WRITE for project 
    if (fd < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    // Populate name[] with pathname using '/' as the delimiter
    i = 0;
    tok_ptr = strtok(pathname, "/");
    while(tok_ptr != NULL)
    {
        int length = strlen(tok_ptr) + 1; // +1 for null char
        name[i] = (char*)calloc(length, sizeof(char) * length);
        strcpy(name[i], tok_ptr);
        tok_ptr = strtok(NULL, "/");
        i++;
    }
    name[i] = NULL;
    n = i; // n = number of components in name[]

    // Super Block
    get_block(fd, 1, buf); 
    sp = (SUPER *)buf;
    if (sp->s_magic != 0xEF53)
    {
        printf("Your device '%s' is NOT using the ext2/3 file system!\n", dev);
        printf("Magic Number = %4x\n", sp->s_magic);
        exit(1);
    }
    printSuper();
    printDivider('-');
    blockSize = 1024 << sp->s_log_block_size;
    inodesPerBlock = blockSize / sp->s_inode_size;
    printf("InodesPerBlock = %d", inodesPerBlock);

    // Group Descriptor Block
    get_block(fd, 2, buf);     
    gp = (GD *)buf;
    printGD();
    inodeBegin = gp->bg_inode_table; // Mark start of inode blocks
    printDivider('-');
    printf("inodeBegin = %d", inodeBegin);

    // Root Inode
    ip = iget(2);
    printInode();
    printTitle("Root Dir Entries", '*');
    print(ip);

    // Follow user provided pathname 
    for (i = 0; i < n; i++)
    {
        ino = search(ip, name[i]);
        if (ino == 0)
        {
            printf("\n Failure: '%s' does not exist!\n\n", pathname);
            exit(1);
        }

        printf("ino = %d\n", ino);
        ip = iget(ino);

        if (S_ISDIR(ip->i_mode) && i < n - 1)
        {
            printDivider('=');
            print(ip);
        }
    }
    printDivider('-');
    printf(" Success: '%s' has been found!\n", name[n-1]);

    // Disk Blocks - list i_block[] non-zeroes
    printTitle("Disk Blocks", '*');
    printf("size=%d  blocks=%d\n", ip->i_size, (ip->i_size + BLKSIZE - 1) / BLKSIZE);
    for (i = 0; i <= 13 && ip->i_block[i] > 0; i++)
        printf("\nblock[%2d] = %d", i, ip->i_block[i]);

    // Direct Blocks - The first 12 blocks are direct blocks.
    printTitle("Direct Blocks", '=');
    for (i = 0; i < 12; i++)
    {
        if (i % 10 == 0)
            putchar('\n');
        else   
            putchar(' ');

        if (ip->i_block[i] == 0)
        {
            printf("\n\n");
            return 0;
        }
        else
            printf("%4d", ip->i_block[i]);
    }

    //The 13th entry in this array is the block number of the first indirect block; which is a block containing an array of block ID containing the data. Therefore, the 13th block of the file will be the first block ID contained in the indirect block. With a 1KiB block size, blocks 13 to 268 of the file data are contained in this indirect block.
    printTitle("Indirect Blocks", '=');
    get_block(fd, ip->i_block[12], (char*)dbuf[0]);
    printf(" %d:\n", ip->i_block[12]);
    printDivider('-');

    for (i = 0; i < blockSize / sizeof(int); i++)
    {
        if (i % 10 == 0)
            putchar('\n');
        else   
            putchar(' ');

        if (dbuf[i] == 0)
        {
            printf("\n\n");
            return 0;
        }
        else
            printf("%4d", dbuf[0][i]);
    }

    //The 14th entry in this array is the block number of the first doubly-indirect block; which is a block containing an array of indirect block IDs, with each of those indirect blocks containing an array of blocks containing the data. In a 1KiB block size, there would be 256 indirect blocks per doubly-indirect block, with 256 direct blocks per indirect block for a total of 65536 blocks per doubly-indirect block.
    printTitle("Double Indirect Blocks", '=');
    get_block(fd, ip->i_block[13], (char*)dbuf[0]);  

    for (i = 0; i < blockSize / sizeof(int); i++)
    {
        get_block(fd, dbuf[0][i], (char*)dbuf[1]);
        if (i > 0)
        {
            putchar('\n'); 
            printDivider('-');
        }
        printf(" %d -> %d:\n", ip->i_block[13], dbuf[0][i]);

        for (j = 0; j < blockSize / sizeof(int); j++)
        {
            if (j % 10 == 0)
                putchar('\n');
            else   
                putchar(' ');

            if (dbuf[1][j] == 0)
            {
                printf("\n\n");
                return 0;
            }
            else
                printf("%4d", dbuf[1][j]);
        }
    }

    //The 15th entry in this array is the block number of the triply-indirect block; which is a block containing an array of doubly-indrect block IDs, with each of those doubly-indrect block containing an array of indrect block, and each of those indirect block containing an array of direct block. In a 1KiB file system, this would be a total of 16777216 blocks per triply-indirect block.
    printTitle("Triple Indirect Blocks", '=');
    get_block(fd, ip->i_block[14], (char*)dbuf[0]);  

    for (i = 0; i < blockSize / sizeof(int); i++)
    {
        get_block(fd, dbuf[0][i], (char*)dbuf[1]);

        for (j = 0; j < blockSize / sizeof(int); j++)
        {
            get_block(fd, dbuf[1][j], (char*)dbuf[2]);
            putchar('\n'); printDivider('-');
            printf(" %d -> %d -> %d:\n", ip->i_block[14], dbuf[0][i], dbuf[1][j]);

            for (k = 0; k < blockSize / sizeof(int); k++)
            {
                if (k % 10 == 0)
                    putchar('\n');
                else   
                    putchar(' ');

                if (dbuf[2][k] == 0)
                {
                    printf("\n\n");
                    return 0;
                }
                else
                    printf("%4d", dbuf[2][k]);
            }
        }
    }

    return 0;
}
