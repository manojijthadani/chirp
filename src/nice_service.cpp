#include "nice_service.h"
#include <thread>

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


