#ifndef __TPK_EXCEPTION_H__
#define __TPK_EXCEPTION_H__

#include <iostream>
#include <execinfo.h>
#include <unistd.h>
#include <exception>



namespace tpk {

using namespace std;

class Exception: public exception {
  public:
    Exception() {
      this->message = "";
      storeStackTrace();
    };
    Exception(const char *message) {
      this->message = message;
      storeStackTrace();
    };
    virtual void storeStackTrace(void) {
      size = backtrace(stack, MAX_STACKSIZE);
    };
    virtual void printStackTrace(void) {
      cerr << "Exception: " << message << endl;
      // Exclude top 1 stack entry (storeStackTrace)
      backtrace_symbols_fd(stack+1, size-1, STDERR_FILENO);
    };

  protected:
    const char* message;

  private:
    static const size_t MAX_STACKSIZE = 100;
    size_t size;
    void *stack[MAX_STACKSIZE];
};

}  // ns: tpk
#endif // __TPK_EXCEPTION_H__
