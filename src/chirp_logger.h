#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <cstdlib>

class ChirpLogger {
public:
    static ChirpLogger& instance(const std::string& serviceName = "default_service");

    template<typename T>
    ChirpLogger& operator<<(const T& value) {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_ofs.is_open()) {
            _buffer << value;
        }
        return *this;
    }

    // Overload for std::endl
    ChirpLogger& operator<<(std::ostream& (*manip)(std::ostream&));

    void setServiceName(const std::string& serviceName);

private:
    ChirpLogger(const std::string& filename = "nice_log.txt");
    ~ChirpLogger();
    ChirpLogger(const ChirpLogger&) = delete;
    ChirpLogger& operator=(const ChirpLogger&) = delete;

    void flush();
    std::ofstream _ofs;
    std::ostringstream _buffer;
    std::mutex _mtx;
    std::string _serviceName;
}; 