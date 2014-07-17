#include "Core.hpp"
#include "Plat.hpp"
#include <fstream>
#include "Log.hpp"

gt::Core::Core() :
	m_running(true)
{
	libtorrent::error_code ec;
	m_session.listen_on(make_pair(6881, 6889), ec);

	// Resume where we last left off
	loadTorrentSession();
}

bool gt::Core::isMagnetLink(string const& url)
{
	const string prefix = "magnet:";
	return url.compare(0, prefix.length(), prefix) == 0;
}

bool gt::Core::isRunning()
{
	return m_running;
}

string gt::Core::getDefaultSavePath()
{
	// Keeping this for legacy purposes for now, could be moved to gt::Plat

	#ifndef _WIN32
	char *savepath = getenv("HOME");
	return savepath == NULL ? string("") : string(savepath)+"/Downloads";
	#else
	char *savedrive = getenv("HOMEDRIVE");
	char *savepath = getenv("HOMEPATH");
	return savepath == NULL ? string("") : string(savedrive)+string(savepath)+"/Downloads";
	#endif
}

vector<shared_ptr<Torrent> > &gt::Core::getTorrents()
{
	return m_torrents;
}

shared_ptr<Torrent> gt::Core::addTorrent(string path)
{
	if (path.empty())
		return NULL;
	
	shared_ptr<Torrent> t = make_shared<Torrent>(path);
	libtorrent::torrent_handle h = m_session.add_torrent(t->getTorrentParams());

	t->setHandle(h);
	m_torrents.push_back(t);

	// TODO: Event polling for magnet update information
	//libtorrent::file_storage fs = info.files();
	//libtorrent::torrent_info info = h.get_torrent_info();

	// An Example For Fetching All Files From Torrent:

	/*unsigned int numFiles = fs.num_files();

	printf("Total Files: %i\n", fs.num_files());
	printf("=========================================\n");

	for (unsigned int i = 0; i < numFiles; ++i)
	{
		libtorrent::file_entry e = fs.at(i);

		printf("Name:\t\t%s\n", e.path.c_str());
		printf("Size:\t\t%i\n", e.size);
		printf("-----------------------------------------\n");
	}

	printf("Total Size:\t%Li\n\n", fs.total_size());

	printf("Downloading data from \"%s\"...\n", path.c_str());*/

	return t;
}

void gt::Core::saveTorrentSession()
{
	// session::save_state only saves session information, not 
	ofstream of(gt::Plat::getFileTorrentSession(), ios::binary);

	size_t len = getTorrents().size();
	of.write(reinterpret_cast<char*>(&len), sizeof(size_t));

	for (auto &t : getTorrents()) {
		len = t->getPath().length();

		of.write(reinterpret_cast<char*>(&len), sizeof(size_t));
		of.write((char*)t->getPath().c_str(), len);
	}

	of.close();
}

void gt::Core::loadTorrentSession()
{
	ifstream in(gt::Plat::getFileTorrentSession(), ios::binary);

	if (in.good()) {
		size_t t_files = 0;
		in.read(reinterpret_cast<char*>(&t_files), sizeof(size_t));

		for (size_t i = 0; i < t_files; ++i) {
			size_t len = 0;
			in.read(reinterpret_cast<char*>(&len), sizeof(size_t));
			char file[len] = {0};
			in.read((char*) file, len);

			addTorrent(file);
		}
	}
}

void gt::Core::update()
{
	/*auto iter = begin(m_torrents);

	while (iter != end(m_torrents))
	{
		auto &t = **iter;

		gt::Event event;

		if (t.pollEvent(event))
		{
			switch (event.type)
			{
				case gt::Event::DownloadCompleted:
					printf("Done!\n");
					iter = m_torrents.erase(iter);
				break;
			}
		}
		else
		{
			++iter;
		}
	}*/
}

void gt::Core::shutdown()
{
	gt::Log::Debug("Shutting down core library...");
	saveTorrentSession();
	m_running = false;
}
