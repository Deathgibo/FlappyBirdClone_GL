#pragma once

struct Pipe {
	float x_pos;
	float x_vel = 0;
	glm::vec2 y_centers; //(topy, boty)
	bool increment_score;

	const glm::vec2 dimensions;
	Pipe() : increment_score(true), dimensions(glm::vec2(.175, .75)) {}

	void MoveX(float x)
	{
		x_pos += x;
		x_vel = x;
	}

	void SetX(float x)
	{
		x_pos = x;
		x_vel = 0;
	}

	glm::mat4 GetMatrixTop(float a_blend)
	{
		if (Config::TEST_VELOCITY)
			x_vel *= Config::TEST_VELOCITY_FACTOR;
		//calculate new position based off linearly interpolation of frame time
		float new_xpos = x_pos + x_vel * a_blend;

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(dimensions.x, dimensions.y, 1));
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(new_xpos*2.0f - 1.0f, -y_centers.x*2.0f + 1.0f, 0));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 3.14f, glm::vec3(0, 0, 1));
		return translate * rotation * scale;
	}

	glm::mat4 GetMatrixBot(float a_blend) 
	{
		//calculate new position based off linearly interpolation of frame time
		float new_xpos = x_pos + x_vel * a_blend;

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(dimensions.x, dimensions.y, 1));
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(new_xpos*2.0f - 1.0f, -y_centers.y*2.0f + 1.0f, 0));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1));
		return translate * rotation * scale;
	}

	void CalculatePipesY(float pipe_height, float pipe_y_seperation)
	{
		//top pipe y position
		y_centers.x = pipe_height - dimensions.y / 2.0f;
		
		//bottom pipe y position
		y_centers.y = pipe_height + pipe_y_seperation + dimensions.y / 2.0f;
	}
};