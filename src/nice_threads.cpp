#include "nice_threads.h"
#include "nice_logger.h"

NiceThread::NiceThread(const std::string& service_name)
    : _service_name(service_name), 
      _state(ThreadState::NOT_STARTED) {
    _mloop.setServiceName(service_name);
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

void NiceThread::enqueueSyncMsg(Message* m) {
    _mloop.enqueueSync(m);
}

void NiceThread::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {
    _mloop.getCbMap(funcMap);
}

void NiceThread::stopThread() {  
    _mloop.stop();
    _t->join();
    _mloop.drainQueue();
    NiceLogger::instance(_service_name) << "Normal shutdown. Q Drained" << std::endl;    
    _state = ThreadState::STOPPED;
}

bool NiceThread::isThreadStopped() {
    return _state == ThreadState::STOPPED;
}