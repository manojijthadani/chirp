#pragma once
#include <chrono>
#include <string>

#include "ichirp_factory.h"

// Forward declaration
class IChirp;

class IChirpWatchDog {
public:
    static constexpr const char* MissedPetMessage = "ChirpMissedPetting";

    explicit IChirpWatchDog(const std::string& name);
    virtual ~IChirpWatchDog() = default;

    virtual ChirpError::Error configure(IChirpFactory* factory,
                                        const std::chrono::milliseconds& petDuration) = 0;
    virtual ChirpError::Error start() = 0;
    virtual ChirpError::Error stop() = 0;
    
    // Getter for the internal Chirp service
    virtual IChirp* getChirpService() = 0;
    virtual const IChirp* getChirpService() const = 0;
};

