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

		static int createDir_u(const char *path);
		static int unzip_u(char *src, char *dest);
		/*Extracts the current entry to the given output temp directory path.
		* Sub directories are created based on the file path
		*/
		int extactTo_u(char * tmp_dir, char *source_dir);
		step_unzip();
};
#endif
