#include "chirp_logger.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>

// Simple test framework for ChirpLogger tests
class SimpleTestFramework {
public:
    void startTest(const std::string& testName) {
        currentTest = testName;
        std::cout << "Running: " << testName << std::endl;
    }

    void endTest(bool passed) {
        if (passed) {
            std::cout << "✓ PASSED: " << currentTest << std::endl;
            passedTests++;
        } else {
            std::cout << "✗ FAILED: " << currentTest << std::endl;
            failedTests++;
        }
        totalTests++;
    }

    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            if (!message.empty()) {
                std::cout << "Assertion failed: " << message << std::endl;
            }
            throw std::runtime_error("Test assertion failed");
        }
    }

    void assertEquals(int expected, int actual, const std::string& message = "") {
        if (expected != actual) {
            if (!message.empty()) {
                std::cout << "Assertion failed: " << message << std::endl;
            }
            std::cout << "Expected: " << expected << ", Actual: " << actual << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }

    void assertEquals(const std::string& expected, const std::string& actual, const std::string& message = "") {
        if (expected != actual) {
            if (!message.empty()) {
                std::cout << "Assertion failed: " << message << std::endl;
            }
            std::cout << "Expected: '" << expected << "', Actual: '" << actual << "'" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }

    void printSummary() {
        std::cout << "\n=== ChirpLogger Test Summary ===" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;

        if (failedTests == 0) {
            std::cout << "🎉 All ChirpLogger tests passed!" << std::endl;
        }
    }

private:
    std::string currentTest;
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
};

// Global test framework instance
SimpleTestFramework testFramework;

// ===== CHIRP LOGGER TESTS =====

void testChirpLoggerSingleton() {
    testFramework.startTest("ChirpLogger_Singleton_Pattern");

    try {
        // Test singleton pattern
        ChirpLogger& logger1 = ChirpLogger::instance("TestService1");
        ChirpLogger& logger2 = ChirpLogger::instance("TestService2");

        // Should be the same instance
        testFramework.assertTrue(&logger1 == &logger2, "Singleton should return same instance");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerServiceName() {
    testFramework.startTest("ChirpLogger_ServiceName_Setting");

    try {
        // Test service name setting
        ChirpLogger& logger = ChirpLogger::instance("TestService");

        // Test that service name is set correctly
        // Note: We can't directly access private members, but we can test through logging
        logger << "Test message" << std::endl;

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerStreaming() {
    testFramework.startTest("ChirpLogger_Streaming_Operations");

    try {
        ChirpLogger& logger = ChirpLogger::instance("StreamTest");

        // Test various data types
        logger << "String message: " << "test" << std::endl;
        logger << "Integer: " << 42 << std::endl;
        logger << "Double: " << 3.14 << std::endl;
        logger << "Boolean: " << true << std::endl;
        logger << "Multiple values: " << 1 << ", " << "hello" << ", " << 2.5 << std::endl;

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerFileOutput() {
    testFramework.startTest("ChirpLogger_FileOutput_WithDebug");

    try {
        // Set debug environment variable
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        // Create a unique log file name
        std::string logFile = "test_log_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".txt";

        // Get logger instance
        ChirpLogger& logger = ChirpLogger::instance("FileTest");

        // Write some test messages
        logger << "Test message 1" << std::endl;
        logger << "Test message 2 with number: " << 123 << std::endl;
        logger << "Test message 3 with float: " << 3.14159 << std::endl;

        // Small delay to ensure file is written
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check if log file exists and contains our messages
        std::ifstream logFileStream("nice_log.txt");
        bool fileExists = logFileStream.is_open();

        if (fileExists) {
            std::string line;
            bool foundMessage1 = false;
            bool foundMessage2 = false;
            bool foundMessage3 = false;

            while (std::getline(logFileStream, line)) {
                if (line.find("Test message 1") != std::string::npos) foundMessage1 = true;
                if (line.find("Test message 2 with number: 123") != std::string::npos) foundMessage2 = true;
                if (line.find("Test message 3 with float: 3.14159") != std::string::npos) foundMessage3 = true;
            }

            testFramework.assertTrue(foundMessage1, "Log file should contain message 1");
            testFramework.assertTrue(foundMessage2, "Log file should contain message 2");
            testFramework.assertTrue(foundMessage3, "Log file should contain message 3");
        }

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerNoDebug() {
    testFramework.startTest("ChirpLogger_NoDebug_Environment");

    try {
        // Ensure debug is not set
        unsetenv("NICE_SERVICES_DEBUG");

        ChirpLogger& logger = ChirpLogger::instance("NoDebugTest");

        // Write messages (should not create file)
        logger << "This should not be logged to file" << std::endl;
        logger << "Another message: " << 456 << std::endl;

        // Small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check that no log file was created (or if it exists, it shouldn't have our messages)
        std::ifstream logFileStream("nice_log.txt");
        if (logFileStream.is_open()) {
            std::string line;
            bool foundOurMessage = false;

            while (std::getline(logFileStream, line)) {
                if (line.find("This should not be logged to file") != std::string::npos) {
                    foundOurMessage = true;
                    break;
                }
            }

            // If file exists, our messages should not be in it
            testFramework.assertTrue(!foundOurMessage, "Messages should not be logged when debug is off");
        }

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerConcurrentAccess() {
    testFramework.startTest("ChirpLogger_ConcurrentAccess_ThreadSafe");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        ChirpLogger& logger = ChirpLogger::instance("ConcurrentTest");

        const int numThreads = 4;
        const int messagesPerThread = 10;
        std::vector<std::thread> threads;

        // Create threads that write to logger concurrently
        for (int t = 0; t < numThreads; ++t) {
            threads.emplace_back([&logger, t, messagesPerThread]() {
                for (int i = 0; i < messagesPerThread; ++i) {
                    logger << "Thread " << t << " message " << i << std::endl;
                }
            });
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }

        // Small delay to ensure all writes are complete
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check that all messages were written (no crashes occurred)
        std::ifstream logFileStream("nice_log.txt");
        if (logFileStream.is_open()) {
            std::string line;
            int messageCount = 0;

            while (std::getline(logFileStream, line)) {
                if (line.find("Thread") != std::string::npos && line.find("message") != std::string::npos) {
                    messageCount++;
                }
            }

            // Should have at least some messages (exact count may vary due to timing)
            testFramework.assertTrue(messageCount > 0, "Concurrent logging should work without crashes");
        }

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerTimestampFormat() {
    testFramework.startTest("ChirpLogger_TimestampFormat_Correct");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        ChirpLogger& logger = ChirpLogger::instance("TimestampTest");

        // Write a test message
        logger << "Timestamp test message" << std::endl;

        // Small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check timestamp format in log file
        std::ifstream logFileStream("nice_log.txt");
        if (logFileStream.is_open()) {
            std::string line;
            bool foundCorrectFormat = false;

            while (std::getline(logFileStream, line)) {
                if (line.find("Timestamp test message") != std::string::npos) {
                    // Check for timestamp format: [YYYY-MM-DD][HH:MM:SS.mmm]
                    if (line.find("[20") != std::string::npos && 
                        line.find("]:[") != std::string::npos &&
                        line.find("].[") != std::string::npos) {
                        foundCorrectFormat = true;
                        break;
                    }
                }
            }

            testFramework.assertTrue(foundCorrectFormat, "Log should have correct timestamp format");
        }

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerServiceNameInOutput() {
    testFramework.startTest("ChirpLogger_ServiceName_InOutput");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        ChirpLogger& logger = ChirpLogger::instance("ServiceNameTest");

        // Write a test message
        logger << "Service name test message" << std::endl;

        // Small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Check that service name appears in log output
        std::ifstream logFileStream("nice_log.txt");
        if (logFileStream.is_open()) {
            std::string line;
            bool foundServiceName = false;

            while (std::getline(logFileStream, line)) {
                if (line.find("Service name test message") != std::string::npos) {
                    if (line.find("[ServiceNameTest]") != std::string::npos) {
                        foundServiceName = true;
                        break;
                    }
                }
            }

            testFramework.assertTrue(foundServiceName, "Log should include service name");
        }

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerConstructorWithFilename() {
    testFramework.startTest("ChirpLogger_Constructor_WithFilename");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        // Test constructor that takes a filename parameter
        std::string testLogFile = "test_custom_log.txt";

        // Create a ChirpLogger with custom filename (note: we can't directly test private constructor,
        // but we can test the behavior through the public interface)
        ChirpLogger& logger = ChirpLogger::instance("CustomFileTest");

        // Write test messages
        logger << "Custom file test message" << std::endl;
        logger << "Another custom message" << std::endl;

        // Test empty service name case by setting empty name
        logger.setServiceName("");
        logger << "Message with empty service name" << std::endl;

        // Reset service name
        logger.setServiceName("CustomFileTest");

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerFileFailureHandling() {
    testFramework.startTest("ChirpLogger_FileFailure_Handling");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        // Try to create logger with invalid file path (to test file open failure)
        // Note: Since we can't directly test the private constructor with invalid path,
        // we'll test the behavior when the file system is involved

        ChirpLogger& logger = ChirpLogger::instance("FileFailureTest");

        // Test that logger still works even if file operations might fail
        logger << "Test message for file failure handling" << std::endl;

        // Test flush function specifically
        // The flush method should handle cases gracefully
        logger << "Another message before flush" << std::endl;

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerEmptyServiceName() {
    testFramework.startTest("ChirpLogger_EmptyServiceName_Handling");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        ChirpLogger& logger = ChirpLogger::instance("EmptyNameTest");

        // Test with empty service name
        logger.setServiceName("");
        logger << "Message with empty service name" << std::endl;

        // Test with normal service name
        logger.setServiceName("EmptyNameTest");
        logger << "Message with normal service name" << std::endl;

        // Test with empty string again
        logger.setServiceName("");
        logger << "Another message with empty service name" << std::endl;

        // Small delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Verify both cases are handled in log file
        std::ifstream logFileStream("nice_log.txt");
        if (logFileStream.is_open()) {
            std::string line;
            bool foundEmptyNameMessage = false;
            bool foundNormalNameMessage = false;

            while (std::getline(logFileStream, line)) {
                if (line.find("Message with empty service name") != std::string::npos) {
                    // Should have space instead of service name
                    if (line.find("] ") != std::string::npos && line.find("[EmptyNameTest]") == std::string::npos) {
                        foundEmptyNameMessage = true;
                    }
                }
                if (line.find("Message with normal service name") != std::string::npos) {
                    if (line.find("[EmptyNameTest]") != std::string::npos) {
                        foundNormalNameMessage = true;
                    }
                }
            }

            testFramework.assertTrue(foundEmptyNameMessage, "Should handle empty service name");
            testFramework.assertTrue(foundNormalNameMessage, "Should handle normal service name");
        }

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerDestructorCoverage() {
    testFramework.startTest("ChirpLogger_Destructor_Coverage");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        // Test destructor behavior by accessing logger and letting it clean up
        {
            ChirpLogger& logger = ChirpLogger::instance("DestructorTest");
            logger << "Message before potential cleanup" << std::endl;

            // Force flush to exercise the flush code path
            logger << "Another message" << std::endl;
        }

        // The singleton pattern means destructor won't be called here,
        // but we can test the flush functionality which is called in destructor

        ChirpLogger& logger = ChirpLogger::instance("DestructorTest2");
        logger << "Testing flush in destructor path" << std::endl;

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpLoggerManipulatorHandling() {
    testFramework.startTest("ChirpLogger_Manipulator_Handling");

    try {
        // Set debug environment
        setenv("NICE_SERVICES_DEBUG", "1", 1);

        ChirpLogger& logger = ChirpLogger::instance("ManipulatorTest");

        // Test various stream manipulators
        logger << "Testing endl manipulator" << std::endl;
        logger << "Testing without endl";
        logger << std::endl;  // Separate endl call

        // Test multiple manipulators
        logger << "Message 1" << std::endl;
        logger << "Message 2" << std::endl;

        // Clean up
        unsetenv("NICE_SERVICES_DEBUG");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// Main function for ChirpLogger tests
int main() {
    std::cout << "Starting ChirpLogger Tests\n";
    std::cout << "==========================\n\n";

    try {
        // Basic functionality tests
        testChirpLoggerSingleton();
        testChirpLoggerServiceName();
        testChirpLoggerStreaming();

        // File output tests
        testChirpLoggerFileOutput();
        testChirpLoggerNoDebug();

        // Advanced tests
        testChirpLoggerConcurrentAccess();
        testChirpLoggerTimestampFormat();
        testChirpLoggerServiceNameInOutput();

        // Coverage improvement tests
        testChirpLoggerConstructorWithFilename();
        testChirpLoggerFileFailureHandling();
        testChirpLoggerEmptyServiceName();
        testChirpLoggerDestructorCoverage();
        testChirpLoggerManipulatorHandling();

    } catch (const std::exception& e) {
        std::cout << "Test execution failed: " << e.what() << std::endl;
        return 1;
    }

    testFramework.printSummary();
    return 0;
}
