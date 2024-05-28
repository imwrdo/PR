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
#include <pthread.h>

#define FIFO_NAME "potok_WEJSCIOWY"

typedef struct {
    int number;
    int write_fd;
} ThreadData;

int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void* check_prime(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    int number = data->number;
    int write_fd = data->write_fd;

    if (is_prime(number)) {
        if (write(write_fd, &number, sizeof(int)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    free(data);
    return NULL;
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
            int min, max, K;
            sscanf(command, "szukaj [%d;%d] %d", &min, &max, &K);

            int pipes[K][2];
            for (int i = 0; i < K; ++i) {
                if (pipe(pipes[i]) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            printf("Creating %d child processes to search for primes in range [%d;%d]\n", K, min, max);

            for (int i = 0; i < K; ++i) {
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) { // Child process
                    close(pipes[i][0]); // Close unused read end

                    int start = min + (max - min + 1) * i / K;
                    int end = min + (max - min + 1) * (i + 1) / K - 1;

                    pthread_t* threads = malloc((end - start + 1) * sizeof(pthread_t));
                    if (threads == NULL) {
                        perror("malloc");
                        exit(EXIT_FAILURE);
                    }

                    for (int j = start; j <= end; ++j) {
                        ThreadData* data = malloc(sizeof(ThreadData));
                        if (data == NULL) {
                            perror("malloc");
                            exit(EXIT_FAILURE);
                        }
                        data->number = j;
                        data->write_fd = pipes[i][1];

                        if (pthread_create(&threads[j - start], NULL, check_prime, data) != 0) {
                            perror("pthread_create");
                            exit(EXIT_FAILURE);
                        }
                    }

                    for (int j = start; j <= end; ++j) {
                        pthread_join(threads[j - start], NULL);
                    }

                    free(threads);
                    close(pipes[i][1]); // Close write end
                    exit(EXIT_SUCCESS);
                } else { // Parent process
                    close(pipes[i][1]); // Close unused write end
                }
            }

            for (int i = 0; i < K; ++i) {
                wait(NULL); // Wait for all child processes to finish
            }

            printf("Liczby pierwsze w zakresie [%d;%d]: ", min, max);
            bool* primes_present = calloc(max - min + 1, sizeof(bool));
            if (primes_present == NULL) {
                perror("calloc");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < K; ++i) {
                int prime;
                while (read(pipes[i][0], &prime, sizeof(int)) > 0) {
                    if (!primes_present[prime - min]) {
                        printf("%d ", prime);
                        primes_present[prime - min] = true;
                    }
                }
                close(pipes[i][0]); // Close read end
            }
            printf("\n");

            free(primes_present);
        }
    }

    close(fifo);
    unlink(FIFO_NAME);

    return 0;
}
