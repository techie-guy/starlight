#include "ui_imgui.h"
#include "../core/game_engine.h"

#include "cimgui.h"
#include "cimgui_impl_glfw.h"
#include "cimgui_impl_opengl3.h"

#include <string.h>

#include <stb_ds.h>

static ImGuiIO* imgui_io;
static struct { char* key; ImFont* value; }* fonts = NULL;

void init_imgui(const char* font_path, int font_size, const char* chars_add, const char* glsl_version)
{
	ImGui_CreateContext(NULL);
    imgui_io = ImGui_GetIO();

	imgui_io->IniFilename = "/data/data/io.github.techieguy.mistlib/files/imgui.ini";

	ImVector_ImWchar ranges;
	ImVector_Construct(&ranges);
	ImFontGlyphRangesBuilder builder;
	memset(&builder, 0, sizeof(builder));

	ImFontGlyphRangesBuilder_Clear(&builder);
	ImFontGlyphRangesBuilder_AddRanges(&builder, ImFontAtlas_GetGlyphRangesDefault(imgui_io->Fonts));
	ImFontGlyphRangesBuilder_AddText(&builder, chars_add, NULL);
	ImFontGlyphRangesBuilder_BuildRanges(&builder, &ranges);
	
	shput(fonts, "Normal", ImFontAtlas_AddFontFromFileTTF(imgui_io->Fonts, font_path, font_size, NULL, ranges.Data));
	shput(fonts, "Double", ImFontAtlas_AddFontFromFileTTF(imgui_io->Fonts, font_path, font_size * 2.0f, NULL, ranges.Data));
	shput(fonts, "Triple", ImFontAtlas_AddFontFromFileTTF(imgui_io->Fonts, font_path, font_size * 3.0f, NULL, ranges.Data));

	ImFontAtlas_Build(imgui_io->Fonts);

	ImVector_Destruct(&ranges);

	cImGui_ImplGlfw_InitForOpenGL(game_engine.current_window.handle, true);
	cImGui_ImplOpenGL3_InitEx(glsl_version);
}

ImFont* ui_get_font(const char* name)
{
	return shget(fonts, name);
}

void new_imgui_frame()
{
	cImGui_ImplOpenGL3_NewFrame();
	cImGui_ImplGlfw_NewFrame();
	ImGui_NewFrame();
}

void render_imgui()
{
	ImGui_Render();
	cImGui_ImplOpenGL3_RenderDrawData(ImGui_GetDrawData());
}

void ui_component_joystick(const char* joystick_box_tag, const char* joystick_circle_tag, ImVec2 joystick_box_size, float joystick_radius, ImVec4 joystick_color, float* joystick_angle, bool* is_joystick_active)
{
	ImVec2 joystick_center = (ImVec2){ImGui_GetWindowPos().x + joystick_box_size.x/2, ImGui_GetWindowPos().y + joystick_box_size.y/2};

	ImGui_Begin(joystick_box_tag, NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
	{
		static ImVec2 joystick_circle_pos;
		ImGui_SetWindowSize(joystick_box_size, ImGuiCond_Always);
		ImGui_SetWindowPos((ImVec2){50, ImGui_GetIO()->DisplaySize.y - joystick_box_size.y*1.2}, ImGuiCond_Always);

		ImDrawList_AddCircleFilled(ImGui_GetWindowDrawList(), joystick_circle_pos, joystick_radius, ImGui_ColorConvertFloat4ToU32(joystick_color), 32);
		ImGui_InvisibleButton(joystick_circle_tag, (ImVec2){joystick_radius*4, joystick_radius*4}, ImGuiButtonFlags_MouseButtonLeft);
		ImGui_SetCursorPos(joystick_circle_pos);
		if(ImGui_IsItemActive() && ImGui_IsMouseDragging(0, -1.0f))
		{
			*is_joystick_active = true;

			joystick_color = (ImVec4){0.0f, 225.0f, 225.0f, 225.0f};
			joystick_circle_pos = imgui_io->MousePos;
			vec2s mouse_drag_delta = (vec2s){ImGui_GetMouseDragDelta(0, -1.0f).x, ImGui_GetMouseDragDelta(0, -1.0f).y};

			// Bad workaround
			float dot_product_by_norm_x = glms_vec2_dot((vec2s){1.0f, 0.0f}, mouse_drag_delta)/glms_vec2_norm(mouse_drag_delta);
			float dot_product_by_norm_y = glms_vec2_dot((vec2s){0.0f, -1.0f}, mouse_drag_delta)/glms_vec2_norm(mouse_drag_delta);
			
			if(dot_product_by_norm_y > 0.0f) // 1st or 2nd Quadrant
			{
				*joystick_angle = glm_deg(acosf(dot_product_by_norm_x));
			}
			else // 3rd or 4th Quadrant
			{
				*joystick_angle = glm_deg(2*M_PI - acosf(dot_product_by_norm_x));
			}	
		}
		else
		{
			*is_joystick_active = false;
			joystick_color = (ImVec4){225.0f, 225.0f, 225.0f, 225.0f};
			joystick_circle_pos = (ImVec2){ImGui_GetWindowPos().x + joystick_box_size.x/2, ImGui_GetWindowPos().y + joystick_box_size.y/2};;
		}
	}
	ImGui_End();

}

void destroy_imgui()
{
	cImGui_ImplOpenGL3_Shutdown();
	cImGui_ImplGlfw_Shutdown();
	ImGui_DestroyContext(NULL);

}
