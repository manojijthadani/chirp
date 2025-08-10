#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>

#include "chirp.h"

class ServiceMsgHandlers {
public:
    ServiceMsgHandlers() = default;
    ~ServiceMsgHandlers() = default;

    void TestIntegerTypesHandler(int a, short b, long c, long long d);
    void TestFloatingTypesHandler(float a, double b, long double c);
    void TestStringTypesHandler(std::string a);
    void TestBoolTypesHandler(bool a);
    void TestCharTypesHandler(char a);
    void TestVoidTypesHandler();
    void TestPointerTypesHandler(int* a);
    void TestVectorTypesHandler(std::vector<int> a);
};

void ServiceMsgHandlers::TestIntegerTypesHandler(int a, short b, long c, long long d) {
    std::cout << "In TestIntegerTypesHandler callback "
              << a << " " << b << " " << c << " " << d << std::endl;
}

void ServiceMsgHandlers::TestFloatingTypesHandler(float a, double b, long double c) {
    std::cout << "In TestFloatingTypesHandler callback "
              << a << " " << b << " " << c << std::endl;
}

void ServiceMsgHandlers::TestStringTypesHandler(std::string a) {
    std::cout << "In TestStringTypesHandler callback " << a << std::endl;
}

void ServiceMsgHandlers::TestBoolTypesHandler(bool a) {
    std::cout << "In TestBoolTypesHandler callback "
              << std::boolalpha << a << std::endl;
}

void ServiceMsgHandlers::TestCharTypesHandler(char a) {
    std::cout << "In TestCharTypesHandler callback " << a << std::endl;
}

void ServiceMsgHandlers::TestVectorTypesHandler(std::vector<int> a) {
    std::cout << "In TestVectorTypesHandler callback vector: ";
    for (const auto& val : a) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

void ServiceMsgHandlers::TestPointerTypesHandler(int* a) {
    std::cout << "In TestPointerTypesHandler callback value: " << *a << std::endl;
}

void ServiceMsgHandlers::TestVoidTypesHandler() {
    std::cout << "In TestVoidTypesHandler callback" << std::endl;
}

int main() {
    std::cout << "Chirp API version: " << Chirp::getVersion() << std::endl;

    ServiceMsgHandlers mh;

    Chirp svc1("Service1");
    svc1.registerMsgHandler("TestIntegerTypes", &mh, &ServiceMsgHandlers::TestIntegerTypesHandler);
    svc1.registerMsgHandler("TestStringTypes", &mh, &ServiceMsgHandlers::TestStringTypesHandler);
    svc1.registerMsgHandler("TestCharTypes", &mh, &ServiceMsgHandlers::TestCharTypesHandler);
    svc1.registerMsgHandler("TestPointerTypes", &mh, &ServiceMsgHandlers::TestPointerTypesHandler);
    svc1.start();

    Chirp svc2("Service2");
    svc2.registerMsgHandler("TestFloatingTypes", &mh, &ServiceMsgHandlers::TestFloatingTypesHandler);
    svc2.registerMsgHandler("TestBoolTypes", &mh, &ServiceMsgHandlers::TestBoolTypesHandler);
    svc2.registerMsgHandler("TestVectorTypes", &mh, &ServiceMsgHandlers::TestVectorTypesHandler);
    svc2.registerMsgHandler("TestVoidTypes", &mh, &ServiceMsgHandlers::TestVoidTypesHandler);
    svc2.start();

    svc1.postMsg("TestIntegerTypes", 2, (short)100, (long)1000, (long long)10000);
    svc2.postMsg("TestFloatingTypes", (float)3.14, (double)2.718, (long double)1.618);
    svc1.postMsg("TestStringTypes", std::string("Hello, World!"));
    svc2.postMsg("TestBoolTypes", true);
    svc1.postMsg("TestCharTypes", 'a');

    int a = 10;
    svc1.postMsg("TestPointerTypes", &a);

    std::vector<int> vec = {1, 2, 3, 4, 5};
    svc2.postMsg("TestVectorTypes", vec);
    svc2.postMsg("TestVoidTypes");
    std::cout << "Syncing TestVoidTypes" << std::endl;
    svc2.syncMsg("TestVoidTypes");
    std::cout << "Synced TestVoidTypes" << std::endl;

    // Now lets try some negative testing.
    // This should fail. Invalid number of arguments.
    svc1.postMsg("TestIntegerTypes", 2);
    // This should fail. Invalid type of arguments.
    svc1.postMsg("TestIntegerTypes", std::string("Negative test"));    
    // This should fail. Invalid order of arguments.
    svc1.postMsg("TestIntegerTypes", (short)100, 2, (long)1000, (long long)10000);
    
    // Wait for 3 seconds to allow the services to process the messages before
    // shutting down the application.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    svc1.shutdown();
    svc2.shutdown();

    return 0;
}
