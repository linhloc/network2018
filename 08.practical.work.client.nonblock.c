#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>

char* resolve_hostname_IP(struct hostent *h){
    if (h == NULL){
        printf("Unknown host\n");
        return 0;
    }
    return inet_ntoa(*((struct in_addr *)h->h_addr_list[0]));
}

int main(int argc, char *argv[]){
    struct sockaddr_in saddr;
    struct hostent *h;
    int sockfd;
    char myhostname[100];
    char *ip;
    char buff[1000], buff_send[1000], buff_recv[1000];
    
    if(argc > 1){
        //gets server hostname from program arguments
        strncpy(myhostname, argv[1], 100);
    }
    else{
        //asks hostname from STDIN
        printf("Enter hostname: ");
        scanf("%s", &myhostname[0]);
    }
    h = gethostbyname(myhostname);
    
    //resolves its IP address, print to STDOUT
    ip = resolve_hostname_IP(h);
    printf("The IP address is: %s\n", ip);
    
    //connects to that server, TCP port 8784 [It’s USTH]
    unsigned short port = 8784;
    
    //socket
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Error creating socket\n");
    }
    
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    saddr.sin_port = htons(port);
    
    //connect
    if(connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0){
        printf("Cannot connect\n");
        return 0;
    }else{
        //prints a message if it connects to server successfully
        printf("Connects to server successfully\n");
    }
    
    //reuse address
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    
    //nonblocking
    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);
    
    while(1) {
        memset(&buff_send, 0, sizeof(buff_send));
        //scanf() from STDIN;
        printf("Client> "); scanf("%s", buff_send);
        //send() to server;
        //write(sockfd, buff_send, strlen(buff_send));
        send(sockfd, buff_send, sizeof(buff_send), 0);
        
        //recv() from server;
        memset(&buff_recv, 0, sizeof(buff_recv));
        //        if(recv(sockfd, buff_recv, sizeof(buff_recv), 0) < 0){
        //                printf("recv failed");
        //                break;
        //        }
        //printf() to STDOUT;
        recv(sockfd, buff_recv, sizeof(buff_recv), 0);
        printf("Server> %s\n", buff_recv);
    }
    return 0;
}
