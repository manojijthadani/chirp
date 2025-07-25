//
// Created by manoj ij thadani on 7/9/25.
//
#pragma once
#include <queue>
#include <functional>
#include <map>
#include <any>
#include <vector>
#include <mutex>

#include "message.h"

class MessageLoop {

public:
    MessageLoop() = default;
    ~MessageLoop() = default;

    void spin();
    void enqueue(Message* m);
    void enqueueSync(Message* m);
    void setServiceName(const std::string& service_name);
    void getCbMap(std::map<std::string, 
                  std::function<void(std::vector<std::any>)>>*& funcMap);

    void stop();
    void drainQueue();

private:

    void setStopThread(bool st);
    void enqueueInternal(Message* m, Message::MessageType type);
    
    std::queue<Message*> _message_queue;
    std::string _service_name;
    std::map<std::string, std::function<void(std::vector<std::any>)>> _functions;
    std::mutex _empty_mtx;
    std::mutex _task_exec_mtx;
    volatile bool _stop_thread;
};
