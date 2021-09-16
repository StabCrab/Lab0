#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
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
}
int myls(struct dirent * dir)
{
	char* path;
	path = calloc(sizeof(char)*(strlen(dir->d_name)) + 2, sizeof(char));
	path[0] = '.';
	path[1] = '/';
	strcpy(&(path[2]), dir->d_name);

	struct stat s;
	memset(&s, 0, sizeof(struct stat));
	stat(path, &s);
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

	printf(" %ld", s.st_nlink); // number of hardlinks

	struct passwd * usr; // need that to get user data 
	usr = getpwuid(s.st_uid); // find data using ID from stat
	printf(" %s", usr->pw_name);
	struct group * grp;
	grp = getgrgid(s.st_gid); // just like user, but group now
	printf(" %s", grp->gr_name);
	printf(" %5d", (int)s.st_size); //size of the file
	struct tm * time_ptr;
	time_t It = s.st_ctime;
	time_ptr = localtime(&It);
	printf(" %s", monthNumberToName(time_ptr->tm_mon+1));
	printf(" %02d", time_ptr->tm_mday);
	printf(" %02d:%02d ", time_ptr->tm_hour, time_ptr->tm_min);
	printf("%s", dir->d_name);
	printf("\n");
	
	return 0;
}
int main (int argc, char** argv)
{
	struct dirent *de;
	int c;
	bool isPath = false;
	bool isL = false;
	int counter = 1;
	int countToPath;
	while (counter < argc)
	{
		if ((c = getopt(argc, argv, "l")) != -1)   
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

	DIR *dir;
	if (isPath)
		dir = opendir(argv[countToPath]);
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
			stat(argv[countToPath], &s);
		else
        		stat(".", &s);

		printf("total %ld \n", s.st_blocks);

		while ((de = readdir(dir)) != NULL)
        	{
                	if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                        	continue;
                	else
                		myls(de);
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
