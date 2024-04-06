#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>

#define MAX_THREADS 10

typedef struct {
    HANDLE handle;
    DWORD id;
    int priority;
    BOOL finished; // Indicates whether the thread has finished
    BOOL removed;  // Indicates whether the thread has been removed
    CRITICAL_SECTION cs; // Critical section for synchronization
} ThreadInfo;

ThreadInfo threads[MAX_THREADS];
int num_threads = 0;

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

DWORD WINAPI thread_function(LPVOID arg) {
    int* params = (int*)arg;
    int num1 = params[0];
    int num2 = params[1];
    Sleep(60000);
    int threadIndex = -1; // Variable to store the index of the thread in the array
    // Find the thread index
    for (int i = 0; i < num_threads; i++) {
        if (threads[i].id == GetCurrentThreadId()) {
            threadIndex = i;
            break;
        }
    }
    if (threadIndex != -1 && !threads[threadIndex].removed) {
        printf("\nCalculating GCD of %d and %d\n", num1, num2);
        int result = gcd(num1, num2);
        printf("\nGCD of %d and %d is %d\n", num1, num2, result);
    }
    free(params); // Free the allocated memory
    return 0;
}


int create_new_thread(int priority, int num1, int num2) {
    if (num_threads >= MAX_THREADS) {
        printf("Maximum number of threads reached.\n");
        return 1;
    }

    // Validate the priority range
    if (priority < THREAD_PRIORITY_IDLE || priority > THREAD_PRIORITY_TIME_CRITICAL) {
        printf("Invalid thread priority. Priority must be in the range of %d to %d.\n",
               THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL);
        return 1;
    }

    HANDLE threadHandle;
    int* params = malloc(2 * sizeof(int));
    if (params == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    params[0] = num1;
    params[1] = num2;

    InitializeCriticalSection(&threads[num_threads].cs); // Initialize critical section

    threadHandle = CreateThread(NULL, 0, thread_function, params, 0, &threads[num_threads].id);
    if (threadHandle == NULL) {
        printf("Failed to create thread. Error code: %d\n", GetLastError());
        free(params);
        DeleteCriticalSection(&threads[num_threads].cs); // Cleanup critical section
        return 1;
    }

    // Set the thread priority
    if (!SetThreadPriority(threadHandle, priority)) {
        printf("Failed to set thread priority. Error code: %d\n", GetLastError());
        CloseHandle(threadHandle);
        free(params);
        DeleteCriticalSection(&threads[num_threads].cs); // Cleanup critical section
        return 1;
    }

    // Store thread information
    threads[num_threads].handle = threadHandle;
    threads[num_threads].priority = priority;
    threads[num_threads].finished = FALSE; // Initialize finished flag
    threads[num_threads].removed = FALSE;  // Initialize removed flag
    num_threads++;
    printf("New thread created successfully.\n");
    return 0;
}

void remove_thread(int threadIndex) {
    if (threadIndex < 0 || threadIndex >= num_threads) {
        printf("Invalid thread index.\n");
        return;
    }

    // Set the removed flag
    threads[threadIndex].removed = TRUE;

    // Close the handle and remove from the list
    CloseHandle(threads[threadIndex].handle);
    for (int i = threadIndex; i < num_threads - 1; i++) {
        threads[i] = threads[i + 1];
    }
    num_threads--;
    printf("Thread removed successfully.\n");
}

void change_thread_priority(int threadIndex, int newPriority) {
    if (threadIndex < 0 || threadIndex >= num_threads) {
        printf("Invalid thread index.\n");
        return;
    }

    // Validate the priority range
    if (newPriority < THREAD_PRIORITY_IDLE || newPriority > THREAD_PRIORITY_TIME_CRITICAL) {
        printf("Invalid thread priority. Priority must be in the range of %d to %d.\n",
               THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL);
        return;
    }

    if (!SetThreadPriority(threads[threadIndex].handle, newPriority)) {
        printf("Failed to change thread priority. Error code: %d\n", GetLastError());
        return;
    }

    threads[threadIndex].priority = newPriority;
    printf("Thread priority changed successfully.\n");
}

void display_thread_list(DWORD processId) {
    printf("Thread List:\n");
    for (int i = 0; i < num_threads; i++) {
        printf("Thread ID: %lu, Priority: %d\n", threads[i].id, threads[i].priority);
    }
}

void cleanup_finished_threads() {
    for (int i = 0; i < num_threads; i++) {
        DWORD exitCode;
        if (GetExitCodeThread(threads[i].handle, &exitCode) && exitCode != STILL_ACTIVE) {
            CloseHandle(threads[i].handle);
            for (int j = i; j < num_threads - 1; j++) {
                threads[j] = threads[j + 1];
            }
            num_threads--;
            i--; // Adjust loop index as elements are shifted
        }
    }
}

int main() {
    int option, priority, num1, num2;
    do {
        display_submenu();
        scanf(" %c", &option);

        switch (option) {
            case 'a':
                printf("Enter thread priority: ");
                scanf("%d", &priority);
                printf("Enter first number: ");
                scanf("%d", &num1);
                printf("Enter second number: ");
                scanf("%d", &num2);
                create_new_thread(priority, num1, num2);
                break;
            case 'b':
                display_thread_list(GetCurrentProcessId());
                printf("Enter Thread ID to remove: ");
                scanf("%d", &num1); // Changed %lu to %d
                remove_thread(num1);
                break;
            case 'c':
                display_thread_list(GetCurrentProcessId());
                printf("Enter Thread index to change priority: ");
                scanf("%d", &num1); // Changed %lu to %d
                printf("Enter new priority : ");
                scanf("%d", &priority);
                change_thread_priority(num1, priority);
                break;
            case 'd':
                cleanup_finished_threads();
                display_thread_list(GetCurrentProcessId());
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
