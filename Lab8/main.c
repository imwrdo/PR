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
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    int fifo = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fifo, &readfds);
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int ret = select(fifo + 1, &readfds, NULL, NULL, &timeout);
        if (ret == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            // No data to read, continue loop
            continue;
        }

        char command[100];
        int bytes_read = read(fifo, command, sizeof(command));
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (bytes_read == 0) {
            // No data was read, continue loop
            continue;
        }

        command[bytes_read] = '\0'; // Null-terminate the command string

        if (strncmp(command, "koniec", 6) == 0) {
            break;
        }

        if (strncmp(command, "szukaj", 6) == 0) {
            Task task;
            int K;
            sscanf(command, "szukaj [%d;%d] %d", &task.min, &task.max, &K);

            int fifo_write = open(FIFO_NAME, O_WRONLY);
            if (fifo_write == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            printf("Creating %d child processes to search for primes in range [%d;%d]\n", K, task.min, task.max);

            for (int i = 0; i < K; ++i) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    Task subtask;
                    subtask.min = task.min + (task.max - task.min + 1) * i / K;
                    subtask.max = task.min + (task.max - task.min + 1) * (i + 1) / K - 1;

                    ThreadResult* result = find_primes_range(&subtask);

                    int write_fd = open(FIFO_NAME, O_WRONLY);
                    if (write_fd == -1) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }

                    printf("Child %d: Writing primes to FIFO\n", i);
                    if (write(write_fd, result->primes, result->count * sizeof(int)) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }

                    free(result->primes);
                    free(result);
                    close(write_fd);
                    exit(EXIT_SUCCESS);
                }
            }

            for (int i = 0; i < K; ++i) {
                wait(NULL);
            }

            close(fifo_write);

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
