#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

#include "nice_logger.h"

NiceLogger& NiceLogger::instance(const std::string& serviceName) {

    static NiceLogger loggerInstance;
    if (!serviceName.empty()) {
        loggerInstance.setServiceName(serviceName);
    }
    return loggerInstance;
}

NiceLogger::NiceLogger(const std::string& filename) {
    
    // Open in write-only mode, truncating the file each time
    _ofs.open(filename, std::ios::out | std::ios::trunc);
    if (!_ofs.is_open()) {

        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
    _serviceName = "";
}

NiceLogger::~NiceLogger() {

    flush();
    if (_ofs.is_open()) {

        _ofs.close();
    }
}

void NiceLogger::setServiceName(const std::string& serviceName) {
    
    _serviceName = serviceName;
}

NiceLogger& NiceLogger::operator<<(std::ostream& (*manip)(std::ostream&)) {

    std::lock_guard<std::mutex> lock(_mtx);
    if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {

        flush();
    }
    return *this;
}

void NiceLogger::flush() {

    if (_ofs.is_open()) {

        // Get current time with milliseconds
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        _ofs << '[' << std::put_time(&tm, "%Y-%m-%d") << "]";
        _ofs << '[' << std::put_time(&tm, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count() << "] ";
        if (!_serviceName.empty()) {
            _ofs << '[' << _serviceName << "] ";
        } else {
            _ofs << " ";
        }
        _ofs << _buffer.str() << std::endl;
        _ofs.flush();
        _buffer.str("");
        _buffer.clear();
    }
} 