#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main(){
    int n=0;
    for(int i=0; i<10; i++){
        n=n+1;
        printf("n=%d\n", n);
    }

    //for(;;){}
    int pid=fork(); //pid=identificati del processo
    if(pid=0){
        //processo figlio
        for(int i=0; i<5; i++){
            printf("PID=%d, n=%d\n", pid, n);
        }
    }else{
        //processo padre
        for(int i=0; i<5; i++){

        }
    }

    return 0;
}