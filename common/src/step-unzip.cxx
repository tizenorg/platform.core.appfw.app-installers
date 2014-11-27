#include <AppInstaller.hxx>
#include <iostream>
#include <stdio.h>
#include <step.hxx>
#include <stdlib.h>


namespace step_unzip {

static int
process (Context_installer * data)
{

  assert (data.file_path != NULL);
  assert (!access (data.file_path, F_OK));

  context.unpack_directory = mktemp("Unpack_temp");
  return unzip (data.unpack_directory, context.package_path);

}

static int
clean (void * data)
{
  int status;

  if (context.unpack_directory)
    {
      assert (!access (context.unpack_directory, W_OK));
      status = fs_remove_directory (context.unpack_directory, 1);
      free (context.unpack_directory);
      context.unpack_directory = NULL;
    }
  else
    status = 0;

  return status;
}


struct step step_unpack = {.process = process,.undo = clean,.clean = clean, .data = 0 };



AppInstaller::AppInstaller(int request,char * file, char* pkgid) {
	if(asprintf(&(ctx.file),"%s",file) == -1)
		ctx.file = NULL;
	if(asprintf(&(ctx.pkgid),"%s",pkgid) == -1)
		ctx.pkgid = NULL;
	ctx.req = request;
}

AppInstaller::~AppInstaller() {
	free(ctx.file);
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
