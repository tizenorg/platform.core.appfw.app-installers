#ifndef __STEPCOPY_H__
#define __STEPCOPY_H__
#include <step.h>
#include <context_installer.h>

class step_copy : public Step{
protected:
	char *app_dir;
public:
	int process(Context_installer*);
	int clean(Context_installer*);
	int undo(Context_installer*);
	step_copy();
};
#endif

