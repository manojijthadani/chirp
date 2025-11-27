/**
 * @file timer_mgr.h
 * @brief Timer manager for managing multiple timers in the Chirp framework
 * @author Chirp Team
 * @date 2025
 * @version 2.0
 * 
 * This file defines the TimerManager class which provides centralized
 * management of multiple timers within the Chirp framework.
 */

#pragma once
#include <vector>
#include <chrono>
#include "chirp_timer.h"

/**
 * @brief Timer manager class for managing multiple timers
 */
class TimerManager {
public:
    /**
     * @brief Constructor
     */
    TimerManager();

    /**
     * @brief Destructor
     */
    ~TimerManager();

    /**
     * @brief Add a timer to the internal vector
     * @param chirpTimer The timer to add
     * 
     * Adds the provided ChirpTimer to the internal vector for management.
     */
    void addTimer(ChirpTimer* chirpTimer);

    /**
     * @brief Remove a timer from the internal vector
     * @param chirpTimer The timer to remove
     * 
     * Removes the provided ChirpTimer from the internal vector.
     * Also removes it from the firing times list if present.
     */
    void removeTimer(ChirpTimer* chirpTimer);

    /**
     * @brief Compute next timer firing time for all timers
     * 
     * Creates a vector of all timer instances and calculates their next firing time.
     * For first-time additions, reads start time and adds duration. Sorts the vector
     * by firing time with lowest times at the top. Result is stored in _timerFiringTimes.
     * Skips any timers that are in the elapsedTimers list.
     * 
     * @param elapsedTimers Vector of timers that have just elapsed and should be skipped
     */
    void computeNextTimerFirringTime();

    /**
     * @brief Get duration to wait before next timer event
     * @return Duration in milliseconds to wait before the next timer fires
     * 
     * Calculates the time remaining until the next timer event by finding the
     * lowest firing time in _timerFiringTimes and subtracting current time.
     * Returns 0 if no timers are scheduled or if the next event is in the past.
     */
    std::chrono::milliseconds getDurationToNextTimerEvent() const;

    /**
     * @brief Generate a list of timers that have elapsed
     * @param elapsedTimers Output parameter - vector to be populated with elapsed timers
     * 
     * Compares the current time with _timerFiringTimes to identify timers that have elapsed.
     * Populates the provided vector with pointers to timers that should fire now.
     */
    void getElapsedTimers(std::vector<ChirpTimer*>& elapsedTimers) const;

    /**
     * @brief Reschedule specific timers that have fired
     * @param firedTimers Vector of timers that have just fired and need rescheduling
     * 
     * For each timer in the list, removes its old firing time and calculates a new one
     * based on the previous firing time plus the timer's duration.
     */
    void rescheduleTimers(const std::vector<ChirpTimer*>& firedTimers);

private:
    std::vector<ChirpTimer*> _timers;  /**< Vector of timer pointers */
    std::vector<std::pair<ChirpTimer*, std::chrono::steady_clock::time_point>> _timerFiringTimes;  /**< Vector of timer firing times */
    std::chrono::steady_clock::time_point _nextFiringTime;  /**< Time point of the next timer to fire */
    size_t _nextFirringTimerIndex;  /**< Index of the timer with the lowest firing time */
};
