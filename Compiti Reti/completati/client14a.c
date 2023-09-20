#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>


int main(int argc , char ** argv){
    int sockfd,n;
    struct sockaddr_in remote_addr;
    socklen_t len= sizeof(struct sockaddr_in);
    char mandamsg[1000];
    char ricevimsg[1000];

    if(argc != 3){
        printf("passare ip+porta\n");
        exit(-1);
    }

    if((sockfd= socket(AF_INET ,SOCK_DGRAM ,0))<0){
        printf("errore socket\n");
        exit(-1);
    }

    memset(&remote_addr, 0 , len);
    remote_addr.sin_family = AF_INET;
    inet_pton(AF_INET , argv[1], &remote_addr.sin_addr);
    remote_addr.sin_port= htons(atoi(argv[2]));

    sendto(sockfd, "REG", 999 ,0 , (struct sockaddr*)&remote_addr, len);

    for(;;){
        fgets(mandamsg, 999 , stdin);
        sendto(sockfd, mandamsg, 999 , 0, (struct sockaddr*)&remote_addr, len);

        if(strncmp(mandamsg, "RISORSA" , 7 )==0){
            n=recvfrom(sockfd, ricevimsg, 999 , 0, (struct sockaddr*)&remote_addr, &len);
            ricevimsg[n]=0;
            printf("%s\n", ricevimsg);
        }

        if(strncmp(mandamsg, "VEDIRISORSA" , 11)==0){
            n=recvfrom(sockfd, ricevimsg, 999 , 0, (struct sockaddr*)&remote_addr, &len);
            ricevimsg[n]=0;
            printf("%s\n", ricevimsg);
        }

    }
}
