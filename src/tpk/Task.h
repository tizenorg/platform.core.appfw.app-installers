/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_TASK_H_
#define TPK_TASK_H_

#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#else
#include <pkgmgr_installer.h>
#endif

namespace tpk {

class Task {
 public:
  Task(const int argc, char** argv);
  ~Task();
  void run(void);


 private:
  pkgmgr_installer *pi;
  int request;
  void install(void);
  void uninstall(void);
  void reinstall(void);
};  //  class Task

}  //  namespace tpk
#endif  // TPK_TASK_H_
