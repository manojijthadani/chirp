//
// Created by manoj ij thadani on 7/9/25.
//

#pragma once
#include <any>
#include <string>
#include <vector>

class Message {

public:
    Message() = default;
    ~Message() = default;

    Message(std::string& message, std::vector<std::any>& args);
    void getMessage(std::string& message);
    void getArgs(std::vector<std::any>& args);

private:
    std::string _msg;
    std::vector<std::any> _args;
};
