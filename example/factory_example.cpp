#include <iostream>
#include <thread>
#include <chrono>
#include "ichirp_factory.h"
#include "ichirp.h"
#include "chirp_error.h"

class FactoryServiceHandlers {
public:
    void GreetingHandler(const std::string& name) {
        std::cout << "Hello, " << name << "! Welcome to ChirpFactory!" << std::endl;
    }
    
    void StatusHandler(const std::string& service_name, int status_code) {
        std::cout << "Service '" << service_name << "' status: " << status_code << std::endl;
    }
    
    void ShutdownHandler() {
        std::cout << "Service is shutting down gracefully..." << std::endl;
    }
};

int main() {
    std::cout << "=== ChirpFactory Example ===" << std::endl;
    
    // Get the singleton instance via interface
    IChirpFactory& factory = IChirpFactory::getInstance();
    std::cout << "ChirpFactory version: " << factory.getVersion() << std::endl;
    
    // Demonstrate interface pointer usage
    IChirpFactory* factory_interface = &factory;
    std::cout << "Interface version: " << factory_interface->getVersion() << std::endl;
    
    // Create multiple services using the factory
    ChirpError::Error error;
    IChirp* service1 = nullptr;
    error = factory.createService("LoggerService", &service1);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create LoggerService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    IChirp* service2 = nullptr;
    error = factory.createService("NetworkService", &service2);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create NetworkService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    IChirp* service3 = nullptr;
    error = factory.createService("DatabaseService", &service3);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create DatabaseService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Demonstrate using the interface for service creation
    IChirp* service4 = nullptr;
    error = factory_interface->createService("InterfaceService", &service4);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create InterfaceService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Create handler instance
    FactoryServiceHandlers handlers;
    
    // Register message handlers for each service
    error = service1->registerMsgHandler("Greeting", &handlers, &FactoryServiceHandlers::GreetingHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register Greeting handler for LoggerService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = service1->registerMsgHandler("Status", &handlers, &FactoryServiceHandlers::StatusHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register Status handler for LoggerService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = service2->registerMsgHandler("Greeting", &handlers, &FactoryServiceHandlers::GreetingHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register Greeting handler for NetworkService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = service2->registerMsgHandler("Shutdown", &handlers, &FactoryServiceHandlers::ShutdownHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register Shutdown handler for NetworkService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = service3->registerMsgHandler("Status", &handlers, &FactoryServiceHandlers::StatusHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register Status handler for DatabaseService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = service4->registerMsgHandler("Greeting", &handlers, &FactoryServiceHandlers::GreetingHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register Greeting handler for InterfaceService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Start all services
    service1->start();
    service2->start();
    service3->start();
    service4->start(); 
    std::cout << "Created " << factory.getServiceCount() << " services" << std::endl; 
    
    // Post messages to different services with error handling
    error = service1->postMsg("Greeting", std::string("Alice"));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post Greeting to LoggerService: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = service2->postMsg("Greeting", std::string("Bob"));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post Greeting to NetworkService: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = service1->postMsg("Status", std::string("LoggerService"), 200);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post Status to LoggerService: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = service3->postMsg("Status", std::string("DatabaseService"), 404);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post Status to DatabaseService: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = service4->postMsg("Greeting", std::string("Interface User"));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post Greeting to InterfaceService: " << ChirpError::errorToString(error) << std::endl;
    }
    
    // Demonstrate retrieving a service by name
    auto retrieved_service = factory.getService("LoggerService");
    if (retrieved_service) {
        std::cout << "Successfully retrieved LoggerService" << std::endl;
        retrieved_service->postMsg("Greeting", std::string("Charlie"));
    }
    
    // Try to get a non-existent service
    auto non_existent = factory.getService("NonExistentService");
    if (!non_existent) {
        std::cout << "Non-existent service correctly returned nullptr" << std::endl;
    }
    
    // Wait for messages to be processed
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Demonstrate destroying a specific service
    std::cout << "Destroying NetworkService..." << std::endl;
    bool destroyed = factory.destroyService("NetworkService");
    if (destroyed) {
        std::cout << "NetworkService destroyed successfully" << std::endl;
        std::cout << "Remaining services: " << factory.getServiceCount() << std::endl;
    }
    
    // Try to create a service with the same name (should return error)
    IChirp* duplicate_service = nullptr;
    error = factory.createService("LoggerService", &duplicate_service);
    if (error == ChirpError::SERVICE_ALREADY_EXISTS) {
        std::cout << "Factory correctly returned SERVICE_ALREADY_EXISTS error for duplicate service" << std::endl;
    }
    
    // Wait a bit more to see all the messages being processed
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Shutdown all remaining services using the factory
    std::cout << "Shutting down all services..." << std::endl;
    factory.shutdownAllServices();
    
    std::cout << "Final service count: " << factory.getServiceCount() << std::endl;
    std::cout << "=== Example completed ===" << std::endl;
    
    return 0;
} 