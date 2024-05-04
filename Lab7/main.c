#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 100

// Struktura przechowująca dane dla wątków
typedef struct {
    int thread_id;
    int lower_bound;
    int upper_bound;
    int* primes_found;
    int primes_count; // Licznik znalezionych liczb pierwszych
    int primes_found_count;
    FILE* log_file;
    pthread_mutex_t* mutex;
} ThreadData;

// Funkcja sprawdzająca czy liczba jest pierwsza
int is_prime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i += 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    return 1;
}

// Funkcja wykonywana przez wątki
void* find_primes(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    fprintf(data->log_file, "Thread %d started.\n", data->thread_id);

    for (int i = data->lower_bound; i <= data->upper_bound; ++i) {
        if (is_prime(i)) {
            pthread_mutex_lock(data->mutex);
            fprintf(data->log_file, "Thread %d found prime: %d\n", data->thread_id, i);
            data->primes_found[data->primes_count++] = i;
            data->primes_found_count++; 
            pthread_mutex_unlock(data->mutex);
        }
    }

    
    pthread_exit(NULL);
}

// Porównywanie dwóch liczb dla sortowania malejącego
int compare_desc(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <num_threads> <lower_bound> <upper_bound>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int lower_bound = atoi(argv[2]);
    int upper_bound = atoi(argv[3]);

    if (num_threads <= 0 || num_threads > MAX_THREADS || lower_bound >= upper_bound) {
        printf("Invalid input.\n");
        return 1;
    }

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    FILE* log_file = fopen("primes_log.txt", "w");

    if (!log_file) {
        printf("Error opening log file.\n");
        return 1;
    }

    // Obliczanie liczby liczb pierwszych w zakresie
    int count = 0;
    for (int i = lower_bound; i <= upper_bound; ++i) {
        if (is_prime(i)) {
            count++;
        }
    }

    // Obliczanie zakresów dla każdego wątku
    int primes_per_thread = count / num_threads;
    int remaining_primes = count % num_threads;
    int current_lower_bound = lower_bound;
    int current_upper_bound = lower_bound - 1;
    for (int i = 0; i < num_threads; ++i) {
        current_lower_bound = current_upper_bound + 1;
        current_upper_bound = current_lower_bound - 1;
        int primes_to_find = primes_per_thread;
        if (remaining_primes > 0) {
            primes_to_find++;
            remaining_primes--;
        }
        while (primes_to_find > 0) {
            current_upper_bound++;
            if (is_prime(current_upper_bound)) {
                primes_to_find--;
            }
        }
        thread_data[i].thread_id = i + 1;
        thread_data[i].lower_bound = current_lower_bound;
        thread_data[i].upper_bound = current_upper_bound;
        thread_data[i].primes_found = malloc((current_upper_bound - current_lower_bound + 1) * sizeof(int));
        thread_data[i].primes_count = 0;
        thread_data[i].primes_found_count = 0;
        thread_data[i].log_file = log_file;
        thread_data[i].mutex = &mutex;

        pthread_create(&threads[i], NULL, find_primes, (void*)&thread_data[i]);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        
    }

    fclose(log_file);

    // Print all found primes in descending order
    printf("All primes found:\n");
    int total_primes_found = 0;
    for (int i = 0; i < num_threads; ++i) {
        total_primes_found += thread_data[i].primes_count;
    }
    int* all_primes = malloc(total_primes_found * sizeof(int));
    int index = 0;
    for (int i = 0; i < num_threads; ++i) {
        for (int j = 0; j < thread_data[i].primes_count; ++j) {
            all_primes[index++] = thread_data[i].primes_found[j];
        }
        free(thread_data[i].primes_found); // Zwolnienie pamięci zaalokowanej dla tablicy znalezionych liczb pierwszych w danym wątku
    }

    qsort(all_primes, total_primes_found, sizeof(int), compare_desc);

    for (int i = 0; i < total_primes_found; ++i) {
        printf("%d\n", all_primes[i]);
    }

    free(all_primes);

    puts("\nStatystics:");
    for(int i = 0;i<num_threads;i++){
        printf("Thread %d found %d primes.\n", i + 1, thread_data[i].primes_found_count);
    }

    return 0;
}
