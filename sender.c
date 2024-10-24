#include <windows.h>
#include <stdio.h>
#include <string.h>

#define PIPE_NAME "\\\\.\\pipe\\my_pipe"
#define BUFFER_SIZE 256

int main() {
    HANDLE hPipe;
    char message[BUFFER_SIZE];
    DWORD bytesWritten;
    
    printf("IPC Sender Program Starting...\n");
    printf("Waiting to connect to receiver...\n");

    // Connect to the pipe
    while (1) {
        hPipe = CreateFileA(
            PIPE_NAME,           // pipe name
            GENERIC_WRITE,       // write access
            0,                   // no sharing
            NULL,               // default security attributes
            OPEN_EXISTING,      // opens existing pipe
            0,                  // default attributes
            NULL                // no template file
        );

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY) {
            printf("Could not open pipe. Error: %d\n", GetLastError());
            printf("Make sure the receiver is running first!\n");
            return 1;
        }

        if (!WaitNamedPipeA(PIPE_NAME, 5000)) {
            printf("Could not open pipe: 5 second wait timed out.\n");
            printf("Make sure the receiver is running first!\n");
            return 1;
        }
    }

    printf("Connected to receiver successfully!\n");
    printf("You can now start sending messages.\n");
    printf("Type 'exit' to quit the program.\n\n");

    while (1) {
        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);
        
        // Remove trailing newline
        message[strcspn(message, "\n")] = 0;

        if (strcmp(message, "exit") == 0) {
            printf("Exiting program...\n");
            break;
        }

        // Write to pipe
        if (!WriteFile(
            hPipe,              // pipe handle
            message,            // message
            strlen(message),    // message length
            &bytesWritten,      // bytes written
            NULL                // not overlapped
        )) {
            printf("WriteFile failed. Error: %d\n", GetLastError());
            break;
        }

        printf("Message sent successfully!\n\n");
    }

    CloseHandle(hPipe);
    return 0;
}