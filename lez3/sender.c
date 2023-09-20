#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>

int main(int argc, char **argv){

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in dest_addr;
    char buffer[100];

    if(sockfd < 0){
        printf("Errore nell' apertura di socket.\n");
        return 0;
    }

    memset((char*) dest_addr, 0, sizeof(buffer));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr));
    dest_addr.sin_port = hton(atoi(argv[2]));

    printf("inserisci una stringa da invire al Server\n");
    scanf("%s", buffer);
    sendto(sockfd, buffer, sizeof(buffer)+1, 0, 
    (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    close(sockfd);

    return 0;
}