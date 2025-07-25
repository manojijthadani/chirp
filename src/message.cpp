//
// Created by manoj ij thadani on 7/9/25.
//
#include "message.h"

Message::Message(std::string& message, Message::MessageType mt, std::vector<std::any>& args)
    : _msg(message), _args(args), _type(mt) {}

void Message::getMessage(std::string& message) {
    message = _msg;
}

void Message::getArgs(std::vector<std::any>& args) {
    args = _args;
}

void Message::getMessageType(Message::MessageType& type) {
    type = _type;
}

void Message::sync_wait() {
    _sync_mtx.lock();
    _sync_mtx.lock();
}

void Message::sync_notify() {
    _sync_mtx.unlock();
}