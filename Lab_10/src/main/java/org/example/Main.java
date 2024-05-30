package org.example;

import java.io.FileWriter;
import java.io.IOException;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;

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
    private final Map<String, Integer> stock = new HashMap<>();
    private final int capacity;
    private int currentStock = 0;

    public Warehouse(int capacity) {
        this.capacity = capacity;
    }

    public synchronized boolean addProduct(Product product, String producerId) {
        while (currentStock + product.getQuantity() > capacity) {
            try {
                wait();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                return false;
            }
        }
        stock.merge(product.getType(), product.getQuantity(), Integer::sum);
        currentStock += product.getQuantity();
        System.out.println("Producer " + producerId + " added: " + product);
        notifyAll();
        return true;
    }

    public synchronized boolean removeProduct(String type, int quantity, String consumerId) {
        while (stock.getOrDefault(type, 0) < quantity) {
            try {
                wait();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                return false;
            }
        }
        stock.put(type, stock.get(type) - quantity);
        currentStock -= quantity;
        System.out.println("Consumer " + consumerId + " removed: " + quantity + " of " + type);
        notifyAll();
        return true;
    }

    public synchronized void printStock() {
        System.out.println("Current stock: " + stock);
    }

    public synchronized Map<String, Integer> getStockSnapshot() {
        return new HashMap<>(stock);
    }
}

class Producer implements Runnable {
    private final Warehouse warehouse;
    private final Random random = new Random();
    private final AtomicBoolean running;
    private final int productionSpeed;
    private final String producerId;

    public Producer(Warehouse warehouse, AtomicBoolean running, int productionSpeed, String producerId) {
        this.warehouse = warehouse;
        this.running = running;
        this.productionSpeed = productionSpeed;
        this.producerId = producerId;
    }

    @Override
    public void run() {
        while (running.get()) {
            try {
                Thread.sleep(productionSpeed + random.nextInt(1000));
                String type = "Type " + random.nextInt(5);
                int quantity = random.nextInt(10) + 1;
                Product product = new Product(type, quantity);
                if (!warehouse.addProduct(product, producerId)) {
                    System.out.println("Warehouse full. Producer " + producerId + " couldn't add: " + product);
                }
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
    private final int consumptionSpeed;
    private final String consumerId;

    public Consumer(Warehouse warehouse, AtomicBoolean running, int consumptionSpeed, String consumerId) {
        this.warehouse = warehouse;
        this.running = running;
        this.consumptionSpeed = consumptionSpeed;
        this.consumerId = consumerId;
    }

    @Override
    public void run() {
        while (running.get()) {
            try {
                Thread.sleep(consumptionSpeed + random.nextInt(1000));
                String type = "Type " + random.nextInt(5);
                int quantity = random.nextInt(10) + 1;
                if (!warehouse.removeProduct(type, quantity, consumerId)) {
                    System.out.println("Consumer " + consumerId + " couldn't remove " + quantity + " of " + type);
                }
                
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}

public class Main {
    private static final int WAREHOUSE_CAPACITY = 100;
    private static final int X_PRODUCERS = 2;
    private static final int Y_PRODUCERS = 2;
    private static final int Y_CONSUMERS = 2;
    private static final int Z_CONSUMERS = 2;
    private static final AtomicBoolean running = new AtomicBoolean(true);

    private static final int PRODUCTION_SPEED = 1000;
    private static final int CONSUMPTION_SPEED = 1000;

    public static void main(String[] args) throws InterruptedException, IOException {
        Warehouse warehouseA = new Warehouse(WAREHOUSE_CAPACITY);
        Warehouse warehouseB = new Warehouse(WAREHOUSE_CAPACITY);

        ExecutorService producerPool = Executors.newFixedThreadPool(X_PRODUCERS + Y_PRODUCERS);
        ExecutorService consumerPool = Executors.newFixedThreadPool(Y_CONSUMERS + Z_CONSUMERS);

        for (int i = 0; i < X_PRODUCERS; i++) {
            producerPool.submit(new Producer(warehouseA, running,  PRODUCTION_SPEED, "X" + i));
        }

        for (int i = 0; i < Y_PRODUCERS; i++) {
            producerPool.submit(new Producer(warehouseB, running,  PRODUCTION_SPEED, "Y" + i));
        }

        for (int i = 0; i < Y_CONSUMERS; i++) {
            consumerPool.submit(new Consumer(warehouseA, running,  CONSUMPTION_SPEED, "Y" + i));
        }

        for (int i = 0; i < Z_CONSUMERS; i++) {
            consumerPool.submit(new Consumer(warehouseB, running,  CONSUMPTION_SPEED, "Z" + i));
        }

        // Periodic status logging
        ScheduledExecutorService statusLogger = Executors.newScheduledThreadPool(1);
        statusLogger.scheduleAtFixedRate(() -> {
            System.out.println("Status report:");
            System.out.print("A: ");
            warehouseA.printStock();
            System.out.print("B: ");
            warehouseB.printStock();
            try (FileWriter writer = new FileWriter("warehouse_status.csv", true)) {
                writer.write("Warehouse A: " + warehouseA.getStockSnapshot() + "\n");
                writer.write("Warehouse B: " + warehouseB.getStockSnapshot() + "\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }, 0, 5, TimeUnit.SECONDS);

        // User input to stop the program
        System.out.println("Press Enter to stop...");
        Scanner scan = new Scanner(System.in);
        scan.nextLine();

        running.set(false);

        producerPool.shutdown();
        consumerPool.shutdown();
        statusLogger.shutdown();

        if (!producerPool.awaitTermination(1, TimeUnit.SECONDS)) {
            producerPool.shutdownNow();
        }

        if (!consumerPool.awaitTermination(1, TimeUnit.SECONDS)) {
            consumerPool.shutdownNow();
        }

        if (!statusLogger.awaitTermination(1, TimeUnit.SECONDS)) {
            statusLogger.shutdownNow();
        }

        System.out.println("Final stock status:");
        warehouseA.printStock();
        warehouseB.printStock();

        System.out.println("Program terminated.");
    }
}
