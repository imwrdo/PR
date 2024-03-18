#include <stdio.h>
#include "function_1.h"
#include "function_2.h"
#include "function_3.h"
#include "function_4.h"

int main() {
  int a = 24, b = 36;
  printf("Największy wspólny dzielnik liczb %d i %d: %d\n", a, b, nwd(a, b));

  float a1 = 5;
  float b1 = 6;
  printf("Srednia harmoniczna liczb %.2f, %.2f  to: %.2f\n", a1,b1, srednia_harmoniczna(a1,b1));

  int n = 6;
  printf("Silnia z liczby %d: %lld\n", n, silnia(n));

  int podstawa = 2;
  int wykladnik = 3;
  int odp = potegowanie(podstawa, wykladnik);
  printf("%d do potęgi %d wynosi: %d\n", podstawa, wykladnik, odp);

  return 0;
}
