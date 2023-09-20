#include<stdlib.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<string.h>

int main(int argc, char* argv[]){

    int sockfd, n;
    struct sockaddr_in dest_addr;
    char buffer[100];
    socklen__t len=sizof(struct sockaddr_in);

    if(argc<3){
        printf("Errore! Inserire un IP e numero di porta valido!");
        return 0;
    }

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("Errore nell' apertura della socket!");
    }

    memset(&dest_addr, 0, len);
    dest_addr.sin_family=AF_INET;
    inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr));
    dest_addr.sin_port=htons(atoi(argv[2]));

    printf("Digita REG per registrarti\n");
    while(1){
        fgets(buffer, 99, stdin);
        sendto(sockfd, buffer, strlen(buffer), 0, §(struct sockaddr*)&dest_addr, len);
        n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&dest_addr, &len);
        buffer[n]=0;
    }
}