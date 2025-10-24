#include "ichirp.h"
#include "chirp_factory.h"
#include "chirp_logger.h"
#include "message.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <random>
#include <iomanip>
#include <sstream>
#include <memory>

class BenchmarkSuite {
private:
    std::vector<std::string> results;
    std::mt19937 rng;
    
public:
    BenchmarkSuite() : rng(std::random_device{}()) {}
    
    void addResult(const std::string& test, double time_ms, const std::string& details = "") {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << test << ": " << time_ms << " ms";
        if (!details.empty()) {
            oss << " (" << details << ")";
        }
        results.push_back(oss.str());
    }
    
    void printResults() {
        std::cout << "\n=== BENCHMARK RESULTS ===\n";
        for (const auto& result : results) {
            std::cout << result << std::endl;
        }
        std::cout << "========================\n\n";
    }
    
    template<typename Func>
    double measureTime(Func&& func, int iterations = 1) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0 / iterations; // Convert to milliseconds
    }
};

void benchmarkServiceCreation() {
    BenchmarkSuite suite;
    std::cout << "Running Service Creation Benchmarks...\n";
    
    // Benchmark 1: Single service creation
    double time1 = suite.measureTime([]() {
        ChirpError::Error error;
        Chirp service("BenchmarkService", error);
        if (error != ChirpError::SUCCESS) {
            std::cerr << "Service creation failed\n";
        }
    }, 1000);
    suite.addResult("Single Service Creation", time1, "1000 iterations");
    
    // Benchmark 2: Factory service creation
    double time2 = suite.measureTime([]() {
        ChirpError::Error error;
        Chirp* service = nullptr;
        error = ChirpFactory::getInstance().createService("FactoryService", &service);
        if (error != ChirpError::SUCCESS) {
            std::cerr << "Factory service creation failed\n";
        }
        ChirpFactory::getInstance().destroyService("FactoryService");
    }, 1000);
    suite.addResult("Factory Service Creation", time2, "1000 iterations");
    
    // Benchmark 3: Multiple service creation
    double time3 = suite.measureTime([]() {
        ChirpError::Error error;
        for (int i = 0; i < 100; ++i) {
            std::string serviceName = "Service" + std::to_string(i);
            Chirp service(serviceName, error);
            if (error != ChirpError::SUCCESS) {
                std::cerr << "Multiple service creation failed\n";
                break;
            }
        }
    }, 10);
    suite.addResult("Multiple Service Creation", time3, "100 services, 10 iterations");
    
    suite.printResults();
}

void benchmarkServiceLifecycle() {
    BenchmarkSuite suite;
    std::cout << "Running Service Lifecycle Benchmarks...\n";
    
    // Benchmark 1: Start/Stop cycle
    double time1 = suite.measureTime([]() {
        ChirpError::Error error;
        Chirp service("LifecycleService", error);
        if (error == ChirpError::SUCCESS) {
            service.start();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            service.shutdown();
        }
    }, 100);
    suite.addResult("Start/Stop Cycle", time1, "100 iterations");
    
    // Benchmark 2: Rapid start/stop
    double time2 = suite.measureTime([]() {
        ChirpError::Error error;
        Chirp service("RapidService", error);
        if (error == ChirpError::SUCCESS) {
            for (int i = 0; i < 10; ++i) {
                service.start();
                service.shutdown();
            }
        }
    }, 10);
    suite.addResult("Rapid Start/Stop", time2, "10 cycles, 10 iterations");
    
    suite.printResults();
}

void benchmarkMessageHandling() {
    BenchmarkSuite suite;
    std::cout << "Running Message Handling Benchmarks...\n";
    
    ChirpError::Error error;
    Chirp service("MessageService", error);
    if (error != ChirpError::SUCCESS) {
        std::cerr << "Failed to create service for message benchmarks\n";
        return;
    }
    
    service.start();
    
    // Benchmark 1: Message creation
    double time1 = suite.measureTime([]() {
        std::string msgStr = "test_message";
        std::vector<std::any> args = {42};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 10000);
    suite.addResult("Message Creation", time1, "10000 iterations");
    
    // Benchmark 2: Message with multiple arguments
    double time2 = suite.measureTime([]() {
        std::string msgStr = "complex_message";
        std::vector<std::any> args = {42, std::string("test"), 3.14};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 10000);
    suite.addResult("Complex Message Creation", time2, "10000 iterations");
    
    // Benchmark 3: Message retrieval
    double time3 = suite.measureTime([&service]() {
        std::string msgStr = "retrieve_test";
        std::vector<std::any> args = {123};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
        
        std::string retrieved;
        std::vector<std::any> retrievedArgs;
        Message::MessageType type;
        msg.getMessage(retrieved);
        msg.getArgs(retrievedArgs);
        msg.getMessageType(type);
    }, 10000);
    suite.addResult("Message Retrieval", time3, "10000 iterations");
    
    service.shutdown();
    suite.printResults();
}

void benchmarkLogging() {
    BenchmarkSuite suite;
    std::cout << "Running Logging Benchmarks...\n";
    
    // Benchmark 1: Simple logging
    double time1 = suite.measureTime([]() {
        ChirpLogger::instance("LogBenchmark") << "Simple log message" << std::endl;
    }, 10000);
    suite.addResult("Simple Logging", time1, "10000 iterations");
    
    // Benchmark 2: Complex logging
    double time2 = suite.measureTime([]() {
        ChirpLogger::instance("LogBenchmark") << "Complex log: " << 42 << " " << "string" << " " << 3.14 << std::endl;
    }, 10000);
    suite.addResult("Complex Logging", time2, "10000 iterations");
    
    // Benchmark 3: Service name changes
    double time3 = suite.measureTime([]() {
        auto& logger = ChirpLogger::instance("Service1");
        logger << "Message from Service1" << std::endl;
        logger.setServiceName("Service2");
        logger << "Message from Service2" << std::endl;
    }, 5000);
    suite.addResult("Service Name Changes", time3, "5000 iterations");
    
    suite.printResults();
}

void benchmarkConcurrency() {
    BenchmarkSuite suite;
    std::cout << "Running Concurrency Benchmarks...\n";
    
    // Benchmark 1: Concurrent service creation
    double time1 = suite.measureTime([]() {
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([i]() {
                ChirpError::Error error;
                Chirp service("ConcurrentService" + std::to_string(i), error);
                if (error != ChirpError::SUCCESS) {
                    std::cerr << "Concurrent service creation failed\n";
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }, 10);
    suite.addResult("Concurrent Service Creation", time1, "10 threads, 10 iterations");
    
    // Benchmark 2: Concurrent logging
    double time2 = suite.measureTime([]() {
        std::vector<std::thread> threads;
        for (int i = 0; i < 5; ++i) {
            threads.emplace_back([i]() {
                for (int j = 0; j < 1000; ++j) {
                    ChirpLogger::instance("ConcurrentLog" + std::to_string(i)) 
                        << "Thread " << i << " message " << j << std::endl;
                }
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }, 1);
    suite.addResult("Concurrent Logging", time2, "5 threads, 1000 messages each");
    
    suite.printResults();
}

void benchmarkMemoryUsage() {
    BenchmarkSuite suite;
    std::cout << "Running Memory Usage Benchmarks...\n";
    
    // Benchmark 1: Memory allocation for services
    double time1 = suite.measureTime([]() {
        std::vector<std::unique_ptr<Chirp>> services;
        for (int i = 0; i < 1000; ++i) {
            ChirpError::Error error;
            auto service = std::make_unique<Chirp>("MemoryService" + std::to_string(i), error);
            if (error == ChirpError::SUCCESS) {
                services.push_back(std::move(service));
            }
        }
        // Services will be destroyed when vector goes out of scope
    }, 1);
    suite.addResult("Memory Allocation (1000 services)", time1, "1 iteration");
    
    // Benchmark 2: Message memory usage (using pointers to avoid copy issues)
    double time2 = suite.measureTime([]() {
        std::vector<std::unique_ptr<Message>> messages;
        for (int i = 0; i < 10000; ++i) {
            std::string msgStr = "memory_test";
            std::vector<std::any> args = {i};
            messages.push_back(std::make_unique<Message>(msgStr, Message::MessageType::ASYNC, args));
        }
    }, 1);
    suite.addResult("Message Memory Usage (10000 messages)", time2, "1 iteration");
    
    suite.printResults();
}

void benchmarkFactoryOperations() {
    BenchmarkSuite suite;
    std::cout << "Running Factory Operations Benchmarks...\n";
    
    // Benchmark 1: Factory singleton access
    double time1 = suite.measureTime([]() {
        ChirpFactory& factory = ChirpFactory::getInstance();
        std::string version = factory.getVersion();
    }, 100000);
    suite.addResult("Factory Singleton Access", time1, "100000 iterations");
    
    // Benchmark 2: Service count operations
    double time2 = suite.measureTime([]() {
        ChirpFactory& factory = ChirpFactory::getInstance();
        int count = factory.getServiceCount();
    }, 100000);
    suite.addResult("Service Count Operations", time2, "100000 iterations");
    
    // Benchmark 3: Create and destroy cycle
    double time3 = suite.measureTime([]() {
        ChirpFactory& factory = ChirpFactory::getInstance();
        ChirpError::Error error;
        Chirp* service = nullptr;
        error = factory.createService("TempService", &service);
        if (error == ChirpError::SUCCESS) {
            factory.destroyService("TempService");
        }
    }, 1000);
    suite.addResult("Create/Destroy Cycle", time3, "1000 iterations");
    
    suite.printResults();
}

int main() {
    std::cout << "Chirp Library Performance Benchmarks\n";
    std::cout << "====================================\n\n";
    
    try {
        benchmarkServiceCreation();
        benchmarkServiceLifecycle();
        benchmarkMessageHandling();
        benchmarkLogging();
        benchmarkConcurrency();
        benchmarkMemoryUsage();
        benchmarkFactoryOperations();
        
        std::cout << "All benchmarks completed successfully!\n";
        std::cout << "Performance analysis complete.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Benchmark failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}