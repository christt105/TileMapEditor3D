#include "Modules/m1Events.h"
#include "Tools/System/Logger.h"

#include "Core/Application.h"

#include "Modules/m1Resources.h"
#include "Tools/FileWatch.h"

#include "Modules/m1MapEditor.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"

#include "Tools/System/Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Events::m1Events() : Module("Events", true)
{
}

m1Events::~m1Events()
{
}

UpdateStatus m1Events::PreUpdate()
{
	PROFILE_FUNCTION();
	//std::unique_lock<std::mutex> lock(App->resources->GetFileWatcher()->mtx);
	while (!events.empty()) {
		auto e = events.top();
		switch (e->type)
		{
		case Event::Type::FILE_CREATED:
			// Generate metadata and generate library
			LOG("File %s created", ((sTypeVar*)e->info["basic_info"])->value.c_str());
			if (FileSystem::GetFileExtension(((sTypeVar*)e->info["basic_info"])->value.c_str()).compare("meta") != 0)
				App->resources->NewResource(((sTypeVar*)e->info["basic_info"])->value.c_str());
			break;
		case Event::Type::FILE_MOVED:
		{
			// Check if metadata is within and change resource assets path
			LOG("File %s moved", ((sTypeVar*)e->info["to"])->value.c_str());
			uint64_t r = App->resources->FindByPath(((sTypeVar*)(e->info["from"]))->value.c_str());
			if (r != 0ULL) {
				Resource* res = App->resources->Get(r);
				if (res != nullptr) {
					res->path.assign(((sTypeVar*)(e->info["to"]))->value);
				}
				else
					LOGE("FileWatcher: resource with id %ull not found to move", r);
			}
			else {
				LOGE("FileWatcher: resource with path %s not found to move", ((sTypeVar*)(e->info["from"]))->value.c_str());
			}
			break;
		}
		case Event::Type::FILE_REMOVED:
			// Remove from library and from resources map
			LOG("File %s removed", ((sTypeVar*)e->info["basic_info"])->value.c_str());
			App->resources->DeleteResource(App->resources->FindByPath(((sTypeVar*)e->info["basic_info"])->value.c_str()));
			break;
		case Event::Type::FILE_RENAMED:
		{
			//Change metadata name file and resources assets path
			LOG("File %s renamed", ((sTypeVar*)(e->info["to"]))->value.c_str());
			uint64_t r = App->resources->FindByPath(((sTypeVar*)(e->info["from"]))->value.c_str());
			if (r != 0ULL) {
				Resource* res = App->resources->Get(r);
				if (res != nullptr) {
					res->path.assign(((sTypeVar*)(e->info["to"]))->value);
				}
				else
					LOGE("FileWatcher: resource with id %ull not found to move", r);
			}
			else {
				LOGE("FileWatcher: resource with path %s not found to move", ((sTypeVar*)(e->info["from"]))->value.c_str());
			}
			break;
		}
		case Event::Type::FILE_MODIFIED:
			App->resources->ReimportResource(((sTypeVar*)e->info["basic_info"])->value.c_str());
			break;
		case Event::Type::FOLDER_CREATED:
			//Check for resources inside
			LOG("Folder %s created", ((sTypeVar*)e->info["basic_info"])->value.c_str());
			break;
		case Event::Type::FOLDER_MOVED:
			// Change resources assets path
			LOG("Folder %s moved", ((sTypeVar*)e->info["to"])->value.c_str());
			break;
		case Event::Type::FOLDER_REMOVED:
			// delete all resources inside folder
			LOG("Folder %s removed", ((sTypeVar*)e->info["basic_info"])->value.c_str());
			break;

		case Event::Type::DELETE_OBJ:
			delete (Object*)e->info["basic_info"]->iGetValue();
			break;

		case Event::Type::RESIZE_MAP:
			App->map_editor->ResizeMap(((iTypeVar*)e->info["width"])->value, ((iTypeVar*)e->info["height"])->value);
			break;
		case Event::Type::REORDER_LAYERS:
			App->map_editor->ReorderLayers();
			break;
		case Event::Type::SAVE_MAP:
			App->map_editor->SaveMap();
			break;
		case Event::Type::EXPORT_MAP: {
			m1MapEditor::MapTypeExport t = (m1MapEditor::MapTypeExport)(e->info["basic_info"]->iGetValue());
			switch (t)
			{
			case m1MapEditor::MapTypeExport::XML:
			case m1MapEditor::MapTypeExport::JSON:
				App->map_editor->ExportMap(t, (MapLayer::DataTypeExport)((iTypeVar*)e->info["datatype"])->value);
				break;
			case m1MapEditor::MapTypeExport::OBJ:
				App->map_editor->ExportMap(t, MapLayer::DataTypeExport::NONE);
				break;
			case m1MapEditor::MapTypeExport::MAX:
				break;
			}
		}
			break;
		case Event::Type::SAVE_CONFIGURATION:
			App->SaveConfiguration();
			break;
		case Event::Type::LOAD_CONFIGURATION:
			App->LoadConfiguration("prefs/Configuration.json");
			break;
		case Event::Type::LOAD_DEFAULT_CONFIGURATION:
			App->LoadConfiguration((FileSystem::sAppdata + "/Configuration/Configuration_default.json").c_str());
			break;
		default:
			LOG("Event with type %i not handled", (int)e->type);
			break;
		}
		delete e;
		events.pop();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

void m1Events::AddEvent(Event* e)
{
	//std::unique_lock<std::mutex> lock(App->resources->GetFileWatcher()->mtx);
	events.push(e);
}

m1Events::Event::Event(): type(m1Events::Event::Type::NONE)
{
}

m1Events::Event::Event(Type t) : type(t)
{
}

m1Events::Event::Event(Type t, const char* basic_info) : type(t)
{
	if (basic_info != NULL)
		info["basic_info"] = new sTypeVar(basic_info);
}

m1Events::Event::Event(Type t, int basic_info) : type(t)
{
	info["basic_info"] = new iTypeVar(basic_info);
}

m1Events::Event::~Event()
{
	for (auto i = info.begin(); i != info.end(); ++i) {
		delete (*i).second;
	}
}
