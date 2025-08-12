#include <iostream>
#include <thread>
#include <chrono>
#include "ichirp_factory.h"
#include "chirp_factory.h"
#include "chirp.h"
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
    
    // Get the singleton instance of ChirpFactory
    auto& factory = ChirpFactory::getInstance();
    std::cout << "ChirpFactory version: " << factory.getVersion() << std::endl;
    
    // Demonstrate interface usage - we can use the interface pointer
    IChirpFactory* factory_interface = &factory;
    std::cout << "Interface version: " << factory_interface->getVersion() << std::endl;
    
    // Create multiple services using the factory
    ChirpError::Error error;
    auto service1 = factory.createService("LoggerService", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create LoggerService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    auto service2 = factory.createService("NetworkService", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create NetworkService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    auto service3 = factory.createService("DatabaseService", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create DatabaseService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Demonstrate using the interface for service creation
    auto service4 = factory_interface->createService("InterfaceService", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create InterfaceService: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Create handler instance
    FactoryServiceHandlers handlers;
    
    // Register message handlers for each service
    service1->registerMsgHandler("Greeting", &handlers, &FactoryServiceHandlers::GreetingHandler);
    service1->registerMsgHandler("Status", &handlers, &FactoryServiceHandlers::StatusHandler);
    
    service2->registerMsgHandler("Greeting", &handlers, &FactoryServiceHandlers::GreetingHandler);
    service2->registerMsgHandler("Shutdown", &handlers, &FactoryServiceHandlers::ShutdownHandler);
    
    service3->registerMsgHandler("Status", &handlers, &FactoryServiceHandlers::StatusHandler);
    service4->registerMsgHandler("Greeting", &handlers, &FactoryServiceHandlers::GreetingHandler);
    
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
    
    // Try to create a service with the same name (should return existing)
    auto duplicate_service = factory.createService("LoggerService", error);
    if (duplicate_service == service1) {
        std::cout << "Factory correctly returned existing LoggerService" << std::endl;
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