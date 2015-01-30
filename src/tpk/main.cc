/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include <iostream>
#include <cerrno>
#include "tpk/task.h"
#include "tpk/exception.h"


int main(const int argc, char* argv[]) {
  try {
    // Create a task to do
    tpk::Task t(argc, argv);

    // Do the task
    t.Run();
  } catch(tpk::Exception *e) {
    e->printStackTrace();
    return -1;
  }
  return 0;
}
