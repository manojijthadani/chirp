/**
 * @file nice_service.h
 * @brief Main service interface for Nice Services framework
 * @author Nice Services Team (Manoj IJ Thadani)
 * @date 2025
 * @version 1.0
 * 
 * This file defines the NiceService class, which provides a high-level interface
 * for creating and managing services with message-passing capabilities.
 * The class uses template metaprogramming to provide type-safe message handling.
 */

#pragma once
#include <functional>
#include <map>
#include <any>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "nice_service_types.h"

class NiceServiceImpl;

/**
 * @brief Main service class for Nice Services framework
 * 
 * NiceService provides a high-level interface for creating services that can
 * handle typed messages asynchronously. Each service runs in its own thread
 * and processes messages through registered handlers.
 * 
 * @note This class is thread-safe for message posting, but handlers are
 *       executed sequentially in the service thread.
 * 
 * @example
 * @code
 * NiceService service("MyService");
 * service.registerMsgHandler("TestMessage", myHandler);
 * service.start();
 * service.postMsg("TestMessage", 42, "hello");
 * service.shutdown(ShutdownType::NORMAL);
 * @endcode
 */
class NiceService {
public:
    /**
     * @brief Default constructor
     */
    NiceService() = default;
    
    /**
     * @brief Default destructor
     */
    ~NiceService() = default;

    /**
     * @brief Constructor with service name
     * @param service_name The name of the service for identification and logging
     */
    explicit NiceService(const std::string& service_name);
    
    /**
     * @brief Start the service
     * 
     * Initializes and starts the service thread. The service will begin
     * processing messages once started.
     */
    void start();
    
    /**
     * @brief Shutdown the service
     * @param s The shutdown type (NORMAL or FORCE)
     * 
     * Gracefully stops the service and waits for completion.
     */
    void shutdown(ShutdownType s);
    
    /**
     * @brief Get the service name
     * @return The name of the service
     */
    std::string getServiceName();

    /**
     * @brief Register a message handler
     * @tparam Ret Return type of the handler function
     * @tparam Args Variadic template for handler function arguments
     * @param msgName The message name to register the handler for
     * @param func Pointer to the handler function
     * 
     * Registers a typed message handler that will be called when a message
     * with the specified name is posted. The function signature must match
     * the arguments passed to postMsg().
     * 
     * @note Handlers are executed in the service thread in FIFO order
     */
    template<typename Ret, typename... Args>
    void registerMsgHandler(std::string msgName, Ret(*func)(Args...)) {
        std::map<std::string, std::function<void(std::vector<std::any>)>>* functions = nullptr;
        getCbMap(functions);
        (*functions)[msgName] = [func](std::vector<std::any> args) {
            if (args.size() < sizeof...(Args) + 1) {
                std::cerr << "Argument count mismatch (expected at least one extra).\n";
                return;
            }
            std::vector<std::any> slicedArgs(args.begin() + 1, args.end());
            try {
                helper(func, slicedArgs, std::index_sequence_for<Args...>{});
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Argument type mismatch: " << e.what() << '\n';
            }
        };
    }

    /**
     * @brief Post a message to the service
     * @tparam T Type of the first argument (message name)
     * @tparam Args Variadic template for remaining arguments
     * @param first_arg The message name (first argument)
     * @param remaining_args The arguments to pass to the handler
     * 
     * Posts a message to the service queue. The first argument is converted
     * to a string and used as the message name. The remaining arguments
     * are passed to the registered handler.
     * 
     * @note This method is thread-safe and can be called from any thread
     */
    template<typename T, typename... Args>
    void postMsg(T first_arg, Args... remaining_args) {
        std::ostringstream oss;
        oss << first_arg;
        std::string first_arg_str = oss.str();
        std::vector<std::any> args;
        args.push_back(first_arg);
        collectArgs(args, remaining_args...);
        enqueMsg(first_arg_str, args);
    }

private:
    /**
     * @brief Helper function to collect arguments (base case)
     * @tparam T Type of the argument
     * @param args Vector to collect arguments in
     * @param arg The argument to add
     */
    template<typename T>
    void collectArgs(std::vector<std::any>& args, T arg) {
        args.push_back(arg);
    }

    /**
     * @brief Helper function to collect arguments (recursive case)
     * @tparam T Type of the first argument
     * @tparam Args Types of remaining arguments
     * @param args Vector to collect arguments in
     * @param first The first argument to add
     * @param rest The remaining arguments
     */
    template<typename T, typename... Args>
    void collectArgs(std::vector<std::any>& args, T first, Args... rest) {
        args.push_back(first);
        collectArgs(args, rest...);
    }

    /**
     * @brief Helper function to call registered handlers with typed arguments
     * @tparam Ret Return type of the handler function
     * @tparam Args Types of handler function arguments
     * @tparam I Index sequence for argument unpacking
     * @param func Pointer to the handler function
     * @param args Vector of arguments to pass to the handler
     * @param seq Index sequence for unpacking arguments
     */
    template<typename Ret, typename... Args, size_t... I>
    static void helper(Ret(*func)(Args...), const std::vector<std::any>& args, std::index_sequence<I...>) {
        try {
            func(std::any_cast<Args>(args[I])...);
        } catch (const std::bad_any_cast& e) {
            std::cerr << "Argument type mismatch: " << e.what() << "\n";
        }
    }
    
    /**
     * @brief Enqueue a message for processing
     * @param msgName The message name
     * @param args The message arguments
     */
    void enqueMsg(std::string& msgName, std::vector<std::any>& args);
    
    /**
     * @brief Get the callback map for message handlers
     * @param funcMap Reference to store the callback map pointer
     */
    void getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap);
    
    NiceServiceImpl* _impl; ///< Implementation pointer
}; 