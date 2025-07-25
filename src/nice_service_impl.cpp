#include <thread>

#include "nice_service.h"
#include "nice_threads.h"
#include "nice_logger.h"
#include "nice_service_impl.h"

NiceServiceImpl::NiceServiceImpl(const std::string& service_name) {
    _service_name = service_name;
    _nthread = new NiceThread(_service_name);
}

void NiceServiceImpl::start() {
    NiceLogger::instance(_service_name) << "Starting " << _service_name << std::endl;
    _nthread->startThread();
}

void NiceServiceImpl::shutdown() {
    NiceLogger::instance(_service_name) << "Stopping " << _service_name << std::endl;
    _nthread->stopThread();
    waitUntilServiceStopped();
}

void NiceServiceImpl::waitUntilServiceStopped() {
    while (!_nthread->isThreadStopped()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::string NiceServiceImpl::getServiceName() {
    return _service_name;
}

void NiceServiceImpl::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    Message* msg = new Message(msgName, Message::MessageType::ASYNC, args);
    _nthread->enqueueMsg(msg);
}

void NiceServiceImpl::enqueSyncMsg(std::string& msgName, std::vector<std::any>& args) {
    Message* msg = new Message(msgName, Message::MessageType::SYNC, args);
    _nthread->enqueueSyncMsg(msg);
}

void NiceServiceImpl::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {
    _nthread->getCbMap(funcMap);
}

