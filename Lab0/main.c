#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>
char path[PATH_MAX];

char * monthNumberToName(int monthNumber)
{
    char* month;
    month = calloc(sizeof(char) * 3, sizeof(char));
    switch (monthNumber)
    {
        case 1:
            strcpy(month, "Jan");
            break;
        case 2:
            strcpy(month, "Feb");
            break;
        case 3:
            strcpy(month, "Mar");
            break;
        case 4:
            strcpy(month, "Apr");
            break;
        case 5:
            strcpy(month, "May");
            break;
        case 6:
            strcpy (month, "Jun");
            break;
        case 7:
            strcpy(month, "Jul");
            break;
        case 8:
            strcpy(month, "Aug");
            break;
        case 9:
            strcpy(month, "Sep");
            break;
        case 10:
            strcpy(month, "Oct");
            break;
        case 11:
            strcpy(month, "Nov");
            break;
        case 12:
            strcpy (month, "Dec");
            break;
    }
    return month;
}
void myls(struct dirent * dir, bool isPath)
{
    char buf[PATH_MAX];
    buf[0] = '\0';
    struct stat s;
    memset(&s, 0, sizeof(struct stat));
    if (isPath)
    {
        strcat(buf, path);
        strcat(buf, "/");
        strcat(buf,dir->d_name);
        stat(buf, &s);

    }
        //printf("%s \n",buf);
    else
    {
        path[0] = '.';
        path[1] = '\0';
        strcat(buf, path);
        strcat(buf, "/");
        strcat(buf,dir->d_name);
        stat(buf, &s);
    }

    if (s.st_mode & S_IFDIR) //is directory
        printf("d");
    else
        printf("-");

    if (s.st_mode & S_IRUSR) // read permission owner
        printf("r");
    else
        printf("-");

    if (s.st_mode & S_IWUSR) // write permission owner
        printf("w");
    else
        printf("-");

    if (s.st_mode & S_IXUSR) // execute permission owner
        printf("x");
    else
        printf("-");

    if (s.st_mode & S_IRGRP) // read permission group
        printf("r");
    else
        printf("-");

    if (s.st_mode & S_IWGRP) // write permission group
        printf("w");
    else
        printf("-");

    if (s.st_mode & S_IXGRP) // execute permission group
        printf("x");
    else
        printf("-");

    if (s.st_mode & S_IROTH) // read permission other
        printf("r");
    else
        printf("-");

    if (s.st_mode & S_IWOTH) // write permission other
        printf("w");
    else
        printf("-");

    if (s.st_mode & S_IXOTH) // execute permission other
        printf("x");
    else
        printf ("-");
    printf(" %2ld", s.st_nlink); // number of hardlinks

    struct passwd * user; // need that to get user data
    user = getpwuid(s.st_uid); // find data using ID from stat
    if (user->pw_name != 0)
        printf(" %s", user->pw_name);
    else
        printf(" ");
    struct group * group;
    group = getgrgid(s.st_gid); // just like user, but group now
    if (group->gr_name != 0)
        printf(" %s ", group->gr_name);
    else
        printf(" ");
    printf("%5ld", (long)s.st_size); //size of the file
    struct tm * time_ptr;
    time_t time = s.st_ctime;
    time_ptr = localtime(&time);
    printf(" %s", monthNumberToName(time_ptr->tm_mon + 1));
    printf(" %02d", time_ptr->tm_mday);
    printf(" %02d:%02d ", time_ptr->tm_hour, time_ptr->tm_min);
    printf("%s", dir->d_name);
    printf("\n");
}

int main (int argc, char** argv)
{
    static struct option long_options[] =
    {
        {"l", no_argument, NULL, 'l'}
    };
    int option_index = 0;
    struct dirent *de;
    int c;
    bool isPath = false;
    bool isL = false;
    int counter = 1;
    int countToPath;
    while (counter < argc)
    {
        if ((c = getopt_long(argc, argv, "l", long_options, &option_index)) != -1)
        {
            switch (c)
            {
                case 'l':
                    isL = true;
                    break;
                default :
                    printf("ERROR");
                    return 1;
            }
        }
        else
        {
            countToPath = counter;
            isPath = true;
        }
        counter++;
    }
    //printf("%s \n",argv[countToPath]);
    DIR *dir;
    if (isPath)
    {
        realpath(argv[countToPath], path);
        dir = opendir(path);
    }
    else
        dir = opendir(".");
    if (dir == NULL)
    {
        printf("ERROR: Could not open current directory" );
        return 1;
    }
    if (isL)
    {
        struct stat s;
        memset(&s, 0, sizeof(struct stat));
        if (isPath)
        {
            stat(path, &s);
        }
        else
            stat(".", &s);

        printf("total %ld \n", s.st_blocks);
        while ((de = readdir(dir)) != NULL)
        {
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;
            else
                myls(de, isPath);
        }
    }
    else
    {
        while ((de = readdir(dir)) != NULL)
        {
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;
            else
                printf("%s\n", de->d_name);
        }

    }

}
