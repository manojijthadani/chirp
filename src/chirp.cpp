#include <thread>

#include "ichirp.h"
#include "chirp_threads.h"
#include "chirp_logger.h"
#include "chirp_impl.h"
#include "chirp_timer.h"

// A simple reflection pattern implemented to abstract IChirp class.
// Cannot implement a typical interface pattern because templated functions 
// cannot be virtual as of C++ 20.
const std::string IChirp::_version = "1.0";

const std::string& IChirp::getVersion() {
    return _version;
}

IChirp::IChirp(const std::string& service_name, ChirpError::Error& error) {
    _impl = new (std::nothrow) ChirpImpl(service_name, error);
    if (!_impl) {
        error = ChirpError::RESOURCE_ALLOCATION_FAILED;
    }
}

IChirp::~IChirp() {
    if (_impl) {
        delete _impl;
    }
}

ChirpError::Error IChirp::start() {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE; // Cannot start if not properly initialized
    }
    _impl->start();
    return ChirpError::SUCCESS;
}

ChirpError::Error IChirp::shutdown() {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE; // Cannot shutdown if not properly initialized
    }
    _impl->shutdown();
    return ChirpError::SUCCESS;
}

std::string IChirp::getServiceName() {
    if (!_impl) {
        return ""; // Return empty string if not properly initialized
    }
    return _impl->getServiceName();
}

ChirpError::Error IChirp::enqueMsg(std::string& msgName, std::vector<std::any>& args) {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    return _impl->enqueMsg(msgName, args);
}

ChirpError::Error IChirp::enqueSyncMsg(std::string& msgName, std::vector<std::any>& args) {
    if (!_impl) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    return _impl->enqueSyncMsg(msgName, args);
}

void IChirp::getCbMap(std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap) {
    if (!_impl) {
        funcMap = nullptr;
        return;
    }
    _impl->getCbMap(funcMap);
}

ChirpError::Error IChirp::addChirpTimer(IChirpTimer* timer) {
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

ChirpError::Error IChirp::removeChirpTimer(IChirpTimer* timer) {
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

