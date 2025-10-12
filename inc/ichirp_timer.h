/**
 * @file ichirp_timer.h
 * @brief Abstract timer interface for the Chirp framework
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 * 
 * This file defines the IChirpTimer interface for creating and managing timer instances.
 * This interface ensures that different timer implementations can be used
 * while maintaining the same API contract.
 */

#pragma once
#include <chrono>
#include <functional>
#include <string>
#include "chirp_error.h"

// Forward declaration to prevent inclusion of private headers
class Chirp;

/**
 * @brief Timer mode enumeration
 */
enum class TimerMode {
    ONE_TIME,      /**< Timer fires once and stops */
    CONTINUOUS     /**< Timer repeats indefinitely */
};

/**
 * @brief Abstract timer interface for the Chirp framework
 * 
 * IChirpTimer defines the interface for creating and managing timer instances.
 * This interface ensures that different timer implementations can be used
 * while maintaining the same API contract.
 * 
 * @note Implementations should be thread-safe
 * 
 * @example
 * @code
 * IChirpTimer* timer = new ChirpTimer("MyTimer");
 * timer->configure(TimerMode::CONTINUOUS, std::chrono::milliseconds(1000), 
 *                  &chirpService, "TimerMessage");
 * timer->start();
 * @endcode
 */
class IChirpTimer {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~IChirpTimer() = default;

    /**
     * @brief Constructor with optional timer name
     * @param name Optional name for the timer (defaults to empty string)
     * 
     * Creates a timer instance with an optional name that can be used
     * for identification and logging purposes.
     */
    IChirpTimer(const std::string& name = "") {
        // Base class constructor - intentionally empty
        (void)name; // Suppress unused parameter warning
    }

    /**
     * @brief Configure the timer with mode, duration, and Chirp service settings
     * @param mode The timer mode (one-time or continuous)
     * @param duration The duration for the timer interval
     * @param chirpObj Pointer to the Chirp service instance
     * @param messageName The name of the message to send
     * @param args Vector of arguments to pass with the message
     * @return ChirpError::Error indicating success or failure
     * 
     * @note This method must be called before starting the timer
     * @note This method can be called multiple times to reconfigure the timer
     * @note Timer must be stopped before reconfiguration
     * @note The message will be posted to the Chirp service when the timer expires
     * @note Arguments are passed as a vector of std::any for type flexibility
     */
    virtual ChirpError::Error configure(TimerMode mode, 
                                      const std::chrono::milliseconds& duration,
                                      Chirp* chirpObj, 
                                      const std::string& messageName) = 0; 


    /**
     * @brief Start the timer
     * @return ChirpError::Error indicating success or failure
     * 
     * Starts the timer thread and begins the countdown. The timer will fire
     * after the specified duration and execute the callback function.
     * 
     * @note Timer must be properly configured (mode, duration, callback) before starting
     * @note This method is thread-safe
     */
    virtual ChirpError::Error start() = 0;

    /**
     * @brief Stop the timer
     * @return ChirpError::Error indicating success or failure
     * 
     * Stops the timer and waits for the timer thread to finish gracefully.
     * 
     * @note This method is thread-safe
     * @note If the timer is not running, this method returns SUCCESS
     */
    virtual ChirpError::Error stop() = 0;

    /**
     * @brief Check if the timer is currently running
     * @return true if the timer is running, false otherwise
     */
    virtual bool isRunning() const = 0;

    /**
     * @brief Get the current timer mode
     * @return The current timer mode
     */
    virtual TimerMode getMode() const = 0;

    /**
     * @brief Get the current timer duration
     * @return The current timer duration in milliseconds
     */
    virtual std::chrono::milliseconds getDuration() const = 0;

    /**
     * @brief Get the timer name
     * @return The name of the timer
     */
    virtual std::string getName() const = 0;
};
