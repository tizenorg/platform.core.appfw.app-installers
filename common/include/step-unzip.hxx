#ifndef __STEPUNZIP_H__
#define __STEPUNZIP_H__
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <step.hxx>
#include <context_installer.h>

class step_unzip : public Step{
private:
		bool __is_extracted;
		int process(Context_installer*);
		int clean(Context_installer*);
		int undo(Context_installer*);
		int extactTo_u(char * tmp_dir, char * source_dir);
public:
		step_unzip();
};
#endif

