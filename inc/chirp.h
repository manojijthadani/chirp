/**
 * @file chirp.h
 * @brief Main service interface for Chirp framework
 * @author Chirp Team 
 * @date 2025
 * @version 1.0
 * 
 * This file defines the Chirp class, which provides a high-level interface
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
#include "chirp_error.h"


// Note: Forward declaration to prevent the inclusion of any private headers.
// This keeps the spirit of data abstraction, so API consumer shall include only
// this one public header file.
class ChirpImpl;

/**
 * @brief Main service class for Chirp framework
 * 
 * Chirp provides a high-level interface for creating services that can
 * handle typed messages asynchronously. Each service runs in its own thread
 * and processes messages through registered handlers.
 * 
 * @note This class is thread-safe for message posting, but handlers are
 *       executed sequentially in the service thread.
 * 
 * @example
 * @code
 * Chirp service("MyService");
 * service.registerMsgHandler("TestMessage", myHandler);
 * service.start();
 * service.postMsg("TestMessage", 42, "hello");
 * service.shutdown();
 * @endcode
 */
class Chirp {
public:
    /**
     * @brief Default constructor
     * @note This constructor is provided for backward compatibility but should not be used
     *       for creating functional services. Use the service_name constructor instead.
     */
    Chirp() = default;
    
    /**
     * @brief Default destructor
     */
    ~Chirp() = default;

    /**
     * @brief Constructor with service name
     * @param service_name The name of the service for identification and logging
     * @param error Output parameter for error status
     * 
     * This constructor creates a new Chirp service instance. If any resource allocation
     * fails during construction, the error parameter will be set to the appropriate
     * error code and the service will not be fully initialized.
     * 
     * @note Always check the error parameter after construction to ensure the service
     *       was created successfully before using it.
     */
    explicit Chirp(const std::string& service_name, ChirpError::Error& error);
    
    /**
     * @brief Start the service
     * 
     * Initializes and starts the service thread. The service will begin
     * processing messages once started.
     */
    void start();
    
    /**
     * @brief Shutdown the service
     * 
     * Gracefully stops the service and waits for completion.
     */
    void shutdown();
    
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
     * @note This method returns SUCCESS if registration succeeds, or an appropriate error code on failure
     */
    template<typename Obj, typename Ret, typename... Args>
    ChirpError::Error registerMsgHandler(std::string msgName, Obj* object, Ret(Obj::*method)(Args...)) {
        std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>* functions = nullptr;
        getCbMap(functions);
        (*functions)[msgName] = std::bind(&Chirp::executeHandler<Obj, Ret, Args...>, 
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
     * @note This method returns SUCCESS if registration succeeds, or an appropriate error code on failure
     * @note Use this version when registering handlers for const member functions
     */
    template<typename Obj, typename Ret, typename... Args>
    ChirpError::Error registerMsgHandler(std::string msgName, Obj* object, Ret(Obj::*method)(Args...) const) {
        std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>* functions = nullptr;
        getCbMap(functions);
        (*functions)[msgName] = std::bind(&Chirp::executeConstHandler<Obj, Ret, Args...>, 
                                          this, 
                                          object, 
                                          method, 
                                          std::placeholders::_1);
        return ChirpError::SUCCESS;
    }

    /**
     * @brief Get the version of the Chirp API
     * @return The version string (e.g., "1.0")
     */
    static const std::string& getVersion();

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
    ChirpError::Error validateArgCount(const std::vector<std::any>& args, const std::string& service_name) {
        if (args.size() < sizeof...(Args) + 1) {
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
    ChirpError::Error executeHandler(Obj* object, Ret(Obj::*method)(Args...), const std::vector<std::any>& args) {
        ChirpError::Error validateResult = validateArgCount<Args...>(args, 
                                                                     this->getServiceName());
        if (validateResult != ChirpError::SUCCESS) {
                        
            // If we have a validation callback, call it to capture the error
            if (_validationCallback) {
                _validationCallback(validateResult);
            }
            
            // Also call the async validation callback if it exists
            if (_asyncValidationCallback) {
                _asyncValidationCallback(validateResult);
            }
            
            return validateResult;
        }
        std::vector<std::any> slicedArgs(args.begin() + 1, args.end());
        
        // Inline the helper logic
        ChirpError::Error result = ChirpError::SUCCESS;
        try {
            executeHandlerImpl(object, method, slicedArgs, std::index_sequence_for<Args...>{});
        } catch (const std::bad_any_cast& e) {
            
            result = ChirpError::INVALID_ARGUMENTS;
            
            // If we have a validation callback, call it to capture the error
            if (_validationCallback) {
                _validationCallback(result);
            }
            
            // Also call the async validation callback if it exists
            if (_asyncValidationCallback) {
                _asyncValidationCallback(result);
            }
        }
        
        return result;
    }

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
    void executeHandlerImpl(Obj* object, Ret(Obj::*method)(Args...), const std::vector<std::any>& args, std::index_sequence<I...>) {
        (object->*method)(std::any_cast<Args>(args[I])...);
    }

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
    void executeHandlerImpl(Obj* object, Ret(Obj::*method)(Args...) const, const std::vector<std::any>& args, std::index_sequence<I...>) {
        (object->*method)(std::any_cast<Args>(args[I])...);
    }

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
    ChirpError::Error executeConstHandler(Obj* object, Ret(Obj::*method)(Args...) const, const std::vector<std::any>& args) {
        ChirpError::Error validateResult = validateArgCount<Args...>(args, this->getServiceName());
        if (validateResult != ChirpError::SUCCESS) {
                        
            // If we have a validation callback, call it to capture the error
            if (_validationCallback) {
                _validationCallback(validateResult);
            }
            
            // Also call the async validation callback if it exists
            if (_asyncValidationCallback) {
                _asyncValidationCallback(validateResult);
            }
            
            return validateResult;
        }
        std::vector<std::any> slicedArgs(args.begin() + 1, args.end());
        
        // Inline the helper logic
        ChirpError::Error result = ChirpError::SUCCESS;
        try {
            executeHandlerImpl(object, method, slicedArgs, std::index_sequence_for<Args...>{});
        } catch (const std::bad_any_cast& e) {
            
            result = ChirpError::INVALID_ARGUMENTS;
            
            // If we have a validation callback, call it to capture the error
            if (_validationCallback) {
                _validationCallback(result);
            }
            
            // Also call the async validation callback if it exists
            if (_asyncValidationCallback) {
                _asyncValidationCallback(result);
            }
        }
        
        return result;
    }



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
     * @brief Get the callback map for message handlers
     * @param funcMap Reference to store the callback map pointer
     * 
     * This method provides access to the internal callback map that stores
     * all registered message handlers. The map contains function objects that
     * can execute the registered handlers with the appropriate arguments.
     * 
     * @note This method is intended for internal use by the message processing system
     * @note The returned map contains functions that return ChirpError::Error codes
     */
    void getCbMap(std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap);
    
    ChirpImpl* _impl; ///< Pointer to the implementation class (PIMPL idiom)
    
    // Callback to capture validation errors for sync messages
    std::function<void(ChirpError::Error)> _validationCallback;
    
    // Callback to capture validation errors for async messages
    std::function<void(ChirpError::Error)> _asyncValidationCallback;

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
    ChirpError::Error buildAndEnqueue(EnqueueFunc enqueue, T first_arg, Args... remaining_args) {
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
        // For postMsg, we need to validate arguments synchronously
        // We'll use a different approach: create a temporary validation message
        // that gets processed immediately to check for validation errors
        
        ChirpError::Error validationError = ChirpError::SUCCESS;
        
        // Set up a temporary validation callback
        _asyncValidationCallback = [&validationError](ChirpError::Error error) {
            validationError = error;
        };
        
        // Create and process a validation message immediately
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
            _asyncValidationCallback = nullptr;
            return ChirpError::HANDLER_NOT_FOUND;
        }
        
        // Process the message immediately to trigger validation
        ChirpError::Error result = (it->second)(args);
        
        // Clear the validation callback
        _asyncValidationCallback = nullptr;
        
        // If validation failed, return the error
        if (result != ChirpError::SUCCESS) {
            return result;
        }
        
        // If validation passed, enqueue the message normally
        ChirpError::Error error = enqueMsg(msgName, args);
        return error;
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