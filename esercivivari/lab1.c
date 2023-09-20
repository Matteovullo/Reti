#include<stdio.h>

void search_max_min(int vect[], int n){
    int min=vect[0];
    int max=vect[0];
    for(int i=0; i<n; i++){
        if(vect[i] > max) max=vect[i];
        if(vect[i] < min) min=vect[i];
    }
    printf("Max=%d", max);
    printf(" Min=%d", min);
}

int main(){
    int vect[10];
    for(int i=0; i<10; i++){
        vect[i]=i;
    }
    search_max_min(vect, 10);
}