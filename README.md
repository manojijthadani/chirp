# Chirp

# README

## Table of Contents
1. [Purpose](#purpose)
2. [Build System](#build-system)
3. [Build](#Build)
4. [Contributors](#Contributors)
5. [License](#License)

## Purpose

Software architects are often faced with the decision of choosing between a multi-threaded and a multi-process system design. A multi-process architecture offers strong fault tolerance, as individual processes can be independently restarted in case of failure. However, this benefit comes with the cost of inter-process communication (IPC), which introduces overhead. Modern technologies like socket communication and gRPC facilitate IPC, but they also require serialization, deserialization, and secure data transmission—each of which can add significant latency.

In contrast, a multi-threaded design can offer a compelling trade-off. By leveraging lightweight threads within a single process boundary, systems can achieve parallelism with lower latency, as there's no need for data serialization or security overhead typically required in cross-process communication. However, this approach may reduce flexibility in fault isolation, making the system less resilient to individual thread failures.

Chirp is a project that aims at providing a very light weight and simple API written in C++ for C++ developers that alllows for inter thread communication. 

## Build 

### Configure

```
mkdir -p build-chirp 
cd chirp
cmake -S . -B ../build-chirp
cd ../build-chirp
cmake . --build
```

### Run Examples
```
./examples/chirp-demo
```

## Contributors

Manoj Thadani

​	E-Mail : maanojjthadanni@gmail.com

​	LinkedIn :

Abhilash Kothapalli

​	E-Mail : 

​	LinkedIn :

## License

This app is open-sourced software licensed under the MIT license.