#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 5000

char *registra_client_e_torna_id(char *ip, int porta){
    FILE * f;
    f= fopen("database10.txt", "a+");
    if( f == NULL){
        printf("ERRORE FILE\n");
        exit(-1);
    }
    char ip_tmp[16];
    int conta=1;
    char tmp[4];
    char registra_nome[4];
    char porta_char[10];
    char tmp1[10]; 
    while(fscanf(f,"%s%s%s\n",ip_tmp,registra_nome,tmp1)==3){
        if(strncmp(ip_tmp, ip, 15)==0){
            return strdup(registra_nome);
            fclose(f);
        }   
        conta++;
    }

    rewind(f);
    sprintf(tmp,"%d",conta);
    strcpy(registra_nome,"c");
    strcat(registra_nome, tmp);

    sprintf(porta_char, "%d", porta);
    fprintf(f,"%s %s %s\n",ip, registra_nome, porta_char);
    fclose(f);
    return strdup(registra_nome);
}

char * torna_ip_porta(){
    FILE * f;
    f= fopen("database10.txt", "r");
    if( f == NULL){
        printf("ERRORE FILE\n");
        exit(-1);
    }

    char ip[16];
    char buffer[1000];
    memset(&buffer, 0, 999);
    char client[5];
    char porta_char[10];
    strcpy(buffer,"IPPORTA:");
    while(fscanf(f,"%s%s%s\n",ip, client ,porta_char)==3){
            strcat(buffer,ip);
            strcat(buffer," ");
            strcat(buffer,porta_char);
            strcat(buffer,"-");
        }
    printf("TORNO:%s\n",buffer);
    fclose(f);
    return strdup(buffer);
}


char *manda_elenco_risorse_prenotate(char *idclient){
    FILE * f;
    f= fopen("risorse.txt", "r");
    if( f == NULL){
        printf("ERRORE FILE\n");
        exit(-1);
    }
    char buffer[1000];
    strcpy(buffer,"RISORSE:");
    memset(buffer,0,sizeof(buffer));
    char tmp[100];
    char *nome_risorsa;
    char *resto;
    int i=0;
    while(fscanf(f,"%s\n",tmp)==1){
        nome_risorsa=strtok(tmp,":");
        resto =strtok(NULL,":");
        while(resto!=NULL){
            resto=strtok(NULL,":");
            if(resto != NULL  && strcmp(resto,idclient)==0){
                strcat(buffer,nome_risorsa);
                strcat(buffer," ");
                i++;
            }
        }
    }
    fclose(f);
    if(i==0) return strdup("NESSUNA RISORSA");
    return strdup(buffer);
}

char *manda_elenco_risorse_disponibili(){
    FILE * f;
    f= fopen("risorse.txt", "r");
    if( f == NULL){
        printf("ERRORE FILE\n");
        exit(-1);
    }
    char buffer[1000];
    memset(buffer,0,sizeof(buffer));
    char tmp[100];
    char *nome_risorsa;
    char *quantità;
    while(fscanf(f,"%s\n",tmp)==1){
        nome_risorsa=strtok(tmp,":");
        quantità =strtok(NULL,":");
        if(atoi(quantità) > 0){
            strcat(buffer,nome_risorsa);
            strcat(buffer,"\n");
        }
    }
    fclose(f);
    return strdup(buffer);
}

void prenota_risorsa(char *idclient,char * risorsa){
    FILE * f, *p;
    f= fopen("risorse.txt", "r");
    p= fopen("temp.txt", "w");
    if( f == NULL || p == NULL){
        printf("ERRORE FILE\n");
        exit(-1);
    }
    char buffer[1000];
    char tmp[1000];
    char *id,*quantita,*resto;
    char nuova_quantita[10];
    int aggiorna_quantita;
    memset(&buffer, 0 , 999);
    while(fscanf(f, "%[^\n]\n", buffer)==1){
        strcpy(tmp,buffer);
        id= strtok(buffer,":");
       

        if(strncmp(id,risorsa,strlen(id))==0){
            quantita=strtok(NULL,":");
            aggiorna_quantita= (atoi(quantita))-1;
            sprintf(nuova_quantita,"%d", aggiorna_quantita);
            strcpy(quantita, nuova_quantita);
            strcpy(nuova_quantita,":");
            strcat(nuova_quantita, quantita);
            strcat(nuova_quantita,":"); 
            resto = strtok(NULL,"\n");
            if(resto != NULL)
                fprintf(p,"%s%s%s%s%c\n",id,nuova_quantita,resto,idclient,':');
            else
                fprintf(p,"%s%s%s%c\n",id,nuova_quantita,idclient,':');
        }else{
            fprintf(p,"%s\n",tmp);
        }
    }

    fclose(f);
    fclose(p);
    remove("risorse.txt");
    rename("temp.txt","risorse.txt");
}


void libera_risorsa(char *nome_risorsa, char *idclient){
    FILE * f, *p;
    f= fopen("risorse.txt", "r");
    p= fopen("temp.txt", "w");
    if( f == NULL || p == NULL){
        printf("ERRORE FILE\n");
        exit(-1);
    }
    char buffer[1000];
    char tmp[1000];
    char *resto;
    memset(&buffer,0,999);
    while(fscanf(f,"%s",buffer)==1){
        if(strncmp(buffer,nome_risorsa,strlen(nome_risorsa))==0){
            resto= strtok(buffer,":");
            strcpy(tmp,resto);
            strcat(tmp,":");
            while(resto!=NULL){
                resto= strtok(NULL,":");
                if((resto!=NULL) && (strcmp(resto,idclient)!=0)){
                    strcat(tmp,resto);
                    strcat(tmp,":");
                    printf("%s\n", tmp);
                }
            }
            fprintf(p,"%s\n",tmp);
        }else{
            fprintf(p,"%s\n",buffer);
        }
    }
    fclose(f);
    fclose(p);
    remove("risorse.txt");
    rename("temp.txt","risorse.txt");
}

int main(int argc, char **argv){
    int sockfd,n;
    struct sockaddr_in remote_addr, local_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char buffer[1000];
    char *sendline;
    char *comando;
    char *idclient;
    char *risorsa;
    char *ip, *porta;

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

    for(;;){
        n = recvfrom(sockfd,buffer, 999 ,0, (struct sockaddr*)&remote_addr, &len);
        buffer[n]=0;

        comando = strtok(buffer," ");
        
        if(strncmp(comando,"DISPONIBILI",11)==0){
            idclient=registra_client_e_torna_id(inet_ntoa(remote_addr.sin_addr),PORT);
            sendline= manda_elenco_risorse_disponibili();
            sendto(sockfd,sendline, 999 ,0, (struct sockaddr*)&remote_addr, len);
        }

        if(strncmp(comando,"PRENOTA",7)==0){
            idclient=registra_client_e_torna_id(inet_ntoa(remote_addr.sin_addr),PORT);
            risorsa= strtok(NULL,"\n");
            prenota_risorsa(idclient, risorsa);
            strcat(risorsa," prenotata");
            sendline= torna_ip_porta();
            comando= strtok(sendline,":");
            for(;;){
                    ip= strtok(NULL," ");
                    porta= strtok(NULL, "-");
                    if(ip == NULL || porta == NULL) break;
                    memset(&client_addr, 0 ,len);
                    client_addr.sin_family= AF_INET;
                    inet_pton(AF_INET, ip, &(client_addr.sin_addr));
                    client_addr.sin_port = htons(atoi(porta));
                    sendto(sockfd,risorsa, 999 ,0, (struct sockaddr*)&client_addr, len);
                }
        }

        if(strncmp(comando,"MOSTRARISORSE",13)==0){
            idclient=registra_client_e_torna_id(inet_ntoa(remote_addr.sin_addr),PORT);
            sendline = manda_elenco_risorse_prenotate(idclient);
            sendto(sockfd,sendline, 999 ,0, (struct sockaddr*)&remote_addr, len);
        }

        if(strncmp(comando,"LIBERARISORSA",13)==0){
            idclient=registra_client_e_torna_id(inet_ntoa(remote_addr.sin_addr),PORT);
            risorsa= strtok(NULL,"\n");
            strcat(risorsa," liberata");
            libera_risorsa(risorsa,idclient);
            sendline= torna_ip_porta();
              for(;;){
                    ip= strtok(NULL," ");
                    porta= strtok(NULL, "-");
                    if(ip == NULL || porta == NULL) break;
                    memset(&client_addr, 0 ,len);
                    client_addr.sin_family= AF_INET;
                    inet_pton(AF_INET, ip, &(client_addr.sin_addr));
                    client_addr.sin_port = htons(atoi(porta));
                    sendto(sockfd,risorsa, 999 ,0, (struct sockaddr*)&client_addr, len);
                }
        }
    }
}
