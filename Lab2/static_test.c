#include <stdio.h>

int main() {
  int a = 24, b = 36;
  printf("Największy wspólny dzielnik liczb %d i %d: %d\n", a, b, nwd(a, b));

  int x = 10, y = 5, z = 7;
  printf("Najwieksza liczba sposrod %d, %d i %d to: %d\n", x, y, z, maksimum(x, y, z));

  int n = 6;
  printf("Silnia z liczby %d: %lld\n", n, silnia(n));

  int podstawa = 2;
  int wykladnik = 3;
  int odp = potegowanie(podstawa, wykladnik);
  printf("%d do potęgi %d wynosi: %d\n", podstawa, wykladnik, odp);

  return 0;
}
