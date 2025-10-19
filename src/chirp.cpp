#include <thread>

#include "chirp.h"
#include "chirp_threads.h"
#include "chirp_logger.h"
#include "chirp_impl.h"
#include "chirp_timer.h"

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

Chirp::~Chirp() {
    if (_impl) {
        delete _impl;
    }
}

ChirpError::Error Chirp::start() {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE; // Cannot start if not properly initialized
    }
    _impl->start();
    return ChirpError::SUCCESS;
}

ChirpError::Error Chirp::shutdown() {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE; // Cannot shutdown if not properly initialized
    }
    _impl->shutdown();
    return ChirpError::SUCCESS;
}

std::string Chirp::getServiceName() {
    if (!_impl) {
        return ""; // Return empty string if not properly initialized
    }
    return _impl->getServiceName();
}

ChirpError::Error Chirp::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    return _impl->enqueMsg(msgName, args);
}

ChirpError::Error Chirp::enqueSyncMsg(std::string& msgName, std::vector<std::any>& args) {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    return _impl->enqueSyncMsg(msgName, args);
}

void Chirp::getCbMap(std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap) {
    if (!_impl) {
        funcMap = nullptr;
        return;
    }
    _impl->getCbMap(funcMap);
}

ChirpError::Error Chirp::addChirpTimer(IChirpTimer* timer) {
    ChirpError::Error result = ChirpError::SUCCESS;
    
    if (!_impl) {
        result = ChirpError::INVALID_SERVICE_STATE;
    } else if (!timer) {
        result = ChirpError::INVALID_ARGUMENTS;
    } else {
        // Dynamic cast from IChirpTimer to ChirpTimer
        ChirpTimer* chirpTimer = dynamic_cast<ChirpTimer*>(timer);
        if (!chirpTimer) {
            ChirpLogger::instance(getServiceName()) << "Failed to cast IChirpTimer to ChirpTimer" << std::endl;
            result = ChirpError::INVALID_ARGUMENTS;
        } else {
            _impl->addChirpTimer(chirpTimer);
            result = ChirpError::SUCCESS;
        }
    }
    
    return result;
}

ChirpError::Error Chirp::removeChirpTimer(IChirpTimer* timer) {
    ChirpError::Error result = ChirpError::SUCCESS;
    
    if (!_impl) {
        result = ChirpError::INVALID_SERVICE_STATE;
    } else if (!timer) {
        result = ChirpError::INVALID_ARGUMENTS;
    } else {
        // Dynamic cast from IChirpTimer to ChirpTimer
        ChirpTimer* chirpTimer = dynamic_cast<ChirpTimer*>(timer);
        if (!chirpTimer) {
            ChirpLogger::instance(getServiceName()) << "Failed to cast IChirpTimer to ChirpTimer" << std::endl;
            result = ChirpError::INVALID_ARGUMENTS;
        } else {
            _impl->removeChirpTimer(chirpTimer);
            result = ChirpError::SUCCESS;
        }
    }
    
    return result;
}

