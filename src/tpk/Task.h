#ifndef __Task_H__
#define __Task_H__

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

};  // end class: Task

}  // end ns: tpk

#endif  //__Task_H__
