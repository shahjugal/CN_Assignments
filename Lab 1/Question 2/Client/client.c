#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

//  by defrault use localhost
const char* Address = "localhost";

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void ReadFromSocketToBuffer(char buffer[], int* newsockfd){

     bzero(buffer,256);
     int n = read(*newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");

}

void WriteToSocket(int* newsockfd, const void* message){
     int n = write(*newsockfd,message,sizeof(message));
     if (n < 0) error("ERROR writing to socket");
}


int main(int argc, char *argv[])
{
    int sockfd, portno, n, fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 2) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(Address);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    bzero(buffer,256);

    ReadFromSocketToBuffer(buffer, &sockfd);
    printf("%s\n",buffer);

    printf("You may enter the filename: ");

    char filename[255];

    fgets(filename,255,stdin);
    filename[strcspn(filename, "\n")] = '\0';

    if (strcmp(filename, "EXIT") == 0)
    {
        WriteToSocket(&sockfd, "Q");
        close(sockfd);
        close(fd);
        exit(0);
    }
    
    WriteToSocket(&sockfd, filename);

    char buf[BUFSIZ] = { 0 };

    int num = read(sockfd, buf, BUFSIZ);

    if (strcmp(buf, "N") != 0)
    {

    fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (num == -1) {
        perror("Read error");
        exit(1);
    }
    else {
        while (num != 0) {
            printf("\nDownloading...");
            if (write(fd, buf, num) == -1) {
                perror("Error");
            }
            num = read(sockfd, buf, BUFSIZ);
        }
        printf("\nDownloaded\n");

    }
    close(fd);
    }


    close(sockfd);
    return 0;
}