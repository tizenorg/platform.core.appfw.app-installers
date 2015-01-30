#include <iostream>
#include <cerrno>

#include "Exception.h"
#include "Task.h"

int
main(const int argc, char* argv[]) {
	try {
		// Create a task to do
		tpk::Task t(argc, argv);

		// Do the task
		t.run();
	} catch(tpk::Exception *e) {
    e->printStackTrace();
		return -1;
	}
	return 0;
}
