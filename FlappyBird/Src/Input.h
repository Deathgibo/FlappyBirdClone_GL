#pragma once

class Input
{
private:
	double mousex, mousey;
	
	bool left_click = false;
	bool left_click_once = false;
	bool left_click_release = true;

	bool space_down = false;
	bool space_down_once = false;
	bool space_down_release = true;

	bool escape_down = false;

	bool not_disabled = true;
public:
	double MouseX() { return mousex; }
	double MouseY() { return mousey; }
	bool LeftClick() { return left_click && not_disabled; }
	bool SpaceDown() { return space_down && not_disabled; }
	bool EscapeDown() { return escape_down; }
	
	void Disable(bool val) { not_disabled = !val; }

	void HandleInput()
	{
		glfwPollEvents();

		//handle left click
		if (left_click_once)
		{
			left_click = false;
			left_click_once = false;
		}
		if (left_click)
		{
			left_click_once = true;
		}

		//handle space bar
		if (space_down_once)
		{
			space_down = false;
			space_down_once = false;
		}
		if (space_down)
		{
			space_down_once = true;
		}
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_SPACE)
		{
			if (action == GLFW_PRESS)
			{
				if (space_down_release == true)
				{
					space_down = true;
					space_down_release = false;
				}
			}
			else if (action == GLFW_RELEASE)
			{
				space_down_release = true;
			}
		}
		
		if (key == GLFW_KEY_ESCAPE)
		{
			if (action == GLFW_PRESS)
				escape_down = true;
			else if (action == GLFW_RELEASE)
				escape_down = false;	
		}
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		mousex = xpos;
		mousey = ypos;
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				if (left_click_release == true)
				{
					left_click = true;
					left_click_release = false;
				}
			}
			else if (action == GLFW_RELEASE) {
				left_click_release = true;
			}
		}
	}
};

Input InputManager;
