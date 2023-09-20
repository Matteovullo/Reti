#include<stdlib.h>
#include<stdio.h>
#include<string.h>


typedef struct{
    char *nome;
    char *indirizzo;
    char *numero;
}Persona;

int main(){
    FILE* f;
    f=fopen("rubrica.txt", "r");
    if(f==NULL){
        puts("errore apertura file");
        exit(1);
    }

    Persona *array[5];
    int i=0;
    char nome[20];
    char indirizzo[30];
    char numero[15];
    
    while(fscanf(f, "%s %s %s", nome, indirizzo, numero)==3 && i<5){
        array[i] = malloc(sizeof(Persona*));
        array[i]->nome= malloc(strlen(nome)+1);
        strcpy(array[i]->nome,nome);
        array[i]->indirizzo = malloc(strlen(indirizzo)+1);
        strcpy(array[i]->indirizzo,indirizzo);
        array[i]->numero = malloc(strlen(numero)+1);
        strcpy(array[i]->numero,numero);
        i++;
    }

    for(int j = 0; j<2; j++) {
        printf("%s %s %s", array[j]->nome, array[j]->indirizzo, array[j]->numero);
    }

}