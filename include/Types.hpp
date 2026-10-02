#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>

typedef struct Client_Data{
    std::string user_name;
    int room_id = -1;
    int client_fd = -1;
}Client_Data;

#endif
