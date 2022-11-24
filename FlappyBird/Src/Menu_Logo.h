#pragma once

class Menu_Logo
{
private:
	bool init = false; //should be set to false whenever you switch menus
	
	Timer switch_screens_timer;
	const int logo_display_time = 1000; //milliseconds
public:
	void Update(MENU_TYPE& current_menu, Menu_Main& main_menu)
	{
		if (!init)
		{
			switch_screens_timer.Begin();
			init = true;
		}

		//LOGIC
		if (switch_screens_timer.TimeElapsed(logo_display_time))
		{
			current_menu = MENU_TYPE::MAIN;
			main_menu.Init();
			init = false;
			return;
		}
	}

	void Render(float a_blend)
	{
		RenderManager.RenderRect(texture_logomenu, glm::mat4(1.0f));
		checkError("end of menucompanylogo draw commands");
	}
};