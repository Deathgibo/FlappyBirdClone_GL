#pragma once

class Menu_Score
{
private:
	bool init = false; //should be set to false whenever you switch menus
	
	Rect rect_top;
	Rect rect_recent;
	Rect button_scoremenu;
	std::array<Digits, 3> digits_topscore;
	std::array<Digits, 5> digits_recentscores;
	std::array<Rect, 8> rect_scoreranks;
	std::array<Rect, 8> rects_dots;
	std::array<int, 8> scores;
public:
	Menu_Score()
	{
		rect_top.center = glm::vec2(.5, .05);
		rect_top.dimensions = glm::vec2(.35, .1);

		rect_recent.center = glm::vec2(.5, .45);
		rect_recent.dimensions = glm::vec2(.35, .1);

		button_scoremenu.center = glm::vec2(.5, .95);
		button_scoremenu.dimensions = glm::vec2(.2, .05);

		//button_scoremenu.center = glm::vec2(0.288, 0.81);
		button_scoremenu.dimensions = glm::vec2(.274, .0723);

		for (int i = 0; i < digits_topscore.size(); i++)
		{
			digits_topscore[i].dim = glm::vec2(.035, .035);
			digits_topscore[i].center = glm::vec2(.5, .2 + .1*i - .05);
		}

		for (int i = 0; i < digits_recentscores.size(); i++)
		{
			digits_recentscores[i].dim = glm::vec2(.035, .035);
			digits_recentscores[i].center = glm::vec2(.5, .6 + .075*i - .05);
		}

		for (int i = 0; i < rect_scoreranks.size(); i++)
		{
			if (i < 3)
			{
				rect_scoreranks[i].dimensions = glm::vec2(.035, .035);
				rect_scoreranks[i].center = glm::vec2(.25, .2 + .1*i - .05);
			}
			else
			{
				rect_scoreranks[i].dimensions = glm::vec2(.035, .035);
				rect_scoreranks[i].center = glm::vec2(.25, .6 + .075*(i - 3) - .05);
			}
		}

		for (int i = 0; i < rects_dots.size(); i++)
		{
			if (i < 3)
			{
				rects_dots[i].dimensions = glm::vec2(.01, .01);
				rects_dots[i].center = glm::vec2(.273, .2 + .1*i - .05);
			}
			else
			{
				rects_dots[i].dimensions = glm::vec2(.01, .01);
				rects_dots[i].center = glm::vec2(.273, .6 + .075*(i - 3) - .05);
			}
			rects_dots[i].center += glm::vec2(0.0, 0.0125);
		}
	}

	void Init()
	{
		if (!init)
		{
			//retrieve scores from file
			scores = ReadStatsFile();
			init = true;
		}
	}

	void Input(std::pair<bool, MENU_TYPE>& transition_info)
	{
		if (ClickedButton(button_scoremenu, InputManager.LeftClick()))
		{
			transition_info.first = true;
			transition_info.second = MENU_TYPE::MAIN;
			init = false;
			return;
		}
	}

	void Update()
	{
	}

	void Render(float a_blend)
	{
		//BACKGROUND
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1.34, 1));
		RenderManager.RenderRect(texture_background, scale);

		//TOP SCORES
		RenderManager.RenderRect(texture_top, rect_top.GetMatrix());

		for (int i = 0; i < digits_topscore.size(); i++)
		{
			digits_topscore[i].render(scores[i]);
		}

		RenderManager.RenderRect(texture_numbers[1], rect_scoreranks[0].GetMatrix());
		RenderManager.RenderRect(texture_numbers[2], rect_scoreranks[1].GetMatrix());
		RenderManager.RenderRect(texture_numbers[3], rect_scoreranks[2].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[0].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[1].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[2].GetMatrix());

		//MOST RECENT
		RenderManager.RenderRect(texture_recent, rect_recent.GetMatrix());

		for (int i = 0; i < digits_recentscores.size(); i++)
		{
			digits_recentscores[i].render(scores[i + 3]);
		}

		RenderManager.RenderRect(texture_numbers[1], rect_scoreranks[3].GetMatrix());
		RenderManager.RenderRect(texture_numbers[2], rect_scoreranks[4].GetMatrix());
		RenderManager.RenderRect(texture_numbers[3], rect_scoreranks[5].GetMatrix());
		RenderManager.RenderRect(texture_numbers[4], rect_scoreranks[6].GetMatrix());
		RenderManager.RenderRect(texture_numbers[5], rect_scoreranks[7].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[3].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[4].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[5].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[6].GetMatrix());
		RenderManager.RenderRect(texture_dot, rects_dots[7].GetMatrix());

		RenderManager.RenderRect(texture_menu, button_scoremenu.GetMatrix());

		checkError("end of stats draw commands");
	}
};