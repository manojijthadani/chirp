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
     * @brief Constructor with optional timer name
     * @param name Optional name for the timer (defaults to empty string)
     * 
     * This constructor creates a new ChirpTimer instance. The timer must be
     * configured with mode and duration before it can be started.
     */
    ChirpTimer(const std::string& name = "");

    
    /**
     * @brief Destructor
     * 
     * Ensures the timer is properly stopped and cleaned up.
     */
    ~ChirpTimer();

    // Implementation of IChirpTimer interface
    ChirpError::Error configure(TimerMode mode, 
                               const std::chrono::milliseconds& duration,
                               Chirp* chirpObj, 
                               const std::string& messageName) override;
    ChirpError::Error start() override;
    ChirpError::Error stop() override;
    bool isRunning() const override;
    TimerMode getMode() const override;
    std::chrono::milliseconds getDuration() const override;
    std::string getName() const override;

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
     * @brief Send the configured message to the chirp service
     * @return ChirpError::Error indicating success or failure
     */
    ChirpError::Error sendMessage();

    /**
     * @brief Main timer thread function
     * 
     * This function runs in the timer thread and handles the actual
     * timer logic, including waiting for the duration and executing callbacks.
     */
    void timerThreadFunction();

    /**
     * @brief Validate timer configuration before starting
     * @return ChirpError::Error indicating success or failure
     */
    ChirpError::Error validateConfiguration() const;
    
    std::atomic<TimerState> _state;              /**< Current timer state */
    mutable std::mutex _configMutex;             /**< Mutex for configuration changes */
    std::mutex _stopMutex;                       /**< Mutex for stop condition variable */
    std::condition_variable _stopCondition;      /**< Condition variable for stop signaling */
    std::atomic<bool> _shouldStop;               /**< Flag to signal timer thread to stop */
    std::string _name;                           /**< Timer name for identification */
    TimerMode _mode;                             /**< Current timer mode */
    std::chrono::milliseconds _duration;         /**< Timer duration */ 
    Chirp* _chirpObj;                            /**< Chirp service object for message sending */
    std::string _messageName;                    /**< Message name to send */
    std::thread _timerThread;                    /**< Timer thread handle */
};
