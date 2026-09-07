// Save as: multiselect_test.c
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
        int srv_sock = swrapSocket(SWRAP_TCP, SWRAP_BIND, SWRAP_NOBLOCK, NULL, "7070");
        swrapListen(srv_sock, 5);
        printf("[MultiSelect Server] Running (2-second timeout). Open another terminal and run the client.\n");

        int active_sockets[3] = {srv_sock, -1, -1};
        
        while (1) {
            // Your modified function returns the exact socket handle
            int ready_sock = swrapMultiSelect(active_sockets, 3, 2.0);
            
            if (ready_sock == 0) {
                printf("[Tick] No activity. Event Loop spinning...\n");
            } else if (ready_sock == srv_sock) {
                // Main listener socket? Handles a new incoming connection
                struct swrap_addr addr;
                int new_client = swrapAccept(srv_sock, &addr);
                printf("[Server] New client connected! (Socket Handle: %d)\n", new_client);
                
                // Add to the monitoring list
                if (active_sockets[1] == -1) active_sockets[1] = new_client;
                else if (active_sockets[2] == -1) active_sockets[2] = new_client;
                
            } else if (ready_sock > 0) {
                // Client socket ready! Receiving data
                char buffer[128] = {0};
                int bytes = swrapReceive(ready_sock, buffer, sizeof(buffer));
                
                if (bytes <= 0) {
                    printf("[Server] Client (Socket %d) disconnected.\n", ready_sock);
                    swrapClose(ready_sock);
                    if (active_sockets[1] == ready_sock) active_sockets[1] = -1;
                    if (active_sockets[2] == ready_sock) active_sockets[2] = -1;
                } else {
                    printf("[Server] Data from Socket %d: %s\n", ready_sock, buffer);
                    swrapSend(ready_sock, "ACK\n", 4);
                }
            }
        }
    } else if (strcmp(argv[1], "client") == 0) {
        int sock = swrapSocket(SWRAP_TCP, SWRAP_CONNECT, SWRAP_DEFAULT, "127.0.0.1", "7070");
        if (sock == -1) return 1;
        
        printf("[Client] Press ENTER to send a message...");
        getchar();
        swrapSend(sock, "MultiSelect Test", 16);
        
        char buffer[32] = {0};
        swrapReceive(sock, buffer, sizeof(buffer));
        printf("[Client] Received: %s\n", buffer);
        
        swrapClose(sock);
    }

    swrapTerminate();
    return 0;
}
