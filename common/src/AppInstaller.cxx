#include <AppInstaller.hxx>
#include <iostream>
#include <stdio.h>

AppInstaller::AppInstaller(int request,char * file, char* pkgid) {
	if(asprintf(&(ctx.file_path),"%s",file) == -1)
		ctx.file_path = NULL;
	if(asprintf(&(ctx.pkgid),"%s",pkgid) == -1)
		ctx.pkgid = NULL;
	ctx.req = request;
}

AppInstaller::~AppInstaller() {
	free(ctx.file_path);
	free(ctx.pkgid);
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
