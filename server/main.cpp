#include <cstdint>
#include <iostream>
#include <Wait.hpp>
#include <mutex>
#include <ostream>
#include <sys/types.h>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "../include/Types.hpp"

const char *dots[] = {"", ".", "..", "..."};

std::atomic<bool> server_ready = false;

std::unordered_map<int, Client_Data> clients;
std::mutex clients_mutex;

std::string parse_username(int sock){
    uint32_t net_len;
    recv(sock, &net_len, sizeof(net_len), 0);
    uint32_t len = ntohl(net_len);

    std::string username(len, '\0');
    recv(sock, username.data(), len, 0);
    return username;
}

void handle_client(int client_fd){
    Client_Data data;
    data.client_fd = client_fd;
    data.user_name = parse_username(client_fd);

    std::cout << "User [" << data.user_name << "] has joined the server!\n";

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients[client_fd] = data;
    }

    while (true){
        char buff[1024];
        ssize_t bytes_read = recv(client_fd, buff, sizeof(buff), 0);

        if (bytes_read <= 0) break;
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(client_fd);
    }

    close(client_fd);
}

void spin_up_server(){
    int listed_fd = socket(AF_INET, SOCK_STREAM, 0);

    int yes = 1;
    setsockopt(listed_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(5000);

    int result = bind(listed_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listed_fd, SOMAXCONN);

    std::vector<std::thread> threads;

    server_ready = true;

    while (true){
        sockaddr_in cli{};
        socklen_t len = sizeof(cli);
        int client_fd = accept(listed_fd, (sockaddr*)&cli, &len);
        if (client_fd < 0) continue;

        threads.emplace_back(handle_client, client_fd);
    }

    for (std::thread& thread : threads){
        thread.join();
    }

    close(listed_fd);
}

int main(){
    system("clear");

    std::cout << "Starting ChatRoom Servers!\n";

    std::thread server_startup(spin_up_server);

    unsigned int i = 0;
    while (!server_ready){
        std::cout << "\33[2K\r" << std::flush;

        std::cout << dots[i % 4] << std::flush;
        wait(0.5);
        
        i++;
    }

    std::cout << "\33[2K\r" << std::flush;
    std::cout << "Server Started!\n";

    server_startup.join();

    return 0;
}
