#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORTA_CLIENT 5000


int registrazione(char *username , char *ip){
    FILE *f; 
    f= fopen("registro14.txt","a+");
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char usernametmp[16];
    char iptmp[16];
    int token;
    while(fscanf(f,"%s %s %d\n",usernametmp, iptmp,&token)==3){
        if(strncmp(usernametmp,username,strlen(usernametmp))==0){
            fclose(f);
            return -1;
        }
    }

    token= rand()% __INT_MAX__;
    fprintf(f,"%s %s %d\n",username,ip,token);
    fclose(f);
    return token;
}


char *dato_token_torna_username(char *token){
    FILE *f; 
    f= fopen("registro14.txt","r");
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char usernametmp[16];
    char iptmp[16];
    char tokentmp[10];
    while(fscanf(f,"%s %s %s\n",usernametmp, iptmp, tokentmp)==3){
        if(strncmp(token,tokentmp,strlen(tokentmp))==0){
            fclose(f);
            return strdup(usernametmp);
        }
    }
    fclose(f);
    return strdup("ERRORE");
}

char * torna_ip(int i){
    FILE *f; 
    f= fopen("registro14.txt","r");
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char usernametmp[16];
    char iptmp[20];
    char tokentmp[10];
    int contatore=0;
    while(fscanf(f,"%s%s%s\n",usernametmp, iptmp, tokentmp)==3){
        if(contatore == i){
            fclose(f);
            return strdup(iptmp);
        }
        contatore++;
    }
    return strdup("error");
}




//nome eseguibile + porta ascolto
int main(int argc, char** argv){
    int sockfd, n ;
    struct sockaddr_in remote_addr, local_addr,client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevimsg[1000];
    char mandamsg[1000];
    char *comando, *username , *tokenz, *ip, *messaggio;
    int token;

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
    client_addr.sin_family= AF_INET;
    client_addr.sin_port= htons(PORTA_CLIENT);



    if((bind(sockfd, (struct sockaddr*)&local_addr, len))<0){
        printf("errore binding\n");
        exit(-1);
    }

    for(;;){
        n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
        ricevimsg[n]=0;

        if(strncmp(ricevimsg,"REG",3)==0){
            comando= strtok(ricevimsg," ");
            username= strtok(NULL,"\n");
            token= registrazione(username, inet_ntoa(remote_addr.sin_addr));
            sprintf(mandamsg,"%d",token);
            if(token == -1)
                sendto(sockfd, "[-] USERNAME OCCUPATO" , 999 ,0,  (struct sockaddr*)&remote_addr, len);
            else
                sendto(sockfd, mandamsg , 999 ,0,  (struct sockaddr*)&remote_addr, len);
        }


        if(strncmp(ricevimsg,"MSG",3)==0){
            comando= strtok(ricevimsg," ");
            tokenz= strtok(NULL," ");
            messaggio= strtok(NULL,"\n");
            username=dato_token_torna_username(tokenz);

            for(int i=0; i<100 ; i++){
                ip= torna_ip(i);
                if(strncmp(ip,"error",5)==0) break;
                inet_pton(AF_INET, ip ,&client_addr.sin_addr);
                strcpy(mandamsg,"");
                strcat(mandamsg,username);
                strcat(mandamsg," ");
                strcat(mandamsg, messaggio);
                sendto(sockfd, mandamsg , 999 ,0,  (struct sockaddr*)&client_addr, len);
            }
        }
    }

    
}