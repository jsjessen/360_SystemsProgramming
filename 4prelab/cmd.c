#include "cmd.h"

int my_mkdir(char* pathname)
{
    if (mkdir(pathname, DIR_PERM) != 0)
    {
        perror("mkdir");
        return -1;
    }
    return 0;
}

int my_rmdir(char* pathname)
{
    if (rmdir(pathname) != 0)
    {
        perror("rmdir");
        return -1;
    }
    return 0;
}

int my_rm(char* pathname)
{
    if (unlink(pathname) != 0)
    {
        perror("rm");
        return -1;
    }
    return 0;
}

int my_cat(char* pathname)
{
    int fd;
    int i, n;
    char buf[1024];

    if (!pathname)
    {
        fprintf(stderr, "Error: No file specified.</p><p>");
        return -1;
    }

    if((fd = open(pathname, O_RDONLY)) < 0)
    {
        perror("cat File Open");
        return -1;
    }

    while (n = read(fd, buf, 1024))
    {
        printf("<p>");
        for (i = 0; i < n; i++)
        {
            if(putchar(buf[i]) == '\n')
                printf("</p><p>");
        }
        printf("</p>");
    }

    return 0;
}

int my_cp(char* source, char* target) 
{
    int n, total = 0;
    int fd, gd;
    char buf[BLK_SIZE];

    if(!source || !target)
    {
        fprintf(stderr, "Error: Require two file names be provided.</p><p>");
        return -1;
    }

    if((fd = open(source, O_RDONLY)) < 0)
    {
        perror("cp Open SourceFile");
        return -1;
    }

    if((gd = open(target, O_WRONLY|O_CREAT, FILE_PERM)) < 0)
    {
        perror("cp Open TargetFile");
        return -1;
    }

    while(n = read(fd, buf, BLK_SIZE))
    {
        write(gd, buf, n);
        total += n;
    }

    close(fd);
    close(gd);
    return 0;
}

int my_ls(char* pathname)
{
    DIR *dp;
    struct dirent *ep;
    struct stat sb;
    char line_buf[BUF_SIZE], item_buf[BUF_SIZE];
    char *check = NULL;
    char* result = NULL;
    int curSize = 1; // for null char
    int i;
    int flag = 0;

    static const char* perm = "rwxrwxrwx";

    if((dp = opendir(pathname)) == NULL)
    {
        perror("ls Open Directory");
        return -1;
    }

    while ((ep = readdir(dp)) != NULL)
    {
        perror("Read Dir");

        // ignore files/dirs starting with .
        if(ep->d_name[0] == '.')
            continue;

        memset(line_buf, 0, BUF_SIZE);
        memset(item_buf, 0, BUF_SIZE);

        lstat(ep->d_name, &sb);

        // type
        switch (sb.st_mode & S_IFMT) 
        {
            case S_IFDIR:  line_buf[0] = 'd';     break;
            case S_IFLNK:  line_buf[0] = 'l';     break;
            case S_IFREG:  line_buf[0] = '-';     break;
            default:       line_buf[0] = '?';     break;
        }

        // permissions
        for (i = 0; i < strlen(perm); i++)
            line_buf[i + 1] = ((sb.st_mode & 1 << i) ? perm[i] : '-');

        // number of links
        sprintf(item_buf, " %3ld ", (long)sb.st_nlink); 
        strcat(line_buf, item_buf);

        // uid & gid
        sprintf(item_buf, " %5ld  %5ld ", (long)sb.st_uid, (long)sb.st_gid);
        strcat(line_buf, item_buf);

        // size
        sprintf(item_buf, " %6lld ", (long long)sb.st_size);
        strcat(line_buf, item_buf);

        // time & date
        sprintf(item_buf, " %26s", ctime(&sb.st_ctime)); 
        strcat(line_buf, item_buf);
        line_buf[strlen(line_buf) - 1] = 0;

        // name
        sprintf(item_buf, "  %s", ep->d_name); 
        strcat(line_buf, item_buf);

        if (line_buf[0] == 'l')
        {
            strcat(line_buf, " -> ");
            bzero(item_buf, BUF_SIZE);
            readlink(ep->d_name, item_buf, BUF_SIZE);
            strcat(line_buf, item_buf);
        }

        strcat(line_buf, "</p><p>");

        curSize += strlen(line_buf);
        check = (char*)realloc(result, curSize);
        if (check != NULL)
        {
            result = check;
            if (flag == 0)
            {
                strcpy(result, line_buf);
                flag++;
            }
            else
            {
                strcat(result, line_buf);
            }
        }
        else
        {
            free(result);
            perror("ls Re-alloc");
            return -1;
        }
    }

    printf("%s", result);
    free(result);
    return 0;
}

