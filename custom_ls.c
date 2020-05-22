#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

static char
ftypelet (mode_t bits)
{
  /* These are the most common, so test for them first.  */
  if (S_ISREG (bits))
    return '-';
  if (S_ISDIR (bits))
    return 'd';
  return '?';
}

void
strmode (mode_t mode, char *str)
{
  str[0] = ftypelet (mode);
  str[1] = mode & S_IRUSR ? 'r' : '-';
  str[2] = mode & S_IWUSR ? 'w' : '-';
  str[3] = (mode & S_ISUID
            ? (mode & S_IXUSR ? 's' : 'S')
            : (mode & S_IXUSR ? 'x' : '-'));
  str[4] = mode & S_IRGRP ? 'r' : '-';
  str[5] = mode & S_IWGRP ? 'w' : '-';
  str[6] = (mode & S_ISGID
            ? (mode & S_IXGRP ? 's' : 'S')
            : (mode & S_IXGRP ? 'x' : '-'));
  str[7] = mode & S_IROTH ? 'r' : '-';
  str[8] = mode & S_IWOTH ? 'w' : '-';
  str[9] = (mode & S_ISVTX
            ? (mode & S_IXOTH ? 't' : 'T')
            : (mode & S_IXOTH ? 'x' : '-'));
  str[10] = ' ';
  str[11] = '\0';
}

int f_longform = 0;
int f_all = 0;

static int
one (const struct dirent *ent)
{
  if( !f_all && ent->d_name[0] == '.')
    return 0;
    return 1;
}

static struct option const long_options[] =
{
  {"all", no_argument, NULL, 'a'},
  {NULL, 0, NULL, 0}
};

void printstat(char* name, struct stat* stat_buff)
{
    if(f_longform)
        {
            char time_buff[20];
            strftime(time_buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&(stat_buff->st_atime)));
            char perm_str[12];
            strmode(stat_buff->st_mode, perm_str);
            printf("%s %ld %9.9s %9.9s %7.1ld %s %s\n",
            perm_str,
            stat_buff->st_nlink,
            getpwuid(stat_buff->st_uid)->pw_name,
            getgrgid(stat_buff->st_gid)->gr_name,
            stat_buff->st_size,
            time_buff,
            name);
        }
        else
        {
            printf("%s\n", name);
        }
}

int lsdir(char* dir_path)
{
  struct dirent **eps;
  int n;
  n = scandir (dir_path, &eps, one, alphasort);
  if (n >= 0)
    {
      int cnt;
      struct stat stat_buff;

      for (cnt = 0; cnt < n; ++cnt)
      {
        stat(eps[cnt]->d_name, &stat_buff);
        printstat(eps[cnt]->d_name, &stat_buff);
      }
    }
  else
    perror ("Couldn't open the directory");

  return 0;
}

int
main (int argc, char **argv)
{
  struct dirent **eps;
  int n;
  char ch;
  while((ch = getopt(argc, argv, "1ACFHLRSTacdfghiklmnopqrstux")) != -1)
  {
    switch (ch)
    {
        case 'l':
			f_longform = 1;
			break;
        case 'a':
            f_all = 1;
            break;
    }
  }
  char* dir_path = NULL;
  int idx = 1;
  int nonKeyArgs[argc];
  int fcount = 0;
  while(idx<argc)
  {
    if(argv[idx][0]!='-')
    {
      nonKeyArgs[fcount++] = idx;
    }
    ++idx;
  }
  if(fcount < 2)
  {
    if(!fcount)
        dir_path = "./";
    else
        dir_path = argv[nonKeyArgs[0]];
    lsdir(dir_path);
  }
  else
  {
    idx = 0;
    while(idx<fcount)
    {
        struct stat path_stat;
        stat(argv[nonKeyArgs[idx]], &path_stat);
        if(S_ISDIR(path_stat.st_mode))
        {
        printf("%s:\n", argv[nonKeyArgs[idx]]);
        lsdir(argv[nonKeyArgs[idx]]);
        }
        else
        {
        printstat(argv[nonKeyArgs[idx]], &path_stat);
        }
        ++idx;
    }
  }

  return 0;
}
