// Salve como: zudp_test.c
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
        printf("[Servidor UDP] Aguardando datagramas na porta 9090...\n");
        int sock = swrapSocket(SWRAP_UDP, SWRAP_BIND, SWRAP_DEFAULT, NULL, "9090");
        
        struct swrap_addr sender_addr;
        char buffer[128] = {0};
        
        // Recebe pacote e preenche sender_addr (Testando o alinhamento da struct)
        swrapReceiveFrom(sock, &sender_addr, buffer, sizeof(buffer));
        printf("[Servidor UDP] Recebido: %s\n", buffer);
        
        const char* response = "PONG";
        // Testa o swrapSendTo passando a struct alinhada de volta
        swrapSendTo(sock, &sender_addr, response, strlen(response));
        
        swrapClose(sock);

    } else if (strcmp(argv[1], "client") == 0) {
        printf("[Cliente UDP] Enviando PING para porta 9090...\n");
        // UDP Client usa CONNECT no swrap apenas para associar o endereço destino padrão
        int sock = swrapSocket(SWRAP_UDP, SWRAP_CONNECT, SWRAP_DEFAULT, "127.0.0.1", "9090");
        
        const char* msg = "PING";
        swrapSend(sock, msg, strlen(msg));
        
        char buffer[128] = {0};
        swrapReceive(sock, buffer, sizeof(buffer));
        printf("[Cliente UDP] Resposta: %s\n", buffer);
        
        swrapClose(sock);
    }

    swrapTerminate();
    return 0;
}
