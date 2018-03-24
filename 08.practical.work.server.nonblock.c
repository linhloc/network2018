#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

int main(){
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    char buff[1000],buff_send[1000], buff_recv[1000];
    
    //socket
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket\n");
    }
    
    //setsockopt: reuse address
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    
    //file control: nonblocking
    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);
    
    //binding
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("Error binding\n");
    }
    
    //listen
    if (listen(sockfd, 5) < 0) {
        printf("Error listening\n");
    }
    
    clen=sizeof(caddr);
    
    while(1){
        clientfd=accept(sockfd, (struct sockaddr*) &caddr, (socklen_t *)&clen);
        if(clientfd>0){
            //file control: nonblocking
            int fl = fcntl(sockfd, F_GETFL, 0);
            fl |= O_NONBLOCK;
            fcntl(sockfd, F_SETFL, fl);
            
            while(1){
                //recv() from client;
                memset(&buff_recv, 0, sizeof(buff_recv));
                if(recv(clientfd, buff_recv, sizeof(buff_recv), 0)>0){
                    //printf() to STDOUT;
                    printf("Client> %s\n", buff_recv);
                }
                //scanf() from STDIN;
                printf("Server> "); scanf("%s", buff_send);
                //send() to client;
                send(clientfd, buff_send, sizeof(buff_send),0);
                memset(&buff_send, 0, sizeof(buff_send));
            }
        }
    }
    //shutdown(sockfd, SHUT_RDWR);
    return 0;
}

