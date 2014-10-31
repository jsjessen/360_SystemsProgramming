#include "cmd.h"

typedef struct
{
	char* name;
	int (*cmd)(int, char**);
} COMMAND;

static const COMMAND lookup[] = 
{
	{ "?"	    , &menu    },
	{ "help"    , &menu    },
	{ "menu"    , &menu    },

	{ "ls"      , &ls      },
	{ "cd"      , &cd      },
	{ "pwd"     , &pwd     },
	{ "mkdir"   , &mkdir   },
	{ "rmdir"   , &rmdir   },
	{ "creat"   , &creat   },
	{ "link"    , &link    },
	{ "unlink"  , &unlink  },
	{ "symlink" , &symlink },
	{ "rm"      , &rm      },
	{ "chmod"   , &chmod   },
	{ "chown"   , &chown   },
	{ "stat"    , &stat    },
	{ "touch"   , &touch   },

	{ "open"    , &open    },
	{ "close"   , &close   },
	{ "pfd"     , &pfd     },
	{ "lseek"   , &lseek   },
	{ "access"  , &access  },
	{ "read"    , &read    },
	{ "write"   , &write   },
	{ "cat"     , &cat     },
	{ "cp"      , &cp      },
	{ "mv"      , &mv      },

	{ "mount"   , &mount   },
	{ "umount"  , &umount  },
	{ "cs"      , &cs      },
	{ "sync"    , &sync    },

	{ "quit"    , &quit    },
	{ "exit"    , &quit    }
};

static const int cmd_count = sizeof(lookup) / sizeof(lookup[0]);

int (*get_cmd(char* cmd_name))(int, char**)
{
	int i;
	for(i = 0; i < cmd_count; i++)
	{
		if(strcmp(cmd_name, lookup[i].name) == 0)
			return lookup[i].cmd;
	}
	return &invalid;
}

int invalid(int argc, char* argv[])
{
    printf("invalid command\n");
	return 0;
}

int menu(int argc, char* argv[])
{
    print_title("Menu", '=');
	printf("ls  cd  pwd quit\n");
    print_divider('-');

	return 0;
}


int ls(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int cd(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int pwd(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int mkdir(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int rmdir(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int creat(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int link(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int unlink(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int symlink(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int rm(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int chmod(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int chown(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int stat(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int touch(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int open(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int close(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int pfd(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int lseek(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int access(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int read(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int write(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int cat(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int cp(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int mv(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int mount(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int umount(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int cs(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int sync(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int quit(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}
