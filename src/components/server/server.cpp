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

// #include "message.h"

constexpr int PORT = 54000;
constexpr int BUFFER_SIZE = 1024;

std::vector<int> clients;  // list of client devices
std::mutex clients_mutex;

/**
 * Sends a message to every connected client except the one who sent it
 *
 * [const string&] msg
 * [int] sender file descriptor (basically ID)
 *
 */
void broadcast(const std::string& msg, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);  // create mutex lock
    // we need thread saftey to handle multiple clients
    // lockguard to avoid manual locking
    for (int file_descriptor : clients) {
        if (file_descriptor != sender_fd) {  // skip sender
            send(file_descriptor, msg.c_str(), msg.size(), 0);
            // send to other file descriptors
            // pointer to data
            // bytes to send
            // no special flags
        }
    }
}  // mutex unlocks

/**
 * handle client communication and exit protocol
 */
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    while (true) {
        // blocks until data is sent or client is disconnected
        ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);  // read bytes from socket
        if (bytes <= 0) {
            break;  // disconnected
        }

        std::string msg(buffer, bytes);  // convert bytes to string
        broadcast(msg, client_fd);       // send msg
    }

    close(client_fd);  // close socket connection per user

    // remove client from global list
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(
        std::remove(clients.begin(), clients.end(), client_fd),
        clients.end());
    std::cout << "User " << client_fd << " disconnected\n";
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

        {  // store client list safely
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_fd);
        }

        // spawn a new thread for the client
        std::thread(handle_client, client_fd).detach();
    }

    close(server_fd);
}