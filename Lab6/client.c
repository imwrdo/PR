#include <stdio.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

int main() {
    WSADATA wsa;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024] = {0};
    int choice;
    int start, end;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        return 1;
    }

    // Prepare the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Connection failed.\n");
        return 1;
    }

    printf("Connected to server.\n");

    // Menu for user choice
    printf("Menu:\n");
    printf("1. Perform time-consuming calculation (finding prime numbers).\n");
    printf("2. Shutdown server.\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        // Send request for time-consuming calculation
        printf("Enter the range (start end) for prime number calculation: ");
        scanf("%d %d", &start, &end);

        sprintf(buffer, "%d %d", start, end);
        send(clientSocket, buffer, strlen(buffer), 0);

        // Receive response
        recv(clientSocket, buffer, sizeof(buffer), 0);
        printf("Server response: %s\n", buffer);
    } else if (choice == 2) {
        // Send request to shutdown server
        const char* request = "Shutdown server.";
        send(clientSocket, request, strlen(request), 0);

        // Receive response
        recv(clientSocket, buffer, sizeof(buffer), 0);
        printf("Server response: %s\n", buffer);
    } else {
        printf("Invalid choice.\n");
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
