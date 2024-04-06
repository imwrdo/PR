#include <stdio.h>
#include <stdlib.h>
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

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void thread_function(void* arg) {
    int* params = (int*)arg;
    int num1 = params[0];
    int num2 = params[1];
    Sleep(60000);
    printf("\nCalculating GCD of %d and %d\n", num1, num2);
    int result = gcd(num1, num2);
    printf("\nGCD of %d and %d is %d\n", num1, num2, result);
    _endthread();
}

void create_new_thread(int priority, int num1, int num2) {
    int minPriority = THREAD_PRIORITY_IDLE;
    int maxPriority = THREAD_PRIORITY_TIME_CRITICAL;

    if (priority < minPriority || priority > maxPriority) {
        printf("Invalid thread priority. Priority must be in the range of %d to %d.\n", minPriority, maxPriority);
        return;
    }
    int* params = malloc(2 * sizeof(int));
    if (params == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    params[0] = num1;
    params[1] = num2;

    uintptr_t threadHandle = _beginthread(thread_function, 0, params);
    if (threadHandle == -1) {
        printf("_beginthread failed.\n");
        free(params);
        return;
    }

    HANDLE hThread = (HANDLE)threadHandle;

    if (!SetThreadPriority(hThread, priority)) {
        printf("SetThreadPriority failed (%d).\n", GetLastError());
        free(params);
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
        if (te32.th32OwnerProcessID == processId) {
            HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
            if (hThread == NULL) {
                printf("OpenThread failed (%d).\n", GetLastError());
                continue;
            }
            int priority = GetThreadPriority(hThread);
            printf("Thread ID: %lu, Priority: %d\n", te32.th32ThreadID, priority);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hSnapshot, &te32));

    CloseHandle(hSnapshot);
}
void change_thread_priority(DWORD threadId, int newPriority) {
    HANDLE hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, threadId);
    if (hThread == NULL) {
        printf("OpenThread failed (%d).\n", GetLastError());
        return;
    }

    if (!SetThreadPriority(hThread, newPriority)) {
        printf("SetThreadPriority failed (%d).\n", GetLastError());
        return;
    }

    CloseHandle(hThread);
}

int main() {
    int option, priority, num1, num2;
    DWORD processId = GetCurrentProcessId();
    DWORD threadId;
    do {
        display_submenu();
        scanf(" %c", &option);

        switch (option) {
            case 'a':
                printf("Enter thread priority (0-31): ");
                scanf("%d", &priority);
                printf("Enter first number: ");
                scanf("%d", &num1);
                printf("Enter second number: ");
                scanf("%d", &num2);
                create_new_thread(priority, num1, num2);
                break;
            case 'b':
                printf("Enter Thread ID to remove: ");
                scanf("%lu", &threadId); 
                remove_thread(threadId);
                break;
            case 'c':
                printf("Enter Thread ID to change priority: ");
                scanf("%lu", &threadId);
                printf("Enter new priority (0-31): ");
                scanf("%d", &priority);
                change_thread_priority(threadId, priority);
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
