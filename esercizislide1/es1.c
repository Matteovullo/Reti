#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int main(){
    FILE *fl;
    char str[20];
    fl=fopen("testo.txt", "w");
    if(fl==NULL){
        printf("errore!!");
        exit(-1);
    }

    while(strcmp(str, "FINE")){
        scanf("%s", &str);
        fprintf(fl,"%s\n", str);
    }
    
    fclose(fl);
    return 0;
}