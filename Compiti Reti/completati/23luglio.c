#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 5000


char * ricevi_dati(char *nomefile){
    FILE *f;
    f= fopen(nomefile,"r");
    if( f== NULL){
        printf("ERRORE APERTURA FILE\n");
        exit(-1);
    }

    char buffer[1000];
    char msg[1000];

    strcpy(msg,"");
    while(fscanf(f,"%[^/n]/n", buffer)==1){
        strcat(msg,buffer);
        strcat(msg,"\n");
    }
    fclose(f);
    return strdup(msg);
}

//nome ip porta
int main(int argc, char **argv){
    int sockfd , n ;
    struct sockaddr_in remote_addr, client_addr,local_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    char mandamsg[1000];
    char reg[50];
    char file[50];
    char ricevimsg[1000];
    char *ip,*porta,*ipporta,*comando,*filez,*dati;
    char mandafile[50];

    if(argc != 3){
        printf("passare ip e porta server\n");
        exit(-1);
    }

    if (!fork()){    

        if((sockfd=socket(PF_INET,SOCK_DGRAM,0)) <0)
        { 
            printf("\nError in socket opening ... exit!");
            return -1;
        }


        memset(&local_addr,0,sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_port=htons((PORT));
        if( bind(sockfd, (struct sockaddr *) &local_addr, len)<0)
        { 
            return -1;
        }

        for (;;){ 
            n = recvfrom(sockfd,ricevimsg,999,0,(struct sockaddr *) &remote_addr,&len);
            ricevimsg[n] = 0;
            comando = strtok(ricevimsg," ");
            filez= strtok(NULL,"\n");
            dati= ricevi_dati(filez);
            sendto(sockfd, dati , 999 ,0,  (struct sockaddr*)&remote_addr, len);
	        printf("From IP:%s Port:%d msg:%s \n", inet_ntoa(remote_addr.sin_addr),  ntohs(remote_addr.sin_port), ricevimsg);
        }
        
    }else{
        if((sockfd= socket(AF_INET, SOCK_DGRAM, 0))<0){
            printf("errore nella socket\n");
            exit(-1);
        }


        memset(&remote_addr, 0, len);
        remote_addr.sin_family= AF_INET;
        remote_addr.sin_port= htons(atoi(argv[2]));
        inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);

        /* ogni volta che il client si collega invia una reg
        se è presente nel file effettua il login */
        printf("[+] USERNAME:\n");
        fgets(mandamsg, 999 , stdin);
        strcpy(reg,"REG ");
        strcat(reg, mandamsg);
        sendto(sockfd,reg, 50 ,0 , (struct sockaddr*)&remote_addr, len);
        n= recvfrom(sockfd,ricevimsg, 999 , 0 ,(struct sockaddr*)&remote_addr,&len);
        printf("%s\n", ricevimsg);

        printf("[+] invia nome file disponibili \n");
        fgets(mandamsg, 999 , stdin);
        strcpy(file,"FILE ");
        strcat(file,mandamsg);
        sendto(sockfd,file, 50 ,0 , (struct sockaddr*)&remote_addr, len);


        for(;;){
            printf("[+] in attesa di comandi [+]\n");
            printf("[+] LISTAUTENTIFILES [+]\n");
            fgets(mandamsg, 999 ,stdin);
            sendto(sockfd, mandamsg , 999 ,0,  (struct sockaddr*)&remote_addr, len);

            if(strncmp(mandamsg, "UTENTI", 6)==0){
                n= recvfrom(sockfd,ricevimsg, 999 , 0 ,(struct sockaddr*)&remote_addr,&len);
                printf("%s\n",ricevimsg);
            }
            if(strncmp(mandamsg, "UTENTE", 6)==0){
                n= recvfrom(sockfd,ricevimsg, 999 , 0 ,(struct sockaddr*)&remote_addr,&len);
                printf("%s\n",ricevimsg);
            }

            if(strncmp(mandamsg, "IP", 2)==0){
                n= recvfrom(sockfd,ricevimsg, 999 , 0 ,(struct sockaddr*)&remote_addr,&len);
                ip= strtok(ricevimsg," ");
                porta= strtok(NULL,"\n");
                client_addr.sin_family= AF_INET;
                printf("IP=%s porta=%s\n", ip,porta);
                inet_pton(AF_INET, ip, &client_addr.sin_addr);
                client_addr.sin_port= htons(atoi(porta));
                printf("che file vuoi scaricare?\n");
                fgets(mandamsg,999,stdin);
                strcpy(mandafile,"MANDA ");
                strcat(mandafile,mandamsg);
                sendto(sockfd, mandafile , 999 ,0,  (struct sockaddr*)&client_addr, len);
                n= recvfrom(sockfd,mandafile, 999 , 0 ,(struct sockaddr*)&remote_addr,&len);
                printf("%s\n", mandafile);
            }
        }
    }

}