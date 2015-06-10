/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#include <iostream>
#include "tpk/task.h"
#include "common/utils/logging.h"


int main(const int argc, char* argv[]) {
  tpk::Task t;
  if (!t.Init(argc, argv)) {
    std::cerr << "Task init failure" << std::endl;
    return -1;
  }
  if (!t.Run()) {
    std::cerr << "Task run failure" << std::endl;
    return -1;
  }
  return 0;
}
