#include <stdio.h>
#include <math.h>
#include "function_5.h"
#include "function_6.h"
#include "function_7.h"
#include "function_8.h"

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
    
    float r = 2.0;
    float h = 5.0;
    float pole_w = poleWalca(r,h);
    printf("Pole walca z radiusem %.2f i bokiem %.2f: %.2f\n",r,h,pole_w);

    int liczba = 123;
    int suma_c = sumaCyfr(liczba);
    printf("Suma cyfr liczby %d: %d\n",liczba,suma_c);
    
    int a = 5;
    int b = 2;
    int reszta = reszta_z_dzielenia(a,b);
    printf("Reszta z dzielenia liczb %d,%d: %d\n",a,b,reszta);
    
    return 0;
}
