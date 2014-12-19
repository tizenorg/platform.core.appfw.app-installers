#ifndef __STEPUNZIP_H__
#define __STEPUNZIP_H__
#include <step.h>
#include <context_installer.h>

class step_unzip : public Step{
private:
		bool __is_extracted;
public:
		int process(Context_installer*);
		int clean(Context_installer*);
		int undo(Context_installer*);

		/*Extracts the current entry to the given output temp directory path.
		 * Sub directories are created based on the file path
		 */
		int extactTo_u(const char * tmp_dir, const char * source_dir);

		step_unzip();
};
#endif

