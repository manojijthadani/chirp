#include <thread>

#include "chirp.h"
#include "chirp_threads.h"
#include "chirp_logger.h"
#include "chirp_impl.h"

ChirpImpl::ChirpImpl(const std::string& service_name) {
    _service_name = service_name;
    _nthread = new ChirpThread(_service_name);
}

void ChirpImpl::start() {
    ChirpLogger::instance(_service_name) << "Starting " << _service_name << std::endl;
    _nthread->startThread();
}

void ChirpImpl::shutdown() {
    ChirpLogger::instance(_service_name) << "Stopping " << _service_name << std::endl;
    _nthread->stopThread();
    waitUntilServiceStopped();
}

void ChirpImpl::waitUntilServiceStopped() {
    while (!_nthread->isThreadStopped()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::string ChirpImpl::getServiceName() {
    return _service_name;
}

void ChirpImpl::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    Message* msg = new Message(msgName, Message::MessageType::ASYNC, args);
    _nthread->enqueueMsg(msg);
}

void ChirpImpl::enqueSyncMsg(std::string& msgName, std::vector<std::any>& args) {
    Message* msg = new Message(msgName, Message::MessageType::SYNC, args);
    _nthread->enqueueSyncMsg(msg);
}

void ChirpImpl::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {
    _nthread->getCbMap(funcMap);
}

