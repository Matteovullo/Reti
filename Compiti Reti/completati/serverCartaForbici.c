#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

typedef struct{
    int vita;
    struct sockaddr_in client;
    char mossa[20];
    int vittoria;
}Giocatore;


void mossa_vincitore(Giocatore *uno , Giocatore *due){
    printf("GIOCATORE UNO MOSSA:%s  GIOCATORE DUE MOSSA:%s\n",uno->mossa, due->mossa);

    if(strcmp(uno->mossa,"FORBICE")==0 && strcmp(due->mossa,"CARTA")==0){
        uno->vittoria=1;
        return;
    }
    if(strcmp(uno->mossa,"FORBICE")==0 && strcmp(due->mossa,"SASSO")==0){
        due->vittoria=1;
        return;
    }
    if(strcmp(uno->mossa,"CARTA")==0 && strcmp(due->mossa,"SASSO")==0){
        uno->vittoria=1;
        return;
    }
    if(strcmp(uno->mossa,"CARTA")==0 && strcmp(due->mossa,"FORBICE")==0){
        due->vittoria=1;
        return;
    }

    if(strcmp(uno->mossa,"SASSO")==0 && strcmp(due->mossa,"CARTA")==0){
        due->vittoria=1;
        return;
    }
    if(strcmp(uno->mossa,"SASSO")==0 && strcmp(due->mossa,"FORBICE")==0){
        uno->vittoria=1;
        return;
    }
}


void partita(Giocatore uno, Giocatore due, int sockfd, socklen_t len){
    int n;
    char ricevimsg[1000];
    sendto(sockfd,"[+]INIZIO PARTITA",10, 0, (struct sockaddr*)&uno.client, len);
    sendto(sockfd,"[+]INIZIO PARTITA",10, 0, (struct sockaddr*)&due.client, len);

    printf("[+]INIZIO PARTITA\n");
    int turno=0;
    char *mossa;
    int n_giocatore_vincitore;
    int contamosse=0;

    while(uno.vita != 0 && due.vita != 0){
        if(turno == 0){
            sendto(sockfd,"TURNO - CARTA FORBICE SASSO",40, 0, (struct sockaddr*)&uno.client, len);
            n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&uno.client, &len);
            mossa = strtok(ricevimsg,"\n");
            strcpy(uno.mossa, mossa);
            turno=1;
            contamosse++;
        }else{
            sendto(sockfd,"TURNO - CARTA=C FORBICE=F SASSO=S",40, 0, (struct sockaddr*)&due.client, len);
            n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&due.client, &len);
            mossa = strtok(ricevimsg,"\n");
            strcpy(due.mossa, mossa);
            turno=0;
            contamosse++;
        }
        if(contamosse % 2 == 0 ){
            if(strcmp(uno.mossa, due.mossa)==0){
                sendto(sockfd,"[+] PAREGGIO",20, 0, (struct sockaddr*)&uno.client, len);
                sendto(sockfd,"[+] PAREGGIO",20, 0, (struct sockaddr*)&due.client, len);
            }else{
                mossa_vincitore(&uno,&due);
                if(uno.vittoria == 1){
                    sendto(sockfd,"VINTO",20, 0, (struct sockaddr*)&uno.client, len);
                    sendto(sockfd,"PERSO",20, 0, (struct sockaddr*)&due.client, len);
                    due.vita= due.vita-1;
                }else if(due.vittoria == 1){
                    sendto(sockfd,"PERSO",20, 0, (struct sockaddr*)&uno.client, len);
                    sendto(sockfd,"VINTO",20, 0, (struct sockaddr*)&due.client, len);
                    uno.vita= uno.vita-1;
                }
            }
            printf("VITA 1:%d VITA 2=%d\n", uno.vita, due.vita);
            uno.vittoria=0;
            due.vittoria=0; 
        }   
    }
    printf("FINE PARTITA\n");
}   




void inizializza_giocatore(Giocatore *x , struct sockaddr_in remote_addr){
    int porta= ntohs(remote_addr.sin_port);
    x->client.sin_addr= remote_addr.sin_addr;
    x->client.sin_port= htons(porta);
    x->client.sin_family= AF_INET;
    x->vita=1;
}


//nome eseguibile + porta ascolto
int main(int argc, char** argv){
    int sockfd, n ;
    struct sockaddr_in remote_addr, local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevimsg[1000];
    int n_giocatori=0;
    Giocatore uno;
    Giocatore due;


    if(argc != 2){
        printf("erore negli argomenti : passare porta\n");
        exit(-1);
    }

    if((sockfd= socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("errore nella creazione socket\n");
        exit(-1);
    }

    memset(&local_addr, 0 , len);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port= htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr*)&local_addr, len))<0){
        printf("errore binding\n");
        exit(-1);
    }

    int porta;
    for(;;){
        while(n_giocatori!=2){
            n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
            ricevimsg[n]=0;
            if(strncmp(ricevimsg,"REG",3)==0 && n_giocatori == 0){
                inizializza_giocatore(&uno, remote_addr);
                n_giocatori++;
            }else if(strncmp(ricevimsg,"REG",3)==0 && n_giocatori == 1){
                inizializza_giocatore(&due, remote_addr);
                n_giocatori++;
            }
        }
        partita(uno,due,sockfd,len);
        n_giocatori=0;
    }

    
}