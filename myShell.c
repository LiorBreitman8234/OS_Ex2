#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>


int count_malloc = 0;
int count_free = 0;
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

int countWords(char* command){
    int counter = 1;
    for(int i =0; i < strlen(command);i++)
    {
        if(command[i] == ' ')
        {
            counter++;
        }
    }
    return counter;
}
char** parseCommand(char* command){
    command[strlen(command)-1] == '\n'? command[strlen(command)-1] = '\0' : command[strlen(command)-1];
    int word_counter = countWords(command);
    char** parts = (char**)malloc(sizeof(char*)*(word_counter+1));
    parts[word_counter] = NULL;
    for(int i =0 ;i < word_counter;i++)
    {
        parts[i] = (char*)malloc(strlen(command));
    }
    char* token = strtok(command," ");
    int i = 0;
    while(token!= NULL)
    {
        strcpy(parts[i++],token);
        token = strtok(NULL," ");
    }
    return parts;
}

void execCommand(char** args)
{
    char path[1000] = "/bin/";
    strcat(path,args[0]);
    strcpy(args[0],path);
    int childpid = -1;
    int fork_id = fork();
    if(fork_id == -1)
    {
        perror("error when creating thread");
        exit(0);
    }
    if(fork_id == 0)
    {
        childpid = getpid();
        execv(path,args);
    }
    else{
        waitpid(childpid,NULL,0);
    }
}

void pipeFunc(char* copyLine){

    char* first=strtok(copyLine, "|");
    char* second=strtok(NULL,"|");
    char** parsed_first = parseCommand(first);
    char** parsed_second = parseCommand(second);

    char path_first[1000] = "/bin/";
    char path_second[1000] = "/bin/";
    strcat(path_first,parsed_first[0]);
    strcpy(parsed_first[0],path_first);
    strcat(path_second,parsed_second[0]);
    strcpy(parsed_second[0],path_second);


    int pfds[2];
    pipe(pfds);
    int childpid = -1;
    int fork_id = fork();
    if(fork_id == -1)
    {
        perror("error when creating thread");
        exit(1);
    }
    if(fork_id == 0)
    {
        childpid = getpid();
        close(pfds[0]);            
        dup2(pfds[1], 1);
        close(pfds[1]); 
        execv(path_first,parsed_first);
        exit(1);
    }
    else{
        waitpid(childpid,NULL,0);
        fork_id = fork();
        if(fork_id == -1)
        {
            perror("error when creating thread");
            exit(1);
        }
        if(fork_id == 0)
        {
            childpid = getpid();
            close(pfds[1]);             
            dup2(pfds[0], 0);
            close(pfds[0]); 
            execv(path_second,parsed_second);
            return;
        }
        else{
            close(pfds[0]);
            close(pfds[1]);
            waitpid(childpid,NULL,0);
        }
    }
}


int main(int argc, char * argv[])
{
    char * line = NULL;
    size_t lineLength  = 0;
    ssize_t lengthRead = 0;
    int childpid = -1;
    while(line != 3 || line != 4)
    {
        lengthRead = getline(&line, &lineLength, stdin);
        int amountWords = countWords(line);
        char* copyLine = (char*)malloc(lineLength);
        strcpy(copyLine,line);
        char** parsed = parseCommand(line);
        if(strcmp(parsed[0],"exit") == 0)
        {
            return 0;
        }
        else if(strchr(copyLine,'|') != NULL)
        {
            pipeFunc(copyLine);
        }
        else if(strcmp(parsed[0],"{") == 0)
        {
            int port = atoi(parsed[1]);
            int sock;
            int acceptedSocket;
            struct sockaddr_in server;
            size_t len;
            sock = socket(AF_INET,SOCK_STREAM,0);
            if(sock < 0)
            {
                perror("error opening socket");
                close(sock);
                exit(1);
            }
            bzero(&server, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_port = htons(port);

            if(bind(sock, (struct sockaddr *) &server, sizeof(server))<0)
            {
                perror("error binding");
                exit(1);
            }

            if(listen(sock,1) <0){
                perror("error listening");
                exit(1);
            }


            len = sizeof(server);
            acceptedSocket = accept(sock, (struct sockaddr *) &server,&len);
            if(acceptedSocket < 0)
            {
                perror("error connecting");
                exit(1);
            }
            
            char buffer[256];
            while(read(acceptedSocket,&buffer,255) != 0)
            {
                printf("%s",buffer);
            }
            close(sock);

        }
        else if(strchr(copyLine,'}') != NULL)
        {
            char** firstPart = (char**)malloc(sizeof(char*)*amountWords);
            char** secondPart = (char**)malloc(sizeof(char*)*amountWords);
            int lengthFirst = 0;
            int lengthSecond = 0;
            int i =0; 
            while(i < amountWords)
            {
                if(strcmp(parsed[i],"}") == 0)
                {
                    break;
                }
                else
                {
                    firstPart[i] = (char*)malloc(lineLength);
                    strcpy(firstPart[i],parsed[i]);
                }
                i++;
            }
            lengthFirst = i;
            i++;
            int j = 0;
            while(i < amountWords)
            {
                secondPart[j] = (char*)malloc(lineLength);
                strcpy(secondPart[j],parsed[i]);
                i++;
                j++;
            }
            lengthSecond = j;


            //opening socket
            int client_sock;
            struct sockaddr_in addr;
            char* IP = (char*)malloc(16);
            char* port = (char*)malloc(6);
            char* parsingAddress = strtok(secondPart[0],":");
            int index = 0;
            while(parsingAddress != NULL)
            {
                if(!index)
                {
                    IP = parsingAddress;
                    parsingAddress= strtok(NULL,":");
                    index++;
                }
                else{
                    port = parsingAddress;
                    parsingAddress = strtok(NULL,":");
                }
            }
            client_sock = socket(AF_INET, SOCK_STREAM,0);
            if(client_sock < 0)
            {
                perror("error creating socket");
                exit(0);
            }
            printf("IP: %s , port: %s\n",IP,port);
            addr.sin_family = AF_INET;
            addr.sin_port = htons(atoi(port));
            

            if(inet_pton(AF_INET,IP,&addr.sin_addr) <=0)
            {
                perror("error with IP");
                return 0;
            }
            int saved_stdout = dup(1);
            if(connect(client_sock,(struct sockaddr*)&addr,sizeof(addr)) < 0 )
            {
                perror("error connecting");
                exit(0);
            }
            //free(IP);
            //free(port);
            dup2(client_sock,1);
            execCommand(firstPart);
            dup2(saved_stdout,1);
            close(saved_stdout);
            close(client_sock);
            printf("finished passing command\n");
        }
        else if(strcmp(parsed[0],"DIR") == 0)
        {
            myDir();
        }
        else if(strcmp("COPY",parsed[0]) == 0)
        {
            copy(parsed[1],parsed[2]);
        }
        else
        {
            execCommand(parsed);
        }
        for(int i = 0; i < amountWords+1;i++)
        {
            free(parsed[i]);
        }
        free(parsed);
    }
    return 0;
}