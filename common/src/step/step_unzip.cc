#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <zlib.h>
#include <unzip.h>
#include <cassert>
#include <fcntl.h>
#include <step_unzip.h>
#include <boost/filesystem.hpp>
#include <boost/chrono/detail/system.hpp>

#define DIR_DELIMITER '/'
#define ZIPBUFSIZE 8192
#define ZIPMAXPATH 256
#define DIR_MODE S_IRWXU

step_unzip::step_unzip()
:
__is_extracted(false)
{}

int createDir_u(const char* path) {
	boost::filesystem::path dir(path);
	boost::system::error_code error;
	boost::filesystem::create_directories(dir, error);

	if (error != 0) {
		std::cout <<  "[process unzip] : Failed to create directory " << boost::system::system_error(error).what() <<std::endl;
		return -1;
	}

	boost::filesystem::permissions(
		dir,
		boost::filesystem::owner_read | boost::filesystem::owner_write | boost::filesystem::owner_exe |boost::filesystem::group_read | boost::filesystem::others_read,
		error);

	if (error != 0) {
		std::cout << "[process unzip] : Failed to set permission "<< boost::system::system_error(error).what() <<std::endl;
		return -1;
	}
	return 0;
}

int unzip_u(const char *src, const char *dest) {
		unz_global_info info;
		char read_buffer[ZIPBUFSIZE];
		unz_file_info raw_file_info;
		char raw_file_name_in_zip[ZIPMAXPATH];
		uLong i;

	chdir(dest);

	unzFile *zip_file_ = (unzFile *)unzOpen(src);
	if (zip_file_ == NULL) {
		std::cout << "[process unzip] : Failed to open the source dir :" << src << std::endl;
		return -1;
	}

	if (unzGetGlobalInfo(zip_file_, &info) != UNZ_OK) {
		std::cout << "[process unzip] : Failed to read global info" << std::endl;
		unzClose(zip_file_);
		return -1;
	}

	for (i = 0; i < info.number_entry; i++) {
		if (unzGetCurrentFileInfo(zip_file_,
															&raw_file_info,
															raw_file_name_in_zip,
															sizeof(raw_file_name_in_zip) - 1,
															NULL,  // extraField.
															0,  // extraFieldBufferSize.
															NULL,  // szComment.
															0)  // commentBufferSize.
				!= UNZ_OK) {
			std::cout << "[process unzip] : Failed to read file info" << std::endl;
			unzClose(zip_file_);
			return -1;
		}

		if (raw_file_name_in_zip[0] == '\0')
			return -1;

		const size_t raw_file_name_in_zip_length = strlen(raw_file_name_in_zip);

		if (strrchr(raw_file_name_in_zip, DIR_DELIMITER) != NULL) {
			char *dir = (char*) malloc (raw_file_name_in_zip_length);
			char *file = (char*) malloc (raw_file_name_in_zip_length);

			dir = strdup(raw_file_name_in_zip);
			file = strrchr(dir, DIR_DELIMITER);

			int it = 0;
			while (file[it] != '\0') {
				file[it] = '\0';
				it++;
			}
			if (createDir_u(dir) != 0) {
				std::cout << "[process unzip] : Failed to create directory " << dir << std::endl;
				return -1;
			}
			if (file != NULL)
				free(dir);
		}
		if (raw_file_name_in_zip[raw_file_name_in_zip_length-1] != DIR_DELIMITER) {
			if (unzOpenCurrentFile(zip_file_) != UNZ_OK) {
				std::cout << "[process unzip] : Failed to open file" << std::endl;
				unzClose(zip_file_);
				return -1;
			}

			FILE *out = fopen(raw_file_name_in_zip, "wb");
			if (out == NULL) {
				std::cout << "[process unzip] : Failed to open destination " << std::endl;
				unzCloseCurrentFile(zip_file_);
				unzClose(zip_file_);
				return -1;
			}

			int ret = UNZ_OK;
			do
			{
				ret = unzReadCurrentFile(zip_file_, read_buffer, ZIPBUFSIZE);
				if (ret < 0) {
					std::cout << "[process unzip] : Failed to read data " << ret << std::endl;
					unzCloseCurrentFile(zip_file_);
					unzClose(zip_file_);
					return -1;
				}
				if (ret > 0) {// Some data is read. Write it to the output.
					fwrite(read_buffer, ret, 1, out);
				}
			} while (ret > 0);

			fclose(out);
		}

		unzCloseCurrentFile(zip_file_);

		if ((i+1) < info.number_entry) {
			if (unzGoToNextFile(zip_file_) != UNZ_OK) {
				std::cout << "[process unzip] : Failed to read next file" << std::endl;
				unzClose(zip_file_);
				return -1;
			}
		}
	}

	unzClose(zip_file_);

	return Z_OK;
}

int step_unzip::extactTo_u(const char * tmp_dir, const char * source_dir) {
	int ret = APPINST_R_OK;

	if (__is_extracted) {
		std::cout << source_dir << "[process unzip] : is already extracted " << std::endl;
		return APPINST_R_OK;
	}

	if (unzip_u(source_dir, tmp_dir) != Z_OK) {
		std::cout << "[process unzip] : Failed to uncompress " << source_dir << std::endl;
		ret = APPINST_R_ERROR;
	}
	__is_extracted = true;

	return ret;
}

int step_unzip::process (ContextInstaller* data) {
	assert (!data->file_path().empty());
	assert (!access (data->file_path().c_str(), F_OK));

	char *install_temp_dir;

	for(;;) {
		install_temp_dir = tempnam("/tmp", "unpack_dir_");
		if (install_temp_dir == NULL) {
			std::cerr << "[process unzip] : Failed to allocate memory" << std::endl;
			return APPINST_R_ERROR;
		}
		if (mkdir(install_temp_dir, DIR_MODE) == 0)
			break;

		free(install_temp_dir);
		if (errno != EEXIST && errno != EAGAIN) {
			std::cerr << "[process unzip] : Failed to create temporary directory" << std::endl;
			return APPINST_R_ERROR;
		}
	}

	if (extactTo_u(install_temp_dir, data->file_path().c_str()) != APPINST_R_OK) {
		std::cout << "[process unzip] : Failed to process unpack step"<< std::endl;
		return APPINST_R_ERROR;
	}
	data->set_unpack_directory(install_temp_dir);
	free(install_temp_dir);

	std::cout << "[process unzip] : extract " << data->file_path() << " into temp directory : " << data->unpack_directory() << std::endl;
	return APPINST_R_OK;
}

int step_unzip::clean (ContextInstaller* data) {
	int ret = APPINST_R_OK;
	std::cout << "[clean unzip] data->unpack_directory"<< data->unpack_directory() << std::endl;/*
	if (access (data->unpack_directory().c_str(), F_OK) == 0) {
		ret = boost::filesystem::remove_all(data->unpack_directory()) ? APPINST_R_OK : APPINST_R_ERROR;
	}*/
	return ret;
}

int step_unzip::undo (ContextInstaller* data) {
	return clean(data);
}
