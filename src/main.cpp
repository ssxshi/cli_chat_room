#include <cstdint>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <Wait.hpp>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tuple>
#include <cstring>
#include <unistd.h>

#include "../include/Join.hpp"
#include "../include/Create.hpp"

#define NULL_CHAR '\0'

#define SERVER_IP "96.255.240.118"
#define SERVER_PORT 5000

Client_Data* make_client_data(){
    system("clear");

    Client_Data *data = new Client_Data();

    getchar();

    std::cout << "Enter your username: ";
    std::getline(std::cin, data->user_name);

    return data;
}

typedef std::tuple<bool, std::string, int> connection_info;
connection_info connect_to_server(Client_Data *data){
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1){
        return connection_info(false, "Failed to create client socket! Try again!", -1);
    }

    sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0){
        close(client_socket);
        return connection_info(false, "Failed to connect to server! E[0]", -1);
    }

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1){
        close(client_socket);
        return connection_info(false, "Failed to connect to server! E[-1]", -1);
    }

    uint32_t name_len = data->user_name.size();
    uint32_t net_len = htonl(name_len);

    send(client_socket, &net_len, sizeof(net_len), 0);
    send(client_socket, data->user_name.data(), name_len, 0);

    return connection_info(true, "Connected to Server!", client_socket);
}

int main(){
    std::system("clear");
    
    std::cout << "Welcome to the chat room!\n\n";

    char option = NULL_CHAR;

    while (option == NULL_CHAR){
        std::cout << "Would you like to [J]oin a room or [C]reate one?\nEnter [x] to leave.\n";

        option = getchar();

        if (option == 'j' || option == 'c') break;
        if (option == 'x') return 0;

        option = NULL_CHAR;
    
        std::cout << "Invalid selection. Try Again.\n";
        wait(2);

        system("clear");
    }

    Client_Data *data = make_client_data();
    
    std::cout << "Connecting to server...\n";

    bool state;
    std::string msg;
    int client_socket;

    std::tie(state, msg, client_socket) = connect_to_server(data);

    if (!state){
        std::cerr << msg << std::endl;
        return 1;
    }

    data->client_fd = client_socket;

    if (option == 'j') join(data);
    else if (option == 'c') create(data);
    
    return 0;
}
