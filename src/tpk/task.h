/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */
#ifndef TPK_TASK_H_
#define TPK_TASK_H_

#include <memory>

#ifdef HOSTTEST
#include "test/mock_pkgmgr_installer.h"
#endif

#include "tpk/tpk_app_query_interface.h"

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

  std::unique_ptr<TpkAppQueryInterface> query_interface_;
};  //  class Task

}  //  namespace tpk

#endif  // TPK_TASK_H_
