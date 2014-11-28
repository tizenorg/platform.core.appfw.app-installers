#ifndef __APPINSTALLER_H__
#define __APPINSTALLER_H__
#include <context_installer.h>
#include <step.hxx>
#include <list>

class AppInstaller {

protected:
	std::list<Step*> ListStep;
	Context_installer ctx;
public:
	AppInstaller(int request,char * file, char* pkgid);
	~AppInstaller();
	int AddStep(Step *step);
	int Run();
};
#endif
