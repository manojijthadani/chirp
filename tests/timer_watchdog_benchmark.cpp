/**
 * Benchmark tests for Timer and Watchdog APIs
 * Measures performance of timer and watchdog operations
 */

#include "ichirp_timer.h"
#include "chirp_timer.h"
#include "ichirp_watchdog.h"
#include "chirp_watchdog.hpp"
#include "ichirp_factory.h"
#include "ichirp.h"
#include "chirp_error.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <random>
#include <iomanip>
#include <sstream>
#include <memory>
#include <atomic>

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

void benchmarkTimerCreation() {
    BenchmarkSuite suite;
    std::cout << "Running Timer Creation Benchmarks...\n";
    
    // Benchmark 1: Factory timer creation
    double time1 = suite.measureTime([]() {
        IChirpTimer* timer = IChirpTimer::createTimer();
        if (timer) {
            delete timer;
        }
    }, 10000);
    suite.addResult("Factory Timer Creation", time1, "10000 iterations");
    
    // Benchmark 2: Direct timer construction (default)
    double time2 = suite.measureTime([]() {
        ChirpTimer timer;
    }, 10000);
    suite.addResult("Default Timer Construction", time2, "10000 iterations");
    
    // Benchmark 3: Parameterized timer construction
    double time3 = suite.measureTime([]() {
        ChirpTimer timer("TestMessage", std::chrono::milliseconds(1000));
    }, 10000);
    suite.addResult("Parameterized Timer Construction", time3, "10000 iterations");
    
    // Benchmark 4: Timer creation and configuration
    double time4 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("TestMessage", std::chrono::milliseconds(1000));
    }, 10000);
    suite.addResult("Timer Creation + Configuration", time4, "10000 iterations");
    
    suite.printResults();
}

void benchmarkTimerOperations() {
    BenchmarkSuite suite;
    std::cout << "Running Timer Operations Benchmarks...\n";
    
    // Benchmark 1: Configure operation
    double time1 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("TestMessage", std::chrono::milliseconds(1000));
    }, 10000);
    suite.addResult("Timer Configure", time1, "10000 iterations");
    
    // Benchmark 2: Start operation
    double time2 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("TestMessage", std::chrono::milliseconds(1000));
        timer.start();
        timer.stop();
    }, 1000);
    suite.addResult("Timer Start/Stop Cycle", time2, "1000 iterations");
    
    // Benchmark 3: State query (isRunning)
    double time3 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("TestMessage", std::chrono::milliseconds(1000));
        timer.start();
        bool running = timer.isRunning();
        timer.stop();
        (void)running; // Suppress unused warning
    }, 10000);
    suite.addResult("Timer isRunning Query", time3, "10000 iterations");
    
    // Benchmark 4: Get duration
    double time4 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("TestMessage", std::chrono::milliseconds(1000));
        auto duration = timer.getDuration();
        (void)duration; // Suppress unused warning
    }, 10000);
    suite.addResult("Timer getDuration", time4, "10000 iterations");
    
    // Benchmark 5: Get message
    double time5 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("TestMessage", std::chrono::milliseconds(1000));
        std::string msg = timer.getMessage();
        (void)msg; // Suppress unused warning
    }, 10000);
    suite.addResult("Timer getMessage", time5, "10000 iterations");
    
    // Benchmark 6: Multiple start/stop cycles
    double time6 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("TestMessage", std::chrono::milliseconds(1000));
        for (int i = 0; i < 10; ++i) {
            timer.start();
            timer.stop();
        }
    }, 100);
    suite.addResult("Multiple Start/Stop Cycles", time6, "10 cycles, 100 iterations");
    
    suite.printResults();
}

void benchmarkTimerReconfiguration() {
    BenchmarkSuite suite;
    std::cout << "Running Timer Reconfiguration Benchmarks...\n";
    
    // Benchmark 1: Reconfigure stopped timer
    double time1 = suite.measureTime([]() {
        ChirpTimer timer;
        timer.configure("Message1", std::chrono::milliseconds(1000));
        timer.start();
        timer.stop();
        timer.configure("Message2", std::chrono::milliseconds(2000));
    }, 1000);
    suite.addResult("Reconfigure Stopped Timer", time1, "1000 iterations");
    
    // Benchmark 2: Multiple reconfigurations
    double time2 = suite.measureTime([]() {
        ChirpTimer timer;
        for (int i = 0; i < 10; ++i) {
            std::string msg = "Message" + std::to_string(i);
            timer.configure(msg, std::chrono::milliseconds(1000 + i * 100));
            timer.start();
            timer.stop();
        }
    }, 100);
    suite.addResult("Multiple Reconfigurations", time2, "10 reconfigs, 100 iterations");
    
    suite.printResults();
}

void benchmarkWatchdogCreation() {
    BenchmarkSuite suite;
    std::cout << "Running Watchdog Creation Benchmarks...\n";
    
    // Benchmark 1: Factory watchdog creation
    double time1 = suite.measureTime([]() {
        IChirpWatchDog* watchdog = IChirpWatchDog::createWatchdog("TestWatchdog");
        if (watchdog) {
            delete watchdog;
        }
    }, 1000);
    suite.addResult("Factory Watchdog Creation", time1, "1000 iterations");
    
    // Benchmark 2: Direct watchdog construction
    double time2 = suite.measureTime([]() {
        ChirpWatchDog watchdog("TestWatchdog");
        watchdog.stop();
    }, 1000);
    suite.addResult("Direct Watchdog Construction", time2, "1000 iterations");
    
    // Benchmark 3: Watchdog creation with service
    double time3 = suite.measureTime([]() {
        ChirpWatchDog watchdog("TestWatchdog");
        IChirp* service = watchdog.getChirpService();
        (void)service; // Suppress unused warning
        watchdog.stop();
    }, 1000);
    suite.addResult("Watchdog Creation + Service Access", time3, "1000 iterations");
    
    suite.printResults();
}

void benchmarkWatchdogConfiguration() {
    BenchmarkSuite suite;
    std::cout << "Running Watchdog Configuration Benchmarks...\n";
    
    IChirpFactory& factory = IChirpFactory::getInstance();
    
    // Setup: Create a test service
    IChirp* testService = nullptr;
    factory.createService("BenchmarkService", &testService);
    if (testService) {
        testService->setWatchDogMonitoring(true);
    }
    
    // Benchmark 1: Configure watchdog
    double time1 = suite.measureTime([&factory]() {
        ChirpWatchDog watchdog("BenchmarkWatchdog");
        watchdog.configure(&factory, std::chrono::milliseconds(1000));
        watchdog.stop();
    }, 1000);
    suite.addResult("Watchdog Configure", time1, "1000 iterations");
    
    // Benchmark 2: Configure with multiple services
    double time2 = suite.measureTime([&factory]() {
        // Create additional services
        for (int i = 0; i < 5; ++i) {
            IChirp* svc = nullptr;
            std::string name = "Service" + std::to_string(i);
            factory.createService(name, &svc);
            if (svc) {
                svc->setWatchDogMonitoring(true);
            }
        }
        
        ChirpWatchDog watchdog("BenchmarkWatchdog2");
        watchdog.configure(&factory, std::chrono::milliseconds(1000));
        watchdog.stop();
        
        // Cleanup
        for (int i = 0; i < 5; ++i) {
            std::string name = "Service" + std::to_string(i);
            factory.destroyService(name);
        }
    }, 100);
    suite.addResult("Watchdog Configure (5 services)", time2, "100 iterations");
    
    // Cleanup
    if (testService) {
        factory.destroyService("BenchmarkService");
    }
    
    suite.printResults();
}

void benchmarkWatchdogLifecycle() {
    BenchmarkSuite suite;
    std::cout << "Running Watchdog Lifecycle Benchmarks...\n";
    
    IChirpFactory& factory = IChirpFactory::getInstance();
    
    // Setup: Create a test service
    IChirp* testService = nullptr;
    factory.createService("LifecycleService", &testService);
    if (testService) {
        testService->setWatchDogMonitoring(true);
    }
    
    // Benchmark 1: Start/Stop cycle
    double time1 = suite.measureTime([&factory]() {
        ChirpWatchDog watchdog("LifecycleWatchdog");
        watchdog.configure(&factory, std::chrono::milliseconds(1000));
        watchdog.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        watchdog.stop();
    }, 100);
    suite.addResult("Watchdog Start/Stop Cycle", time1, "100 iterations");
    
    // Benchmark 2: Rapid start/stop
    double time2 = suite.measureTime([&factory]() {
        ChirpWatchDog watchdog("RapidWatchdog");
        watchdog.configure(&factory, std::chrono::milliseconds(1000));
        for (int i = 0; i < 5; ++i) {
            watchdog.start();
            watchdog.stop();
        }
    }, 10);
    suite.addResult("Rapid Watchdog Start/Stop", time2, "5 cycles, 10 iterations");
    
    // Cleanup
    if (testService) {
        factory.destroyService("LifecycleService");
    }
    
    suite.printResults();
}

void benchmarkConcurrentTimers() {
    BenchmarkSuite suite;
    std::cout << "Running Concurrent Timer Benchmarks...\n";
    
    // Benchmark 1: Concurrent timer creation
    double time1 = suite.measureTime([]() {
        std::vector<std::thread> threads;
        std::atomic<int> successCount(0);
        
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&successCount]() {
                for (int j = 0; j < 100; ++j) {
                    ChirpTimer timer;
                    timer.configure("TestMessage", std::chrono::milliseconds(1000));
                    if (timer.getDuration().count() == 1000) {
                        successCount++;
                    }
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
    }, 1);
    suite.addResult("Concurrent Timer Creation", time1, "10 threads, 100 timers each");
    
    // Benchmark 2: Concurrent timer operations
    double time2 = suite.measureTime([]() {
        std::vector<ChirpTimer*> timers;
        for (int i = 0; i < 10; ++i) {
            timers.push_back(new ChirpTimer());
            timers[i]->configure("Message" + std::to_string(i), 
                               std::chrono::milliseconds(1000));
        }
        
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&timers, i]() {
                timers[i]->start();
                timers[i]->stop();
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        for (auto* timer : timers) {
            delete timer;
        }
    }, 100);
    suite.addResult("Concurrent Timer Operations", time2, "10 threads, 100 iterations");
    
    suite.printResults();
}

void benchmarkTimerStateQueries() {
    BenchmarkSuite suite;
    std::cout << "Running Timer State Query Benchmarks...\n";
    
    ChirpTimer timer;
    timer.configure("TestMessage", std::chrono::milliseconds(1000));
    timer.start();
    
    // Benchmark 1: isRunning query
    double time1 = suite.measureTime([&timer]() {
        bool running = timer.isRunning();
        (void)running;
    }, 100000);
    suite.addResult("isRunning Query (Running)", time1, "100000 iterations");
    
    timer.stop();
    
    // Benchmark 2: isRunning query (stopped)
    double time2 = suite.measureTime([&timer]() {
        bool running = timer.isRunning();
        (void)running;
    }, 100000);
    suite.addResult("isRunning Query (Stopped)", time2, "100000 iterations");
    
    // Benchmark 3: getDuration query
    double time3 = suite.measureTime([&timer]() {
        auto duration = timer.getDuration();
        (void)duration;
    }, 100000);
    suite.addResult("getDuration Query", time3, "100000 iterations");
    
    // Benchmark 4: getMessage query
    double time4 = suite.measureTime([&timer]() {
        std::string msg = timer.getMessage();
        (void)msg;
    }, 100000);
    suite.addResult("getMessage Query", time4, "100000 iterations");
    
    suite.printResults();
}

void benchmarkWatchdogMonitoring() {
    BenchmarkSuite suite;
    std::cout << "Running Watchdog Monitoring Benchmarks...\n";
    
    IChirpFactory& factory = IChirpFactory::getInstance();
    
    // Setup: Create multiple monitored services
    std::vector<IChirp*> services;
    for (int i = 0; i < 10; ++i) {
        IChirp* svc = nullptr;
        std::string name = "MonitorService" + std::to_string(i);
        factory.createService(name, &svc);
        if (svc) {
            svc->setWatchDogMonitoring(true);
            services.push_back(svc);
        }
    }
    
    ChirpWatchDog watchdog("MonitorWatchdog");
    watchdog.configure(&factory, std::chrono::milliseconds(100));
    watchdog.start();
    
    // Benchmark 1: Monitor tick processing
    double time1 = suite.measureTime([&watchdog]() {
        // Simulate monitor tick by waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }, 10);
    suite.addResult("Monitor Tick Processing", time1, "10 ticks");
    
    watchdog.stop();
    
    // Cleanup
    for (auto* svc : services) {
        if (svc) {
            std::string name = svc->getServiceName();
            factory.destroyService(name);
        }
    }
    
    suite.printResults();
}

void benchmarkMemoryUsage() {
    BenchmarkSuite suite;
    std::cout << "Running Memory Usage Benchmarks...\n";
    
    // Benchmark 1: Timer memory allocation
    double time1 = suite.measureTime([]() {
        std::vector<ChirpTimer*> timers;
        for (int i = 0; i < 1000; ++i) {
            timers.push_back(new ChirpTimer());
            timers[i]->configure("Message" + std::to_string(i), 
                               std::chrono::milliseconds(1000));
        }
        for (auto* timer : timers) {
            delete timer;
        }
    }, 1);
    suite.addResult("Timer Memory Allocation (1000 timers)", time1, "1 iteration");
    
    // Benchmark 2: Watchdog memory allocation
    double time2 = suite.measureTime([]() {
        std::vector<ChirpWatchDog*> watchdogs;
        for (int i = 0; i < 100; ++i) {
            watchdogs.push_back(new ChirpWatchDog("Watchdog" + std::to_string(i)));
        }
        for (auto* wd : watchdogs) {
            wd->stop();
            delete wd;
        }
    }, 1);
    suite.addResult("Watchdog Memory Allocation (100 watchdogs)", time2, "1 iteration");
    
    suite.printResults();
}

int main(int argc, char* argv[]) {
    std::cout << "=== Timer and Watchdog Benchmark Suite ===\n\n";
    
    if (argc > 1) {
        std::string benchmark = argv[1];
        if (benchmark == "timer-creation") {
            benchmarkTimerCreation();
        } else if (benchmark == "timer-ops") {
            benchmarkTimerOperations();
        } else if (benchmark == "timer-reconfig") {
            benchmarkTimerReconfiguration();
        } else if (benchmark == "watchdog-creation") {
            benchmarkWatchdogCreation();
        } else if (benchmark == "watchdog-config") {
            benchmarkWatchdogConfiguration();
        } else if (benchmark == "watchdog-lifecycle") {
            benchmarkWatchdogLifecycle();
        } else if (benchmark == "concurrent") {
            benchmarkConcurrentTimers();
        } else if (benchmark == "state-queries") {
            benchmarkTimerStateQueries();
        } else if (benchmark == "monitoring") {
            benchmarkWatchdogMonitoring();
        } else if (benchmark == "memory") {
            benchmarkMemoryUsage();
        } else {
            std::cout << "Unknown benchmark: " << benchmark << "\n";
            std::cout << "Available benchmarks:\n";
            std::cout << "  timer-creation, timer-ops, timer-reconfig\n";
            std::cout << "  watchdog-creation, watchdog-config, watchdog-lifecycle\n";
            std::cout << "  concurrent, state-queries, monitoring, memory\n";
            return 1;
        }
    } else {
        // Run all benchmarks
        benchmarkTimerCreation();
        benchmarkTimerOperations();
        benchmarkTimerReconfiguration();
        benchmarkWatchdogCreation();
        benchmarkWatchdogConfiguration();
        benchmarkWatchdogLifecycle();
        benchmarkConcurrentTimers();
        benchmarkTimerStateQueries();
        benchmarkWatchdogMonitoring();
        benchmarkMemoryUsage();
    }
    
    return 0;
}

