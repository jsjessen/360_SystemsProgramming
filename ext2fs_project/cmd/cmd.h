#ifndef __CMD_H__
#define __CMD_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <global.h>
#include <print.h>
#include <util.h>
#include <search.h>


int (*get_cmd(char* cmd_name))(int, char**);

int menu       (int argc, char* argv[]);
int quit       (int argc, char* argv[]);

int my_ls      (int argc, char* argv[]);
int my_cd      (int argc, char* argv[]);
int my_pwd     (int argc, char* argv[]);
int my_mkdir   (int argc, char* argv[]);
int my_rmdir   (int argc, char* argv[]);
int my_creat   (int argc, char* argv[]);
int my_link    (int argc, char* argv[]);
int my_unlink  (int argc, char* argv[]);
int my_symlink (int argc, char* argv[]);
int my_rm      (int argc, char* argv[]);
int my_chmod   (int argc, char* argv[]);
int my_chown   (int argc, char* argv[]);
int my_stat    (int argc, char* argv[]);
int my_touch   (int argc, char* argv[]);

int my_open    (int argc, char* argv[]);
int my_close   (int argc, char* argv[]);
int my_pfd     (int argc, char* argv[]);
int my_lseek   (int argc, char* argv[]);
int my_access  (int argc, char* argv[]);
int my_read    (int argc, char* argv[]);
int my_write   (int argc, char* argv[]);
int my_cat     (int argc, char* argv[]);
int my_cp      (int argc, char* argv[]);
int my_mv      (int argc, char* argv[]);

int my_mount   (int argc, char* argv[]);
int my_umount  (int argc, char* argv[]);
int my_cs      (int argc, char* argv[]);
int my_sync    (int argc, char* argv[]);


#endif
