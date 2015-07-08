/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_TASK_H_
#define TPK_TASK_H_

#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#endif
#include "common/utils/logging.h"

namespace tpk {

class Task {
 public:
  Task();
  ~Task();
  bool Init(int argc, char** argv);
  bool Run();

 private:
  int Install();
  int Update();
  int Uninstall();
  int Reinstall();

  SCOPE_LOG_TAG(TpkTask)
};  //  class Task

}  //  namespace tpk

#endif  // TPK_TASK_H_
