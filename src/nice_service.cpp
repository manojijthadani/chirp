#include <thread>

#include "nice_service.h"
#include "nice_threads.h"
#include "nice_logger.h"
#include "nice_service_impl.h"

// A simple reflection pattern implemented to abstract NiceService class.
// Cannot implement a typical interface pattern because templated functions 
// cannot be virtual as of C++ 20.
const std::string NiceService::_version = "1.0";

const std::string& NiceService::getVersion() {
    return _version;
}

NiceService::NiceService(const std::string& service_name) {
    _impl = new NiceServiceImpl(service_name);
}

void NiceService::start() {
    _impl->start();
}

void NiceService::shutdown() {
    _impl->shutdown();
}

std::string NiceService::getServiceName() {
    return _impl->getServiceName();
}

void NiceService::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    _impl->enqueMsg(msgName, args);
}

void NiceService::enqueSyncMsg(std::string& msgName, std::vector<std::any>& args) {
    _impl->enqueSyncMsg(msgName, args);
}

void NiceService::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {
    _impl->getCbMap(funcMap);
}

