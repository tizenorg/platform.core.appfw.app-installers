#include <app_installer.h>
#include <iostream>
#include <cstdio>

AppInstaller::AppInstaller(int request, const char * file, const char* pkgid){
    ctx.file_path = file;
    ctx.pkgid = pkgid;
    ctx.req = request;

}

AppInstaller::~AppInstaller() {
}


int AppInstaller::AddStep(Step *step) {
	try {
		ListStep.push_back(step);
	} catch (int e) {
		return -1;
	}
	return 0;
}

int AppInstaller::Run() {
	std::list<Step *>::iterator it (ListStep.begin());
	std::list<Step *>::iterator itStart(ListStep.begin());
	std::list<Step *>::iterator itEnd(ListStep.end());

	int ret = 0;
	for(;it!=itEnd;++it) {
		if((*it)->process(&ctx) != APPINST_R_OK) {
			std::cout << "Error during processing" << std::endl;
			ret = -1;
			break;
		}
	}
	if (it!=itEnd) {
			std::cout << "Faillure occurs" << std::endl;
		do {
			if((*it)->undo(&ctx) != APPINST_R_OK) {
				std::cout << "Error during undo operation" << std::endl;
				ret = -2;
				break;
			}
		} while (it--!=itStart);
	} else {
			while (it--!=itStart) {
				if((*it)->clean(&ctx) != APPINST_R_OK) {
					std::cout << "Error during clean operation" << std::endl;
					ret = -3;
					break;
				}
			}
	}
	return ret;
}
