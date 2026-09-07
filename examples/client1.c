#include <stdio.h>
#include <string.h>

#define SWRAP_IMPLEMENTATION
#include "../swrap.h"

int main() {
    swrapInit();

    // Connect to localhost on port 8080
    int sock = swrapSocket(SWRAP_TCP, SWRAP_CONNECT, SWRAP_DEFAULT, "127.0.0.1", "8080");
    if (sock == -1) {
        printf("Failed to connect to server.\n");
        return 1;
    }

    const char* msg = "Hello from Client!";
    swrapSend(sock, msg, strlen(msg));

    char buffer[128] = {0};
    int bytes = swrapReceive(sock, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        printf("Server response: %s\n", buffer);
    }

    swrapClose(sock);
    swrapTerminate();
    return 0;
}
