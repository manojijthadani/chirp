#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <mutex>
#include <sstream>

#include "ichirp.h"
#include "chirp_error.h"

// Thread-safe print utility for this test module
namespace {
    std::mutex g_print_mutex;
}

inline void threadSafePrint(const std::string& text) {
    std::lock_guard<std::mutex> lock(g_print_mutex);
    std::cout << text << std::endl;
}

// Macros to simplify repetitive error checks
#define CHECK_ERR_OR_RETURN(MSG, ERRVAR) \
    do { \
        if ((ERRVAR) != ChirpError::SUCCESS) { \
            std::ostringstream _oss; \
            _oss << (MSG) << ": " << ChirpError::errorToString((ERRVAR)); \
            threadSafePrint(_oss.str()); \
            return 1; \
        } \
    } while (0)

class ServiceMsgHandlers {
public:
    ServiceMsgHandlers() = default;
    ~ServiceMsgHandlers() = default;

    void setServiceName(const std::string& name) { _svcName = name; }
    void incrementPost() { ++_postCount; }
    void incrementSync() { ++_syncCount; }
    int getPostCount() const { return _postCount; }
    int getSyncCount() const { return _syncCount; }
    int getAckCount() const { return _ackCount; }

    void TestIntegerTypesHandler(int a, short b, long c, long long d);
    void TestFloatingTypesHandler(float a, double b, long double c);
    void TestStringTypesHandler(std::string a);
    void TestBoolTypesHandler(bool a);
    void TestCharTypesHandler(char a);
    void TestVoidTypesHandler();
    void TestPointerTypesHandler(int* a);
    void TestVectorTypesHandler(std::vector<int> a);

private:
    std::string _svcName;
    int _postCount = 0;
    int _syncCount = 0;
    int _ackCount = 0;
};

void ServiceMsgHandlers::TestIntegerTypesHandler(int a, short b, long c, long long d) {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestIntegerTypesHandler "
              << a << " " << b << " " << c << " " << d;
        threadSafePrint(oss.str());
    }
}

void ServiceMsgHandlers::TestFloatingTypesHandler(float a, double b, long double c) {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestFloatingTypesHandler "
              << a << " " << b << " " << c;
        threadSafePrint(oss.str());
    }
}

void ServiceMsgHandlers::TestStringTypesHandler(std::string a) {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestStringTypesHandler " << a;
        threadSafePrint(oss.str());
    }
}

void ServiceMsgHandlers::TestBoolTypesHandler(bool a) {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestBoolTypesHandler "
              << std::boolalpha << a;
        threadSafePrint(oss.str());
    }
}

void ServiceMsgHandlers::TestCharTypesHandler(char a) {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestCharTypesHandler " << a;
        threadSafePrint(oss.str());
    }
}

void ServiceMsgHandlers::TestVectorTypesHandler(std::vector<int> a) {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestVectorTypesHandler vector: ";
        for (const auto& val : a) { oss << val << " "; }
        threadSafePrint(oss.str());
    }
}

void ServiceMsgHandlers::TestPointerTypesHandler(int* a) {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestPointerTypesHandler value: " << *a;
        threadSafePrint(oss.str());
    }
}

void ServiceMsgHandlers::TestVoidTypesHandler() {
    ++_ackCount;
    {
        std::ostringstream oss; oss << "[" << _svcName << "] In TestVoidTypesHandler ";
        threadSafePrint(oss.str());
    }
}

int main() {

    std::ostringstream oss; oss << "====Simple Test Application : Chirp API version=" 
                                << IChirp::getVersion() << "====";
    threadSafePrint(oss.str());

    ServiceMsgHandlers mh1;
    ServiceMsgHandlers mh2;

    ChirpError::Error error;
    IChirp svc1("Service1", error);
    CHECK_ERR_OR_RETURN("Failed to create Service1", error);
    mh1.setServiceName("Service1");
    error = svc1.registerMsgHandler("TestIntegerTypes", &mh1, &ServiceMsgHandlers::TestIntegerTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestIntegerTypes handler", error);
    
    error = svc1.registerMsgHandler("TestStringTypes", &mh1, &ServiceMsgHandlers::TestStringTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestStringTypes handler", error);
    
    error = svc1.registerMsgHandler("TestCharTypes", &mh1, &ServiceMsgHandlers::TestCharTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestCharTypes handler", error);
    
    error = svc1.registerMsgHandler("TestPointerTypes", &mh1, &ServiceMsgHandlers::TestPointerTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestPointerTypes handler", error);
    svc1.start();

    IChirp svc2("Service2", error);
    CHECK_ERR_OR_RETURN("Failed to create Service2", error);
    mh2.setServiceName("Service2");
    error = svc2.registerMsgHandler("TestFloatingTypes", &mh2, &ServiceMsgHandlers::TestFloatingTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestFloatingTypes handler", error);
    
    error = svc2.registerMsgHandler("TestBoolTypes", &mh2, &ServiceMsgHandlers::TestBoolTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestBoolTypes handler", error);
    
    error = svc2.registerMsgHandler("TestVectorTypes", &mh2, &ServiceMsgHandlers::TestVectorTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestVectorTypes handler", error);
    
    error = svc2.registerMsgHandler("TestVoidTypes", &mh2, &ServiceMsgHandlers::TestVoidTypesHandler);
    CHECK_ERR_OR_RETURN("Failed to register TestVoidTypes handler", error);
    svc2.start();

    error = svc1.postMsg("TestIntegerTypes", 2, (short)100, (long)1000, (long long)10000);
    if (error == ChirpError::SUCCESS) mh1.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestIntegerTypes", error);
    
    error = svc1.postMsg("TestStringTypes", std::string("Hello, World!"));
    if (error == ChirpError::SUCCESS) mh1.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestStringTypes", error);
    
    error = svc1.postMsg("TestCharTypes", 'a');
    if (error == ChirpError::SUCCESS) mh1.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestCharTypes", error);

    int a = 10;
    error = svc1.postMsg("TestPointerTypes", &a);
    if (error == ChirpError::SUCCESS) mh1.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestPointerTypes", error);

    std::vector<int> vec = {1, 2, 3, 4, 5};
    error = svc2.postMsg("TestVectorTypes", vec);
    if (error == ChirpError::SUCCESS) mh2.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestVectorTypes", error);
    
    error = svc2.postMsg("TestVoidTypes");
    if (error == ChirpError::SUCCESS) mh2.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestVoidTypes", error);

    error = svc2.postMsg("TestFloatingTypes", (float)3.14, (double)2.718, (long double)1.618);
    if (error == ChirpError::SUCCESS) mh2.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestFloatingTypes", error);

    error = svc2.postMsg("TestBoolTypes", true);
    if (error == ChirpError::SUCCESS) mh2.incrementPost();
    CHECK_ERR_OR_RETURN("Failed to post TestBoolTypes", error);

    threadSafePrint("[Main] Syncing TestVoidTypes on Service 2");
    error = svc2.syncMsg("TestVoidTypes");
    if (error == ChirpError::SUCCESS) mh2.incrementSync();
    CHECK_ERR_OR_RETURN("Failed to sync TestVoidTypes", error);
    threadSafePrint("[Main] Synced TestVoidTypes on Service 2");

    // Now lets try some negative testing.
    // This should fail. Invalid number of arguments.
    error = svc1.syncMsg("TestIntegerTypes", 2);
    if (error != ChirpError::SUCCESS) {
        std::ostringstream oss; oss << "[Main] Failed to sync TestIntegerTypes (negative test): "
                                    << ChirpError::errorToString(error);
        threadSafePrint(oss.str());
    }

    // This should fail. Invalid type of arguments.
    error = svc1.syncMsg("TestIntegerTypes", std::string("Negative test"));
    if (error != ChirpError::SUCCESS) {
        std::ostringstream oss; oss << "[Main] Failed to sync TestIntegerTypes (negative test): "
                                    << ChirpError::errorToString(error);
        threadSafePrint(oss.str());
    }

    // This should fail. Invalid order of arguments & number of arguments.
    error = svc1.syncMsg("TestIntegerTypes", (short)100, 2, (long)1000, (long long)10000);
    if (error != ChirpError::SUCCESS) {
        std::ostringstream oss; oss << "[Main] Failed to sync TestIntegerTypes (negative test): "
                                    << ChirpError::errorToString(error);
        threadSafePrint(oss.str());
    }

    // This should fail. Invalid number of arguments.
    error = svc1.postMsg("TestIntegerTypes", (int)2);
    if (error != ChirpError::SUCCESS) {
        std::ostringstream oss; oss << "[Main] Failed to post TestIntegerTypes (negative test): "
                                    << ChirpError::errorToString(error);
        threadSafePrint(oss.str());
    }
    
    // This should fail. Invalid type of arguments.
    error = svc1.postMsg("TestIntegerTypes", std::string("Negative test"));
    if (error != ChirpError::SUCCESS) {
        std::ostringstream oss; oss << "[Main] Failed to post TestIntegerTypes (negative test): "
                                    << ChirpError::errorToString(error);
        threadSafePrint(oss.str());
    }
    
    // Wait for 3 seconds to allow the services to process the messages before
    // shutting down the application.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Validate counts
    int svc1Expected = mh1.getPostCount() + mh1.getSyncCount();
    int svc2Expected = mh2.getPostCount() + mh2.getSyncCount();
    int svc1Ack = mh1.getAckCount();
    int svc2Ack = mh2.getAckCount();

    bool svc1Ok = (svc1Expected == svc1Ack);
    bool svc2Ok = (svc2Expected == svc2Ack);

    std::ostringstream oss1; oss1 << "[Service1] Posts+Syncs=" << svc1Expected << ", Acks=" << svc1Ack
                                  << " => " << (svc1Ok ? "OK" : "MISMATCH");
    threadSafePrint(oss1.str());
    std::ostringstream oss2; oss2 << "[Service2] Posts+Syncs=" << svc2Expected << ", Acks=" << svc2Ack
                                  << " => " << (svc2Ok ? "OK" : "MISMATCH");
    threadSafePrint(oss2.str());

    svc1.shutdown();
    svc2.shutdown();

    if (!(svc1Ok && svc2Ok)) {
        return 2;
    }

    return 0;
}
