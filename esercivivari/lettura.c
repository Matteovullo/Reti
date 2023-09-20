#include<stdlib.h>
#include<stdio.h>

typedef struct{
   int x;
   int y; 
} Punto2d;

int getX(Punto2d p2d){
    return p2d.x;
}

int getY(Punto2d p2d){
    return p2d.y;
}

int getSomma(Punto2d p2d){
    return p2d.x+p2d.y;
}

void stampa(Punto2d p2d){
    printf("primo punto:%d\n", p2d.x);
    printf("secondo punto:%d\n", p2d.y);
}

int main(){
    Punto2d p2d;
    p2d.x=5;
    p2d.y=7;
    printf("%d\n", getX(p2d));
    printf("%d\n", getY(p2d));
    printf("%d\n", getSomma(p2d));
    stampa(p2d);    
}