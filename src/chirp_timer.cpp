/**
 * @file chirp_timer.cpp
 * @brief Implementation of ChirpTimer concrete class
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 */

#include "chirp_timer.h"
#include "chirp_logger.h"
#include "ichirp.h"
#include <iostream>


ChirpTimer::ChirpTimer(std::string messageToDeliver, const std::chrono::milliseconds& duration) 
    : _messageToDeliver(messageToDeliver)
    , _duration(duration)
    , _state(TimerState::STOPPED)
    , _shouldStop(false) {
     
}

ChirpTimer::~ChirpTimer() {
    // Ensure timer is stopped before destruction
    stop();
}

ChirpError::Error ChirpTimer::configure( std::string messageToDeliver,
                                       const std::chrono::milliseconds& duration) {
                                        
    ChirpError::Error result = ChirpError::SUCCESS;
    std::lock_guard<std::mutex> lock(_configMutex);
    
    // Check if timer is running
    result = validateConfiguration();
    if (result == ChirpError::SUCCESS) {
        if (messageToDeliver.empty() || duration.count() <= 0) {
            ChirpLogger::instance("ChirpTimer") << "Invalid message to deliver or duration" << std::endl;
            result = ChirpError::INVALID_ARGUMENTS;
        }
        else {
            _messageToDeliver = messageToDeliver;
            _duration = duration;
        }
    }
    
    return result;
}

ChirpError::Error ChirpTimer::start() {

    ChirpError::Error result = ChirpError::SUCCESS;
    std::lock_guard<std::mutex> lock(_configMutex);
    
    // Validate configuration
    result = validateConfiguration();
    
    // Check if timer is already running
    if (result == ChirpError::SUCCESS) {
        
        // Record the start time
        _startTime = std::chrono::steady_clock::now();
        _state = TimerState::RUNNING;
    }
    
    return result;
}

ChirpError::Error ChirpTimer::stop() {

    ChirpError::Error result = ChirpError::SUCCESS;
    
    std::lock_guard<std::mutex> lock(_configMutex);
            
    _state = TimerState::STOPPING;
    _shouldStop = true;
    
    return result;
}

bool ChirpTimer::isRunning() const {

    return _state.load() == TimerState::RUNNING;
}

std::chrono::milliseconds ChirpTimer::getDuration() const {

    std::lock_guard<std::mutex> lock(_configMutex);
    return _duration;
}

ChirpError::Error ChirpTimer::validateConfiguration() const {

    ChirpError::Error result = ChirpError::SUCCESS;
    
    // Check if timer is running
    if (_state == TimerState::RUNNING) {
        ChirpLogger::instance("ChirpTimer") << "Cannot configure timer while it is running" << std::endl;
        result = ChirpError::INVALID_SERVICE_STATE;
    }
    // Validate duration
    else if (_duration.count() <= 0) {
        ChirpLogger::instance("ChirpTimer") << "Invalid timer duration: " << _duration.count() << "ms" << std::endl;
        result = ChirpError::INVALID_ARGUMENTS;
    }
    
    return result;
}


std::chrono::steady_clock::time_point ChirpTimer::getTimerStartTime() const {

    std::lock_guard<std::mutex> lock(_configMutex);
    return _startTime;
}

std::string ChirpTimer::getMessage() const {

    std::lock_guard<std::mutex> lock(_configMutex);
    return _messageToDeliver;
}
