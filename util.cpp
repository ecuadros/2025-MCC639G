#include "util.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <mutex>

using namespace std;
using namespace std::chrono;

mutex logMutex;

string getCurrentTimestamp() {
    auto now = system_clock::now();
    auto time_t_now = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    
    stringstream ss;
    ss << put_time(localtime(&time_t_now), "%H:%M:%S");
    ss << "." << setfill('0') << setw(3) << ms.count();
    return ss.str();
}

void logMessage(const string& message) {
    lock_guard<mutex> lock(logMutex);
    cout << message << endl;
}