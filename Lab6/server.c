// server.c
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_CLIENTS 10
#define RESULT_BUFFER_SIZE 4096

HANDLE hMutex;
FILE* sharedFile;
bool shutdownRequested = false;
SOCKET connectedClients[MAX_CLIENTS]; // Tablica przechowująca sockety klientów
int numClients = 0; // Liczba aktualnie podłączonych klientów

void handleClient(void* clientSocket) {
    SOCKET client = *((SOCKET*)clientSocket);
    free(clientSocket);
    char buffer[1024] = {0};
    int bytesReceived = recv(client, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        int start, end;
        sscanf(buffer, "%d %d", &start, &end);

        int i, j, isPrime;
        char result[RESULT_BUFFER_SIZE] = {0};
        Sleep(200*end);
        for (i = start; i <= end; i++) {
            if (i <= 1)
                continue;

            isPrime = 1;
            for (j = 2; j * j <= i; j++) {
                if (i % j == 0) {
                    isPrime = 0;
                    break;
                }
            }

            if (isPrime) {
                snprintf(result + strlen(result), RESULT_BUFFER_SIZE - strlen(result), "%d ", i);
            }
        }

        // Send result to client
        send(client, result, strlen(result), 0);

        if (strncmp(buffer, "Shutdown server", 15) == 0) {
            WaitForSingleObject(hMutex, INFINITE);
            fprintf(sharedFile, "Shutdown request received from client %lld.\n", client);
            fflush(sharedFile);
            ReleaseMutex(hMutex);

            shutdownRequested = true; // Ustawienie flagi zamknięcia serwera

            // Zamknij połączenia z innymi klientami
            for (int i = 0; i < numClients; i++) {
                if (connectedClients[i] != client) {
                    const char* shutdownMsg = "Server shutdown initiated. Closing connection.\n";
                    send(connectedClients[i], shutdownMsg, strlen(shutdownMsg), 0);
                    closesocket(connectedClients[i]);
                }
            }

            closesocket(client);
            exit(0);
        } else {
            const char* response = "Invalid request.\n";
            send(client, response, strlen(response), 0);
            closesocket(client);
        }
    } else {
        printf("Failed to receive data from client.\n");
        closesocket(client);
    }
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(clientAddr);
    HANDLE thread;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(serverSocket);
        return 1;
    }

    sharedFile = fopen("shared_file.txt", "a"); // Append mode to avoid overwriting
    if (sharedFile == NULL) {
        printf("Failed to create or open shared file.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        printf("Mutex initialization failed.\n");
        fclose(sharedFile);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Server initialized. Waiting for connections...\n");

    while (!shutdownRequested) { // Keep accepting connections until shutdown is requested
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen)) == INVALID_SOCKET) {
            printf("Accept failed.\n");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        if (numClients < MAX_CLIENTS) {
            connectedClients[numClients++] = clientSocket; // Dodanie nowego klienta do tablicy
        } else {
            printf("Maximum number of clients reached. Cannot accept new connections.\n");
            const char* maxClientsMsg = "Server is currently full. Please try again later.\n";
            send(clientSocket, maxClientsMsg, strlen(maxClientsMsg), 0);
            closesocket(clientSocket);
            continue;
        }

        SOCKET* clientSocketPtr = malloc(sizeof(SOCKET));
        if (clientSocketPtr == NULL) {
            printf("Memory allocation failed.\n");
            closesocket(clientSocket);
            continue;
        }
        *clientSocketPtr = clientSocket;

        thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handleClient, clientSocketPtr, 0, NULL);
        if (thread == NULL) {
            printf("Thread creation failed.\n");
            free(clientSocketPtr);
            closesocket(clientSocket);
            continue;
        }

        CloseHandle(thread);
    }

    // Zamknij połączenia z innymi klientami
    for (int i = 0; i < numClients; i++) {
        closesocket(connectedClients[i]);
    }

    fclose(sharedFile);
    CloseHandle(hMutex);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
