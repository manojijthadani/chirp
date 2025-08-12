#include <thread>

#include "chirp.h"
#include "chirp_threads.h"
#include "chirp_logger.h"
#include "chirp_impl.h"

ChirpImpl::ChirpImpl(const std::string& service_name, ChirpError::Error& error) {
    _service_name = service_name;
    _nthread = new (std::nothrow) ChirpThread(_service_name);
    if (!_nthread) {
        error = ChirpError::RESOURCE_ALLOCATION_FAILED;
        return;
    }
    error = ChirpError::SUCCESS;
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

ChirpError::Error ChirpImpl::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    ChirpError::Error result = ChirpError::SUCCESS;
    Message* msg = new (std::nothrow) Message(msgName, Message::MessageType::ASYNC, args);
    if (!msg) {
        // Log error but continue - message allocation failure shouldn't crash the service
        ChirpLogger::instance(_service_name) << "Failed to allocate message for: " << msgName << std::endl;
        result = ChirpError::RESOURCE_ALLOCATION_FAILED;
    } else {
        result = _nthread->enqueueMsg(msg);
        if (result != ChirpError::SUCCESS) {
            delete msg; // Clean up the allocated message
        }
    }
    return result;
}

ChirpError::Error ChirpImpl::enqueSyncMsg(std::string& msgName, std::vector<std::any>& args) {
    ChirpError::Error result = ChirpError::SUCCESS;
    Message* msg = new (std::nothrow) Message(msgName, Message::MessageType::SYNC, args);
    if (!msg) {
        // Log error but continue - message allocation failure shouldn't crash the service
        ChirpLogger::instance(_service_name) << "Failed to allocate sync message for: " << msgName << std::endl;
        result = ChirpError::RESOURCE_ALLOCATION_FAILED;
    } else {
        result = _nthread->enqueueSyncMsg(msg);
        if (result != ChirpError::SUCCESS) {
            delete msg; // Clean up the allocated message
        }
    }
    return result;
}

void ChirpImpl::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {
    _nthread->getCbMap(funcMap);
}

