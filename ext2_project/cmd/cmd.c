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

int (*get_cmd(char* cmd_name))(int, char**)
{
    int i = 0;
    static int cmd_count = sizeof(lookup) / sizeof(lookup[0]);

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
    int i = 0;

    for(i = 0; i < NMINODES; i++)
    {
        MINODE* mip = &MemoryInodeTable[i];

        mip->refCount = 1;
        iput(mip);
    }
    exit(0);
}
