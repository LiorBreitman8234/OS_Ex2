#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    char * line = NULL;
    size_t lineLength  = 0;
    while(line != 3 || line != 4)
    {
        getline(&line, &lineLength, stdin);
        printf("%s",line);
    }
    return 0;
}