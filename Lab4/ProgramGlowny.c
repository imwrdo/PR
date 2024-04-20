#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>

#define MAX_PROCESSES 10 

DWORD processIds[MAX_PROCESSES]; 
int numProcesses = 0; 

char* get_current_executable_path() {
    static char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    return path;
}

void display_menu() {
    printf("Menu:\n");
    printf("1. Stworz nowy proces\n");
    printf("2. Zatrzymaj wybrany proces\n");
    printf("3. Wyswietl liste procesow\n");
    printf("4. Zakoncz program\n");
    printf("Wybierz opcje: ");
}

void stop_process(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (hProcess == NULL) {
        printf("OpenProcess failed (%ld).\n", GetLastError());
        return;
    }

    if (!TerminateProcess(hProcess, 0)) {
        printf("TerminateProcess failed (%ld).\n", GetLastError());
        return;
    }

    CloseHandle(hProcess);
}

BOOL is_process_from_this_application(DWORD processId) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess != NULL) {
        char path[MAX_PATH];
        if (GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH) > 0) {
            char* currentExecutablePath = get_current_executable_path();
            if (strcmp(path, currentExecutablePath) == 0) {
                CloseHandle(hProcess);
                return TRUE;
            }
        }
        if (GetModuleFileNameEx(hProcess, NULL, path, MAX_PATH) > 0) {
            
            if (strstr(path, "ProgramPodrzedny.exe") != NULL) {
                CloseHandle(hProcess);
                return TRUE;
            }
        }

        CloseHandle(hProcess);
    }
    return FALSE;
}

void display_process_list() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot failed (%ld).\n", GetLastError());
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        printf("Process32First failed (%ld).\n", GetLastError());
        CloseHandle(hSnapshot);
        return;
    }

    printf("\nProcess List:\n");
    do {
        if (is_process_from_this_application(pe32.th32ProcessID)) {
            printf("Process ID: %lu, Name: %s\n", pe32.th32ProcessID, pe32.szExeFile);
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}

void create_new_process() {
    char exe_path[] = "ProgramPodrzedny.exe";
    STARTUPINFO structStartupInfo;
    PROCESS_INFORMATION structProcInfo;

    GetStartupInfo(&structStartupInfo);

    if (!CreateProcess(NULL, exe_path, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &structStartupInfo, &structProcInfo)) {
        printf("CreateProcess failed (%ld).\n", GetLastError());
        return;
    }

    processIds[numProcesses++] = structProcInfo.dwProcessId;

    display_process_list();
    
    
    if(is_process_from_this_application(structProcInfo.dwProcessId)) {
        printf("New process belongs to this application.\n");
    } else {
        printf("New process does not belong to this application.\n");
    }
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
                printf("\nEnter Process ID to stop: ");
                scanf("%lu", &processId);
                stop_process(processId);
                printf("Process is stopped\n");
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

