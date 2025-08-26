#pragma once

#include <thread>
#include "message_loop.h"
#include "chirp_error.h"

class ChirpThread {

public:
    ChirpThread() = default;
    ~ChirpThread();

    explicit ChirpThread(const std::string& service_name);

    void startThread();
    void stopThread();
    ChirpError::Error enqueueMsg(Message* m);
    ChirpError::Error enqueueSyncMsg(Message* m);
    void getCbMap(std::map<std::string, 
                  std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap);
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

