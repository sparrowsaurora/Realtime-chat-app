#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>

// constexpr int PORT = 54000;
// constexpr int BUFFER_SIZE = 1024;

/**
 * Listen for messages coming from the server and print them to the screen.
 */
void receive_loop(int sock);

int run_client(void);
