#include "client.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>

constexpr int PORT = 54000;
constexpr int BUFFER_SIZE = 1024;

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

int run_client() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) != 0) {
        perror("connect");
        return 1;
    }

    std::thread(receive_loop, sock).detach();

    std::string input;
    while (std::getline(std::cin, input)) {
        input += "\n";
        send(sock, input.c_str(), input.size(), 0);
    }

    close(sock);
    return 0;
}
