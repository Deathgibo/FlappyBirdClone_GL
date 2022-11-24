#pragma once

class Menu_Main
{
private:
	bool init = false; //should be set to false whenever you switch menus
	
	Rect button_play;
	Rect button_stats;

	Ground ground;

	Rect rect_studio;
	Rect rect_title;
	Rect rect_bird;

	const glm::vec2 title_pos = glm::vec2(.4, .35);
	const glm::vec2 bird_pos = glm::vec2(.85, .325);
	const float amplitude = 0.01f; //amplitude of title animation
	const float quickness = 0.08f; //quickness of title animation
	const float reset_value = (2*3.14159265359) / quickness;
	float counter = 0;

	Timer animation_timer;
	int animation_index;
public:
	Menu_Main()
	{
		button_play.center = glm::vec2(0.288, 0.81);
		button_play.dimensions = glm::vec2(.274, .0723);

		button_stats.center = glm::vec2(0.712, 0.81);
		button_stats.dimensions = glm::vec2(.274, .0723);

		rect_title.center = title_pos;
		rect_title.dimensions = glm::vec2(.7, .15);

		rect_bird.center = bird_pos;
		rect_bird.dimensions = glm::vec2(.118, .0648);

		rect_studio.center = glm::vec2(0.5, 0.94);
		rect_studio.dimensions = glm::vec2(.70, .04);
	}
	
	void Init()
	{
		if (!init)
		{
			//write to gl error file
			GLErrorFileWrite();
			
			//only reason I call this is to free up memory
			AudioManager.stopAllSound();

			counter = 0;
			rect_title.SetX(title_pos.x);
			rect_title.SetY(title_pos.y);

			rect_bird.SetX(bird_pos.x);
			rect_bird.SetY(bird_pos.y);

			animation_index = 0;
			animation_timer.Begin();

			init = true;
		}
	}

	void Input(std::pair<bool, MENU_TYPE>& transition_info)
	{
		//BUTTONS
		if (ClickedButton(button_play, InputManager.LeftClick()))
		{
			transition_info.first = true;
			transition_info.second = MENU_TYPE::GAMELOOP;
			init = false;
			return;
		}
		if (ClickedButton(button_stats, InputManager.LeftClick()))
		{
			transition_info.first = true;
			transition_info.second = MENU_TYPE::SCORE;
			init = false;
			return;
		}
	}

	void Update()
	{
		//GROUND
		ground.move();

		//calculate animation
		float y_change = amplitude*std::sin(counter*quickness);
		counter++;
		if (counter > reset_value)
			counter = 0;

		//title
		rect_title.SetYwithVel(title_pos.y + y_change);

		//bird
		rect_bird.SetYwithVel(bird_pos.y + y_change);
	}

	void Render(float a_blend)
	{
		//Background
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1.34, 1));
		RenderManager.RenderRect(texture_background, scale);

		//Ground
		ground.render(a_blend);

		//Play button
		RenderManager.RenderRect(texture_start, button_play.GetMatrix());

		//Score button
		RenderManager.RenderRect(texture_score, button_stats.GetMatrix());

		//studio
		RenderManager.RenderRect(texture_studios, rect_studio.GetMatrix());

		//title
		RenderManager.RenderRect(texture_flappybird, rect_title.GetMatrixVelocity(a_blend));

		//bird
		if (animation_timer.TimeElapsed(Bird::bird_animation_step_time))
		{
			animation_index = (animation_index + 1) % texture_bird.size();
			animation_timer.Begin();
		}

		RenderManager.RenderRect(texture_bird[animation_index], rect_bird.GetMatrixVelocity(a_blend, 0));
		
		/* sprite rotation test code
		float min_x, min_y;
		Rect new_rect = Bird::spriterotate(rect_bird, angle, min_x, min_y);

		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(-.5, -.5, 0));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1));
		glm::mat4 scalez = glm::scale(glm::mat4(1.0f), glm::vec3(new_rect.dimensions.x / rect_bird.dimensions.x, new_rect.dimensions.y / rect_bird.dimensions.y, 1)); //original / new
		glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3((min_x - (new_rect.center.x - new_rect.dimensions.x/2.0f))/new_rect.dimensions.x, (min_y - (new_rect.center.y - new_rect.dimensions.y / 2.0f)) / new_rect.dimensions.y, 0));
		glm::mat4 final_m = glm::inverse(trans) * scalez * rot * trans;
		//RenderManager.RenderRect2(texture_bird[animation_index], new_rect.GetMatrixVelocity(a_blend, 0), final_m);
		*/
		

		checkError("end of main menu draw commands");
	}
};