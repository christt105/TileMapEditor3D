#pragma once

#include "FileSystem.h"
#include <thread>
#include <mutex>

#include "m1Events.h"

class FileWatch
{
public:
	FileWatch();
	~FileWatch();

public:
	void Subscribe(const char* folder);
	void StartWatching();
	void Watch();

	void Pause(bool pause);

private:
	void CheckFolders(std::list<m1Events::Event*>& ev);
	void CheckFilesCreatedAndRemoved(Folder* f, std::stack<Folder*>& stack, std::list<m1Events::Event*>& ev);
	void CheckRemovedFolders(Folder* f, std::list<m1Events::Event*>& ev);
	void CheckRemovedFiles(Folder* f, std::list<m1Events::Event*>& ev);
	void HandleEvents(std::list<m1Events::Event*>& e);

	void CheckIfFileMoved(std::list<m1Events::Event*>& evs, m1Events::Event* e, m1Events::Event::Type type);

public:
	std::mutex mtx;

private:
	std::string folder = "Assets/";
	std::thread thread;
	bool watch = true;
	Folder* root = nullptr;

	bool pause_watch = false;
};

