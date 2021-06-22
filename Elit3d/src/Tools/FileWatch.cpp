#include "Tools/FileWatch.h"
#include "Tools/System/Logger.h"
#include "Core/Application.h"
#include "Core/Globals.h"

#include <filesystem>

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

namespace fs = std::filesystem;

FileWatch::FileWatch()
{
	fileWatcher = new efsw::FileWatcher();
	listener = new FileWatch::Listener();
	std::string path = "./Assets";
	assetsID = fileWatcher->addWatch(path, listener);
	fileWatcher->watch();
}

FileWatch::~FileWatch()
{
	delete listener;
	delete fileWatcher;
}

void FileWatch::Subscribe(const char* froot)
{
}

void FileWatch::Listener::handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
{
	switch (action)
	{
	case efsw::Actions::Add:
		LOG("DIR (%s) FILE (%s) has event Added", dir.c_str(), filename.c_str());
		break;
	case efsw::Actions::Delete:
		LOG("DIR (%s) FILE (%s) has event Delete", dir.c_str(), filename.c_str());
		break;
	case efsw::Actions::Modified:
		LOG("DIR (%s) FILE (%s) has event Modified", dir.c_str(), filename.c_str());
		break;
	case efsw::Actions::Moved:
		LOG("DIR (%s) FILE (%s) has event Moved from (%s)", dir.c_str(), filename.c_str(), oldFilename.c_str());
		break;
	default:
		LOGE("Should never happen!", dir.c_str(), filename.c_str());
	}
}
