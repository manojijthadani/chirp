//
// Created by manoj ij thadani on 7/9/25.
//
#include "message.h"

Message::Message(std::string& message, std::vector<std::any>& args)
    : _msg(message), _args(args) {}

void Message::getMessage(std::string& message) {
    message = _msg;
}

void Message::getArgs(std::vector<std::any>& args) {
    args = _args;
}