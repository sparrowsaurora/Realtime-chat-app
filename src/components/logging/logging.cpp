/*******************************************************************
 *
 * File Name: logging.cpp
 *
 * Description:
 *   Server logging helper to file
 *
 * Author: sparrow
 * Date: 10/2/2026
 *
 *******************************************************************/

#include "logging.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>

void Logging::message_log(const std::string& user, const std::string& message) {
    std::string cleaned_msg = message;
    cleaned_msg.erase(std::remove_if(
                          cleaned_msg.begin(), cleaned_msg.end(), [](unsigned char x) { return x == '\n' || x == '\r'; }),
                      cleaned_msg.end());
    std::string logged_message = "[" + get_current_time() + "] " + user + " :: " + cleaned_msg;
    log(logged_message);
}

void Logging::server_log(const std::string& message) {
    std::string logged_message = "[" + get_current_time() + "] " + message;
    log(logged_message);
}

std::string Logging::get_current_time() {
    static std::mutex m;

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::tm tm;
    {
        std::lock_guard<std::mutex> lock(m);
        tm = *std::localtime(&t);
    }

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void Logging::log(const std::string& log) {
    const std::string log_file = "log.txt";
    static std::mutex file_mutex;
    static std::ofstream out(log_file, std::ios::app);

    if (out) {
        out << log << '\n';
        out.flush();
    } else {
        std::cerr << "Error: Unable to open log file\n";
    }
}