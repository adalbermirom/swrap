// Salve como: multiselect_test.c
#include <stdio.h>
#include <string.h>

#define SWRAP_IMPLEMENTATION
#include "swrap.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s [server|client]\n", argv[0]);
        return 1;
    }

    swrapInit();

    if (strcmp(argv[1], "server") == 0) {
        int srv_sock = swrapSocket(SWRAP_TCP, SWRAP_BIND, SWRAP_NOBLOCK, NULL, "7070");
        swrapListen(srv_sock, 5);
        printf("[MultiSelect Server] Rodando (timeout de 2 segundos). Abra outro terminal e rode o cliente.\n");

        int active_sockets[3] = {srv_sock, -1, -1};
        
        while (1) {
            // A sua função modificada retorna o handle do socket exato
            int ready_sock = swrapMultiSelect(active_sockets, 3, 2.0);
            
            if (ready_sock == 0) {
                printf("[Tick] Nenhuma atividade. Event Loop girando...\n");
            } else if (ready_sock == srv_sock) {
                // É o socket principal? Então é uma nova conexão
                struct swrap_addr addr;
                int new_client = swrapAccept(srv_sock, &addr);
                printf("[Servidor] Novo cliente conectado! (Socket Handle: %d)\n", new_client);
                
                // Adiciona na lista de monitoramento
                if (active_sockets[1] == -1) active_sockets[1] = new_client;
                else if (active_sockets[2] == -1) active_sockets[2] = new_client;
                
            } else if (ready_sock > 0) {
                // É um socket de cliente! Recebendo dados
                char buffer[128] = {0};
                int bytes = swrapReceive(ready_sock, buffer, sizeof(buffer));
                
                if (bytes <= 0) {
                    printf("[Servidor] Cliente (Socket %d) desconectou.\n", ready_sock);
                    swrapClose(ready_sock);
                    if (active_sockets[1] == ready_sock) active_sockets[1] = -1;
                    if (active_sockets[2] == ready_sock) active_sockets[2] = -1;
                } else {
                    printf("[Servidor] Dados do Socket %d: %s\n", ready_sock, buffer);
                    swrapSend(ready_sock, "ACK\n", 4);
                }
            }
        }
    } else if (strcmp(argv[1], "client") == 0) {
        int sock = swrapSocket(SWRAP_TCP, SWRAP_CONNECT, SWRAP_DEFAULT, "127.0.0.1", "7070");
        if (sock == -1) return 1;
        
        printf("[Cliente] Pressione ENTER para enviar mensagem...");
        getchar();
        swrapSend(sock, "Teste de MultiSelect", 20);
        
        char buffer[32] = {0};
        swrapReceive(sock, buffer, sizeof(buffer));
        printf("[Cliente] Recebeu: %s\n", buffer);
        
        swrapClose(sock);
    }

    swrapTerminate();
    return 0;
}
