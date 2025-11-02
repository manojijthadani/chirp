#include "chirp_factory.h"
#include "ichirp.h"
#include "chirp_error.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

// Simple test framework for ChirpFactory tests
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
        std::cout << "\n=== ChirpFactory Test Summary ===" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;

        if (failedTests == 0) {
            std::cout << "🎉 All ChirpFactory tests passed!" << std::endl;
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

// Test message handler for testing service lifecycle
class TestMessageHandler {
public:
    void handleMessage(int value) {
        lastValue = value;
        messageCount++;
    }

    int getLastValue() const { return lastValue; }
    int getMessageCount() const { return messageCount; }
    void reset() { lastValue = 0; messageCount = 0; }

private:
    int lastValue = 0;
    int messageCount = 0;
};

// ===== CHIRP FACTORY TESTS =====

void testChirpFactorySingleton() {
    testFramework.startTest("ChirpFactory_Singleton_Pattern");

    try {
        // Test singleton pattern
        IChirpFactory& factory1 = IChirpFactory::getInstance();
        IChirpFactory& factory2 = IChirpFactory::getInstance();

        // Should be the same instance
        testFramework.assertTrue(&factory1 == &factory2, "Singleton should return same instance");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryGetVersion() {
    testFramework.startTest("ChirpFactory_GetVersion_Correct");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        const std::string& version = factory.getVersion();
        testFramework.assertTrue(!version.empty(), "Version should not be empty");
        testFramework.assertEquals("1.0", version, "Version should be 1.0");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryGetServiceCount() {
    testFramework.startTest("ChirpFactory_GetServiceCount_Initial");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();

        // Should have 0 services initially
        size_t count = factory.getServiceCount();
        testFramework.assertTrue(count == 0, "Initial service count should be 0");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryShutdownAllServices() {
    testFramework.startTest("ChirpFactory_ShutdownAllServices_Empty");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Shutdown all services (should work even with no services)
        factory.shutdownAllServices();
        testFramework.assertTrue(factory.getServiceCount() == 0, "Service count should be 0 after shutdown");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryCreateService() {
    testFramework.startTest("ChirpFactory_CreateService_Success");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();

        // Create a new service
        IChirp* service = nullptr;
        ChirpError::Error result = factory.createService("TestService1", &service);

        testFramework.assertTrue(result == ChirpError::SUCCESS, "Service creation should succeed");
        testFramework.assertTrue(service != nullptr, "Service pointer should not be null");
        testFramework.assertEquals("TestService1", service->getServiceName(), "Service name should match");

        // Clean up
        factory.destroyService("TestService1");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryCreateDuplicateService() {
    testFramework.startTest("ChirpFactory_CreateDuplicateService_Error");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();

        // Create first service
        IChirp* service1 = nullptr;
        ChirpError::Error result1 = factory.createService("TestService2", &service1);
        testFramework.assertTrue(result1 == ChirpError::SUCCESS, "First service creation should succeed");

        // Try to create duplicate service
        IChirp* service2 = nullptr;
        ChirpError::Error result2 = factory.createService("TestService2", &service2);
        testFramework.assertTrue(result2 == ChirpError::SERVICE_ALREADY_EXISTS, "Duplicate service should return error");
        testFramework.assertTrue(service2 == nullptr, "Duplicate service pointer should be null");

        // Clean up
        factory.destroyService("TestService2");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryGetService() {
    testFramework.startTest("ChirpFactory_GetService_Success");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();

        // Create a service
        IChirp* createdService = nullptr;
        factory.createService("TestService3", &createdService);

        // Get the service
        IChirp* retrievedService = factory.getService("TestService3");
        testFramework.assertTrue(retrievedService != nullptr, "Retrieved service should not be null");
        testFramework.assertTrue(retrievedService == createdService, "Retrieved service should be same instance");
        testFramework.assertEquals("TestService3", retrievedService->getServiceName(), "Service name should match");

        // Clean up
        factory.destroyService("TestService3");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryGetNonExistentService() {
    testFramework.startTest("ChirpFactory_GetNonExistentService_Null");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Try to get non-existent service
        IChirp* service = factory.getService("NonExistentService");
        testFramework.assertTrue(service == nullptr, "Non-existent service should return null");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryDestroyService() {
    testFramework.startTest("ChirpFactory_DestroyService_Success");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();

        // Create a service
        IChirp* service = nullptr;
        factory.createService("TestService4", &service);

        // Verify service exists
        testFramework.assertTrue(factory.getServiceCount() == 1, "Service count should be 1");

        // Destroy the service
        bool destroyResult = factory.destroyService("TestService4");
        testFramework.assertTrue(destroyResult == true, "Service destruction should succeed");

        // Verify service is gone
        testFramework.assertTrue(factory.getServiceCount() == 0, "Service count should be 0");
        testFramework.assertTrue(factory.getService("TestService4") == nullptr, "Service should not exist after destruction");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryDestroyNonExistentService() {
    testFramework.startTest("ChirpFactory_DestroyNonExistentService_False");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Try to destroy non-existent service
        bool result = factory.destroyService("NonExistentService");
        testFramework.assertTrue(result == false, "Destroying non-existent service should return false");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryMultipleServices() {
    testFramework.startTest("ChirpFactory_MultipleServices_Management");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();
        testFramework.assertTrue(factory.getServiceCount() == 0, "Initial service count should be 0");

        // Create multiple services
        IChirp* service1 = nullptr;
        IChirp* service2 = nullptr;
        IChirp* service3 = nullptr;

        factory.createService("CountTest1", &service1);
        testFramework.assertTrue(factory.getServiceCount() == 1, "Service count should be 1");

        factory.createService("CountTest2", &service2);
        testFramework.assertTrue(factory.getServiceCount() == 2, "Service count should be 2");

        factory.createService("CountTest3", &service3);
        testFramework.assertTrue(factory.getServiceCount() == 3, "Service count should be 3");

        // Destroy one service
        factory.destroyService("CountTest2");
        testFramework.assertTrue(factory.getServiceCount() == 2, "Service count should be 2 after destruction");

        // Clean up
        factory.shutdownAllServices();
        testFramework.assertTrue(factory.getServiceCount() == 0, "Service count should be 0 after shutdown all");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryServiceLifecycle() {
    testFramework.startTest("ChirpFactory_ServiceLifecycle_Basic");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();

        // Create service
        IChirp* service = nullptr;
        ChirpError::Error createResult = factory.createService("LifecycleTest", &service);
        testFramework.assertTrue(createResult == ChirpError::SUCCESS, "Service creation should succeed");
        testFramework.assertTrue(service != nullptr, "Service should not be null");

        // Test basic operations
        testFramework.assertEquals("LifecycleTest", service->getServiceName(), "Service name should match");

        // Destroy the service through factory
        bool destroyResult = factory.destroyService("LifecycleTest");
        testFramework.assertTrue(destroyResult == true, "Service destruction should succeed");

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

void testChirpFactoryErrorHandling() {
    testFramework.startTest("ChirpFactory_ErrorHandling_Basic");

    try {
        IChirpFactory& factory = IChirpFactory::getInstance();

        // Clean start
        factory.shutdownAllServices();

        // Test with empty service name (might succeed or fail, but shouldn't crash)
        IChirp* service = nullptr;
        ChirpError::Error result = factory.createService("", &service);
        // Result can be SUCCESS or error, as long as no crash occurs

        // Clean up any created services
        factory.shutdownAllServices();

        testFramework.endTest(true);
    } catch (...) {
        testFramework.endTest(false);
    }
}

// Main function for ChirpFactory tests
int main() {
    std::cout << "Starting ChirpFactory Tests\n";
    std::cout << "===========================\n\n";

    try {
        // Basic functionality tests
        testChirpFactorySingleton();
        testChirpFactoryGetVersion();
        testChirpFactoryGetServiceCount();
        testChirpFactoryShutdownAllServices();

        // Service management tests
        testChirpFactoryCreateService();
        testChirpFactoryCreateDuplicateService();
        testChirpFactoryGetService();
        testChirpFactoryGetNonExistentService();
        testChirpFactoryDestroyService();
        testChirpFactoryDestroyNonExistentService();

        // Advanced tests
        testChirpFactoryMultipleServices();
        testChirpFactoryServiceLifecycle();
        testChirpFactoryErrorHandling();
    } catch (const std::exception& e) {
        std::cout << "Test execution failed: " << e.what() << std::endl;
        return 1;
    }

    testFramework.printSummary();
    return 0;
}
