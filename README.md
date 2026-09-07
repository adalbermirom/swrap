# README.md

## Documentation

This is a header-only library, as such most of its functional documentation is contained within the "header section" of the
source code in the form of comments. It is highly recommended that you read said documentation before using this library.

## Features

The swrap library provides a cross-platform socket wrapper with automatic protocol-agnosticity, its features include:

- Encapsulates platform-specific socket implementations for both Windows and POSIX systems
- Future-proof protocol-agnosticity thanks to the system-internal getaddrinfo function
- Includes select and multi-select functions with timeouts in double seconds

## Improvements in this Fork

This fork introduces several critical bug fixes and API additions:

- **Socket Introspection (`swrapGetType`):** Added capability to validate and query socket protocol types (`SWRAP_TCP`, `SWRAP_UDP`).
- **Thread-Safe Error Reporting (`swrapGetLastSocketError`):** Cross-platform, thread-safe function to retrieve human-readable socket error messages.
- **Memory Leak Fixes:** Resolved missing freeaddrinfo calls when socket creation or binding fails.
- **Improved Alignment:** Updated swrap_addr memory layout to guarantee 64-bit alignment across different platforms (ARM/x86).
- **POSIX fcntl Fix:** Corrected non-blocking flag application on POSIX systems by preserving existing file status flags.
- **Robust DNS Resolution:** Enhanced swrapSocket to iterate over linked getaddrinfo results for IPv4/IPv6 dual-stack support.
- **Enhanced swrapMultiSelect:** Improved socket handle lookup and added guard checks against invalid socket descriptors.

---

## API Additions

```C 
// Query socket protocol type (returns SWRAP_TCP, SWRAP_UDP, or -1 on invalid handle)
SWDEF int swrapGetType(int sock);

// Populate buffer with last thread-safe socket error message
SWDEF void swrapGetLastSocketError(char* buf, size_t buf_size);
``` 

---

## Quick Start Examples

### 1. TCP Echo Server

```C
#include <stdio.h>
#include <string.h>

#define SWRAP_IMPLEMENTATION
#include "swrap.h"

int main() {
    swrapInit();

    // Create a TCP server socket bound to port 8080
    int server_sock = swrapSocket(SWRAP_TCP, SWRAP_BIND, SWRAP_DEFAULT, NULL, "8080");
    if (server_sock == -1) {
        char err[256];
        swrapGetLastSocketError(err, sizeof(err));
        printf("Failed to create server socket: %s\n", err);
        return 1;
    }

    // Verify socket type
    if (swrapGetType(server_sock) == SWRAP_TCP) {
        printf("Socket validated as TCP.\n");
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
``` 

### 2. TCP Client

```C
#include <stdio.h>
#include <string.h>

#define SWRAP_IMPLEMENTATION
#include "swrap.h"

int main() {
    swrapInit();

    // Connect to localhost on port 8080
    int sock = swrapSocket(SWRAP_TCP, SWRAP_CONNECT, SWRAP_DEFAULT, "127.0.0.1", "8080");
    if (sock == -1) {
        char err[256];
        swrapGetLastSocketError(err, sizeof(err));
        printf("Failed to connect to server: %s\n", err);
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
``` 

---

## Attribution

You are not required to give attribution when using this library. If you want to give attribution anyway, either link to
this repository, [my website](https://www.slopegames.com/), or credit me as [BareRose](https://github.com/BareRose).
If you want to support me financially, consider giving to my [Patreon](https://www.patreon.com/slopegames).

## License

Licensed under CC0 aka the most lawyer-friendly way of spelling "public domain".
