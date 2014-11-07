#include <cmd.h>

// General
#include<menu.c>
#include<my_sync.c>
#include<quit.c>

// Level 1
#include<my_ls.c>
#include<my_cd.c>
#include<my_pwd.c>
#include<my_mkdir.c>
#include<my_rmdir.c>
#include<my_creat.c>
#include<my_link.c>
#include<my_unlink.c>
#include<my_symlink.c>
#include<my_rm.c>
#include<my_chmod.c>
#include<my_chown.c>
#include<my_stat.c>
#include<my_touch.c>

// Level 2
#include<my_open.c>
#include<my_close.c>
#include<my_pfd.c>
#include<my_lseek.c>
#include<my_access.c>
#include<my_read.c>
#include<my_write.c>
#include<my_cat.c>
#include<my_cp.c>
#include<my_mv.c>

// Level 3
#include<my_mount.c>
#include<my_umount.c>
#include<my_cs.c>


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

int invalid(int argc, char* argv[])
{
    printf("invalid command\n");
    return 0;
}

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
