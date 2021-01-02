#include "m1Scene.h"

#include "Application.h"
#include "m1Input.h"
#include "m1Camera3D.h"

#include "OpenGLHelper.h"

#include "m1Render3D.h"
#include "r1Shader.h"

#include "m1GUI.h"
#include "p1Scene.h"
#include "Viewport.h"

#include "m1MapEditor.h"

#include "Logger.h"

#include "Profiler.h"

#include "ExternalTools/mmgr/mmgr.h"

m1Scene::m1Scene(bool start_enabled) : Module("Scene", start_enabled)
{
}

m1Scene::~m1Scene()
{
}

bool m1Scene::Init(const nlohmann::json& node)
{
	return true;
}

bool m1Scene::Start()
{
	PROFILE_FUNCTION();

	panel_scene = App->gui->scene;

	return true;
}

UpdateStatus m1Scene::Update()
{
	PROFILE_FUNCTION();

	App->render->GetViewport("scene")->Begin();

	if (draw_grid) {
		static auto shader1 = App->render->GetShader("grid");
		shader1->Use();
		oglh::DrawArrays(6);
	}

	static auto shader = App->render->GetShader("default");
	shader->Use();
	shader->SetMat4("model", float4x4::identity);

	static float3 xd0 = float3::zero;
	static float3 xd1 = float3::one;
	if (panel_scene->IsOnHover()) {
		int2 mouse_panel = panel_scene->GetMousePosition();
		int2 size_panel = panel_scene->GetSize();
		mouse_panel.y = size_panel.y - mouse_panel.y;
		float2 mouse_perc(2 * ((float)mouse_panel.x) / ((float)size_panel.x) - 1, 2 * ((float)mouse_panel.y) / ((float)size_panel.y) - 1);

		auto ray = App->camera->frustum.UnProject(mouse_perc);
		App->map_editor->Mouse(ray);
		shader->Use();

		if (App->input->IsMouseButtonPressed(1)) {
			xd0 = ray.pos;
			xd1 = ray.pos + ray.dir * 250.f;
		}
	}

	if (App->debug.draw_mouse_pick_line) {
		oglh::OldDrawLines(xd0, xd1);
	}

	App->render->GetViewport("scene")->End();
	
	return UpdateStatus::UPDATE_CONTINUE;
}

bool m1Scene::CleanUp()
{
	PROFILE_FUNCTION();

	return true;
}
