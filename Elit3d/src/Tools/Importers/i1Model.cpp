#include "Tools/Importers/i1Model.h"
#include "Core/Application.h"
#include "Tools/FileSystem.h"
#include "Modules/m1Resources.h"
#include "Resources/r1Model.h"

i1Model::i1Model()
{
}

i1Model::~i1Model()
{
}

void i1Model::Import(const char* path)
{
	if (!FileSystem::Exists(ASSETS_MODELS_PATH)) {
		FileSystem::CreateFolder(ASSETS_MODELS_PATH);
	}

	std::string assets_file; 

	/*if (FileSystem::IsFileInFolder(FileSystem::GetNameFile(path).c_str(), ASSETS_MODELS_PATH, true)) {
		assets_file.assign(ASSETS_MODELS_PATH + FileSystem::GetNameFile(path) + "(1)" + FileSystem::GetFileExtension(path, true)); //TODO ADD number of instancies if there is already a (1)
	}
	else
		assets_file.assign(ASSETS_MODELS_PATH + FileSystem::GetNameFile(path, true));*/

	if (FileSystem::CopyTo(path, assets_file.c_str())) {
		uint64_t meta = App->resources->GenerateMeta(assets_file.c_str());
		r1Model* model = (r1Model*)App->resources->CreateResource(Resource::Type::Model, assets_file.c_str(), meta);
		
		//model->GenerateFiles();
	}
}
