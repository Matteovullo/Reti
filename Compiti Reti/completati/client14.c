#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#define PORT 5000

int main(int argc, char **argv){
    int sockfd,n;
    struct sockaddr_in remote_addr,local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevi[1000];
    char manda[1000];

    if(argc!=3 ){
        printf("passare ip + porta d \n");
        exit(-1);
    }


    if (!fork()){    

        if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0)
        { 
            printf("\nError in socket opening ... exit!");
            return -1;
        }


        memset(&local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port=htons((PORT));
        if( bind(sockfd, (struct sockaddr *) &local_addr, len)<0)
        { 
            return -1;
        }

        for (;;){ 
            n = recvfrom(sockfd,ricevi,999,0,(struct sockaddr *) &remote_addr,&len);
            ricevi[n] = 0;
	        printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port), ricevi);
        }
        
    }else{
        if((sockfd= socket(AF_INET ,SOCK_DGRAM ,0))<0){
                printf("errore nella creazione della socket\n");
                exit(-1);
            }
        
        memset(&remote_addr, 0 ,len);
        remote_addr.sin_family= AF_INET;
        inet_pton(AF_INET, argv[1], &(remote_addr.sin_addr));
        remote_addr.sin_port = htons(atoi(argv[2]));

        for(;;){
            printf("COMANDI DA MANDARE: \n <DISPONIBILI>-<PRENOTA><nomerisorsa> <MOSTRARISORSE>\n");
            fgets(manda,999,stdin);
            sendto(sockfd, manda , 999 ,0 , (struct sockaddr*)&remote_addr, len);
            n = recvfrom(sockfd, ricevi , 999 ,0 , (struct sockaddr*)&remote_addr, &len);
            ricevi[n]=0;
            printf("%s\n", ricevi);
        }
    }
}