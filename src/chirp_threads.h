#pragma once

#include <thread>
#include "message_loop.h"

class ChirpThread {

public:
    ChirpThread() = default;
    ~ChirpThread() = default;

    explicit ChirpThread(const std::string& service_name);

    void startThread();
    void stopThread();
    void enqueueMsg(Message* m);
    void enqueueSyncMsg(Message* m);
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

