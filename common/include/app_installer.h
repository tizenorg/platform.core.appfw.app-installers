#ifndef __APPINSTALLER_H__
#define __APPINSTALLER_H__
#include <context_installer.h>
#include <step.h>
#include <list>

class AppInstaller {

protected:
	std::list<Step*> ListStep;
	ContextInstaller* ctx_;
public:
	AppInstaller(int request, const char * file, const char* pkgid);
	~AppInstaller();
	int AddStep(Step *step);
	int Run();
};
#endif
