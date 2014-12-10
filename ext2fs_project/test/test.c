#include <level_1.h>
#include <level_2.h>
#include <level_3.h>

typedef struct result_struct
{
    char* str;
    result_t res;
} RESULT;

static const RESULT result_lookup[] =
{
    { "SUCCESS"        , SUCCESS         },
    { "NONE"           , NONE            },
    { "MISSING_OPERAND", MISSING_OPERAND },
    { "ALREADY_EXISTS" , ALREADY_EXISTS  },
    { "DOES_NOT_EXIST" , DOES_NOT_EXIST  },
    { "NO_PARENT"      , NO_PARENT       },
    { "PARENT_NOT_DIR" , PARENT_NOT_DIR  },
    { "IS_DIR"         , IS_DIR          },
    { "NOT_DIR"        , NOT_DIR         },
    { "IS_REG"         , IS_REG          },
    { "NOT_REG"        , NOT_REG         },
    { "PERM_DENIED"    , PERM_DENIED     },
    { "BUSY"           , BUSY            },
    { "NOT_EMPTY"      , NOT_EMPTY       },
    { "NAME_TOO_LONG"  , NAME_TOO_LONG   },
    { "CROSS_DEVICE"   , CROSS_DEVICE    },
    { "MEM_ALLOC_FAIL" , MEM_ALLOC_FAIL  },
    { "BAD_MODE"       , BAD_MODE        },
    { "BAD_FD"         , BAD_FD          },
    { "FILE_LIMIT"     , FILE_LIMIT      },
    { "NOT_OPEN"       , NOT_OPEN        }
};

static int result_count = sizeof(result_lookup) / sizeof(result_lookup[0]);

char* get_result_str(result_t result)
{
    for(int i = 0; i < result_count; i++)
    {
        if(result == result_lookup[i].res)
            return result_lookup[i].str;
    }
    return 0; 
}

void test()
{
    //    printf("Level 1 Testing\n");
    //    level_1();
    //
    printf("***************************\n");
    printf("* Testing File Operations *\n");
    printf("***************************\n\n");
    level_2();
    //
    //    printf("Level 3 Testing\n");
    //    level_3();
}
