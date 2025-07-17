//
// Created by manoj ij thadani on 7/9/25.
//
#include <iostream>
#include <bits/ostream.tcc>
#include <thread>
#include <chrono>

#include "message_loop.h"
#include "nice_logger.h"


void MessageLoop::spin() {

    bool st_thread = false;
    
    NiceLogger::instance(_service_name) << "starting spin loop for " << _service_name << std::endl;
    setStopThread(st_thread);
    _empty_mtx.lock();
    while (!st_thread) {

        if (_message_queue.empty()) {

            NiceLogger::instance(_service_name) << _service_name << " is blocked " 
                                   << " st_thread is " << st_thread 
                                   <<  std::endl;
            _empty_mtx.lock();
        }

        _task_exec_mtx.lock();
        if (!_message_queue.empty()) {

            Message* m = _message_queue.front();
            _message_queue.pop();

            std::string msg;
            std::vector<std::any> args;
            m->getMessage(msg);
            m->getArgs(args);

            auto it = _functions.find(msg);
            if (it != _functions.end()) {
                 
                it->second(args);
            }

            delete m;
        }

        st_thread = getStopThread();
        _task_exec_mtx.unlock();
    }
}

void MessageLoop::enqueue(Message* m) {

    bool st_thread = getStopThread();

    if (!st_thread) {

        NiceLogger::instance(_service_name) << "Enqueing a message " << std::endl;
        _message_queue.push(m);
        if (!_message_queue.empty()) {

            _empty_mtx.unlock();
        }
    }
}

void MessageLoop::setServiceName(const std::string& service_name) {

    _service_name = service_name;
}

void MessageLoop::getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap) {

    funcMap = &_functions;
}

void MessageLoop::setStopThread(bool st) {

    _task_exec_mtx.lock();
    _stop_thread = st;
    _task_exec_mtx.unlock();
    _empty_mtx.unlock();
    NiceLogger::instance(_service_name) << " Stop thread is " << st << std::endl;
}

bool MessageLoop::getStopThread() {

    bool st_thread;
    
    st_thread = _stop_thread;
    return st_thread;
}

void MessageLoop::drainQueue() {

    Message* m;
    _task_exec_mtx.lock();
    while (!_message_queue.empty()) {

        m = _message_queue.front();
        delete m;
        _message_queue.pop();
    }         
    _task_exec_mtx.unlock();
}

void MessageLoop::stop(bool sType) {

    if (sType == false) {

        setStopThread(true);
        _empty_mtx.unlock();
    }
}