#include "Plat.hpp"

string gt::Plat::getHomeDirectory()
{
	string d = "./";

	#ifndef __WIN32__
		char *path = getenv("HOME");
		return (path == NULL) ? d : string(path);
	#else
		char *drive = getenv("HOMEDRIVE");
		char *path = getenv("HOMEPATH");
		return (path == NULL) ? d : string(drive) + string(path);
	#endif
}

string gt::Plat::getSettingsDirectory()
{
	string final = "gtorrent";

	#ifndef __WIN32__
		string path = getHomeDirectory() + PLAT_FILE_SEP + ".config" + PLAT_FILE_SEP + final;

		if (!dirExists(path))
			mkdir(path, true);

		return path;
	#else
		TCHAR path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path))) {
		    PathAppend(path, final);

		    return final;
		} else {
			return "." + PLAT_FILE_SEP;
		}
	#endif
}

string gt::Plat::getFileTorrentSession()
{
	return getSettingsDirectory() + PLAT_FILE_SEP + "session.gto";
}

bool gt::Plat::dirExists(string path)
{
	#ifndef __WIN32__
		struct stat s;
		stat(path.c_str(), &s);

		if (S_ISDIR(s.st_mode))
			return true;

		return false;
	#else
		return false;
	#endif
}

bool gt::Plat::mkdir(string name, bool rec)
{
	#ifndef __WIN32__
		return (::mkdir(name.c_str(), 0755) == 0) ? true : false;
	#else
		return false;
	#endif
}