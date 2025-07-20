#pragma once

#include <thread>
#include "message_loop.h"

class NiceThread {

public:
    NiceThread() = default;
    ~NiceThread() = default;

    explicit NiceThread(const std::string& service_name);

    void startThread();

    void stopThread(bool shutdownType);

    void enqueueMsg(Message* m);

    void getCbMap(std::map<std::string, 
                  std::function<void(std::vector<std::any>)>>*& funcMap);

    bool isThreadStopped();        
    
private:

enum class ThreadState {
    NOT_STARTED,
    STARTED,
    RUNNING,
    STOPPED
};

    MessageLoop _mloop;
    std::thread* _t;
    std::string _service_name;
    ThreadState _state;
};

