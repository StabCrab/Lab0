#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

void getNearestStringFromCString(char* string, int* current, char* word)
{
    for (int i = *current; i < strlen(string); ++i)
    {
        if (string[i] != ' ')
        {
            word [i - *current] = string[i];
        }
        else
        {
            *current = i + 1;
            return;
        }
    }
}
int getNearestNumberFromCString(char* string, int* current)
{
    char number [10] = {'\0'};
    for (int i = *current; i < strlen(string); ++i)
    {
        if (string[i] != ' ')
        {
            number [i - *current] = string[i];
        }
        else
        {
            *current = i + 1;
            return atoi(number);
        }
    }
    return -1;
}
int findLast(char* string, char symbol)
{
    for (int i = (int)strlen(string) - 1; i > 0; --i)
    {
        if (string[i] == symbol)
            return i;
    }
    return -1;
}
int addToArchive(FILE* archive, char* path)
{
    FILE* file;
    char name[256];
    struct stat s;
    stat(path, &s);
    int lastSlash = findLast(path, '/');
    for (int i = lastSlash + 1; i <= strlen(path); ++i)
    {
        if (i == strlen(path))
            name[i - lastSlash - 1] = '\0';
        else
            name[i - lastSlash - 1] = path[i];
    }
    int byteCount = 0;
    int ch;
    file = fopen(path, "r");
    if (file == NULL)
    {
        printf ("ERROR: couldn't find file %s", name);

        return 1;
    }
    else
    {
        fprintf(archive, "FILE:\n%s\n", name);
        fprintf(archive,"%d ", s.st_mode);
        fprintf(archive,"%ld", s.st_nlink); // number of hardlinks

        struct passwd * user; // need that to get user data
        user = getpwuid(s.st_uid); // find data using ID from stat
        if (user->pw_name != 0)
            fprintf(archive," %s", user->pw_name);
        else
            fprintf(archive,"no_group");
        struct group * group;
        group = getgrgid(s.st_gid); // just like user, but group now
        if (group->gr_name != 0)
            fprintf(archive," %s ", group->gr_name);
        else
            fprintf(archive,"no_group");
        fprintf(archive,"%ld", (long)s.st_size); //size of the file
        struct tm * time_ptr;
        time_t time = s.st_ctime;
        time_ptr = localtime(&time);
        fprintf(archive," %d", time_ptr->tm_mon + 1);
        fprintf(archive," %d", time_ptr->tm_mday);
        fprintf(archive," %d %d\n", time_ptr->tm_hour, time_ptr->tm_min);
        fprintf(archive, "BEGIN_ARCHIVE\n");
        while ((ch = fgetc(file)) != EOF)
        {
            fprintf(archive,"%c", ch);
            byteCount++;
        }
        fprintf(archive, "END_ARCHIVE\n");
    }
    return 0;
}
int extractFromArchive(char* fileName, char* archName)
{
    FILE * archive;
    archive = fopen(archName, "r");
    strcat(fileName, "\n");
    char buf[100000];
    size_t length = 100000;
    char * b = buf;
    FILE* bufFile;
    bufFile = fopen("buf", "w");
    while (true)
    {
        if (getline(&b, &length, archive) == EOF)
        {
            printf("Couldn't find file");
            fclose(bufFile);
            remove(archName);
            rename("buf", archName);
            return 1;
        }
        if (strcmp(buf, "FILE:\n") == 0)
        {
            getline(&b, &length, archive);
            if (strcmp(buf, fileName) == 0)
                break;
            else
                fprintf(bufFile, "FILE:\n");
        }
        fprintf(bufFile, "%s", buf);
    }
    getline(&b, &length, archive);
    int it = 0;
    int permissions = getNearestNumberFromCString(b, &it);
    int hardlinks = getNearestNumberFromCString(b, &it);
    char user[20] = {'\0'};
    getNearestStringFromCString(b, &it, user);
    char group[20] = {'\0'};
    getNearestStringFromCString(b, &it, group);
    long int size = getNearestNumberFromCString(b, &it);
    int month = getNearestNumberFromCString(b, &it);
    int day = getNearestNumberFromCString(b, &it);
    int hour = getNearestNumberFromCString(b, &it);
    int minute = getNearestNumberFromCString(b, &it);

    getline(&b, &length, archive);
    int c;
    FILE* file;
    if (fileName[strlen(fileName) - 1] == '\n')
        fileName[strlen(fileName) - 1] = '\0';
    file = fopen(fileName, "w");
    if (strcmp(buf, "BEGIN_ARCHIVE\n") == 0)
    {
        for (int i = 0; i < size; ++i)
        {
            c = fgetc(archive);
            fprintf(file, "%c", c);
        }
    }
    getline(&b, &length, archive);
    char path[PATH_MAX];
    char rl_path[PATH_MAX];
    path[0] = '.';
    path [1] = '/';
    strcat(path, fileName);
    realpath(path,rl_path);
    for (int i = 0; i < strlen(rl_path); ++i)
    {
        path[i] = rl_path[i];
    }
    path[strlen(rl_path)] = '\0';

    if (chmod(path, permissions) < 0)
    {
        printf("%s\n",path);
        printf("%d (%s)\n",errno, strerror(errno));
    }
    while ((c = fgetc(archive)) != EOF)
    {
        fprintf(bufFile, "%c", c);
    }
    fclose(archive);
    fclose(bufFile);
    remove(archName);
    rename("buf", archName);
    fclose(file);
    path[0] = '\0';
    rl_path[0] = '\0';
    path[2] = '\0';
    rl_path[2] = '\0';
    return 0;
}
void stats(FILE * archive)
{
    long int overallSize = 0;
    int filesCounter = 0;
    char buf[100000];
    size_t length = 100000;
    char * b = buf;

    while (true)
    {
        if (getline(&b, &length, archive) == EOF)
        {
            break;
        }
        if (strcmp(buf, "FILE:\n") == 0)
        {
            filesCounter++;
            getline(&b, &length, archive);
            getline(&b, &length, archive);
            int it = 0;
            int permissions = getNearestNumberFromCString(b, &it);
            int hardlinks = getNearestNumberFromCString(b, &it);
            char user[20] = {'\0'};
            getNearestStringFromCString(b, &it, user);
            char group[20];
            getNearestStringFromCString(b, &it, group);
            long int size = getNearestNumberFromCString(b, &it);
            overallSize += size;
        }
    }
    printf("There are %d files in archive. Overall size is %ld\n", filesCounter, overallSize);
}
int main(int argc, char** argv)
{
    static struct option long_options[] =
            {
                {"input", no_argument, NULL, 'i'},
                {"extract", no_argument, NULL, 'e'},
                {"stat", no_argument, NULL, 's'},
                {"help", no_argument, NULL, 'h'}
            };
    int option_index = 0;
    int c;
    int argument;
    char archName[200];
    char fileName[10][256];
    short int filesCounter = 0;
    bool isDone = false;
    bool isArchName = false;
    while ((c = getopt_long(argc, argv, "iesh", long_options, &option_index)) != -1)
    {
        if (isDone == false)
        {
            isDone = true;
            argument = c;
        }
        else
        {
            printf("Error: arguments are excessive");
            return 1;
        }
    }

    for(;optind < argc; optind++)
    {
        if (isArchName)
        {
            strcpy(fileName[filesCounter], argv[optind]);
            filesCounter++;
            for (int i = 0; i < filesCounter - 1; ++i)
            {
                if (strcmp(fileName[i], fileName[filesCounter - 1]) == 0)
                {
                    printf("%s Already exist in argument\n", fileName[filesCounter - 1]);
                    fileName[filesCounter - 1][0] = '\0';
                    filesCounter--;
                    break;
                }
            }
        }
        else
        {
            strcpy(archName, argv[optind]);
            isArchName = true;
        }
    }

    if (isDone == false)
    {
        printf("ERROR: no arguments");
        return 1;
    }
    switch (argument)
    {
        case 'i':
        {
            if (!isArchName)
            {
                printf("ERROR: no archive name");
                return 1;
            }
            strcat(archName, ".arch");
            if (filesCounter == 0)
            {
                printf("ERROR: no files to work with");
                return 1;
            }
            FILE* archive;
            for (int i = 0; i < filesCounter; ++i)
            {
                if (strpbrk(fileName[i], "/.") == NULL)
                {
                    char buf [256];
                    buf[0] = '.';
                    buf[1] = '/';
                    buf[2] = '\0';
                    strcat(buf, fileName[i]);
                    realpath(buf, fileName[i]);
                }
                archive = fopen( archName,"a");
                if (archive == NULL)
                {
                    printf("ERROR: something went wrong");
                }
                addToArchive(archive, fileName[i]);
            }
            break;
        }
        case 'e':
        {
            if (!isArchName)
            {
                printf("ERROR: no archive name");
                return 1;
            }
            strcat(archName, ".arch");
            if (filesCounter == 0)
            {
                printf("ERROR: no files to work with");
                return 1;
            }
            FILE * archive;
            archive = fopen(archName, "r");
            for (int i = 0; i < filesCounter; ++i)
            {
                extractFromArchive(fileName[i], archName);
            }
            break;
        }
        case 's':
        {
            FILE * archive;
            archive = fopen(archName, "r");
            if (archive == NULL)
            {
                printf("Can't find archive");
            }
            else
                stats(archive);
            break;
        }
        case 'h':
        {
            printf("Use -s or --stat for stats of archive\nUse -i or --input to input file into archive\nUse -e or --extract to extract file from archive\n");
            break;
        }
    }
}
