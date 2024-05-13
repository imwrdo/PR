#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>

#define FIFO_NAME "potok_WEJSCIOWY"
#define MAX_THREADS 100

typedef struct {
    int min;
    int max;
} Task;

typedef struct {
    int* primes;
    int count;
} ThreadResult;

int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void* find_primes_range(void* arg) {
    Task* task = (Task*) arg;

    ThreadResult* result = malloc(sizeof(ThreadResult));
    if (result == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    result->primes = malloc((task->max - task->min + 1) * sizeof(int));
    if (result->primes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    result->count = 0;

    for (int i = task->min; i <= task->max; ++i) {
        if (is_prime(i)) {
            result->primes[result->count++] = i;
        }
    }

    return result;
}

int main() {
    mkfifo(FIFO_NAME, 0666);

    int fifo = open(FIFO_NAME, O_RDONLY); // Otwarcie potoku do czytania
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fifo, &readfds);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int ret = select(fifo + 1, &readfds, NULL, NULL, &timeout);
        if (ret == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            // Brak danych do odczytu, kontynuuj pętlę
            continue;
        }

        char command[100];
        int bytes_read = read(fifo, command, sizeof(command));
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (strncmp(command, "koniec", 6) == 0) {
            break;
        }

        if (strncmp(command, "szukaj", 6) == 0) {
            Task task;
            int K;
            sscanf(command, "szukaj [%d;%d] %d", &task.min, &task.max, &K);

            // Otwarcie potoku do zapisu
            int fifo_write = open(FIFO_NAME, O_WRONLY);
            if (fifo_write == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            // Tworzenie K pod-procesów
            for (int i = 0; i < K; ++i) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) { // Proces potomny
                    Task subtask;
                    subtask.min = task.min + (task.max - task.min + 1) * i / K;
                    subtask.max = task.min + (task.max - task.min + 1) * (i + 1) / K - 1;

                    pthread_t threads[MAX_THREADS];
                    int thread_count = 0;

                    // Tworzenie wątków dla pod-zakresu liczb
                    for (int j = subtask.min; j <= subtask.max; ++j) {
                        pthread_t thread;
                        if (pthread_create(&thread, NULL, find_primes_range, &subtask) != 0) {
                            perror("pthread_create");
                            exit(EXIT_FAILURE);
                        }
                        threads[thread_count++] = thread;
                    }

                    // Oczekiwanie na zakończenie wątków
                    for (int j = 0; j < thread_count; ++j) {
                        ThreadResult* result;
                        if (pthread_join(threads[j], (void**)&result) != 0) {
                            perror("pthread_join");
                            exit(EXIT_FAILURE);
                        }

                        // Wysłanie wyników przez potok do procesu nadrzędnego
                        if (write(fifo_write, result->primes, result->count * sizeof(int)) == -1) {
                            perror("write");
                            exit(EXIT_FAILURE);
                        }

                        free(result->primes);
                        free(result);
                    }

                    close(fifo_write);
                    exit(EXIT_SUCCESS);
                }
            }

            close(fifo_write);

            // Odczytywanie wyników z potoku od pod-procesów
            printf("Liczby pierwsze w zakresie [%d;%d]: ", task.min, task.max);
            bool* primes_present = calloc(task.max - task.min + 1, sizeof(bool));
            if (primes_present == NULL) {
                perror("calloc");
                exit(EXIT_FAILURE);
            }

            int prime;
            while (read(fifo, &prime, sizeof(int)) > 0) {
                if (!primes_present[prime - task.min]) {
                    printf("%d ", prime);
                    primes_present[prime - task.min] = true;
                }
            }
            printf("\n");

            free(primes_present);
        }
    }

    close(fifo);
    unlink(FIFO_NAME);

    return 0;
}
