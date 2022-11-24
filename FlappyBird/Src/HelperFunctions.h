#pragma once

bool checkError(std::string msg)
{
	GLenum errorCode;
	bool any_error = false;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		any_error = true;
		std::string error = "";
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		PRINT(std::cout << "GLERROR (" << msg << "): " << error << " " << errorCode << std::endl;)
		pushGLError("GLERROR (" + msg + "): " + error);
	}
	return any_error;
}

bool ClickedButton(const Rect& button, bool left_click)
{
	if (!left_click) return false;

	//convert percentage units to screen coordinates
	glm::vec2 button_center = button.center * glm::vec2(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
	glm::vec2 world_dimensions = button.dimensions * glm::vec2(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
	world_dimensions *= glm::vec2(.5, .5);
	glm::vec2 button_lr = glm::vec2(button_center.x - world_dimensions.x, button_center.x + world_dimensions.x);

	if (InputManager.MouseX() < button_lr.x || InputManager.MouseX() > button_lr.y) return false;

	glm::vec2 button_tb = glm::vec2(button_center.y - world_dimensions.y, button_center.y + world_dimensions.y);
	if (InputManager.MouseY() < button_tb.x || InputManager.MouseY() > button_tb.y) return false;

	return true;
}

bool AABBCollision(const glm::vec2& c1, const glm::vec2& c2, const glm::vec2& dim1, const glm::vec2& dim2)
{
	glm::vec2 hdim1 = dim1 * glm::vec2(.5, .5);
	glm::vec2 hdim2 = dim2 * glm::vec2(.5, .5);

	//is A to the left of B?
	if (c1.x + hdim1.x < c2.x - hdim2.x) return false;
	//is A to the right of B?
	if (c1.x - hdim1.x > c2.x + hdim2.x) return false;
	//is A on top of B?
	if (c1.y - hdim1.y > c2.y + hdim2.y) return false;
	//is A below B?
	if (c1.y + hdim1.y < c2.y - hdim2.y) return false;

	return true;
}

std::string calculateFPS(double milliseconds)
{
	std::string fps_string = std::to_string((1000.0 / milliseconds));
	int decimal_index = -1;
	for (int i = 0; i < fps_string.size(); i++)
	{
		if (fps_string[i] == '.')
		{
			decimal_index = i;
			break;
		}
	}
	if (decimal_index != -1)
	{
		if (decimal_index + 1 < fps_string.size())
		{
			fps_string = fps_string.substr(0, decimal_index + 2);
		}
		else
		{
			fps_string = fps_string + "0";
		}
	}
	return fps_string;
}