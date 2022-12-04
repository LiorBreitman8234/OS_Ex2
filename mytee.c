#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
    if(argc == 1)
    {
        char * line = NULL;
        size_t lineLength  = 0;
        while(line != 3 || line != 4)
        {
            getline(&line, &lineLength, stdin);
            printf("%s",line);
            printf("line length = %ld\n",lineLength);
        }
        free(line);

    }
    else
    {
        int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC);
        char * line = NULL;
        size_t lineLength  = 0;
        while(line != 3 || line != 4)
        {
            getline(&line, &lineLength, stdin);
            printf("%s",line);
            size_t bytesWritten = write(fd,line,strlen(line));
        }
        free(line);
        close(fd);
    }
    return 0;
}