#pragma once

class ChirpImpl {
public:
    ChirpImpl() = default;
    ~ChirpImpl() = default;

    explicit ChirpImpl(const std::string& service_name);
    void start();
    void shutdown();
    std::string getServiceName();
    void enqueMsg(std::string& msgName, std::vector<std::any>& args);
    void enqueSyncMsg(std::string& msgName, std::vector<std::any>& args);
    void getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap);

private:
    void waitUntilServiceStopped();
    std::string _service_name;
    ChirpThread* _nthread;
}; 