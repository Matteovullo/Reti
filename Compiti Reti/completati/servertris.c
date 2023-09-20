#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

int n_client=0;



char ** creaMatriceTris(){

    char **matrice = (char **)malloc(3*sizeof(char*));

    for(int i=0 ; i<3; i++){
        matrice[i]= (char*)malloc(3*sizeof(char));
        for(int j=0; j<3; j++){
            matrice[i][j] = '-';
        }
    }
    return matrice;
}

void printf_matrix_tris(char **matrice){
    for(int i=0; i<3;i++){
        for(int j=0;j<3;j++){
            printf("%c    ", matrice[i][j]);
        }
        printf("\n");
    }
}

typedef struct{
    char ip[20];
    int porta;
}Giocatore;


int occupa_posizione_tris(char *comando, char**matrix){
    char *giocatore= strtok(comando,":");
    char *posizione1= strtok(NULL,":");
    char *posizione2= strtok(NULL,"\n");
    int i= atoi(posizione1);
    int j= atoi(posizione2);
    int stato;

    if(strncmp(giocatore,"X",1)==0){
        matrix[i][j]='X';
        stato=0;
        for(int z=0; z<3 ; z++){ //controllo riga
            if(matrix[i][z]=='X') stato++;
        }
        if(stato==3) return 1;
        stato=0;
        for(int z=0; z<3 ; z++){ //controllo colonna

            if(matrix[z][i]=='X') stato++;
        }

        printf("STATO = %d\n", stato);
        if(stato==3) return 1;
    }else{    
        matrix[i][j]='0';
        stato=0;
        for(int z=0; z<3 ; z++){ //controllo riga
            if(matrix[i][z]=='0') stato++;;
        }
        if(stato==3) return 2;
        stato=0;
        for(int z=0; z<3 ; z++){ //controllo colonna
            if(matrix[z][i]=='0') stato++;;
        }
        if(stato==3) return 2;
    }

    return 0;
}





//nome eseguibile + porta ascolto
int main(int argc, char** argv){
    int sockfd, n ;
    struct sockaddr_in remote_addr, local_addr;
    int turno=1;
    char buffer[1000];
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevimsg[1000];
    char **tris_matrix = creaMatriceTris();
    printf_matrix_tris(tris_matrix);
    int contamosse=0;

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

    while (n_client != 2){
        printf("-- IN ATTESA DI DUE GIOCATORI --\n");
        n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
        ricevimsg[n]=0;

        if(strncmp(ricevimsg,"REG",3)==0  && n_client == 0){
            strcpy(uno.ip,inet_ntoa(remote_addr.sin_addr));
            uno.porta= ntohs(remote_addr.sin_port);
            n_client++;
        }else if(strncmp(ricevimsg,"REG",3)==0  && n_client == 1){
            strcpy(due.ip,inet_ntoa(remote_addr.sin_addr));
            due.porta= ntohs(remote_addr.sin_port);
            n_client++;
        }
    }

    printf("[+] INIZIO PARTITA \n");
    int stato;
    while(turno!=3){
        while(turno == 1){
            printf("TURNO GIOCATORE 1\n");
            remote_addr.sin_port= htons(uno.porta);
            inet_pton(AF_INET, uno.ip , &remote_addr.sin_addr);
            sendto(sockfd, "TURNO-indicare la posizione dove inserire X - (i:j)\n",999,0,(struct sockaddr*)&remote_addr,len);
            n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
            strcpy(buffer,"X:");
            strcat(buffer,ricevimsg);
            stato=occupa_posizione_tris(buffer,tris_matrix);
            turno=2;
            printf_matrix_tris(tris_matrix);
            if(stato == 1){
                sendto(sockfd, "HAI VINTO\n",999,0,(struct sockaddr*)&remote_addr,len);
                printf("\n HA VINTO IL PRIMO GIOCATORE \n");
                turno=3;

                remote_addr.sin_port= htons(due.porta);
                inet_pton(AF_INET, due.ip , &remote_addr.sin_addr);
                sendto(sockfd, "HAI PERSO\n",999,0,(struct sockaddr*)&remote_addr,len);
            }
            contamosse++;
            if(contamosse==9){
                sendto(sockfd, "PAREGGGIO\n",999,0,(struct sockaddr*)&remote_addr,len);
                return 0;
            }
        }
        while(turno == 2){
            printf("TURNO GIOCATORE 2\n");
            remote_addr.sin_port= htons(due.porta);
            inet_pton(AF_INET, due.ip , &remote_addr.sin_addr);
            sendto(sockfd, "TURNO-indicare la posizione (da 1 a 9) dove inserire 0 - (i:j)\n",999,0,(struct sockaddr*)&remote_addr,len);
            n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
            strcpy(buffer,"0:");
            strcat(buffer,ricevimsg);
            stato=occupa_posizione_tris(buffer,tris_matrix);
            turno=1;
            printf_matrix_tris(tris_matrix);
            if(stato == 2){
                sendto(sockfd, "HAI VINTO\n",999,0,(struct sockaddr*)&remote_addr,len);
                printf("\n HA VINTO IL PRIMO GIOCATORE \n");
                turno=3;

                remote_addr.sin_port= htons(uno.porta);
                inet_pton(AF_INET, uno.ip , &remote_addr.sin_addr);
                sendto(sockfd, "HAI PERSO\n",999,0,(struct sockaddr*)&remote_addr,len);

            }
            contamosse++;
            if(contamosse==9){
                sendto(sockfd, "PAREGGGIO\n",999,0,(struct sockaddr*)&remote_addr,len);
                return 0;
            }
        }
    }
    return 0;
}