#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include <tlhelp32.h>

void display_submenu() {
    printf("\nSubmenu:\n");
    printf("a. Utworz nowy watek\n");
    printf("b. Usun wybrany watek\n");
    printf("c. Zmien priorytet wybranego watku\n");
    printf("d. Wyswietl liste watkow\n");
    printf("e. Zakoncz proces\n");
    printf("Wybierz opcje: ");
}

void thread_function(void* arg) {
    int* param = (int*)arg;
    printf("Thread started with param: %d\n", *param);
    Sleep(60000); // Symulacja obliczeń trwających około 1 minuty
    printf("Thread finished.\n");
    _endthread();
}

void create_new_thread(int priority, int param) {
    uintptr_t threadHandle = _beginthread(thread_function, 0, &param);
    if (threadHandle == -1) {
        printf("_beginthread failed.\n");
        return;
    }

    HANDLE hThread = (HANDLE)threadHandle;

    if (!SetThreadPriority(hThread, priority)) {
        printf("SetThreadPriority failed (%d).\n", GetLastError());
        return;
    }

    CloseHandle(hThread);
}

void remove_thread(DWORD threadId) {
    HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, threadId);
    if (hThread == NULL) {
        printf("OpenThread failed (%d).\n", GetLastError());
        return;
    }

    if (!TerminateThread(hThread, 0)) {
        printf("TerminateThread failed (%d).\n", GetLastError());
        return;
    }

    CloseHandle(hThread);
}

void display_thread_list(DWORD processId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot failed (%d).\n", GetLastError());
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(hSnapshot, &te32)) {
        printf("Thread32First failed (%d).\n", GetLastError());
        CloseHandle(hSnapshot);
        return;
    }

    printf("Thread List:\n");
    do {
        printf("Thread ID: %lu\n", te32.th32ThreadID);
    } while (Thread32Next(hSnapshot, &te32));

    CloseHandle(hSnapshot);
}

int main() {
    int option, priority, param;
    DWORD processId = GetCurrentProcessId();

    do {
        display_submenu();
        scanf(" %c", &option);

        switch (option) {
            case 'a':
                printf("Enter thread priority (0-31): ");
                scanf("%d", &priority);
                printf("Enter thread parameter: ");
                scanf("%d", &param);
                create_new_thread(priority, param);
                break;
            case 'b':
                printf("Enter Thread ID to remove: ");
                scanf("%lu", &processId); // W rzeczywistych warunkach należałoby pobrać ID wątku
                remove_thread(processId);
                break;
            case 'c':
                printf("Not implemented yet.\n");
                break;
            case 'd':
                display_thread_list(processId);
                break;
            case 'e':
                printf("Exiting process.\n");
                break;
            default:
                printf("Invalid option. Please try again.\n");
        }
    } while (option != 'e');

    return 0;
}
