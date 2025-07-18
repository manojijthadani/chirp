#include <thread>

#include "nice_service.h"
#include "nice_threads.h"
#include "nice_logger.h"

NiceService::NiceService(const std::string& service_name) {
    _service_name = service_name;
    _nthread = new NiceThread(_service_name);
}

void NiceService::start() {
    NiceLogger::instance(_service_name) << "Starting " << _service_name << std::endl;
    _nthread->startThread();
}

void NiceService::shutdown(ShutdownType s) {
    NiceLogger::instance(_service_name) << "Stopping " << _service_name << std::endl;
    _nthread->stopThread((s == ShutdownType::NORMAL ? false : true));
    waitUntilServiceStopped();
}

void NiceService::waitUntilServiceStopped() {
    while (!_nthread->isThreadStopped()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::string NiceService::getServiceName() {
    return _service_name;
}

void NiceService::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    Message* msg = new Message(msgName, args);
    _nthread->enqueueMsg(msg);
}

void NiceService::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {
    _nthread->getCbMap(funcMap);
}

