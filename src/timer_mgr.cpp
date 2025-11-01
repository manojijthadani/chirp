/**
 * @file timer_mgr.cpp
 * @brief Implementation of TimerManager class
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 */

#include "timer_mgr.h"
#include <algorithm>
#include <iostream>

TimerManager::TimerManager() {
}

TimerManager::~TimerManager() {
}

void TimerManager::addTimer(ChirpTimer* chirpTimer) {
    
    if (chirpTimer) {
        _timers.push_back(chirpTimer);
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::milliseconds duration = chirpTimer->getDuration();
        auto nextFiringTime = currentTime + duration;
                    
        // Add the new timer to the vector
        _timerFiringTimes.push_back(std::make_pair(chirpTimer, nextFiringTime));
    }
}

void TimerManager::removeTimer(ChirpTimer* chirpTimer) {

    // Remove from main timers list
    auto it = std::find(_timers.begin(), _timers.end(), chirpTimer);
    if (it != _timers.end()) {
        _timers.erase(it);
    }
    
    // Remove from firing times list
    auto firingIt = std::find_if(_timerFiringTimes.begin(), _timerFiringTimes.end(),
        [chirpTimer](const std::pair<ChirpTimer*, std::chrono::steady_clock::time_point>& pair) {
            return pair.first == chirpTimer;
        });
    
    if (firingIt != _timerFiringTimes.end()) {
        _timerFiringTimes.erase(firingIt);
    }
}

void TimerManager::computeNextTimerFirringTime() {

    // Find the index of the timer with the lowest firing time
    if (!_timerFiringTimes.empty()) {
        _nextFirringTimerIndex = 0;
        _nextFiringTime = _timerFiringTimes[0].second;
        
        for (size_t i = 1; i < _timerFiringTimes.size(); ++i) {
            if (_timerFiringTimes[i].second < _nextFiringTime) {
                _nextFirringTimerIndex = i;
                _nextFiringTime = _timerFiringTimes[i].second;
            }
        }
    }
}

std::chrono::milliseconds TimerManager::getDurationToNextTimerEvent() const {

    std::chrono::milliseconds result = std::chrono::milliseconds(0);
    
    if (!_timerFiringTimes.empty()) {
        auto currentTime = std::chrono::steady_clock::now();
        
        if (_nextFiringTime > currentTime) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(_nextFiringTime - currentTime);
            result = duration;
        }
    }
    
    return result;
}

void TimerManager::getElapsedTimers(std::vector<ChirpTimer*>& elapsedTimers) const {

    // Clear the output vector first
    elapsedTimers.clear();
    
    // Get current time
    auto currentTime = std::chrono::steady_clock::now();
    
    // Iterate through all timer firing times to find elapsed timers
    // We add a small tolerance (10ms) to account for scheduling delays and timing precision
    for (const auto& timerPair : _timerFiringTimes) {
        if (timerPair.second <= currentTime + std::chrono::milliseconds(2)) {
            // Timer has elapsed or is about to, add it to the result list
            elapsedTimers.push_back(timerPair.first);
        }
    }
}

void TimerManager::rescheduleTimers(const std::vector<ChirpTimer*>& firedTimers) {
    
    // Reschedule only the timers that have fired
    for (ChirpTimer* timer : firedTimers) {
        if (timer && timer->isRunning()) {
            // Find the timer in the firing times vector
            auto it = std::find_if(_timerFiringTimes.begin(), _timerFiringTimes.end(),
                [timer](const std::pair<ChirpTimer*, std::chrono::steady_clock::time_point>& pair) {
                    return pair.first == timer;
                });
            
            if (it != _timerFiringTimes.end()) {
                // Timer exists, use its current firing time as the base for the next one
                auto startTime = it->second;
                std::chrono::milliseconds duration = timer->getDuration();
                auto nextFiringTime = startTime + duration;
                
                _timerFiringTimes.erase(it);
                
                // Add the updated timer with new firing time
                _timerFiringTimes.push_back(std::make_pair(timer, nextFiringTime));
            }
        }
    }
}

