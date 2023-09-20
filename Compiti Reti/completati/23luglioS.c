#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT 5000

typedef struct{
    char ip[17];
    char username[20];
}CLIENT;


void registra(char * username , char * ip , int porta){
    FILE * f;
    f=fopen("elencoutenti.txt","a+");
    if( f == NULL){
        printf("errore apertura file\n");
        exit(-1);
    }

    fprintf(f,"%s %s %d\n", ip, username, porta);
    fclose(f);
}


bool login(char * username , char * ip , int porta){
    FILE * f;
    CLIENT c;
    f=fopen("elencoutenti.txt","r");
    if( f == NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    rewind(f);
    int p;
    char buffer[1000];
    char *iptmp;
    char *usernametmp;
    while(fscanf(f, "%[^\n]\n",buffer)==1){
        iptmp= strtok(buffer," ");
        usernametmp=strtok(NULL," ");
        printf("%s=%s %s=%s\n", iptmp,ip,usernametmp,username);
        if((strncmp(username,usernametmp,strlen(username))==0)&&(strncmp(iptmp,ip,strlen(ip))==0))
            return true;
    }
    fclose(f);
    return false;
}


void registra_file_client(char * ip, char * files){
    FILE * f;
    FILE * fp;
    CLIENT c;
    f=fopen("elencoutenti.txt","r");
    fp=fopen("elencotmp.txt","w+");

    if( f == NULL || fp == NULL){
        printf("errore apertura file\n");
        exit(-1);
    }

    int p;
    char buffer[1000];
    char msg[1000];
    char *iptmp;
    while(fscanf(f, "%[^\n]\n", buffer)==1){
        strcpy(msg, buffer);
        iptmp=strtok(msg," ");
        if(strcmp(iptmp,ip)==0){
            fprintf(fp,"%s %s\n",buffer,files);
        }else{
            fprintf(fp,"%s\n",buffer);
        }
    }

    fclose(f);
    fclose(fp);
    remove("elencoutenti.txt");
    rename("elencotmp.txt", "elencoutenti.txt");
}

char * manda_lista_utenti(){
    FILE * f;
    CLIENT c;
    f=fopen("elencoutenti.txt","r");
    if( f == NULL){
        printf("errore apertura file\n");
        exit(-1);
    }

    char buffer[1000];
    char msg[1000];
    char *ip, *username;
    char manda[1000];
    strcpy(manda,"");
    while(fscanf(f, "%[^\n]\n",buffer)==1){
        strcpy(msg,buffer);
        ip=strtok(buffer," ");
        username=strtok(NULL," ");
        strcat(manda,username);
        strcat(manda," ");
    }
    
    printf("%s\n", manda);
    fclose(f);
    return strdup(manda);
}

char * dato_utente_torna_lista(char *utente){
    FILE * f;
    CLIENT c;
    f=fopen("elencoutenti.txt","r");
    if( f == NULL){
        printf("errore apertura file\n");
        exit(-1);
    }

    char buffer[1000];
    char manda[1000];
    char *ip, *username,*p, *resto;
    strcpy(manda,"");
    while(fscanf(f, "%[^\n]\n",buffer)==1){
        ip=strtok(buffer," ");
        username=strtok(NULL," ");
        p=strtok(NULL, " ");
        resto = strtok(NULL,"\n");
        if(strncmp(username,utente,strlen(utente))==0)
            strcat(manda,resto);
    }
    fclose(f);
    return strdup(manda);
}

char * torna_ip_porta(char *utente){
    FILE * f;
    CLIENT c;
    f=fopen("elencoutenti.txt","r");
    if( f == NULL){
        printf("errore apertura file\n");
        exit(-1);
    }

    char buffer[1000];
    char manda[1000];
    char *ip, *username,*p, *resto;
    strcpy(manda,"");
    while(fscanf(f, "%[^\n]\n",buffer)==1){
        ip=strtok(buffer," ");
        username=strtok(NULL," ");
        p=strtok(NULL, " ");
        resto = strtok(NULL,"\n");
        if(strncmp(username,utente,strlen(utente))==0){
            strcat(manda,ip);
            strcat(manda," ");
            strcat(manda,p);
        }
    }
    fclose(f);
    return strdup(manda);
}


//nome eseguibile + porta ascolto
int main(int argc, char** argv){
    int sockfd, n ;
    struct sockaddr_in remote_addr, local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevimsg[1000];
    char *comando, *username , *file, *utenti;

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

        if(strncmp(ricevimsg, "REG", 3 )==0){
            comando= strtok(ricevimsg," ");
            username= strtok(NULL,"\n");
            if(!(login(username, inet_ntoa(remote_addr.sin_addr), PORT))){
                registra(username, inet_ntoa(remote_addr.sin_addr), PORT);
                sendto(sockfd, "[-]REG COMPLETATA",18, 0, (struct sockaddr*)&remote_addr, len);
            }else{
                sendto(sockfd, "[-]LOG COMPLETATA",18, 0, (struct sockaddr*)&remote_addr, len);
            }
        }

        if(strncmp(ricevimsg, "FILE", 4 )==0){
            comando= strtok(ricevimsg," ");
            file= strtok(NULL,"\n");
            printf("%s\n",file);
            registra_file_client(inet_ntoa(remote_addr.sin_addr), file);
        }

        if(strncmp(ricevimsg, "UTENTI", 6)==0){
            utenti = manda_lista_utenti();
            sendto(sockfd, utenti,100, 0, (struct sockaddr*)&remote_addr, len);
        }

        if(strncmp(ricevimsg, "UTENTE", 6)==0){
            comando= strtok(ricevimsg," ");
            username= strtok(NULL,"\n");
            printf("%s\n", username);
            utenti=dato_utente_torna_lista(username);
            sendto(sockfd, utenti,100, 0, (struct sockaddr*)&remote_addr, len);
        }

        if(strncmp(ricevimsg, "IP", 2)==0){
            comando= strtok(ricevimsg," ");
            username= strtok(NULL,"\n");
            utenti=torna_ip_porta(username);
            sendto(sockfd, utenti,100, 0, (struct sockaddr*)&remote_addr, len);
        }

    }

    
}