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
#include <AppInstaller.hxx>
#include <context_installer.h>

typedef int (*action_member) (Context_installer*);

class Step {
public:	
	action_member process;
	action_member undo;
	action_member clean;
	Step(action_member process_args,action_member undo_args,action_member clean_args);
};

Step::Step(action_member process_args,action_member undo_args,action_member clean_args){
	process = process_args;
	undo = undo_args;
	clean = clean_args;
};


#endif
