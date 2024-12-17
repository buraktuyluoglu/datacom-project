# Quote of the Day - Client-Server Application in C

A simple client-server application that sends random inspirational quotes to connected clients, implemented in C.

## Requirements
- GCC compiler
- POSIX-compliant system (Linux/Unix/macOS)
- pthread library

## Compilation

Compile the server:
```bash
gcc -o server server.c -pthread
```

Compile the client:
```bash
gcc -o client client.c
```

## How to Run

1. First, start the server:
```bash
./server
```

2. Then, in a different terminal, run the client:
```bash
./client
```

The client will connect to the server and receive a random quote.

## Features
- TCP-based communication
- Multi-threaded server that can handle multiple clients
- Simple and lightweight implementation
- Uses POSIX sockets and pthread for multi-threading