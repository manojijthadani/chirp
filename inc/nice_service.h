#pragma once
#include <functional>
#include <map>
#include <any>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

enum class ShutdownType {
    NORMAL,
    FORCE
};

class NiceServiceImpl;

class NiceService {
public:
    NiceService() = default;
    ~NiceService() = default;

    explicit NiceService(const std::string& service_name);
    void start();
    void shutdown(ShutdownType s);
    std::string getServiceName();

    template<typename Ret, typename... Args>
    void registerMsgHandler(std::string msgName, Ret(*func)(Args...)) {
        std::map<std::string, std::function<void(std::vector<std::any>)>>* functions = nullptr;
        getCbMap(functions);
        (*functions)[msgName] = [func](std::vector<std::any> args) {
            if (args.size() < sizeof...(Args) + 1) {
                std::cerr << "Argument count mismatch (expected at least one extra).\n";
                return;
            }
            std::vector<std::any> slicedArgs(args.begin() + 1, args.end());
            try {
                helper(func, slicedArgs, std::index_sequence_for<Args...>{});
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Argument type mismatch: " << e.what() << '\n';
            }
        };
    }

    template<typename T, typename... Args>
    void postMsg(T first_arg, Args... remaining_args) {
        std::ostringstream oss;
        oss << first_arg;
        std::string first_arg_str = oss.str();
        std::vector<std::any> args;
        args.push_back(first_arg);
        collectArgs(args, remaining_args...);
        enqueMsg(first_arg_str, args);
    }

private:
    template<typename T>
    void collectArgs(std::vector<std::any>& args, T arg) {
        args.push_back(arg);
    }

    template<typename T, typename... Args>
    void collectArgs(std::vector<std::any>& args, T first, Args... rest) {
        args.push_back(first);
        collectArgs(args, rest...);
    }

    template<typename Ret, typename... Args, size_t... I>
    static void helper(Ret(*func)(Args...), const std::vector<std::any>& args, std::index_sequence<I...>) {
        try {
            func(std::any_cast<Args>(args[I])...);
        } catch (const std::bad_any_cast& e) {
            std::cerr << "Argument type mismatch: " << e.what() << "\n";
        }
    }
    void enqueMsg(std::string& msgName, std::vector<std::any>& args);
    void getCbMap(std::map<std::string, std::function<void(std::vector<std::any>)>>*& funcMap);
    NiceServiceImpl* _impl;
}; 