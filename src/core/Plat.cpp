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
		string path = getHomeDirectory() + "/" + ".config" + "/" + final;

		if (!dirExists(path))
			mkdir(path, true);

		return path;
	#else
		string path = string(getenv("APPDATA")) + string("/") + final;
		mkdir(path);

		return path;
	#endif
}

string gt::Plat::getFileTorrentSession()
{
	return getSettingsDirectory() + "/" + "session.gto";
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
	// TODO: Recognize `req` argument

	#ifndef __WIN32__
		return (::mkdir(name.c_str(), 0755) == 0) ? true : false;
	#else
		_mkdir(name.c_str());
		return true;
	#endif
}
