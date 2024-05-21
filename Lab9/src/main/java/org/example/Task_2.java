package org.example;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.locks.*;

public class Task_2 {
    private static class CachingPrimeChecker {
        private final Map<Long, Boolean> cache = new ConcurrentHashMap<>();
        private final Lock lock = new ReentrantLock();
        private final Condition computationDone = lock.newCondition();

        public boolean isPrime(final long x) {
            Boolean cachedResult = cache.get(x);
            if (cachedResult != null) {
                System.out.printf("\t[Cache hit] %d is %s%n", x, cachedResult ? "prime" : "not prime");
                return cachedResult;
            }

            lock.lock();
            try {
                cachedResult = cache.get(x);
                if (cachedResult != null) {
                    return cachedResult;
                }

                boolean result = computeIfIsPrime(x);
                cache.put(x, result);
                computationDone.signalAll();
                return result;
            } finally {
                lock.unlock();
            }
        }

        private boolean computeIfIsPrime(long x) {
            final String currentThreadName = Thread.currentThread().getName();
            System.out.printf("\t[%s] Running computation for: %d%n", currentThreadName, x);
            try {
                // Simulating long computations
                Thread.sleep(TimeUnit.SECONDS.toMillis(1));
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }

            if (x < 2) {
                return false;
            }
            for (long i = 2; i * i <= x; i++) {
                if (x % i == 0) {
                    return false;
                }
            }
            return true;
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        ExecutorService executorService = Executors.newFixedThreadPool(4);
        CachingPrimeChecker primeChecker = new CachingPrimeChecker();

        try {
            while (true) {
                System.out.println("Enter 4 numbers (separated by space):");
                String input = scanner.nextLine();
                if (input.trim().isEmpty()) {
                    break;
                }
                String[] parts = input.split("\\s+");
                if (parts.length != 4) {
                    System.out.println("Please enter exactly 4 numbers.");
                    continue;
                }

                long[] numbers = new long[4];
                try {
                    for (int i = 0; i < 4; i++) {
                        numbers[i] = Long.parseLong(parts[i]);
                    }
                } catch (NumberFormatException e) {
                    System.out.println("Invalid input. Please enter valid numbers.");
                    continue;
                }

                CountDownLatch latch = new CountDownLatch(4);
                for (long number : numbers) {
                    executorService.submit(() -> {
                        try {
                            boolean isPrime = primeChecker.isPrime(number);
                            System.out.printf("Number %d is %s%n", number, isPrime ? "prime" : "not prime");
                        } finally {
                            latch.countDown();
                        }
                    });
                }

                latch.await();
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        } finally {
            executorService.shutdown();
        }

        System.out.println("Program terminated.");
    }
}
