#include <iostream>
#include <thread>
#include <chrono>

#include "chirp.h"
#include "chirp_error.h"

class SimpleHandlers {
public:
    void AsyncHandler(const std::string& message) {
        std::cout << "[ASYNC] Received message: " << message << std::endl;
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "[ASYNC] Finished processing: " << message << std::endl;
    }
    
    void SyncHandler(const std::string& message, int value) {
        std::cout << "[SYNC] Processing message: " << message << " with value: " << value << std::endl;
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "[SYNC] Completed processing: " << message << std::endl;
    }
};

int main() {
    std::cout << "=== Simple Chirp Example ===" << std::endl;
    std::cout << "Chirp API version: " << Chirp::getVersion() << std::endl;
    
    // Create a Chirp service
    ChirpError::Error error;
    Chirp service("DemoService", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create service: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    SimpleHandlers handlers;
    
    // Register message handlers
    error = service.registerMsgHandler("AsyncMessage", &handlers, &SimpleHandlers::AsyncHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register AsyncMessage handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = service.registerMsgHandler("SyncMessage", &handlers, &SimpleHandlers::SyncHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register SyncMessage handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Start the service
    service.start();
    std::cout << "Service started successfully" << std::endl;
    
    std::cout << "\n--- Demonstrating postMsg (Asynchronous) ---" << std::endl;
    
    // Send one asynchronous message - this doesn't block
    std::cout << "Sending async message..." << std::endl;
    error = service.postMsg("AsyncMessage", std::string("Hello Async World!"));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post async message: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    std::cout << "Async message sent immediately (non-blocking)" << std::endl;
    
    // Give some time for async message to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    std::cout << "\n--- Demonstrating syncMsg (Synchronous) ---" << std::endl;
    
    // Send one synchronous message - this blocks until handler completes
    std::cout << "Sending sync message (this will block)..." << std::endl;
    error = service.syncMsg("SyncMessage", std::string("Hello Sync World!"), 42);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post sync message: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    std::cout << "Sync message completed" << std::endl;  
        
    // Shutdown the service
    std::cout << "\nShutting down service..." << std::endl;
    service.shutdown();
    std::cout << "Service shut down successfully" << std::endl;
    
    std::cout << "=== Example completed ===" << std::endl;
    return 0;
} 