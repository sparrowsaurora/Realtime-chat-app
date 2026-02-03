#include "message.h"

#include <iostream>

Message::Message(const std::string& content)
    : content(content), timestamp(std::time(nullptr)) {
}

void Message::print() const {
    std::cout << "# " << content << " [" << timestamp << "]" << std::endl;
}