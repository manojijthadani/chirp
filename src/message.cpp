//
// Created by manoj ij thadani on 7/9/25.
//
#include "message.h"

Message::Message(std::string& message, Message::MessageType mt, std::vector<std::any>& args)
    : _msg(message), _args(args), _type(mt), _sync_done(false) {}

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
    std::unique_lock<std::mutex> lock(_sync_mtx);
    _sync_cv.wait(lock, [this]{ return _sync_done; });
}

void Message::sync_notify() {
    {
        std::lock_guard<std::mutex> lock(_sync_mtx);
        _sync_done = true;
    }
    _sync_cv.notify_one();
}