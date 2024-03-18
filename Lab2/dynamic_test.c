#include <stdio.h>


int main() {
    
    int a1 = 3;
    int b1 = 4;
    int c1 = 5;
    int odp = czy_trojkat(a1,b1,c1) ;
    if(odp==1){
        puts("Mozna zbudowac trojkat");
    }
    else{   
        puts("Nie mozna zbudowac trojkata z podanymi bokami");
    }
    
    
    int n = 3;
    int Fib = fibonacci(n);
    printf("Suma %d pierwszych elementow ciagu Fibbonaciego wynosi: %d\n",n,Fib);

    int liczba = 123;
    int suma_c = sumaCyfr(liczba);
    printf("Suma cyfr liczby %d: %d\n",liczba,suma_c);
    
    int a = 5;
    int b = 2;
    int reszta = reszta_z_dzielenia(a,b);
    printf("Reszta z dzielenia liczb %d,%d: %d\n",a,b,reszta);
    
    return 0;
}
