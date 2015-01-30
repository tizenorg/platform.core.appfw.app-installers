/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_EXCEPTION_H_
#define TPK_EXCEPTION_H_

#include <execinfo.h>
#include <string.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "utils/logging.h"

namespace tpk {

class Exception: public std::exception {
SCOPE_LOG_TAG(UncaughtException)

 public:
  Exception() {
    this->message_ = nullptr;
    StoreStackTrace();
  }
  explicit Exception(const char *message) {
    this->message_ = strdup(message);
    StoreStackTrace();
  }
  ~Exception() {
    if (message_) free(message_);
  }
  virtual void StoreStackTrace() {
    stack_size_ = backtrace(stack_, kMaxStackSize);
  }
  virtual void PrintStackTrace() {
    if (message_) {
      LOG(ERROR) << message_;
    }
    // Exclude top 1 stack entry (storeStackTrace)
    backtrace_symbols_fd(stack_+1, stack_size_-1, STDERR_FILENO);
  }


 protected:
  char* message_;


 private:
  static const size_t kMaxStackSize = 100;
  size_t stack_size_;
  void *stack_[kMaxStackSize];
};

}  // namespace tpk
#endif  // TPK_EXCEPTION_H_
