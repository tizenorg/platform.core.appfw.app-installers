/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

/*
  A step is made of 3 functions (that can be defined or NULL)
  and one data pointer.

  The functions are:
    - process  process the installation step
    - undo     undo the installation step after failure
    - clean    remove temporary data of the step after success

  These functions all have the same signature: they accept
  a pointer to something and they return an integer value stating
  the execution issue. 

  At the execution time, this functions if not NULL, will receive
  the 'data' pointer as first argument.

  The returned code of 0 indicates a succeful execution.
  Otherwise, the returned code should be set to -1 with errno set
  to some meaningful value.
*/
#ifndef COMMON_INCLUDE_STEP_STEP_H_
#define COMMON_INCLUDE_STEP_STEP_H_

#include "common/context_installer.h"

namespace common_installer {

class Step {
 public:
  virtual int process(ContextInstaller* context) = 0;
  virtual int undo(ContextInstaller* context) = 0;
  virtual int clean(ContextInstaller* context) = 0;
};

}  // namespace common_installer

#endif  // COMMON_INCLUDE_STEP_STEP_H_
