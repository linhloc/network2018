#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

char* resolve_hostname_IP(struct hostent *h){
    if (h == NULL){
        printf("Unknown host\n");
    }
    return inet_ntoa(*((struct in_addr *)h->h_addr_list[0]));
}

int main(int argc, char *argv[]){
    struct sockaddr_in saddr;
    struct hostent *h;
    int sockfd;
    char myhostname[100];
    char *ip;
    
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
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Error creating socket\n");
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    saddr.sin_port = htons(port);

    if(connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0){
        printf("Cannot connect\n");
    }else{
        //prints a message if it connects to server successfully
        printf("Connects to server successfully\n");
    }
    return 0;
}
