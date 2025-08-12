/*
 * chirp_factory.h
 * Concrete factory implementation for creating Chirp services
 * 
 * This file defines the ChirpFactory concrete implementation of the IChirpFactory interface,
 * which provides a singleton factory for creating and managing Chirp service instances.
 * The factory pattern ensures centralized control over service creation and lifecycle management.
 */

#pragma once
#include "ichirp_factory.h"
#include <map>
#include <mutex>

/*
 * Concrete implementation of the ChirpFactory interface
 * 
 * ChirpFactory provides a singleton implementation of the IChirpFactory interface.
 * It ensures centralized control over service creation and can manage service
 * lifecycle, configuration, and resource allocation.
 * 
 * This class is thread-safe and follows the singleton pattern
 */
class ChirpFactory : public IChirpFactory {
public:
    /*
     * Get the singleton instance of ChirpFactory
     * Returns: Reference to the singleton instance
     */
    static ChirpFactory& getInstance();

    // Implementation of IChirpFactory interface
    Chirp* createService(const std::string& service_name, ChirpError::Error& error) override;
    Chirp* getService(const std::string& service_name) override;
    bool destroyService(const std::string& service_name) override;
    size_t getServiceCount() const override;
    void shutdownAllServices() override;
    const std::string& getVersion() const override;

private:
 
    ChirpFactory() = default;
    ~ChirpFactory() = default;
    ChirpFactory(const ChirpFactory&) = delete;
    ChirpFactory& operator=(const ChirpFactory&) = delete;
    ChirpFactory(ChirpFactory&&) = delete;
    ChirpFactory& operator=(ChirpFactory&&) = delete;

    static const std::string _version;
    std::map<std::string, std::shared_ptr<Chirp>> _services;
    mutable std::mutex _mutex;
}; 