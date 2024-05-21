package org.example;

import java.io.IOException;
import java.util.Random;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.Scanner;
class Product {
    private final String type;
    private final int quantity;

    public Product(String type, int quantity) {
        this.type = type;
        this.quantity = quantity;
    }

    public String getType() {
        return type;
    }

    public int getQuantity() {
        return quantity;
    }

    @Override
    public String toString() {
        return "Product{" +
                "type='" + type + '\'' +
                ", quantity=" + quantity +
                '}';
    }
}

class Warehouse {
    private final ConcurrentHashMap<String, Integer> stock = new ConcurrentHashMap<>();
    private final int capacity;
    private int currentStock = 0;

    public Warehouse(int capacity) {
        this.capacity = capacity;
    }

    public synchronized boolean addProduct(Product product) {
        if (currentStock + product.getQuantity() <= capacity) {
            stock.merge(product.getType(), product.getQuantity(), Integer::sum);
            currentStock += product.getQuantity();
            System.out.println("Added: " + product);
            return true;
        }
        return false;
    }

    public synchronized boolean removeProduct(String type, int quantity) {
        Integer currentQuantity = stock.get(type);
        if (currentQuantity != null && currentQuantity >= quantity) {
            stock.put(type, currentQuantity - quantity);
            currentStock -= quantity;
            System.out.println("Removed: " + quantity + " of " + type);
            return true;
        }
        return false;
    }

    public synchronized void printStock() {
        System.out.println("Current stock: " + stock);
    }
}

class Producer implements Runnable {
    private final Warehouse warehouse;
    private final Random random = new Random();
    private final AtomicBoolean running;

    public Producer(Warehouse warehouse, AtomicBoolean running) {
        this.warehouse = warehouse;
        this.running = running;
    }

    @Override
    public void run() {
        while (running.get()) {
            try {
                Thread.sleep(random.nextInt(1000) + 1000);
                String type = "Type " + random.nextInt(5);
                int quantity = random.nextInt(10) + 1;
                Product product = new Product(type, quantity);
                if (!warehouse.addProduct(product)) {
                    System.out.println("Warehouse full. Couldn't add: " + product);
                }
                warehouse.printStock();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}

class Consumer implements Runnable {
    private final Warehouse warehouse;
    private final Random random = new Random();
    private final AtomicBoolean running;

    public Consumer(Warehouse warehouse, AtomicBoolean running) {
        this.warehouse = warehouse;
        this.running = running;
    }

    @Override
    public void run() {
        while (running.get()) {
            try {
                Thread.sleep(random.nextInt(1000) + 1000);
                String type = "Type " + random.nextInt(5);
                int quantity = random.nextInt(10) + 1;
                if (!warehouse.removeProduct(type, quantity)) {
                    System.out.println("Couldn't remove " + quantity + " of " + type);
                }
                warehouse.printStock();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}

public class Task_1 {
    private static final int WAREHOUSE_CAPACITY = 100;
    private static final int PRODUCERS = 5;
    private static final int CONSUMERS = 5;
    private static final AtomicBoolean running = new AtomicBoolean(true);

    public static void main(String[] args) throws InterruptedException, IOException {
        Warehouse warehouse = new Warehouse(WAREHOUSE_CAPACITY);
        ExecutorService producerPool = Executors.newFixedThreadPool(PRODUCERS);
        ExecutorService consumerPool = Executors.newFixedThreadPool(CONSUMERS);

        for (int i = 0; i < PRODUCERS; i++) {
            producerPool.submit(new Producer(warehouse, running));
        }

        for (int i = 0; i < CONSUMERS; i++) {
            consumerPool.submit(new Consumer(warehouse, running));
        }

        System.out.println("Press Enter to stop...");
        Scanner scan = new Scanner(System.in);
        scan.nextLine();

        running.set(false);

        producerPool.shutdown();
        consumerPool.shutdown();

        if (!producerPool.isTerminated()) {
            producerPool.shutdownNow();
        }

        if (!consumerPool.isTerminated()) {
            consumerPool.shutdownNow();
        }

        System.out.println("Program terminated.");
    }
}
