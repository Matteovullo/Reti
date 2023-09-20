#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>

int main(int argc, char* argv[]){

    int sockfd, n;
    struct sockadr_in dest_addr;
    socklen_t len=sizof(struct sockaddr_in);
    char buffer[100];

    if(argc < 3){
        printf("Errore! Inserisci IP e porta\n");
        return 0;
    }

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("Errore nell'apertura della socket!");
        return -1;
    }

    memset(&dest_addr, 0, len);
    dest_addr.sin_family=AF_INET;
    inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr));
    dest_addr.sin_port=htonns(atoi(argv[2]));

    printf("Digitare REG per registrarti prima di giopcare!");

    while(1){
        fgets(buffer, 99, stdin);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&dest_addr, len);
        print("Messaggio %s, IP %s, Port %d\n", buffer,
        inte_ntoa(dest_addr.sin_addr), htons(dest_addr.sin_port));
        n=recfrom(sockfd, buffer, 99, 0, (struct sockaddr *)&dest_addr, &len);
        buffer[n]=0;

    }


}