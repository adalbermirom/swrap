// Save as: zudp_test.c
#include <stdio.h>
#include <string.h>

#define SWRAP_IMPLEMENTATION
#include "swrap.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s [server|client]\n", argv[0]);
        return 1;
    }

    swrapInit();

    if (strcmp(argv[1], "server") == 0) {
        printf("[UDP Server] Waiting for datagrams on port 9090...\n");
        int sock = swrapSocket(SWRAP_UDP, SWRAP_BIND, SWRAP_DEFAULT, NULL, "9090");
        
        struct swrap_addr sender_addr;
        char buffer[128] = {0};
        
        // Receives packet and populates sender_addr (Testing struct alignment)
        swrapReceiveFrom(sock, &sender_addr, buffer, sizeof(buffer));
        printf("[UDP Server] Received: %s\n", buffer);
        
        const char* response = "PONG";
        // Tests swrapSendTo passing the aligned struct back
        swrapSendTo(sock, &sender_addr, response, strlen(response));
        
        swrapClose(sock);

    } else if (strcmp(argv[1], "client") == 0) {
        printf("[UDP Client] Sending PING to port 9090...\n");
        // UDP Client uses CONNECT in swrap solely to set the default target address
        int sock = swrapSocket(SWRAP_UDP, SWRAP_CONNECT, SWRAP_DEFAULT, "127.0.0.1", "9090");
        
        const char* msg = "PING";
        swrapSend(sock, msg, strlen(msg));
        
        char buffer[128] = {0};
        swrapReceive(sock, buffer, sizeof(buffer));
        printf("[UDP Client] Response: %s\n", buffer);
        
        swrapClose(sock);
    }

    swrapTerminate();
    return 0;
}
