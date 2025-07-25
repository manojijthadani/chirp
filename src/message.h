//
// Created by manoj ij thadani on 7/9/25.
//

#pragma once
#include <any>
#include <string>
#include <vector>
#include <mutex>

class Message {

public:
    enum class MessageType {
        SYNC,
        ASYNC
    };

public:
    Message() = default;
    ~Message() = default;

    Message(std::string& message, Message::MessageType mt, std::vector<std::any>& args);
    void getMessage(std::string& message);
    void getArgs(std::vector<std::any>& args);
    void getMessageType(Message::MessageType& type);
    void sync_wait();
    void sync_notify();

private:
    std::string _msg;
    std::vector<std::any> _args;
    MessageType _type;
    std::mutex _sync_mtx;
};
