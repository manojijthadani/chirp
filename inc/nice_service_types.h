/**
 * @file nice_service_types.h
 * @brief Common type definitions for Nice Services framework
 * @author Nice Services Team (Manoj IJ Thadani)
 * @date 2025
 * @version 1.0
 * 
 * This file contains common type definitions used throughout the Nice Services
 * framework, including enums and other shared types.
 */

#pragma once

/**
 * @brief Enumeration for service shutdown types
 * 
 * Defines the different ways a service can be shut down:
 * - NORMAL: Graceful shutdown, allowing current operations to complete
 * - FORCE: Immediate shutdown, potentially interrupting ongoing operations
 */
enum class ShutdownType {
    NORMAL,  ///< Normal shutdown - graceful termination
    FORCE    ///< Force shutdown - immediate termination
}; 