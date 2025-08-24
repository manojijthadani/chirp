![Project Logo](./asset/chirp_logo.png)

# README

## Table of Contents
1. [Purpose](#purpose)
2. [Basic API Usage](#basic-api-usage)
3. [Build](#build)
4. [Authors](#authors)
5. [Credits](#credits)
6. [License](#license)

## Purpose

Software architects are often faced with the decision of choosing between a multi-threaded and a multi-process system design. A multi-process architecture offers strong fault tolerance, as individual processes can be independently restarted in case of failure. However, this benefit comes with the cost of inter-process communication (IPC), which introduces overhead. Modern technologies like socket communication and gRPC facilitate IPC, but they also require serialization, deserialization, and secure data transmission—each of which can add significant latency.

In contrast, a multi-threaded design can offer a compelling trade-off. By leveraging lightweight threads within a single process boundary, systems can achieve parallelism with lower latency, as there's no need for data serialization or security overhead typically required in cross-process communication. However, this approach may reduce flexibility in fault isolation, making the system less resilient to individual thread failures.

Chirp is a project that aims at providing a very light weight and simple API written in C++ for C++ developers that alllows for inter thread communication. Developers must breakdown a thread activity into smaller tasks which can be triggered with simple messages sent to the thread. These tasks are executed in the order the messages were sent to the thread. 

## Basic API Usage

```cpp
// Handler class with methods that match expected arguments
class MessageHandlers {
public:
    void handler(int a, std::string b, std::vector<int> c);
};

// Create handler instance
MessageHandlers handlers;

// Registration using object instance and member method pointer
ChirpError::Error error = service.registerMsgHandler("SomeMessage", 
                                                     &handlers,                                                                                &MessageHandlers::handler);
if (error != ChirpError::SUCCESS) {
    std::cout << "Failed to register MessageType handler: " 
              << ChirpError::errorToString(error) << std::endl;
    return;
}

// Any other piece of code that has an access to service can invoke the 
// handler with a postMsg(..) or syncMsg(..) call. Example shown below
int arg1;
std::string arg2;
std::vector<int> arg3;
// Note: The order and type of parameters of the arguments must match that of the handler.
ChirpError::Error error = service.postMsg("SomeMessage", arg1, arg2, arg3);
if (error != ChirpError::SUCCESS) {
    std::cout << "Failed to post message: " << ChirpError::errorToString(error);
    // Handle the error appropriately
}

```

## Build 

Create the build folder in the same folder that contains the chirp clone.

```
mkdir -p build-chirp && cd chirp
cmake -S . -B ../build-chirp
cd ../build-chirp
cmake . --build
```

### Run Examples
```
./examples/chirp-demo
```

## Authors

***Manoj Thadani***

​	E-Mail : maanojjthadanni@gmail.com

​	LinkedIn : https://www.linkedin.com/in/manojthadani/

***Abhilash Kothapalli***

​	E-Mail : 

​	LinkedIn :

## Credits

[Cursor](https://cursor.com/agents)

## License

This app is open-sourced software licensed under the [MIT license](https://github.com/manojijthadani/chirp/blob/main/LICENSE).
