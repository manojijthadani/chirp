/**
 * @file timer_example.cpp
 * @brief Example demonstrating how to use the ChirpTimer class
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 */

#include "chirp_timer.h"
#include "chirp.h"
#include "chirp_error.h"
#include <iostream>
#include <chrono>
#include <thread>

class TimerHandlers {
public:
    void OneTimeTimerHandler() {
        std::cout << "[ONE-TIME TIMER] Timer fired! One-time timer completed." << std::endl;
    }
    
    void ContinuousTimerHandler() {
        std::cout << "[CONTINUOUS TIMER] Timer fired! Continuous timer tick." << std::endl;
    }
};

int main() {
    std::cout << "=== ChirpTimer Example ===" << std::endl;
    
    // Create a Chirp service for the timer to send messages to
    ChirpError::Error error = ChirpError::SUCCESS;
    Chirp chirpService("TimerService", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create Chirp service: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Create handler instance
    TimerHandlers handlers;
    
    // Register message handlers for timer messages
    error = chirpService.registerMsgHandler("TimerMessage", &handlers, &TimerHandlers::OneTimeTimerHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register OneTimeTimerHandler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = chirpService.registerMsgHandler("ContinuousTimerMessage", &handlers, &TimerHandlers::ContinuousTimerHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register ContinuousTimerHandler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    // Start the chirp service
    chirpService.start();
    std::cout << "ChirpService started with registered handlers" << std::endl;
    
    // Example 1: One-time timer with Chirp service
    std::cout << "\n1. One-time timer example:" << std::endl;

    // Create a timer instance with a name
    ChirpTimer timer("OneTimeTimer");

    // Configure timer with mode, duration, and chirp service settings
    timer.configure(TimerMode::ONE_TIME, 
                    std::chrono::milliseconds(2000), 
                    &chirpService, 
                    "TimerMessage");
    
    std::cout << "Starting one-time timer (2 seconds)..." << std::endl;
    timer.start();
    
    // Wait for the timer to complete
    std::cout << "Waiting for one-time timer to fire..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    
    // Example 2: Continuous timer
    std::cout << "\n2. Continuous timer example:" << std::endl;
    
    // Create a new timer for continuous mode with a name
    ChirpTimer continuousTimer("ContinuousTimer");
    
    // Configure continuous timer with mode, duration, and chirp service settings
    continuousTimer.configure(TimerMode::CONTINUOUS, 
                              std::chrono::milliseconds(1000),
                              &chirpService, 
                              "ContinuousTimerMessage");
    
    std::cout << "Starting continuous timer (1 second intervals)..." << std::endl;
    continuousTimer.start();
    
    // Let it run for 5 seconds
    std::cout << "Letting continuous timer run for 5 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    
    // Stop the timer
    std::cout << "Stopping continuous timer..." << std::endl;
    continuousTimer.stop();
    
    // Stop the chirp service
    chirpService.shutdown();
    
    std::cout << "\nTimer example completed!" << std::endl;
    
    return 0;
}
