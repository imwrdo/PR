int potegowanie(int podstawa, int wykladnik) {
    int wynik = 1;
    for (int i = 0; i < wykladnik; i++) {
        wynik = wynik * podstawa;
    }
    return wynik;
}
