/**
 * Unit tests for chirp_timer.h and ichirp_timer.h
 * Generated from function analysis on 2025-01-27
 * Framework: Custom Simple Test Framework
 */

#include "ichirp_timer.h"
#include "chirp_timer.h"
#include "chirp_error.h"
#include "chirp_logger.h"
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <iostream>
#include <cassert>
#include <atomic>
#include <limits>

// Simple test framework without external dependencies
class SimpleTestFramework {
private:
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    std::string currentTestName;

public:
    void startTest(const std::string& testName) {
        currentTestName = testName;
        totalTests++;
        std::cout << "Running: " << testName << std::endl;
    }

    void endTest(bool passed) {
        if (passed) {
            passedTests++;
            std::cout << "✓ PASSED: " << currentTestName << std::endl;
        } else {
            failedTests++;
            std::cout << "✗ FAILED: " << currentTestName << std::endl;
        }
    }

    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            std::cout << "  Assertion failed: " << message << std::endl;
            endTest(false);
            throw std::runtime_error("Test failed: " + message);
        }
    }

    void assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            std::cout << "  Assertion failed: " << message << std::endl;
            endTest(false);
            throw std::runtime_error("Test failed: " + message);
        }
    }

    void assertEquals(int expected, int actual, const std::string& message = "") {
        if (expected != actual) {
            std::cout << "  Expected " << expected << " but got " << actual << ": " << message << std::endl;
            endTest(false);
            throw std::runtime_error("Test failed: " + message);
        }
    }

    void assertEquals(const std::string& expected, const std::string& actual, const std::string& message = "") {
        if (expected != actual) {
            std::cout << "  Expected '" << expected << "' but got '" << actual << "': " << message << std::endl;
            endTest(false);
            throw std::runtime_error("Test failed: " + message);
        }
    }

    void assertEquals(ChirpError::Error expected, ChirpError::Error actual, const std::string& message = "") {
        if (expected != actual) {
            std::cout << "  Expected error " << static_cast<int>(expected) 
                      << " but got " << static_cast<int>(actual) << ": " << message << std::endl;
            endTest(false);
            throw std::runtime_error("Test failed: " + message);
        }
    }

    void assertNoThrow(std::function<void()> func, const std::string& message = "") {
        try {
            func();
        } catch (const std::exception& e) {
            std::cout << "  Exception thrown: " << e.what() << ": " << message << std::endl;
            endTest(false);
            throw std::runtime_error("Test failed: " + message);
        }
    }

    void printSummary() {
        std::cout << "\n=== ChirpTimer Test Summary ===" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;

        if (failedTests == 0) {
            std::cout << "🎉 All ChirpTimer tests passed!" << std::endl;
        }
    }

    int getFailedTests() const { return failedTests; }
};

// Test data
struct TimerTestData {
    static const std::string validMessage;
    static const std::string emptyMessage;
    static constexpr int validDurationMs = 1000;
    static constexpr int zeroDurationMs = 0;
    static constexpr int negativeDurationMs = -100;
    static constexpr int largeDurationMs = 3600000; // 1 hour
};

const std::string TimerTestData::validMessage = "TestTimerMessage";
const std::string TimerTestData::emptyMessage = "";

// Global test framework instance
SimpleTestFramework testFramework;

// ===== IChirpTimer::createTimer TESTS =====

void testCreateTimer_ValidInput_ReturnsNonNull() {
    testFramework.startTest("IChirpTimer_createTimer_ValidInput_ReturnsNonNull");
    
    try {
        IChirpTimer* timer = IChirpTimer::createTimer();
        testFramework.assertTrue(timer != nullptr, "createTimer should return non-null pointer");
        testFramework.assertFalse(timer->isRunning(), "New timer should not be running");
        
        delete timer;
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testCreateTimer_ValidInput_CanBeDeleted() {
    testFramework.startTest("IChirpTimer_createTimer_ValidInput_CanBeDeleted");
    
    try {
        IChirpTimer* timer = IChirpTimer::createTimer();
        testFramework.assertNoThrow([&timer]() { delete timer; }, "Timer should be deletable");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer Constructor TESTS =====

void testChirpTimer_DefaultConstructor_CreatesStoppedTimer() {
    testFramework.startTest("ChirpTimer_DefaultConstructor_CreatesStoppedTimer");
    
    try {
        ChirpTimer timer;
        testFramework.assertFalse(timer.isRunning(), "Default timer should not be running");
        testFramework.assertEquals(ChirpError::SUCCESS, timer.getDuration().count(), 
                                   "Default timer should have zero duration");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpTimer_ParameterizedConstructor_CreatesConfiguredTimer() {
    testFramework.startTest("ChirpTimer_ParameterizedConstructor_CreatesConfiguredTimer");
    
    try {
        ChirpTimer timer(TimerTestData::validMessage, 
                        std::chrono::milliseconds(TimerTestData::validDurationMs));
        testFramework.assertFalse(timer.isRunning(), "Timer should not be running after construction");
        testFramework.assertEquals(TimerTestData::validDurationMs, 
                                   timer.getDuration().count(), 
                                   "Timer should have correct duration");
        testFramework.assertEquals(TimerTestData::validMessage, 
                                   timer.getMessage(), 
                                   "Timer should have correct message");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpTimer_ParameterizedConstructor_WithEmptyMessage_StillCreates() {
    testFramework.startTest("ChirpTimer_ParameterizedConstructor_WithEmptyMessage_StillCreates");
    
    try {
        ChirpTimer timer(TimerTestData::emptyMessage, 
                        std::chrono::milliseconds(TimerTestData::validDurationMs));
        testFramework.assertFalse(timer.isRunning(), "Timer should not be running");
        testFramework.assertEquals(TimerTestData::emptyMessage, timer.getMessage(), 
                                   "Timer should store empty message");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer::configure TESTS =====

void testConfigure_ValidInput_ReturnsSuccess() {
    testFramework.startTest("ChirpTimer_configure_ValidInput_ReturnsSuccess");
    
    try {
        ChirpTimer timer;
        ChirpError::Error result = timer.configure(TimerTestData::validMessage, 
                                                   std::chrono::milliseconds(TimerTestData::validDurationMs));
        testFramework.assertEquals(ChirpError::SUCCESS, result, "Configure should return SUCCESS");
        testFramework.assertEquals(TimerTestData::validMessage, timer.getMessage(), 
                                   "Message should be set");
        testFramework.assertEquals(TimerTestData::validDurationMs, 
                                   timer.getDuration().count(), 
                                   "Duration should be set");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testConfigure_EmptyMessage_ReturnsInvalidArguments() {
    testFramework.startTest("ChirpTimer_configure_EmptyMessage_ReturnsInvalidArguments");
    
    try {
        ChirpTimer timer;
        ChirpError::Error result = timer.configure(TimerTestData::emptyMessage, 
                                                   std::chrono::milliseconds(TimerTestData::validDurationMs));
        testFramework.assertEquals(ChirpError::INVALID_ARGUMENTS, result, 
                                   "Configure with empty message should return INVALID_ARGUMENTS");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testConfigure_ZeroDuration_ReturnsInvalidArguments() {
    testFramework.startTest("ChirpTimer_configure_ZeroDuration_ReturnsInvalidArguments");
    
    try {
        ChirpTimer timer;
        ChirpError::Error result = timer.configure(TimerTestData::validMessage, 
                                                   std::chrono::milliseconds(TimerTestData::zeroDurationMs));
        testFramework.assertEquals(ChirpError::INVALID_ARGUMENTS, result, 
                                   "Configure with zero duration should return INVALID_ARGUMENTS");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testConfigure_NegativeDuration_ReturnsInvalidArguments() {
    testFramework.startTest("ChirpTimer_configure_NegativeDuration_ReturnsInvalidArguments");
    
    try {
        ChirpTimer timer;
        // Note: negative duration will be converted to milliseconds, but count() will be negative
        std::chrono::milliseconds negDuration(TimerTestData::negativeDurationMs);
        ChirpError::Error result = timer.configure(TimerTestData::validMessage, negDuration);
        testFramework.assertEquals(ChirpError::INVALID_ARGUMENTS, result, 
                                   "Configure with negative duration should return INVALID_ARGUMENTS");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testConfigure_WhileRunning_ReturnsInvalidServiceState() {
    testFramework.startTest("ChirpTimer_configure_WhileRunning_ReturnsInvalidServiceState");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        ChirpError::Error result = timer.configure("NewMessage", 
                                                   std::chrono::milliseconds(2000));
        testFramework.assertEquals(ChirpError::INVALID_SERVICE_STATE, result, 
                                   "Configure while running should return INVALID_SERVICE_STATE");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testConfigure_ReconfigureStoppedTimer_ReturnsSuccess() {
    testFramework.startTest("ChirpTimer_configure_ReconfigureStoppedTimer_ReturnsSuccess");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        timer.stop();
        
        ChirpError::Error result = timer.configure("NewMessage", 
                                                   std::chrono::milliseconds(2000));
        testFramework.assertEquals(ChirpError::SUCCESS, result, 
                                   "Reconfigure stopped timer should return SUCCESS");
        testFramework.assertEquals(std::string("NewMessage"), timer.getMessage(), 
                                   "Message should be updated");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testConfigure_LargeDuration_ReturnsSuccess() {
    testFramework.startTest("ChirpTimer_configure_LargeDuration_ReturnsSuccess");
    
    try {
        ChirpTimer timer;
        ChirpError::Error result = timer.configure(TimerTestData::validMessage, 
                                                   std::chrono::milliseconds(TimerTestData::largeDurationMs));
        testFramework.assertEquals(ChirpError::SUCCESS, result, 
                                   "Configure with large duration should return SUCCESS");
        testFramework.assertEquals(TimerTestData::largeDurationMs, 
                                   timer.getDuration().count(), 
                                   "Large duration should be stored correctly");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer::start TESTS =====

void testStart_ValidConfiguration_ReturnsSuccess() {
    testFramework.startTest("ChirpTimer_start_ValidConfiguration_ReturnsSuccess");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        
        ChirpError::Error result = timer.start();
        testFramework.assertEquals(ChirpError::SUCCESS, result, 
                                   "Start should return SUCCESS");
        testFramework.assertTrue(timer.isRunning(), "Timer should be running after start");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testStart_UnconfiguredTimer_ReturnsInvalidArguments() {
    testFramework.startTest("ChirpTimer_start_UnconfiguredTimer_ReturnsInvalidArguments");
    
    try {
        ChirpTimer timer;
        ChirpError::Error result = timer.start();
        testFramework.assertEquals(ChirpError::INVALID_ARGUMENTS, result, 
                                   "Start unconfigured timer should return INVALID_ARGUMENTS");
        testFramework.assertFalse(timer.isRunning(), "Timer should not be running");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testStart_ZeroDuration_ReturnsInvalidArguments() {
    testFramework.startTest("ChirpTimer_start_ZeroDuration_ReturnsInvalidArguments");
    
    try {
        ChirpTimer timer;
        // Configure with zero duration (should fail, but test the start path)
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(0));
        ChirpError::Error result = timer.start();
        testFramework.assertEquals(ChirpError::INVALID_ARGUMENTS, result, 
                                   "Start with zero duration should return INVALID_ARGUMENTS");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testStart_AlreadyRunning_ReturnsInvalidServiceState() {
    testFramework.startTest("ChirpTimer_start_AlreadyRunning_ReturnsInvalidServiceState");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        ChirpError::Error result = timer.start();
        testFramework.assertEquals(ChirpError::INVALID_SERVICE_STATE, result, 
                                   "Start already running timer should return INVALID_SERVICE_STATE");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testStart_RecordsStartTime() {
    testFramework.startTest("ChirpTimer_start_RecordsStartTime");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        
        auto beforeStart = std::chrono::steady_clock::now();
        timer.start();
        auto afterStart = std::chrono::steady_clock::now();
        auto recordedStart = timer.getTimerStartTime();
        
        // Start time should be between before and after
        testFramework.assertTrue(recordedStart >= beforeStart, 
                                "Start time should be >= before start");
        testFramework.assertTrue(recordedStart <= afterStart, 
                                "Start time should be <= after start");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer::stop TESTS =====

void testStop_RunningTimer_ReturnsSuccess() {
    testFramework.startTest("ChirpTimer_stop_RunningTimer_ReturnsSuccess");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        ChirpError::Error result = timer.stop();
        testFramework.assertEquals(ChirpError::SUCCESS, result, 
                                   "Stop should return SUCCESS");
        testFramework.assertFalse(timer.isRunning(), "Timer should not be running after stop");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testStop_StoppedTimer_ReturnsSuccess() {
    testFramework.startTest("ChirpTimer_stop_StoppedTimer_ReturnsSuccess");
    
    try {
        ChirpTimer timer;
        ChirpError::Error result = timer.stop();
        testFramework.assertEquals(ChirpError::SUCCESS, result, 
                                   "Stop stopped timer should return SUCCESS");
        testFramework.assertFalse(timer.isRunning(), "Timer should remain stopped");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testStop_MultipleTimes_ReturnsSuccess() {
    testFramework.startTest("ChirpTimer_stop_MultipleTimes_ReturnsSuccess");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        timer.stop();
        ChirpError::Error result1 = timer.stop();
        ChirpError::Error result2 = timer.stop();
        
        testFramework.assertEquals(ChirpError::SUCCESS, result1, 
                                   "First stop should return SUCCESS");
        testFramework.assertEquals(ChirpError::SUCCESS, result2, 
                                   "Second stop should return SUCCESS");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer::isRunning TESTS =====

void testIsRunning_AfterStart_ReturnsTrue() {
    testFramework.startTest("ChirpTimer_isRunning_AfterStart_ReturnsTrue");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        testFramework.assertTrue(timer.isRunning(), "Timer should be running after start");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testIsRunning_AfterStop_ReturnsFalse() {
    testFramework.startTest("ChirpTimer_isRunning_AfterStop_ReturnsFalse");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        timer.stop();
        
        testFramework.assertFalse(timer.isRunning(), "Timer should not be running after stop");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testIsRunning_DefaultConstructor_ReturnsFalse() {
    testFramework.startTest("ChirpTimer_isRunning_DefaultConstructor_ReturnsFalse");
    
    try {
        ChirpTimer timer;
        testFramework.assertFalse(timer.isRunning(), "Default timer should not be running");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer::getDuration TESTS =====

void testGetDuration_AfterConfiguration_ReturnsCorrectValue() {
    testFramework.startTest("ChirpTimer_getDuration_AfterConfiguration_ReturnsCorrectValue");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        
        testFramework.assertEquals(TimerTestData::validDurationMs, 
                                   timer.getDuration().count(), 
                                   "Duration should match configured value");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testGetDuration_AfterReconfiguration_ReturnsNewValue() {
    testFramework.startTest("ChirpTimer_getDuration_AfterReconfiguration_ReturnsNewValue");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        timer.stop();
        timer.configure("NewMessage", std::chrono::milliseconds(2000));
        
        testFramework.assertEquals(2000, timer.getDuration().count(), 
                                   "Duration should be updated after reconfiguration");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testGetDuration_DefaultConstructor_ReturnsZero() {
    testFramework.startTest("ChirpTimer_getDuration_DefaultConstructor_ReturnsZero");
    
    try {
        ChirpTimer timer;
        testFramework.assertEquals(0, timer.getDuration().count(), 
                                   "Default timer should have zero duration");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer::getMessage TESTS =====

void testGetMessage_AfterConfiguration_ReturnsCorrectValue() {
    testFramework.startTest("ChirpTimer_getMessage_AfterConfiguration_ReturnsCorrectValue");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        
        testFramework.assertEquals(TimerTestData::validMessage, timer.getMessage(), 
                                   "Message should match configured value");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testGetMessage_AfterReconfiguration_ReturnsNewValue() {
    testFramework.startTest("ChirpTimer_getMessage_AfterReconfiguration_ReturnsNewValue");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        timer.stop();
        timer.configure("NewMessage", std::chrono::milliseconds(2000));
        
        testFramework.assertEquals(std::string("NewMessage"), timer.getMessage(), 
                                   "Message should be updated after reconfiguration");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testGetMessage_DefaultConstructor_ReturnsEmpty() {
    testFramework.startTest("ChirpTimer_getMessage_DefaultConstructor_ReturnsEmpty");
    
    try {
        ChirpTimer timer;
        testFramework.assertEquals(std::string(""), timer.getMessage(), 
                                   "Default timer should have empty message");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer::getTimerStartTime TESTS =====

void testGetTimerStartTime_AfterStart_ReturnsValidTime() {
    testFramework.startTest("ChirpTimer_getTimerStartTime_AfterStart_ReturnsValidTime");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        
        auto beforeStart = std::chrono::steady_clock::now();
        timer.start();
        auto startTime = timer.getTimerStartTime();
        auto afterStart = std::chrono::steady_clock::now();
        
        testFramework.assertTrue(startTime >= beforeStart, 
                                "Start time should be >= before start");
        testFramework.assertTrue(startTime <= afterStart, 
                                "Start time should be <= after start");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== ChirpTimer Destructor TESTS =====

void testDestructor_RunningTimer_StopsTimer() {
    testFramework.startTest("ChirpTimer_destructor_RunningTimer_StopsTimer");
    
    try {
        {
            ChirpTimer timer;
            timer.configure(TimerTestData::validMessage, 
                           std::chrono::milliseconds(TimerTestData::validDurationMs));
            timer.start();
            // Timer should be stopped in destructor
        }
        // If we get here without issues, destructor worked
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testDestructor_StoppedTimer_NoErrors() {
    testFramework.startTest("ChirpTimer_destructor_StoppedTimer_NoErrors");
    
    try {
        {
            ChirpTimer timer;
            timer.configure(TimerTestData::validMessage, 
                           std::chrono::milliseconds(TimerTestData::validDurationMs));
            timer.start();
            timer.stop();
            // Destructor should handle stopped timer
        }
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== Thread Safety Tests =====

void testThreadSafety_ConcurrentConfigure() {
    testFramework.startTest("ChirpTimer_threadSafety_ConcurrentConfigure");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        timer.stop();
        
        std::atomic<int> successCount(0);
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back([&timer, &successCount, i]() {
                std::string msg = "Message" + std::to_string(i);
                ChirpError::Error result = timer.configure(msg, 
                                                           std::chrono::milliseconds(1000 + i));
                if (result == ChirpError::SUCCESS) {
                    successCount++;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        // At least one configure should succeed
        testFramework.assertTrue(successCount > 0, 
                                "At least one concurrent configure should succeed");
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testThreadSafety_ConcurrentIsRunning() {
    testFramework.startTest("ChirpTimer_threadSafety_ConcurrentIsRunning");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        std::atomic<int> runningCount(0);
        std::vector<std::thread> threads;
        
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&timer, &runningCount]() {
                if (timer.isRunning()) {
                    runningCount++;
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        // All should see running state
        testFramework.assertEquals(10, runningCount.load(), 
                                   "All threads should see running state");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== Additional Coverage Tests =====

void testStart_WithRunningState_ValidateConfigurationFails() {
    testFramework.startTest("ChirpTimer_start_WithRunningState_ValidateConfigurationFails");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        // Try to start again - validateConfiguration should detect RUNNING state
        ChirpError::Error result = timer.start();
        testFramework.assertEquals(ChirpError::INVALID_SERVICE_STATE, result, 
                                   "Start when already running should fail validation");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testConfigure_SuccessPath_SetsValues() {
    testFramework.startTest("ChirpTimer_configure_SuccessPath_SetsValues");
    
    try {
        ChirpTimer timer;
        std::string newMessage = "NewTestMessage";
        int newDuration = 2000;
        
        ChirpError::Error result = timer.configure(newMessage, 
                                                   std::chrono::milliseconds(newDuration));
        testFramework.assertEquals(ChirpError::SUCCESS, result, 
                                   "Configure should succeed");
        testFramework.assertEquals(newMessage, timer.getMessage(), 
                                   "Message should be set");
        testFramework.assertEquals(newDuration, timer.getDuration().count(), 
                                   "Duration should be set");
        
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testValidateConfiguration_ThroughStart_WhenRunning() {
    testFramework.startTest("ChirpTimer_validateConfiguration_ThroughStart_WhenRunning");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        // validateConfiguration is called by start() - should detect RUNNING state
        ChirpError::Error result = timer.start();
        testFramework.assertEquals(ChirpError::INVALID_SERVICE_STATE, result, 
                                   "validateConfiguration should detect RUNNING state");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testValidateConfiguration_ThroughStart_WhenInvalidDuration() {
    testFramework.startTest("ChirpTimer_validateConfiguration_ThroughStart_WhenInvalidDuration");
    
    try {
        ChirpTimer timer;
        // Configure with zero duration (should fail, but test the start path)
        timer.configure(TimerTestData::validMessage, std::chrono::milliseconds(0));
        
        // Start should call validateConfiguration which checks duration
        ChirpError::Error result = timer.start();
        testFramework.assertEquals(ChirpError::INVALID_ARGUMENTS, result, 
                                   "validateConfiguration should detect invalid duration");
        
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testGetDuration_WhileRunning_StillReturnsValue() {
    testFramework.startTest("ChirpTimer_getDuration_WhileRunning_StillReturnsValue");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        // Should be able to get duration even while running
        testFramework.assertEquals(TimerTestData::validDurationMs, 
                                   timer.getDuration().count(), 
                                   "Should get duration while running");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testGetMessage_WhileRunning_StillReturnsValue() {
    testFramework.startTest("ChirpTimer_getMessage_WhileRunning_StillReturnsValue");
    
    try {
        ChirpTimer timer;
        timer.configure(TimerTestData::validMessage, 
                       std::chrono::milliseconds(TimerTestData::validDurationMs));
        timer.start();
        
        // Should be able to get message even while running
        testFramework.assertEquals(TimerTestData::validMessage, timer.getMessage(), 
                                   "Should get message while running");
        
        timer.stop();
        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// ===== Main Test Runner =====

int main() {
    std::cout << "=== ChirpTimer Unit Tests ===" << std::endl;
    
    // IChirpTimer::createTimer tests
    testCreateTimer_ValidInput_ReturnsNonNull();
    testCreateTimer_ValidInput_CanBeDeleted();
    
    // Constructor tests
    testChirpTimer_DefaultConstructor_CreatesStoppedTimer();
    testChirpTimer_ParameterizedConstructor_CreatesConfiguredTimer();
    testChirpTimer_ParameterizedConstructor_WithEmptyMessage_StillCreates();
    
    // Configure tests
    testConfigure_ValidInput_ReturnsSuccess();
    testConfigure_EmptyMessage_ReturnsInvalidArguments();
    testConfigure_ZeroDuration_ReturnsInvalidArguments();
    testConfigure_NegativeDuration_ReturnsInvalidArguments();
    testConfigure_WhileRunning_ReturnsInvalidServiceState();
    testConfigure_ReconfigureStoppedTimer_ReturnsSuccess();
    testConfigure_LargeDuration_ReturnsSuccess();
    
    // Start tests
    testStart_ValidConfiguration_ReturnsSuccess();
    testStart_UnconfiguredTimer_ReturnsInvalidArguments();
    testStart_ZeroDuration_ReturnsInvalidArguments();
    testStart_AlreadyRunning_ReturnsInvalidServiceState();
    testStart_RecordsStartTime();
    
    // Stop tests
    testStop_RunningTimer_ReturnsSuccess();
    testStop_StoppedTimer_ReturnsSuccess();
    testStop_MultipleTimes_ReturnsSuccess();
    
    // isRunning tests
    testIsRunning_AfterStart_ReturnsTrue();
    testIsRunning_AfterStop_ReturnsFalse();
    testIsRunning_DefaultConstructor_ReturnsFalse();
    
    // getDuration tests
    testGetDuration_AfterConfiguration_ReturnsCorrectValue();
    testGetDuration_AfterReconfiguration_ReturnsNewValue();
    testGetDuration_DefaultConstructor_ReturnsZero();
    
    // getMessage tests
    testGetMessage_AfterConfiguration_ReturnsCorrectValue();
    testGetMessage_AfterReconfiguration_ReturnsNewValue();
    testGetMessage_DefaultConstructor_ReturnsEmpty();
    
    // getTimerStartTime tests
    testGetTimerStartTime_AfterStart_ReturnsValidTime();
    
    // Destructor tests
    testDestructor_RunningTimer_StopsTimer();
    testDestructor_StoppedTimer_NoErrors();
    
    // Thread safety tests
    testThreadSafety_ConcurrentConfigure();
    testThreadSafety_ConcurrentIsRunning();
    
    // Additional coverage tests
    testStart_WithRunningState_ValidateConfigurationFails();
    testConfigure_SuccessPath_SetsValues();
    testValidateConfiguration_ThroughStart_WhenRunning();
    testValidateConfiguration_ThroughStart_WhenInvalidDuration();
    testGetDuration_WhileRunning_StillReturnsValue();
    testGetMessage_WhileRunning_StillReturnsValue();
    
    testFramework.printSummary();
    return testFramework.getFailedTests() > 0 ? 1 : 0;
}

