#pragma once

#include "Tools/FileSystem.h"
#include <thread>
#include <mutex>
#include "ExternalTools/efsw/efsw.hpp"

#include "Modules/m1Events.h"

class FileWatch
{
	friend class m1Resources;
private:
	class Listener : public efsw::FileWatchListener {
	public:
		Listener() = default;

		void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "");
	};
public:
	FileWatch();
	~FileWatch();

private:
	efsw::FileWatcher* fileWatcher = nullptr;
	void Subscribe(const char* folder);

	Listener* listener = nullptr;
	efsw::WatchID assetsID = 0ULL;
};

