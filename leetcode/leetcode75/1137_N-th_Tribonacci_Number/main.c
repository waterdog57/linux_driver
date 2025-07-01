#include <stdio.h>
int tribonacci(int n) {
    if(n < 0 || n>37) return 0;

    int a=0,b=1,c=1,d =2;
    if(n == 0){
         return 0;
    }else if(n == 1){
        return 1;
    }else if(n == 2){
        return 1;
    }else if(n == 3){
        return 2;
    }else{
        n -= 3;
        a = 1;
        b = 1;
        c = 2;

        while(n--){
            d = a + b + c;
            a = b;
            b = c;
            c = d;
        }
    }
    return d;
}
