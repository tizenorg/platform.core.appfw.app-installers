#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cstring>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <zlib.h>
#include <unzip.h>
#include <assert.h>
#include <fcntl.h>
#include <step-unzip.hxx>

#define dir_delimter '/'
#define READ_SIZE 8192

step_unzip::step_unzip()
:
__is_extracted(false)
{}

/* Create the directory of 'path' and if needed its parents
 with the current creation mode. Don't returns error if the
 directory already exists. */
int mkdir_u (const char *path)
{
	int result, length, iter;
	char buffer[PATH_MAX];
	struct stat s;

	result = mkdir (path, 0700);
	if (result && errno == ENOENT)
	{
		length = (int) strlen (path);
		if (length >= PATH_MAX)
		{
			errno = ENAMETOOLONG;
			return -1;
		}
		memcpy (buffer, path, length + 1);
		iter = length;
		do
		{
			while (iter && buffer[--iter] != '/');
			while (iter && buffer[iter - 1] == '/')
				iter--;
			buffer[iter] = 0;
			result = mkdir (buffer, 0700);
		}
			while (result && errno == ENOENT);
			if (!result)
			{
				do
				{
					buffer[iter] = '/';
					while (++iter < length && buffer[iter]);
					result = mkdir (buffer, 0700);
				}
				while (!result && iter < length);
			}
	}
	if (result && errno == EEXIST)
	{
		if (stat (path, &s) || !S_ISDIR (s.st_mode))
			errno = EEXIST;
		else
			result = 0;
	}

	return result;
}

/* Decompress directory from src to dest.
		returns Z_OK on success, Z_MEM_ERROR if memory could not be
		allocated for processing, Z_DATA_ERROR if the deflate data is
		invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
		the version of the library linked do not match, or Z_ERRNO if there
		is an error reading or writing the files. */
int unzip_u(char *src, char *dest)
{
	int ret;
	chdir(dest);

	unzFile *zipfile = (unzFile *)unzOpen(src);
	if (zipfile == NULL)
	{
		std::cout << "[process unzip] : Failed to found the source dir :" << src << std::endl;
		return -1;
	}

	unz_global_info info;
	if (unzGetGlobalInfo(zipfile, &info) != UNZ_OK)
	{
		std::cout << "[process unzip] : Failed to read file global info" << std::endl;
		unzClose(zipfile);
		return -1;
	}

	// Hold data read from the zip file in buffer.
	char read_buffer[READ_SIZE];

	// Extract all files
	uLong i;
	for (i = 0; i < info.number_entry; i++)
	{
		// Get info about current file.
		unz_file_info file_info;
		char filename[PATH_MAX];
		if (unzGetCurrentFileInfo(zipfile, &file_info, filename, PATH_MAX,
															NULL, 0, NULL, 0 )
				!= UNZ_OK)
		{
			std::cout << "[process unzip] : Failed to read file info" << std::endl;
			unzClose(zipfile);
			return -1;
		}

		// Check if this entry is a directory or file.
		const size_t filename_length = strlen(filename);
		// Entry is a set of directory, create it.
		if(strrchr(filename, dir_delimter) != NULL)
		{
			char *dir = (char*) malloc (filename_length);
			char *file = (char*) malloc (filename_length);

			dir = strdup(filename);
			file = strrchr(dir, dir_delimter);
			int it = 0;
			while(file[it] != '\0')
			{
				file[it] = '\0';
				it++;
			}
			mkdir_u(dir);
			if(file != NULL)
				free(dir);
		}
		if (filename[filename_length-1] == dir_delimter)
		{
			// Entry is a directory create it.
			ret = mkdir(filename, 0700);
		}
		else
		{
			// Entry is a file extract it.
			if (unzOpenCurrentFile(zipfile) != UNZ_OK)
			{
				std::cout << "[process unzip] : Failed to open file" << std::endl;
				unzClose(zipfile);
				return -1;
			}
			// Write out the data.
			FILE *out = fopen(filename, "wb");
			if ( out == NULL )
			{
				std::cout << "[process unzip] : Failed to open destination file " << std::endl;
				unzCloseCurrentFile(zipfile);
				unzClose(zipfile);
				return -1;
			}

			int ret = UNZ_OK;
			do
			{
				ret = unzReadCurrentFile(zipfile, read_buffer, READ_SIZE);
				if ( ret < 0 )
				{
					std::cout << "[process unzip] : error " << ret << std::endl;
					unzCloseCurrentFile(zipfile);
					unzClose(zipfile);
					return -1;
				}
				if (ret > 0)
				{
					fwrite(read_buffer, ret, 1, out);
				}
			} while (ret > 0);

			fclose(out);
		}

		unzCloseCurrentFile(zipfile);

		// Go the the next entry listed in the zip file.
		if ((i+1) < info.number_entry)
		{
			if (unzGoToNextFile( zipfile ) != UNZ_OK)
			{
				std::cout << "[process unzip] : Failed to read next file" << std::endl;
				unzClose(zipfile);
				return -1;
			}
		}
	}

	unzClose( zipfile );

	return Z_OK;
}

int step_unzip::extactTo_u(char * tmp_dir, char * source_dir) {
	int ret = APPINST_R_OK;
	if (__is_extracted)
	{
		std::cout << source_dir << "[process unzip] : is already extracted " << std::endl;
		return APPINST_R_OK;
	}
	if (unzip_u(source_dir, tmp_dir) != Z_OK)
	{
		std::cout << "[process unzip] : Failed to uncompress " << source_dir << std::endl;
		ret = APPINST_R_ERROR;
	}
	__is_extracted = true;

	return ret;
}

int step_unzip::process (Context_installer* data) {
	assert (data->file_path != NULL);
	assert (!access (data->file_path, F_OK));
	char *install_temp_dir;
	for(;;)
	{
		install_temp_dir = tempnam("/tmp", "unpack_dir_");
		if (install_temp_dir == NULL)
		{
			std::cerr << "[process unzip] : Failed to allocate memory" << std::endl;
			return APPINST_R_ERROR;
		}
		if (mkdir(install_temp_dir, 0700) == 0)
			break;
		free(install_temp_dir);
		if (errno != EEXIST && errno != EAGAIN)
		{
			std::cerr << "[process unzip] : Failed to create temporary directory" << std::endl;
			return APPINST_R_ERROR;
		}
	}
	std::cout << "[process unzip] : extract " << data->file_path << " into temp directory : " << install_temp_dir << std::endl;

	if(extactTo_u(install_temp_dir, data->file_path) != APPINST_R_OK) {
		std::cout << "[process unzip] : Failed to process unpack step"<< std::endl;
		return APPINST_R_ERROR;
	}
	free(install_temp_dir);
	return APPINST_R_OK;
}

int step_unzip::clean (Context_installer* data) {
	int ret = APPINST_R_OK;

	if(access (data->file_path, F_OK))
		ret = remove(data->file_path) ? APPINST_R_OK : APPINST_R_EINVAL;

	return ret;
}

int step_unzip::undo (Context_installer* data) {
	return APPINST_R_OK;
}
