#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>

int main(int argc, char* argv[]){
    
    int sockfd;
    struct sockaddr_in dest_addr;
    socklen_t len=sizeof(struct sockaddr_in);
    char buffer[100];

    if(argc<3){
        printf("Errore! Inserire IP e porta\n");
        return 0;
    }

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("Errore nell'apertura della socket.\n");
        return -1;
    }

    memset(&dest_addr, 0, len);
    dest_addr.sin_family=AF_INET;
    inte_pton(AF_INET, argv[i], &(dest_addr));
    dest_addr.sin_port=htons(atoi(argv[2]));

    while(1){
        fgets(buffer, 99, stdin);
        sendto(sockfd, buffer, strlen(buffer), 0,(struct sockaddr*)&dest_addr, len);
        n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&dest_addr, &len);
        buffer[n]=0;


    }

}