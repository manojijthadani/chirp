#include "chirp_threads.h"
#include "chirp_logger.h"

ChirpThread::ChirpThread(const std::string& service_name)
    : _service_name(service_name), 
      _state(ThreadState::NOT_STARTED) {
    _mloop.setServiceName(service_name);
}

void ChirpThread::startThread() {
    _t = new std::thread(&MessageLoop::spin, &_mloop);
    _state = ThreadState::STARTED;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    _state = ThreadState::RUNNING;
}

void ChirpThread::enqueueMsg(Message* m) {
    _mloop.enqueue(m);
}

void ChirpThread::enqueueSyncMsg(Message* m) {
    _mloop.enqueueSync(m);
}

void ChirpThread::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {
    _mloop.getCbMap(funcMap);
}

void ChirpThread::stopThread() {  
    _mloop.stop();
    _t->join();
    _mloop.drainQueue();
    ChirpLogger::instance(_service_name) << "Normal shutdown. Q Drained" << std::endl;    
    _state = ThreadState::STOPPED;
}

bool ChirpThread::isThreadStopped() {
    return _state == ThreadState::STOPPED;
}