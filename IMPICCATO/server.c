#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdbool.h>

typedef struct {
    struct sockaddr_in client_addr;
    char* nome;
    int vite;
}client;

bool verifica_lettera(char* parola, char c){
    int i=0
    while(parola[i]!='\n'){
        if(parola[i]==c){
            return true;
        }
    }
    return false;
}

bool verifica_parola(char* parola, char* p){
    if(!strcmp(parola, p)) return true;
    else return false;
}

int main(int argc, char* argv[]){

    int sockfd, n;
    struct sockaddr_in dest_addr, local_addr;
    socklen_t len=sizeof(struct sockaddr_in);
    char buffer[100];
    client c1, c2;
    int n_giocatori=0;

    char dizionario[5]={"Miao", "si ah", "popito", "rtgtrg", "ciao"};

    c1.vite=5;
    c2.vite=5;

    if(argc<2){
        printf("Errore! Inserire la porta\n");
        return 0;
    }

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("Errore nell'apertura della socket\n");
        return -1;
    }

    memset(&local_addr, 0, len);
    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htoons(atoi(argv[1]));

    if(bind(sockfd, (struct sockaddr*)&local_addr, len)<0){
        printf("Errore! Inserisci un numero di porta valido\n");
        return -1;
    }

    while(1){
        if(n_giocatori==0){
            n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&dest_addr, &len);
            if(strcmp(buffer, "REG", 3)==0 && n_giocatori==0){
                c1.client_addr.sin_family=AF_INET;
                inet_pton(AF_FAMILY, inet_ntoa(&dest_addr), &c1.client_addr.sin_addr);
                c1.client_addr.sin_port=dest_addr.sin_port;
            }
            n_giocatori++;
        }else if(n_giocatori==1){
            if(strcmp(buffer, "REG", 3)==0 && n_giocatori==1){
                c2.client_addr.sin_family=AF_INET;
                inet_pton(AF_FAMILY, inet_ntoa(&dest_addr), &c2.client_addr.sin_addr);
                c2.client_addr.sin_port=dest_addr.sin_port;
            }
            n_giocatori++;
            break;
        }
    }

    int turno=1;

    int r=rand()%(5-0)+0;
    char* parola=dizionario[r];

    int count=0;

    while(1){
        if(turno==1){
            sendto(sockfd, "Inserire una carattere o una parola", strlen("Inserire una mossa o una parola"), 0, (struct sockaddr*)&c1.client_addr, len);
            n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&c1.client_addr, &len);
            buffer[n]=0;
            if(strlen(buffer)==1){
                if(verifica_lettera(dizionario[r])) count++;
                else c1.vite--;
            }else{
                if(verifica_parola(dizionario[r]));{
                    sendto(sockfd, "Parola indovinata!", strlen("Parola indovinata!"), 0, (struct sockaddr*)&c1.client_addr, len);
                    break;
                }
            }
            turno=2;
        }

        if(count==strlen(parola)){
            sendto(sockfd, "Parola indovinata!", strlen("Parola indovinata!"), 0, (struct sockaddr*)&c1.client_addr, len);
        }

        if(turno==2){
            sendto(sockfd, "Inserire una carattere o una parola", strlen("Inserire una mossa o una parola"), 0, (struct sockaddr*)&c2.client_addr, len);
            n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&c2.client_addr, &len);
            buffer[n]=0;
            if(strlen(buffer)==1){
                if(verifica_lettera(dizionario[r])) count++;
                else c1.vite--;
            }else{
                if(verifica_parola(dizionario[r]));{
                    sendto(sockfd, "Parola indovinata!", strlen("Parola indovinata!"), 0, (struct sockaddr*)&c2.client_addr, len);
                    break;
                }
            }
            turno=1;
        }

        if(count==strlen(parola)){
            sendto(sockfd, "Parola indovinata!", strlen("Parola indovinata!"), 0, (struct sockaddr*)&c2.client_addr, len);
        }
    }
}