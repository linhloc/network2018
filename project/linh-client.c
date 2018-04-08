#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#define BUFF_LEN 1000

char *getSeverHostname(int argc, char *argv[]){
    char *hostname;
    hostname = malloc (sizeof (char) * 100);
    
    if(argc > 1){
        //gets server hostname from program arguments
        strncpy(hostname, argv[1], 100);
    }
    else{
        //asks hostname from STDIN
        printf("Enter hostname: ");
        scanf("%s", hostname);
    }
    return hostname;
}

char* resolve_hostname_IP(struct hostent *h){
    if (h == NULL){
        printf("Unknown host\n");
        return 0;
    }
    return inet_ntoa(*((struct in_addr *)h->h_addr_list[0]));
}

int openClientfd(int sockfd, unsigned short port, struct sockaddr_in saddr, struct hostent *h){
    //create socket
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Error creating socket\n");
    }
    
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    saddr.sin_port = htons(port);
    
    //connect to server
    if(connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0){
        printf("Cannot connect\n");
        return 0;
    }else{
        //prints a message if it connects to server successfully
        printf("Connects to server successfully\n");
    }
    
    return sockfd;
}

int nonblockingClient_p1(int sockfd){
    //reuse address
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    //nonblocking
    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);
    return sockfd;
}

void handleInput(int sockfd){
    char buff[BUFF_LEN];
    while(1){
        memset(&buff, 0, sizeof(buff));
        printf("Client> ");
        fgets(buff,BUFF_LEN-1,stdin);
        send(sockfd, buff, strlen(buff), 0);
    }
}
void handleReceivingMessage(sockfd){
    char buff[BUFF_LEN];
    while(1){
        while (recv(sockfd, buff, sizeof(buff), 0) > 0)
        {
            printf("Server> %s\n", buff);
        }
        memset(&buff, 0, sizeof(buff));
    }
}


int main(int argc, char *argv[]){
    struct sockaddr_in saddr;
    struct hostent *h;
    int sockfd;
    char *ip;
    unsigned short port = 8784;
    
    if((h = gethostbyname(getSeverHostname(argc, argv))) == NULL) printf("Unknown host\n");
    
    //resolves its IP address, print to STDOUT
    ip = resolve_hostname_IP(h);
    printf("The IP address is: %s\n", ip);
    
    sockfd = openClientfd(sockfd, port, saddr, h);
    sockfd = nonblockingClient_p1(sockfd);
    
    //separate thread for input and thread networking
    pthread_t inputThread;
    pthread_create(&inputThread, NULL, handleInput, sockfd);
    handleReceivingMessage(sockfd);
    
    return 0;
}
