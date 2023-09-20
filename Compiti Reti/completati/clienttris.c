#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


//nome ip porta
int main(int argc, char **argv){
    int sockfd , n ;
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char mandamsg[1000];
    char ricevimsg[1000];


    if(argc != 3){
        printf("passare ip e porta server\n");
        exit(-1);
    }

    if((sockfd= socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("errore nella socket\n");
        exit(-1);
    }


    memset(&remote_addr, 0, len);
    remote_addr.sin_family= AF_INET;
    remote_addr.sin_port= htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);


    sendto(sockfd, "REG" , 999 ,0,  (struct sockaddr*)&remote_addr, len);

    for(;;){
        n= recvfrom(sockfd, ricevimsg, 999 ,0 ,(struct sockaddr*)&remote_addr, &len);
        printf("%s\n", ricevimsg);
        if(strncmp(ricevimsg,"HAI VINTO",9)==0){
            return 0;
        }
        if(strncmp(ricevimsg,"HAI PERSO",9)==0){
            return 0;
        }


        fgets(mandamsg, 999 ,stdin);
        sendto(sockfd, mandamsg , 999 ,0,  (struct sockaddr*)&remote_addr, len);

      
    }
}