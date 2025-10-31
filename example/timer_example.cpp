/**
 * @file timer_example.cpp
 * @brief Simple example demonstrating how to use IChirpTimer with Chirp services
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 * 
 * This example shows how to:
 * 1. Create an IChirpTimer using the factory method
 * 2. Register a message handler in a Chirp service
 * 3. Add the timer to the service
 * 4. Start the service and timer
 * 5. Stop the timer
 */

#include "ichirp.h"
#include "ichirp_timer.h"
#include "chirp_error.h"
#include <iostream>
#include <chrono>
#include <thread>

class TimerHandler {
public:
    TimerHandler() : slowTickCount(0), fastTickCount(0) {}
    
    ChirpError::Error onSlowTimerTick(const std::string& timerMessage) {
        slowTickCount++;
        std::cout << "[SLOW TIMER] Tick #" << slowTickCount << " (1 second interval) - Message: " << timerMessage << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
    ChirpError::Error onFastTimerTick(const std::string& timerMessage) {
        fastTickCount++;
        std::cout << "[FAST TIMER] Tick #" << fastTickCount << " (100ms interval) - Message: " << timerMessage << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
    // Regular (non-timer) message handler
    ChirpError::Error onRegularEvent(const std::string& payload) {
        std::cout << "[REGULAR] Received: " << payload << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
private:
    int slowTickCount;
    int fastTickCount;
};

int main() {
    std::cout << "=== Multi-Timer IChirpTimer Example ===" << std::endl;
    std::cout << "Demonstrates multiple timers with different intervals\n" << std::endl;
    
    // Create a Chirp service
    ChirpError::Error error = ChirpError::SUCCESS;
    IChirp chirpService("TimerService", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create Chirp service" << std::endl;
        return 1;
    }
    
    // Create handler instance
    TimerHandler handler;
    
    // Register message handlers for timer and regular events
    std::cout << "Registering message handler 'SlowTimerTick'..." << std::endl;
    error = chirpService.registerMsgHandler("SlowTimerTick", &handler, &TimerHandler::onSlowTimerTick);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register slow timer handler" << std::endl;
        return 1;
    }
    
    std::cout << "Registering message handler 'FastTimerTick'..." << std::endl;
    error = chirpService.registerMsgHandler("FastTimerTick", &handler, &TimerHandler::onFastTimerTick);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register fast timer handler" << std::endl;
        return 1;
    }
    
    std::cout << "Registering message handler 'RegularEvent'..." << std::endl;
    error = chirpService.registerMsgHandler("RegularEvent", &handler, &TimerHandler::onRegularEvent);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register regular event handler" << std::endl;
        return 1;
    }
    
    // Create timers
    std::cout << "Creating slow timer with 1 second interval..." << std::endl;
    IChirpTimer* slowTimer = IChirpTimer::createTimer();
    IChirpTimer* fastTimer = IChirpTimer::createTimer();
    
    // Configure slow timer
    error = slowTimer->configure("SlowTimerTick", std::chrono::milliseconds(1000));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to configure slow timer: " << ChirpError::errorToString(error) << std::endl;
        delete slowTimer;
        delete fastTimer;
        return 1;
    }
    
    // Configure fast timer with 100ms interval using IChirpTimer
    std::cout << "Creating fast timer with 100ms interval..." << std::endl;
    error = fastTimer->configure("FastTimerTick", std::chrono::milliseconds(100));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to configure fast timer: " << ChirpError::errorToString(error) << std::endl;
        delete slowTimer;
        delete fastTimer;
        return 1;
    }
    
    // IMPORTANT: Start the service FIRST
    std::cout << "Starting Chirp service..." << std::endl;
    error = chirpService.start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start service" << std::endl;
        delete slowTimer;
        delete fastTimer;
        return 1;
    }
    
    // Start the slow timer BEFORE adding it to the service
    std::cout << "Starting slow timer..." << std::endl;
    error = slowTimer->start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start slow timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        return 1;
    }
    
    // Start the fast timer BEFORE adding it to the service
    std::cout << "Starting fast timer..." << std::endl;
    error = fastTimer->start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start fast timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        return 1;
    }
    
    // Add the slow timer to the service
    std::cout << "Adding slow timer to service..." << std::endl;
    error = chirpService.addChirpTimer(slowTimer);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to add slow timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        return 1;
    }
    
    // Add the fast timer to the service
    std::cout << "Adding fast timer to service...\n" << std::endl;
    error = chirpService.addChirpTimer(fastTimer);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to add fast timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        return 1;
    }
    
    // While timers run for 3 seconds, interleave regular events every 250ms
    std::cout << "Timers and regular events running for 3 seconds..." << std::endl;
    for (int i = 1; i <= 12; ++i) { // 12 iterations * 250ms = 3 seconds
        ChirpError::Error pe = chirpService.postMsg("RegularEvent", std::string("Hello #") + std::to_string(i));
        if (pe != ChirpError::SUCCESS) {
            std::cout << "Failed to post RegularEvent: " << static_cast<int>(pe) << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    
    // Stop the fast timer first
    std::cout << "\nStopping fast timer..." << std::endl;
    fastTimer->stop();
    chirpService.removeChirpTimer(fastTimer);
    
    // Stop the slow timer
    std::cout << "Stopping slow timer..." << std::endl;
    slowTimer->stop();
    chirpService.removeChirpTimer(slowTimer);
    
    // Shutdown the service
    std::cout << "Shutting down service..." << std::endl;
    chirpService.shutdown();
    
    // Clean up timer memory
    delete slowTimer;
    delete fastTimer;
    
    std::cout << "\nExample completed successfully!" << std::endl;
    
    return 0;
}
