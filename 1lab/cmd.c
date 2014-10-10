#include "cmd.h"
#include "tree.h"

typedef struct
{
	char* name;
	int (*cmd)(char*, char*);
} COMMAND;

static const COMMAND lookup[] = 
{
	{ "?"	  , &help   },
	{ "mkdir" , &mkdir  },
	{ "rmdir" , &rmdir  },
	{ "ls"    , &ls     },
	{ "cd"    , &cd     },
	{ "pwd"   , &pwd    },
	{ "creat" , &creat  },
	{ "rm"    , &rm     },
	{ "save"  , &save   },
	{ "reload", &reload },
	{ "quit"  , &quit   }
};

static const int cmd_count = sizeof(lookup) / sizeof(lookup[0]);


int (*get_cmd(char* cmd_name))(char*, char*)
{
	int i;
	for(i = 0; i < cmd_count; i++)
	{
		if(strcmp(cmd_name, lookup[i].name) == 0)
			return lookup[i].cmd;
	}

	return NULL;
}

int help(char* dirname, char* basename)
{
	printf("======================= MENU ========================\n");
	printf("mkdir rmdir ls  cd  pwd  creat  rm  save reload  quit\n");
	printf("=====================================================\n");

	return 0;
}


int mkdir(char* dirname, char* basename)
{
	if(add_node(dirname, basename,'D') == NULL)
		return -1;

	return 0;
}

int creat(char* dirname, char* basename)
{
	if(add_node(dirname, basename,'F') == NULL)
		return -1;
	
	return 0;
}

int rmdir(char* dirname, char* basename)
{
	if(remove_node(dirname, basename, 'D') != 0)
		return -1;

	return 0;
}

int rm(char* dirname, char* basename)
{
	if(remove_node(dirname, basename, 'F') != 0)
		return -1;

	return 0;
}

int ls(char* dirname, char* basename)
{
	if(print_children(dirname, basename) != 0)
		return -1;

	return 0;
}

int cd(char* dirname, char* basename)
{
	if(set_cwd(dirname, basename) != 0)
		return -1;

	return 0;
}

int pwd(char* dirname, char* basename)
{
	char* wd = (char*)calloc(256, 1);

	if(print_cwd(&wd) != 0)
    {
        free(wd);
		return -1;
    }

	printf("%s\n", wd);
    free(wd);
	return 0;
}


int save(char * dirname, char* basename)
{
	FILE *save_file;
	char* filename;

	printf("Enter a name for your save file: ");

	if(get_input(&filename) != 0)
	{
		fprintf(stderr, "Invalid Filename\n");
        free(filename);
		return -1;
	}

	save_file = fopen(filename, "w");
	if (save_file == NULL) {
		printf("Failed to open %s\n", filename);
        free(filename);
		return -1;
	}

	printf("Saving...\n");
	if(print_tree(save_file) != 0)
        return -1;

	fclose(save_file);
	free(filename);

	return 0;
}

int reload(char * dirname, char* basename)
{
	char* filename;
	FILE *save_file;

	printf("Load file: ");

	if(get_input(&filename) != 0)
	{
		fprintf(stderr, "Invalid Filename\n");
		return -1;
	}

	save_file = fopen(filename, "r");
	if (save_file == NULL) {
		printf("Failed to open %s.\n", filename);
		return -1;
	}

    if(build_tree(save_file) != 0)
        return -1;

    return 0;
}

int quit(char* dirname, char* basename)
{
	FILE *save_file;

    save_file = fopen("default_save", "w");
    printf("Saving file system to file: default_save\n");
    if(print_tree(save_file) != 0)
        return -1;

    fclose(save_file);

    delete_tree();
    exit(0);
}
