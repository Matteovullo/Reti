#include <stdio.h>
#include <stdlib.h>

int main(){
    int a, b;
    char s;
    scanf("%d", &a);
    scanf("%d", &b);
    scanf("%s", &s);

    if(s=='+') {
        int c=a+b;
        printf("la somma dei due numeri e'");
        printf("%d", c);
    }
    else if(s=='-') printf("la sottrazione dei due numeri é", a-b);
    else if(s=='/') printf("la divisione dei due numeri é", a/b);
    else if(s=='*') printf("la moltiplicazione dei due numeri é", a*b);
    else printf("suca");
    return 0;
}