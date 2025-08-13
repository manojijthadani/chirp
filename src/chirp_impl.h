#pragma once
#include "chirp_error.h"

class ChirpImpl {
public:
    ChirpImpl() = default;
    ~ChirpImpl() = default;

    explicit ChirpImpl(const std::string& service_name, ChirpError::Error& error);
    void start();
    void shutdown();
    std::string getServiceName();
    ChirpError::Error enqueMsg(std::string& msgName, std::vector<std::any>& args);
    ChirpError::Error enqueSyncMsg(std::string& msgName, std::vector<std::any>& args);
    void getCbMap(std::map<std::string, std::function<ChirpError::Error(std::vector<std::any>)>>*& funcMap);

private:
    void waitUntilServiceStopped();
    std::string _service_name;
    ChirpThread* _nthread;
}; 