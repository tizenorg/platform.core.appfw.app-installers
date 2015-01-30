/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_EXCEPTION_H_
#define TPK_EXCEPTION_H_

#include <execinfo.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "utils/logging.h"

namespace tpk {

class Exception: public std::exception {
SCOPE_LOG_TAG(UncaughtException)

 public:
  Exception() {
    this->message = "";
    storeStackTrace();
  }
  explicit Exception(const char *message) {
    this->message = message;
    storeStackTrace();
  }
  virtual void storeStackTrace() {
    size = backtrace(stack, MAX_STACKSIZE);
  }
  virtual void printStackTrace() {
    LOG(ERROR) << message;
    // Exclude top 1 stack entry (storeStackTrace)
    backtrace_symbols_fd(stack+1, size-1, STDERR_FILENO);
  }


 protected:
  const char* message;


 private:
  static const size_t MAX_STACKSIZE = 100;
  size_t size;
  void *stack[MAX_STACKSIZE];
};

}  // namespace tpk
#endif  // TPK_EXCEPTION_H_
