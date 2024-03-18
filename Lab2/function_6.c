int fibonacci(int n) {
    int fib1 = 0; 
    int fib2 = 1; 
    int suma = 0; 

    if (n == 0)
        return 0;

    suma = fib1 + fib2;

    for (int i = 2; i <= n; ++i) {
        int fib = fib1 + fib2; 
        suma += fib; 
        fib1 = fib2; 
        fib2 = fib;
    }

    return suma;
}
