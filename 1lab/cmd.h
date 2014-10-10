#ifndef __CMD_H__
#define __CMD_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int (*get_cmd(char* cmd_name))(char*, char*);

int help  (char * dirname, char* basename);
int mkdir (char * dirname, char* basename);
int rmdir (char * dirname, char* basename);
int ls    (char * dirname, char* basename);
int cd    (char * dirname, char* basename);
int pwd   (char * dirname, char* basename);
int creat (char * dirname, char* basename);
int rm    (char * dirname, char* basename);
int save  (char * dirname, char* basename);
int reload(char * dirname, char* basename);
int quit  (char * dirname, char* basename);


#endif
