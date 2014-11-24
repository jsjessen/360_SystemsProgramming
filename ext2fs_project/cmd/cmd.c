#include <cmd.h>

// General
#include <menu.c>
#include <quit.c>

// Level 1
#include <ls.c>
#include <cd.c>
#include <pwd.c>
#include <mkdir.c>
#include <rmdir.c>
#include <creat.c>
#include <link.c>
#include <unlink.c>
#include <symlink.c>
#include <rm.c>
#include <chmod.c>
#include <chown.c>
#include <stat.c>
#include <touch.c>

// Level 2
#include <open.c>
#include <close.c>
#include <pfd.c>
#include <lseek.c>
#include <access.c>
#include <read.c>
#include <write.c>
#include <cat.c>
#include <cp.c>
#include <mv.c>

// Level 3
#include <mount.c>
#include <umount.c>
#include <cs.c>
#include <sync.c>


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
