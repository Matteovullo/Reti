#include<stdlib.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<string.h>

typedef struct{
    struct sockaddr_in client_addr;
}client;

void inserimento(char** matrix, int colonna, int mossa){

    for(int i=5; i<=0; i++){
        if(matrix[i][colonna]==' '){
            if(mossa==1) matrix[i][colonna]='x';
            else matrix[i][colonna] = 'o';
        }
    }
}

int vincitore(char** matrix){
    int countx=0;
    int counto=0;

    for(int j=0; j<7; j++){
        countx=0;
        counto=0;

        for(int i=0; i<6; i++){
            if(matrix[i][j]=='x') {
                countx++;
                counto=0;
            }
            else if(matrix[i][j]=='o') {
                counto++;
                countx=0;
            }

            if(countx == 4) return 1;
            else if(counto == 4) return 2;
        } 
    }

    for(int i=0; i<6; i++){
        countx=0;
        counto=0;

        for(int j=0; j<7; j++){
            if(matrix[i][j]=='x') {
                countx++;
                counto=0;
            }
            else if(matrix[i][j]=='o') {
                counto++;
                countx=0;
            }

            if(countx == 4) return 1;
            else if(counto == 4) return 2;
        } 
    }

    int k=0;

    for(int i=0; i<n-k; i++){
            for(int j=0; j<n-k-1; j++){
            if(matrix[i][j]=='x') {
                countx++;
                counto=0;
            }
            else if(matrix[i][j]=='o') {
                counto++;
                countx=0;
            }

            if(countx == 4) return 1;
            else if(counto == 4) return 2;
            }
            k++;
    }

    int l=0;

    for(int i=n-1; i>0; i--){
            for(int j=n-1; j>l; j--){
            if(matrix[i][j]=='x') {
                countx++;
                counto=0;
            }
            else if(matrix[i][j]=='o') {
                counto++;
                countx=0;
            }

            if(countx == 4) return 1;
            else if(counto == 4) return 2;
            }
            l++;
    }
}

void stampa(char** matrix){

    for(int i=0; i<6; i++){
        for(int j=0; j<7; j++){ 
            printf("%s", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]){

    int sockfd, n;
    struct sockaddr_in dest_addr, local_addr;
    char buffer[100];
    socklen__t len=sizof(struct sockaddr_in);

    client c1, c2;
    int n_giocatori=0;
    int turno=0;

    if(argc<2){
        printf("Errore! Inserire la porta\n");
        return 0;
    }

    if((sockfd=socket(AF_INET, SOCK_DGRAM, 0))<0){
        printf("Errore nell' apertura della socket!");
        return -1;
    }

    memset(&local_addr, 0, len);
    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));

    if(bind(sockfd, (struct sockaddr*)&local_addr, len)<0){
        printf("Erroe nella bind\n");
        return -1;
    }

    char** matrix=(char*)malloc(6*sizeof(*char));
    for(int i=0; i<6; i++){
        matrix[i]=(char)malloc(7*sizeof(char));
        for(int j=0; j<7; j++){
            matrix[i][j]=' ';
        }
    }

    while(1){
        printf("ATTESA DEI GIOCATORI!\n");
        n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&dest_addr, &len);

        if(strcmp(buffer, "REG", 3) == 0 && n_giocatori==0){
            c1.client_addr.sin_family=AF_INET;
            inet_pton(AF_INET, inet_ntoa(&dest_addr), &c1.client_addr.sin_addr);
            c1.client_addr.sin_port=dest_addr.sin_port;
            n_giocatori++;
        }else if(strcmp(buffer, "REG", 3) == 0 && n_giocatori==1){
            c2.client_addr.sin_family=AF_INET;
            inet_pton(AF_INET, inet_ntoa(&dest_addr), &c2.client_addr.sin_addr);
            c2.client_addr.sin_port=dest_addr.sin_port;
            n_giocatori++;
            break;
        }
    }

    turno=1;
        sendto(sockfd, buffer, strlen(buffer), 0, §(struct sockaddr*)c1.clientt_addr, len);
        n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&c1.client_addr, &len);
        buffer[n]=0;
    while(1){
        if(turno==1){
            sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, §(struct sockaddr*)&c1.client_addr, len);
            n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&c1.client_addr, &len);
            buffer[n]=0;

            while(atoi(buffer) < 1 && atoi(buffer) > 7){
                sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, §(struct sockaddr*)&c1.client_addr, len);
                n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&c1.client_addr, &len);
                buffer[n]=0;
            }
            inserimento(matrix, atoi(buffer), 1);
            stampa(matrix);

            if(vincitore(matrix) == 1){
                sendto(sockfd, "Hai vinto!", strlen("Hai vinto!"), 0, §(struct sockaddr*)&c1.client_addr, len);
                break;
            }

            turno=2;
        }else if(turno==2){
            sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, §(struct sockaddr*)&c2.client_addr, len);
            n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&c2.client_addr, &len);
            buffer[n]=0;

            while(atoi(buffer) < 1 && atoi(buffer) > 7){
                sendto(sockfd, "Inserisci colonna!", strlen("Inserisci colonna!"), 0, §(struct sockaddr*)&c2.client_addr, len);
                n=recvfrom(sockfd, buffer, 99, 0, (struct sockaddr*)&c2.client_addr, &len);
                buffer[n]=0;
            }
            inserimento(matrix, atoi(buffer), 2);
            stampa(matrix);

            if(vincitore(matrix) == 2){
                sendto(sockfd, "Hai vinto!", strlen("Hai vinto!"), 0, §(struct sockaddr*)&c2.client_addr, len);
                break;
            }

            turno=1;
        }
    }
}