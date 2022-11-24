#pragma once

struct Ground
{
public:
	std::array<Rect, 3> rects;
	const float ground_speed = .0045;
	const float eps = 0.001f;
	Ground()
	{
		glm::vec2 starting(.35, .95);
		glm::vec2 dimensions(.7, .15);

		for (int i = 0; i < rects.size(); i++)
		{
			rects[i].center = glm::vec2(starting.x + i*(dimensions.x- eps), starting.y);
			rects[i].dimensions = dimensions;
		}
	}
	
	void move()
	{
		for (int i = 0; i < rects.size(); i++)
		{
			rects[i].Move(glm::vec2(-ground_speed, 0));
		}
		
		for (int i = 0; i < rects.size(); i++)
		{
			if (rects[i].center.x + rects[i].dimensions.x / 2.0f < 0.0)
			{
				int next_index = (i + 2) % 3;
				rects[i].SetX(rects[next_index].center.x + rects[next_index].dimensions.x - eps);
			}
		}
	}

	void render(float a_blend)
	{
		for (int i = 0; i < rects.size(); i++)
		{
			RenderManager.RenderRect(texture_ground, rects[i].GetMatrixVelocity(a_blend));
		}
	}
};