#pragma once

enum class WINDOW_TYPE : uint8_t { FULLSCREEN, WINDOWED };
enum class MENU_TYPE : uint8_t { COMPANY_LOGO, MAIN, SCORE, GAMELOOP };
enum class TEXTURE_QUALITY : uint8_t { LOW, HIGH };
std::array<int, 5> REFRESH_RATES = { 60, 100, 120, 144, 240 };

//750 x 1000 default window size. 633 x 847
namespace Config
{
	//Internal config variables
	bool override_config = false;
	bool DOUBLE_BUFFERING = true;
	int SWAP_INTERVAL_DEBUG = 1; //should be 1. Used to slow down program for testing purposes. 
	bool TEST_VELOCITY = false; //multiplies all velocities by a bigger factor to show rendering interpolation at work
	float TEST_VELOCITY_FACTOR = 10.0f; //multiplying factor

	bool TEST_HARD_PIPES = false; //will alternate between the farthest apart pipes
	bool TEST_INVINCIBILITY = false; //player won't ever die

	//will try and force OpenGL context to specified version
	bool TEST_OPENGL_VERSION = false;
	int TEST_MAJOR_VERSION = 3;
	int TEST_MINOR_VERSION = 1;
	
	bool LOAD_FROM_DISTRIBUTION = true; //whether we are loading from our developer game resources or the distributed ones
	bool GENERATE_DISTRIBUTION_DATA = false; //this will generate the texture and audio distribution data and close program

	//User config variables
	int MULTISAMPLE_COUNT = 2;
	TEXTURE_QUALITY TEXTUREQUALITY = TEXTURE_QUALITY::HIGH;
	WINDOW_TYPE WINDOWTYPE = WINDOW_TYPE::WINDOWED;
	int SCREEN_WIDTH = 633;
	int SCREEN_HEIGHT = 847;
	//Resolution: resolution is whatever the screen size is
	int REFRESH_RATE = 3; //0:60, 1:100, 2:120, 3:144, 4:240 .Refresh rate doesnt matter if its windowed
	bool V_SYNC = true;
	bool ENABLE_AUDIO = true;
	float MASTER_VOLUME = 0.25;
	bool DISPLAY_FPS = false;
	
	void LoadConfigs()
	{
		if (override_config)
			return;

		SeeIfConfigFileExists();

		std::array<int, 10> config_values = ReadConfigFile();
		if (config_values[config_values.size()-1] != -1)
		{
			//WINDOW_TYPE
			if (config_values[0] == 0) WINDOWTYPE = WINDOW_TYPE::WINDOWED;
			if (config_values[0] == 1) WINDOWTYPE = WINDOW_TYPE::FULLSCREEN;

			//SCREEN_WIDTH
			SCREEN_WIDTH = config_values[1];

			//SCREEN_HEIGHT
			SCREEN_HEIGHT = config_values[2];

			//REFRESH_RATE
			if (config_values[3] >= 0 && config_values[3] <= 4)
				REFRESH_RATE = config_values[3];

			//VSYNC
			if (config_values[4] == 0) V_SYNC = false;
			if (config_values[4] == 1) V_SYNC = true;

			//MULTISAMPLE_COUNT
			if (config_values[5] >= 0 && config_values[5] <= 16) MULTISAMPLE_COUNT = config_values[5];

			//TEXTURE_QUALITY
			if (config_values[6] == 0) TEXTUREQUALITY = TEXTURE_QUALITY::LOW;
			if (config_values[6] == 1) TEXTUREQUALITY = TEXTURE_QUALITY::HIGH;

			//ENABLE_AUDIO
			if (config_values[7] == 0) ENABLE_AUDIO = false;
			if (config_values[7] == 1) ENABLE_AUDIO = true;

			//MASTER_VOLUME
			config_values[8] = std::clamp(config_values[8], 0, 100);
			MASTER_VOLUME = config_values[8] / 100.0f;
			
			//DISPLAY_FPS
			if (config_values[9] == 0) DISPLAY_FPS = false;
			if (config_values[9] == 1) DISPLAY_FPS = true;

			PRINT(std::cout << "Successfully loaded config file\n";)
		}
		else
		{
			PRINT(std::cout << "Failed to load config file\n";)
			WriteErrorFile("Config File Failed to load. (using default values)");
			CreateDefaultConfigFile();
		}
	}
}
