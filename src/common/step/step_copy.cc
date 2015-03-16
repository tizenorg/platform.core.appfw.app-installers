/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */

#include "common/step/step_copy.h"

#include <sys/stat.h>

#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>

#include "utils/file_util.h"

namespace bf = boost::filesystem;
namespace bs = boost::system;

namespace {

int64_t GetBlockSizeForPath(const bf::path& path_in_partition) {
  struct stat stats;
  if (stat(path_in_partition.string().c_str(), &stats)) {
    LOG(ERROR) << "stat(" << path_in_partition.string()
               << ") failed - error code: " << errno;
    return -1;
  }
  return stats.st_blksize;
}

int64_t DirectorySize(const bf::path &p, int64_t block_size) {
  auto transform = [block_size](const bf::path& p) {
    struct stat stats;
    if (stat(p.string().c_str(), &stats)) {
      LOG(ERROR) << "stat(" << p.string()
                 << ") failed - error code: " << errno;
      return static_cast<boost::uintmax_t>(0);
    }
    return static_cast<boost::uintmax_t>(
        ((static_cast<int64_t>(stats.st_size) + block_size - 1) / block_size)
        * block_size);
  };
  return static_cast<int64_t>(std::accumulate(
      boost::make_transform_iterator(bf::recursive_directory_iterator(p),
          transform),
      boost::make_transform_iterator(bf::recursive_directory_iterator(),
          transform),
      static_cast<boost::uintmax_t>(0)));
}

bool CheckFreeSpaceAtPath(uint64_t required_size,
    const boost::filesystem::path& target_location) {
  bs::error_code error;
  bf::space_info space_info = bf::space(target_location, error);
  if (error) {
    LOG(ERROR) << "Failed to get space_info: " << error.message();
    return false;
  }
  LOG(DEBUG) << "Required free space: " << required_size << "B";
  LOG(DEBUG) << "Available free space: " << space_info.free << "B";
  return (space_info.free >= required_size);
}

}  // namespace

namespace common_installer {
namespace copy {

namespace bf = boost::filesystem;
namespace bs = boost::system;

Step::Status StepCopy::process() {
  assert(!context_->pkgid().empty());

  bf::path install_path = bf::path(context_->GetApplicationPath());

  context_->set_pkg_path(install_path.string());

  // FIXME: correctly order app's data.
  // If there is 1 app in package, app's data are stored in <pkg_path>/<app_id>
  // If there are >1 apps in package, app's data are stored in <pkg_path>
  // considering that multiple apps data are already separated in folders.
  if (context_->manifest_data()->uiapplication &&
      !context_->manifest_data()->uiapplication->next)
    install_path /= bf::path(context_->manifest_data()->mainapp_id);

  bs::error_code error;
  bf::create_directories(install_path.parent_path(), error);
  if (error) {
    LOG(ERROR) << "Cannot create directory: "
               << install_path.parent_path().string();
    return Step::Status::ERROR;
  }
  bf::rename(context_->unpacked_dir_path(), install_path, error);
  if (error) {
    // block size will be calculated depending on root directory for all
    // application files.
    int64_t size = GetBlockSizeForPath(context_->GetRootApplicationPath());
    if (size < 0) {
      LOG(ERROR) << "Cannot get block size of device of installation path";
      return Step::Status::ERROR;
    }

    int64_t unpacked_size =
        DirectorySize(bf::path(context_->unpacked_dir_path()), size);

    // FIXME: this approach makes one assumption:
    //   (block size of install path) == (block size of unpacked path)
    // In the case that this is not true, it will work incorrectly.
    if (!CheckFreeSpaceAtPath(
        static_cast<uint64_t>(unpacked_size),
        context_->GetRootApplicationPath())) {
      LOG(ERROR) << "There is no free space at application root";
      return Step::Status::OUT_OF_SPACE;
    }

    LOG(DEBUG) << "Cannot move directory. Will try to copy...";
    if (!utils::CopyDir(bf::path(context_->unpacked_dir_path()),
        install_path)) {
      LOG(ERROR) << "Fail to copy tmp dir: " << context_->unpacked_dir_path()
                 << " to dst dir: " << install_path.string();
      return Step::Status::ERROR;
    }
    bs::error_code error;
    bf::remove_all(context_->unpacked_dir_path(), error);
    if (error) {
      LOG(WARNING) << "Cannot remove temporary directory: "
                   << context_->unpacked_dir_path();
    }
  }
  LOG(INFO) << "Successfully move/copy: " << context_->unpacked_dir_path()
            << " to: " << install_path.string() << " directory";
  return Status::OK;
}

Step::Status StepCopy::clean() {
  return Status::OK;
}

Step::Status StepCopy::undo() {
  if (bf::exists(context_->pkg_path()))
    bf::remove_all(context_->pkg_path());
  return Status::OK;
}

}  // namespace copy
}  // namespace common_installer
