/*******************************************************************
 *
 * File Name: server.c
 *
 * Description:
 *   Runs the server that clients connect to.
 *
 * Author: sparrow
 * Date: 3/2/2026
 *
 *******************************************************************/

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// constexpr int PORT = 54000;
// constexpr int BUFFER_SIZE = 1024;

// enum Colour {
//     RED,
//     BLUE,
//     GREEN,
//     PURPLE,
//     YELLOW,
//     PINK,
//     WHITE,
// };

struct User {
    int file_descriptor;
    std::string username;
    std::string colour;
};

std::string get_colour(void);

std::string colour_msg(std::string msg, const std::string& colour);

/**
 * Sends a message to every connected client except the one who sent it
 *
 * [const string&] msg
 * [int] sender file descriptor (basically ID)
 *
 */
void broadcast(const std::string& msg, int sender_fd, const std::string& colour);

/**
 * handle client communication and exit protocol
 */
void handle_client(int client_fd);

int run_server(void);