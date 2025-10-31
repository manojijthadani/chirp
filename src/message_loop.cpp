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
#include "chirp_timer.h"

void MessageLoop::spin() {

    bool lockAcquired = false;
    bool st_thread = false;
    setStopThread(st_thread);

    _empty_mtx.lock();
    while (!st_thread) {

      if (_message_queue.empty()) {

          ChirpLogger::instance(_service_name) << "waiting. MsgQ empty." << std::endl;
          // Get duration to next timer event
          std::chrono::milliseconds duration = _timer_mgr.getDurationToNextTimerEvent();
            
          if (duration.count() == 0) {

              // No timers or timer already elapsed, just wait on mutex
              _empty_mtx.lock();
              lockAcquired = true;

          } else {

          // Wait on mutex with timeout
          lockAcquired = _empty_mtx.try_lock_for(duration);
                
          // Timeout occurred, timers have elapsed
          if (!lockAcquired) 
              fireTimerHandlers(st_thread);
        }
      } else {
          // Queue is not empty, we should process messages
          lockAcquired = true;
      }

      if (lockAcquired) {
          lockAcquired = false;
          fireRegularHandlers(st_thread);
      }
    }
  ChirpLogger::instance(_service_name) << "Spin loop stopped." << std::endl;
}

void MessageLoop::enqueue(Message* m) {
    enqueueInternal(m, Message::MessageType::ASYNC);
}

void MessageLoop::enqueueSync(Message* m) {
    enqueueInternal(m, Message::MessageType::SYNC);
}

void MessageLoop::enqueueInternal(Message* m, Message::MessageType type, EnqueuePosition position) {
    
    if (!_stop_thread) {

        std::string msg;
        m->getMessage(msg);
        ChirpLogger::instance(_service_name) << "Enqueing message " << msg << std::endl;
        
        if (position == EnqueuePosition::ENQUEUE_FRONT) {
            _message_queue.push_front(m);
        } else {
            _message_queue.push_back(m);
        }
        
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

void MessageLoop::getCbMap(std::map<std::string, 
                           std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap) {
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
        _message_queue.pop_front();
    }
    _task_exec_mtx.unlock();
}

void MessageLoop::stop() {
    
    setStopThread(true);
    _empty_mtx.unlock();
}

void MessageLoop::fireTimerHandlers(bool& st_thread) {

    // Timeout occurred, timers have elapsed
    std::vector<ChirpTimer*> elapsedTimers;
    _timer_mgr.getElapsedTimers(elapsedTimers);
    
    // Process elapsed timers
    _task_exec_mtx.lock();
    for (ChirpTimer* timer : elapsedTimers) {
        if (timer) {
            std::string timerMsg = timer->getMessage();
            
            // Call the handler for this timer
            auto it = _functions.find(timerMsg);
            if (it != _functions.end()) {
                // Args vector must include the message name as first element,
                // followed by the actual arguments (timer message name)
                std::vector<std::any> args;
                args.push_back(timerMsg);  // Message name (required by handler framework)
                args.push_back(timerMsg);  // Actual argument: the timer message
                it->second(args);
            }
        }
    }
    
    // Update st_thread before unlocking
    st_thread = _stop_thread;
    _task_exec_mtx.unlock();

    // Reschedule only the timers that just fired
    _timer_mgr.rescheduleTimers(elapsedTimers);
    
    // Recompute which timer fires next
    _timer_mgr.computeNextTimerFirringTime();
}

void MessageLoop::fireRegularHandlers(bool& st_thread) {
    
    _task_exec_mtx.lock();
    if (!_message_queue.empty()) {

        Message* m = _message_queue.front();
        _message_queue.pop_front();
        std::string msg;
        std::vector<std::any> args;
        m->getMessage(msg);
        m->getArgs(args);
        auto it = _functions.find(msg);
        if (it != _functions.end()) {
            it->second(args);
        }
        Message::MessageType mt;
        m->getMessageType(mt);
        if (mt == Message::MessageType::SYNC) {
            m->sync_notify();
        }
        delete m;
    }
    
    // Update st_thread before unlocking
    st_thread = _stop_thread;
    _task_exec_mtx.unlock();
}

void MessageLoop::addChirpTimer(ChirpTimer* timer) {

    _timer_mgr.addTimer(timer);
    // Always compute firing times and wake up the loop when a timer is added
    // This ensures the timer schedule is updated immediately
    _timer_mgr.computeNextTimerFirringTime();
    // Wake up the message loop so it can recalculate the wait duration
    _empty_mtx.unlock();
}

void MessageLoop::removeChirpTimer(ChirpTimer* timer) {

    _timer_mgr.removeTimer(timer);
    // Recompute schedule after removing a timer
    _timer_mgr.computeNextTimerFirringTime();
    // Wake up the message loop so it can recalculate the wait duration
    _empty_mtx.unlock();
}