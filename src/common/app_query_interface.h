// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved

#ifndef COMMON_APP_QUERY_INTERFACE_H_
#define COMMON_APP_QUERY_INTERFACE_H_

namespace common_installer {


/**
 * \brief Base interface for some helper functionalities used before
 *        configuring app-installer steps.
 *        Eg. it is used to check, if package is to be installed or updated
 */
class __attribute__ ((visibility ("default"))) AppQueryInterface {
 public:
  /** virtual destructor (for inheritance) */
  virtual ~AppQueryInterface() { }

  /**
   * \brief abstract method for checking if package is installed based
   *        on argv
   *
   * \param argc main() argc argument passed to the backend
   * \param argv main() argv argument passed to the backend
   *
   * \return true if package is installed
   */
  virtual bool IsAppInstalledByArgv(int argc, char** argv) = 0;
};

}  // namespace common_installer

#endif  // COMMON_APP_QUERY_INTERFACE_H_
