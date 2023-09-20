/*
*/

#include <stdio.h>
#include <stdlib.h>


typedef struct{
    char nome[20];
    char indirizzo[30];
    char numero[15];
}Persone;

int main(){
    Persone v[2];
    int k=0;
    FILE *f;
    char c[20];
    scanf("%s", &c);
    if((f=fopen("rubrica.txt", "r"))==NULL){
        perror("Il file non esiste");
        exit(1);
    }
    /*
    while(!feof(f)){
        char nome[20];
        char indirizzo[30];
        char numero[15];
        fscanf(f, "%s\n", nome);
        fscanf(f, "%s\n", indirizzo);
        fscanf(f, "%s\n", numero);
        printf("%s\n", nome);
        printf("%s\n", indirizzo);
        printf("%s\n", numero);
    }
    */
    while(fscanf(f, "%s%s%s\n", v[k].nome, v[k].indirizzo, v[k].numero) != EOF){
        //v[k].nome[20]=v[k].indirizzo[30]=v[k].numero[15]='\0';
        k++;
    }
    /*
    for(int i=0; i<2; i++){
        printf("%s\n", v[i].nome);
        printf("%s\n", v[i].indirizzo);
         printf("%s\n", v[i].numero);       
    }
    */
    //printf("%s\n", c);
    int j=0, count=0;
   for(int i=0; i<2; i++){
    while(v[i].nome[j] == c[j]){
        j++;
        count++;
    }
    if(count == j){
        printf("%s\n", v[i].nome);
        printf("%s\n", v[i].indirizzo);
        printf("%s\n", v[i].numero);
    }
    j=count=0;
   }

    fclose(f);
    return 0;
}