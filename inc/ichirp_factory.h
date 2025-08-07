/**
 * @file ichirp_factory.h
 * @brief Abstract factory interface for creating Chirp services
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 * 
 * This file defines the IChirpFactory interface for creating and managing Chirp service instances.
 * This interface ensures that different factory implementations can be used
 * while maintaining the same API contract.
 */

#pragma once
#include <memory>
#include <string>

// Forward declaration to prevent inclusion of private headers
class Chirp;

/**
 * @brief Abstract factory interface for creating and managing Chirp services
 * 
 * IChirpFactory defines the interface for creating and managing Chirp service instances.
 * This interface ensures that different factory implementations can be used
 * while maintaining the same API contract.
 * 
 * @note Implementations should be thread-safe
 * 
 * @example
 * @code
 * IChirpFactory* factory = &ChirpFactory::getInstance();
 * auto service = factory->createService("MyService");
 * service->start();
 * @endcode
 */
class IChirpFactory {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~IChirpFactory() = default;

    /**
     * @brief Create a new Chirp service instance
     * @param service_name The name of the service to create
     * @return Shared pointer to the created Chirp service
     */
    virtual std::shared_ptr<Chirp> createService(const std::string& service_name) = 0;

    /**
     * @brief Get an existing service by name
     * @param service_name The name of the service to retrieve
     * @return Shared pointer to the service if it exists, nullptr otherwise
     */
    virtual std::shared_ptr<Chirp> getService(const std::string& service_name) = 0;

    /**
     * @brief Destroy a service by name
     * @param service_name The name of the service to destroy
     * @return true if the service was found and destroyed, false otherwise
     */
    virtual bool destroyService(const std::string& service_name) = 0;

    /**
     * @brief Get the total number of active services
     * @return Number of services currently managed by the factory
     */
    virtual size_t getServiceCount() const = 0;

    /**
     * @brief Shutdown all services managed by the factory
     */
    virtual void shutdownAllServices() = 0;

    /**
     * @brief Get the version of the factory implementation
     * @return The version string (e.g., "1.0")
     */
    virtual const std::string& getVersion() const = 0;
}; 