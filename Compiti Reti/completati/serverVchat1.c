#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define N_CLIENT 3
int N_IP=0;

bool verifica_ip_file(char *ip , int porta){
    FILE *f;

    f= fopen("database1.txt", "r");
    
    if(f==NULL){
        printf("errore apertura del file database1.txt\n");
        exit(-1);
    }
    char ip1[15];
    int porta1;
    while(fscanf(f,"%s%d\n",ip1, &porta1)==2){
        if((strncmp(ip,ip1,strlen(ip))==0) && porta==porta1){
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

void registra_ip_porta_file(char * ip, int porta){
    FILE *f;

    f= fopen("database1.txt", "a+");
    
    if(f==NULL){
        printf("errore apertura del file database1.txt\n");
        exit(-1);
    }

    fprintf(f, "%s %d\n",ip, porta);
    N_IP++;
    fclose(f);
}

void print_file() {
    FILE *f;

    f= fopen("database1.txt", "r");
    
    if(f==NULL){
        printf("errore apertura del file database1.txt\n");
        exit(-1);
    }
    char ip[15];
    int porta;
    while(fscanf(f,"%s%d\n",ip, &porta)==2){
        printf("ip=%s porta=%d\n",ip,porta);
    }
    fclose(f);
}


int indice_client_file(char *ip){
    FILE *f;

    f= fopen("database1.txt", "r");
    
    if(f==NULL){
        printf("errore apertura del file database1.txt\n");
        exit(-1);
    }
    int indice=0;
    char ip1[15];
    int porta1;
    while(fscanf(f,"%s%d\n",ip1, &porta1)==2){
        if(strncmp(ip,ip1,strlen(ip))==0){
            fclose(f);
            return indice;
        }
        indice++;
    }
    fclose(f);
    return -1;
}

char * torna_ip_porta_file(int riga, int *porta){
    FILE *f;

    f= fopen("database1.txt", "r");
    
    if(f==NULL){
        printf("errore apertura del file database1.txt\n");
        exit(-1);
    }
    int indice=0;
    char ip[15];
    int porta1;
    while(fscanf(f,"%s%d\n",ip, &porta1)==2){
        if(indice == riga){
            fclose(f);
            *porta= porta1;
            return strdup(ip);
        }
        indice++;
    }
    fclose(f);
    return strdup("errore");
}


int **genera_matrice(){
    
   int **matrice = malloc(sizeof(int*)*N_CLIENT);
   for(int i=0; i<N_CLIENT ; i++){
    matrice[i] = malloc(sizeof(int)* N_CLIENT);
    for(int j=0; j<N_CLIENT ; j++){
        if(i == j ) matrice[i][j]= 0;
        else
            matrice[i][j] = rand()%(246)+10;
    }
   }
   return matrice;
}

  void stampa_matrice(int **matrice,int n){
    printf("--------   CLIENT 1  CLIENT 2   CLIENT 3   CLIENT 4   CLIENT 5   CLIENT 6\n");
    for(int i=0; i<n ; i++){
        printf("\nCLIENT %d   ",i+1);
        for(int j=0; j<n; j++){
            printf("%d         ", matrice[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}


int* indice_client_piu_vicini(int n, int **matrice , int indice_client){
    int *vet_temporaneo_indici =  malloc(sizeof(int)*n+1);
    int index=0;
    int tmpvalore;
    int tmpindice;
    bool trovato;

    for(int i=0; i<n; i++){ //n ip che mi servono
        tmpvalore=0;
        tmpindice=0;
        for(int j=0; j< N_CLIENT ; j++){ // tutti i client
            trovato=false;

            //se invece ho già un elemento allora di conseguenza se l'indice è gia presente nell'array posso continuare il ciclo
            for(int k=0; k<index;k++){
                if(vet_temporaneo_indici[k] == j ) trovato=true;
            }

            if(matrice[indice_client][j] > tmpvalore && trovato == false) {
                tmpvalore= matrice[indice_client][j];
                tmpindice = j;
            }
        }
        vet_temporaneo_indici[index] = tmpindice;
        index++;
    }
    return vet_temporaneo_indici;
}




int main(int argc ,char **argv){
    int sockfd,n, indice_client, nclientmandare,porta_client;
    struct sockaddr_in remote_addr, local_addr,client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char recvline[999];
    char *n_mandare;
    char *stringa;
    char *ip_tmp;
    int index_tmp;
    int *temp_vet;
    char *ip_client;

    if((sockfd= socket(AF_INET, SOCK_DGRAM , 0))<0){
        printf("errore nella creazione del socket\n");
        exit(-1);
    }

    if(argc!=2){
        printf(" passare la porta \n");
        exit(-1);
    }

    memset(&local_addr, 0 , len);
    local_addr.sin_port = htons(atoi(argv[1]));
    local_addr.sin_family = AF_INET;

    if((bind(sockfd, (struct sockaddr*)&local_addr , len))<0){
        printf("errore nel binding\n");
        exit(-1);
    }

    int **matrice_distanza = genera_matrice();
    stampa_matrice(matrice_distanza,N_CLIENT);
    
    for(;;){
        puts("in attesa del client");
        int n= recvfrom(sockfd,recvline, 999 , 0 , (struct sockaddr*)&remote_addr, &len);
        //ogni volta che il server riceve dal client un messaggio salva nel file se esso non è presente l'indirizzo ip+prta
        recvline[n]=0;
        //REGISTRAZIONE IP + PORTA DEI VARI CLIENT- LA PRIMA RIGA INDICA IL PRIMO CLIENT, LA IESIMA RIGA L'IESIMO CLIENT
        if(strncmp(recvline,"REG",3)==0){
            if(!(verifica_ip_file(inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port)))){
                registra_ip_porta_file(inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
            }
            print_file();
        }

        else{
            indice_client= indice_client_file(inet_ntoa(remote_addr.sin_addr));
            n_mandare = strtok(recvline, " ");
            stringa = strtok(NULL,"\n");
            //DEVO PRENDERE ALLORA GLI INDICI DELLA MATRICE DEI CLIENT E SCORRERE IL FILE
            //PER PRENDERE GLI INDIRIZZI IP+PORTA DEL CLIENT
            //INDICE DEL CLIENT DELLA MATRICE RAPPRESENTA LA POSIZIONE IN TERMINE DI RIGA DEL FILE DOVE SI TROVA IP+PORTA
            //QUINDI SE HO INDICE_CLIENT_PIU_VICINI MI TORNA 2, ANDRò A PRENDERE DAL FILE IP+PORTA DELLA RIGA 2
            nclientmandare = atoi(n_mandare);
            printf("DAL CLIENT=%d MANDARE LA STRINGA=%s A N:%d CLIENT\n", indice_client+1, stringa , nclientmandare);

            //sono gli indici dei client della matrice a cui devo inviare
            temp_vet=indice_client_piu_vicini(nclientmandare, matrice_distanza, indice_client);
            for(int i=0; i<nclientmandare ; i++) printf("\n mando al client:%d\n ",temp_vet[i]+1);

            //per ogni indice di questo vettore, prendo dal file l'iesima riga che rappresenta ip+porta del client
            for(int i=0 ; i< nclientmandare; i++){
                ip_client =torna_ip_porta_file(temp_vet[i], &porta_client);
                client_addr.sin_family = AF_INET;
                client_addr.sin_port = htons(porta_client);
                inet_pton(AF_INET, ip_client , &(client_addr.sin_addr));
                sendto(sockfd,stringa, 999 , 0 , (struct sockaddr*)&client_addr, len);
            }

            free(temp_vet);
        }
    }
}

