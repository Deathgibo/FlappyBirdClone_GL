#pragma once

class Menu_Game
{
	enum class GAME_STATE : uint8_t { STARTING, PLAYING, GAMEOVER };
private:
	bool init = false; //should be set to false whenever you switch menus

	Rect button_pause;
	Rect button_menu;
	Rect button_ok;
	Rect button_share;
	Rect rect_game_over;
	Rect rect_overback;
	Rect rect_Coin;
	Rect rect_getready;
	Rect rect_tap;

	Ground ground;

	Digits current_score_digits;
	Digits best_digits;
	Digits score_digits;

	std::array<Pipe, 10> Pipes;
	const float pipe_starting_x = 2.25f;
	const float pipe_starting_y = .3;
	const float pipe_y_seperation = .275;
	const float pipe_x_seperation = .55;
	int back_pipe_index = 0;
	const float pipe_min_distance_from_roof = .08;
	const float pipe_min_distance_from_ground = .08;
	const float pipe_movespeed = .0045; //takes 2 and a half seconds to go 100%. Same as ground
	
	Bird bird;
	Timer animation_timer;
	int animation_index;

	const float starting_amplitude = 0.01f; //amplitude of title animation
	const float starting_quickness = 0.08f; //quickness of title animation
	const float starting_reset_value = (2 * 3.14159265359) / starting_quickness;
	float starting_counter = 0;

	const float game_over_speed = 0.001;
	const int game_over_up_frames = 12;
	const int game_over_down_frames = 30;
	int game_over_frames = 0;

	float current_alpha = 0.0f;
	const float alpha_lower = 0.015f;
	bool alpha_done = false;
	
	
	int current_score;
	float climb_score;
	const float climb_speed = .5;
	const int climb_wait_time = 50;
	int climb_wait_count = 0;
	Timer game_over_timer;
	const float spawn_game_over_time = 600;
	const float spawn_results_time = 1500;
	const float results_animation_time = 300;
	const int spawn_buttons_time = 15;
	int spawn_button_frame = 0;
	bool start_climbing_score;
	bool spawn_buttons;

	const float score_end_y = .5;
	const float beggining_y = 1.3f;
	float last_y = score_end_y;
	bool score_animation_done = false;

	bool play_move_score_sound_once = false;
	const int falling_frames = 25;
	int falling_count = 0;
	bool play_falling_sound = false;

	int best_score = 0;
	
	bool paused; 
	GAME_STATE current_state;
public:
	Menu_Game() 
	{
		current_score_digits.center = glm::vec2(.5, 0.075);
		current_score_digits.dim = glm::vec2(0.05, 0.05);

		bird.rect.dimensions = glm::vec2(.118, .0648);
		bird.rect.dimensions = glm::vec2(.09, .0648);
		
		button_pause.dimensions = glm::vec2(.08, .07);
		
		button_menu.dimensions = glm::vec2(.274, .0723);

		rect_getready.center = glm::vec2(.50, .25);
		rect_getready.dimensions = glm::vec2(.65, .1);

		rect_tap.center = glm::vec2(.5, .55);
		rect_tap.dimensions = glm::vec2(.5, .25);

		rect_overback.dimensions = glm::vec2(.75, .30);

		rect_Coin.dimensions = glm::vec2(.145, .12);

		button_ok.center = glm::vec2(0.288, 0.81);
		button_ok.dimensions = glm::vec2(.274, .0723);

		button_share.center = glm::vec2(0.712, 0.81);
		button_share.dimensions = glm::vec2(.274, .0723);

		score_digits.dim = glm::vec2(.045, .045);
		best_digits.dim = glm::vec2(.045, .045);
	}

	void Init()
	{
		if (!init)
		{
			current_state = GAME_STATE::STARTING;
			paused = false;

			current_score = 0;
			climb_score = 0;
			start_climbing_score = false;
			spawn_buttons = false;
			spawn_button_frame = 0;
			animation_index = 0;
			climb_wait_count = 0;
			starting_counter = 0;


			current_alpha = 0.0f;
			alpha_done = false;

			rect_game_over.center = glm::vec2(.5, .23);
			rect_game_over.dimensions = glm::vec2(.6, .1);
			rect_game_over.velocity = glm::vec2(0, 0);

			rect_overback.center = glm::vec2(.5, beggining_y);
			rect_Coin.center = glm::vec2(.285, 1.32);
			score_digits.center = glm::vec2(.78, 1.27);
			best_digits.center = glm::vec2(.78, 1.385);

			rect_overback.velocity = glm::vec2(0, 0);
			rect_Coin.velocity = glm::vec2(0, 0);
			for (int i = 0; i < score_digits.button_nums.size(); i++)
			{
				score_digits.button_nums[i].velocity = glm::vec2(0, 0);
				best_digits.button_nums[i].velocity = glm::vec2(0, 0);
			}
			score_animation_done = false;
			last_y = beggining_y;
			game_over_frames = 0;

			falling_count = 0;
			play_falling_sound = false;

			play_move_score_sound_once = false;

			bird.rotation_angle = 0.0f;
			animation_timer.Begin();
			bird.rect.center = glm::vec2(.33, 0.5);

			button_pause.center = glm::vec2(0.085, 0.075);

			button_menu.center = glm::vec2(.5, .5);

			//initialize pipes
			back_pipe_index = 0;
			for (int i = 0; i < Pipes.size(); i++)
			{
				Pipe& pipe = Pipes[i];
				pipe.increment_score = true;
				if (i == 0)
				{
					pipe.SetX(pipe_starting_x);
					float ground_plus_seperation = ground.rects[0].center.y - ground.rects[0].dimensions.y / 2.0f - pipe_y_seperation;
					float max_y = ground_plus_seperation - pipe_min_distance_from_ground;
					float pipe_height = GetRandomNumber(pipe_min_distance_from_roof, max_y);
					pipe.CalculatePipesY(pipe_height, pipe_y_seperation);
				}
				else
				{
					pipe.SetX(100.0f);
					pipe.CalculatePipesY(pipe_starting_y, pipe_y_seperation);
				}
			}
			init = true;
		}
	}

	void Input(std::pair<bool, MENU_TYPE>& transition_info)
	{
		if (current_state != GAME_STATE::GAMEOVER && paused && ClickedButton(button_menu, InputManager.LeftClick()))
		{
			transition_info.first = true;
			transition_info.second = MENU_TYPE::MAIN;
			init = false;
			return;
		}

		if (spawn_buttons && spawn_button_frame >= spawn_buttons_time)
		{
			if (current_state == GAME_STATE::GAMEOVER && ClickedButton(button_ok, InputManager.LeftClick()))
			{
				transition_info.first = true;
				transition_info.second = MENU_TYPE::MAIN;
				init = false;
				return;
			}

			if (current_state == GAME_STATE::GAMEOVER && ClickedButton(button_share, InputManager.LeftClick()))
			{
				if (current_score >= 40)
				{
					if (AudioManager.isPlaying(SOUND::NABOKOV))
						AudioManager.stopAllSound();
					AudioManager.playSound(SOUND::NABOKOV);
				}
			}
		}

		if (current_state != GAME_STATE::GAMEOVER && ClickedButton(button_pause, InputManager.LeftClick()))
		{
			if (paused)
				paused = false;
			else
				paused = true;
		}

		bird.input((current_state == GAME_STATE::GAMEOVER), paused);

		//START GAME
		if (current_state == GAME_STATE::STARTING && !paused && InputManager.SpaceDown())
			current_state = GAME_STATE::PLAYING;
	}

	void Update()
	{
		//get ready transparency
		if (current_state != GAME_STATE::GAMEOVER)
		{
			if (!alpha_done)
			{
				if (!paused)
				{
					if (current_state == GAME_STATE::STARTING)
					{
						current_alpha = std::min(current_alpha + alpha_lower, 1.0f);
					}
					else if (current_state == GAME_STATE::PLAYING)
					{
						current_alpha -= alpha_lower;
						if (current_alpha <= 0)
						{
							alpha_done = true;
						}
					}
				}
			}
		}

		//BIRD
		if (current_state == GAME_STATE::STARTING && !paused)
		{
			//calculate animation
			float y_change = starting_amplitude * std::sin(starting_counter*starting_quickness);
			starting_counter++;
			if (starting_counter > starting_reset_value)
				starting_counter = 0;

			bird.rect.SetYwithVel(0.5 + y_change);
		}
		if (current_state != GAME_STATE::STARTING && !paused)
		{
			bird.update();
		}

		//PIPES and COLLISION
		if (current_state == GAME_STATE::PLAYING && !paused)
		{
			//gameplay logic
			bool spawn_pipe = false;
			for (int i = 0; i < Pipes.size(); i++)
			{
				Pipe& pipe = Pipes[i];
				pipe.MoveX(-pipe_movespeed);

				//see if the pipe "line" intersects the bird square. If so increment timer
				if (pipe.increment_score && pipe.x_pos <= bird.rect.center.x + bird.rect.dimensions.x / 2.0f)
				{
					current_score++;
					if (current_score > 999) current_score = 999;
					pipe.increment_score = false;
					AudioManager.playSound(SOUND::POINT);
				}

				//see if back pipe has reached past xseperation from starting
				if (i == back_pipe_index && pipe_starting_x - pipe.x_pos >= pipe_x_seperation)
				{
					spawn_pipe = true;
				}
			}
			if (spawn_pipe)
			{
				//get new pip height
				float ground_plus_seperation = ground.rects[0].center.y - ground.rects[0].dimensions.y / 2.0f - pipe_y_seperation;
				float max_y = ground_plus_seperation - pipe_min_distance_from_ground;
				float pipe_height = GetRandomNumber(pipe_min_distance_from_roof, max_y);

				//alternate between the minimum and maximum pipe heights
				if (Config::TEST_HARD_PIPES)
				{
					static int lolz = 0;
					lolz = (lolz + 1) % 2;
					if (lolz == 0)
						pipe_height = pipe_min_distance_from_roof;
					else
						pipe_height = max_y;
				}

				int next_index = (back_pipe_index + 1) % Pipes.size();
				Pipes[next_index].CalculatePipesY(pipe_height, pipe_y_seperation);
				Pipes[next_index].SetX(Pipes[back_pipe_index].x_pos + pipe_x_seperation);
				Pipes[next_index].increment_score = true;
				back_pipe_index = next_index;
			}

			//COLLISION
			bool collision = false;
			bool collide_pipe = false;
			for (int i = 0; i < Pipes.size(); i++)
			{
				Pipe& pipe = Pipes[i];
				collision |= AABBCollision(bird.rect.center, glm::vec2(pipe.x_pos, pipe.y_centers.x), bird.rect.dimensions, pipe.dimensions);
				collision |= AABBCollision(bird.rect.center, glm::vec2(pipe.x_pos, pipe.y_centers.y), bird.rect.dimensions, pipe.dimensions);
				if (collision)
				{
					collide_pipe = true;
					break;
				}
			}
			if (!collision)
			{
				collision |= AABBCollision(bird.rect.center, ground.rects[0].center, bird.rect.dimensions, ground.rects[0].dimensions);
				collision |= AABBCollision(bird.rect.center, ground.rects[1].center, bird.rect.dimensions, ground.rects[1].dimensions);
				collision |= AABBCollision(bird.rect.center, ground.rects[2].center, bird.rect.dimensions, ground.rects[2].dimensions);
			}

			if (Config::TEST_INVINCIBILITY)
				collision = false;

			if (collision)
			{
				AudioManager.playSound(SOUND::HIT);
				if (collide_pipe)
					play_falling_sound = true;

				animation_index = 1;
				current_state = GAME_STATE::GAMEOVER;
				game_over_timer.Begin();

				for (int i = 0; i < Pipes.size(); i++)
				{
					Pipe& pipe = Pipes[i];
					pipe.x_vel = 0;
				}
				ground.rects[0].velocity = glm::vec2(0, 0);
				ground.rects[1].velocity = glm::vec2(0, 0);
				ground.rects[2].velocity = glm::vec2(0, 0);

				//update scores and write to file
				std::array<int, 8> scores = ReadStatsFile();
				if (scores[7] != -1)
				{
					if (current_score > scores[0]) { scores[2] = scores[1]; scores[1] = scores[0]; scores[0] = current_score; }
					else if (current_score > scores[1]) { scores[2] = scores[1]; scores[1] = current_score; }
					else if (current_score > scores[2]) { scores[2] = current_score; }
					scores[7] = scores[6]; scores[6] = scores[5]; scores[5] = scores[4]; scores[4] = scores[3]; scores[3] = current_score;

					WriteStatsFile(scores);
				}
				best_score = std::max(scores[0], 0);
			}
		}

		//GROUND
		if (current_state != GAME_STATE::GAMEOVER && !paused)
		{
			ground.move();
		}

		if (current_state == GAME_STATE::GAMEOVER)
		{
			//if we want to play falling sound player it after x frames
			falling_count = std::min(falling_count + 1, falling_frames);
			if (falling_count >= falling_frames)
			{
				if (play_falling_sound)
				{
					AudioManager.playSound(SOUND::FALL);
					play_falling_sound = false;
				}
			}

			//animate game_over
			if (game_over_timer.TimeElapsed(spawn_game_over_time))
			{
				if(game_over_frames == 0)
					AudioManager.playSound(SOUND::TRANSITION);
				if (game_over_frames < game_over_down_frames)
				{
					if (game_over_frames < game_over_up_frames)
					{
						rect_game_over.Move(glm::vec2(0, -game_over_speed));
					}
					else
					{
						rect_game_over.Move(glm::vec2(0, game_over_speed));
					}
					game_over_frames++;
				}
				else
				{
					rect_game_over.velocity = glm::vec2(0, 0);
				}
			}

			//animate score rects
			if (game_over_timer.TimeElapsed(spawn_results_time))
			{
				if (!play_move_score_sound_once)
				{
					AudioManager.playSound(SOUND::TRANSITION);
					play_move_score_sound_once = true;
				}

				float a = std::clamp((game_over_timer.getTime() - spawn_results_time) / results_animation_time, 0.0f, 1.0f);
				if (!score_animation_done)
				{
					float y_pos = (1 - a)*beggining_y + a * score_end_y;
					float y_diff = y_pos - last_y;
					last_y = y_pos;

					rect_overback.Move(glm::vec2(0, y_diff));
					rect_Coin.Move(glm::vec2(0, y_diff));
					score_digits.Move(glm::vec2(0, y_diff));
					best_digits.Move(glm::vec2(0, y_diff));
					if (a == 1.0)
						score_animation_done = true;
				}
				else
				{
					rect_overback.velocity = glm::vec2(0, 0);
					rect_Coin.velocity = glm::vec2(0, 0);
					for (int i = 0; i < score_digits.button_nums.size(); i++)
					{
						score_digits.button_nums[i].velocity = glm::vec2(0, 0);
						best_digits.button_nums[i].velocity = glm::vec2(0, 0);
					}
				}
			}

			//animate score climbing
			if (score_animation_done)
			{
				climb_wait_count = std::min(climb_wait_count + 1, climb_wait_time);
				if (climb_wait_count >= climb_wait_time)
				{
					climb_score = std::min(climb_score+= climb_speed, (float)current_score);
					if (climb_score >= current_score)
					{
						spawn_buttons = true;
						spawn_button_frame = std::min(spawn_button_frame + 1, spawn_buttons_time);
					}
				}
			}
		}

		if (paused)
		{
			for (int i = 0; i < Pipes.size(); i++)
			{
				Pipe& pipe = Pipes[i];
				pipe.x_vel = 0;
			}
			ground.rects[0].velocity = glm::vec2(0, 0);
			ground.rects[1].velocity = glm::vec2(0, 0);
			ground.rects[2].velocity = glm::vec2(0, 0);

			bird.rect.velocity = glm::vec2(0, 0);
		}
	}

	void Render(float a_blend)
	{
		//BACKGROUND
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1.34, 1));
		RenderManager.RenderRect(texture_background, scale);

		//PIPES 
		for (int i = 0; i < Pipes.size(); i++)
		{
			RenderManager.RenderRect(texture_pipes, Pipes[i].GetMatrixTop(a_blend));
			RenderManager.RenderRect(texture_pipes, Pipes[i].GetMatrixBot(a_blend));
		}

		//BIRD: animated
		if (current_state != GAME_STATE::GAMEOVER && !paused)
		{
			if (animation_timer.TimeElapsed(Bird::bird_animation_step_time))
			{
				animation_index = (animation_index + 1) % texture_bird.size();
				animation_timer.Begin();
			}
		}

		RenderManager.RenderRect(texture_bird[animation_index], bird.GetMatrixVelocity(a_blend, bird.rotation_angle));
		
		/* code for sprite rotation
		float min_x, min_y;
		Rect new_rect = Bird::spriterotate(bird.rect, bird.rotation_angle, min_x, min_y);

		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(-.5, -.5, 0));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(bird.rotation_angle), glm::vec3(0, 0, 1));
		glm::mat4 scalez = glm::scale(glm::mat4(1.0f), glm::vec3(new_rect.dimensions.x / bird.rect.dimensions.x, new_rect.dimensions.y / bird.rect.dimensions.y, 1)); //original / new
		glm::mat4 move = glm::translate(glm::mat4(1.0f), glm::vec3((min_x - (new_rect.center.x - new_rect.dimensions.x / 2.0f)) / new_rect.dimensions.x, (min_y - (new_rect.center.y - new_rect.dimensions.y / 2.0f)) / new_rect.dimensions.y, 0));

		glm::mat4 final_m = glm::inverse(trans) * scalez * rot * trans;
		//glm::mat4 final_m = glm::inverse(trans) * glm::inverse(rot) * scalez * trans;
		//RenderManager.RenderRect2(texture_bird[animation_index], new_rect.GetMatrixVelocity(a_blend, 0), final_m);
		*/

		//GROUND
		ground.render(a_blend);

		//GET READY (TAP)
		if (current_state != GAME_STATE::GAMEOVER)
		{
			if (!alpha_done)
			{
				float alpha = current_alpha; 
				if(current_state == GAME_STATE::STARTING) alpha += a_blend * alpha_lower;
				if (current_state == GAME_STATE::PLAYING) alpha -= a_blend * alpha_lower;

			
				RenderManager.RenderRect(texture_getready, rect_getready.GetMatrix(), glm::vec4(0, 0, 0, alpha), true, true);
				RenderManager.RenderRect(texture_tap, rect_tap.GetMatrix(), glm::vec4(0, 0, 0, alpha), true, true);
			}
		}

		//Game over menu 
		if (current_state == GAME_STATE::GAMEOVER)
		{

			if (game_over_timer.TimeElapsed(spawn_game_over_time))
				RenderManager.RenderRect(texture_gameover, rect_game_over.GetMatrixVelocity(a_blend));

			if (game_over_timer.TimeElapsed(spawn_results_time))
			{
				RenderManager.RenderRect(texture_overback, rect_overback.GetMatrixVelocity(a_blend));

				int medal = -1; //nothing
				if (current_score >= 10) medal++; //bronze
				if (current_score >= 20) medal++; //silver
				if (current_score >= 30) medal++; //gold
				if (current_score >= 40) medal++; //plat

				if (medal != -1)
					RenderManager.RenderRect(texture_medals[medal], rect_Coin.GetMatrixVelocity(a_blend)); //animations?

				if(climb_score < current_score)
					score_digits.render(std::floor(climb_score), false, a_blend);
				else
					score_digits.render(current_score, false, a_blend);

				best_digits.render(best_score, false, a_blend);
			}

			if (spawn_buttons && spawn_button_frame >= spawn_buttons_time)
			{
				RenderManager.RenderRect(texture_ok, button_ok.GetMatrix());

				if (current_score >= 40)
					RenderManager.RenderRect(texture_share, button_share.GetMatrix());
				else
					RenderManager.RenderRect(texture_sharedark, button_share.GetMatrix());
			}

			//white flash
			if (!game_over_timer.TimeElapsed(spawn_game_over_time))
			{
				float alpha_val = 1.0f - (game_over_timer.getTime() / spawn_game_over_time);
				RenderManager.RenderRect(texture_dot, glm::mat4(1.0f), glm::vec4(1, 1, 1, alpha_val), false);
			}
		}

		//SCORE
		if (current_state != GAME_STATE::GAMEOVER)
		{
			current_score_digits.render(current_score);
		}

		//GUI
		if (current_state != GAME_STATE::GAMEOVER)
		{
			if (paused)
			{
				RenderManager.RenderRect(texture_default, glm::mat4(1.0f), glm::vec4(0, 0, 0, .5), false);

				RenderManager.RenderRect(texture_menu, button_menu.GetMatrix());
			}
			RenderManager.RenderRect((paused) ? texture_play : texture_pause, button_pause.GetMatrix());
		}

		checkError("end of game loop draw commands");
	}
};