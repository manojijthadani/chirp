#include "chirp_logger.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdlib>

// This test must be run with NICE_SERVICES_DEBUG=1 set BEFORE the program starts
// to ensure the singleton constructor creates the file stream

int main() {
    std::cout << "Starting ChirpLogger Debug Path Test\n";
    std::cout << "=====================================\n\n";

    // Verify debug environment is set
    const char* debug_env = std::getenv("NICE_SERVICES_DEBUG");
    if (!debug_env || std::string(debug_env) != "1") {
        std::cout << "ERROR: NICE_SERVICES_DEBUG must be set to '1' before running this test\n";
        return 1;
    }

    try {
        std::cout << "Testing file logging with debug enabled...\n";

        // Get logger instance - this should create file stream since debug is set
        ChirpLogger& logger = ChirpLogger::instance("DebugPathTest");

        // Test empty service name first
        logger.setServiceName("");
        logger << "Message with empty service name" << std::endl;

        // Test normal service name
        logger.setServiceName("DebugPathTest");
        logger << "Message with service name" << std::endl;

        // Test multiple types
        logger << "Integer: " << 42 << " String: " << "test" << " Float: " << 3.14 << std::endl;

        // Test timestamp formatting
        logger << "Timestamp test message" << std::endl;

        // Test manipulator handling
        logger << "Testing endl manipulator" << std::endl;

        // Give some time for file operations
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Verify file was created and contains our messages
        std::ifstream logFile("nice_log.txt");
        if (!logFile.is_open()) {
            std::cout << "ERROR: Log file was not created\n";
            return 1;
        }

        std::string line;
        int messageCount = 0;
        bool foundEmptyServiceName = false;
        bool foundNormalServiceName = false;
        bool foundTimestamp = false;

        while (std::getline(logFile, line)) {
            std::cout << "Log line: " << line << std::endl;
            messageCount++;

            // Check for empty service name (should have space after timestamp)
            if (line.find("Message with empty service name") != std::string::npos) {
                if (line.find("] ") != std::string::npos && line.find("[DebugPathTest]") == std::string::npos) {
                    foundEmptyServiceName = true;
                }
            }

            // Check for normal service name
            if (line.find("Message with service name") != std::string::npos) {
                if (line.find("[DebugPathTest]") != std::string::npos) {
                    foundNormalServiceName = true;
                }
            }

            // Check for timestamp format: [YYYY-MM-DD][HH:MM:SS.mmm]
            if (line.find("Timestamp test message") != std::string::npos) {
                if (line.find("[20") != std::string::npos && line.find("][") != std::string::npos && line.find(".") != std::string::npos) {
                    foundTimestamp = true;
                }
            }
        }

        logFile.close();

        std::cout << "\nTest Results:\n";
        std::cout << "- Messages found: " << messageCount << std::endl;
        std::cout << "- Empty service name handling: " << (foundEmptyServiceName ? "PASS" : "FAIL") << std::endl;
        std::cout << "- Normal service name handling: " << (foundNormalServiceName ? "PASS" : "FAIL") << std::endl;
        std::cout << "- Timestamp format: " << (foundTimestamp ? "PASS" : "FAIL") << std::endl;

        if (messageCount > 0 && foundEmptyServiceName && foundNormalServiceName && foundTimestamp) {
            std::cout << "\n✓ All debug path tests PASSED!\n";
            return 0;
        } else {
            std::cout << "\n✗ Some debug path tests FAILED!\n";
            return 1;
        }

    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
