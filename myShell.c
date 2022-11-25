#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>

int myDir()
{
    //https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
    //https://www.gnu.org/software/libc/manual/html_node/Accessing-Directories.html
    char cwd[256];
    if(getcwd(cwd ,sizeof(cwd)) != NULL)
    {
        DIR * dir;
        struct dirent * currfile;
        if((dir = opendir(cwd)) != NULL)
        {
            while((currfile = readdir(dir))!= NULL)
            {
                printf("%s ",currfile->d_name);
            }
            closedir(dir);
        }
        else
        {
            perror("opendir() error\n");
            return(errno);
        }
    }
    else
    {
        perror("cwd() error\n");
        return errno;
    }
    printf("\n");
    return 0;
}
int main(int argc, char * argv[])
{
    char * line = NULL;
    size_t lineLength  = 0;
    ssize_t lengthRead = 0;
    while(line != 3 || line != 4)
    {
        lengthRead = getline(&line, &lineLength, stdin);
        if(strcmp(line,"dir\n") == 0)
        {
            myDir();
        }
        else if(strcmp(line,"exit\n") == 0)
        {
            return 0;
        }
        else
        {
            printf("%s",line);
        }
    }
    return 0;
}