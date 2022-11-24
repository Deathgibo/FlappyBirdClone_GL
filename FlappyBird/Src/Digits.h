#pragma once

struct Digits
{
	std::array<Rect, 3> button_nums;
	glm::vec2 dim;
	glm::vec2 center;

	void Move(glm::vec2 vel)
	{
		center += vel;
		for (int i = 0; i < button_nums.size(); i++)
		{
			button_nums[i].velocity = vel;
		}
	}

	void render(int number, bool centered = true, float a_blend = -1.0f)
	{
		if (number < 0) number = 0;

		int first_digit = number % 10;
		int second_digit = (number / 10) % 10;
		int third_digit = (number / 100) % 10;

		int number_of_digits = 1;
		if (number > 9) number_of_digits = 2;
		if (number > 99) number_of_digits = 3;

		if (centered)
		{
			if (number_of_digits == 1)
			{
				button_nums[0].center = center;
				button_nums[0].dimensions = dim;

				RenderManager.RenderRect(texture_numbers[first_digit],(a_blend == -1.0f) ? button_nums[0].GetMatrix() : button_nums[0].GetMatrixVelocity(a_blend));
			}
			else if (number_of_digits == 2)
			{
				button_nums[0].center = glm::vec2(center.x - dim.x / 2.0f, center.y);
				button_nums[0].dimensions = dim;

				button_nums[1].center = glm::vec2(center.x + dim.x / 2.0f, center.y);
				button_nums[1].dimensions = dim;

				RenderManager.RenderRect(texture_numbers[second_digit], (a_blend == -1.0f) ?  button_nums[0].GetMatrix() : button_nums[0].GetMatrixVelocity(a_blend));
				RenderManager.RenderRect(texture_numbers[first_digit], (a_blend == -1.0f) ?  button_nums[1].GetMatrix() : button_nums[1].GetMatrixVelocity(a_blend));
			}
			else
			{
				button_nums[0].center = glm::vec2(center.x - dim.x, center.y);
				button_nums[0].dimensions = dim;

				button_nums[1].center = glm::vec2(center.x, center.y);
				button_nums[1].dimensions = dim;

				button_nums[2].center = glm::vec2(center.x + dim.x, center.y);
				button_nums[2].dimensions = dim;

				RenderManager.RenderRect(texture_numbers[third_digit], (a_blend == -1.0f) ?  button_nums[0].GetMatrix() : button_nums[0].GetMatrixVelocity(a_blend));
				RenderManager.RenderRect(texture_numbers[second_digit], (a_blend == -1.0f) ?  button_nums[1].GetMatrix() : button_nums[1].GetMatrixVelocity(a_blend));
				RenderManager.RenderRect(texture_numbers[first_digit], (a_blend == -1.0f) ?  button_nums[2].GetMatrix() : button_nums[2].GetMatrixVelocity(a_blend));
			}
		}
		else
		{
			if (number_of_digits == 1)
			{
				button_nums[0].center = center;
				button_nums[0].dimensions = dim;

				RenderManager.RenderRect(texture_numbers[first_digit], (a_blend == -1.0f) ?  button_nums[0].GetMatrix() : button_nums[0].GetMatrixVelocity(a_blend));
			}
			else if (number_of_digits == 2)
			{
				button_nums[0].center = glm::vec2(center.x - dim.x, center.y);
				button_nums[0].dimensions = dim;

				button_nums[1].center = center;
				button_nums[1].dimensions = dim;

				RenderManager.RenderRect(texture_numbers[second_digit], (a_blend == -1.0f) ?  button_nums[0].GetMatrix() : button_nums[0].GetMatrixVelocity(a_blend));
				RenderManager.RenderRect(texture_numbers[first_digit], (a_blend == -1.0f) ?  button_nums[1].GetMatrix() : button_nums[1].GetMatrixVelocity(a_blend));
			}
			else
			{
				button_nums[0].center = glm::vec2(center.x - 2 * dim.x, center.y);
				button_nums[0].dimensions = dim;

				button_nums[1].center = glm::vec2(center.x - dim.x, center.y);
				button_nums[1].dimensions = dim;

				button_nums[2].center = center;
				button_nums[2].dimensions = dim;

				RenderManager.RenderRect(texture_numbers[third_digit], (a_blend == -1.0f) ?  button_nums[0].GetMatrix() : button_nums[0].GetMatrixVelocity(a_blend));
				RenderManager.RenderRect(texture_numbers[second_digit], (a_blend == -1.0f) ?  button_nums[1].GetMatrix() : button_nums[1].GetMatrixVelocity(a_blend));
				RenderManager.RenderRect(texture_numbers[first_digit], (a_blend == -1.0f) ?  button_nums[2].GetMatrix() : button_nums[2].GetMatrixVelocity(a_blend));
			}
		}
	}
};