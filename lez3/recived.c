#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<arpa/inet.h>

int main(int argc, char **argv){

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in mittente_addr;
    char buffer[100];
    int len = sizeof(mittente_addr);

    if(sockfd<0){
        printf("Errore nell' apertura di socket\n");
    }

    bind(sockfd, mittente_addr, len);

    recvfrom();
    close(sockfd);

    return 0;
}