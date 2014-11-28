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
#ifndef __STEP_H__
#define __STEP_H__
#include <context_installer.h>

class Step {
public:
	virtual int process(Context_installer*)=0;
	virtual int undo(Context_installer*)=0;
	virtual int clean(Context_installer*)=0;

};
#endif
