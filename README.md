# Chirp					![Project Logo](./asset/chirp_logo.png)

# README

## Table of Contents
1. [Purpose](#purpose)
2. [Build](#Build)
3. [Authors](#Authors)
4. [Credits](#Credits)
5. [License](#License)

## Purpose

Software architects are often faced with the decision of choosing between a multi-threaded and a multi-process system design. A multi-process architecture offers strong fault tolerance, as individual processes can be independently restarted in case of failure. However, this benefit comes with the cost of inter-process communication (IPC), which introduces overhead. Modern technologies like socket communication and gRPC facilitate IPC, but they also require serialization, deserialization, and secure data transmission—each of which can add significant latency.

In contrast, a multi-threaded design can offer a compelling trade-off. By leveraging lightweight threads within a single process boundary, systems can achieve parallelism with lower latency, as there's no need for data serialization or security overhead typically required in cross-process communication. However, this approach may reduce flexibility in fault isolation, making the system less resilient to individual thread failures.

Chirp is a project that aims at providing a very light weight and simple API written in C++ for C++ developers that alllows for inter thread communication. Developers must breakdown a thread activity into smaller tasks which can be triggered with simple messages sent to the thread. These tasks are executed in the order the messages were sent to the thread. 

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