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
     * @brief Default constructor
     */
    IChirpTimer() = default;

    /**
     * @brief Configure the timer with message and duration
     * @param messageToDeliver The message to deliver when timer fires
     * @param duration The duration for the timer interval
     * @return ChirpError::Error indicating success or failure
     * 
     * @note This method can be called multiple times to reconfigure the timer
     * @note Timer must be stopped before reconfiguration
     */
    virtual ChirpError::Error configure(std::string messageToDeliver, 
                                      const std::chrono::milliseconds& duration) = 0; 


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

    /**
     * @brief Get the current timer duration
     * @return The current timer duration in milliseconds
     */
    virtual std::chrono::milliseconds getDuration() const = 0;

    /**
     * @brief Get the timer name
     * @return The name of the timer
     */
};
