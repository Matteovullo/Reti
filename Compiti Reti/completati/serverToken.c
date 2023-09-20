#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>

void registrazione(char *username, char *password ,char * ip, int porta,int token){
    FILE *f;
    
    f= fopen("database4.txt", "a+");

    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }

    fprintf(f,"%s %s %s %d %d\n",username, password,ip, porta, token);
    fclose(f);
}

bool verifica_username_esistente(char * username){
    FILE * f; 


    f= fopen("database4.txt", "r");

    
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char tmp[20];
    while(fscanf(f, "%s\n",tmp )==1){
        if(strcmp(username, tmp)==0){
             fclose(f);
             return true;
        }
    }   
    fclose(f);
    return false;
}

char *torna_username_dato_token(int token){
    FILE * f; 


    f= fopen("database4.txt", "r");

    
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char username[20];
    int porta_tmp;
    int t_tmp;
    char pw_tmp[20];
    char ip_tmp[20];
    while(fscanf(f, "%s %s %s %d %d\n", username, pw_tmp,ip_tmp, &porta_tmp , &t_tmp )==5){
        if(token == t_tmp) {
            fclose(f);
            return strdup(username);
        }
    }   
    fclose(f);
    return strdup("errore");
}

int restituisci_porta(int i){
    FILE * f; 


    f= fopen("database4.txt", "r");

    
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char username[20];
    int porta_tmp;
    int t_tmp;
    char pw_tmp[20];
    int contatore=0;
    char ip_tmp[20];
    while(fscanf(f, "%s %s %s %d %d\n", username, pw_tmp, ip_tmp,&porta_tmp , &t_tmp )==5){
        if(contatore == i) {
            fclose(f);
            return porta_tmp;
        }
        contatore++;
    }   
    fclose(f);
    return -1;
}


char * restituisci_ip(int i){
    FILE * f; 


    f= fopen("database4.txt", "r");

    
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char username[20];
    int porta_tmp;
    int t_tmp;
    char pw_tmp[20];
    int contatore=0;
    char ip_tmp[20];
    while(fscanf(f, "%s %s %s %d %d\n", username, pw_tmp, ip_tmp,&porta_tmp , &t_tmp )==5){
        if(contatore == i) {
            fclose(f);
            return strdup(ip_tmp);
        }
        contatore++;
    }   
    fclose(f);
    return strdup("errore");
}

int torna_token_dato_username(char * username){
    FILE * f; 

    f= fopen("database4.txt", "r");

    
    if(f==NULL){
        printf("errore apertura file\n");
        exit(-1);
    }
    char username_tmp[20];
    int porta_tmp;
    int t_tmp;
    char pw_tmp[20];
    char ip_tmp[20];
    while(fscanf(f, "%s %s %s %d %d\n", username_tmp, pw_tmp,ip_tmp, &porta_tmp , &t_tmp )==5){
        if(strncmp(username,username_tmp, strlen(username_tmp))==0){
            fclose(f);
            return t_tmp;
        }
    }   
    fclose(f);
    return -1;
}


int main(int argc ,char **argv){
    srand(time(NULL));

    int sockfd,n;
    struct sockaddr_in remote_addr, local_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char ricevi[1000], manda[1000];
    char * comando, *username , *password, *porta_tmp, *token_client, *msg_client;
    int porta, token;
    char token_tmp[8];
    char *ip;

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

    client_addr.sin_family = AF_INET;

    for(;;){
        puts("--- in attesa di connessioni ---");
        n = recvfrom(sockfd , ricevi , 999 ,0, (struct sockaddr*)&remote_addr, &len);
        ricevi[n]=0;
        printf("ricevi:%s", ricevi);
        comando = strtok(ricevi," ");


        if(strncmp(comando,"REG",3)==0){
            username= strtok(NULL, " ");
            password= strtok(NULL, " ");
            porta_tmp= strtok(NULL, "\n");
            porta= atoi(porta_tmp);
            token= rand()% 32768;

            if(!(verifica_username_esistente(username))){
                registrazione(username, password, inet_ntoa(remote_addr.sin_addr), porta, token);
                sprintf(token_tmp, "%d", token);
                sendto(sockfd , token_tmp ,999 , 0, (struct sockaddr*)&remote_addr, len);
                printf("[+] REG COMPLETA\n");
                sendto(sockfd , "OKREG" ,9 , 0, (struct sockaddr*)&remote_addr, len);
            }else{
                printf("[-] REG NON COMPLETA\n");
                sendto(sockfd , "ERROREREG" ,9 , 0, (struct sockaddr*)&remote_addr, len);
            }
        }

        if(strncmp(comando,"LOG",3)==0){
            username= strtok(NULL, " ");
            password= strtok(NULL, "\n");

            if((verifica_username_esistente(username))){
                token = torna_token_dato_username(username);
                sprintf(token_tmp, "%d", token);
                printf("[+] LOG COMPLETO\n");
                sendto(sockfd , token_tmp ,  strlen(token_tmp),0, (struct sockaddr*)&remote_addr, len);
            }else{
                printf("[-] LOG NON COMPLETO\n");
                sendto(sockfd , "ERRORELOG" ,9 , 0, (struct sockaddr*)&remote_addr, len);
            }
        }


        if(strncmp(comando,"MSG",3)==0){
            token_client=strtok(NULL, " ");
            token = atoi(token_client);
            msg_client= strtok(NULL, "\n");
            username =torna_username_dato_token(token);
            sendto(sockfd ,"MSG RICEVUTO" ,999 , 0, (struct sockaddr*)&remote_addr, len);
            printf("[+] MSG DA %s : %s\n", username, msg_client);
            for(int i=0; i< 2 ; i++){
                porta= restituisci_porta(i);
                ip= restituisci_ip(i);
                client_addr.sin_port = htons(porta);
                inet_pton(AF_INET , ip , &(client_addr.sin_addr));
                if(strcmp(ip, inet_ntoa(remote_addr.sin_addr))!=0){
                    strcat(msg_client," da ");
                    strcat(msg_client, username);
                    sendto(sockfd , msg_client ,999 , 0, (struct sockaddr*)&client_addr, len);
                }
            }
        }
    }
}