#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>

#include "nice_service.h"
#include "nice_logger.h"


class ServiceMsgHandlers {
    public:
    ServiceMsgHandlers() = default;
    ~ServiceMsgHandlers() = default;

    static void TestIntegerTypesHandler(int a, short b, long c, long long d);
    static void TestFloatingTypesHandler(float a, double b, long double c);
    static void TestStringTypesHandler(std::string a);
    static void TestBoolTypesHandler(bool a);
    static void TestCharTypesHandler(char a);
    static void TestVoidTypesHandler();
    static void TestArrayTypesHandler(int a[3]);
    static void TestPointerTypesHandler(int* a);
    static void TestReferenceTypesHandler(int& a);
    static void TestVectorTypesHandler(std::vector<int> a);
    static void TestMapTypesHandler(std::map<std::string, int> a);
    static void TestSetTypesHandler(std::set<int> a);
    static void TestListTypesHandler(std::list<int> a);
    static void TestDequeTypesHandler(std::deque<int> a);
};

void ServiceMsgHandlers::TestIntegerTypesHandler(int a, short b, long c, long long d) {

    NiceLogger::instance("Service1") << "****In TestIntegerTypesHandler callback " 
                                     << a << " " 
                                     << b << " " 
                                     << c << " " 
                                     << d  << std::endl;
}

void ServiceMsgHandlers::TestFloatingTypesHandler(float a, double b, long double c) {

    NiceLogger::instance("Service2") << "****In TestFloatingTypesHandler callback " 
                                     << a  << " " 
                                     << b << " " 
                                     << c << std::endl;
}

void ServiceMsgHandlers::TestStringTypesHandler(std::string a) {

    NiceLogger::instance("Service1") << "****In TestStringTypesHandler callback " 
                                     << a << std::endl;
}

void ServiceMsgHandlers::TestBoolTypesHandler(bool a) {

    NiceLogger::instance("Service1") << "****In TestBoolTypesHandler callback " 
                                     << std::boolalpha << a << std::endl;
}

void ServiceMsgHandlers::TestCharTypesHandler(char a) {

    NiceLogger::instance("Service1") << "****In TestCharTypesHandler callback " 
                                     << a << std::endl;
}

int main() {

    ServiceMsgHandlers mh;

    NiceService svc1("Service1");
    svc1.registerMsgHandler("TestIntegerTypes", mh.TestIntegerTypesHandler);
    svc1.registerMsgHandler("TestStringTypes", mh.TestStringTypesHandler);
    svc1.registerMsgHandler("TestCharTypes", mh.TestCharTypesHandler);
    svc1.start();

    NiceService svc2("Service2");
    svc2.registerMsgHandler("TestFloatingTypes", mh.TestFloatingTypesHandler);
    svc2.registerMsgHandler("TestBoolTypes", mh.TestBoolTypesHandler);
    svc2.start();

    svc1.postMsg("TestIntegerTypes", 2, (short)100, (long)1000, (long long)10000);
    svc2.postMsg("TestFloatingTypes", (float)3.14, (double)2.718, (long double)1.618);
    svc1.postMsg("TestStringTypes", std::string("Hello, World!"));
    svc2.postMsg("TestBoolTypes", true);
    svc1.postMsg("TestCharTypes", 'a');
    
    std::this_thread::sleep_for(std::chrono::seconds(1));

    svc1.shutdown(ShutdownType::NORMAL);
    svc2.shutdown(ShutdownType::NORMAL);

    return 0;
}
