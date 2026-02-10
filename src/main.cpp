/*******************************************************************
 *
 * File Name: server.c
 *
 * Description:
 *   Runs the server that clients connect to.
 *
 * Author: sparrow
 * Date: 5/2/2026
 *
 *******************************************************************/

#include <algorithm>
#include <argparse.hpp>
#include <cctype>
#include <iostream>
#include <string>

#include "client/client.h"
#include "server/server.h"

#define VERSION 1

std::string clean_command(std::string cmd) {
    // Find first non-whitespace character
    cmd.erase(0, cmd.find_first_not_of(" \t\r\n\f\v"));
    // Find last non-whitespace character and erase after it
    cmd.erase(cmd.find_last_not_of(" \t\r\n\f\v") + 1);

    std::transform(
        cmd.begin(), cmd.end(), cmd.begin(),
        [](unsigned char c) { return std::tolower(c); });

    return cmd;
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("Chat App", std::to_string(VERSION));

    program.add_argument("command")
        .help("Select the mode to run [ Server / Client ]")
        .default_value(std::string{"client"});

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    std::string command = clean_command(program.get<std::string>("command"));

    if (command == "server") {
        std::cout << "running Server\n";
        run_server();
        return 0;
    } else if (command == "client") {
        std::cout << "running Client\n";
        std::string username = get_username();
        std::string colour = get_colour();
        run_client(username, colour);
        return 0;
    } else {
        std::cout << "Error: <" << command << "> is not valid\n";
        std::cout << "Please enter Server or Client\n";
        return 1;
    }
}