# CLI Chat Room

A multi-client, terminal-based chat room written in C++ using POSIX TCP sockets.

## Features

- TCP server listening on port `5000`
- Thread-per-client connection handling
- Length-prefixed username handshake (network byte order)
- Thread-safe client registry (`std::unordered_map` guarded by `std::mutex`)
- Terminal startup animation while the server boots
- No external dependencies, just the C++ standard library and POSIX sockets

## Project Structure

```
cli_chat_room/
├── include/    # shared headers (Types.hpp, Wait.hpp, ...)
├── src/        # client source code
├── server/     # server source code
├── Makefile
└── ChatRoom    # compiled client binary
```

## Requirements

- Linux or macOS (uses POSIX sockets)
- `g++` with C++17 or newer
- `make`

Arch:

```bash
sudo pacman -S gcc make
```

## Build

```bash
git clone https://github.com/ssxshi/cli_chat_room.git
cd cli_chat_room
make all
```

Build the server:

```bash
g++ server/*.cpp -o ChatServer -Iinclude -pthread
```

Clean:

```bash
make clean
```

## Usage

Start the server:

```bash
./ChatServer
```

Connect a client:

```bash
./ChatRoom
```

## How It Works

1. The server binds to `0.0.0.0:5000` and calls `accept()` in a loop.
2. Each new connection gets its own `std::thread`.
3. On connect, the client sends its username as a 4-byte length (`htonl`) followed by the raw bytes.
4. The server stores the client in a shared map (protected by a mutex) and removes it on disconnect.

## Roadmap

- [ ] Broadcast messages to all connected clients
- [ ] Join/leave notifications
- [ ] Handle partial `recv` reads and validate username length
- [ ] Graceful shutdown and thread cleanup
- [ ] Private messages and chat commands

## License

MIT
