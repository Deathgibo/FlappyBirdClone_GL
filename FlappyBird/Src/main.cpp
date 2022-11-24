//Code written by: Giovanni Miliziano 11/24/2022

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/matrix_transform.hpp"
#include "ThirdParty/glm/gtx/string_cast.hpp"
#include "ThirdParty/glm/gtc/type_ptr.hpp"

#include <chrono>
#include <array>
#include <unordered_map>
#include <queue>
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>

#ifdef _DEBUG
#define PRINT(x) x
#else
#define PRINT(x)
#endif

std::random_device rd;
std::mt19937 mt(rd());
float GetRandomNumber(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}

#include "Timer.h"
#include "File.h"
#include "Config.h"

#include "Audio.h"
#include "Input.h"
#include "Rect.h"
#include "HelperFunctions.h"

#include "Renderer.h"
#include "Digits.h"
#include "Pipe.h"
#include "Bird.h"
#include "Ground.h"

#include "Menu_Score.h"
#include "Menu_Game.h"
#include "Menu_Main.h"
#include "Menu_Logo.h"

bool Init();
bool InitGLFW();
void LoadTextures();
void GameLoop();
void CleanUp();
void error_callback(int error_code, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

GLFWwindow* window;
int main()
{
	if (Config::GENERATE_DISTRIBUTION_DATA)
	{
		InitAssetFile();
		LoadTextures();
		RenderManager.ExportTexture("GameData/Textures/icon_16.png", true, false);
		RenderManager.ExportTexture("GameData/Textures/icon_32.png", true, false);
		RenderManager.ExportTexture("GameData/Textures/icon_48.png", true, false);
		PRINT(std::cout << "Exporting Textures done...\n");

		AudioManager.ExportAudio();
		PRINT(std::cout << "Exporting Audio done...\n");

		return EXIT_SUCCESS;
	}

	CreateErrorFile();
	CreateGLErrorFile();
	SeeIfStatsFileExists();

	Config::LoadConfigs();
	if (!Init())
	{
		CleanUp();
		return EXIT_FAILURE;
	}
	else
	{
		GameLoop();
		CleanUp();
		return EXIT_SUCCESS;
	}
}

bool Init()
{
	if (!InitGLFW())
		return false;
	if (!RenderManager.Initialize())
		return false;
	AudioManager.Init();
	LoadTextures();

	return true;
}

bool InitGLFW()
{
	if (!glfwInit())
	{
		PRINT(std::cout << "glfw could not initialize!";)
		WriteErrorFile("GLFW failed to initialize. (this is the window library so game cannot be played)");
		return false;
	}

	glfwSetErrorCallback(error_callback);

	//create window
	
	//creation will fail if the OpenGL version of the created context is less than the one requested.It is therefore perfectly safe to use the 
	//default of version 1.0 for legacy code and you will still get backwards - compatible contexts of version 3.0 and above when available	
	if (Config::TEST_OPENGL_VERSION)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, Config::TEST_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, Config::TEST_MINOR_VERSION);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		if(Config::TEST_MAJOR_VERSION <= 2 || (Config::TEST_MAJOR_VERSION == 3 && Config::TEST_MINOR_VERSION <= 2))
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
		else
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //GLFW_OPENGL_COMPAT_PROFILE(3.2 and above), GLFW_OPENGL_CORE_PROFILE(3.2 and above), GLFW_OPENGL_ANY_PROFILE (3.2 and below must be used)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE); //only defined for 3.0 and above
	}
	else
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); //GLFW_OPENGL_COMPAT_PROFILE(3.2 and above), GLFW_OPENGL_CORE_PROFILE(3.2 and above), GLFW_OPENGL_ANY_PROFILE (3.2 and below must be used)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE); //only defined for 3.0 and above
	}
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DOUBLEBUFFER, Config::DOUBLE_BUFFERING);
	glfwWindowHint(GLFW_SAMPLES, 0); 
	glfwWindowHint(GLFW_DEPTH_BITS, 0);
	glfwWindowHint(GLFW_STENCIL_BITS, 0);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_REFRESH_RATE, REFRESH_RATES[Config::REFRESH_RATE]);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);

	//monitor
	GLFWmonitor* monitor = nullptr;
	if (Config::WINDOWTYPE == WINDOW_TYPE::FULLSCREEN)
	{
		monitor = glfwGetPrimaryMonitor();
		if (!monitor)
		{
			PRINT(std::cout << "Monitor doesnt exist?? can't fullscreen" << std::endl;)
		}
		else
		{
			const GLFWvidmode* vid_mode = glfwGetVideoMode(monitor);
			Config::SCREEN_WIDTH = vid_mode->width;
			Config::SCREEN_HEIGHT = vid_mode->height;

			glfwWindowHint(GLFW_RED_BITS, vid_mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, vid_mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, vid_mode->blueBits);

			PRINT(std::cout << "refresh rate: " << vid_mode->refreshRate << std::endl;)

			//print video modes
			/*
				int count = 0;
				const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);
				for (int i = 0; i < count; i++)
				{
					PRINT(std::cout << modes[i].redBits <<" "<<modes[i].greenBits <<" "<<modes[i].blueBits<<" "<< modes[i].width<<" "<< modes[i].height<<" "<< modes[i].refreshRate << std::endl;)
				}
				PRINT(std::cout << glfwGetMonitorName(monitor) << std::endl;)
			*/
		}
	}
	/* Notes on full screen
		
	   Pretty much the operating system has a fullscreen mode, which pretty much makes it so that your application
	   is the only one of the monitor vs windowed mode. The main reason for this is refresh rate. If you are on windowed,
	   then refresh rate is just the current one because you can't just single out that application to refresh. Fullscreen
	   you have power to set refresh rate though because its the only one of the whole screen.

	   Refresh rate won't slow down your program, it doesn't know what refresh rate your monitor is. V-sync will because
	   it actually stalls your computer for the refresh rate length. If you set refresh rate your program will still
	   refesh at that length on the monitor, you might get some screen tearing because your application doesn't wait for that.

	   glfwSwapBuffers() - This function stalls the GPU driver to wait a specified number of screen updates before
	   swapping buffer. So if its 0 it won't stall, if its not 0 it will stall. A screen update refers to the refresh
	   rate. So if refresh had is 60hz it will stall for 60 fps or 16 milliseconds.
	   **The important thing to note here is that v-sync doesn't make it 60 fps the stall time is dependant on the refresh
	   rate, higher refresh rate stalls it less. And of course how many intervals you want to stall it for
	*/

	PRINT(std::cout << "width: " << Config::SCREEN_WIDTH << " height: " << Config::SCREEN_HEIGHT << std::endl;)
	window = glfwCreateWindow(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, "Flappy Bird", monitor, NULL);
	
	if (!window)
	{
		PRINT(std::cout << "Could not create glfw Window!";)
		WriteErrorFile("Failed to create GLFW window. (no window game can't be played)");
		return false;
	}

	if (Config::WINDOWTYPE == WINDOW_TYPE::FULLSCREEN)
	{
		//set refresh rate
		glfwSetWindowMonitor(window, glfwGetWindowMonitor(window), 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, REFRESH_RATES[Config::REFRESH_RATE]);
	}


	glfwMakeContextCurrent(window);

	if (Config::V_SYNC)
		glfwSwapInterval(Config::SWAP_INTERVAL_DEBUG);
	else
		glfwSwapInterval(0);

	//set input callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	//set window icon
	GLFWimage images[3];
	int bitDepth = -1;
	if (Config::LOAD_FROM_DISTRIBUTION)
	{
		images[0].pixels = ReadTextureFromAssetFile("GameData/Textures/icon_16.png", images[0].width, images[0].height);
		images[1].pixels = ReadTextureFromAssetFile("GameData/Textures/icon_32.png", images[1].width, images[1].height);
		images[2].pixels = ReadTextureFromAssetFile("GameData/Textures/icon_48.png", images[2].width, images[2].height);
	}
	else
	{
		images[0].pixels = stbi_load("GameData/Textures/icon_16.png", &images[0].width, &images[0].height, &bitDepth, 0);
		images[1].pixels = stbi_load("GameData/Textures/icon_32.png", &images[1].width, &images[1].height, &bitDepth, 0);
		images[2].pixels = stbi_load("GameData/Textures/icon_48.png", &images[2].width, &images[2].height, &bitDepth, 0);
	}
	if (images[0].pixels == nullptr || images[1].pixels == nullptr || images[2].pixels == nullptr) {
		PRINT(std::cout << "can't load GameData/Textures/icon.png" << std::endl;)
		if (Config::LOAD_FROM_DISTRIBUTION)
		{
			if (images[0].pixels)
				delete[] images[0].pixels;
			if (images[1].pixels)
				delete[] images[1].pixels;
			if (images[2].pixels)
				delete[] images[2].pixels;
		}
		else
		{
			if (images[0].pixels)
				stbi_image_free(images[0].pixels);
			if (images[1].pixels)
				stbi_image_free(images[1].pixels);
			if (images[2].pixels)
				stbi_image_free(images[2].pixels);
		}
	}
	else
	{
		glfwSetWindowIcon(window, 3, images);
		if (Config::LOAD_FROM_DISTRIBUTION)
		{
			if (images[0].pixels)
				delete[] images[0].pixels;
			if (images[1].pixels)
				delete[] images[1].pixels;
			if (images[2].pixels)
				delete[] images[2].pixels;
		}
		else
		{
			if (images[0].pixels)
				stbi_image_free(images[0].pixels);
			if (images[1].pixels)
				stbi_image_free(images[1].pixels);
			if (images[2].pixels)
				stbi_image_free(images[2].pixels);
		}
	}

	return true;
}

void GameLoop()
{
	//main classes
	MENU_TYPE current_menu = MENU_TYPE::COMPANY_LOGO;
	Menu_Main menu_main;
	Menu_Logo menu_logo;
	Menu_Score menu_score;
	Menu_Game menu_game;

	//update loop
	//This is the fixed timestep of our simulation loop in milliseconds. We simulate in fixed steps, the renderer will interpolate the remainders.
	//.01666 is 1/60th of a frame. so a little less than that. Too fast and slow computers might not be able to catch up.
	const double MS_PER_UPDATE = 10.0; //10.0
	//This is the max time in milliseconds that we will simulate per frame. If computer is super slow and reaches this, it will go out of sync.
	const double MS_MAX_TIME = 1000.0; //1000.0
	//accumulated time passed that we need to simulate
	double lag = 0.0f;
	auto current_time = std::chrono::high_resolution_clock::now();

	//transition variables
	std::pair<bool, MENU_TYPE> transition_info;
	MENU_TYPE transition_menu;
	const int transition_first_timer = 60;
	const int transition_first_black_timer = 20;
	static_assert(0 <= transition_first_black_timer && transition_first_black_timer <= transition_first_timer);
	const int transition_second_timer = 10;
	bool transitioning_first = false;
	bool transitioning_second = false;
	int transition_first_count = 0;
	int transition_second_count = 0;
	float transition_ablend = 0.0f;

	Timer fps_timer;
	if (Config::DISPLAY_FPS)
		fps_timer.Begin();
	while (!(InputManager.EscapeDown() || glfwWindowShouldClose(window)))
	{
		//calculate how long last frame took to process in milliseconds and store in frameTime
		auto new_time = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(current_time).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(new_time).time_since_epoch().count();
		double frameTime = end - start;
		//microseconds to milliseconds
		frameTime /= 1000.0;
		if (frameTime > MS_MAX_TIME) {
			frameTime = MS_MAX_TIME;
			PRINT(std::cout << "Warning: max time reached. processing " << std::to_string(int(std::round(MS_MAX_TIME / 10.0))) << " update frames\n";)
		}
		current_time = new_time;

		
		if (Config::DISPLAY_FPS && window)
		{
			//Interesing Note: If you run this with vsync off and you get like 1000+ frames a second, this will
			//freeze the windows task bar. I guess Windows isn't used to getting a window rename 1000+ times a second, or
			//its completely on GLFW's end. I'll just run every 10 milliseconds instead to not lock it up.
			if (fps_timer.TimeElapsed(10))
			{
				std::string title = "Flappy Bird " + calculateFPS(frameTime) + " FPS";
				glfwSetWindowTitle(window, title.c_str());
				fps_timer.Begin();
			}
		}
		
		lag += frameTime;

		InputManager.HandleInput();

		transition_info.first = false;
		switch (current_menu)
		{
		case MENU_TYPE::COMPANY_LOGO:   break;
		case MENU_TYPE::MAIN:			menu_main.Input(transition_info);    break;
		case MENU_TYPE::SCORE:          menu_score.Input(transition_info);   break;
		case MENU_TYPE::GAMELOOP:       menu_game.Input(transition_info);    break;
		default: PRINT(std::cout << "invalid Menu type??";) std::abort(); break;
		}
		if (!transitioning_second && !transitioning_first && transition_info.first)
		{
			transition_menu = transition_info.second;
			transition_ablend = 0.0f;
			transitioning_first = true;
			InputManager.Disable(true);
			AudioManager.playSound(SOUND::TRANSITION);
		}
	
		while (lag >= MS_PER_UPDATE)
		{
			//simulate physics
			switch (current_menu)
			{
			case MENU_TYPE::COMPANY_LOGO:   menu_logo.Update(current_menu, menu_main);	break;
			case MENU_TYPE::MAIN:			menu_main.Update();    break;
			case MENU_TYPE::SCORE:          menu_score.Update();   break;
			case MENU_TYPE::GAMELOOP:       menu_game.Update();    break;
			default: PRINT(std::cout << "invalid Menu type??";) std::abort(); break;
			}
			lag -= MS_PER_UPDATE;
			
			//handle transition
			if (transitioning_first)
			{
				transition_first_count = std::min(transition_first_count + 1, transition_first_timer);
				transition_ablend = std::clamp((float)transition_first_count / (float)transition_first_black_timer, 0.0f,1.0f);
				if (transition_first_count >= transition_first_timer)
				{
					current_menu = transition_menu;
					switch (current_menu)
					{
					case MENU_TYPE::COMPANY_LOGO:   break;
					case MENU_TYPE::MAIN:			menu_main.Init();    break;
					case MENU_TYPE::SCORE:          menu_score.Init();   break;
					case MENU_TYPE::GAMELOOP:       menu_game.Init();    break;
					default: PRINT(std::cout << "invalid Menu type??";) std::abort(); break;
					}
					InputManager.Disable(false);
					transitioning_first = false;
					transitioning_second = true;
				}
			}
			if (transitioning_second)
			{
				transition_second_count = std::min(transition_second_count + 1, transition_second_timer);
				transition_ablend = std::clamp(1.0f - ((float)transition_second_count / (float)transition_second_timer), 0.0f,1.0f);
				if (transition_second_count >= transition_second_timer)
				{
					transitioning_second = false;
					transition_first_count = 0;
					transition_second_count = 0;
				}
			}
		}

		//how much the renderer needs to interpolate, because physics simulate in a fixed step
		const float a_blend = std::clamp(lag / MS_PER_UPDATE, 0.0, 1.0);

		RenderManager.Begin();
		switch (current_menu)
		{
		case MENU_TYPE::COMPANY_LOGO:   menu_logo.Render(a_blend);	  break;
		case MENU_TYPE::MAIN:			menu_main.Render(a_blend);    break;
		case MENU_TYPE::SCORE:          menu_score.Render(a_blend);   break;
		case MENU_TYPE::GAMELOOP:       menu_game.Render(a_blend);    break;
		default: PRINT(std::cout << "invalid Menu type??";) std::abort(); break;
		}
		//transition overlay
		if(transitioning_first || transitioning_second)
			RenderManager.RenderRect(texture_default, glm::mat4(1.0f), glm::vec4(0, 0, 0, transition_ablend), false);
		RenderManager.End();

		glfwSwapBuffers(window);
	}

	AudioManager.stopAllSound();
}

void LoadTextures()
{
	RenderManager.CreateTexture(texture_default, "GameData/Textures/default.jpg", true);
	RenderManager.CreateTexture(texture_logomenu, "GameData/Textures/gibologo.png", true);
	RenderManager.CreateTexture(texture_pause, "GameData/Textures/paused.png", true);
	RenderManager.CreateTexture(texture_play, "GameData/Textures/play.png", true);
	RenderManager.CreateTexture(texture_studios, "GameData/Textures/gibo_studios.png", true);
	RenderManager.CreateTexture(texture_menu, "GameData/Textures/menu.png", true);
	RenderManager.CreateTexture(texture_background, "GameData/Textures/flappy.png", true);
	RenderManager.CreateTexture(texture_ground, "GameData/Textures/base.png", true);
	RenderManager.CreateTexture(texture_pipes, "GameData/Textures/pipe-green.png", true);
	RenderManager.CreateTexture(texture_bird[0], "GameData/Textures/yellowbird-downflap.png", true);
	RenderManager.CreateTexture(texture_bird[1], "GameData/Textures/yellowbird-midflap.png", true);
	RenderManager.CreateTexture(texture_bird[2], "GameData/Textures/yellowbird-upflap.png", true);
	RenderManager.CreateTexture(texture_getready, "GameData/Textures/get_ready.png", true);
	RenderManager.CreateTexture(texture_flappybird, "GameData/Textures/flappybird.png", true);
	RenderManager.CreateTexture(texture_tap, "GameData/Textures/tap.png", true);
	RenderManager.CreateTexture(texture_start, "GameData/Textures/start.png", true);
	RenderManager.CreateTexture(texture_score, "GameData/Textures/score.jpg", true);
	RenderManager.CreateTexture(texture_gameover, "GameData/Textures/gameover.png", true);
	RenderManager.CreateTexture(texture_overback, "GameData/Textures/overback.png", true);
	RenderManager.CreateTexture(texture_ok, "GameData/Textures/ok.png", true);
	RenderManager.CreateTexture(texture_share, "GameData/Textures/share.png", true);
	RenderManager.CreateTexture(texture_sharedark, "GameData/Textures/share_dark.png", true);
	RenderManager.CreateTexture(texture_medals[3], "GameData/Textures/medal_plat.png", true);
	RenderManager.CreateTexture(texture_medals[2], "GameData/Textures/medal_gold.png", true);
	RenderManager.CreateTexture(texture_medals[1], "GameData/Textures/medal_silver.png", true);
	RenderManager.CreateTexture(texture_medals[0], "GameData/Textures/medal_bronze.png", true);
	RenderManager.CreateTexture(texture_numbers[0], "GameData/Textures/0.png", true);
	RenderManager.CreateTexture(texture_numbers[1], "GameData/Textures/1.png", true);
	RenderManager.CreateTexture(texture_numbers[2], "GameData/Textures/2.png", true);
	RenderManager.CreateTexture(texture_numbers[3], "GameData/Textures/3.png", true);
	RenderManager.CreateTexture(texture_numbers[4], "GameData/Textures/4.png", true);
	RenderManager.CreateTexture(texture_numbers[5], "GameData/Textures/5.png", true);
	RenderManager.CreateTexture(texture_numbers[6], "GameData/Textures/6.png", true);
	RenderManager.CreateTexture(texture_numbers[7], "GameData/Textures/7.png", true);
	RenderManager.CreateTexture(texture_numbers[8], "GameData/Textures/8.png", true);
	RenderManager.CreateTexture(texture_numbers[9], "GameData/Textures/9.png", true);
	RenderManager.CreateTexture(texture_recent, "GameData/Textures/recent.png", true);
	RenderManager.CreateTexture(texture_top, "GameData/Textures/top.png", true);
	RenderManager.CreateTexture(texture_dot, "GameData/Textures/white.png", true);
}

void error_callback(int error_code, const char* description)
{
	PRINT(std::cout << "GLFW ERROR " << error_code << ": " << description << std::endl);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	RenderManager.framebuffer_size_callback(window, width, height);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	InputManager.key_callback(window, key, scancode, action, mods);
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	InputManager.cursor_position_callback(window, xpos, ypos);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	InputManager.mouse_button_callback(window, button, action, mods);
}

void CleanUp()
{
	RenderManager.CleanUp();
	glfwTerminate();
	window = nullptr;
}