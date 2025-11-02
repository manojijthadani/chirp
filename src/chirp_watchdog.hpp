#pragma once
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

#include "ichirp_watchdog.h"
#include "chirp_timer.h"

class ChirpWatchDog : public IChirpWatchDog {

public:
    explicit ChirpWatchDog(const std::string& name);
    ~ChirpWatchDog() override;

    ChirpError::Error configure(IChirpFactory* factory,
                                const std::chrono::milliseconds& petDuration) override;
    ChirpError::Error start() override;
    ChirpError::Error stop() override;
    
    // Getter for the internal Chirp service
    IChirp* getChirpService() override;

private:

    void installPetTimers();
    void uninstallPetTimers();
    void installMonitorTimer();
    ChirpError::Error onPetTimerFired(const std::string& serviceName);
    ChirpError::Error onMonitorTick(const std::string& timerMessage);
    
    // Pet handler members (for storing last pet times)
    std::map<std::string, std::chrono::steady_clock::time_point> _lastPetTime;
    std::mutex _petMutex;
    const std::map<std::string, std::chrono::steady_clock::time_point>& getLastPetTimes() const { return _lastPetTime; }

    // Internal Chirp service instance
    IChirp* _chirpService = nullptr;

    IChirpFactory* _factory = nullptr;
    std::chrono::milliseconds _petDuration{0};

    std::map<std::string, ChirpTimer*> _servicePetTimers;

    ChirpTimer* _monitorTimer = nullptr;
};



