#pragma once

// We can defines these options here in once place,
// or in a large collection of files organized in a directory 
// served out during compilation time

#ifndef __WIN32__
	#include <sys/types.h>
	#include <sys/stat.h>
	#define PLAT_FILE_SEP string("/")
#else
	#define PLAT_FILE_SEP string("\\")
#endif

namespace gt
{
	// Provides easy static interface for cross-platform requirements

	class Plat
	{
	public:
		static string getHomeDirectory();
		static string getSettingsDirectory();

		// Specific file definitions :

		static string getFileTorrentSession();

		static bool dirExists(string path);
		static bool mkdir(string name, bool rec = false);
	};
}