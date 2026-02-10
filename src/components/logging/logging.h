/*******************************************************************
 *
 * File Name: logging.h
 *
 * Description:
 *   header for the server msg logging
 *
 * Author: sparrow
 * Date: 10/2/2026
 *
 *******************************************************************/

#pragma once

#include <string>

class Logging {
   public:
    static void message_log(const std::string& user, const std::string& message);
    static void server_log(const std::string& message);

   private:
    static std::string get_time(void);
    static void log(const std::string& log);
};