/**
 * @file chirp_watchdog_example.cpp
 * @brief Example demonstrating ChirpWatchDog monitoring unresponsive services
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 * 
 * This example shows how to:
 * 1. Create multiple Chirp services with handlers that simulate work
 * 2. Enable watchdog monitoring on services
 * 3. Use ChirpWatchDog to detect when services become unresponsive
 * 4. Observe behavior changes when service load increases
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <mutex>

#include "ichirp_watchdog.h"
#include "ichirp.h"
#include "chirp_factory.h"
#include "chirp_error.h"
#include "chirp_watchdog.hpp"

// Mutex for thread-safe console output
std::mutex consoleMutex;

void threadSafePrint(const std::string& message) {
    std::lock_guard<std::mutex> lock(consoleMutex);
    std::cout << message << std::endl;
    std::cout.flush();
}

std::string getCurrentTimeWithMsec() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::tm* timeinfo = std::localtime(&time_t_now);
    std::stringstream ss;
    ss << std::put_time(timeinfo, "%H:%M:%S");
    ss << ":" << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

class ServiceHandler {
public:
    explicit ServiceHandler(const std::string& name) : _serviceName(name), _messageCount(0), _responseTime(250) {}
    
    void setResponseTime(int milliseconds) {
        _responseTime = milliseconds;
    }
    
    void bindService(IChirp* service) { _service = service; }
    void setPostingRange(int startIndex, int endIndex) {
        _postRangeStart = startIndex;
        _postRangeEnd = endIndex;
    }
    
    ChirpError::Error handleMessage(const std::string& payload) {
        std::ostringstream oss;
        oss << "[" << getCurrentTimeWithMsec() << "] [" << _serviceName << "] Received message: " << payload
            << " (simulating " << _responseTime << "ms work)";
        threadSafePrint(oss.str());
        
        // Simulate work by sleeping
        std::this_thread::sleep_for(std::chrono::milliseconds(_responseTime));
        
        //std::cout << "[" << _serviceName << "] Completed message processing" << std::endl;
        //std::cout.flush();
        _messageCount++;
        
        // Chain next post if in range
        int idx = parsePacketIndex(payload);
        if (_service && idx >= _postRangeStart && idx < _postRangeEnd) {
            std::ostringstream oss;
            oss << "Data packet #" << (idx + 1);
            _service->postMsg("ProcessData", oss.str());
        }
        return ChirpError::SUCCESS;
    }
    
    int getMessageCount() const { return _messageCount; }
    
private:
    int parsePacketIndex(const std::string& text) const {
        std::size_t pos = text.find('#');
        if (pos == std::string::npos) return 0;
        try { return std::stoi(text.substr(pos + 1)); } catch (...) { return 0; }
    }
    std::string _serviceName;
    int _messageCount;
    int _responseTime;
    IChirp* _service = nullptr;
    int _postRangeStart = 0;
    int _postRangeEnd = 0;
};

class WatchdogHandler {
public:
    ChirpError::Error onMissedPet(const std::string& serviceName) {
        threadSafePrint("\n!!! [WATCHDOG ALERT] Service '" + serviceName + "' missed its petting - may be unresponsive !!!\n");
        return ChirpError::SUCCESS;
    }
};

int main() {
    threadSafePrint("\n=== ChirpWatchDog Example ===");
    threadSafePrint("Demonstrates watchdog monitoring of service responsiveness\n");
    
    // Get the factory singleton
    IChirpFactory* factory = &ChirpFactory::getInstance();
    
    // Create two services
    threadSafePrint("Creating Service Parrot...");
    IChirp* service1 = nullptr;
    ChirpError::Error error = factory->createService("Parrot", &service1);
    if (error != ChirpError::SUCCESS || !service1) {
        threadSafePrint("ERROR: Failed to create Parrot");
        return 1;
    }
    
    threadSafePrint("Creating Service Sparrow...");
    IChirp* service2 = nullptr;
    error = factory->createService("Sparrow", &service2);
    if (error != ChirpError::SUCCESS || !service2) {
        threadSafePrint("ERROR: Failed to create Sparrow");
        return 1;
    }
    
    // Create handlers for services
    ServiceHandler handler1("Parrot");
    ServiceHandler handler2("Sparrow");
    handler1.bindService(service1);
    handler2.bindService(service2);
    
    // Register message handlers
    threadSafePrint("Registering handlers for services...");
    error = service1->registerMsgHandler("ProcessData", &handler1, &ServiceHandler::handleMessage);
    if (error != ChirpError::SUCCESS) {
        threadSafePrint("ERROR: Failed to register Parrot handler");
        return 1;
    }
    
    error = service2->registerMsgHandler("ProcessData", &handler2, &ServiceHandler::handleMessage);
    if (error != ChirpError::SUCCESS) {
        threadSafePrint("ERROR: Failed to register Sparrow handler");
        return 1;
    }
    
    // Enable watchdog monitoring on both services
    threadSafePrint("Enabling watchdog monitoring on Parrot...");
    service1->setWatchDogMonitoring(true);
    
    threadSafePrint("Enabling watchdog monitoring on Sparrow...");
    service2->setWatchDogMonitoring(true);
    
    // Start the services
    threadSafePrint("Starting services...");
    error = service1->start();
    if (error != ChirpError::SUCCESS) {
        threadSafePrint("ERROR: Failed to start Service Parrot");
        return 1;
    }
    
    error = service2->start();
    if (error != ChirpError::SUCCESS) {
        threadSafePrint("ERROR: Failed to start Service Sparrow");
        return 1;
    }
    
    // Create and configure the watchdog service
    threadSafePrint("Creating ChirpWatchDog service...");
    IChirpWatchDog* watchdog = IChirpWatchDog::createWatchdog("Watchdog");
    if (!watchdog) {
        threadSafePrint("ERROR: Failed to create watchdog");
        return 1;
    }
    
    // Configure watchdog with 1 second pet duration
    threadSafePrint("Configuring watchdog with 1 second pet duration...");
    error = watchdog->configure(factory, std::chrono::milliseconds(1000));
    if (error != ChirpError::SUCCESS) {
        threadSafePrint("ERROR: Failed to configure watchdog");
        delete watchdog;
        return 1;
    }
    
    // Register handler for missed pet messages
    WatchdogHandler watchdogHandler;
    error = watchdog->getChirpService()->registerMsgHandler(IChirpWatchDog::MissedPetMessage, 
                                                            &watchdogHandler, 
                                                            &WatchdogHandler::onMissedPet);
    if (error != ChirpError::SUCCESS) {
        threadSafePrint("ERROR: Failed to register watchdog handler");
        delete watchdog;
        return 1;
    }
    
    // Start the watchdog
    threadSafePrint("Starting watchdog service...\n");
    error = watchdog->start();
    if (error != ChirpError::SUCCESS) {
        threadSafePrint("ERROR: Failed to start watchdog");
        delete watchdog;
        return 1;
    }
    
    // Phase 1: Chain posts from handlers for indices 1..12
    handler1.setPostingRange(1, 12);
    handler2.setPostingRange(1, 12);
    service1->postMsg("ProcessData", std::string("Data packet #1"));
    service2->postMsg("ProcessData", std::string("Data packet #1"));
    while (handler1.getMessageCount() < 12 || handler2.getMessageCount() < 12) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    threadSafePrint("\n=== PHASE 2: Sparrow becomes slow (3 seconds per message) ===");
    handler2.setResponseTime(3000);
    
    // Phase 2: Continue chained posts 13..20, make Hulk slow
    handler1.setPostingRange(13, 15);
    handler2.setPostingRange(13, 15);
    service1->postMsg("ProcessData", std::string("Data packet #13"));
    service2->postMsg("ProcessData", std::string("Data packet #13"));
    while (handler1.getMessageCount() < 15 || handler2.getMessageCount() < 15) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Cleanup
    threadSafePrint("\n=== Shutting down ===");
    watchdog->stop();
    delete watchdog;
    
    //std::cout << "Stopping services..." << std::endl;
    service1->shutdown();
    service2->shutdown();
    
    //std::cout << "Destroying services..." << std::endl;
    factory->destroyService("Tarzan");
    factory->destroyService("Hulk");
    
    return 0;
}
