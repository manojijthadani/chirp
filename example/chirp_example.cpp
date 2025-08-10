#include <iostream>
#include <thread>
#include <chrono>

#include "chirp.h"

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
    Chirp service("DemoService");
    
    SimpleHandlers handlers;
    
    // Register message handlers
    service.registerMsgHandler("AsyncMessage", &handlers, &SimpleHandlers::AsyncHandler);
    service.registerMsgHandler("SyncMessage", &handlers, &SimpleHandlers::SyncHandler);
    
    // Start the service
    service.start();
    std::cout << "Service started successfully" << std::endl;
    
    std::cout << "\n--- Demonstrating postMsg (Asynchronous) ---" << std::endl;
    
    // Send one asynchronous message - this doesn't block
    std::cout << "Sending async message..." << std::endl;
    service.postMsg("AsyncMessage", std::string("Hello Async World!"));
    std::cout << "Async message sent immediately (non-blocking)" << std::endl;
    
    // Give some time for async message to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    std::cout << "\n--- Demonstrating syncMsg (Synchronous) ---" << std::endl;
    
    // Send one synchronous message - this blocks until handler completes
    std::cout << "Sending sync message (this will block)..." << std::endl;
    service.syncMsg("SyncMessage", std::string("Hello Sync World!"), 42);
    std::cout << "Sync message completed" << std::endl;  
        
    // Shutdown the service
    std::cout << "\nShutting down service..." << std::endl;
    service.shutdown();
    std::cout << "Service shut down successfully" << std::endl;
    
    std::cout << "=== Example completed ===" << std::endl;
    return 0;
} 