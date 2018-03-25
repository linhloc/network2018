#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/select.h>
#include<pthread.h>

#define MAX_CLIENT 100

int clientfds[MAX_CLIENT]; // list of connected clients, >0 if valid
char buff[1000],buff_send[1000], buff_recv[1000];

void input_handle(int *clientfds){
    printf("\nserver> ");
    scanf("%s", buff);
    for (int i = 0; i < MAX_CLIENT; i++) {
        if(clientfds[i] > 0)
            send(clientfds[i], buff, sizeof(buff),0);
    }
    memset(&buff, 0, sizeof(buff));
}

int main(){
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    pthread_t input_thread;
    pthread_t main_thread;

    memset(clientfds, 0, sizeof(clientfds)); // clear the list
    
    
    
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
    
    //Bind
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
    printf("Waiting for incoming connections...");
    clen=sizeof(caddr);
    
    pthread_create( &input_thread , NULL ,  input_handle , clientfds);
    
    while(1){
        fd_set set;// declaration of the set
        FD_ZERO(&set);// clear the set
        FD_SET(sockfd, &set);// add listening sockfd to set
        int maxfd = sockfd; // a required value to pass to select()
        
        for (int i = 0; i < 100; i++) {
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
            fl = fcntl(clientfd, F_GETFL, 0);
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
                if (read(clientfds[i], buff, sizeof(buff)) > 0) {
                    printf("client %d says: %s\nserver>", clientfds[i], buff);
                    memset(&buff, 0, sizeof(buff));
                    
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

