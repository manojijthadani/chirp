/**
 * @file timer_example.cpp
 * @brief Simple example demonstrating how to use ChirpTimer with Chirp services
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 * 
 * This example shows how to:
 * 1. Create a ChirpTimer with a message and duration
 * 2. Register a message handler in a Chirp service
 * 3. Add the timer to the service
 * 4. Start the service and timer
 * 5. Stop the timer
 */

#include "chirp_timer.h"
#include "ichirp.h"
#include "chirp_error.h"
#include <iostream>
#include <chrono>
#include <thread>

class TimerHandler {
public:
    TimerHandler() : slowTickCount(0), fastTickCount(0) {}
    
    ChirpError::Error onSlowTimerTick() {
        slowTickCount++;
        std::cout << "[SLOW TIMER] Tick #" << slowTickCount << " (1 second interval)" << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
    ChirpError::Error onFastTimerTick() {
        fastTickCount++;
        std::cout << "[FAST TIMER] Tick #" << fastTickCount << " (100ms interval)" << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
private:
    int slowTickCount;
    int fastTickCount;
};

int main() {
    std::cout << "=== Multi-Timer ChirpTimer Example ===" << std::endl;
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
    
    // Register message handlers for timer events
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
    
    // Create a slow timer with 1 second interval
    std::cout << "Creating slow timer with 1 second interval..." << std::endl;
    ChirpTimer slowTimer("SlowTimerTick", std::chrono::milliseconds(1000));
    
    // Create a fast timer with 100ms interval
    std::cout << "Creating fast timer with 100ms interval..." << std::endl;
    ChirpTimer fastTimer("FastTimerTick", std::chrono::milliseconds(100));
    
    // IMPORTANT: Start the service FIRST
    std::cout << "Starting Chirp service..." << std::endl;
    error = chirpService.start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start service" << std::endl;
        return 1;
    }
    
    // Start the slow timer BEFORE adding it to the service
    std::cout << "Starting slow timer..." << std::endl;
    error = slowTimer.start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start slow timer" << std::endl;
        return 1;
    }
    
    // Start the fast timer BEFORE adding it to the service
    std::cout << "Starting fast timer..." << std::endl;
    error = fastTimer.start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start fast timer" << std::endl;
        return 1;
    }
    
    // Add the slow timer to the service
    std::cout << "Adding slow timer to service..." << std::endl;
    error = chirpService.addChirpTimer(&slowTimer);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to add slow timer" << std::endl;
        return 1;
    }
    
    // Add the fast timer to the service
    std::cout << "Adding fast timer to service...\n" << std::endl;
    error = chirpService.addChirpTimer(&fastTimer);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to add fast timer" << std::endl;
        return 1;
    }
    
    // Let both timers run for 3 seconds (fast timer will fire ~30 times, slow timer 3 times)
    std::cout << "Timers running for 3 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    
    // Stop the fast timer first
    std::cout << "\nStopping fast timer..." << std::endl;
    fastTimer.stop();
    chirpService.removeChirpTimer(&fastTimer);
    
    // Stop the slow timer
    std::cout << "Stopping slow timer..." << std::endl;
    slowTimer.stop();
    chirpService.removeChirpTimer(&slowTimer);
    
    // Shutdown the service
    std::cout << "Shutting down service..." << std::endl;
    chirpService.shutdown();
    
    std::cout << "\nExample completed successfully!" << std::endl;
    
    return 0;
}
