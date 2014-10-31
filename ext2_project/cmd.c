#include "cmd.h"

typedef struct command
{
    char* name;
    int (*cmd)(int, char**);
} COMMAND;

static const COMMAND lookup[] = 
{
    { "?"	    , &menu       },
    { "help"    , &menu       },
    { "menu"    , &menu       },

    { "ls"      , &my_ls      },
    { "cd"      , &my_cd      },
    { "pwd"     , &my_pwd     },
    { "mkdir"   , &my_mkdir   },
    { "rmdir"   , &my_rmdir   },
    { "creat"   , &my_creat   },
    { "link"    , &my_link    },
    { "unlink"  , &my_unlink  },
    { "symlink" , &my_symlink },
    { "rm"      , &my_rm      },
    { "chmod"   , &my_chmod   },
    { "chown"   , &my_chown   },
    { "stat"    , &my_stat    },
    { "touch"   , &my_touch   },

    { "open"    , &my_open    },
    { "close"   , &my_close   },
    { "pfd"     , &my_pfd     },
    { "lseek"   , &my_lseek   },
    { "access"  , &my_access  },
    { "read"    , &my_read    },
    { "write"   , &my_write   },
    { "cat"     , &my_cat     },
    { "cp"      , &my_cp      },
    { "mv"      , &my_mv      },

    { "mount"   , &my_mount   },
    { "umount"  , &my_umount  },
    { "cs"      , &my_cs      },
    { "sync"    , &my_sync    },

    { "quit"    , &quit       },
    { "exit"    , &quit       }
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

int menu(int argc, char* argv[])
{
    print_title("Menu", '=');
    printf("ls  cd  pwd quit\n");
    print_divider('-');

    return 0;
}

int invalid(int argc, char* argv[])
{
    printf("invalid command\n");
    return 0;
}

int quit(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_ls(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_cd(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_pwd(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_mkdir(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_rmdir(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_creat(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_link(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_unlink(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_symlink(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_rm(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_chmod(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_chown(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_stat(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_touch(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_open(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_close(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_pfd(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_lseek(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_access(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_read(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_write(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_cat(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_cp(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_mv(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_mount(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_umount(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_cs(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}

int my_sync(int argc, char* argv[])
{
    printf("This command is not yet implemented\n");
    return 0;
}
