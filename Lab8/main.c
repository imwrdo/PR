#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

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
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
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
            close(fifo);
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            // No data to read, continue loop
            continue;
        }

        char command[100];
        int bytes_read = read(fifo, command, sizeof(command) - 1); // leave space for null terminator
        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue; // No data available, continue loop
            } else {
                perror("read");
                close(fifo);
                exit(EXIT_FAILURE);
            }
        }

        if (bytes_read == 0) {
            // FIFO closed, reopen
            close(fifo);
            fifo = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
            if (fifo == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
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

            int pipes[K][2];
            for (int i = 0; i < K; ++i) {
                if (pipe(pipes[i]) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            printf("Creating %d child processes to search for primes in range [%d;%d]\n", K, task.min, task.max);

            for (int i = 0; i < K; ++i) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    close(pipes[i][0]);  // Close unused read end

                    Task subtask;
                    subtask.min = task.min + (task.max - task.min + 1) * i / K;
                    subtask.max = task.min + (task.max - task.min + 1) * (i + 1) / K - 1;

                    ThreadResult* result = find_primes_range(&subtask);

                    printf("Child %d: Writing primes to pipe\n", i);
                    if (write(pipes[i][1], result->primes, result->count * sizeof(int)) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }

                    free(result->primes);
                    free(result);
                    close(pipes[i][1]);  // Close write end after writing
                    exit(EXIT_SUCCESS);
                }
                close(pipes[i][1]);  // Parent closes unused write end
            }

            for (int i = 0; i < K; ++i) {
                wait(NULL);
            }

            printf("Liczby pierwsze w zakresie [%d;%d]: ", task.min, task.max);
            bool* primes_present = calloc(task.max - task.min + 1, sizeof(bool));
            if (primes_present == NULL) {
                perror("calloc");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < K; ++i) {
                int prime;
                while (read(pipes[i][0], &prime, sizeof(int)) > 0) {
                    if (!primes_present[prime - task.min]) {
                        printf("%d ", prime);
                        primes_present[prime - task.min] = true;
                    }
                }
                close(pipes[i][0]);  // Close read end after reading
            }
            printf("\n");

            free(primes_present);
        }
    }

    close(fifo);
    unlink(FIFO_NAME);

    return 0;
}
