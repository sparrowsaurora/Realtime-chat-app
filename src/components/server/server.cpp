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
#include "server.h"

constexpr int PORT = 54000;
constexpr int BUFFER_SIZE = 1024;

std::vector<User> clients;  // list of client devices
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
    for (const User& client : clients) {
        if (client.file_descriptor != sender_fd) {  // skip sender
            send(client.file_descriptor, msg.c_str(), msg.size(), 0);
            // send to other file descriptors
            // pointer to data
            // bytes to send
            // no special flags
        } else {
            const std::string self_msg = "\r" + msg;
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
    ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);  // read bytes from socket
    if (bytes <= 0) {
        close(client_fd);  // disconnected
        return;
    }

    std::string username(buffer, bytes);

    // trim newline if present
    username.erase(username.find_last_not_of("\r\n") + 1);

    User client;
    client.file_descriptor = client_fd;
    client.username = username;
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client);
    }

    std::string join_msg = "[SERVER] " + username + " has joined.\n";
    broadcast(join_msg, client_fd);

    // chat loop
    while (true) {
        // blocks until data is sent or client is disconnected
        ssize_t bytes = recv(client_fd, buffer, BUFFER_SIZE, 0);  // read bytes from socket
        if (bytes <= 0) {
            break;  // disconnected
        }

        std::string msg(buffer, bytes);  // convert bytes to string
        std::string named_msg = username + ": " + msg;
        broadcast(named_msg, client_fd);  // send msg
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
    broadcast(leave_msg, client_fd);
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