#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORTA_CLIENT 5000

char *manda_lista_utenti(){
    FILE * f;
    f= fopen("tabellaip.txt", "r");
    if( f == NULL){
        printf("errore file\n");
        exit(-1);
    }

    char buffer[1000];
    strcpy(buffer,"");
    char iptmp[20];
    int porta;
    char username[20];
    int i=0;
    char indice[10];
    while(fscanf(f, "%s%d%s\n", iptmp, &porta,username)==3){
        sprintf(indice,"%d",i);
        strcat(buffer,indice);
        strcat(buffer,")");
        strcat(buffer,username);
        strcat(buffer,"\n");
        i++;
    }

    fclose(f);
    return strdup(buffer);
}


void registrazione(char *ip, int p, char*username){
    FILE * f;
    f= fopen("tabellaip.txt", "a+");
    if( f == NULL){
        printf("errore file\n");
        exit(-1);
    }

    fprintf(f,"%s %d %s\n",ip, p,username);
    fclose(f);
}

bool autenticazione(char* ip, int p){
    FILE * f;
    f= fopen("tabellaip.txt", "r");
    if( f == NULL){
        printf("errore file\n");
        exit(-1);
    }

    char iptmp[20];
    int porta;
    while(fscanf(f, "%s%d\n", iptmp, &porta)==2){
        if((strncmp(ip,iptmp,strlen(ip))==0) && (porta == p)){
            fclose(f);
            return true;
        }
    }
    fclose(f);
    return false;
}

char *dato_username_torna_ip_e_porta(char *u,int *p){
    FILE * f;
    f= fopen("tabellaip.txt", "r");
    if( f == NULL){
        printf("errore file\n");
        exit(-1);
    }
    char iptmp[20];
    int porta;
    char username[20];
    while(fscanf(f, "%s%d%s\n", iptmp, &porta,username)==3){
        if(strncmp(u,username,strlen(u))==0){
            *p = porta;
            fclose(f);
            return strdup(iptmp);
        }
    }
    fclose(f);
    return strdup("ERROR");
}



//nome eseguibile + porta ascolto
int main(int argc, char** argv){
    int sockfd, n ;
    struct sockaddr_in remote_addr, local_addr,client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevimsg[1000];
    char *comando, *username, *riga;
    char buffer[80];
    char lista_utenti[100];
    char *ip, *porta,*lista;
    int n_utenti;
    char *ipclient;
    int n_client=0;
    int portatmp;
    char usernametmp[100];

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

    for(;;){
        n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
        ricevimsg[n]=0;


        if(strncmp(ricevimsg,"AUT",3)==0){
            if(autenticazione(inet_ntoa(remote_addr.sin_addr), PORTA_CLIENT)){
                sendto(sockfd,"OKAUT",5,0,(struct sockaddr*)&remote_addr, len);
                printf("OKAUT\n");
            }
            else{
                sendto(sockfd,"NOAUT",5,0,(struct sockaddr*)&remote_addr, len);
                printf("NOAUT\n");
            }
        }

        if(strncmp(ricevimsg,"REG",3)==0){
            comando= strtok(ricevimsg, " ");
            username= strtok(NULL,"\n");
            registrazione(inet_ntoa(remote_addr.sin_addr), PORTA_CLIENT,username);
        }


        if(strncmp(ricevimsg,"UTENTI",6)==0){
            username = manda_lista_utenti();
            sendto(sockfd,username,999,0,(struct sockaddr*)&remote_addr, len);
        }

        if(strncmp(ricevimsg,"MANDA",5)==0){
            comando = strtok(ricevimsg," ");
            riga= strtok(NULL,"\n");
            strcpy(buffer, riga);
            sendto(sockfd,"MANDA N.UTENTI",200,0,(struct sockaddr*)&remote_addr, len);
            strcpy(ricevimsg,"");
            FILE *u = fopen("utenti.txt","w+");
            if( u == NULL) printf("errore apertura file\n");
            for(;;){
                n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
                ricevimsg[n]=0;
                if(strncmp(ricevimsg,"STOP",4)==0) break;
                fprintf(u,"%s",ricevimsg);
                n_client++;
            }
            rewind(u);
            client_addr.sin_family= AF_INET;
            client_addr.sin_port= htons(PORTA_CLIENT);
            while(fscanf(u,"%s\n",usernametmp) == 1){
                ipclient=dato_username_torna_ip_e_porta(usernametmp,&portatmp);
                printf("mando:%s a %s %d\n",buffer,ipclient,portatmp);
                inet_pton(AF_INET, ipclient , &client_addr.sin_addr);
                sendto(sockfd,buffer,100,0,(struct sockaddr*)&client_addr, len);
            }
            fclose(u);
        }
    }   
}