#ifndef __Exceptions_H__
#define __Exceptions_H__

#include <iostream>
#include <exception>
#include <execinfo.h>
#include <unistd.h>

using namespace std;

const size_t MAX_STACKSIZE = 100;

namespace tpk {

class Exception: public exception {
  public:
    Exception(const char *message) {
      this->message = message;
      storeStackTrace();
    };
    void storeStackTrace(void) {
      size = backtrace(stack, MAX_STACKSIZE);
    };
    void printStackTrace(void) {
      cerr << "Exception: " << message << endl;
      // Exclude top 1 stack entry (storeStackTrace)
      backtrace_symbols_fd(stack+1, size-1, STDERR_FILENO);
    };

  private:
    const char* message;
    size_t size;
    void *stack[MAX_STACKSIZE];
};

}  // ns: tpk
#endif // __Exceptions_H__
