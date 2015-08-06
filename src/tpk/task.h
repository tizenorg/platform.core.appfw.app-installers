/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_TASK_H_
#define TPK_TASK_H_

#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#endif

#include "common/pkgmgr_interface.h"

namespace tpk {

class Task {
 public:
  Task();
  ~Task();
  bool Init(int argc, char** argv);
  bool Run();

 private:
  bool Install();
  bool Update();
  bool Uninstall();
  bool Reinstall();

  common_installer::PkgMgrPtr pkgmgr_;
};  //  class Task

}  //  namespace tpk

#endif  // TPK_TASK_H_
