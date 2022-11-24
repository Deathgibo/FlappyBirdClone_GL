#pragma once

//units are in percentages. 0 means the very lefttop of your screen. (.5,.5) is in the center of the screen
//dimensions are whole dimensions, not half. so 50% covers 50% of your screen.
struct Rect {
	glm::vec2 center; //in percentages
	glm::vec2 dimensions; //in percentages  
	glm::vec2 velocity = glm::vec2(0, 0); //velocity to linearly interpolate render frame based off frameTime

	void Move(glm::vec2 vel)
	{
		center += vel;
		velocity = vel;
	}

	void SetX(float x)
	{
		center.x = x;
		velocity.x = 0;
	}

	void SetY(float y)
	{
		center.y = y;
		velocity.y = 0;
	}

	void SetYwithVel(float y)
	{
		velocity.y = y - center.y;
		center.y = y;
	}

	//angle in degrees
	glm::mat4 GetMatrix(float rotation_angle = 0.0f) const
	{
		//convert percentages to ndc
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(dimensions.x, dimensions.y, 1));
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(center.x*2.0f - 1.0f, -center.y*2.0f + 1.0f, 0));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_angle), glm::vec3(0, 0, 1));
		return translate * rotation * scale;
	}

	glm::mat4 GetMatrixVelocity(float a_blend, float rotation_angle = 0.0f)
	{
		if (Config::TEST_VELOCITY)
			velocity *= glm::vec2(Config::TEST_VELOCITY_FACTOR, Config::TEST_VELOCITY_FACTOR);
	
		//calculate new position based off linearly interpolation of frame time
		glm::vec2 new_center = center + velocity * glm::vec2(a_blend, a_blend);
		
		//rotation would also need to get interpolated, we'd need to know the angular velocity

		//convert percentages to ndc
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(dimensions.x, dimensions.y, 1));
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(new_center.x*2.0f - 1.0f, -new_center.y*2.0f + 1.0f, 0));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_angle), glm::vec3(0, 0, 1));

		return translate * rotation * scale;


		/* code messing around with sprite rotation	
		glm::mat4 PtoW = glm::scale(glm::mat4(1.0f), glm::vec3(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, 1));
		glm::mat4 WtoP = glm::inverse(PtoW);
		glm::mat4 sr = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 0, 1));
		glm::mat4 sr_inv = glm::inverse(sr);

		glm::mat4 cam = glm::translate(glm::mat4(1.0f), glm::vec3(InputManager.MouseX()*.001, InputManager.MouseY()*.001, 0));
		glm::mat4 ortho = glm::ortho(-(float)Config::SCREEN_WIDTH/4, (float)Config::SCREEN_WIDTH / 4, -(float)Config::SCREEN_HEIGHT/4, (float)Config::SCREEN_HEIGHT/4);
		translate = glm::translate(glm::mat4(1.0f), glm::vec3(new_center.x, new_center.y, 0));	
		return  ortho * translate * rotation * scale;	
		*/
	}
};


/*
--window size independence notes--
. anchoring: you usually pick some area of the screen, top left, middle middle, bottom right, and you anchor. And anchoring just keeps it x and y pixels from that point. So if I anchor a point on the top left and set its pixel center to 200x300. No matter what resolution it will be 200 from the top left and 300 from the top. Or you probably can make it percentage as well, its always x% from the left and right.

* vector graphics. One method is just to render squares with textures, but another method for 2D is vector graphics, its like 3D graphics, you define  actual geometry and render that with maybe textures or just lighting. Or you can create vector images and render that

. scaling methods: up to an extent, same aspect ratio, percentage of screen
. absolute
. hybrid
. force screen to be same

. you can either scale the object, or keep it absolute size. Then theres questions of what type of scaling: 1 to 1, aspect ratio, up to a certain size. Then how do you handle background space. Or you can force it to be a certain size.

* so scaling is fine, one problem though is if its too big it could look blocky and stretchy. Either you have higher resolution images or you do like vector graphics. The other problem is you can have a wonky aspect ratio and warp the image a lot. Honestly this isnt too bad because people will usually play on a normal one, but it still could look weird if its not your target aspect ratio.
* So you can lock aspect ratio, then you need to make window black though in the areas that are extra theres not really a way around it. If you cant show any more or any less of the world, you dont want aspect ratio stretching and you want everything to be relative then you need to just render a border around
* You can make it so you just show what the camera sees, its easy but then you can see more or less of the world
* technically you can actually have different layouts for different screen sizes, like landscape mode where you literally design everything different based on that
For windows you probably want to design for a 16:9 aspect ratio. This is all the major rations like 2k,4k,8k, etc

. absolute space: absolute space is when your perspective camera has the same dimensions as your screen because now viewport transformation won't warp. Even your world space is in screen space. That way rotation stays clean in screen space, but of course you can't make everything relative with a screen size shift

**The problem is that rendering sprites in 2D are different. If you just are rendering straight geomtry rotating is fine, but for a sprite it expects it to be rendered on an AABB box, so if you actually rotate the quad rendering it it will get stretched and look weird. Its subtle but like if you have a game object the physics box will actually rotate but the sprite rendering box shouldnt, it should just be an aabb box. That is the issue I think that if I rotate a sprite I should rotate the render quad but no, you'd create a bigger aabbox then correctly rotate the sprite to render inside the physics box so they match in the world

***To test if sprite rotating works. Render the collision box of your game object, then render your actual aabb render quad just to see, then rotate your sprite and if the actual drawing of your sprite fits into the collision box you are good
**if you get rid of viewport warp it of course will work, maybe theres some way
**EVEN Unity if I rotate a rect and look at its global, the rect is changing the sprite is being redrawn
*** 1 problem in 2d sprite rendering is rotation. Its a common problem. One, when you rotate a sprite it will get pixelated and look warped. So there are algorithms to dynamically resize the texture aabb and blit a new rotated sprite. Another problem is simply still with the aspect ratio which will simply cause a rotation to look like a shear if you scale with aspect ratio because width and height will be scaled asymettrically
The problem is that in 2D games you dont really want to rotate boxes that much with sprites, without sprites it looks fine but you just want to render the sprite rotated, so its a little different
What im doing is rotating the aabb box itself with the sprite on it. In reality you want to just create a new AABB box and blit the rotated sprite on it

. game objects you want everything to have same proportions relative to eachother, and you dont want to see more of the screen or less so you have to just proportionally scale, it can look weird though if aspect ratio changes but its still correct

. with buttons and ui you can be more lenient, it doesn't have to stay completely relative, you can stretch with screen, anchor, etc.
*/