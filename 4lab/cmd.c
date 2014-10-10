/* James Jessen
 * 10918967   
 *
 * CptS 360
 * Lab #4 
 */

#include "cmd.h"

#define SMALL_PAD 8
#define MED_PAD 128 
#define BIG_PAD 1024 
#define BUF_SIZE 1024

static const char* Success = "Success!";

typedef struct
{
    char* name;
    char*(*cmd)(char*);
} Command;

static const Command client_lookup[] =
{
    { "help"  , &my_help   },
    { "?"	  , &my_help   },
    { "lcat"  , &my_cat    },
    { "lpwd"  , &my_pwd    },
    { "lls"   , &my_ls     },
    { "lcd"   , &my_cd     },
    { "lmkdir", &my_mkdir  },
    { "lrmdir", &my_rmdir  },
    { "lrm"   , &my_rm     },
    { "quit"  , &my_quit   },
    { "exit"  , &my_quit   },
    { "logout", &my_quit   }
};

static const Command server_lookup[] =
{
    { "cat"   , &my_cat    },
    { "pwd"   , &my_pwd    },
    { "ls"    , &my_ls     },
    { "cd"    , &my_cd     },
    { "mkdir" , &my_mkdir  },
    { "rmdir" , &my_rmdir  },
    { "rm"    , &my_rm     },
};

static const int client_cmd_count = sizeof(client_lookup) / sizeof(client_lookup[0]);
static const int server_cmd_count = sizeof(server_lookup) / sizeof(server_lookup[0]);

char* (*get_cmd(char* cmd_name, char* caller))(char*)
{
    int i;
    int cmd_count;
    const Command* lookup;

    if(!cmd_name || !caller)
        return NULL;

    if(strcmp(caller, "client") == 0)
    {
        lookup = client_lookup;
        cmd_count = client_cmd_count;
    }
    else if(strcmp(caller, "server") == 0)
    {
        lookup = server_lookup;
        cmd_count = server_cmd_count;
    }
    else
        return NULL;

    for (i = 0; i < cmd_count; i++)
    {
        if (strcmp(cmd_name, lookup[i].name) == 0)
            return lookup[i].cmd;
    }
    return NULL;
}

char* my_help(char* path)
{
    char* output = (char*)malloc(BIG_PAD);

    sprintf(output, 
            "======================================================\n"
            "  Server:  cat  pwd   ls   cd   mkdir   rmdir   rm\n"
            "  Client: lcat lpwd  lls  lcd  lmkdir  lrmdir  lrm\n\n"

            "            get     put     help    quit\n"
            "======================================================\n");
    return output;
}

char* my_cat(char* path)
{
    int size, fd, n;
    char* output = NULL;
    char buf[BUF_SIZE];

    if (!path)
    {
        output = (char*)malloc(MED_PAD);
        sprintf(output, "cat: No file specified\n");
        return output;
    }

    if((fd = open(path, O_RDONLY)) < 0)
    {
        char* error = strerror(errno);
        size = strlen(error) + strlen(path) + MED_PAD;
        output = (char*)malloc(size);
        sprintf(output, "cat: cannot open '%s': %s\n", path, error);
        return output;
    }

    size = 1; // 1 for null char
    while (n = read(fd, buf, BUF_SIZE))
    {
        size += n;
        output = (char*)realloc(output, size);
        strncpy(output + (size - n - 1), buf, n);
    }
    output[size - 1] = 0;
    return output;
}

char* my_pwd(char* path)
{
    int size = BUF_SIZE;
    char *cwd = (char*)malloc(size);

    // getcwd() returns NULL if cwd is too small
    while(!getcwd(cwd, size))
    {
        size += size;
        cwd = (char*)realloc(cwd, size);
    }
    return cwd;
}

char* my_ls(char* path)
{
    DIR *dp = NULL;            // Directory Pointer
    struct dirent *ep = NULL;  // Directory Entry Pointer
    struct stat sb;            // Stat Block
    char line_buf[BUF_SIZE];   // Buffer for each line
    char item_buf[BUF_SIZE];   // Buffer for each item within line
    char* output = NULL;       // String of entire output
    int i; 
    int size = 1; // 1 for null char

    static const char* Permissions = "rwxrwxrwx";

    // ls path or if no path is provided, ls cwd
    if (!path || strcmp(path, "") == 0 || path[0] == 0)
    {
        char* cwd = my_pwd(NULL);
        dp = opendir(cwd);
        free(cwd);
    }
    else 
        dp = opendir(path);

    if (!dp)
    {
        char* error = strerror(errno);
        size = strlen(error) + strlen(path) + MED_PAD;
        output = (char*)malloc(size);
        sprintf(output, "ls: opendir(%s): %s\n", path, error);
        return output;
    }

    // Loop through each entry in directory
    while(ep = readdir(dp))
    {
        // Ignore . files
        if(ep->d_name[0] == '.')
            continue;

        memset(line_buf, 0, sizeof(line_buf));
        memset(item_buf, 0, sizeof(item_buf));

        lstat(ep->d_name, &sb);

        // Type 
        // The leading 4 bits of st_mode (2 bytes/16 bits) indicate type
        // 0xF000 = 1111 0000 0000 0000
        switch (sb.st_mode & 0xF000) 
        {
            case 0x8000:  line_buf[0] = '-';     break; // 0x8 = 1000
            case 0x4000:  line_buf[0] = 'd';     break; // 0x4 = 0100
            case 0xA000:  line_buf[0] = 'l';     break; // oxA = 1010
            default:      line_buf[0] = '?';     break;
        }

        // Permissions
        for (i = 0; i < strlen(Permissions); i++)
            line_buf[i + 1] = ((sb.st_mode & 1 << i) ? Permissions[i] : '-');

        // Number of links
        sprintf(item_buf, " %ld ", (long)sb.st_nlink);
        strcat(line_buf, item_buf);

        // User ID & Group ID 
        sprintf(item_buf, " %4ld  %4ld ", (long)sb.st_uid, (long)sb.st_gid);
        strcat(line_buf, item_buf);

        // Size
        sprintf(item_buf, " %6lld ", (long long)sb.st_size);
        strcat(line_buf, item_buf);

        // Time last modified
        sprintf(item_buf, " %26s", ctime(&sb.st_ctime));
        strcat(line_buf, item_buf);
        line_buf[strlen(line_buf) - 1] = 0;

        // Name
        sprintf(item_buf, "  %s", ep->d_name);
        strcat(line_buf, item_buf);

        // Trace link
        if (line_buf[0] == 'l')
        {
            strcat(line_buf, " -> ");
            memset(item_buf, 0, sizeof(item_buf));
            readlink(ep->d_name, item_buf, sizeof(item_buf));
            strcat(line_buf, item_buf);
        }

        // Prepare for next line
        strcat(line_buf, "\n");

        // strcat null-terminates

        size += strlen(line_buf);
        output = (char*)realloc(output, size);
        strcat(output, line_buf);
    }

    closedir(dp);
    return output;
}

char* my_cd(char* path)
{
    int size = 0;
    char* output = NULL;
    char* new_cwd = NULL;
    char* home = getenv("HOME");

    if(path)
        new_cwd = path; 
    else
        new_cwd = home;

    if(chdir(new_cwd) == 0)
    {
        size = strlen(Success) + 1;
        output = (char*)malloc(size);
        strcpy(output, Success);
    }
    else
    {
        char* error = strerror(errno);
        size = strlen(error) + strlen(path) + MED_PAD;
        output = (char*)malloc(size);
        sprintf(output, "cd: '%s': %s\n", new_cwd, error);
    }
    return output;
}

char* my_mkdir(char* path)
{
    int size = 0;
    char* output = NULL;

    if (mkdir(path, DIR_PERM) == 0)
    {
        size = strlen(Success) + 1;
        output = (char*)malloc(size);
        strcpy(output, Success);
    }
    else
    {
        char* error = strerror(errno);
        size = strlen(error) + strlen(path) + MED_PAD;
        output = (char*)malloc(size);
        sprintf(output, "mkdir: cannot create directory '%s': %s\n", path, error);
    }
    return output;
}

char* my_rmdir(char* path)
{
    int size = 0;
    char* output = NULL;

    if (rmdir(path) == 0)
    {
        size = strlen(Success) + 1;
        output = (char*)malloc(size);
        strcpy(output, Success);
    }
    else
    {
        char* error = strerror(errno);
        size = strlen(error) + strlen(path) + MED_PAD;
        output = (char*)malloc(size);
        sprintf(output, "rmdir: cannot remove '%s': %s\n", path, error);
    }
    return output;
}

char* my_rm(char* path)
{
    int size = 0;
    char* output = NULL;

    if (unlink(path) == 0)
    {
        size = strlen(Success) + 1;
        output = (char*)malloc(size);
        strcpy(output, Success);
    }
    else
    {
        char* error = strerror(errno);
        size = strlen(error) + strlen(path) + MED_PAD;
        output = (char*)malloc(size);
        sprintf(output, "rm: cannot remove '%s': %s\n", path, error);
    }
    return output;
}

int my_send(int socket, char* path, char* message)
{
    int fd;
    int n = 0;
    char buf[MAX];
    char* sizeStr = NULL;
    struct stat sb;

    // Check that file exists
    if (stat(path, &sb) != 0)
    {
        // Notify reciever that no transfer will occur
        write(socket, "BAD", MAX);
        return -1;
    }

    // Tell reciever how many bytes to expect during transfer
    sprintf(sizeStr, "SIZE=%d", (int)sb.st_size);
    write(socket, sizeStr, MAX);

    // Open and read from file, send to reciever
    fd = open(path, O_RDONLY);
    while (n = read(fd, buf, MAX))
        write(socket, buf, n);

    close(fd);
    return 0;
}

int my_receive(int socket, char* path, int size)
{
    char* sizeStr = NULL;
    char buf[MAX];
    int count;
    int fd;
    int n = 0;

    // Get information about transfer from sender
    read(socket, sizeStr, MAX);

    // Check that there will be a transfer
    if (strcmp(sizeStr, "BAD") == 0) 
    { 
        //
        return -1;
    }

    // Get size of the transfer from sender 
    sscanf(sizeStr, "SIZE=%d", &size);

    // Write data from sender into spcecified file
    count = 0;
    fd = open(path, O_WRONLY | O_CREAT, FILE_PERM);
    while (count < size)
    {
        n = read(socket, buf, MAX);
        count += n;
        write(fd, buf, n);
    }
    close(fd);
    return 0;
}

char* my_quit(char* path)
{
    exit(EXIT_SUCCESS);
}
