#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMANDS 10
#define MAX_COMMAND_LENGTH 100

char commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
pid_t pids[MAX_COMMANDS];
int num_commands = 0;

void add_command(const char *command) {
    if (num_commands < MAX_COMMANDS) {
        strcpy(commands[num_commands], command);
        num_commands++;
    } else {
        printf("Nie można dodać więcej poleceń.\n");
    }
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
            printf("Błąd podczas tworzenia procesu potomnego.\n");
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
                char new_command[MAX_COMMAND_LENGTH];
                printf("Podaj nowe polecenie: ");
                scanf("%s", new_command);
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
                return 0;
            default:
                printf("Nieprawidłowy wybór. Wybierz opcję od 1 do 3.\n");
        }
    }

    return 0;
}
