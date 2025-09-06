#include "chirp.h"
#include "chirp_logger.h"
#include "message.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <sstream>
#include <memory>
#include <thread>
#include <atomic>

class ThroughputBenchmark {
private:
    std::vector<std::string> results;
    std::mt19937 rng;
    
public:
    ThroughputBenchmark() : rng(std::random_device{}()) {}
    
    void addResult(const std::string& test, double throughput, const std::string& details = "") {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << test << ": " << throughput << " messages/sec";
        if (!details.empty()) {
            oss << " (" << details << ")";
        }
        results.push_back(oss.str());
    }
    
    void printResults() {
        std::cout << "\n=== MESSAGE THROUGHPUT RESULTS ===\n";
        for (const auto& result : results) {
            std::cout << result << std::endl;
        }
        std::cout << "===================================\n\n";
    }
    
    std::string generateRandomString(size_t length) {
        const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::uniform_int_distribution<> dist(0, chars.size() - 1);
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += chars[dist(rng)];
        }
        return result;
    }
    
    template<typename Func>
    double measureThroughput(Func&& func, int iterations, double duration_seconds = 1.0) {
        auto start = std::chrono::high_resolution_clock::now();
        auto end_time = start + std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
            std::chrono::duration<double>(duration_seconds));
        
        int actual_iterations = 0;
        while (std::chrono::high_resolution_clock::now() < end_time) {
            for (int i = 0; i < iterations; ++i) {
                func();
                actual_iterations++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double seconds = duration.count() / 1000000.0;
        
        return actual_iterations / seconds;
    }
};

void benchmarkSmallMessages() {
    ThroughputBenchmark benchmark;
    std::cout << "Testing Small Message Throughput (1-100 bytes)...\n";
    
    // 1 byte message
    double throughput1 = benchmark.measureThroughput([]() {
        std::string msgStr = "a";
        std::vector<std::any> args = {1};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 1000, 2.0);
    benchmark.addResult("1 byte message", throughput1, "single character");
    
    // 10 byte message
    double throughput10 = benchmark.measureThroughput([]() {
        std::string msgStr = "1234567890";
        std::vector<std::any> args = {10, std::string("test")};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 1000, 2.0);
    benchmark.addResult("10 byte message", throughput10, "short string");
    
    // 100 byte message
    double throughput100 = benchmark.measureThroughput([]() {
        std::string msgStr = std::string(100, 'x');
        std::vector<std::any> args = {100, std::string(50, 'y'), 3.14};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 1000, 2.0);
    benchmark.addResult("100 byte message", throughput100, "medium string");
    
    benchmark.printResults();
}

void benchmarkMediumMessages() {
    ThroughputBenchmark benchmark;
    std::cout << "Testing Medium Message Throughput (1KB-10KB)...\n";
    
    // 1KB message
    double throughput1k = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(1024);
        std::vector<std::any> args = {1024, std::string(512, 'a'), 42.0};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 100, 2.0);
    benchmark.addResult("1KB message", throughput1k, "1 kilobyte payload");
    
    // 5KB message
    double throughput5k = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(5120);
        std::vector<std::any> args = {5120, std::string(2560, 'b'), 3.14159};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 50, 2.0);
    benchmark.addResult("5KB message", throughput5k, "5 kilobyte payload");
    
    // 10KB message
    double throughput10k = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(10240);
        std::vector<std::any> args = {10240, std::string(5120, 'c'), 2.71828};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 25, 2.0);
    benchmark.addResult("10KB message", throughput10k, "10 kilobyte payload");
    
    benchmark.printResults();
}

void benchmarkLargeMessages() {
    ThroughputBenchmark benchmark;
    std::cout << "Testing Large Message Throughput (100KB-1MB)...\n";
    
    // 100KB message
    double throughput100k = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(102400);
        std::vector<std::any> args = {102400, std::string(51200, 'd'), 1.41421};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 10, 3.0);
    benchmark.addResult("100KB message", throughput100k, "100 kilobyte payload");
    
    // 500KB message
    double throughput500k = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(512000);
        std::vector<std::any> args = {512000, std::string(256000, 'e'), 1.73205};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 5, 3.0);
    benchmark.addResult("500KB message", throughput500k, "500 kilobyte payload");
    
    // 1MB message
    double throughput1m = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(1048576);
        std::vector<std::any> args = {1048576, std::string(524288, 'f'), 2.23607};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
    }, 2, 3.0);
    benchmark.addResult("1MB message", throughput1m, "1 megabyte payload");
    
    benchmark.printResults();
}

void benchmarkMessageRetrievalThroughput() {
    ThroughputBenchmark benchmark;
    std::cout << "Testing Message Retrieval Throughput...\n";
    
    // Small message retrieval
    double throughput_small = benchmark.measureThroughput([]() {
        std::string msgStr = "test";
        std::vector<std::any> args = {42, std::string("hello")};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
        
        std::string retrieved;
        std::vector<std::any> retrievedArgs;
        Message::MessageType type;
        msg.getMessage(retrieved);
        msg.getArgs(retrievedArgs);
        msg.getMessageType(type);
    }, 1000, 2.0);
    benchmark.addResult("Small message retrieval", throughput_small, "4 byte message");
    
    // Medium message retrieval
    double throughput_medium = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(1024);
        std::vector<std::any> args = {1024, std::string(512, 'x')};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
        
        std::string retrieved;
        std::vector<std::any> retrievedArgs;
        Message::MessageType type;
        msg.getMessage(retrieved);
        msg.getArgs(retrievedArgs);
        msg.getMessageType(type);
    }, 100, 2.0);
    benchmark.addResult("Medium message retrieval", throughput_medium, "1KB message");
    
    // Large message retrieval
    double throughput_large = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(10240);
        std::vector<std::any> args = {10240, std::string(5120, 'y')};
        Message msg(msgStr, Message::MessageType::ASYNC, args);
        
        std::string retrieved;
        std::vector<std::any> retrievedArgs;
        Message::MessageType type;
        msg.getMessage(retrieved);
        msg.getArgs(retrievedArgs);
        msg.getMessageType(type);
    }, 10, 2.0);
    benchmark.addResult("Large message retrieval", throughput_large, "10KB message");
    
    benchmark.printResults();
}

void benchmarkConcurrentMessageThroughput() {
    ThroughputBenchmark benchmark;
    std::cout << "Testing Concurrent Message Throughput...\n";
    
    std::atomic<int> messageCount{0};
    const int numThreads = 4;
    const int messagesPerThread = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&benchmark, &messageCount, messagesPerThread, t]() {
            for (int i = 0; i < messagesPerThread; ++i) {
                std::string msgStr = benchmark.generateRandomString(100);
                std::vector<std::any> args = {i, std::string(50, 'a' + t)};
                Message msg(msgStr, Message::MessageType::ASYNC, args);
                messageCount++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double seconds = duration.count() / 1000000.0;
    double throughput = messageCount.load() / seconds;
    
    benchmark.addResult("Concurrent message creation", throughput, 
                       std::to_string(numThreads) + " threads, " + 
                       std::to_string(messagesPerThread) + " messages each");
    
    benchmark.printResults();
}

void benchmarkMemoryThroughput() {
    ThroughputBenchmark benchmark;
    std::cout << "Testing Memory Allocation Throughput...\n";
    
    // Memory allocation throughput for different message sizes
    double throughput_small = benchmark.measureThroughput([]() {
        std::string msgStr = "small";
        std::vector<std::any> args = {1};
        auto msg = std::make_unique<Message>(msgStr, Message::MessageType::ASYNC, args);
    }, 1000, 2.0);
    benchmark.addResult("Small message memory allocation", throughput_small, "5 byte message");
    
    double throughput_medium = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(1024);
        std::vector<std::any> args = {1024};
        auto msg = std::make_unique<Message>(msgStr, Message::MessageType::ASYNC, args);
    }, 100, 2.0);
    benchmark.addResult("Medium message memory allocation", throughput_medium, "1KB message");
    
    double throughput_large = benchmark.measureThroughput([&benchmark]() {
        std::string msgStr = benchmark.generateRandomString(10240);
        std::vector<std::any> args = {10240};
        auto msg = std::make_unique<Message>(msgStr, Message::MessageType::ASYNC, args);
    }, 10, 2.0);
    benchmark.addResult("Large message memory allocation", throughput_large, "10KB message");
    
    benchmark.printResults();
}

int main() {
    std::cout << "Chirp Library Message Throughput Benchmarks\n";
    std::cout << "===========================================\n\n";
    
    try {
        benchmarkSmallMessages();
        benchmarkMediumMessages();
        benchmarkLargeMessages();
        benchmarkMessageRetrievalThroughput();
        benchmarkConcurrentMessageThroughput();
        benchmarkMemoryThroughput();
        
        std::cout << "All throughput benchmarks completed successfully!\n";
        std::cout << "Message throughput analysis complete.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Throughput benchmark failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
