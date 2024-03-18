unsigned long long silnia(int n) {
    if (n < 0) {
        return 0; 
    }
    unsigned long long wynik = 1;
    for (int i = 1; i <= n; i++) {
        wynik *= i;
    }
    return wynik;
}
