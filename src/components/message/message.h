#ifndef MESSAGE_H
#define MESSAGE_H

#include <ctime>
#include <string>

class Message {
   public:
    Message(const std::string& content);
    void print() const;

   private:
    std::string content;
    time_t timestamp;
};

#endif