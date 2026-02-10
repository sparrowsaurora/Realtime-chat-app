/*******************************************************************
 *
 * File Name: logging.h
 *
 * Description:
 *   Server logging helper to file
 *
 * Author: sparrow
 * Date: 10/2/2026
 *
 *******************************************************************/

#include "logging.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

void Logging::message_log(const std::string& user, const std::string& message) {
    std::string logged_message = user + " :: " + message + " [" + get_time() + "]";
    log(logged_message);
}

void server_log(const std::string& message) {
    std::string logged_message = message + " [" + get_time() + "]";
    log(logged_message);
}

std::string get_time(void) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << std::endl;
}

void log(const std::string& log) {
    const std::string log_file = "log.txt";
    std::ofstream outputFile(log_file, std::ios::app);
    if (outputFile.is_open()) {
        outputFile << log << std::endl;
        outputFile.close();
    } else {
        std::cout << "Error: Unable to open the file." << std::endl;
    }
}