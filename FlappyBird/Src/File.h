#pragma once

//HELPER FUNCTIONS
bool stoi_exceptions(std::string str, int& out_val)
{
	try {
		out_val = std::stoi(str);
	}
	catch (const std::invalid_argument& ia) {
		std::cerr << "Invalid argument: " << ia.what() << std::endl;
		return false;
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what() << std::endl;
		return false;
	}
	return true;
}

std::array<unsigned char, 4> InttoU(int val)
{
	std::array<unsigned char, 4> bytes;

	bytes[0] = (val >> 24) & 0xFF;
	bytes[1] = (val >> 16) & 0xFF;
	bytes[2] = (val >> 8) & 0xFF;
	bytes[3] = (val >> 0) & 0xFF;

	return bytes;
}

int UtoInt(std::array<unsigned char, 4> bytes)
{
	return (bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3]);
}

int hash_function(int val)
{
	//input is 0-999

	std::array<unsigned char, 4> bytes = InttoU(val);
	
	bytes[0] |= 0x45;
	bytes[1] &= 0x10;
	bytes[2] <<= 2;
	bytes[3] ^= 0x31;

	unsigned char save = bytes[0];
	bytes[0] = bytes[2];
	bytes[2] = save;
	save = bytes[1];
	bytes[1] = bytes[3];
	bytes[3] = save;

	int mapped_int = UtoInt(bytes);

	return mapped_int;
}

//STATS FILE
const std::string stats_file_name = "DistributionData/áthiktos.gib";

void WriteStatsFile(const std::array<int, 8>& data)
{
	std::fstream stats_file;
	stats_file.open(stats_file_name, std::fstream::out | std::fstream::binary);
	if (stats_file.is_open())
	{
		for (int i = 0; i < data.size(); i++)
		{
			//generate hash value and store after number
			int hash_value = hash_function(data[i]);
			//generate 3 random numbers to append
			int random_number = GetRandomNumber(100, 999);

			std::array<unsigned char, 4> score_bytes = InttoU(data[i]);
			std::array<unsigned char, 4> hash_bytes = InttoU(hash_value);
			std::array<unsigned char, 4> random_bytes = InttoU(random_number);

			stats_file.write((char*)score_bytes.data(), 4);
			stats_file.write((char*)hash_bytes.data(), 4);
			stats_file.write((char*)random_bytes.data(), 4);

			if (stats_file.bad() || stats_file.fail())
			{
				PRINT(std::cout << "file opened but failed writing to file\n";)
				return;
			}
		}
	}
	else
	{
		PRINT(std::cout << "failed to open " << stats_file_name << std::endl;)
	}
}


void CreateDefaultStatsFile()
{
	std::array<int, 8> default_vals;
	default_vals.fill(0);
	WriteStatsFile(default_vals);
}

void SeeIfStatsFileExists()
{
	std::fstream stats_file(stats_file_name);
	if (!stats_file.good())
	{
		CreateDefaultStatsFile();
	}
}

std::array<int, 8> ReadStatsFile()
{
	std::array<int, 8> scores;
	scores.fill(-1);
	bool cheater_caught = false;

	std::fstream stats_file;
	stats_file.open(stats_file_name, std::fstream::in | std::fstream::binary);
	if (stats_file.is_open())
	{
		int counter = 0;
		std::array<unsigned char, 4> int_binary;
		while (!stats_file.eof() && counter < 8)
		{
			//score
			stats_file.read((char*)int_binary.data(), sizeof(int));
			scores[counter] = UtoInt(int_binary);

			//hash val
			stats_file.read((char*)int_binary.data(), sizeof(int));
			int hash_value = UtoInt(int_binary);

			//random number
			stats_file.read((char*)int_binary.data(), sizeof(int));
			int random = UtoInt(int_binary);

			//check has value
			if (hash_function(scores[counter]) != hash_value)
			{
				//someone tampered with file. overwrite with 0
				PRINT(std::cout << "cheater caught!\n");
				cheater_caught = true;
				break;
			}

			counter++;
		}
	}
	else
	{
		PRINT(std::cout << "failed to open " << stats_file_name << std::endl;)
			//file can't be opened or not found, so create a new one thats default
			return scores;
	}

	if (cheater_caught)
	{
		scores.fill(-1);
		stats_file.close();
		CreateDefaultStatsFile();
	}
	return scores;
}

//CONFIG FILE
std::string default_config_text =
"WINDOW_TYPE= 0          //0-WINDOWED, 1-FULLSCREEN\n"
"SCREEN_WIDTH= 633       //(only applicable on windowed mode)\n"
"SCREEN_HEIGHT= 847      //(only applicable on windowed mode)\n"
"REFRESH_RATE= 3         //0-60, 1-100, 2-120, 3-144, 4-240 (only applicable on fullscreen mode)\n"
"VSYNC= 1                //0-disable, 1-enable\n"
"MULTISAMPLE_COUNT= 2    //values usually range between 0 and 16\n"
"TEXTURE_QUALITY= 1      //0-Low, 1-High\n"
"ENABLE_AUDIO= 1         //0-DISABLE, 1-ENABLE\n"
"MASTER_VOLUME= 25       //0 is silent, 100 full volume\n"
"DISPLAY_FPS= 0          //0-DISABLE, 1-ENABLE\n"
"\n"
"***Only change values in this file.\n"
"If anything else changes default values\n"
"will be used and file will be recreated.\n"
"If you want to reset just destroy file ***\n";

const std::string config_file_name = "Config.txt";

void CreateDefaultConfigFile()
{
	std::fstream config_file;
	config_file.open(config_file_name, std::fstream::out);
	if (config_file.is_open())
	{
		config_file.write(default_config_text.data(), default_config_text.size());
		if (config_file.bad() || config_file.fail())
		{
			PRINT(std::cout << "failed to create default config file\n";)
		}
	}
}

void SeeIfConfigFileExists()
{
	std::fstream config_file(config_file_name);
	if (!config_file.good())
	{
		CreateDefaultConfigFile();
	}
}

std::array<int, 10> ReadConfigFile()
{
	std::array<int, 10> config_values;
	config_values.fill(-1);

	std::fstream config_file;
	config_file.open(config_file_name, std::fstream::in);
	if (config_file.is_open())
	{
		int counter = 0;
		std::string out_string;
		while (!config_file.eof() && counter < config_values.size())
		{
			//get string up to =
			std::getline(config_file, out_string, '=');
			if (config_file.bad() || config_file.fail())
			{
				PRINT(std::cout << "file opened but getline failed\n";)
				return config_values;
			}

			//validate it with counter
			switch (counter)
			{
			case 0: if (out_string != "WINDOW_TYPE") {
				PRINT(std::cout << "WINDOW_TYPE error while parsing\n";)       return config_values;}    break;
			case 1: if (out_string != "SCREEN_WIDTH") {
				PRINT(std::cout << "SCREEN_WIDTH error while parsing\n";)      return config_values;}    break;
			case 2: if (out_string != "SCREEN_HEIGHT") {
				PRINT(std::cout << "SCREEN_HEIGHT error while parsing\n";)     return config_values;}    break;
			case 3: if (out_string != "REFRESH_RATE") {
				PRINT(std::cout << "REFRESH_RATE error while parsing\n";)      return config_values;}    break;
			case 4: if (out_string != "VSYNC") {
				PRINT(std::cout << "VSYNC error while parsing\n";)             return config_values;}    break;
			case 5: if (out_string != "MULTISAMPLE_COUNT") {
				PRINT(std::cout << "MULTISAMPLE_COUNT error while parsing\n";) return config_values;}    break;
			case 6: if (out_string != "TEXTURE_QUALITY") {
				PRINT(std::cout << "TEXTURE_QUALITY error while parsing\n";)   return config_values;}    break;
			case 7: if (out_string != "ENABLE_AUDIO") {
				PRINT(std::cout << "ENABLE_AUDIO error while parsing\n";)      return config_values;}    break;
			case 8: if (out_string != "MASTER_VOLUME") {
				PRINT(std::cout << "MASTER_VOLUME error while parsing\n";)     return config_values;}    break;
			case 9: if (out_string != "DISPLAY_FPS") {
				PRINT(std::cout << "DISPLAY_FPS error while parsing\n";)       return config_values;}    break;
			default: break;
			};

			//get string up to /
			std::getline(config_file, out_string, '/');
			if (config_file.bad() || config_file.fail())
			{
				PRINT(std::cout << "file opened but getline failed\n";)
				return config_values;
			}

			//convert that to a number
			if (!stoi_exceptions(out_string, config_values[counter]))
			{
				return config_values;
			}

			//skip everything and go nextline
			std::getline(config_file, out_string);
			if (config_file.bad() || config_file.fail())
			{
				PRINT(std::cout << "file opened but getline failed\n";)
				return config_values;
			}
		
			counter++;
		}
	}
	else
	{
		PRINT(std::cout << "failed to open " << config_file_name << std::endl;)
		return config_values;
	}

	return config_values;
}

//ERROR LOG FILE
const std::string error_file_name = "Logs/Error_Log.txt";

void CreateErrorFile()
{
	std::fstream error_file;
	error_file.open(error_file_name, std::fstream::out);
	if (!error_file.is_open())
	{
		PRINT(std::cout << "error file failed to be created\n";)
			return;
	}
}

void WriteErrorFile(std::string error_message)
{
	std::fstream error_file;
	error_file.open(error_file_name, std::fstream::app);
	if (error_file.is_open())
	{
		error_file << error_message << std::endl;
		if (error_file.bad() || error_file.fail())
		{
			PRINT(std::cout << "error file opened but failed writing to file\n";)
				return;
		}
	}
	else
	{
		PRINT(std::cout << "failed to open " << error_file_name << std::endl;)
	}
}

//GL_ERROR LOG FILE
const std::string glerror_file_name = "Logs/GL_Error.txt";
std::queue<std::string> gl_errors;
const int max_queue_size = 100;
const int max_file_writes = 1000;
int current_file_writes = 0;
void pushGLError(const std::string& glerror)
{
	if(gl_errors.size() < max_queue_size)
		gl_errors.push(glerror);
}

void CreateGLErrorFile()
{
	std::fstream glerror_file;
	glerror_file.open(glerror_file_name, std::fstream::out);
	if (!glerror_file.is_open())
	{
		PRINT(std::cout << "glerror file failed to be created\n";)
			return;
	}
}

void GLErrorFileWrite()
{
	std::fstream glerror_file;
	glerror_file.open(glerror_file_name, std::fstream::app);
	if (glerror_file.is_open())
	{
		int counter = 0;
		while (!gl_errors.empty() && counter < max_queue_size && current_file_writes < max_file_writes)
		{
			glerror_file << gl_errors.front() << std::endl;
			gl_errors.pop();
			if (glerror_file.bad() || glerror_file.fail())
			{
				PRINT(std::cout << "glerror file opened but failed writing to file\n";)
					return;
			}
			counter++;
			current_file_writes++;
		}
	}
	else
	{
		PRINT(std::cout << "failed to open " << error_file_name << std::endl;)
	}
}


//ASSETS
const std::string asset_file_name = "DistributionData/eikóna.gib";

void InitAssetFile()
{
	std::fstream asset_file;
	asset_file.open(asset_file_name, std::fstream::out | std::fstream::binary);
	asset_file.close();
}

//<string><int><int><int><unsigned char*>
//<64><4><4><4><x>
//<filename><width><height><texdata_size><texdata>
unsigned char* ReadTextureFromAssetFile(const char* fileLocation, int& width_out, int& height_out)
{
	std::string desired_file_name(fileLocation);
	desired_file_name.resize(64);

	std::fstream asset_file;
	asset_file.open(asset_file_name, std::fstream::in | std::fstream::binary);
	if (asset_file.is_open())
	{
		std::array<char, 64> string_binary;
		std::array<unsigned char, 4> int_binary;
		int offset = 0;
		while (!asset_file.eof())
		{
			//FILE NAME
			asset_file.read(string_binary.data(), string_binary.size());
			std::string found_file_name;
			found_file_name.resize(64);
			for (int i = 0; i < found_file_name.size(); i++)
			{
				found_file_name[i] = (string_binary[i] - 100) % 256;
			}

			//WIDTH
			asset_file.read((char*)int_binary.data(), sizeof(int));
			int width = UtoInt(int_binary);

			//HEIGHT
			asset_file.read((char*)int_binary.data(), sizeof(int));
			int height = UtoInt(int_binary);

			//TEXTURE SIZE
			asset_file.read((char*)int_binary.data(), sizeof(int));
			int texturesize = UtoInt(int_binary);

			if (asset_file.bad() || asset_file.fail())
			{
				PRINT(std::cout << "asset file opened but failed writing to file\n";)
				return nullptr;
			}
			//we read string and 3 ints. If strings match parse texData, else skip ahead
			if (desired_file_name == found_file_name)
			{
				//PIXEL DATA
				char* texData = new char[texturesize];
				asset_file.read(texData, texturesize);
				if (asset_file.bad() || asset_file.fail())
				{
					PRINT(std::cout << "asset file opened but failed writing to file\n";)
					return nullptr;
				}
				//output
				width_out = width;
				height_out = height;
				return (unsigned char*)texData;
			}

			offset += 64 + sizeof(int) * 3 + texturesize;
			asset_file.seekg(offset);
		}
	}
	else
	{
		PRINT(std::cout << "failed to open " << asset_file_name << std::endl;)
		return nullptr;
	}
}

void WriteAssetFile(unsigned char* bits, size_t size)
{
	std::fstream asset_file;
	asset_file.open(asset_file_name, std::fstream::app | std::fstream::binary);
	if (asset_file.is_open())
	{
		for (int i = 0; i < size; i++)
		{
			asset_file << bits[i];
			if (asset_file.bad() || asset_file.fail())
			{
				PRINT(std::cout << "asset file opened but failed writing to file\n";)
					return;
			}
		}
	}
	else
	{
		PRINT(std::cout << "failed to open " << asset_file_name << std::endl;)
	}
}

//SOUND ASSET
const std::string audio_asset_file_name = "DistributionData/íchou.gib";
void InitAudioFile()
{
	std::fstream asset_file;
	asset_file.open(audio_asset_file_name, std::fstream::out | std::fstream::binary);
	asset_file.close();
}

//format is <file_name><data_size><data>
//          <string><int><unsigned char*>
unsigned char* ReadAudioFile(std::string file, int& len_out)
{
	std::string desired_file_name(file);
	desired_file_name.resize(64);

	//write to new asset file
	std::fstream asset_file;
	asset_file.open(audio_asset_file_name, std::ios::in | std::ios::binary);
	if (asset_file.is_open())
	{
		std::array<char, 64> string_binary;
		std::array<unsigned char, 4> int_binary;
		int offset = 0;
		while (!asset_file.eof())
		{
			//FILE NAME
			asset_file.read(string_binary.data(), string_binary.size());
			std::string found_file_name;
			found_file_name.resize(64);
			for (int i = 0; i < found_file_name.size(); i++)
			{
				found_file_name[i] = (string_binary[i] - 100) % 256;
			}

			//WAV SIZE
			asset_file.read((char*)int_binary.data(), sizeof(int));
			int wav_length = UtoInt(int_binary);

			if (asset_file.bad() || asset_file.fail())
			{
				PRINT(std::cout << "sound asset file opened but failed writing to file\n";)
				return nullptr;
			}
			if (desired_file_name == found_file_name)
			{
				//AUDIO DATA
				char* wavData = new char[wav_length];
				asset_file.read(wavData, wav_length);
				if (asset_file.bad() || asset_file.fail())
				{
					PRINT(std::cout << "sound asset file opened but failed writing to file\n";)
					return nullptr;
				}

				//output
				len_out = wav_length;
				return (unsigned char*)wavData;
			}

			offset += 64 + sizeof(int) + wav_length;
			asset_file.seekg(offset);
		}
	}
	else
	{
		PRINT(std::cout << "failed to read from sound asset file\n";)
	}
	return nullptr;
}

void WriteAudioFile(std::string file_name)
{
	//Open and Copy data from wav file
	std::fstream wav_file;
	wav_file.open(file_name, std::ios::in | std::ios::binary);
	unsigned char* wav_data = nullptr;
	size_t len = 0;
	if (wav_file.is_open())
	{
		wav_file.seekg(0, std::ios::end);
		len = wav_file.tellg();
		wav_file.seekg(0, std::ios::beg);
		wav_data = new unsigned char[len];

		wav_file.read((char*)&wav_data[0], len);
		if (wav_file.bad() || wav_file.fail())
		{
			delete[] wav_data;
			PRINT(std::cout << "wave file opened but failed writing to file\n";)
			return;
		}
	}
	else
	{
		PRINT(std::cout << "failed to open wav file. cant export\n";)
		return;
	}

	//calculate output string
	std::string file_name_final = file_name;
	file_name_final.resize(64);
	for (int i = 0; i < file_name_final.size(); i++)
	{
		file_name_final[i] = (file_name_final[i] + 100) % 256;
	}
	std::array<unsigned char, 4> wav_bytes = InttoU(len);

	//write to new asset file
	std::fstream asset_file;
	asset_file.open(audio_asset_file_name, std::ios::app | std::ios::binary);
	if (asset_file.is_open())
	{
		asset_file.write(file_name_final.data(), file_name_final.size()); //file name
		asset_file.write((char*)wav_bytes.data(), wav_bytes.size());      //wav data size
		asset_file.write((char*)&wav_data[0], len);                       //wav data
		if (asset_file.bad() || asset_file.fail())
		{
			delete[] wav_data;
			PRINT(std::cout << "failed to write to sound asset file\n";)
			return;
		}
	}
	else
	{
		PRINT(std::cout << "failed to write to sound asset file\n";)
	}

	delete[] wav_data;
}