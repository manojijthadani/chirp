/**
 * @file timer_example.cpp
 * @brief Simple example demonstrating how to use IChirpTimer with Chirp services
 * @author Chirp Team
 * @date 2025
 * @version 2.0
 * 
 * This example shows how to:
 * 1. Create an IChirpTimer using the factory method
 * 2. Register a message handler in a Chirp service
 * 3. Add the timer to the service
 * 4. Start the service and timer
 * 5. Stop the timer
 * 6. Verify timer accuracy
 */

#include "ichirp.h"
#include "ichirp_timer.h"
#include "chirp_error.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <climits>
#include <random>

class TimerHandler {
public:
    TimerHandler() : slowTickCount(0), fastTickCount(0), mediumTickCount(0),
                     dist(0, 300) {
        std::random_device rd;
        gen.seed(rd());
    }
    
    ChirpError::Error onSlowTimerTick(const std::string& timerMessage) {
        // Random delay 0-300ms
        int delay = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        
        auto now = std::chrono::steady_clock::now();
        slowTickCount++;
        {
            std::lock_guard<std::mutex> lock(mtx);
            slowTimestamps.push_back(now);
        }
        std::cout << "[SLOW TIMER] Tick #" << slowTickCount << " (1000ms interval, " << delay << "ms delay) - Message: " << timerMessage << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
    ChirpError::Error onMediumTimerTick(const std::string& timerMessage) {
        // Random delay 0-300ms
        int delay = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        
        auto now = std::chrono::steady_clock::now();
        mediumTickCount++;
        {
            std::lock_guard<std::mutex> lock(mtx);
            mediumTimestamps.push_back(now);
        }
        std::cout << "[MEDIUM TIMER] Tick #" << mediumTickCount << " (600ms interval, " << delay << "ms delay) - Message: " << timerMessage << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
    ChirpError::Error onFastTimerTick(const std::string& timerMessage) {
        // Random delay 0-300ms
        int delay = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        
        auto now = std::chrono::steady_clock::now();
        fastTickCount++;
        {
            std::lock_guard<std::mutex> lock(mtx);
            fastTimestamps.push_back(now);
        }
        std::cout << "[FAST TIMER] Tick #" << fastTickCount << " (100ms interval, " << delay << "ms delay) - Message: " << timerMessage << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
    // Regular (non-timer) message handler
    ChirpError::Error onRegularEvent(const std::string& payload) {
        std::cout << "[REGULAR] Received: " << payload << std::endl;
        std::cout.flush();
        return ChirpError::SUCCESS;
    }
    
    int getSlowTickCount() const { return slowTickCount; }
    int getMediumTickCount() const { return mediumTickCount; }
    int getFastTickCount() const { return fastTickCount; }
    
    void printAccuracyReport() {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "\n=== Timer Accuracy Report ===" << std::endl;
        std::cout << "Slow Timer (1000ms interval): " << slowTickCount << " ticks" << std::endl;
        if (slowTimestamps.size() > 1) {
            long long totalInterval = 0;
            long long minInterval = LLONG_MAX;
            long long maxInterval = 0;
            
            std::cout << "  Intervals:" << std::endl;
            for (size_t i = 1; i < slowTimestamps.size(); ++i) {
                auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(
                    slowTimestamps[i] - slowTimestamps[i-1]).count();
                std::cout << "    Tick " << i << " to " << (i+1) << ": " << interval << "ms" << std::endl;
                totalInterval += interval;
                minInterval = std::min(minInterval, static_cast<long long>(interval));
                maxInterval = std::max(maxInterval, static_cast<long long>(interval));
            }
            
            long long avgInterval = totalInterval / (slowTimestamps.size() - 1);
            std::cout << "  Average interval: " << avgInterval << "ms" << std::endl;
            std::cout << "  Min interval: " << minInterval << "ms" << std::endl;
            std::cout << "  Max interval: " << maxInterval << "ms" << std::endl;
            std::cout << "  Accuracy: " << (100 - (std::abs(avgInterval - 1000) * 100 / 1000)) << "%" << std::endl;
        }
        
        std::cout << "\nMedium Timer (600ms interval): " << mediumTickCount << " ticks" << std::endl;
        if (mediumTimestamps.size() > 1) {
            long long totalInterval = 0;
            long long minInterval = LLONG_MAX;
            long long maxInterval = 0;
            
            std::cout << "  Intervals (first 5):" << std::endl;
            for (size_t i = 1; i < mediumTimestamps.size() && i <= 5; ++i) {
                auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(
                    mediumTimestamps[i] - mediumTimestamps[i-1]).count();
                std::cout << "    Tick " << i << " to " << (i+1) << ": " << interval << "ms" << std::endl;
                totalInterval += interval;
                minInterval = std::min(minInterval, static_cast<long long>(interval));
                maxInterval = std::max(maxInterval, static_cast<long long>(interval));
            }
            
            if (mediumTimestamps.size() > 1) {
                long long avgInterval = totalInterval / std::min((size_t)5, mediumTimestamps.size() - 1);
                std::cout << "  Average interval: " << avgInterval << "ms" << std::endl;
                std::cout << "  Min interval: " << minInterval << "ms" << std::endl;
                std::cout << "  Max interval: " << maxInterval << "ms" << std::endl;
                std::cout << "  Accuracy: " << (100 - (std::abs(avgInterval - 600) * 100 / 600)) << "%" << std::endl;
            }
        }
        
        std::cout << "\nFast Timer (100ms interval): " << fastTickCount << " ticks" << std::endl;
        if (fastTimestamps.size() > 1) {
            long long totalInterval = 0;
            long long minInterval = LLONG_MAX;
            long long maxInterval = 0;
            
            std::cout << "  Intervals (first 10):" << std::endl;
            for (size_t i = 1; i < fastTimestamps.size() && i <= 10; ++i) {
                auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(
                    fastTimestamps[i] - fastTimestamps[i-1]).count();
                std::cout << "    Tick " << i << " to " << (i+1) << ": " << interval << "ms" << std::endl;
                totalInterval += interval;
                minInterval = std::min(minInterval, static_cast<long long>(interval));
                maxInterval = std::max(maxInterval, static_cast<long long>(interval));
            }
            
            if (fastTimestamps.size() > 1) {
                long long avgInterval = totalInterval / std::min((size_t)10, fastTimestamps.size() - 1);
                std::cout << "  Average interval: " << avgInterval << "ms" << std::endl;
                std::cout << "  Min interval: " << minInterval << "ms" << std::endl;
                std::cout << "  Max interval: " << maxInterval << "ms" << std::endl;
                std::cout << "  Accuracy: " << (100 - (std::abs(avgInterval - 100) * 100 / 100)) << "%" << std::endl;
            }
        }
        std::cout << "==============================\n" << std::endl;
    }
    
private:
    int slowTickCount;
    int fastTickCount;
    int mediumTickCount;
    std::vector<std::chrono::steady_clock::time_point> slowTimestamps;
    std::vector<std::chrono::steady_clock::time_point> fastTimestamps;
    std::vector<std::chrono::steady_clock::time_point> mediumTimestamps;
    std::mutex mtx;
    std::mt19937 gen;
    std::uniform_int_distribution<int> dist;
};

int main() {
    std::cout << "=== Multi-Timer IChirpTimer Example ===" << std::endl;
    std::cout << "Demonstrates multiple timers with different intervals and accuracy checking\n" << std::endl;
    
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

    std::cout << "Registering message handler 'MediumTimerTick'..." << std::endl;
    error = chirpService.registerMsgHandler("MediumTimerTick", &handler, &TimerHandler::onMediumTimerTick);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register medium timer handler" << std::endl;
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
    IChirpTimer* mediumTimer = IChirpTimer::createTimer();
    
    // Configure slow timer
    error = slowTimer->configure("SlowTimerTick", std::chrono::milliseconds(1000));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to configure slow timer: " << ChirpError::errorToString(error) << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }
    
    // Configure fast timer with 100ms interval using IChirpTimer
    std::cout << "Creating fast timer with 100ms interval..." << std::endl;
    error = fastTimer->configure("FastTimerTick", std::chrono::milliseconds(100));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to configure fast timer: " << ChirpError::errorToString(error) << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }

    // Configure medium timer with 600ms interval using IChirpTimer
    std::cout << "Creating medium timer with 600ms interval..." << std::endl;
    error = mediumTimer->configure("MediumTimerTick", std::chrono::milliseconds(600));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to configure medium timer: " << ChirpError::errorToString(error) << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }
    
    // IMPORTANT: Start the service FIRST
    std::cout << "Starting Chirp service..." << std::endl;
    error = chirpService.start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start service" << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }
    
    // Start the slow timer BEFORE adding it to the service
    std::cout << "Starting slow timer..." << std::endl;
    error = slowTimer->start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start slow timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }
    
    // Start the fast timer BEFORE adding it to the service
    std::cout << "Starting fast timer..." << std::endl;
    error = fastTimer->start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start fast timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }

    // Start the medium timer BEFORE adding it to the service
    std::cout << "Starting medium timer..." << std::endl;
    error = mediumTimer->start();
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to start medium timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }
    
    // Add the slow timer to the service
    std::cout << "Adding slow timer to service..." << std::endl;
    error = chirpService.addChirpTimer(slowTimer);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to add slow timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }
    
    // Add the fast timer to the service
    std::cout << "Adding fast timer to service...\n" << std::endl;
    error = chirpService.addChirpTimer(fastTimer);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to add fast timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }

    // Add the medium timer to the service
    std::cout << "Adding medium timer to service...\n" << std::endl;
    error = chirpService.addChirpTimer(mediumTimer);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to add medium timer" << std::endl;
        delete slowTimer;
        delete fastTimer;
        delete mediumTimer;
        return 1;
    }
    
    // While timers run for 5 seconds, interleave regular events every 250ms
    std::cout << "Timers and regular events running for 5 seconds..." << std::endl;
    for (int i = 1; i <= 20; ++i) { // 20 iterations * 250ms = 5 seconds
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

    // Stop the medium timer
    std::cout << "Stopping medium timer..." << std::endl;
    mediumTimer->stop();
    chirpService.removeChirpTimer(mediumTimer);
    
    // Shutdown the service
    std::cout << "Shutting down service..." << std::endl;
    chirpService.shutdown();
    
    // Print accuracy report
    handler.printAccuracyReport();
    
    // Clean up timer memory
    delete slowTimer;
    delete fastTimer;
    delete mediumTimer;
    
    std::cout << "Example completed successfully!" << std::endl;
    
    return 0;
}
