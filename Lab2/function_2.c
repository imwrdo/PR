#include <stdio.h>
float srednia_harmoniczna(float x, float y) {
    if (x == 0 || y == 0) {
        printf("Jedna z liczb nie moze wynosic zero\n");
        return 0; 
    }
    return 2.0 / ((1.0 / x) + (1.0 / y));
}