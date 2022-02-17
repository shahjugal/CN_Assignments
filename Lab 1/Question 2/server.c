/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<dirent.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
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
     int sockfd, newsockfd, portno, fd;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");

    WriteToSocket(&newsockfd, "Hello!\n");

    // Fetched File Name
    ReadFromSocketToBuffer(buffer, &newsockfd);
    //  printf("%s\n",buffer);

    fd = open(buffer, O_RDONLY);
    char buf[BUFSIZ];

        int num = read(fd, buf, BUFSIZ);

    if (num == -1) {
        perror("Read error");
        exit(1);
    }
    else {
        while (num != 0) {
            if (write(newsockfd, buf, num) == -1) {
                perror("Error");
            }
            num = read(fd, buf, BUFSIZ);
        }
    }

     close(newsockfd);
     close(sockfd);
     close(fd);
     return 0; 
}