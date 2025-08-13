#include <thread>

#include "chirp.h"
#include "chirp_threads.h"
#include "chirp_logger.h"
#include "chirp_impl.h"

// A simple reflection pattern implemented to abstract Chirp class.
// Cannot implement a typical interface pattern because templated functions 
// cannot be virtual as of C++ 20.
const std::string Chirp::_version = "1.0";

const std::string& Chirp::getVersion() {
    return _version;
}

Chirp::Chirp(const std::string& service_name, ChirpError::Error& error) {
    _impl = new (std::nothrow) ChirpImpl(service_name, error);
    if (!_impl) {
        error = ChirpError::RESOURCE_ALLOCATION_FAILED;
    }
}

void Chirp::start() {
    _impl->start();
}

void Chirp::shutdown() {
    _impl->shutdown();
}

std::string Chirp::getServiceName() {
    return _impl->getServiceName();
}

ChirpError::Error Chirp::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    return _impl->enqueMsg(msgName, args);
}

ChirpError::Error Chirp::enqueSyncMsg(std::string& msgName, std::vector<std::any>& args) {
    return _impl->enqueSyncMsg(msgName, args);
}

void Chirp::getCbMap(std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap) {
    _impl->getCbMap(funcMap);
}

