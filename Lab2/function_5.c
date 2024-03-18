int czy_trojkat(int a, int b, int c) {
    if (a + b > c && a + c > b && b + c > a) {
        return 1; 
    } else {
        return 0; 
    }
}