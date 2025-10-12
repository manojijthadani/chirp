/**
 * @file chirp_timer.cpp
 * @brief Implementation of ChirpTimer concrete class
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 */

#include "chirp_timer.h"
#include "chirp_logger.h"
#include "chirp.h"
#include <iostream>


ChirpTimer::ChirpTimer(const std::string& name) 
    : _name(name)
    , _mode(TimerMode::ONE_TIME)  // Default mode
    , _duration(std::chrono::milliseconds(1000))  // Default duration
    , _chirpObj(nullptr)
    , _messageName("")
    , _state(TimerState::STOPPED)
    , _shouldStop(false) {
    
    std::string timerIdentifier = _name.empty() ? "unnamed" : _name;
    ChirpLogger::instance("ChirpTimer") << "Timer '" << timerIdentifier 
                                        << "' created (default configuration)" << std::endl;
}

ChirpTimer::~ChirpTimer() {
    // Ensure timer is stopped before destruction
    stop();
}

ChirpError::Error ChirpTimer::configure(TimerMode mode, 
                                       const std::chrono::milliseconds& duration,
                                       Chirp* chirpObj, 
                                       const std::string& messageName) {
    ChirpError::Error result = ChirpError::SUCCESS;
    std::lock_guard<std::mutex> lock(_configMutex);
    
    // Check if timer is running
    if (_state == TimerState::RUNNING) {
        ChirpLogger::instance("ChirpTimer") << "Cannot configure timer while it is running" << std::endl;
        result = ChirpError::INVALID_SERVICE_STATE;
    }
    // Validate duration
    else if (duration.count() <= 0) {
        ChirpLogger::instance("ChirpTimer") << "Invalid timer duration: " << duration.count() << "ms" << std::endl;
        result = ChirpError::INVALID_ARGUMENTS;
    }
    // Validate chirp object
    else if (!chirpObj) {
        ChirpLogger::instance("ChirpTimer") << "Invalid chirp object (null pointer)" << std::endl;
        result = ChirpError::INVALID_ARGUMENTS;
    }
    // Validate message name
    else if (messageName.empty()) {
        ChirpLogger::instance("ChirpTimer") << "Invalid message name (empty)" << std::endl;
        result = ChirpError::INVALID_ARGUMENTS;
    }
    // All validations passed, set configuration parameters
    else {
        _mode = mode;
        _duration = duration;
        _chirpObj = chirpObj;
        _messageName = messageName;
        
        ChirpLogger::instance("ChirpTimer") << "Timer configured with mode " 
            << (mode == TimerMode::ONE_TIME ? "ONE_TIME" : "CONTINUOUS") 
            << ", duration " << duration.count() << "ms"
            << ", message '" << messageName << "'" << std::endl;
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
        TimerState expectedState = TimerState::STOPPED;
        if (!_state.compare_exchange_strong(expectedState, TimerState::STARTING)) {
            ChirpLogger::instance("ChirpTimer") << "Timer is already running or starting" << std::endl;
            result = ChirpError::INVALID_SERVICE_STATE;
        }
        // Proceed with starting the timer
        else {
            _shouldStop = false;
            
            try {
                // Create and start the timer thread
                _timerThread = std::thread(&ChirpTimer::timerThreadFunction, this);
                _state = TimerState::RUNNING;
                ChirpLogger::instance("ChirpTimer") << "Timer started with duration "
                                                    << _duration.count() << "ms" << std::endl;
            } catch (const std::exception& e) {
                _state = TimerState::STOPPED;
                ChirpLogger::instance("ChirpTimer") << "Failed to start timer thread: " 
                                                    << e.what() << std::endl;
                result = ChirpError::THREAD_ERROR;
            }
        }
    }
    
    return result;
}

ChirpError::Error ChirpTimer::stop() {
    ChirpError::Error result = ChirpError::SUCCESS;
    
    std::lock_guard<std::mutex> lock(_configMutex);
            
    TimerState currentState = _state.load();
    if (currentState == TimerState::STOPPED) {
        ChirpLogger::instance("ChirpTimer") << "Timer is already stopped" << std::endl;
    } else if (currentState == TimerState::STOPPING) {
        ChirpLogger::instance("ChirpTimer") << "Timer is already stopping" << std::endl;
    } else {
        _state = TimerState::STOPPING;
        _shouldStop = true;
        ChirpLogger::instance("ChirpTimer") << "Stopping timer..." << std::endl;
    }
    
    // Only proceed with thread cleanup if we actually started stopping
    if (_state.load() == TimerState::STOPPING) {
        // Notify the timer thread to wake up (must hold the mutex)
        {
            std::lock_guard<std::mutex> stopLock(_stopMutex);
            _stopCondition.notify_all();
        }
        
        // Wait for the timer thread to finish
        if (_timerThread.joinable()) {
            _timerThread.join();
        }
        
        _state = TimerState::STOPPED;
        ChirpLogger::instance("ChirpTimer") << "Timer stopped" << std::endl;
    }
    
    return result;
}

bool ChirpTimer::isRunning() const {
    return _state.load() == TimerState::RUNNING;
}

TimerMode ChirpTimer::getMode() const {
    std::lock_guard<std::mutex> lock(_configMutex);
    return _mode;
}

std::chrono::milliseconds ChirpTimer::getDuration() const {
    std::lock_guard<std::mutex> lock(_configMutex);
    return _duration;
}

std::string ChirpTimer::getName() const {
    std::lock_guard<std::mutex> lock(_configMutex);
    return _name;
}


void ChirpTimer::timerThreadFunction() {
    
    ChirpLogger::instance("ChirpTimer") << "Timer thread started" << std::endl;
    
    while (!_shouldStop.load()) {
        // Wait for the duration or until stop is signaled
        std::unique_lock<std::mutex> lock(_stopMutex);
        bool timeout = _stopCondition.wait_for(lock, 
                                               _duration, 
                                               [this] { return _shouldStop.load(); });
        
        // If we timed out (didn't get stop signal), send message to chirp service
        if (!timeout && !_shouldStop.load()) {
            if (_chirpObj && !_messageName.empty()) {
                try {
                    ChirpLogger::instance("ChirpTimer") << "Timer fired, sending message '" << _messageName << "' to chirp service" << std::endl;
                    
                    // Send the message using the stored arguments
                    ChirpError::Error result = sendMessage();
                    if (result != ChirpError::SUCCESS) {
                        ChirpLogger::instance("ChirpTimer") << "Error sending message to chirp service: " 
                            << ChirpError::errorToString(result) << std::endl;
                    }
                } catch (const std::exception& e) {
                    ChirpLogger::instance("ChirpTimer") << "Error sending message to chirp service: " << e.what() << std::endl;
                }
            }
            
            // If this is a one-time timer, break out of the loop
            if (_mode == TimerMode::ONE_TIME) {
                ChirpLogger::instance("ChirpTimer") << "One-time timer completed" << std::endl;
                break;
            }
        }
    }
    
    ChirpLogger::instance("ChirpTimer") << "Timer thread finished" << std::endl;
}

ChirpError::Error ChirpTimer::validateConfiguration() const {
    ChirpError::Error result = ChirpError::SUCCESS;
    
    if (_duration.count() <= 0) {
        ChirpLogger::instance("ChirpTimer") << "Invalid timer duration: " << _duration.count() << "ms" << std::endl;
        result = ChirpError::INVALID_CONFIGURATION;
    }
    else if (!_chirpObj || _messageName.empty()) {
        ChirpLogger::instance("ChirpTimer") << "Timer must be configured with chirp message before starting" << std::endl;
        result = ChirpError::INVALID_CONFIGURATION;
    }
    
    return result;
}

ChirpError::Error ChirpTimer::sendMessage() {
    ChirpError::Error result = ChirpError::SUCCESS;

    if (!_chirpObj || _messageName.empty()) {
        result = ChirpError::INVALID_CONFIGURATION;
    } else {
        try {
            result = _chirpObj->postMsg(_messageName);
        } catch (const std::exception& e) {
            ChirpLogger::instance("ChirpTimer") << "Exception in sendMessage: " << e.what() << std::endl;
            result = ChirpError::INTERNAL_ERROR;
        }
    }
    
    return result;
}
