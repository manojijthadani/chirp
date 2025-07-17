#include "nice_threads.h"
#include "nice_logger.h"

NiceThread::NiceThread(const std::string& service_name) {

    _service_name = service_name;
    _mloop.setServiceName(service_name);
    _state = ThreadState::NOT_STARTED;
}

void NiceThread::startThread() {

    _t = new std::thread(&MessageLoop::spin, &_mloop);
    _state = ThreadState::STARTED;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    _state = ThreadState::RUNNING;
}

void NiceThread::enqueueMsg(Message* m) {

    _mloop.enqueue(m);
}

void NiceThread::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {

    _mloop.getCbMap(funcMap);
}

void NiceThread::stopThread(bool shutdownType) {

    _mloop.stop(shutdownType);
    _t->join();
    _mloop.drainQueue();
    NiceLogger::instance(_service_name) 
                           << " thread joined, loop drained " << std::endl;
    _state = ThreadState::STOPPED;
}

bool NiceThread::isThreadStopped() {

    return _state == ThreadState::STOPPED;
}