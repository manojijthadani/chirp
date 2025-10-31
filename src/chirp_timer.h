/**
 * @file chirp_timer.h
 * @brief Concrete timer implementation for the Chirp framework
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 * 
 * This file defines the ChirpTimer concrete implementation of the IChirpTimer interface,
 * which provides thread-based timer functionality with configurable modes and durations.
 */

#pragma once
#include "ichirp_timer.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <vector>
#include <any>

/**
 * @brief Concrete implementation of the IChirpTimer interface
 * 
 * ChirpTimer provides a thread-based implementation of the IChirpTimer interface.
 * It supports both one-time and continuous timer modes with configurable durations.
 * The timer runs in its own thread and executes user-defined callback functions.
 * 
 * This class is thread-safe and can be used from multiple threads concurrently.
 */
class ChirpTimer : public IChirpTimer {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an unconfigured timer. Use configure() to set message and duration.
     */
    ChirpTimer();

    /**
     * @brief Constructor with message and duration
     * @param messageToDeliver Message to deliver when timer fires
     * @param duration Timer duration in milliseconds
     * 
     * This constructor creates a new ChirpTimer instance with the specified
     * message and duration. The timer can be started immediately after construction.
     */
    ChirpTimer(std::string messageToDeliver, const std::chrono::milliseconds& duration);

    
    /**
     * @brief Destructor
     * 
     * Ensures the timer is properly stopped and cleaned up.
     */
    ~ChirpTimer();

    // Implementation of IChirpTimer interface
    ChirpError::Error configure(std::string messageToDeliver,
                               const std::chrono::milliseconds& duration) override;
    ChirpError::Error start() override;
    ChirpError::Error stop() override;
    bool isRunning() const override;
    std::chrono::milliseconds getDuration() const override;
    std::chrono::steady_clock::time_point getTimerStartTime() const;
    std::string getMessage() const;

private:
    /**
     * @brief Timer state enumeration
     */
    enum class TimerState {
        STOPPED,    /**< Timer is not running */
        STARTING,   /**< Timer is starting up */
        RUNNING,    /**< Timer is actively running */
        STOPPING    /**< Timer is shutting down */
    };



    /**
     * @brief Validate timer configuration before starting
     * @return ChirpError::Error indicating success or failure
     */
    ChirpError::Error validateConfiguration() const;
    
    std::atomic<TimerState> _state;              /**< Current timer state */
    mutable std::mutex _configMutex;             /**< Mutex for configuration changes */
    std::atomic<bool> _shouldStop;               /**< Flag to signal timer thread to stop */
    std::string _messageToDeliver;               /**< Message to deliver when timer fires */
    std::chrono::milliseconds _duration;         /**< Timer duration */ 
    std::chrono::steady_clock::time_point _startTime;  /**< Timer start time */
};
