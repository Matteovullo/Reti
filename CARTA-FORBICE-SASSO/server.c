#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdio.h>

typedef struct{
    struct sockaddr_in client_addr;
    int vite;
    
}client;

typedef struct{
    char* mossa1;
    char* mossa2;
}partita;

int vincitore(char* mossa1, char* mossa2){
    if((!strcmp(mossa1, "carta") && !strcmp(mossa2, "sasso")) || 
    (!strcmp(mossa1, "forbice") && !strcmp(mossa2, "carta")) ||
    (!strcmp(mossa1, "sasso") && !strcmp(mossa2, "forbice")) ||) return 1;

    if((!strcmp(mossa1, "sasso") && !strcmp(mossa2, "carta")) || 
    (!strcmp(mossa1, "carta") && !strcmp(mossa2, "forbice")) ||
    (!strcmp(mossa1, "forbice") && !strcmp(mossa2, "sasso")) ||) return 2;

    return 0;
}

int main(int argc, char* argv[]){

    int sockfd, n;
    struct sockaddr_in local_addr, dest_addr;
    socklen_t len=sizeof(struct sockaddr_in);
    char send[100];
    char recive[100];
    client c1, c2;
    partita p;
    int n_giocatori=0;

    c1.vite=3;
    c2.vite=3;

    if(argc < 2){
        printf("Errroe! Inserire la porta!");
        return 0;
    }

    if((sockfd=socket(AF_INET, SOCKDGRAM, 0))<0){
        printf("Errore nell' apertura di socket\n");
        return -1;
    }

    memset(&local_addr, 0, len);
    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr*)&local_addr, len))<0){
        printf("Erroe! Inserisci un numero di porta valido\n");
        return -1;
    }

    while(1){
        printf("ATTESA CONNESSIONI!\n");
        n=recfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);

        if(strcmp(recive, "REG", 3) == 0 && n_giocatore==0){
            c1.client_addr.sin_family=AF_INET;
            inet_pton(AF_FAMILY, inet_ntoa(&dest_addr), &c1.client_addr.sin_addr);
            c1.client_addr.sin_port=dest_addr.sin_port;
            n_giocatori++;
        }else if(strcmp(recive, "REG", 3)==0 && n_giocatore==1){
            c2.client_addr.sin_family=AF_INET;
            inet_pton(AF_FAMILY, inet_ntoa(&dest_addr) &c2.client_addr.sin_addr);
            c2.client_addr.sin_port=dest_addr.sin_port;
            n_giocatori++;
            break;
        }
    }

    int turno=1;

    while(1){
        if(turno==1){
            sendto(sockfd, "Inserire la mossa!", strlen("Inserire la mossa"), 0, (struct sockaddr*)&c1.client_addr, len);
            n=recfrom(sockfd, recive, 99, 0, (struct sockaddr*)&c1.client_addr, &len);
            recive[n]=0;
            strcpy(p.mossa1, recive);
            turno=2;
        }

        if(turno==2){
            sendto(sockfd, "Inserire la mossa!", strlen("Inserire la mossa"), 0, (struct sockaddr*)&c2.client_addr, len);
            n=recfrom(sockfd, recive, 99, 0, (struct sockaddr*)&c2.client_addr, &len);
            recive[n]=0;
            strcpy(p.mossa1, recive);
            turno=1;
        }

        if(vincitore(p.mossa1, p.mossa2) == 1) c2.vite--;
        if(vincitore(p.mossa1, p.mossa2) == 2) c1.vite--;

        if(c1.vite==0){
            sendto(sockfd, "Partita vinta da client 2", strlen("Partita vinta da client 2"), 0, (struct sockaddr*)&c1.client_addr, len);
            sendto(sockfd, "Partita vinta da client 2", strlen("Partita vinta da client 2"), 0, (struct sockaddr*)&c2.client_addr, len);
            break;
        }

        if(c2.vite==0){
            sendto(sockfd, "Partita vinta da client 1", strlen("Partita vinta da client 1"), 0, (struct sockaddr*)&c1.client_addr, len);
            sendto(sockfd, "Partita vinta da client 1", strlen("Partita vinta da client 1"), 0, (struct sockaddr*)&c2.client_addr, len);
            break;
        }
    }
}