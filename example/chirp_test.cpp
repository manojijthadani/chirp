#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>

#include "chirp.h"
#include "chirp_error.h"

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

    ChirpError::Error error;
    Chirp svc1("Service1", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create Service1: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    error = svc1.registerMsgHandler("TestIntegerTypes", &mh, &ServiceMsgHandlers::TestIntegerTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestIntegerTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = svc1.registerMsgHandler("TestStringTypes", &mh, &ServiceMsgHandlers::TestStringTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestStringTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = svc1.registerMsgHandler("TestCharTypes", &mh, &ServiceMsgHandlers::TestCharTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestCharTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = svc1.registerMsgHandler("TestPointerTypes", &mh, &ServiceMsgHandlers::TestPointerTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestPointerTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    svc1.start();

    Chirp svc2("Service2", error);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to create Service2: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    error = svc2.registerMsgHandler("TestFloatingTypes", &mh, &ServiceMsgHandlers::TestFloatingTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestFloatingTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = svc2.registerMsgHandler("TestBoolTypes", &mh, &ServiceMsgHandlers::TestBoolTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestBoolTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = svc2.registerMsgHandler("TestVectorTypes", &mh, &ServiceMsgHandlers::TestVectorTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestVectorTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    
    error = svc2.registerMsgHandler("TestVoidTypes", &mh, &ServiceMsgHandlers::TestVoidTypesHandler);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to register TestVoidTypes handler: " << ChirpError::errorToString(error) << std::endl;
        return 1;
    }
    svc2.start();

    error = svc1.postMsg("TestIntegerTypes", 2, (short)100, (long)1000, (long long)10000);
            if (error != ChirpError::SUCCESS) {
            std::cout << "Failed to post TestIntegerTypes: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = svc2.postMsg("TestFloatingTypes", (float)3.14, (double)2.718, (long double)1.618);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestFloatingTypes: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = svc1.postMsg("TestStringTypes", std::string("Hello, World!"));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestStringTypes: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = svc2.postMsg("TestBoolTypes", true);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestBoolTypes: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = svc1.postMsg("TestCharTypes", 'a');
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestCharTypes: " << ChirpError::errorToString(error) << std::endl;
    }

    int a = 10;
    error = svc1.postMsg("TestPointerTypes", &a);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestPointerTypes: " << ChirpError::errorToString(error) << std::endl;
    }

    std::vector<int> vec = {1, 2, 3, 4, 5};
    error = svc2.postMsg("TestVectorTypes", vec);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestVectorTypes: " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = svc2.postMsg("TestVoidTypes");
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestVoidTypes: " << ChirpError::errorToString(error) << std::endl;
    }
    std::cout << "Syncing TestVoidTypes" << std::endl;
    error = svc2.syncMsg("TestVoidTypes");
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to sync TestVoidTypes: " << ChirpError::errorToString(error) << std::endl;
    }
    std::cout << "Synced TestVoidTypes" << std::endl;

    // Now lets try some negative testing.
    // This should fail. Invalid number of arguments.
    error = svc1.syncMsg("TestIntegerTypes", 2);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to sync TestIntegerTypes (negative test): " << ChirpError::errorToString(error) << std::endl;
    }
    // This should fail. Invalid type of arguments.
    error = svc1.syncMsg("TestIntegerTypes", std::string("Negative test"));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to sync TestIntegerTypes (negative test): " << ChirpError::errorToString(error) << std::endl;
    }
    // This should fail. Invalid order of arguments.
    error = svc1.syncMsg("TestIntegerTypes", (short)100, 2, (long)1000, (long long)10000);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to sync TestIntegerTypes (negative test): " << ChirpError::errorToString(error) << std::endl;
    }
    
    error = svc1.postMsg("TestIntegerTypes", 2);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestIntegerTypes (negative test): " << ChirpError::errorToString(error) << std::endl;
    }
    // This should fail. Invalid type of arguments.
    error = svc1.postMsg("TestIntegerTypes", std::string("Negative test"));
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestIntegerTypes (negative test): " << ChirpError::errorToString(error) << std::endl;
    }
    // This should fail. Invalid order of arguments.
    error = svc1.postMsg("TestIntegerTypes", (short)100, 2, (long)1000, (long long)10000);
    if (error != ChirpError::SUCCESS) {
        std::cout << "Failed to post TestIntegerTypes (negative test): " << ChirpError::errorToString(error) << std::endl;
    }
    // Wait for 3 seconds to allow the services to process the messages before
    // shutting down the application.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    svc1.shutdown();
    svc2.shutdown();

    return 0;
}
