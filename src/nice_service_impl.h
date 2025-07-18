#pragma once

class NiceServiceImpl {
public:
    NiceServiceImpl() = default;
    ~NiceServiceImpl() = default;

    explicit NiceServiceImpl(const std::string& service_name);
    void start();
    void shutdown(ShutdownType s);
    std::string getServiceName();
    void enqueMsg(std::string& msgName, std::vector<std::any>& args);
    void getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap);

private:
    void waitUntilServiceStopped();
    std::string _service_name;
    NiceThread* _nthread;
}; 