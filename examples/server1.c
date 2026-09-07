#include <stdio.h>
#include <string.h>

#define SWRAP_IMPLEMENTATION
#include "../swrap.h"

int main() {
    swrapInit();

    // Create a TCP server socket bound to port 8080
    int server_sock = swrapSocket(SWRAP_TCP, SWRAP_BIND, SWRAP_DEFAULT, NULL, "8080");
    if (server_sock == -1) {
        printf("Failed to create server socket.\n");
        return 1;
    }

    swrapListen(server_sock, 5);
    printf("Server listening on port 8080...\n");

    struct swrap_addr client_addr;
    int client_sock = swrapAccept(server_sock, &client_addr);
    if (client_sock != -1) {
    
        char host[64], porta[16];
        swrapAddressInfo(&client_addr, host, sizeof(host), porta, sizeof(porta));
        printf("Cliente conectado do IP: %s na porta %s\n", host, porta);
        
        char buffer[128] = {0};
        int bytes = swrapReceive(client_sock, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            printf("Received: %s\n", buffer);
            swrapSend(client_sock, "Hello from Server!", 18);
        }
        swrapClose(client_sock);
    }

    swrapClose(server_sock);
    swrapTerminate();
    return 0;
}
