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
    { "SUCCESS"   , SUCCESS   },
    { "FAILURE"   , FAILURE   },
    { "EXISTS"    , EXISTS    },
    { "NOT_EXIST" , NOT_EXIST }
};

static int result_count = sizeof(result_lookup) / sizeof(result_lookup[0]);

result_t get_result(char* result_str)
{
    for(int i = 0; i < result_count; i++)
    {
        if(strcmp(result_str, result_lookup[i].str) == 0)
            return result_lookup[i].res;
    }
    return 0; 
}

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
    printf("Level 2 Testing\n");
    level_2();
    //
    //    printf("Level 3 Testing\n");
    //    level_3();
}
