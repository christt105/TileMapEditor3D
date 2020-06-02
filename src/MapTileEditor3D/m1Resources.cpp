#include "m1Resources.h"

#include "r1Texture.h"
#include "r1Model.h"
#include "r1Mesh.h"
#include "r1Tileset.h"

#include "FileSystem.h"

#include "ExternalTools/DevIL/il.h"
#include "ExternalTools/DevIL/ilu.h"
#include "ExternalTools/DevIL/ilut.h"

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

#pragma comment(lib, "ExternalTools/DevIL/libx86/DevIL.lib")
#pragma comment(lib, "ExternalTools/DevIL/libx86/ILU.lib")
#pragma comment(lib, "ExternalTools/DevIL/libx86/ILUT.lib")

m1Resources::m1Resources(bool start_enabled) : Module("Resources", start_enabled)
{
}

m1Resources::~m1Resources()
{
}

bool m1Resources::Start()
{
	ilInit();

	if (!App->file_system->Exists(LIBRARY_PATH))
		App->file_system->CreateFolder(LIBRARY_PATH);

	if (!App->file_system->Exists(LIBRARY_TEXTURES_PATH))
		App->file_system->CreateFolder(LIBRARY_TEXTURES_PATH);

	if (!App->file_system->Exists(LIBRARY_MODELS_PATH))
		App->file_system->CreateFolder(LIBRARY_MODELS_PATH);

	if (!App->file_system->Exists(LIBRARY_MESHES_PATH))
		App->file_system->CreateFolder(LIBRARY_MESHES_PATH);

	if (!App->file_system->Exists(LIBRARY_TILESETS_PATH))
		App->file_system->CreateFolder(LIBRARY_TILESETS_PATH);

	GenerateLibrary();

	return true;
}

bool m1Resources::CleanUp()
{
	for (auto i = resources.begin(); i != resources.end(); ++i) {
		delete (*i).second;
	}
	resources.clear();

	return true;
}

Uint64 m1Resources::Find(const char* file)
{
	for (auto i = resources.begin(); i != resources.end(); ++i) {
		if ((*i).second->name.compare(file) == 0)
			return (*i).first;
	}

	LOGW("Resource %s not found", file);

	return 0ull;
}

Resource* m1Resources::Get(const Uint64& uid) const
{
	auto ret = resources.find(uid);
	return (ret == resources.end()) ? nullptr : (*ret).second;
}

Resource* m1Resources::CreateResource(Resource::Type type, const char* assets_path, const uint64_t& force_uid)
{
	Resource* ret = nullptr;

	switch (type)
	{
	case Resource::Type::Mesh:		ret = new r1Mesh((force_uid == 0) ? App->random->RandomGUID() : force_uid);		break;
	case Resource::Type::Model:		ret = new r1Model((force_uid == 0) ? App->random->RandomGUID() : force_uid);	break;
	case Resource::Type::Texture:	ret = new r1Texture((force_uid == 0) ? App->random->RandomGUID() : force_uid);	break;
	case Resource::Type::Tileset:	ret = new r1Tileset((force_uid == 0) ? App->random->RandomGUID() : force_uid);	break;
	default:
		LOGW("Resource %i from %s could not be created, resource not setted in switch", (int)type, assets_path);
		break;
	}

	if (ret != nullptr) {
		SetResourceStrings(ret, assets_path);
		resources[ret->GetUID()] = ret;
	}

	return ret;
}

void m1Resources::SetResourceStrings(Resource* ret, const char* assets_path)
{
	if (strcmp(assets_path, "") != 0) {
		ret->assets_path.assign(assets_path);
		ret->name = App->file_system->GetNameFile(assets_path);
		ret->extension.assign(App->file_system->GetFileExtension(assets_path));
	}
	ret->library_path.assign(GetLibraryFromType(ret->GetType()) + std::to_string(ret->GetUID()) + GetLibraryExtensionFromType(ret->GetType()));
}

void m1Resources::GenerateLibrary()
{
	ImportFiles(App->file_system->GetFilesRecursive("Assets/"));
}

void m1Resources::ImportFiles(const Folder& parent)
{
	for (auto dir = parent.folders.begin(); dir != parent.folders.end(); ++dir) {
		ImportFiles(*dir);
	}
	
	for (auto file = parent.files.begin(); file != parent.files.end(); ++file) {
		if (App->file_system->GetFileExtension((*file).c_str()).compare("meta") != 0) {
			if (App->file_system->Exists((parent.full_path + *file + ".meta").c_str())) {
				nlohmann::json meta = App->file_system->OpenJSONFile((parent.full_path + *file + ".meta").c_str());
				std::string extension = meta.value("extension", "none");
				if (meta.value("timestamp", 0ULL) == App->file_system->LastTimeWrite((parent.full_path + *file).c_str())) {
					if (App->file_system->Exists((GetLibraryFromType(extension.c_str()) + std::to_string(meta.value("UID", 0ULL)) + GetLibraryExtension(extension.c_str())).c_str())) {
						Resource* res = CreateResource(GetTypeFromStr(extension.c_str()), (parent.full_path + *file).c_str(), meta.value("UID", 0ULL));
						res->GenerateFilesLibrary();
					}
					else {
						Resource* res = CreateResource(GetTypeFromStr(extension.c_str()), (parent.full_path + *file).c_str(), meta.value("UID", 0ULL));

						res->GenerateFiles();
					}
				}
				else {
					DeleteFromLibrary(GetTypeFromStr(extension.c_str()), meta.value("UID", 0ULL));

					meta["timestamp"] = App->file_system->LastTimeWrite((parent.full_path + *file).c_str());
					App->file_system->SaveJSONFile((parent.full_path + *file + ".meta").c_str(), meta);

					Resource* res = CreateResource(GetTypeFromStr(extension.c_str()), (parent.full_path + *file).c_str(), meta.value("UID", 0ULL));

					res->GenerateFiles();
				}
			}
			else {
				uint64_t meta = GenerateMeta((parent.full_path + *file).c_str());

				Resource* res = CreateResource(GetTypeFromStr(App->file_system->GetFileExtension((*file).c_str()).c_str()), (parent.full_path + *file).c_str(), meta);
				if (res != nullptr)
					res->GenerateFiles();
			}
		}
	}
}

uint64_t m1Resources::GenerateMeta(const char* file)
{
	nlohmann::json meta;

	uint64_t uid = App->random->RandomGUID();
	meta["UID"] = uid;
	meta["timestamp"] = App->file_system->LastTimeWrite(file);
	meta["extension"] = App->file_system->GetFileExtension(file);

	App->file_system->SaveJSONFile((file + std::string(".meta")).c_str(), meta);

	return uid;
}

std::vector<Resource*> m1Resources::GetVectorOf(Resource::Type type)
{
	std::vector<Resource*> ret;

	for (auto i = resources.begin(); i != resources.end(); ++i)
		if ((*i).second->type == type)
			ret.push_back((*i).second);

	return ret;
}

const char* m1Resources::GetLibraryFromType(const char* type)
{
	if(strcmp(type, "fbx") == 0)
		return LIBRARY_MODELS_PATH;
	else if(strcmp(type, "jpg") == 0 || strcmp(type, "png") == 0)
		return LIBRARY_TEXTURES_PATH;
	else if (strcmp(type, "tileset") == 0)
		return LIBRARY_TILESETS_PATH;

	LOGW("No library path found to type %i", (int)type);

	return "";
}

const char* m1Resources::GetLibraryFromType(Resource::Type type)
{
	switch (type)
	{
	case Resource::Type::Mesh:
		return LIBRARY_MESHES_PATH;
	case Resource::Type::Model:
		return LIBRARY_MODELS_PATH;
	case Resource::Type::Texture:
		return LIBRARY_TEXTURES_PATH;
	case Resource::Type::Tileset:
		return LIBRARY_TILESETS_PATH;
	default:
		break;
	}

	LOGW("No library path found to type %i", (int)type);

	return "";
}

std::string m1Resources::GetLibraryExtension(const char* type)
{
	if (strcmp(type, "fbx") == 0)
		return ".model";
	else if (strcmp(type, "jpg") == 0 || strcmp(type, "png") == 0)
		return ".texture";

	return std::string(std::string(".") + type);
}

std::string m1Resources::GetLibraryExtensionFromType(Resource::Type type)
{
	switch (type)
	{
	case Resource::Type::Mesh:
		return ".mesh";
	case Resource::Type::Model:
		return ".model";
	case Resource::Type::Texture:
		return ".texture";
	case Resource::Type::Tileset:
		return ".tileset";
	default:
		break;
	}

	LOGW("No library path found to type %i", (int)type);

	return "";
}

Resource::Type m1Resources::GetTypeFromStr(const char* type)
{
	if (strcmp(type, "fbx") == 0)
		return Resource::Type::Model;
	else if (strcmp(type, "png") == 0)
		return Resource::Type::Texture;
	else if (strcmp(type, "tileset") == 0)
		return Resource::Type::Tileset;

	LOGW("No library path found to type %i", (int)type);

	return Resource::Type::NONE;
}

void m1Resources::DeleteFromLibrary(Resource::Type type, const uint64_t& meta)
{
	switch (type)
	{
	case Resource::Type::Model:
		DeleteMeshes(meta);
		App->file_system->fDeleteFile((LIBRARY_MODELS_PATH + std::to_string(meta) + ".model").c_str());
		break;
	case Resource::Type::Texture:
	case Resource::Type::Tileset:
		App->file_system->fDeleteFile((GetLibraryFromType(type) + std::to_string(meta) + GetLibraryExtensionFromType(type)).c_str());
		break;
	default:
		LOGW("Resource type %i could not be deleted, type not setted in switch", (int)type);
		break;
	}
}

void m1Resources::DeleteMeshes(const uint64_t& meta)
{
	auto model = App->file_system->OpenJSONFile((LIBRARY_MODELS_PATH + std::to_string(meta) + ".model").c_str());
	for (auto i = model["Meshes"].begin(); i != model["Meshes"].end(); ++i)
		App->file_system->fDeleteFile((LIBRARY_MESHES_PATH + std::to_string((uint64_t)*i) + ".mesh").c_str());
}
