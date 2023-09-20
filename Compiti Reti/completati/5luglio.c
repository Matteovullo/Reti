#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT_CLIENT 5000


int n_client=8;


bool registra_username(char * username, char * ip){
    FILE *f;
    f= fopen("registro.txt", "a+");
    if( f == NULL){
        printf("errore apertura registro\n");
        exit(-1);
    }
    char usernametmp[20];
    char iptmp[20];
    while(fscanf(f, "%s %s\n",usernametmp , iptmp )==2){
        if(strcmp(username,usernametmp)==0){
            fclose(f);
            return false;
        }
    }

    fprintf(f,"%s %s\n", username, ip);
    n_client++;
    fclose(f);
    return true;
}

char * dato_riga_torna_ip(int i){

    FILE *f;
    f= fopen("registro.txt", "r");
    if( f == NULL){
        printf("errore apertura registro\n");
        exit(-1);
    }

    int contatore=0;
    char usernametmp[20];
    char iptmp[20];
    while(fscanf(f, "%s %s\n",usernametmp , iptmp )==2){
        if(contatore == i){
            fclose(f);
            return strdup(iptmp);
        }
        contatore++;
    }
    return strdup("errore");
}


bool login(char *username, char * ip){
    FILE *f;
    f= fopen("registro.txt", "r");
    if( f == NULL){
        printf("errore apertura registro\n");
        exit(-1);
    }
    char usernametmp[20];
    char iptmp[20];
    while(fscanf(f, "%s %s\n",usernametmp , iptmp )==2){
        if(strcmp(username,usernametmp)==0 && strcmp(ip, iptmp)==0){
            fclose(f);
            return true;
        }
    }

    fclose(f);
    return false;
}


//nome eseguibile + porta ascolto
int main(int argc, char** argv){
    int sockfd, n ;
    struct sockaddr_in remote_addr, local_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevimsg[1000];
    char *comando, *username;
    char *ip;
    char buffer[1000];
    char ipz[20];

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
    memset(&client_addr, 0, len);
    client_addr.sin_family= AF_INET;
    client_addr.sin_port= htons(5000);


    if((bind(sockfd, (struct sockaddr*)&local_addr, len))<0){
        printf("errore binding\n");
        exit(-1);
    }

    for(;;){
        n=recvfrom(sockfd, ricevimsg, 999, 0 , (struct sockaddr*)&remote_addr, &len);
        ricevimsg[n]=0;
        printf("MSG=%s ip:%s porta:%d\n", ricevimsg, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
        strcpy(buffer, ricevimsg);
    
        if(strncmp(ricevimsg,"REG",3)==0){
            comando = strtok(ricevimsg," ");
            username= strtok(NULL,"\n");

            
            if(registra_username(username, inet_ntoa(remote_addr.sin_addr))){
                sendto(sockfd, "[+] REGISTRAZIONE EFFETTUATA" , 999 ,0,  (struct sockaddr*)&remote_addr, len);
            }else{
                sendto(sockfd, "[-] USERNAME GIA SCELTO - RIPROVA" , 999 ,0,  (struct sockaddr*)&remote_addr, len);
            }
        }
        if(strncmp(ricevimsg,"LOGIN",5)==0){
            comando = strtok(ricevimsg," ");
            username= strtok(NULL,"\n");

            if(login(username, inet_ntoa(remote_addr.sin_addr))){
                sendto(sockfd, "[+] LOGIN EFFETTUATO" , 999 ,0,  (struct sockaddr*)&remote_addr, len);
            }else{
                sendto(sockfd, "[-] ERRORE LOGIN-RIPROVA O REGISTRATI" , 999 ,0,  (struct sockaddr*)&remote_addr, len);
            }
        }


        username = strtok(buffer," ");
        comando = strtok(NULL,"\n");

        if(strncmp(comando,"LOGOUT",6)==0){
            remote_addr.sin_port= htons(PORT_CLIENT);
            sendto(sockfd, "LOGOUT" , 999 ,0,  (struct sockaddr*)&remote_addr, len);
        }


        if(strncmp(comando,"MSG",3)==0){
            for(int i=0; i<n_client; i++){
                ip=dato_riga_torna_ip(i);
                strcpy(ipz, ip);
                inet_pton(AF_INET, ipz , &(client_addr.sin_addr));
                printf("MANDO %s a -%s-\n",comando,ip);
                sendto(sockfd, comando , 999 ,0,  (struct sockaddr*)&client_addr, len);
            }
        }  

    }   

    
}