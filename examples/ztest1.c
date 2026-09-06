// Salve como: tcp_test.c
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
        printf("[Servidor] Iniciando TCP na porta 8080...\n");
        int srv_sock = swrapSocket(SWRAP_TCP, SWRAP_BIND, SWRAP_DEFAULT, NULL, "8080");
        swrapListen(srv_sock, 5);
        
        struct swrap_addr client_addr;
        int client_sock = swrapAccept(srv_sock, &client_addr);
        
        char host[64], serv[16];
        swrapAddressInfo(&client_addr, host, sizeof(host), serv, sizeof(serv));
        printf("[Servidor] Cliente conectado de %s:%s\n", host, serv);
        
        
       

        // Loop continua lendo enquanto houver dados chegando
        char buffer[512];
        int bytes_lidos = swrapReceive(client_sock, buffer, sizeof(buffer) - 1);

        if (bytes_lidos > 0) {
            buffer[bytes_lidos] = '\0';
            printf("[Servidor] Recebido (%d bytes):\n%s\n", bytes_lidos, buffer);
            
            // Responde ao navegador para ele liberar a página
            const char* resposta = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Connection: close\r\n\r\n"
                "<h1>Servidor C funcionando com swrap!</h1>";
                
            swrapSend(client_sock, resposta, strlen(resposta));
        }
        
        swrapClose(client_sock);
        swrapClose(srv_sock);

    } else if (strcmp(argv[1], "client") == 0) {
        printf("[Cliente] Conectando ao servidor via TCP...\n");
        int sock = swrapSocket(SWRAP_TCP, SWRAP_CONNECT, SWRAP_DEFAULT, "127.0.0.1", "8080");
        
        if (sock == -1) {
            printf("[Cliente] Falha ao conectar.\n");
            return 1;
        }

        const char* msg = "Ola, Servidor TCP!";
        swrapSend(sock, msg, strlen(msg));
        
        char buffer[128] = {0};
        swrapReceive(sock, buffer, sizeof(buffer));
        printf("[Cliente] Resposta: %s\n", buffer);
        
        swrapClose(sock);
    }

    swrapTerminate();
    return 0;
}
