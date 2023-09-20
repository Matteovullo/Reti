#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>


void registra(char*ip, int porta){

    FILE *f;
    f= fopen("registro.txt", "a+");
    if( f == NULL){
        printf("errore apertura file registro\n");
        exit(-1);
    }

    int n_generato = rand() % __INT_MAX__;
    fprintf(f, "%s %d %d\n", ip, porta , n_generato);
    fclose(f);
}

int torna_id(char * ip , int porta){
    FILE *f;
    f= fopen("registro.txt", "r");
    if( f == NULL){
        printf("errore apertura file registro\n");
        exit(-1);
    }

    char iptmp[20];
    int p, id;
    while(fscanf(f, "%s%d%d\n", iptmp, &p, &id)==3){
        if((strncmp(iptmp, ip, strlen(iptmp))==0)&&(p==porta)){
            fclose(f);
            return id;
        }
    }
    return -1;
}



void prenota_risorsa(char * ip , int porta, char * nomerisorsa){
    int id;
    id= torna_id(ip, porta);
    char buffer[1000];

    FILE *f, *tmp;
    f= fopen("risorsa.txt", "a+");
    tmp = fopen("dbtmp.txt", "w+");
    if( f == NULL || tmp == NULL){
        printf("errore apertura file f o tmp\n");
        exit(-1);
    }
    
    char *risorsa,* quantita, *resto;
    char t[1000];
    while(fscanf(f,"%s\n",buffer)==1){
        strcpy(t,buffer);
        if(strncmp(buffer,nomerisorsa,strlen(nomerisorsa))!=0)
            fprintf(tmp, "%s\n", buffer);
        else{
            risorsa= strtok(buffer,":");
            quantita = strtok(NULL,":");
            resto= strtok(NULL,"\n");
            if(atoi(quantita)-1 < 0){
                fprintf(tmp, "%s\n", t);
            }
            else if(resto == NULL){
                fprintf(tmp, "%s%c%d%c%d%c\n",risorsa,':',atoi(quantita)-1,':',id,':');
            }else{
                fprintf(tmp, "%s%c%d%c%s%d%c\n", risorsa, ':',atoi(quantita)-1, ':',resto, id,':'); 
            }
        }
    } 

    fclose(f);
    fclose(tmp);
    remove("risorsa.txt");
    rename("dbtmp.txt","risorsa.txt");

}

char * elenco_risorse_prenotate(char * ip  , int porta){
    int id;
    id= torna_id(ip, porta);

    FILE *f;
    f= fopen("risorsa.txt", "r");
    if( f == NULL){
        printf("errore apertura file registro\n");
        exit(-1);
    }

    char buffer[1000];
    memset(buffer, 0 , 999);
    char manda[1000];
    strcpy(manda,"");
    char idstringa[20];
    char *risorsa,*quantita,*resto;
    sprintf(idstringa,"%d",id);
    while(fscanf(f, "%s\n", buffer)==1){
        risorsa = strtok(buffer,":");
        quantita = strtok(NULL, ":");
        while((resto = strtok(NULL,":")) != NULL){
            if(strncmp(resto,idstringa,strlen(idstringa))==0){
                strcat(manda, risorsa);
                strcat(manda," ");
                continue;
            }
        }
    }
    return strdup(manda);
}



char *elenco_risorse_disponibili(){
    FILE *f;
    f= fopen("risorsa.txt", "r");
    if( f == NULL){
        printf("errore apertura file registro\n");
        exit(-1);
    }

    char manda[1000];
    memset(manda, 0 , 999);
    strcpy(manda,"");
    char *risorsa,*quantita;
    char tmp[1000];
    while(fscanf(f,"%s\n",tmp)==1){
        risorsa= strtok(tmp,":");
        quantita= strtok(NULL, ":");
        if(atoi(quantita)>0){
            strcat(manda,risorsa);
            strcat(manda,"\n");
        }
    }

    fclose(f);
    return strdup(manda);
}


void libera_risorsa(char *ip, int porta){

    int id = torna_id(ip, porta);
    FILE *f, *tmp;
    f= fopen("risorsa.txt", "a+");
    tmp = fopen("dbtmp.txt", "w+");
    if( f == NULL || tmp == NULL){
        printf("errore apertura file f o tmp\n");
        exit(-1);
    }

    char buffer[1000];
    char *risorsa,*quantita,*resto;
    char idstringa[20];
    char buftmp[1000];
    bool x;
    sprintf(idstringa, "%d", id);
    while(fscanf(f, "%s", buffer)==1){
        x=true;
        strcpy(buftmp,"");
        risorsa= strtok(buffer,":");
        quantita= strtok(NULL,":");
        while((resto=strtok(NULL,":")) != NULL){
            if(strcmp(resto,idstringa)!=0){
                strcat(buftmp,resto);
                strcat(buftmp,":");
            }else{
                x=false;
            }
        }
        if(x)
            fprintf(tmp,"%s%c%d%c%s\n", risorsa,':',atoi(quantita)-1,':',buftmp);
        else    
            fprintf(tmp,"%s%c%d%c%s\n", risorsa,':',atoi(quantita),':',buftmp);
    }

    fclose(f);
    fclose(tmp);
    remove("risorsa.txt");
    rename("dbtmp.txt","risorsa.txt");
}



int main(int argc , char ** argv){
    int sockfd,n;
    struct sockaddr_in remote_addr, local_addr;
    socklen_t len= sizeof(struct sockaddr_in);
    char mandamsg[1000];
    char ricevimsg[1000];
    char *risorsa,*comando;

    if(argc != 2){
        printf("passare porta\n");
        exit(-1);
    }

    if((sockfd= socket(AF_INET ,SOCK_DGRAM ,0))<0){
        printf("errore socket\n");
        exit(-1);
    }

    memset(&local_addr, 0 , len);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port= htons(atoi(argv[1]));

    if((bind(sockfd, (struct sockaddr*)&local_addr , len))<0){
        printf("errore binding\n");
        exit(-1);
    }

    for(;;){
        n=recvfrom(sockfd, ricevimsg, 999 , 0, (struct sockaddr*)&remote_addr, &len);
        ricevimsg[n]= 0;

        if(strncmp(ricevimsg, "REG" , 3 )==0){
            registra(inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
        } 

        if(strncmp(ricevimsg, "RISORSA" , 7 )==0){
           risorsa = elenco_risorse_disponibili();
           sendto(sockfd, risorsa, 999 , 0, (struct sockaddr*)&remote_addr, len);
        }

        if(strncmp(ricevimsg, "PRENOTA" , 7 )==0){
            comando= strtok(ricevimsg," ");
            risorsa= strtok(NULL,"\n");
           prenota_risorsa(inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), risorsa);
        }

        if(strncmp(ricevimsg, "VEDIRISORSA" , 11 )==0){
            risorsa= elenco_risorse_prenotate(inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
            sendto(sockfd, risorsa, 999 , 0, (struct sockaddr*)&remote_addr, len);
        }

        if(strncmp(ricevimsg, "LIBERARISORSA" , 13 )==0){
           libera_risorsa(inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
        }
    }

}