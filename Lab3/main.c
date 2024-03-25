#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define COMMAND_LENGTH 100

char **commands = NULL;
pid_t *pids = NULL;
int num_commands = 0;
int max_commands = 0;

void add_command(const char *command) {
    if (num_commands >= max_commands) {
        max_commands += 10;  // Increase max_commands by 10
        commands = realloc(commands, max_commands * sizeof(char*));
        pids = realloc(pids, max_commands * sizeof(pid_t));
        if (!commands || !pids) {
            fprintf(stderr, "Błąd alokacji pamięci!\n");
            exit(EXIT_FAILURE);
        }
    }

    commands[num_commands] = strdup(command);
    if (!commands[num_commands]) {
        fprintf(stderr, "Błąd alokacji pamięci!\n");
        exit(EXIT_FAILURE);
    }
    num_commands++;
}

void add_pid(pid_t pid) {
    pids[num_commands - 1] = pid;
}

void wait_for_processes() {
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

void run_all_commands() {
    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            system(commands[i]);
            exit(0);
        } else if (pid > 0) {
            add_pid(pid);
        } else {
            fprintf(stderr, "Błąd podczas tworzenia procesu potomnego.\n");
        }
    }
}

void display_menu() {
    printf("Menu:\n");
    printf("1. Dodaj własne polecenie\n");
    printf("2. Uruchom wszystkie polecenia\n");
    printf("3. Wyjdź\n");
}

int main() {
    commands = malloc(sizeof(char*) * 10);  // Initial allocation for 10 commands
    pids = malloc(sizeof(pid_t) * 10);      // Initial allocation for 10 pids
    if (!commands || !pids) {
        fprintf(stderr, "Błąd alokacji pamięci!\n");
        exit(EXIT_FAILURE);
    }
    max_commands = 10;
    add_command("wget -O file.txt https://pl.wikipedia.org/wiki/Wikipedia:Strona_g%C5%82%C3%B3wna");
    add_command("ping -c 10 pg.edu.pl");
    add_command("sleep 10");
    add_command("uname -a");
    add_command("ps aux");
    while (1) {
        display_menu();

        printf("Wybierz opcję: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char new_command[COMMAND_LENGTH];
                printf("Podaj nowe polecenie: ");
                scanf(" %[^\n]", new_command);
                add_command(new_command);
                break;
            }
            case 2:
                if (num_commands == 0) {
                    printf("Nie dodano jeszcze żadnych poleceń.\n");
                } else {
                    printf("Uruchamianie wszystkich poleceń...\n");
                    run_all_commands();
                    wait_for_processes();
                    printf("Wszystkie procesy zakończone.\n");
                    return 0;
                }
                break;
            case 3:
                for (int i = 0; i < num_commands; i++) {
                    free(commands[i]);
                }
                free(commands);
                free(pids);
                return 0;
            default:
                printf("Nieprawidłowy wybór. Wybierz opcję od 1 do 3.\n");
        }
    }

    return 0;
}
