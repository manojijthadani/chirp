# Nice Services - Architecture and Design Document

## Table of Contents
1. [Abstract](#abstract)
2. [Project Overview](#project-overview)
3. [System Architecture](#system-architecture)
4. [Design Patterns](#design-patterns)
5. [Threading Model](#threading-model)
6. [Message Passing System](#message-passing-system)
7. [Logging System](#logging-system)
8. [API Design](#api-design)
9. [Build System](#build-system)
10. [Testing Strategy](#testing-strategy)
11. [Future Enhancements](#future-enhancements)

## Abstract

Software architects are often faced with the decision of choosing between a multi-threaded and a multi-process system design. A multi-process architecture offers strong fault tolerance, as individual processes can be independently restarted in case of failure. However, this benefit comes with the cost of inter-process communication (IPC), which introduces overhead. Modern technologies like socket communication and gRPC facilitate IPC, but they also require serialization, deserialization, and secure data transmission—each of which can add significant latency.

In contrast, a multi-threaded design can offer a compelling trade-off. By leveraging lightweight threads within a single process boundary, systems can achieve parallelism with lower latency, as there's no need for data serialization or security overhead typically required in cross-process communication. However, this approach may reduce flexibility in fault isolation, making the system less resilient to individual thread failures.

Nice Services is a project that aims at providing a very light weight and simple API written in C++ for C++ developers that alllows for inter thread communication. 

## Project Overview

TThe API for Nice Services enables developers to create services as individual threads. Each service runs in its own thread and is responsible for handling multiple tasks. Tasks are registered to a service using the `registerMsgHandler(..)` function that associates a unique message to a task. Once registered, these tasks are triggered when the service receives corresponding messages through the `postMsg(..)` call. Developers must ensure that the order and data types of parameters remain consistent between the `registerMsgHandler(..)` and `postMsg(..)` calls. If there is a mismatch a message will be provided in the debug log. Internally, each service manages an event queue to handle and dispatch messages to the appropriate task handlers. The tasks registered for a service are gauranteed to run on the same thread and in the order the messages came in. Hence all the tasks for a service are thread safe.

Nice Services is a C++20 library that provides a lightweight, thread-safe message-passing framework for building concurrent services. The framework enables developers to create services that communicate through asynchronous message passing, with support for type-safe message handlers and flexible argument passing.

### Key Features
- **Asynchronous Message Passing**: Services communicate through typed messages that are fast.
- **Thread Safety**: Built-in synchronization mechanisms for concurrent access
- **Type Safety**: Template-based message handlers with compile-time type checking
- **Flexible Argument Passing**: Support for various C++ data types including containers. Parameters can be passed directly to the API's instead of containerising them. 
- **Graceful Shutdown Of Services**: Controlled service termination.
- **Logging Integration**: Debugging support has been provided with the help of a custom built thread-safe logging mechanism.
- **External Dependancies**: The only external library that nice-services depends upon are the standard libraries. 

## System Architecture

The system follows a layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│  ┌─────────────────┐  ┌─────────────────┐  ┌──────────────┐ │
│  │   Service 1     │  │   Service 2     │  │   Service N  │ │
│  └─────────────────┘  └─────────────────┘  └──────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    NiceService Layer                        │
│  ┌────────────────────────────────────────────────────────┐ │
│  │              NiceService                               │ │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐ │ │
│  │  │ Message     │  │ Thread      │  │ Logger          │ │ │
│  │  │ Handler     │  │ Management  │  │ Integration     │ │ │
│  │  │ Registry    │  │             │  │                 │ │ │
│  │  └─────────────┘  └─────────────┘  └─────────────────┘ │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    Threading Layer                          │
│  ┌────────────────────────────────────────────────────────┐ │
│  │              NiceThread                                │ │
│  │  ┌───────────────────────────────────────────────────┐ │ │
│  │  │              MessageLoop                          │ │ │
│  │  │  ┌─────────────┐  ┌─────────────┐  ┌────────────┐ │ │ │
│  │  │  │ Message     │  │ Function    │  │ Thread     │ │ │ │
│  │  │  │ Queue       │  │ Registry    │  │ Control    │ │ │ │
│  │  │  └─────────────┘  └─────────────┘  └────────────┘ │ │ │
│  │  └───────────────────────────────────────────────────┘ │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```



## Design Patterns

### 1. Singleton Pattern (NiceLogger)
- Ensures single logger instance per service
- Thread-safe implementation with mutex protection
- Buffered output for performance

### 2. Template Method Pattern (Message Handlers)
- Type-safe message handler registration
- Compile-time type checking
- Flexible argument passing

### 3. Producer-Consumer Pattern (MessageLoop)
- Message producers (postMsg) and consumers (spin loop)
- Thread-safe queue with mutex synchronization
- Blocking behavior when queue is empty

### 4. Observer Pattern (Message Handlers)
- Services register handlers for specific message types
- Decoupled message processing
- Dynamic handler registration

## Threading Model



### Sequence Diagram

```mermaid
sequenceDiagram
    participant MainThread as Main Thread
    participant ServiceThread as Service Thread (MessageLoop)
    participant Handler as Message Handler

    MainThread->>ServiceThread: registerMsgHandler("MessageType", handler)
    ServiceThread-->>MainThread: Handler registered
    MainThread->>ServiceThread: postMsg("MessageType", args...)
    ServiceThread->>ServiceThread: Enqueue message in queue
    ServiceThread->>ServiceThread: Dequeue message (spin loop)
    ServiceThread->>Handler: Call registered handler with args
    Handler-->>ServiceThread: Handler completes
    ServiceThread-->>MainThread: (optional) Acknowledge or log completion
```



## Message Passing System

### Message Structure
```cpp
class Message {
    std::string _msg;            // Unique message type identifier
    std::vector<std::any> _args; // Parameters associated with the message.
};
```

### Message Flow
1. **Message Creation**: `postMsg()` creates typed Message objects
2. **Message Enqueueing**: Messages are added to thread-safe queue
3. **Message Processing**: MessageLoop processes messages in FIFO order
4. **Handler Execution**: Registered handlers are called with typed arguments

### Type Safety
- Template-based handler registration ensures compile-time type checking
- `std::any` provides runtime type safety for arguments
- Exception handling for type mismatches

## Logging System

### Design Principles
- **Thread Safety**: All logging operations are mutex-protected
- **Performance**: Buffered output with automatic flushing
- **Context Awareness**: Service-specific log contexts
- **Singleton Pattern**: Single logger instance per service

### Usage Pattern
```cpp
NiceLogger::instance("ServiceName") << "Log message" << std::endl;
```

### Features
- Automatic file creation and management
- Thread-safe concurrent access
- Buffered output for performance
- Service name prefixing

## API Design

### Service Creation and Management
```cpp
// Create service
NiceService service("MyService");

// Register message handlers
service.registerMsgHandler("MessageType", handlerFunction);

// Start service
service.start();

// Post messages
service.postMsg("MessageType", arg1, arg2, arg3);

// Shutdown service
service.shutdown(ShutdownType::NORMAL);
```

### Message Handler Registration
```cpp
// Function signature must match expected arguments
void handler(int a, std::string b, std::vector<int> c);

// Registration
service.registerMsgHandler("MessageType", handler);
```

### Supported Data Types
- **Primitive Types**: int, float, double, bool, char, etc.
- **Standard Containers**: vector, map, set, list, deque
- **Pointers and References**: int*, int&, etc.
- **Custom Types**: Any type with proper stream operators

## Build System

### CMake Configuration
- **Minimum Version**: CMake 3.10
- **C++ Standard**: C++20
- **Project Structure**: Modular with separate src, tests, and example directories
- **Testing**: Enabled with `enable_testing()`

### Directory Structure
```
nice-services/
├── CMakeLists.txt          # Root build configuration
├── inc/                    # Header files
├── src/                    # Implementation files
├── tests/                  # Test files
├── example/                # Usage examples
└── docs/                   # Documentation
```

## Testing Strategy

### Current State
- Basic test framework in place
- Example-based testing in `example/main.cpp`
- Comprehensive type testing demonstrated

### Test Categories
- **Message Handler Registration**: Type safety and registration
- **Message Passing**: End-to-end message flow
- **Thread Safety**: Concurrent access patterns
- **Shutdown Behavior**: Graceful termination
- **Error Handling**: Exception scenarios

## Future Enhancements



## Conclusion

Nice Services provides a robust foundation for building concurrent, message-driven applications in C++. The architecture emphasizes simplicity, type safety, and performance while maintaining flexibility for future enhancements. The modular design allows for easy extension and customization to meet specific application requirements.

The framework's thread-safe message passing system, combined with its template-based type safety, makes it suitable for a wide range of concurrent programming scenarios, from simple service communication to complex distributed systems. 