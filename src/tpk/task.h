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
  Task(int argc, char** argv);
  ~Task();
  void Run();


 private:
  void Install();
  void Uninstall();
  void Reinstall();

  pkgmgr_installer* pi_;
  int request_;
};  //  class Task

}  //  namespace tpk
#endif  // TPK_TASK_H_
