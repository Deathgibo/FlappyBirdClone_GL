#pragma once

//const double a = 1.42857142857f;
//const double b = 0.0129f;
struct Bird {
	Rect rect;
	float rotation_angle = 0.0f; //between 30,-90 based off y_vel

	//jumping is just an upside down parabola. -ax^2 + b. So pick your a,b values. 
	//then we jump until we hit the x_intercept across x frames.
	const double a = .8f;
	const double b = .0185;
	const double x_intercept = sqrt(b / a);
	const double jump_time_frames = 63;
	const double jump_timestep = x_intercept / jump_time_frames;
	double jump_x = 0.0; //input into jump function. between 0-x_intercept
	bool jumping = false;
	
	const float gravity = 0.013;

	static const int bird_animation_step_time = 142;

	Bird()
	{
		//verify numberical stability
		float eps = 0.00000001;
		assert(a > 0 && b >= 0); //a and b can't be negative. a can't be equal to 0
		//simulate precision
		jump_x = 0.0;
		for (int i = 0; i < jump_time_frames; i++)
		{
			jump_x += jump_timestep;
		}
		//precision errors are too big. pick different a and b
		assert(std::abs(jump_x - x_intercept) <= eps);
		assert(jump_x + jump_timestep > x_intercept);

		jump_x = 0.0f;
	}

	void input(bool gameover, bool paused)
	{
		if (!gameover && !paused && InputManager.SpaceDown())
		{
			jumping = true;
			jump_x = 0;
			AudioManager.playSound(SOUND::WING);
		}
	}

	void update()
	{
		float old_y = rect.center.y;
		rect.Move(glm::vec2(0, gravity));

		if (jumping)
			jump();

		rect.velocity.y = rect.center.y - old_y;

		if (rect.velocity.y >= .005)
		{
			rotation_angle = std::clamp(rotation_angle - 2.257f, -90.0f, 20.0f);
		}
		else
		{
			rotation_angle = std::clamp(rotation_angle + 7.1428f, -90.0f, 20.0f);
		}

		//ground.rects[0].center.y - ground.rects[0].dimensions.y / 2.0f
		float ground_y = .95 - (.15 / 2.0f);
		if (ground_y < (rect.center.y + rect.dimensions.y/2.0f))
		{
			rect.SetY(ground_y - rect.dimensions.y/2.0f);
		}
		if ((rect.center.y + rect.dimensions.y < 0.0))
		{
			rect.SetY(-rect.dimensions.y);
		}
	}

	//jump function is just a parabola: -ax^2 + b
	void jump()
	{
		float jump_force = -a*pow(jump_x, 2) + b;
		jump_x += jump_timestep;

		rect.Move(glm::vec2(0, -jump_force));
		if (jump_x >= x_intercept)
		{
			jumping = false;
		}
	}

	glm::mat4 GetMatrixVelocity(float a_blend, float _rotation_angle)
	{
		return rect.GetMatrixVelocity(a_blend, _rotation_angle);
	}

	//experimenting code
	static Rect spriterotate(Rect rect, float rotation_angle, float& min_x, float& min_y)
	{
		std::array<glm::vec4, 4> points;
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_angle), glm::vec3(0, 0, 1));
		points[0] = rotation * glm::vec4(-rect.dimensions.x / 2.0f, rect.dimensions.y / 2.0f, 0, 1.0);
		points[1] = rotation * glm::vec4(-rect.dimensions.x / 2.0f, -rect.dimensions.y / 2.0f, 0, 1.0);
		points[2] = rotation * glm::vec4(rect.dimensions.x / 2.0f, -rect.dimensions.y / 2.0f, 0, 1.0);
		points[3] = rotation * glm::vec4(rect.dimensions.x / 2.0f, rect.dimensions.y / 2.0f, 0, 1.0);

		glm::vec2 x_bounds;
		glm::vec2 y_bounds;
		//smallest x
		float x_min = 0;
		float x_max = 0;
		float y_min = 0;
		float y_max = 0;
		for (int i = 0; i < points.size(); i++)
		{
			//x min
			if (i == 0)
				x_min = points[i].x;
			else
				x_min = std::min(x_min, points[i].x);
			//x max
			if (i == 0)
				x_max = points[i].x;
			else
				x_max = std::max(x_max, points[i].x);
			//y min
			if (i == 0)
				y_min = points[i].y;
			else
				y_min = std::min(y_min, points[i].y);
			//y max
			if (i == 0)
				y_max = points[i].y;
			else
				y_max = std::max(y_max, points[i].y);
		}

		Rect new_rect;
		new_rect.center = rect.center;
		new_rect.dimensions = glm::vec2(x_max - x_min, y_max - y_min);
		new_rect.dimensions = glm::vec2(.5, .5);

		min_x = x_min;
		min_y = y_min;

		return new_rect;
		//create a new aabb around rotated image width and height
		//this new aabb will be the new render rect

		//calculate matrix that will map texels on our new aabb to the sprite
	}
};