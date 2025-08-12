/**
 * @file chirp_factory_impl.cpp
 * @brief Implementation of ChirpFactory concrete class
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 */

#include "chirp_factory.h"
#include "chirp.h"
#include "chirp_logger.h"

// Static member initialization
const std::string ChirpFactory::_version = "1.0";

ChirpFactory& ChirpFactory::getInstance() {
    static ChirpFactory instance;
    return instance;
}

Chirp* ChirpFactory::createService(const std::string& service_name, ChirpError::Error& error) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Check if service already exists
    auto it = _services.find(service_name);
    if (it != _services.end()) {
        ChirpLogger::instance("ChirpFactory") << "Service '" << service_name << "' already exists" << std::endl;
        error = ChirpError::SERVICE_ALREADY_EXISTS;
        return it->second.get();
    }
    
    // Create new service
    auto service = new (std::nothrow) Chirp(service_name, error);
    if (!service) {
        error = ChirpError::RESOURCE_ALLOCATION_FAILED;
        return nullptr;
    }
    if (error != ChirpError::SUCCESS) {
        delete service; // Clean up the allocated service
        return nullptr; // Error already set by Chirp constructor
    }
    _services[service_name] = std::shared_ptr<Chirp>(service);
    
    ChirpLogger::instance("ChirpFactory") << "Created service '" << service_name << "'" << std::endl;
    return service;
}

Chirp* ChirpFactory::getService(const std::string& service_name) {
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