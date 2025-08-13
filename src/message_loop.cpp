//
// Created by manoj ij thadani on 7/9/25.
//
#include <iostream>
#include <bits/ostream.tcc>
#include <thread>
#include <chrono>

#include "message_loop.h"
#include "chirp_logger.h"
#include "message.h"

void MessageLoop::spin() {
    bool st_thread = false;
    setStopThread(st_thread);
    _empty_mtx.lock();
    while (!st_thread) {
        if (_message_queue.empty()) {
            ChirpLogger::instance(_service_name) << "waiting. MsgQ empty." << std::endl;
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
                ChirpLogger::instance(_service_name) << "Dispatching handler for "
                                                    << msg << " with " << args.size() << " arguments" << std::endl;
                it->second(args);
            }
            Message::MessageType mt;
            m->getMessageType(mt);
            if (mt == Message::MessageType::SYNC) {
                m->sync_notify();
            }
            delete m;
        }
        st_thread = _stop_thread;
        _task_exec_mtx.unlock();
    }
    ChirpLogger::instance(_service_name) << "Spin loop stopped." << std::endl;
}

void MessageLoop::enqueue(Message* m) {
    enqueueInternal(m, Message::MessageType::ASYNC);
}

void MessageLoop::enqueueSync(Message* m) {
    enqueueInternal(m, Message::MessageType::SYNC);
}

void MessageLoop::enqueueInternal(Message* m, Message::MessageType type) {
    if (!_stop_thread) {
        std::string msg;
        m->getMessage(msg);
        ChirpLogger::instance(_service_name) << "Enqueing message " << msg << std::endl;
        _message_queue.push(m);
        if (!_message_queue.empty()) {
            _empty_mtx.unlock();
        }
        if (type == Message::MessageType::SYNC) {
            ChirpLogger::instance(_service_name) 
            << "Blocking caller thread on a sync call " << std::endl;
            m->sync_wait();
            ChirpLogger::instance(_service_name) 
            << "Unblocking caller thread on a sync call " << std::endl;
        }
    }
}

void MessageLoop::setServiceName(const std::string& service_name) {
    _service_name = service_name;
}

void MessageLoop::getCbMap(std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap) {
    funcMap = &_functions;
}

void MessageLoop::setStopThread(bool st) {
    _task_exec_mtx.lock();
    _stop_thread = st;
    _task_exec_mtx.unlock();
    _empty_mtx.unlock();
    if (st) {
        ChirpLogger::instance(_service_name) << "Main stopping thread." << std::endl;
    }
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

void MessageLoop::stop() {
    setStopThread(true);
    _empty_mtx.unlock();
}