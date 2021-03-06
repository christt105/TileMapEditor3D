#include "Resources/Base/Resource.h"

#include "Core/Application.h"
#include "Tools/FileSystem.h"

#include "ExternalTools/ImGui/imgui.h"

Resource::Resource(Type type, const uint64_t& uid) : type(type), uid(uid)
{
}

Resource::~Resource()
{
}

Uint64 Resource::GetUID() const
{
	return uid;
}

void Resource::Attach()
{
	if (++references == 1u)
		Load();
}

void Resource::Detach()
{
	if (references > 0u)
		if (--references == 0u)
			Unload();
}

void Resource::OnInspector()
{
	ImGui::Text("Name: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), name.c_str());
	ImGui::Text("Type: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), GetStrType().c_str());
	ImGui::Text("Extension: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.6f, 0.6f, 1.f), extension.c_str());
}

std::string Resource::GetStrType() const
{
	assert((int)Resource::Type::MAX == 7);
	switch (type)
	{
	case Resource::Type::NONE:
		return std::string("None");
	case Resource::Type::Mesh:
		return std::string("Mesh");
	case Resource::Type::Model:
		return std::string("Model");
	case Resource::Type::Texture:
		return std::string("Texture");
	case Resource::Type::Tileset:
		return std::string("Tileset");
	case Resource::Type::Tileset3d:
		return std::string("Tileset3d");
	default:
		return std::string("None");
	}
}
