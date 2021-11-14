#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
enum Client
{
    user,
    group,
    other
};
enum Function
{
    add,
    delete
};
enum Attributes
{
    read,
    write,
    execute,
    none
};
int main(int argc, char** argv)
{
    char fileName[PATH_MAX];
    enum Client client;
    enum Function function;
    enum Attributes attributes[3];
    for (int i = 0; i < 3; ++i)
    {
        attributes[i] = none;
    }
    bool isHelp = false;
    bool isClient = false;
    bool isFunction = false;
    short int attributesCount = 0;
    int c;
    while ((c = getopt(argc, argv, "hugorawx")) != -1)
    {
        switch (c)
        {
            case 'h':
            {
                if (isClient == false && isFunction == false && attributesCount == 0)
                    isHelp = true;
                else
                {
                    printf("ERROR\n");
                    return 0;
                }
                break;
            }
            case 'u':
            {
                if (isHelp)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isClient == false)
                {
                    client = user;
                    isClient = true;
                }
                else
                {
                    printf("ERROR\n");
                    return 0;
                }
                break;
            }
            case 'g':
            {
                if (isHelp)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isClient == false)
                {
                    client = group;
                    isClient = true;
                }
                else
                {
                    printf("ERROR\n");
                    return 0;
                }
                break;
            }
            case 'o':
            {
                if (isHelp)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isClient == false)
                {
                    client = other;
                    isClient = true;
                }
                else
                {
                    printf("ERROR\n");
                    return 0;
                }
                break;
            }
            case 'a':
            {
                if (isHelp)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isClient == false)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isFunction == true)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isFunction == false)
                {
                    function = add;
                    isFunction = true;
                }
                break;
            }
            case 'r':
            {
                if (isHelp)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isClient == false)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isFunction == true)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        if (attributes[i] == read)
                        {
                            printf("ERROR\n");
                            return 0;
                        }
                    }
                    attributes[attributesCount] = read;
                    attributesCount++;
                }
                if (isFunction == false)
                {
                    function = delete;
                    isFunction = true;
                }
                break;
            }
            case 'w':
            {
                if (isHelp)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isClient == false)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isFunction == false)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isFunction == true)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        if (attributes[i] == write)
                        {
                            printf("ERROR\n");
                            return 0;
                        }
                    }
                    attributes[attributesCount] = write;
                    attributesCount++;
                }
                break;
            }
            case 'x':
            {
                if (isHelp)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isClient == false)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isFunction == false)
                {
                    printf("ERROR\n");
                    return 0;
                }
                if (isFunction == true)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        if (attributes[i] == execute)
                        {
                            printf("ERROR\n");
                            return 0;
                        }
                    }
                    attributes[attributesCount] = execute;
                    attributesCount++;
                }
                break;
            }
        }
    }
    if (isHelp)
    {
        printf("Help Info");
        return 0;
    }
    if (isClient == false || isFunction == false || attributesCount == 0)
    {
        printf("ERROR");
        return 0;
    }
    bool isName = false;
    for (; optind < argc; optind++)
    {
        if (isName)
        {
            printf("ERROR\n");
            return 0;
        }
        fileName[0] = '.';
        fileName[1] = '/';
        int i = 0;
        while(argv[optind][i] != '\0')
        {
            fileName[i + 2] = argv[optind][i];
            ++i;
        }
        isName = true;
    }
    if (!isName)
    {
        printf("ERROR");
        return 0;
    }
    char path[PATH_MAX];
    realpath(fileName,path);
    isName = true;
    struct stat s;
    stat(path, &s);
    mode_t permissions = s.st_mode;
    switch (client)
    {
        case user:
            for (int i = 0; i < attributesCount; ++i)
            {
                switch (attributes[i])
                {

                    case read:
                        if (function == add)
                            permissions |= S_IRUSR;
                        else
                            permissions -= permissions & S_IRUSR;
                        break;
                    case write:
                        if (function == add)
                            permissions |= S_IWUSR;
                        else
                            permissions -= permissions & S_IWUSR;
                        break;
                    case execute:
                        if (function == add)
                            permissions |= S_IXUSR;
                        else
                            permissions -= permissions & S_IXUSR;
                        break;
                    case none:
                        printf("ERROR\n");
                        return 0;
                }
            }
            break;
        case group:
            for (int i = 0; i < attributesCount; ++i)
            {
                switch (attributes[i])
                {

                    case read:
                        if (function == add)
                            permissions |= S_IRGRP;
                        else
                            permissions -= permissions & S_IRGRP;
                        break;
                    case write:
                        if (function == add)
                            permissions |= S_IWGRP;
                        else
                            permissions -= permissions & S_IWGRP;
                        break;
                    case execute:
                        if (function == add)
                            permissions |= S_IXGRP;
                        else
                            permissions -= permissions & S_IXGRP;
                        break;
                    case none:
                        printf("ERROR\n");
                        return 0;
                }
            }
            break;
        case other:
            for (int i = 0; i < attributesCount; ++i)
            {
                switch (attributes[i])
                {

                    case read:
                        if (function == add)
                            permissions |= S_IROTH;
                        else
                            permissions -= permissions & S_IROTH;
                        break;
                    case write:
                        if (function == add)
                            permissions |= S_IWOTH;
                        else
                            permissions -= permissions & S_IWOTH;
                        break;
                    case execute:
                        if (function == add)
                            permissions |= S_IXOTH;
                        else
                            permissions -= permissions & S_IXOTH;
                        break;
                    case none:
                        printf("ERROR\n");
                        return 0;
                }
            }
            break;
    }
    if (chmod(path, permissions) < 0)
    {
        printf("%s\n", strerror(errno));
        return 0;
    }
    return 0;
}
