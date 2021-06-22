#include "Modules/m1Resources.h"

#include "Resources/r1Texture.h"
#include "Resources/r1Model.h"
#include "Resources/r1Mesh.h"
#include "Resources/r1Tileset.h"
#include "Resources/r1Map.h"
#include "Resources/r1Object.h"
#include "Resources/r1Tileset3d.h"

#include "Tools/FileSystem.h"

#include "ExternalTools/DevIL/include/IL/il.h"
#include "ExternalTools/DevIL/include/IL/ilu.h"
#include "ExternalTools/DevIL/include/IL/ilut.h"

#include "Tools/System/Logger.h"

#include "Tools/FileWatch.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Resources::m1Resources(bool start_enabled) : Module("Resources", start_enabled)
{
}

m1Resources::~m1Resources()
{
}

bool m1Resources::Start()
{
	PROFILE_FUNCTION();
	ilInit();

	glEnable(GL_TEXTURE_2D);
	ilutRenderer(ILUT_OPENGL);

	StartFileWatcher();

	GenerateLibrary();
	LoadEngineResources();

	return true;
}

bool m1Resources::CleanUp()
{
	PROFILE_FUNCTION();
	delete filewatch;

	for (auto i = resources.begin(); i != resources.end(); ++i) {
		delete (*i).second;
	}
	resources.clear();

	for (auto i = engine_resources.begin(); i != engine_resources.end(); ++i) {
		delete (*i).second;
	}
	engine_resources.clear();

	return true;
}

Uint64 m1Resources::FindByName(const char* file)
{
	PROFILE_FUNCTION();
	for (auto i = resources.begin(); i != resources.end(); ++i) {
		if ((*i).second->name.compare(file) == 0)
			return (*i).first;
	}

	LOGW("Resource %s not found", file);

	return 0ull;
}

Uint64 m1Resources::FindByPath(const char* assets_path)
{
	PROFILE_FUNCTION();

	for (auto i = resources.begin(); i != resources.end(); ++i) {
		if ((*i).second->path.compare(assets_path) == 0)
			return (*i).first;
	}

	LOGW("Resource %s not found", assets_path);

	return 0ull;
}

Resource* m1Resources::FindGet(const char* file, bool by_name)
{
	if (by_name) {
		return Get(FindByName(file));
	}
	else {
		return Get(FindByPath(file));
	}
}

Resource* m1Resources::EGet(EResourceType type) const
{
	return ((engine_resources.find(type) == engine_resources.end()) ? nullptr : engine_resources.at(type));
}

const std::map<uint64_t, Resource*> m1Resources::GetResources() const
{
	return resources;
}

FileWatch* m1Resources::GetFileWatcher() const
{
	return filewatch;
}

void m1Resources::PauseFileWatcher(bool pause)
{
	//filewatch->Pause(pause);
	LOGW("Not paused at all...");
}

Resource* m1Resources::CreateResource(Resource::Type type, const char* assets_path, const uint64_t& force_uid)
{
	Resource* ret = nullptr;

	assert((int)Resource::Type::MAX == 7);

	switch (type)
	{
	case Resource::Type::Mesh:		ret = new r1Mesh((force_uid == 0) ? Random::RandomGUID() : force_uid);		break;
	case Resource::Type::Model:		ret = new r1Model((force_uid == 0) ? Random::RandomGUID() : force_uid);	break;
	case Resource::Type::Texture:	ret = new r1Texture((force_uid == 0) ? Random::RandomGUID() : force_uid);	break;
	case Resource::Type::Tileset:	ret = new r1Tileset((force_uid == 0) ? Random::RandomGUID() : force_uid);	break;
	case Resource::Type::Tileset3d:	ret = new r1Tileset3d((force_uid == 0) ? Random::RandomGUID() : force_uid);	break;
	case Resource::Type::Map:		ret = new r1Map((force_uid == 0) ? Random::RandomGUID() : force_uid);	break;
	case Resource::Type::Object:	ret = new r1Object((force_uid == 0) ? Random::RandomGUID() : force_uid);	break;
	default:
		LOGW("Resource %i from %s could not be created, resource not setted in switch", (int)type, assets_path);
		break;
	}

	if (ret != nullptr) {
		SetResourceStrings(ret, assets_path);
		resources[ret->GetUID()] = ret;
		ret->LoadVars();
	}

	return ret;
}

Resource* m1Resources::NewResource(const char* assets_path)
{
	Resource* ret = nullptr;

	ret = CreateResource(GetTypeFromStr(FileSystem::GetFileExtension(assets_path).c_str()), assets_path, GenerateMeta(assets_path));

	return ret;
}

void m1Resources::DeleteResource(const uint64_t& uid)
{
	auto r = Get(uid);
	if (r != nullptr) {
		if (r->references > 0)
			r->Unload();

		FileSystem::fDeleteFile(r->path.c_str());
		FileSystem::fDeleteFile((r->path + ".meta").c_str());

		resources.erase(r->uid);

		delete r;
	}
}

void m1Resources::SetResourceStrings(Resource* ret, const char* assets_path)
{
	if (strcmp(assets_path, "") != 0) {
		ret->path = FileSystem::NormalizePath(assets_path);
		ret->name = FileSystem::GetNameFile(assets_path);
		ret->extension.assign(FileSystem::GetFileExtension(assets_path));
	}
}

void m1Resources::GenerateLibrary()
{
	PROFILE_FUNCTION();
	ImportFiles(FileSystem::GetPtrFolder("Assets/"));
}

void m1Resources::LoadEngineResources()
{
	PROFILE_FUNCTION();

	auto models = FileSystem::GetPtrFolder((FileSystem::sAppdata + "Configuration/EngineResources/3DModels/").c_str(), true);

	for (auto i = models->files.begin(); i != models->files.end(); ++i) {
		r1Mesh* m = new r1Mesh(0ULL);

		m->path = FileSystem::NormalizePath((models->full_path + (*i).first).c_str());
		m->name = (*i).first;
		m->extension = "mesh";
		
		if (m->name.compare("Tile.mesh") == 0) {
			engine_resources[m1Resources::EResourceType::TILE] = m;
		}
		else {
			delete m;
		}
	}

	auto textures = FileSystem::GetPtrFolder((FileSystem::sAppdata + "Configuration/EngineResources/Textures/").c_str(), true);

	for (auto i = textures->files.begin(); i != textures->files.end(); ++i) {
		r1Texture* t = new r1Texture(0ULL);

		t->path = FileSystem::NormalizePath(textures->full_path + (*i).first);
		t->name = (*i).first;
		t->extension = "png";

		if (t->name.compare("Folder.png") == 0) {
			engine_resources[m1Resources::EResourceType::FOLDER] = t;
		}
		else if (t->name.compare("PNG.png") == 0) {
			engine_resources[m1Resources::EResourceType::PNG] = t;
		}
		else if (t->name.compare("Unknown.png") == 0) {
			engine_resources[m1Resources::EResourceType::UNKNOWN] = t;
		}
		else if (t->name.compare("FBX.png") == 0) {
			engine_resources[m1Resources::EResourceType::FBX] = t;
		}
		else if (t->name.compare("Map.png") == 0) {
			engine_resources[m1Resources::EResourceType::MAP] = t;
		}
		else if (t->name.compare("Tileset.png") == 0) {
			engine_resources[m1Resources::EResourceType::TILESET] = t;
		}
		else {
			LOGW("Asset %s not set on engine resource", t->path.c_str());
			delete t;
		}
	}
	
}

void m1Resources::ImportFiles(const Folder* parent)
{
	PROFILE_FUNCTION();
	for (auto dir = parent->folders.begin(); dir != parent->folders.end(); ++dir) { //TODO: DO IT ITERATIVE
		ImportFiles(*dir);
	}
	
	for (auto file = parent->files.begin(); file != parent->files.end(); ++file) {
		if (FileSystem::GetFileExtension((*file).first.c_str()).compare("meta") != 0) {
			if (FileSystem::Exists((parent->full_path + (*file).first + ".meta").c_str())) { // Meta already exist
				nlohmann::json meta = FileSystem::OpenJSONFile((parent->full_path + (*file).first + ".meta").c_str());
				uint64_t timestamp = FileSystem::LastTimeWrite((parent->full_path + (*file).first).c_str());
				std::string extension = meta.value("extension", "none");

				auto res = CreateResource(
					GetTypeFromStr(extension.c_str()),
					(parent->full_path + (*file).first).c_str(),
					meta.value("UID", 0ULL));

				if (meta.value("timestamp", 0ULL) != timestamp) {									// file updated
					meta["timestamp"] = timestamp;
					FileSystem::SaveJSONFile((parent->full_path + (*file).first + ".meta").c_str(), meta);
					res->UpdateFiles();
				}
			}
			else {
				if (auto res = CreateResource(
					GetTypeFromStr(FileSystem::GetFileExtension((*file).first.c_str()).c_str()),	//type
					(parent->full_path + (*file).first).c_str(),									//path
					GenerateMeta((parent->full_path + (*file).first).c_str())						//meta
				))
					res->GenerateFiles();
			}
		}
	}
}

void m1Resources::StartFileWatcher()
{
	filewatch = new FileWatch();
	filewatch->Subscribe("Assets/"); //TODO: don't create events when own program save a file

	//filewatch->StartWatching();
}

uint64_t m1Resources::GenerateMeta(const char* file)
{
	PauseFileWatcher(true);

	nlohmann::json meta;

	uint64_t uid = Random::RandomGUID();
	meta["UID"] = uid;
	meta["timestamp"] = FileSystem::LastTimeWrite(file);
	meta["extension"] = FileSystem::GetFileExtension(file);
	meta["properties"] = nlohmann::json::object();

	FileSystem::SaveJSONFile((file + std::string(".meta")).c_str(), meta);

	PauseFileWatcher(false);

	return uid;
}

void m1Resources::ReimportResource(const char* file)
{
	Resource* r = FindGet(file, false);
	if (r) {
		if (r->references > 0) {
			r->Unload();
			r->LoadVars();
			r->Load();
		}
	}
}



std::vector<Resource*> m1Resources::GetVectorOf(Resource::Type type)
{
	std::vector<Resource*> ret;

	for (auto i = resources.begin(); i != resources.end(); ++i)
		if ((*i).second->type == type)
			ret.push_back((*i).second);

	return ret;
}

std::vector<r1Texture*> m1Resources::GetVectorOfTextures() const
{
	std::vector<r1Texture*> ret;

	for (auto i = resources.begin(); i != resources.end(); ++i)
		if ((*i).second->type == Resource::Type::Texture)
			ret.push_back((r1Texture*)(*i).second);

	return ret;
}

Resource::Type m1Resources::GetTypeFromStr(const char* type) const
{
	assert((int)Resource::Type::MAX == 7);
	if (strcmp(type, "fbx") == 0 || strcmp(type, "obj") == 0)
		return Resource::Type::Model;
	else if (strcmp(type, "png") == 0)
		return Resource::Type::Texture;
	else if (strcmp(type, "tileset") == 0)
		return Resource::Type::Tileset;
	else if (strcmp(type, "tileset3d") == 0)
		return Resource::Type::Tileset3d;
	else if (strcmp(type, "scene") == 0)
		return Resource::Type::Map;
	else if (strcmp(type, "object") == 0)
		return Resource::Type::Object;

	LOGW("No library path found to type %s", type)

	return Resource::Type::NONE;
}
