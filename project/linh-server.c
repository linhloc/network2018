#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/select.h>
#define BUFF_LEN 1000
#define MAX_CLIENT 50

int openListenfd(int sockfd, unsigned short port, struct sockaddr_in saddr){
    //create socket
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
    
    if (listen(sockfd, 5) < 0) {
        printf("Error listening\n");
    }else{
        printf("Waiting for incoming connections...\n");
    }
    return sockfd;
}

int main(){
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    char buff[1000],buff_send[1000], buff_recv[1000];
    int clientfds[MAX_CLIENT];
    memset(clientfds, 0, sizeof(clientfds));
    
    sockfd = openListenfd(sockfd, port, saddr);
    //clen=sizeof(caddr);
    
    while(1){
        fd_set set;// declaration of the set
        FD_ZERO(&set);// clear the set
        FD_SET(sockfd, &set);// add listening sockfd to set
        int maxfd = sockfd; // a required value to pass to select()
        for (int i = 0; i < MAX_CLIENT; i++) {
            // add connected client sockets to set
            if (clientfds[i] > 0) FD_SET(clientfds[i], &set);
            if (clientfds[i] > maxfd) maxfd = clientfds[i];
        }
        
        // poll and wait, blocked indefinitely
        select(maxfd+1, &set, NULL, NULL, NULL);
        
        // a «listening» socket?
        
        if (FD_ISSET(sockfd, &set)) {
            clientfd = accept(sockfd, (struct sockaddr *) &saddr, (socklen_t *)&clen);
            // make it nonblocking
            int fl = fcntl(clientfd, F_GETFL, 0);
            fl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, fl);
            // add it to the clientfds array
            for (int i = 0; i < MAX_CLIENT; i++){
                if (clientfds[i] == 0) {
                    clientfds[i] = clientfd;
                    break;
                }
            }
        }
        
        // is that data from a previously-connect client?
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (clientfds[i] > 0 && FD_ISSET(clientfds[i], &set)) {
                memset(&buff, 0, sizeof(buff));
                if (read(clientfds[i], buff, sizeof(buff)) > 0) {
                    printf("\nClient %d> %s", clientfds[i], buff);
                    memset(&buff, 0, sizeof(buff));
                    
                    printf("\nServer> ");
                    fgets(buff, BUFF_LEN, stdin);
                    send(clientfds[i], buff, sizeof(buff),0);
                }
                else {
                    // some error. remove it from the "active" fd array
                    printf("client %d has disconnected.\n", clientfds[i]);
                    clientfds[i] = 0;
                }
            }
        }
    
     }
         
    return 0;
}

