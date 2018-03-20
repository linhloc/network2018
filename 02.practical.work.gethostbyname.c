#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
    char myhostname[100];
    struct hostent *hstip;
    char *ip;
    if(argc > 1){
        strncpy(myhostname, argv[1], 100);
    }
    else{
        printf("Enter hostname: ");
        scanf("%s", &myhostname[0]);
    }
    
    hstip = gethostbyname(myhostname);
    ip = inet_ntoa(*((struct in_addr *)hstip->h_addr_list[0]));
    
    printf("The IP address of %s is: %s\n",myhostname, ip);

    return 0;
}
