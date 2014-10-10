#ifndef __TREE_H__
#define __TREE_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
	char *name;
	char type;
	struct node *child;
	struct node *sibling;
	struct node *parent;
} Node;

typedef enum { false, true } bool;


int initialize(void);

Node* add_node(char* dirname, char* basename, char type);
int remove_node(char* dirname, char* basename, char type);
int remove_node2(Node* deleting);
int set_cwd(char* dirname, char* basename);

Node* get_dir(char* dirname);
Node* get_node(char* dirname, char* basename);
Node* get_sibling(Node* child);
Node* get_last_child(Node* parent);
Node* get_child(char* childname, Node* parent);

bool hasChild(Node* node);
bool isAbsolute(char* path);
bool isRelative(char* path);
bool isDir(Node* node);
bool isFile(Node* node);

int print_cwd(char** wd);
int rpwd(Node* dir, char** wd);
int print_children(char* dirname, char* basename);
int print_tree(FILE *out);
int rec_print_tree(Node* current, FILE *out);
int delete_tree(void);
int rec_delete_tree(Node* current);
int build_tree(FILE *save_file);


#endif
