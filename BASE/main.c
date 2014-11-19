/****************************************************************************
*                   KCW testing ext2 file system                            *
*****************************************************************************/
#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

char names[64][128],*name[64]; 
int fd, dev, n;
int nblocks, ninodes, bmap, imap, inode_start;

//#include "util.c"  // Use YOUR OWN util.c file

/***** THESE ARE PRE_COMPILED as .o files in mylib ********
#include "alloc_dealloc.c"
#include "cd_ls_pwd.c"
#include "mkdir.c"
#include "creat.c"
#include "rmdir.c"
#include "link.c"
#include "unlink.c"
**********************************************************/

int init()
{
  int i, j;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->uid = 0;
    p->cwd = 0;
    p->status = FREE;
    for (j=0; j<NFD; j++)
      p->fd[j] = 0;
  }

  // write C code to initialize OFTs
}

// load root INODE and set root pointer to it
int mount_root(int dev)
{  
  printf("mount_root()\n");
  root = iget(dev, 2);
}

char *disk = "disk";
main(int argc, char *argv[ ])
{
  int ino;
  char buf[BLKSIZE];
  char cmd[64], pathname[26], parameter[256]; 
  char line[256];

  if (argc > 1)
    disk = argv[1];

  printf("checking EXT2 FS ....");
  if ((fd = open(disk, O_RDWR)) < 0){
    printf("open %s failed\n", disk);  exit(1);
  }

  dev = fd;

  /********** read super block at 1024 ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system *****************/
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf); 
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  inode_start = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, inode_start);

  init();  
  mount_root(dev);
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->status = READY;
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  // write YOUR code to creat P1 with uid = 1 for non-super user

  while(1){
    printf("input command: [ls|cd|pwd|mkdir|creat|rmdir|link|symlink|unlink] ");
    fgets(line, 128, stdin);
    line[strlen(line) - 1] = 0;

    if (line[0]==0)
      continue;
    pathname[0]  = 0;
    parameter[0] = 0;

    sscanf(line, "%s %s %s", cmd, pathname, parameter);
    printf("cmd=%s path=%s param=%s\n", cmd, pathname, parameter);

    /***************************************************************
         REPLACE THE FOLLOWING if cases with BETTER CODE
    ***************************************************************/ 
    if (strcmp(cmd, "ls")==0)
       ls(pathname);

    if (strcmp(cmd, "cd")==0)
       cd(pathname);

    if (strcmp(cmd, "pwd")==0)
       pwd(running->cwd);

    if (strcmp(cmd, "mkdir")==0)
       mk_dir(pathname);

    if (strcmp(cmd, "creat")==0)
       creat_file(pathname);

    if (strcmp(cmd, "rmdir")==0)
      rmdir(pathname);

    if (strcmp(cmd, "link")==0)
      link(pathname, parameter);

    if (strcmp(cmd, "unlink")==0)
      unlink(pathname);

    if (strcmp(cmd, "symlink")==0)
      symlink(pathname, parameter);

    if (strcmp(cmd, "readlink")==0){
      readlink(pathname, line);
      printf("symlink name = %s\n", line);
    }

    if (strcmp(cmd, "quit")==0)
       quit();
  }
}
 
int quit()
{
  int i;
  MINODE *mip;
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount > 0)
       iput(mip);
  }
  exit(0);
}
