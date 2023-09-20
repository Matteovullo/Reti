#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 5000

//nome ip porta
int main(int argc, char **argv){
    int sockfd , n ;
    struct sockaddr_in remote_addr, local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char mandamsg[1000];
    char ricevimsg[1000];
    char *comando,*token;
    char buffer[1000];

    if(argc != 3){
        printf("passare ip e porta server\n");
        exit(-1);
    }

    if(fork()!=0){
        if((sockfd= socket(AF_INET, SOCK_DGRAM, 0))<0){
            printf("errore nella socket\n");
            exit(-1);
        }

        memset(&local_addr, 0, len);
        local_addr.sin_family= AF_INET;
        local_addr.sin_port= htons(PORT);

        if((bind(sockfd, (struct sockaddr*)&local_addr, len))<0){
            printf("errore nel binding\n");
            exit(-1);
        }

        for(;;){
            n= recvfrom(sockfd,ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
            ricevimsg[n]=0;
            printf("MSG=%s IP:%s\n", ricevimsg, inet_ntoa(remote_addr.sin_addr));
        }
        close(sockfd);
        return 0;
    }else{

        if((sockfd= socket(AF_INET, SOCK_DGRAM, 0))<0){
            printf("errore nella socket\n");
            exit(-1);
        }


        memset(&remote_addr, 0, len);
        remote_addr.sin_family= AF_INET;
        remote_addr.sin_port= htons(atoi(argv[2]));
        inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);

        for(;;){
            printf("[+]FASE DI REGISTRAZIONE:<REG> username\n");

            fgets(mandamsg, 999 ,stdin);
            if(strncmp(mandamsg,"REG",3)==0){
                sendto(sockfd, mandamsg , 999 ,0,  (struct sockaddr*)&remote_addr, len);
                n= recvfrom(sockfd,ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
                printf("%s\n",ricevimsg);
                if(strncmp(ricevimsg,"[-]",3)!=0){
                    token= strtok(ricevimsg,"\n");
                    break;
                }
            }
        }

    for(;;){
            printf("[+]FASE DI MESSAGGI: comunicazione\n");

            fgets(mandamsg, 999 ,stdin);
            if(strncmp(mandamsg,"MSG",3)==0){
                strcpy(buffer,"MSG ");
                strcat(buffer,token);
                strcat(buffer," ");
                strcat(buffer,mandamsg);
                sendto(sockfd, buffer , 999 ,0,  (struct sockaddr*)&remote_addr, len);
            }
        }
        close(sockfd);
        return 0;
    }
}

