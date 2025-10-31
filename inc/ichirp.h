/**
 * @file ichirp.h
 * @brief Main service interface for Chirp framework
 * @author Chirp Team 
 * @date 2025
 * @version 1.0
 * 
 * This file defines the IChirp class, which provides a high-level interface
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
#include <utility>
#include <type_traits>
#include "chirp_error.h"


// Note: Forward declaration to prevent the inclusion of any private headers.
// This keeps the spirit of data abstraction, so API consumer shall include only
// this one public header file.
class ChirpImpl;
class IChirpTimer;   

/**
 * @brief Main service class for Chirp framework
 * 
 * IChirp provides a high-level interface for creating services that can
 * handle typed messages asynchronously. Each service runs in its own thread
 * and processes messages through registered handlers.
 * 
 * @note This class is thread-safe for message posting, but handlers are
 *       executed sequentially in the service thread.
 * 
 * @example
 * @code
 * IChirp service("MyService");
 * service.registerMsgHandler("TestMessage", myHandler);
 * service.start();
 * service.postMsg("TestMessage", 42, "hello");
 * service.shutdown();
 * @endcode
 */
class IChirp {
public:
    /**
     * @brief Default constructor
     * @note This constructor is provided for backward compatibility but should not be used
     *       for creating functional services. Use the service_name constructor instead.
     */
    IChirp() : _impl(nullptr) {}

    /**
     * @brief Default destructor
     */
    ~IChirp();

    /**
     * @brief Constructor with service name
     * @param service_name The name of the service for identification and logging
     * @param error Output parameter for error status
     * 
     * This constructor creates a new IChirp service instance. If any resource allocation
     * fails during construction, the error parameter will be set to the appropriate
     * error code and the service will not be fully initialized.
     * 
     * @note Always check the error parameter after construction to ensure the service
     *       was created successfully before using it.
     */
    explicit IChirp(const std::string& service_name, ChirpError::Error& error);

    /**
     * @brief Start the service
     * 
     * Initializes and starts the service thread. The service will begin
     * processing messages once started.
     * 
     * @return ChirpError::SUCCESS if the service started successfully,
     *         ChirpError::INVALID_SERVICE_STATE if the service is not properly initialized
     */
    ChirpError::Error start();

    /**
     * @brief Shutdown the service
     * 
     * Gracefully stops the service and waits for completion.
     * 
     * @return ChirpError::SUCCESS if the service shutdown successfully,
     *         ChirpError::INVALID_SERVICE_STATE if the service is not properly initialized
     */
    ChirpError::Error shutdown();

    /**
     * @brief Get the service name
     * @return The name of the service
     */
    std::string getServiceName();

    /**
     * @brief Register a message handler for a non-const member function
     * @tparam Obj Type of the object
     * @tparam Ret Return type of the handler method
     * @tparam Args Variadic template for handler method arguments
     * @param msgName The message name to register the handler for
     * @param object Pointer to the object instance
     * @param method Pointer to the member method
     * @return ChirpError::Error indicating success or failure
     * 
     * Registers a typed message handler that will be called when a message
     * with the specified name is posted. The method signature must match
     * the arguments passed to postMsg().
     * 
     * @note Handlers are executed in the service thread in FIFO order
     * @note This method returns SUCCESS if registration succeeds, HANDLER_ALREADY_EXISTS if a handler
     *       is already registered for this message name, or an appropriate error code on failure
     */
    template<typename Obj, typename Ret, typename... Args>
    ChirpError::Error registerMsgHandler(std::string msgName, 
                                         Obj* object, 
                                         Ret(Obj::*method)(Args...)) {
        if (!_impl) {
            return ChirpError::INVALID_SERVICE_STATE;
        }

        std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>* functions = nullptr;
        getCbMap(functions);

        // Check if a handler is already registered for this message name
        auto it = functions->find(msgName);
        if (it != functions->end()) {
            return ChirpError::HANDLER_ALREADY_EXISTS;
        }

        (*functions)[msgName] = std::bind(&IChirp::executeHandler<Obj, Ret, Args...>, 
                                          this, 
                                          object, 
                                          method, std::placeholders::_1);
        return ChirpError::SUCCESS;
    }

    /**
     * @brief Register a message handler for a const member function
     * @tparam Obj Type of the object
     * @tparam Ret Return type of the handler method
     * @tparam Args Variadic template for handler method arguments
     * @param msgName The message name to register the handler for
     * @param object Pointer to the object instance
     * @param method Pointer to the const member method
     * @return ChirpError::Error indicating success or failure
     * 
     * Registers a typed message handler for a const member function that will be called when a message
     * with the specified name is posted. The method signature must match
     * the arguments passed to postMsg().
     * 
     * @note Handlers are executed in the service thread in FIFO order
     * @note This method returns SUCCESS if registration succeeds, HANDLER_ALREADY_EXISTS if a handler
     *       is already registered for this message name, or an appropriate error code on failure
     * @note Use this version when registering handlers for const member functions
     */
    template<typename Obj, typename Ret, typename... Args>
    ChirpError::Error registerMsgHandler(std::string msgName, 
                                         Obj* object, 
                                         Ret(Obj::*method)(Args...) const) {
        if (!_impl) {
            return ChirpError::INVALID_SERVICE_STATE;
        }

        std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>* functions = nullptr;
        getCbMap(functions);

        // Check if a handler is already registered for this message name
        auto it = functions->find(msgName);
        if (it != functions->end()) {
            return ChirpError::HANDLER_ALREADY_EXISTS;
        }

        (*functions)[msgName] = std::bind(&IChirp::executeConstHandler<Obj, Ret, Args...>, 
                                          this, 
                                          object, 
                                          method, 
                                          std::placeholders::_1);
        return ChirpError::SUCCESS;
    }

    /**
     * @brief Add a timer to the service
     * @param timer Pointer to IChirpTimer instance to add
     * @return ChirpError::Error indicating success or failure
     * 
     * Adds the provided timer to the service's timer manager. The timer
     * will be managed by the service and can fire events based on its configuration.
     * 
     * @note The timer must be properly configured before being added
     * @note This method is thread-safe
     */
    ChirpError::Error addChirpTimer(IChirpTimer* timer);

    /**
     * @brief Remove a timer from the service
     * @param timer Pointer to IChirpTimer instance to remove
     * @return ChirpError::Error indicating success or failure
     * 
     * Removes the provided timer from the service's timer manager. After removal,
     * the timer will no longer fire events through this service.
     * 
     * @note This method is thread-safe
     * @note The timer instance is not deleted by this method
     */
    ChirpError::Error removeChirpTimer(IChirpTimer* timer);

    /**
     * @brief Get the version of the Chirp API
     * @return The version string (e.g., "1.0")
     */
    static const std::string& getVersion();

    // Watchdog monitoring flag
    void setWatchDogMonitoring(bool enabled);
    bool getWatchDogMonitoring() const;

private:
    static const std::string _version;
    /**
     * @brief Base case for collectArgs when there are no arguments left
     * @param args Vector to collect arguments in
     * 
     * This is the base case for the recursive collectArgs template. When there
     * are no more arguments to collect, this method does nothing.
     */
    void collectArgs(std::vector<std::any>&) {
        // Base case: do nothing
    }
    /**
     * @brief Helper function to collect arguments (single argument case)
     * @tparam T Type of the argument
     * @param args Vector to collect arguments in
     * @param arg The argument to add
     * 
     * This method adds a single argument to the args vector by converting it
     * to std::any and pushing it onto the vector.
     */
    template<typename T>
    void collectArgs(std::vector<std::any>& args, T arg) {
        args.push_back(arg);
    }

    /**
     * @brief Specialization for string literals to convert them to std::string
     * @param args Vector to collect arguments in
     * @param arg The string literal to add
     */
    void collectArgs(std::vector<std::any>& args, const char* arg) {
        args.push_back(std::string(arg));
    }

    /**
     * @brief Helper function to collect arguments (recursive case)
     * @tparam T Type of the first argument
     * @tparam Args Types of remaining arguments
     * @param args Vector to collect arguments in
     * @param first The first argument to add
     * @param rest The remaining arguments
     * 
     * This method recursively collects arguments by adding the first argument
     * to the vector and then recursively processing the remaining arguments.
     * This allows for type-safe collection of variadic template arguments.
     */
    template<typename T, typename... Args>
    void collectArgs(std::vector<std::any>& args, T first, Args... rest) {
        args.push_back(first);
        collectArgs(args, rest...);
    }

    
    /**
     * @brief Helper function to validate argument count
     * @tparam Args Types of handler method arguments
     * @param args Vector of arguments to validate
     * @param service_name Name of the service for logging
     * @return ChirpError::Error indicating success or failure
     */
    template<typename... Args>
    ChirpError::Error validateArgCount(const std::vector<std::any>& args, 
                                       const std::string& service_name) {
        // args[0] is the message name, so we need sizeof...(Args) + 1 total elements
        if (args.size() != sizeof...(Args) + 1) {
            return ChirpError::INVALID_ARGUMENTS;
        }
        return ChirpError::SUCCESS;
    }

    /**
     * @brief Execute a non-const member function handler with proper error handling
     * @tparam Obj Type of the object
     * @tparam Ret Return type of the handler method
     * @tparam Args Types of handler method arguments
     * @param object Pointer to the object instance
     * @param method Pointer to the member method
     * @param args Vector of arguments to pass to the handler
     * @return ChirpError::Error indicating success or failure
     */
    template<typename Obj, typename Ret, typename... Args>
    ChirpError::Error executeHandler(Obj* object, 
                                     Ret(Obj::*method)(Args...), 
                                     const std::vector<std::any>& args);

    /**
     * @brief Helper function to execute handler with proper argument unpacking
     * @tparam Obj Type of the object
     * @tparam Ret Return type of the handler method
     * @tparam Args Types of handler method arguments
     * @tparam I Index sequence for argument unpacking
     * @param object Pointer to the object instance
     * @param method Pointer to the member method
     * @param args Vector of arguments to pass to the handler
     */
    template<typename Obj, typename Ret, typename... Args, size_t... I>
    void executeHandlerImpl(Obj* object, 
                            Ret(Obj::*method)(Args...), 
                            const std::vector<std::any>& args, 
                            std::index_sequence<I...>);

    /**
     * @brief Helper function to execute const handler with proper argument unpacking
     * @tparam Obj Type of the object
     * @tparam Ret Return type of the handler method
     * @tparam Args Types of handler method arguments
     * @tparam I Index sequence for argument unpacking
     * @param object Pointer to the object instance
     * @param method Pointer to the const member method
     * @param args Vector of arguments to pass to the handler
     */
    template<typename Obj, typename Ret, typename... Args, size_t... I>
    void executeHandlerImpl(Obj* object, 
                            Ret(Obj::*method)(Args...) const, 
                            const std::vector<std::any>& args, 
                            std::index_sequence<I...>);

    /**
     * @brief Execute a const member function handler with proper error handling
     * @tparam Obj Type of the object
     * @tparam Ret Return type of the handler method
     * @tparam Args Types of handler method arguments
     * @param object Pointer to the object instance
     * @param method Pointer to the const member method
     * @param args Vector of arguments to pass to the handler
     * @return ChirpError::Error indicating success or failure
     */
    template<typename Obj, typename Ret, typename... Args>
    ChirpError::Error executeConstHandler(Obj* object, 
                                          Ret(Obj::*method)(Args...) const, 
                                          const std::vector<std::any>& args);

    // Validate any_cast ability for each argument type without invoking user handler
    template<typename... T, size_t... I>
    bool validateCastsImpl(const std::vector<std::any>& args, std::index_sequence<I...>);

    template<typename... T>
    bool validateCasts(const std::vector<std::any>& args);

    /**
     * @brief Enqueue a message for processing
     * @param msgName The message name
     * @param args The message arguments
     * @return ChirpError::Error indicating success or failure
     */
    ChirpError::Error enqueMsg(std::string& msgName, std::vector<std::any>& args);

    /**
     * @brief Enqueue a message for synchronous processing
     * @param msgName The message name
     * @param args The message arguments
     * @return ChirpError::Error indicating success or failure
     *
     * Enqueues a message to be processed synchronously by the service. This is
     * used internally by syncMsg to ensure the calling thread waits for the
     * handler to complete and return a result.
     *
     * @note This method is intended for internal use by syncMsg.
     */
    ChirpError::Error enqueSyncMsg(std::string& msgName, std::vector<std::any>& args);

    /**
     * @brief Get the callback map for internal use
     * @param funcMap Output parameter for the function map
     * 
     * This method is used internally by the template methods to access
     * the registered message handlers.
     * 
     * @note The returned map contains functions that return ChirpError::Error codes
     */
    void getCbMap(std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap);

    ChirpImpl* _impl; ///< Pointer to the implementation class (PIMPL idiom)

    // Callback to capture validation errors for sync messages
    std::function<void(ChirpError::Error)> _validationCallback;

    // Callback to capture validation errors for async messages
    std::function<void(ChirpError::Error)> _asyncValidationCallback;

    // Thread-local flag to validate only (no user handler invocation)
    static thread_local bool _validateOnly;

    // Watchdog monitoring state (disabled by default)
    bool _watchdogMonitoringEnabled = false;

    /**
     * @brief Helper to build message and argument vector, then enqueue
     * @tparam EnqueueFunc Type of the enqueue function
     * @tparam T Type of the first argument (message name)
     * @tparam Args Variadic template for remaining arguments
     * @param enqueue The function to call with the built message and args
     * @param first_arg The message name (first argument)
     * @param remaining_args The arguments to pass to the handler
     * @return ChirpError::Error indicating success or failure
     * 
     * This method converts the first argument to a string (message name) and collects
     * all remaining arguments into a vector. It then calls the provided enqueue function
     * with the constructed message and arguments.
     * 
     * @note This method always returns SUCCESS as it performs no operations that can fail
     */
    template<typename EnqueueFunc, typename T, typename... Args>
    ChirpError::Error buildAndEnqueue(EnqueueFunc enqueue, 
                                      T first_arg, 
                                      Args... remaining_args) {
        std::ostringstream oss;
        oss << first_arg;
        std::string first_arg_str = oss.str();
        std::vector<std::any> args;
        args.push_back(first_arg);
        collectArgs(args, remaining_args...);
        enqueue(first_arg_str, args);
        return ChirpError::SUCCESS;
    }

public:
    /**
     * @brief Post a message to the service
     * @tparam T Type of the first argument (message name)
     * @tparam Args Variadic template for remaining arguments
     * @param first_arg The message name (first argument)
     * @param remaining_args The arguments to pass to the handler
     * @return ChirpError::Error indicating success or failure
     *
     * Posts a message to the service queue. The first argument is converted
     * to a string and used as the message name. The remaining arguments
     * are passed to the registered handler.
     *
     * @note This method is thread-safe and can be called from any thread
     */
    template<typename T, typename... Args>
    ChirpError::Error postMsg(T first_arg, Args... remaining_args) {
        if (!_impl) {
            return ChirpError::INVALID_SERVICE_STATE;
        }

        // Build args and validate without invoking user handler
        std::vector<std::any> args;
        args.push_back(first_arg);
        collectArgs(args, remaining_args...);

        std::ostringstream oss;
        oss << first_arg;
        std::string msgName = oss.str();

        // Check if handler exists
        std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>* functions = nullptr;
        getCbMap(functions);
        auto it = functions->find(msgName);
        if (it == functions->end()) {
            return ChirpError::HANDLER_NOT_FOUND;
        }

        // Use validate-only path to check argument count/types
        ChirpError::Error validationError = ChirpError::SUCCESS;
        _asyncValidationCallback = [&validationError](ChirpError::Error e){ validationError = e; };
        _validateOnly = true;
        (void)(it->second)(args);
        _validateOnly = false;
        _asyncValidationCallback = nullptr;

        if (validationError != ChirpError::SUCCESS) {
            return validationError;
        }

        // Enqueue for execution on the service thread (single execution)
        return enqueMsg(msgName, args);
    }

    /**
     * @brief Synchronously post a message to the service and wait for the result
     * @tparam T Type of the first argument (message name)
     * @tparam Args Variadic template for remaining arguments
     * @param first_arg The message name (first argument)
     * @param remaining_args The arguments to pass to the handler
     * @return ChirpError::Error indicating success or failure
     *
     * Posts a message to the service and blocks until the handler has processed
     * the message. The first argument is converted to a string and used as the
     * message name. The remaining arguments are passed to the registered handler.
     *
     * @note This method is thread-safe and can be called from any thread.
     * @note The handler function should not be void.
     */
    template<typename T, typename... Args>
    ChirpError::Error syncMsg(T first_arg, Args... remaining_args) {
        if (!_impl) {
            return ChirpError::INVALID_SERVICE_STATE;
        }

        ChirpError::Error validationError = ChirpError::SUCCESS;

        // Set up validation callback to capture errors
        _validationCallback = [&validationError](ChirpError::Error error) {
            validationError = error;
        };

        ChirpError::Error error = ChirpError::SUCCESS;
        ChirpError::Error buildError = buildAndEnqueue(
            [this, &error](std::string& msg, std::vector<std::any>& args) { 
                error = enqueSyncMsg(msg, args); 
            },
            first_arg, remaining_args...
        );

        // Clear the validation callback
        _validationCallback = nullptr;

        if (buildError != ChirpError::SUCCESS) {
            return buildError;
        }

        // Return validation error if one occurred, otherwise return the enqueue error
        return (validationError != ChirpError::SUCCESS) ? validationError : error;
    }
};

#include "ichirp_detail.hpp"

