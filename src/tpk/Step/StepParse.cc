#include "boost/filesystem.hpp"

#include "StepParse.h"
#include "ManifestParser/Manifest.h"
#include "../Exception.h"
#include "common/step/step.h"


/*
  TODO
 */

namespace tpk {
namespace Step {

/* short namespace/class name */
typedef common_installer::Step::Status Status;
using boost::filesystem::path;

/* Error codes to be returned */
static const Status
  &status_ok = common_installer::Step::Status::OK;
static const Status
  &status_error = common_installer::Step::Status::ERROR;

/* Internal exceptions */
class FileNotFoundException : public std::exception {};


/* process()
 * Parse tizen-manifest.xml and get the data from it
 * Store the data into the context_
 */
Status
StepParse::process() {

  try {
    boost::filesystem::path mPath  = getManifestFilePath(context_->unpacked_dir_path());
    tpk::ManifestParser::Manifest m(mPath.c_str());
    setContextByManifest(m);
  } catch (FileNotFoundException *e) {
    delete e;   // Consume this exception here
    return status_error;
  } catch (tpk::ManifestParser::FileOpenFailureException *e) {

    delete e;
    return status_error;
  }
  return status_ok;
}


/* in parse() : Get manifest file path from the package unzipped directory
 */
boost::filesystem::path
StepParse::getManifestFilePath(const boost::filesystem::path& dir) {
  static const char *manifestFileName = "tizen-manifest.xml";

  path mPath(dir);
  mPath /= manifestFileName;

  std::cout << "[StepParse] manifest file path: " << mPath << endl;
  if (!boost::filesystem::exists(mPath)) {
    std::cerr << "[StepParse] " << manifestFileName << " not found from the package" << std::endl;
    throw new FileNotFoundException();
  }
  return mPath; // object copy
}


/* Read manifest xml, and set up context_ object
 */
void
StepParse::setContextByManifest(tpk::ManifestParser::Manifest& m) {
  std::cout << "[StepParse] Parse manifest and get values" << endl;
}
}  // end ns: Step
}  // end ns: tpk
