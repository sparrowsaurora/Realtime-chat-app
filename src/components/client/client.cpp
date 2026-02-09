#include "client.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>

#define LOCALHOST "127.0.0.1"  // cant constexpr, using c++ v17 (dont want to use string_view)

constexpr int PORT = 54000;
constexpr int BUFFER_SIZE = 1024;

std::string get_username(void) {
    std::string username;
    std::getline(std::cin, username);

    username.erase(0, username.find_first_not_of(" \t\r\n\f\v"));
    username.erase(username.find_last_not_of(" \t\r\n\f\v") + 1);

    return username;
}

/**
 * Listen for messages coming from the server and print them to the screen.
 */
void receive_loop(int sock) {
    char buffer[BUFFER_SIZE];
    while (true) {
        ssize_t bytes = recv(sock, buffer, BUFFER_SIZE, 0);  // reads recieved bytes to buffer
        if (bytes <= 0) break;                               // if nothing or error; break

        std::cout << std::string(buffer, bytes);  // print to screen on client's window
        std::cout.flush();                        // force text to render immediately
    }
}

int run_client(const std::string& username) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);  // init socke

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, LOCALHOST, &server.sin_addr);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) != 0) {  // join
        perror("connect");
        return 1;
    }
    send(sock, username.c_str(), username.size(), 0);
    send(sock, "\n", 1, 0);

    // std::cout << "Enter username: ";
    // std::string username = get_username();

    // std::cout << "Connecting to Room as " << username << "\n";

    std::thread(receive_loop, sock).detach();

    std::string input;
    while (std::getline(std::cin, input)) {
        input += "\n";
        std::string msg = input;
        send(sock, msg.c_str(), msg.size(), 0);
    }

    close(sock);
    return 0;
}
