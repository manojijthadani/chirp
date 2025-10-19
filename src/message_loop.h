//
// Created by manoj ij thadani on 7/9/25.
//
#pragma once
#include <deque>
#include <functional>
#include <map>
#include <any>
#include <vector>
#include <mutex>

#include "message.h"
#include "chirp_error.h"
#include "timer_mgr.h"
#include "chirp_timer.h"

class MessageLoop {

public:
    /**
     * @brief Enumeration for specifying enqueue position
     */
    enum class EnqueuePosition {
        ENQUEUE_FRONT,  /**< Add message to the front of the queue */
        ENQUEUE_BACK    /**< Add message to the back of the queue */
    };

    MessageLoop() = default;
    ~MessageLoop() = default;

    void spin();
    void enqueue(Message* m);
    void enqueueSync(Message* m);
    void setServiceName(const std::string& service_name);
    void getCbMap(std::map<std::string, 
                  std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap);

    void stop();
    void drainQueue();
    void addChirpTimer(ChirpTimer* timer);
    void removeChirpTimer(ChirpTimer* timer);

private:

    void setStopThread(bool st);
    void enqueueInternal(Message* m, Message::MessageType type, EnqueuePosition position = EnqueuePosition::ENQUEUE_BACK);
    void fireTimerHandlers(bool& st_thread);
    void fireRegularHandlers(bool& st_thread);
    
    std::deque<Message*> _message_queue;
    std::string _service_name;
    std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>> _functions;
    std::timed_mutex _empty_mtx;
    std::mutex _task_exec_mtx;
    volatile bool _stop_thread;
    TimerManager _timer_mgr;
};
