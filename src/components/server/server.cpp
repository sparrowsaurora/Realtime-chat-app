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

#include "server.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

constexpr int PORT = 54000;
constexpr int BUFFER_SIZE = 1024;

std::vector<User> clients;  // list of client devices
std::mutex clients_mutex;

std::string get_colour() {
    std::string colour;
    while (true) {
        std::cout << "Enter colour (if blank Colour = white): ";
        std::getline(std::cin, colour);

        colour.erase(0, colour.find_first_not_of(" \t\r\n\f\v"));
        colour.erase(colour.find_last_not_of(" \t\r\n\f\v") + 1);

        const std::set<std::string> available_colours = {"white",
                                                         "red",
                                                         "blue",
                                                         "green",
                                                         "purple",
                                                         "yellow",
                                                         "cyan"};

        if (colour == "") {
            return "white";
        } else if (available_colours.find(colour) == available_colours.end()) {
            std::cout << "<" << colour << "> is not a valid option" << std::endl;
        } else {
            return colour;
        }
    }
}

std::string colour_msg(const std::string msg, const std::string& colour) {
    std::string colour_code = "\e[0m";  // default (reset / no colour)
    if (colour == "white") {
        colour_code = "\e[0;37m";
    } else if (colour == "red") {
        colour_code = "\e[0;31m";
    } else if (colour == "blue") {
        colour_code = "\e[0;34m";
    } else if (colour == "green") {
        colour_code = "\e[0;32m";
    } else if (colour == "purple") {
        colour_code = "\e[0;35m";
    } else if (colour == "yellow") {
        colour_code = "\e[0;33m";
    } else if (colour == "cyan") {
        colour_code = "\e[0;36m";
    } else {
        std::cout << "Unknown colour, using default\n";
    }
    std::cout << "code: " << colour_code << std::endl;
    std::cout << "colour: " << colour << std::endl;
    const std::string reset_colour = "\e[0m";
    std::string new_msg = colour_code + msg + reset_colour;
    return new_msg;
}

/**
 * Sends a message to every connected client except the one who sent it
 *
 * [const string&] msg
 * [int] sender file descriptor (basically ID)
 *
 */
void broadcast(const std::string& msg, int sender_fd, const std::string& colour) {
    std::string coloured_msg = colour_msg(msg, colour);
    std::lock_guard<std::mutex> lock(clients_mutex);  // create mutex lock
    // we need thread saftey to handle multiple clients
    // lockguard to avoid manual locking
    for (const User& client : clients) {
        if (client.file_descriptor != sender_fd) {  // skip sender
            send(client.file_descriptor, coloured_msg.c_str(), coloured_msg.size(), 0);
            // send to other file descriptors
            // pointer to data
            // bytes to send
            // no special flags
        } else {
            const std::string self_msg = "\r" + coloured_msg;
            send(client.file_descriptor, self_msg.c_str(), self_msg.size(), 0);
        }
    }
}  // mutex unlocks

/**
 * handle client communication and exit protocol
 */
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    // get username
    ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }

    std::string line(buffer, bytes);
    line.erase(line.find_last_not_of("\r\n") + 1);

    auto sep = line.find('|');
    if (sep == std::string::npos) {
        close(client_fd);
        return;
    }

    std::string username = line.substr(0, sep);
    std::string colour = line.substr(sep + 1);

    User client;
    client.file_descriptor = client_fd;
    client.username = username;
    client.colour = colour;

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client);
    }

    std::string join_msg = "[SERVER] " + username + " has joined.\n";
    broadcast(join_msg, client_fd, "yellow");

    // chat loop
    while (true) {
        // blocks until data is sent or client is disconnected
        ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);  // read bytes from socket
        if (bytes <= 0) {
            break;  // disconnected
        }

        std::string msg(buffer, bytes);  // convert bytes to string
        std::string named_msg = username + ": " + msg;
        broadcast(named_msg, client_fd, colour);  // send msg
    }

    close(client_fd);  // close socket connection per user

    // remove client from global list
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(
            std::remove_if(
                clients.begin(),
                clients.end(),
                [&](const User& c) {
                    return c.file_descriptor == client_fd;
                }),
            clients.end());
    }

    std::string leave_msg = "[SERVER] " + username + " disconnected\n";
    broadcast(leave_msg, client_fd, "yellow");
}

int run_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);  // create main server socket
    // ipv4, tcp, default protocol
    if (server_fd == -1) {  // ensure socket didnt fail
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    // basically accept from any network interface

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));  // bind socket to address
    listen(server_fd, SOMAXCONN);                     // listen to server with system's normal queue size

    std::cout << "Server listening on port " << PORT << "\n";

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);  // accept client connection
        if (client_fd == -1) continue;                        // if client fails to connect, continue

        // spawn a new thread for the client
        std::thread(handle_client, client_fd)
            .detach();
    }

    close(server_fd);
}