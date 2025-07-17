#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>

class NiceLogger {
public:
    static NiceLogger& instance(const std::string& serviceName = "default_service");

    template<typename T>
    NiceLogger& operator<<(const T& value) {
        std::lock_guard<std::mutex> lock(_mtx);
        _buffer << value;
        return *this;
    }

    // Overload for std::endl
    NiceLogger& operator<<(std::ostream& (*manip)(std::ostream&));

    void setServiceName(const std::string& serviceName);

private:
    NiceLogger(const std::string& filename = "nice_log.txt");
    ~NiceLogger();
    NiceLogger(const NiceLogger&) = delete;
    NiceLogger& operator=(const NiceLogger&) = delete;

    void flush();
    std::ofstream _ofs;
    std::ostringstream _buffer;
    std::mutex _mtx;
    std::string _serviceName;
}; 