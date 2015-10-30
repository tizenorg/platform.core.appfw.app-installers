/* Copyright 2015 Samsung Electronics, license APACHE-2.0, see LICENSE file */

/* pkgdir_maker.cc
 * A tool creating/deleting data directories of global app packages.
 *
 * Global apps are installed in /usr/apps/, but this path is not allowed for
 * users to write their data.  To store app data of global apps, the data and
 * cache directories have to be created.
 *
 * This tool does two things:
 *   1. creates the global app package's data and cache directories
 *   ({pkgid}/data, {pkgid}/cache) into all user's apps_rw/ directories.
 *   Proper owner, group, permission and SMACK labels are set for those dirs.
 *   2. For new users in the future, the same directories are create into the
 *   user home template, /etc/skel/.
 *
 * NOTE: For prototyping, actual implementation is made as a bash script
 * (pkgdir_maker_impl.sh), but later it is to be replaced with native code.
 */

#include <errno.h>
#include <libgen.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, char** argv) {
  char cmd[BUFSIZE] = {0, };
  char tmp[BUFSIZE] = {0, };
  int i;

  snprintf(cmd, BUFSIZE, "pkgdir_maker_impl.sh");

  for (i=1; i < argc; i++) {
    snprintf(tmp, BUFSIZE, "%s %s", cmd, argv[i]);
    snprintf(cmd, BUFSIZE, "%s", tmp);
  }

  // set uid to root
  // NOTE: Even the setuid permission is set to the executable,
  //       this routine is needed.
  (void) setuid(0);
  (void) seteuid(0);

  return system(cmd);
}
