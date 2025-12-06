#include <mutex>
std::mutex m;
void test(){ std::scoped_lock lock(m); }
