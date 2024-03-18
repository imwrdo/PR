#include <stdio.h>
#include "function_5.h"
#include "function_6.h"
#include "function_7.h"
#include "function_8.h"

int main() {

    float r = 2;
    float h = 5;
    float odp = pole_walca(r,h) ;
    printf("Pole walca z radiusem %.2f i wysokoscia %.2f: %.2f\n",r,h,odp);
    
    int n = 3;
    int Fib = fibonacci(n);
    printf("Suma %d pierwszych elementow ciagu Fibbonaciego wynosi: %d\n",n,Fib);

    int liczba = 123;
    int suma_c = sumaCyfr(liczba);
    printf("Suma cyfr liczby %d: %d\n",liczba,suma_c);
    
    unsigned int n1 = 4;
    printf("Suma pierwszych %u liczb naturalnych wynosi %llu\n", n1, suma_naturalnych(n1));
    return 0;
}
