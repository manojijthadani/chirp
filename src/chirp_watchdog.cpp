#include <algorithm>
#include <thread>
#include <iostream>

#include "chirp_watchdog.hpp"
#include "ichirp.h"
#include "chirp_factory.h"
#include "chirp_logger.h"

// IChirpWatchDog constructor implementation
IChirpWatchDog::IChirpWatchDog(const std::string& name) {}

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
    for (auto& kv : _servicePetTimers) {
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

const IChirp* ChirpWatchDog::getChirpService() const {
    
    return _chirpService;
}

ChirpError::Error ChirpWatchDog::configure(IChirpFactory* factory,
                                           const std::chrono::milliseconds& petDuration) {
    //std::cout << "[ChirpWatchDog::configure] Configuring ChirpWatchDog service" << std::endl;
    //std::cout << "[ChirpWatchDog::configure] Configuring ChirpWatchDog service" << std::endl;
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
    
    //std::cout << "[ChirpWatchDog::start] Starting ChirpWatchDog service" << std::endl;
    //std::cout << "[ChirpWatchDog::start] Starting ChirpWatchDog service" << std::endl;
    if (!_chirpService) {
        //std::cout << "[ChirpWatchDog::start] Chirp service not found" << std::endl;
        std::cout << "[ChirpWatchDog::start] Chirp service not found" << std::endl;
        return ChirpError::INVALID_SERVICE_STATE;
    }
    
    auto e = _chirpService->start();
    if (e != ChirpError::SUCCESS) 
    {
        //std::cout << "[ChirpWatchDog::start] Failed to start Chirp service: " << static_cast<int>(e) << std::endl;
        std::cout << "[ChirpWatchDog::start] Failed to start Chirp service: " << static_cast<int>(e) << std::endl;
        return e;
    }
    
    // Start all pet timers on their respective services
    for (auto& kv : _servicePetTimers) {
        if (kv.second) {
            //std::cout << "[ChirpWatchDog::start] Starting pet timer for: " << kv.first << std::endl;
            //std::cout << "[ChirpWatchDog::start] Starting pet timer for: " << kv.first << std::endl;
            kv.second->start();
            
            // Now add to service AFTER it's started (so it's marked as RUNNING)
            IChirp* svc = _factory->getService(kv.first);
            if (svc) {
                //std::cout << "[ChirpWatchDog::start] Adding pet timer to service: " << kv.first << std::endl;
                //std::cout << "[ChirpWatchDog::start] Adding pet timer to service: " << kv.first << std::endl;
                svc->addChirpTimer(kv.second);
            }
        }
    }
    
    if (_monitorTimer) {
        //std::cout << "[ChirpWatchDog::start] Starting monitor timer" << std::endl;
        //std::cout << "[ChirpWatchDog::start] Starting monitor timer" << std::endl;
        _monitorTimer->start();
        
        // Now add to service AFTER it's started (so it's marked as RUNNING)
        _chirpService->addChirpTimer(_monitorTimer);
    } 
    
    return ChirpError::SUCCESS;
}

ChirpError::Error ChirpWatchDog::stop() {
    
    if (!_chirpService) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    
    for (auto& [name, t] : _servicePetTimers) {
        if (t) {
            t->stop();
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
    //std::cout << "[ChirpWatchDog::installPetTimers] Installing pet timers" << std::endl;
    //std::cout << "[ChirpWatchDog::installPetTimers] Installing pet timers" << std::endl;
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
                ChirpError::Error handlerErr = svc->registerMsgHandler(timerMsgName, 
                                                                       this, 
                                                                       &ChirpWatchDog::onPetTimerFired);
                //std::cout << "[ChirpWatchDog::installPetTimers] Registered handler for: " << timerMsgName 
                //          << " (service: " << name << "), error: " << static_cast<int>(handlerErr) << std::endl;
                //std::cout << "[ChirpWatchDog::installPetTimers] Registered handler for: " << timerMsgName 
                //          << " (service: " << name << "), error: " << static_cast<int>(handlerErr) << std::endl;
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
    if (!_chirpService) {
        return;
    }
    //std::cout << "[ChirpWatchDog::installMonitorTimer] Installing monitor timer" << std::endl;
    std::cout << "[ChirpWatchDog::installMonitorTimer] Installing monitor timer" << std::endl;
    
    (void)_chirpService->registerMsgHandler("monitorTimerElapsed", this, &ChirpWatchDog::onMonitorTick);
    
    _monitorTimer = new ChirpTimer("monitorTimerElapsed", 2 * _petDuration);
    //std::cout << "[ChirpWatchDog::installMonitorTimer] Created monitor timer with message='monitorTimerElapsed', duration=" 
    //          << (2 * _petDuration).count() << "ms" << std::endl;
    
    // Timer will be added to service in start() after it's started
}

ChirpError::Error ChirpWatchDog::onPetTimerFired(const std::string& timerMsgName) {
    //std::cout << "[ChirpWatchDog::onPetTimerFired] **CALLED** with: " << timerMsgName << std::endl;
    std::cout << "[ChirpWatchDog::onPetTimerFired] **CALLED** with: " << timerMsgName << std::endl;
    // Extract the service name from the timer message name (e.g., "_PetTimer_Service1" -> "Service1")
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
    
    auto now = std::chrono::steady_clock::now();

    // Get all monitored services and check for missed pets
    std::vector<std::string> serviceNames = _factory->listServiceNames();
    
    //std::cout << "[ChirpWatchDog::onMonitorTick] ************Called at: " 
    //          << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() << "ms" << std::endl;
    //std::cout << "[ChirpWatchDog::onMonitorTick] ************Called at: " 
    //          << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() << "ms" << std::endl;
    
    if (!_factory) {
        return ChirpError::INVALID_SERVICE_STATE;
    }
    
    {
        std::lock_guard<std::mutex> lock(_petMutex);
        
        for (const auto& serviceName : serviceNames) {
            IChirp* svc = _factory->getService(serviceName);
            if (svc && svc->getWatchDogMonitoring()) {
                // Check if this service has been petted before
                auto it = _lastPetTime.find(serviceName);
                if (it == _lastPetTime.end()) {
                    // First time seeing this service - initialize pet time
                    _lastPetTime[serviceName] = now;
                } else {
                    // Check if service missed its petting
                    auto timeSinceLastPet = now - it->second;
                    auto threshold = 3 * _petDuration;  // Threshold is 2 * pet duration
                    
                    if (timeSinceLastPet > threshold) {
                        // Service missed its petting - post missed pet message
                        //std::cout << "[ChirpWatchDog::onMonitorTick] Service '" << serviceName 
                        //          << "' missed pet (time since last pet: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastPet).count() 
                        //          << "ms, threshold: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(threshold).count() 
                        //          << "ms)" << std::endl;
                        //std::cout << "[ChirpWatchDog::onMonitorTick] Service '" << serviceName 
                        //          << "' missed pet (time since last pet: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastPet).count() 
                        //          << "ms, threshold: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(threshold).count() 
                        //          << "ms)" << std::endl;
                        //std::cout << "[ChirpWatchDog::onMonitorTick] BEFORE postMsg - About to post MissedPetMessage" << std::endl;
                        ChirpError::Error postError = _chirpService->postMsg(IChirpWatchDog::MissedPetMessage, serviceName);
                        //std::cout << "[ChirpWatchDog::onMonitorTick] AFTER postMsg - postMsg returned error: " << postError << std::endl;
                    } else {
                        //std::cout << "[ChirpWatchDog::onMonitorTick] Service '" << serviceName 
                        //          << "' OK (time since last pet: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastPet).count() 
                        //          << "ms, threshold: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(threshold).count() 
                        //          << "ms)" << std::endl;
                        //std::cout << "[ChirpWatchDog::onMonitorTick] Service '" << serviceName 
                        //          << "' OK (time since last pet: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastPet).count() 
                        //          << "ms, threshold: " 
                        //          << std::chrono::duration_cast<std::chrono::milliseconds>(threshold).count() 
                        //          << "ms)" << std::endl;
                    }
                }
            }
        }
    }
    
    return ChirpError::SUCCESS;
}



