#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<stdbool.h>

typedef struct {
    struct sockaddr_in client_addr;
}client;

int verifica(char** tris){
    int x=0;
    int o=0;

    for(int i=0; i<3; i++){
        x=0;
        o=0;
        for(int j=0; j<3; j++){
            if(tris[i][j]=='x') x++;
            else if(tris[i][j]=='o') o++;
        }
        if(x==3) return 1;
        else if(o==3) return 2;
    }

    for(int j=0; j<3; j++){
        x=0;
        o=0;
        for(int i=0; i<3; i++){
            if(tris[i][j]=='x') x++;
            else if(tris[i][j]=='o') o++;
        }
        if(x==3) return 1;
        else if(o==3) return 2;
    }

    return 0;
}

void inserisci(char** tris, int i, int j, char c){
    tris[i][j]=c;
}

void azzera(char** tris){
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            tris[i][j]='0';
        }
    }
}

bool occupato(char** tris, int i, int j){
    if(tris[i][j] == 'x' || tris[i][j] == 'o') return true;
    else return false;
}

int main(int argc, char* argv[]){
    int sockfd, n;
    struct sockaddr_in local_addr, dest_addr;
    client c1;
    client c2;
    int n_giocatori=0;
    socklen_t len=sizeof(struct sockaddr_in);
    char recive[100];

    char** tris=(char*)malloc(3*sizeof(*char));
    for(int i=0; i<3; i++){
        tris[i]=(char)malloc(3*sizeof(char));
        for(int j=0; j<3; j++){
            tris[i][j]='0';
        }
    } 

    if(argc<2){
        printf("Errrore inserire una porta\n");
        return 0;
    }

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("Errore nell'apertura della socket\n");
        return -1;
    }

    memset(&local_addr, 0, len);
    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));

    if(bind(sockfd, (struct sockaddr *)&local_addr, len)<0){
        printf("Errroe! inserire un numero di porta valido\n");
        return -1;
    }

    for(;;){
        printf("ATTESA DI CONNESSIONI");
                n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);

        if(strncmp(recive, "REG",3 )==0 && n_giocatore ==0){
            c1.client_addr.sin_family= AF_INET;
            inet_pton(AF_FAMILY , inet_ntoa(dest_addr) , & c1.client_addr.sin_addr);
            c1.client_addr.sin_port = dest_addr.sin_port;
            n_giocatori++;
        }else if(strncmp(recive, "REG",3 )==0 && n_giocatore ==1){
             c2.client_addr.sin_family= AF_INET;
            inet_pton(AF_FAMILY , inet_ntoa(dest_addr) , & c2.client_addr.sin_addr);
            c2.client_addr.sin_port = dest_addr.sin_port;
            n_giocatori++;
            break;
        }
    }


    int turno = 1;

    for(;;){
        if(turno == 1 ){
            sendto(sockfd, "Inserisci mossa!", strlen("Inserisci mossa!"), 0, (struct sockaddr*)&c1.client_addr, len);
            n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
            recive[n]=0;
            strcpy(c, recive);

            sendto(sockfd, "Inserisci riga!", strlen("Inserisci riga!"), 0, (struct sockaddr*)&c1.client_addr, len);
            n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
            recive[n]=0;
            i=atoi(recive);

            sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, (struct sockaddr*)&c1.client_addr, len);
            n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
            recive[n]=0;
            j=atoi(recive);

            if(occupato(tris, i, j)){
                sendto(sockfd, "Inserisci mossa!", strlen("Inserisci mossa!"), 0, (struct sockaddr*)&c1.client_addr, len);
                n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
                recive[n]=0;

                sendto(sockfd, "Inserisci riga!", strlen("Inserisci riga!"), 0, (struct sockaddr*)&c1.client_addr, len);
                n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
                recive[n]=0;

                sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, (struct sockaddr*)&c1.client_addr, len);
                n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
                recive[n]=0;

            }else{
                inserisci(tris, i, j, c);
            }
            turno=2;
        }

        if(turno == 2 ){
            sendto(sockfd, "Inserisci mossa!", strlen("Inserisci mossa!"), 0, (struct sockaddr*)&c2.client_addr, len);
            n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
            recive[n]=0;
            strcpy(c, recive);

            sendto(sockfd, "Inserisci riga!", strlen("Inserisci riga!"), 0, (struct sockaddr*)&c2.client_addr, len);
            n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
            recive[n]=0;
            i=atoi(recive);

            sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, (struct sockaddr*)&c2.client_addr, len);
            n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
            recive[n]=0;
            j=atoi(recive);

            if(occupato(tris, i, j)){
                sendto(sockfd, "Inserisci mossa!", strlen("Inserisci mossa!"), 0, (struct sockaddr*)&c2.client_addr, len);
                n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
                recive[n]=0;

                sendto(sockfd, "Inserisci riga!", strlen("Inserisci riga!"), 0, (struct sockaddr*)&c2.client_addr, len);
                n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
                recive[n]=0;

                sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, (struct sockaddr*)&c2.client_addr, len);
                n=recvfrom(sockfd, recive, 99, 0, (struct sockaddr*)&dest_addr, &len);
                recive[n]=0;
            }else{
                inserisci(tris, i, j, c);
            }
            turno=1;
        }

        if(verifica(tris) == 1){
            sendto(sockfd, "Hai vinto!", strlen("Hai vinto!"), 0, (struct sockaddr*)&c1.client_addr, len);
            sendto(sockfd, "Hai perso!", strlen("Hai perso!"), 0, (struct sockaddr*)&c2.client_addr, len);            
            break;
        }else if(verifica(tris) == 2){
            sendto(sockfd, "Hai vinto!", strlen("Hai vinto!"), 0, (struct sockaddr*)&c2.client_addr, len);
            sendto(sockfd, "Hai perso!", strlen("Hai perso!"), 0, (struct sockaddr*)&c1.client_addr, len);
            break;
        }
    }

    close(sockfd);
    return 0;
}