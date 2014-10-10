//********** shared.c **********

/* James Jessen             Group #1
 * 10918967                  Seq. #4
 *
 * Austin McShane           Group #2
 * 11350053                  Seq. #29
 *
 * CptS 360
 * Lab #4
 */

#include "shared.h"


char* my_pwd(char* pathname)
{
  char buf[MAX];
  char *cwd;

  getcwd(buf, sizeof(buf));
  cwd = (char*)malloc((strlen(buf) + 1) * sizeof(char));
  if (cwd != NULL)
    {
      strcpy(cwd, buf);
    }
  else
    {
      free(cwd);
      fprintf(stderr, "Error allocating memory");
      exit(EXIT_FAILURE);
    }
  
  return cwd;
}//my_pwd()


char* my_mkdir(char* pathname)
{
  if (mkdir(pathname, DIR_PERM) != 0)
    {
      fprintf(stderr, "%s\n", strerror(errno));
    }
  return NULL;
}//my_mkdir()


char* my_rmdir(char* pathname)
{
  if (rmdir(pathname) != 0)
    {
      fprintf(stderr, "%s\n", strerror(errno));
    }
  return NULL;
}//my_rmdir()


char* my_rm(char* pathname)
{
  if (remove(pathname) != 0)
    {
      fprintf(stderr, "%s\n", strerror(errno));
    }
  return NULL;
}//my_rm()


char* my_cd(char* pathname)
{
  char newpath[MAX];

  if (pathname[0] == 0) // no arg1, go to $HOME
    {
      strcpy(newpath, getenv("HOME"));
    }
  else
    {
      strcpy(newpath, pathname);

      if (pathname[0] == '~') // relative to $HOME
	{
	  strcpy(newpath, getenv("HOME"));
	  strcat(newpath, pathname + 1); // +1 to bypass '~'
	}
      else // absolute or relative to CWD (chdir handles relative)
	{
	  strcpy(newpath, pathname);
	}
    }

  // chdir() Success/Failure
  if (chdir(newpath) != 0)
    {
      fprintf(stderr, "%s\n", strerror(errno));
    }

  return NULL;
}//my_cd()


char* my_cat(char* pathname)
{
  int fd;
  int i, n;
  char buf[1024];

  if (pathname == NULL)
    {
      fprintf(stderr, "Error: No file specified.\n");
      return NULL;
    }

  fd = open(pathname, O_RDONLY);

  if (fd < 0)
    {
      fprintf(stderr, "Error: Unable to open '%s'\n", pathname);
      return NULL;
    }

  while (n = read(fd, buf, 1024))
    {
      for (i = 0; i < n; i++)
	{
	  putchar(buf[i]);
	}
    }

  return NULL;
}//my_cat()


char* my_ls(char* pathname)
{
  DIR *dp;
  struct dirent *ep;
  struct stat sb;
  char cwd[MAX], buf[MAX], minibuf[MAX];
  char *check = NULL;
  char* result = NULL;
  int curSize, i, flag = 0;

  static const char* perm = "rwxrwxrwx";

  if (pathname[0] == 0) // no pathname, use cwd
    {
      getcwd(cwd, sizeof(cwd));
      dp = opendir(cwd);
    }
  else // use pathname
    {
      dp = opendir(pathname);
    }
  if (dp == NULL)
    {
      fprintf(stderr, "%s\n", strerror(errno));
      return NULL;
    }

  curSize = 1; // for null char
  // loop through each entry in directory
  while ((ep = readdir(dp)) != NULL)
    {
      bzero(buf, MAX);

      lstat(ep->d_name, &sb);

      // type
      switch (sb.st_mode & S_IFMT) {
      case S_IFDIR:  buf[0] = 'd';     break;
      case S_IFLNK:  buf[0] = 'l';     break;
      case S_IFREG:  buf[0] = '-';     break;
      default:       buf[0] = '?';     break;
      }

      // permissions
      for (i = 0; i < strlen(perm); i++)
	{
	  buf[i + 1] = ((sb.st_mode & 1 << i) ? perm[i] : '-');
	}

      sprintf(minibuf, " %3ld ", (long)sb.st_nlink); // number of links
      strcat(buf, minibuf);

      sprintf(minibuf, " %5ld  %5ld ", (long)sb.st_uid, (long)sb.st_gid); // uid/gid 
      strcat(buf, minibuf);

      sprintf(minibuf, " %6lld ", (long long)sb.st_size); // size
      strcat(buf, minibuf);

      sprintf(minibuf, " %26s", ctime(&sb.st_ctime)); // time/date
      strcat(buf, minibuf);
      buf[strlen(buf) - 1] = 0;

      sprintf(minibuf, "  %s", ep->d_name); // name
      strcat(buf, minibuf);
      if (buf[0] == 'l')
	{
	  strcat(buf, " -> ");
	  bzero(minibuf, MAX);
	  readlink(ep->d_name, minibuf, MAX);
	  strcat(buf, minibuf);
	}

      strcat(buf, "\n");

      curSize += strlen(buf) * sizeof(char);
      check = (char*)realloc(result, curSize);
      if (check != NULL)
	{
	  result = check;
	  if (flag == 0)
	    {
	      strcpy(result, buf);
	      flag++;
	    }
	  else
	    {
	      strcat(result, buf);
	    }
	}
      else
	{
	  free(result);
	  fprintf(stderr, "Error (re)allocating memory");
	  exit(EXIT_FAILURE);
	}
    }

  result[strlen(result) - 1] = 0; // remove last newline
  return result;
}//my_ls()
