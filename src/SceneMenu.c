#include "SceneMenu.h"
#include "Application.h"
#include "GLFW/glfw3.h"
#include "Scene.h"
#include "UI-Imgui.h"

#include "Window.h"
#include "cimgui.h"

#include <math.h>
#include <stb_ds.h>

static vec4s background_color = (vec4s){0.0f, 0.0f, 0.0f, 1.0f};

static ImGuiIO* imgui_io = NULL;

static void init()
{
	imgui_io = ImGui_GetIO();
}

static void render()
{
	const float frequency = 0.5f;
	background_color.b = 0.5f + 0.5f * sin(frequency * glfwGetTime());
	background_color.r = 0.5f + 0.5f * sin(frequency * glfwGetTime() + 2.0f * M_PI / 3.0f);
	background_color.g = 0.5f + 0.5f * sin(frequency * glfwGetTime() + 4.0f * M_PI / 3.0f);
	change_window_color(background_color);

	ImGui_Begin("Menu", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
	{
		ImGui_SetWindowPos((ImVec2){imgui_io->DisplaySize.x * 0.4, 0.0f}, ImGuiCond_Always);
		ImGui_SetWindowSize(imgui_io->DisplaySize, ImGuiCond_Always);

		ImGui_BeginChild("Title", (ImVec2){300.0f, 100.0f}, 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
		{
			ImGui_PushFont(ui_get_font("Triple"));
			ImGui_Text(WINDOW_TITLE);
			ImGui_PopFont();
		}
		ImGui_EndChild();


		ImGui_SetCursorPosX(ImGui_GetCursorPosX() + 50.0f);
		ImGui_BeginChild("Scenes", (ImVec2){300.0f, 500.0f}, 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
		{
			ImGui_SetCursorPosX(ImGui_GetCursorPosX() + 30.0f);
			ImGui_PushFont(ui_get_font("Double"));
			ImGui_Text("Scenes");

			for(int i = 0; i < shlen(game_engine.scenes); i++)
			{
				if(strcmp(game_engine.scenes[i].key, "SceneMenu") == 0)
				{
					continue;
				}

				if(ImGui_Button(game_engine.scenes[i].key))
				{
					change_scene(game_engine.scenes[i].key);
				}
			}
			ImGui_PopFont();
		}
		ImGui_EndChild();
	}
	ImGui_End();
}

static void update()
{
}

static void process_input()
{
}

static void activate()
{
}

static void deactivate()
{
}

static void destroy()
{
}


Scene SceneMenu = {"SceneMenu", init, destroy, activate, deactivate, update, render, process_input};
