#include "chirp_threads.h"
#include "chirp_logger.h"

ChirpThread::ChirpThread(const std::string& service_name)
    : _service_name(service_name), 
      _state(ThreadState::NOT_STARTED),
      _t(nullptr) {

    _mloop.setServiceName(service_name);
}

void ChirpThread::startThread() {

    _t = new std::thread(&MessageLoop::spin, &_mloop);
    _state = ThreadState::STARTED;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    _state = ThreadState::RUNNING;
}

ChirpError::Error ChirpThread::enqueueMsg(Message* m) {

    ChirpError::Error result = ChirpError::SUCCESS;
    if (_state != ThreadState::STARTED && _state != ThreadState::RUNNING) {
        ChirpLogger::instance(_service_name) << "Cannot enqueue message: thread not in STARTED or RUNNING state" << std::endl;
        result = ChirpError::INVALID_SERVICE_STATE;
    } else {
        _mloop.enqueue(m);
    }
    return result;
}

ChirpError::Error ChirpThread::enqueueSyncMsg(Message* m) {

    ChirpError::Error result = ChirpError::SUCCESS;
    if (_state != ThreadState::STARTED && _state != ThreadState::RUNNING) {
        ChirpLogger::instance(_service_name) << "Cannot enqueue sync message: thread not in STARTED or RUNNING state" << std::endl;
        result = ChirpError::INVALID_SERVICE_STATE;
    } else {
        _mloop.enqueueSync(m);
    }
    return result;
}

void ChirpThread::getCbMap(std::map<std::string, 
                           std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap) {

    _mloop.getCbMap(funcMap);
}

void ChirpThread::stopThread() {  

    _mloop.stop();
    if (_t != nullptr) {
        _t->join();
        delete _t;
        _t = nullptr;
    }
    _mloop.drainQueue();
    ChirpLogger::instance(_service_name) << "Normal shutdown. Q Drained" << std::endl;    
    _state = ThreadState::STOPPED;
}

bool ChirpThread::isThreadStopped() {

    return _state == ThreadState::STOPPED;
}

ChirpThread::~ChirpThread() {

    if (_t != nullptr) {
        if (_state != ThreadState::STOPPED) {
            stopThread();
        } else {
            delete _t;
        }
    }
}

void ChirpThread::addChirpTimer(ChirpTimer* timer) {

    _mloop.addChirpTimer(timer);
}

void ChirpThread::removeChirpTimer(ChirpTimer* timer) {
    
    _mloop.removeChirpTimer(timer);
}
