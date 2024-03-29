#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <tlhelp32.h>

void display_menu() {
    printf("Menu:\n");
    printf("1. Stworz nowy proces\n");
    printf("2. Zatrzymaj wybrany proces\n");
    printf("3. Wyswietl liste procesow\n");
    printf("4. Zakoncz program\n");
    printf("Wybierz opcje: ");
}

void create_new_process() {
    char exe_path[] = "ProgramPodrzedny.exe";
    STARTUPINFO structStartupInfo;
    PROCESS_INFORMATION structProcInfo;

    GetStartupInfo(&structStartupInfo);

    if (!CreateProcess(NULL, exe_path, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &structStartupInfo, &structProcInfo)) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }
}

void stop_process(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess == NULL) {
        printf("OpenProcess failed (%d).\n", GetLastError());
        return;
    }

    if (!TerminateProcess(hProcess, 0)) {
        printf("TerminateProcess failed (%d).\n", GetLastError());
        return;
    }

    CloseHandle(hProcess);
}

void display_process_list() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot failed (%d).\n", GetLastError());
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        printf("Process32First failed (%d).\n", GetLastError());
        CloseHandle(hSnapshot);
        return;
    }

    printf("Process List:\n");
    do {
        printf("Process ID: %lu, Name: %s\n", pe32.th32ProcessID, pe32.szExeFile);
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}

int main() {
    int option;
    DWORD processId;

    do {
        display_menu();
        scanf("%d", &option);

        switch (option) {
            case 1:
                create_new_process();
                break;
            case 2:
                printf("Enter Process ID to stop: ");
                scanf("%lu", &processId);
                stop_process(processId);
                break;
            case 3:
                display_process_list();
                break;
            case 4:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid option. Please try again.\n");
        }
    } while (option != 4);

    return 0;
}
