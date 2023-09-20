#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORTA 5000

//nome ip porta
int main(int argc, char **argv){
    int sockfd , n ;
    struct sockaddr_in remote_addr,local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char mandamsg[1000];
    char ricevi[1000];
    char comando[100];

    if(argc != 3){
        printf("passare ip e porta server");
        exit(-1);
    }



    if(fork()!=0){
        if((sockfd= socket(AF_INET, SOCK_DGRAM, 0))<0){
            printf("errore nella socket\n");
            exit(-1);
        }


        memset(&remote_addr, 0, len);
        remote_addr.sin_family= AF_INET;
        remote_addr.sin_port= htons(atoi(argv[2]));
        inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);
    

        sendto(sockfd, "AUT" , 999 ,0,  (struct sockaddr*)&remote_addr, len);

        n= recvfrom(sockfd, ricevi, 999, 0, (struct sockaddr*)&remote_addr, &len);
        ricevi[n]=0;
        
        if(strcmp(ricevi,"NOAUT")==0){
            printf("nome dell'utente\n");
            fgets(mandamsg, 999 , stdin);
            strcpy(comando,"REG ");
            strcat(comando,mandamsg);
            sendto(sockfd, comando , 99 ,0,  (struct sockaddr*)&remote_addr, len);
        }

        for(;;){
            printf("-------\n");
            memset(mandamsg,0, 999);
            fgets(mandamsg, 999 ,stdin);
            sendto(sockfd, mandamsg , 999 ,0,  (struct sockaddr*)&remote_addr, len);

            if(strncmp(mandamsg,"UTENTI",6)==0){
                memset(ricevi,0,999);
                recvfrom(sockfd,ricevi,999,0,(struct sockaddr*)&remote_addr, &len);
                printf("%s\n", ricevi);
            }

            if(strncmp(mandamsg,"MANDA",5)==0){
                n=recvfrom(sockfd,ricevi,999,0,(struct sockaddr*)&remote_addr, &len);
                printf("%s\n", ricevi);
                strcpy(mandamsg,"");
                while(strncmp(mandamsg,"STOP",4)!=0){
                    fgets(mandamsg,999,stdin);
                    sendto(sockfd, mandamsg , 999 ,0,  (struct sockaddr*)&remote_addr, len);
                }
            }
        }
    }else{

        if((sockfd= socket(AF_INET, SOCK_DGRAM, 0))<0){
            printf("errore nella socket\n");
            exit(-1);
        }

        memset(&local_addr, 0, len);
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(PORTA);

        if(bind(sockfd, (struct sockaddr *)&local_addr, len) < 0){
            printf("Errore nella bind \n");
            return -1;
        }

        for(;;){
            puts("IN ATTESA\n");
            n = recvfrom(sockfd, ricevi, 999, 0, 
                    (struct sockaddr *)&remote_addr, &len);
            ricevi[n] = 0;
            printf("Messaggio %s, IP %s, Port %d",
                        ricevi, 
                        inet_ntoa(remote_addr.sin_addr),
                        ntohs(remote_addr.sin_port));
        }
        close(sockfd);
        return 0;
    }
}