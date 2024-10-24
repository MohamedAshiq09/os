#include <windows.h>
#include <stdio.h>

#define PIPE_NAME "\\\\.\\pipe\\my_pipe"
#define BUFFER_SIZE 256

int main() {
    HANDLE hPipe;
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    printf("IPC Receiver Program Starting...\n");
    printf("Creating named pipe...\n");

    // Create the pipe
    hPipe = CreateNamedPipeA(
        PIPE_NAME,                   // pipe name
        PIPE_ACCESS_INBOUND,         // read access
        PIPE_TYPE_MESSAGE |          // message type pipe
        PIPE_READMODE_MESSAGE |      // message-read mode
        PIPE_WAIT,                   // blocking mode
        1,                          // max instances
        BUFFER_SIZE,                // output buffer size
        BUFFER_SIZE,                // input buffer size
        0,                          // client time-out
        NULL                        // default security attributes
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("CreateNamedPipe failed. Error: %d\n", GetLastError());
        return 1;
    }

    printf("Named pipe created successfully!\n");
    printf("Waiting for sender to connect...\n");

    // Wait for the client to connect
    if (!ConnectNamedPipe(hPipe, NULL)) {
        printf("ConnectNamedPipe failed. Error: %d\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    printf("Sender connected successfully!\n");
    printf("Waiting for messages...\n\n");

    // Read from pipe
    while (1) {
        if (ReadFile(
            hPipe,              // pipe handle
            buffer,             // buffer to receive data
            BUFFER_SIZE,        // size of buffer
            &bytesRead,         // number of bytes read
            NULL                // not overlapped
        )) {
            buffer[bytesRead] = '\0';
            printf("Message received: %s\n", buffer);
        } else {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                printf("\nSender disconnected.\n");
            } else {
                printf("\nReadFile failed. Error: %d\n", GetLastError());
            }
            break;
        }
    }

    CloseHandle(hPipe);
    printf("Press Enter to exit...");
    getchar();
    return 0;
}