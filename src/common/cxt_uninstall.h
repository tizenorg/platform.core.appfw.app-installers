/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#ifndef COMMON_CTX_UNINSTALL_H_
#define COMMON_CTX_UNINSTALL_H_

#include <pkgmgr_parser.h>

#include <unistd.h>
#include <sys/types.h>

#include <memory>
#include <string>

#include "common/context_installer.h"

namespace common_installer {
namespace uninstall {

enum {
  APPINST_R_EINVAL = -2,    /**< Invalid argument */
  APPINST_R_ERROR = -1,   /**< General error */
  APPINST_R_OK = 0      /**< General success */
};

class ConfigData;
using ConfigDataPtr = std::unique_ptr<ConfigData>;

class ConfigData {
 public:
  ConfigData() {}

  std::string application_name() const { return application_name_; }
  void set_application_name(const std::string& app_name) {
    application_name_ = app_name;
  }
  std::string required_version() const { return required_version_; }
  void set_required_version(const std::string& required_version) {
    required_version_ = required_version;
  }

 private:
  std::string application_name_;
  std::string required_version_;
};

//TODO ContextInstaller might change name to Context.
//Installation then would use CtxInstall, here CtxUninstall, maybe CtxUpdate?
class CtxUninstall: public ContextInstaller {
 public:
  CtxUninstall();
  virtual ~CtxUninstall();

  int request_type() const { return req_; }
  void set_request_type(int req) {
   req_ = req;
  }

  std::string pkgid() const { return pkgid_; }
  void set_pkgid(const std::string& pkgid) {
   pkgid_ = pkgid;
  }

  uid_t uid() const { return uid_; }

 private :
  //TODO this possibly is not needed here? maybe in Base class?
  // request type: Install, Reinstall, Uninstall, Update.
  //int req_;

  //TODO this variable is also declared in Base class!
  // pkgid used for update or uninstallation processing
  std::string pkgid_;

  //TODO not sure, if it is needed here.
  // uid of the user that request the operation
  //uid_t uid_;
};

}  // namespace uninstall
}  // namespace common_installer

#endif  // COMMON_CTX_UNINSTALL_H_

