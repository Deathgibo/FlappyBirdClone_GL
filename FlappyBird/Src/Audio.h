#pragma once
#include <irrKlang.h>

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

enum SOUND : int { FALL, HIT, POINT, TRANSITION, WING, NABOKOV };
class Audio
{
private:
	bool init = false;
	irrklang::ISoundEngine* soundEngine;
	std::array<irrklang::ISoundSource*, 6> sounds;

	//between 0.0(silent) and 1.0f(full volume)
	const float fall_volume = 1.0f;
	const float hit_volume = 1.0f;
	const float point_volume = 1.0f;
	const float transition_volume = 1.0f;
	const float wing_volume = 1.0f;
	const float nabokov_volume = 1.0f;
public:
	Audio()
	{
		soundEngine = nullptr;
		for (int i = 0; i < sounds.size(); i++)
		{
			sounds[i] = nullptr;
		}
	}

	~Audio()
	{
		if (soundEngine)
		{
			//memory for soundsource is already handled by engine
			soundEngine->drop();
		}
	}

	void playSound(SOUND sound)
	{
		if (soundEngine && sounds[sound])
		{
			//some reason this call is memory leaking, I don't know why. I think its creating a sound source every time? I could
			//delete all sound sources and create again. Note to self: dont use this library!
			//reason for leak is that it never says the sound stops. So isplaying always returns true and I guess it doesn't free memory.
			soundEngine->play2D(sounds[sound]);
		}
	}

	bool isPlaying(SOUND sound)
	{
		if (soundEngine && sounds[sound])
		{
			return soundEngine->isCurrentlyPlaying(sounds[sound]);
		}
		return false;
	}

	void stopAllSound()
	{
		if (soundEngine)
		{
			soundEngine->stopAllSounds();
		}
	}

	void Init()
	{
		if (!Config::ENABLE_AUDIO)
		{
			return;
		}

		if (init)
			return;
		init = true;

#ifdef _DEBUG
		soundEngine = irrklang::createIrrKlangDevice(irrklang::ESOD_AUTO_DETECT, irrklang::E_SOUND_ENGINE_OPTIONS::ESEO_PRINT_DEBUG_INFO_TO_STDOUT);
#else
		soundEngine = irrklang::createIrrKlangDevice();
#endif
		if (!soundEngine)
		{
			PRINT(std::cout << "can't create sound engine, no audio...\n";)
			WriteErrorFile("Audio Player failed to be created. (audio won't work now)");
			return;
		}
		
		//irrklang::ISoundSource::setForcedStreamingThreshold(100000);
		//multiplied with all sounds
		soundEngine->setSoundVolume(Config::MASTER_VOLUME);

		if (soundEngine)
		{
			//preload all sound files and set volume
			LoadSound(SOUND::FALL,       "GameData/Sounds/sfx_die.wav",       fall_volume);
			LoadSound(SOUND::HIT,        "GameData/Sounds/sfx_hit.wav",       hit_volume);
			LoadSound(SOUND::POINT,      "GameData/Sounds/sfx_point.wav",     point_volume);
			LoadSound(SOUND::TRANSITION, "GameData/Sounds/sfx_swooshing.wav", transition_volume);
			LoadSound(SOUND::WING,       "GameData/Sounds/sfx_wing.wav",      wing_volume);
			LoadSound(SOUND::NABOKOV,	 "GameData/Sounds/Nabokov_hotels.ogg", nabokov_volume);
		}
	}

	void ExportAudio()
	{
		InitAudioFile();

		WriteAudioFile("GameData/Sounds/sfx_die.wav");
		PRINT(std::cout << "exported: " << "GameData/Sounds/sfx_die.wav" << std::endl;)
		
		WriteAudioFile("GameData/Sounds/sfx_hit.wav");
		PRINT(std::cout << "exported: " << "GameData/Sounds/sfx_hit.wav" << std::endl;)
		
		WriteAudioFile("GameData/Sounds/sfx_point.wav");
		PRINT(std::cout << "exported: " << "GameData/Sounds/sfx_point.wav" << std::endl;)
		
		WriteAudioFile("GameData/Sounds/sfx_swooshing.wav");
		PRINT(std::cout << "exported: " << "GameData/Sounds/sfx_swooshing.wav" << std::endl;)
		
		WriteAudioFile("GameData/Sounds/sfx_wing.wav");
		PRINT(std::cout << "exported: " << "GameData/Sounds/sfx_wing.wav" << std::endl;)

		WriteAudioFile("GameData/Sounds/Nabokov_hotels.ogg");
		PRINT(std::cout << "exported: " << "GameData/Sounds/Nabokov_hotels.ogg" << std::endl;)
	}

private:
	void LoadSound(SOUND sound, std::string file, float volume)
	{
		if (Config::LOAD_FROM_DISTRIBUTION)
		{
			int len = 0;
			unsigned char* wav_data = ReadAudioFile(file, len);
			sounds[sound] = soundEngine->addSoundSourceFromMemory(wav_data, len, file.c_str(), true);
			delete[] wav_data;
		}
		else
		{
			sounds[sound] = soundEngine->addSoundSourceFromFile(file.c_str(), irrklang::E_STREAM_MODE::ESM_NO_STREAMING, true);
		}
		if (sound == SOUND::NABOKOV)
		{
			sounds[sound]->setForcedStreamingThreshold(5000000);
			sounds[sound]->setStreamMode(irrklang::E_STREAM_MODE::ESM_NO_STREAMING);
		}

		if (sounds[sound])
			sounds[sound]->setDefaultVolume(volume);
		else
		{
			PRINT(std::cout << "could not load sound file: " << file << std::endl;)
			WriteErrorFile("Failed to load sound file: " + file);
		}
	}

	void playSoundExtra(SOUND sound)
	{
		if (soundEngine && sounds[sound])
		{
			irrklang::ISound* sound_track = soundEngine->play2D(sounds[sound], false, false, true, true);
			sound_track->drop();
			/*
			if (sound_track)
			{
				sound_track->setPlaybackSpeed();
				sound_track->setVelocity();

				irrklang::ISoundEffectControl* fx = sound_track->getSoundEffectControl();
				if (fx)
				{
					fx->enableDistortionSoundEffect();
					fx->enableEchoSoundEffect();
				}
			}
			*/
		}
	}
};

Audio AudioManager;