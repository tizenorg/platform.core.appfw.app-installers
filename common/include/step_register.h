#ifndef __STEPREGISTER_H__
#define __STEPREGISTER_H__
#include <step.h>
#include <context_installer.h>

class step_register : public Step{
public:
	int process(Context_installer*);
	int clean(Context_installer*);
	int undo(Context_installer*);
	step_register();
};
#endif

