/**
 * @file chirp_error.h
 * @brief Error handling types and enums for the Chirp framework
 * @author Chirp Team
 * @date 2025
 * @version 1.0
 */

#pragma once

#include <string>

class ChirpError {
public:
    /**
     * @brief Enumeration of error codes used throughout the Chirp framework
     */
    enum Error {
        /** @brief Operation completed successfully */
        SUCCESS = 0,
        
        /** @brief Service with the specified name already exists */
        SERVICE_ALREADY_EXISTS,
        
        /** @brief Service with the specified name was not found */
        SERVICE_NOT_FOUND,
        
        /** @brief Service is not in the correct state for the requested operation */
        INVALID_SERVICE_STATE,
        
        /** @brief Message handler with the specified name already exists */
        HANDLER_ALREADY_EXISTS,
        
        /** @brief Message handler with the specified name was not found */
        HANDLER_NOT_FOUND,
        
        /** @brief Invalid message type or format */
        INVALID_MESSAGE,
        
        /** @brief Invalid arguments provided for message handler */
        INVALID_ARGUMENTS,
        
        /** @brief Service is not started */
        SERVICE_NOT_STARTED,
        
        /** @brief Service is already started */
        SERVICE_ALREADY_STARTED,
        
        /** @brief Service is already shut down */
        SERVICE_ALREADY_SHUTDOWN,
        
        /** @brief Internal error occurred */
        INTERNAL_ERROR,
        
        /** @brief Operation timed out */
        TIMEOUT,
        
        /** @brief Invalid configuration or parameters */
        INVALID_CONFIGURATION,
        
        /** @brief Resource allocation failed */
        RESOURCE_ALLOCATION_FAILED,
        
        /** @brief Thread-related error */
        THREAD_ERROR,
        
        /** @brief Unknown or unspecified error */
        UNKNOWN_ERROR
    };

    /**
     * @brief Convert Error enum to string representation
     * @param error The error code to convert
     * @return String representation of the error
     */
    static std::string errorToString(Error error) {
        switch (error) {
            case SUCCESS:                    return "SUCCESS";
            case SERVICE_ALREADY_EXISTS:     return "SERVICE_ALREADY_EXISTS";
            case SERVICE_NOT_FOUND:          return "SERVICE_NOT_FOUND";
            case INVALID_SERVICE_STATE:      return "INVALID_SERVICE_STATE";
            case HANDLER_ALREADY_EXISTS:     return "HANDLER_ALREADY_EXISTS";
            case HANDLER_NOT_FOUND:          return "HANDLER_NOT_FOUND";
            case INVALID_MESSAGE:            return "INVALID_MESSAGE";
            case INVALID_ARGUMENTS:          return "INVALID_ARGUMENTS";
            case SERVICE_NOT_STARTED:        return "SERVICE_NOT_STARTED";
            case SERVICE_ALREADY_STARTED:    return "SERVICE_ALREADY_STARTED";
            case SERVICE_ALREADY_SHUTDOWN:   return "SERVICE_ALREADY_SHUTDOWN";
            case INTERNAL_ERROR:             return "INTERNAL_ERROR";
            case TIMEOUT:                    return "TIMEOUT";
            case INVALID_CONFIGURATION:      return "INVALID_CONFIGURATION";
            case RESOURCE_ALLOCATION_FAILED: return "RESOURCE_ALLOCATION_FAILED";
            case THREAD_ERROR:               return "THREAD_ERROR";
            case UNKNOWN_ERROR:              return "UNKNOWN_ERROR";
            default:                         return "UNKNOWN_ERROR";
        }
    }

    /**
     * @brief Check if a Error represents a successful operation
     * @param error The error code to check
     * @return true if the operation was successful, false otherwise
     */
    static bool isSuccess(Error error) {
        return error == SUCCESS;
    }

    /**
     * @brief Check if a Error represents a failure
     * @param error The error code to check
     * @return true if the operation failed, false otherwise
     */
    static bool isFailure(Error error) {
        return error != SUCCESS;
    }
};


