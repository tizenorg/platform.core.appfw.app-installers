/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include "Exception.h"
#include "Task.h"
#include <iostream>
#include <cerrno>


int main(const int argc, char* argv[]) {
  try {
    // Create a task to do
    tpk::Task t(argc, argv);

    // Do the task
    t.run();
  } catch(tpk::Exception *e) {
    e->printStackTrace();
    return -1;
  }
  return 0;
}
