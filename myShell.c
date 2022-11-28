#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <fcntl.h>

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

void copy(char * file1, char * file2)
{
    // clear second file
    truncate(file2,0);
    
    int first = open(file1,O_RDONLY);
    if(first == -1)
    {
        perror("error opening first file: ");
        return;
    }
    int second = open(file2,O_WRONLY|O_CREAT, S_IRWXU);
    if(second == -1)
    {
        perror("error opening second file: ");
        close(first);
        return;
    }
    char * data = (char*)malloc(128);
    if(!data)
    {
        close(first);
        close(second);
        perror("Cannot allocate space: ");
        return;
    }
    int bytesWritten,bytesRead = 0; 
    while(1)
    {
        //reading the data and checking validity
        bytesRead = read(first,data,128);
        if(bytesRead < 0)
        {
            close(first);
            close(second);
            free(data);
            perror("Error while reading: ");
            break;
        }
        else if(bytesRead == 0)
        {
            printf("Finished copying\n");
            break;
        }

        //writing the data
        bytesWritten = write(second,data,bytesRead);
        if(bytesWritten < 0)
        {
            close(first);
            close(second);
            free(data);
            perror("Error while writing: ");
            break;
        }
    }
    close(first);
    close(second);
    free(data);

    return;
}

int main(int argc, char * argv[])
{
    char * line = NULL;
    size_t lineLength  = 0;
    ssize_t lengthRead = 0;
    while(line != 3 || line != 4)
    {
        lengthRead = getline(&line, &lineLength, stdin);
        char* copyLine = (char*) malloc(lineLength);
        strcpy(copyLine, line);

        if(strcmp(line,"DIR\n") == 0)
        {
            myDir();
        }
        else if(strcmp("COPY",strtok(copyLine, " ")) == 0)
        {
            char* file1 = (char*) malloc(lineLength);
            char* temp = (char*) malloc(lineLength);
            char* file2 = (char*) malloc(lineLength);

            file1=strtok(NULL, " ");
            temp=strtok(NULL, " ");
            memcpy(file2, temp, strlen(temp)-1);
            copy(file1,file2);
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