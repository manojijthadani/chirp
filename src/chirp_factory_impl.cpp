/**
 * @file chirp_factory_impl.cpp
 * @brief Implementation of ChirpFactory concrete class
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 */

#include "chirp_factory.h"
#include "ichirp_factory.h"
#include "ichirp.h"
#include "chirp_logger.h"

// Static member initialization
const std::string ChirpFactory::_version = "1.0";

// Interface-level singleton accessor
IChirpFactory& IChirpFactory::getInstance() {
    return ChirpFactory::getInstance();
}

// Concrete singleton accessor
ChirpFactory& ChirpFactory::getInstance() {
    static ChirpFactory instance;
    return instance;
}

ChirpError::Error ChirpFactory::createService(const std::string& service_name, IChirp** service) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Initialize the output parameter
    *service = nullptr;
    
    // Check if service already exists
    auto it = _services.find(service_name);
    if (it != _services.end()) {
        ChirpLogger::instance("ChirpFactory") << "Service '" << service_name << "' already exists" << std::endl;
        return ChirpError::SERVICE_ALREADY_EXISTS;
    }
    
    // Create new service
    ChirpError::Error error = ChirpError::SUCCESS;
    auto newService = new (std::nothrow) IChirp(service_name, error);
    if (!newService) {
        ChirpLogger::instance("ChirpFactory") << "Failed to allocate memory for service '" << service_name << "'" << std::endl;
        return ChirpError::RESOURCE_ALLOCATION_FAILED;
    }
    
    if (error != ChirpError::SUCCESS) {
        delete newService; // Clean up the allocated service
        ChirpLogger::instance("ChirpFactory") << "Failed to initialize service '" << service_name << "' with error: " << ChirpError::errorToString(error) << std::endl;
        return error; // Return the error from the Chirp constructor
    }
    
    // Store the service in our map
    _services[service_name] = std::shared_ptr<IChirp>(newService);
    
    // Set the output parameter to point to the created service
    *service = newService;
    
    ChirpLogger::instance("ChirpFactory") << "Created service '" << service_name << "'" << std::endl;
    return ChirpError::SUCCESS;
}

IChirp* ChirpFactory::getService(const std::string& service_name) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _services.find(service_name);
    if (it != _services.end()) {
        return it->second.get();
    }
    
    ChirpLogger::instance("ChirpFactory") << "Service '" << service_name << "' not found" << std::endl;
    return nullptr;
}

bool ChirpFactory::destroyService(const std::string& service_name) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _services.find(service_name);
    if (it != _services.end()) {
        // Shutdown the service before removing it
        it->second->shutdown();
        _services.erase(it);
        ChirpLogger::instance("ChirpFactory") << "Destroyed service '" << service_name << "'" << std::endl;
        return true;
    }
    
    ChirpLogger::instance("ChirpFactory") << "Service '" << service_name << "' not found for destruction" << std::endl;
    return false;
}

size_t ChirpFactory::getServiceCount() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _services.size();
}

std::vector<std::string> ChirpFactory::listServiceNames() const {
    std::lock_guard<std::mutex> lock(_mutex);
    std::vector<std::string> names;
    names.reserve(_services.size());
    for (const auto& kv : _services) {
        names.push_back(kv.first);
    }
    return names;
}

void ChirpFactory::shutdownAllServices() {
    std::lock_guard<std::mutex> lock(_mutex);
    
    ChirpLogger::instance("ChirpFactory") << "Shutting down " << _services.size() << " services" << std::endl;
    
    for (auto& pair : _services) {
        ChirpLogger::instance("ChirpFactory") << "Shutting down service '" << pair.first << "'" << std::endl;
        pair.second->shutdown();
    }
    
    _services.clear();
    ChirpLogger::instance("ChirpFactory") << "All services shut down" << std::endl;
}

const std::string& ChirpFactory::getVersion() const {
    return _version;
} 