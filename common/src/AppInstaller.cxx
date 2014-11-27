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
		ListStep.push_back(*step);
	} catch (int e) {
		return -1;
	}
	return 0;
}

int AppInstaller::Run() {
	std::list<Step>::const_iterator it (ListStep.begin());
	std::list<Step>::const_iterator itStart(ListStep.begin());
	std::list<Step>::const_iterator itend(ListStep.end());
	int ret = 0;
	for(;it!=itend;++it) {
		
		if((*it).process(&ctx) != 0)
			std::cout << "Error during processing";
			ret = -1;
			break;
	}
	if (it!=itend) {
		for(;it!=itStart;it--) {
			if((*it).undo(&ctx) != 0) {
				std::cout << "Error during undo operation";
				ret = -2;
				break;
			}
		}
	} else {
		for(;it!=itStart;it--) {
		if((*it).clean(&ctx) != 0)
			std::cout << "Error during clean operation";
			ret = -3;
			break;
		}
	}
	return ret;
}
