#include <algorithm>
#include <thread>
#include <iostream>

#include "chirp_watchdog.hpp"
#include "ichirp.h"
#include "chirp_factory.h"
#include "chirp_logger.h"

// IChirpWatchDog constructor implementation
IChirpWatchDog::IChirpWatchDog(const std::string& name) {}

// Static factory that hides concrete type from callers
IChirpWatchDog* IChirpWatchDog::createWatchdog(const std::string& name) {
    
    ChirpWatchDog* wd = new (std::nothrow) ChirpWatchDog(name);
    return static_cast<IChirpWatchDog*>(wd);
}

ChirpWatchDog::ChirpWatchDog(const std::string& name) 
    : IChirpWatchDog(name), _factory(nullptr), _petDuration(0) {
 
    ChirpError::Error error;
    _chirpService = new IChirp(name, error);
}

ChirpWatchDog::~ChirpWatchDog() {
 
    if (_chirpService) {
        _chirpService->shutdown();
        delete _chirpService;
        _chirpService = nullptr;
    }
    
    // Clean up pet timers
    for (const auto& kv : _servicePetTimers) {
        if (kv.second) {
            delete kv.second;
        }
    }
    _servicePetTimers.clear();
    
    // Clean up monitor timer
    if (_monitorTimer) {
        delete _monitorTimer;
        _monitorTimer = nullptr;
    }
}

IChirp* ChirpWatchDog::getChirpService() {

    return _chirpService;
}

ChirpError::Error ChirpWatchDog::configure(IChirpFactory* factory,
                                           const std::chrono::milliseconds& petDuration) {
    
    if (!factory || petDuration.count() <= 0) {
        return ChirpError::INVALID_CONFIGURATION;
    }
    _factory = factory;
    _petDuration = petDuration;
    
    installPetTimers();
    installMonitorTimer();
    
    return ChirpError::SUCCESS;
}

ChirpError::Error ChirpWatchDog::start() {
    
    if (!_chirpService) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    
    auto e = _chirpService->start();
    if (e != ChirpError::SUCCESS) {
        return e;
    }
    
    // Start all pet timers on their respective services
    for (auto& kv : _servicePetTimers) {
        if (kv.second) {
            kv.second->start();
            
            // Now add to service AFTER it's started (so it's marked as RUNNING)
            IChirp* svc = _factory->getService(kv.first);
            if (svc) {
                svc->addChirpTimer(kv.second);
            }
        }
    }
    
    if (_monitorTimer) {
        _monitorTimer->start();
        
        // Now add to service AFTER it's started (so it's marked as RUNNING)
        _chirpService->addChirpTimer(_monitorTimer);
    } 
    
    return ChirpError::SUCCESS;
}

ChirpError::Error ChirpWatchDog::stop() {
    
    if (!_chirpService) 
        return ChirpError::INVALID_SERVICE_STATE;
    
    for (auto& [name, timer] : _servicePetTimers) {
        if (timer) {
            timer->stop();
        }
    }

    if (_monitorTimer) {
        _monitorTimer->stop();
        _chirpService->removeChirpTimer(_monitorTimer);
    }
    
    uninstallPetTimers();
    
    return _chirpService->shutdown();
}

void ChirpWatchDog::installPetTimers() {

    if (!_factory) {
        return;
    }
    
    uninstallPetTimers();
    
    std::vector<std::string> serviceNames = _factory->listServiceNames();
    
    for (const auto& name : serviceNames) {
        IChirp* svc = _factory->getService(name);
        
        if (!svc || !svc->getWatchDogMonitoring()) {
            continue;
        }

        if (_servicePetTimers.find(name) == _servicePetTimers.end()) {
            std::string timerMsgName = "_PetTimer_" + name;
            auto timer = new ChirpTimer(timerMsgName, _petDuration);
            if (timer) {
                _servicePetTimers[name] = timer;
                // Timer will be added to service in start() after it's started
                
                // Register a handler on the watched service for this timer
                (void)svc->registerMsgHandler(timerMsgName, 
                                             this, 
                                             &ChirpWatchDog::onPetTimerFired);
            }
        }
    }
}

void ChirpWatchDog::uninstallPetTimers() {

    for (auto& kv : _servicePetTimers) {
        const std::string& name = kv.first;
        if (_factory) {
            IChirp* svc = _factory->getService(name);
            if (svc && kv.second) {
                kv.second->stop();
                svc->removeChirpTimer(kv.second);
                delete kv.second;
            }
        }
    }
    _servicePetTimers.clear();
}

void ChirpWatchDog::installMonitorTimer() {

    if (!_chirpService) 
        return;
        
    (void)_chirpService->registerMsgHandler("monitorTimerElapsed", this, &ChirpWatchDog::onMonitorTick);
    
    _monitorTimer = new ChirpTimer("monitorTimerElapsed", 2 * _petDuration);
}

ChirpError::Error ChirpWatchDog::onPetTimerFired(const std::string& timerMsgName) {
    
    std::string servicePrefix = "_PetTimer_";
    if (timerMsgName.substr(0, servicePrefix.length()) != servicePrefix) {
        return ChirpError::INVALID_ARGUMENTS;
    }
    
    std::string serviceName = timerMsgName.substr(servicePrefix.length());
    
    // Record that the service was "petted" (timer fired, meaning service is responsive)
    std::lock_guard<std::mutex> lock(_petMutex);
    _lastPetTime[serviceName] = std::chrono::steady_clock::now();
    
    return ChirpError::SUCCESS;
}

ChirpError::Error ChirpWatchDog::onMonitorTick(const std::string& timerMessage) {
    
    if (!_factory) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    
    auto now = std::chrono::steady_clock::now();

    // Get all monitored services and check for missed pets
    std::vector<std::string> serviceNames = _factory->listServiceNames();
    
    std::lock_guard<std::mutex> lock(_petMutex);
        
    for (const auto& serviceName : serviceNames) {
        IChirp* svc = _factory->getService(serviceName);
        if (svc && svc->getWatchDogMonitoring()) {
            auto it = _lastPetTime.find(serviceName);
            if (it == _lastPetTime.end()) {
                _lastPetTime[serviceName] = now;
            } else {
                // Check if service missed its petting
                auto timeSinceLastPet = now - it->second;
                auto threshold = 2.1 * _petDuration;  // Threshold is 2 * pet duration
                    
                if (timeSinceLastPet > threshold) {
                    (void)_chirpService->postMsg(IChirpWatchDog::MissedPetMessage, serviceName);
                } 
            }
        }
    }
    return ChirpError::SUCCESS;
}



