#pragma once
#include "fmod_studio.hpp"
#include <assert.h>
#include <string>

struct AudioFile
{
	FMOD::Studio::EventInstance* mySoundInstances;
	std::string myName;
};

class AudioManager
{
public:
	AudioManager(const AudioManager& aAudioManager) = delete;
	static AudioManager& GetInstance();
	void Init(const std::string& aBankPath);
	void Update();
	void LoadAudioFile(const std::string& aAudioName);
	void Play(const std::string& aAudioName);
private:
	static const int myMaxAudioFiles = 100;
	int myUsedAudioFiles = 0;

	AudioManager();
	~AudioManager();
	FMOD_RESULT myTryResults;
	FMOD::Studio::System* myAudioSystem = nullptr;
	FMOD::Studio::Bank* myAudioBank = nullptr;
	FMOD::Studio::Bank* myStringBank = nullptr;
	AudioFile myAudioFiles[myMaxAudioFiles];
	int myEventCount;
	const int myMaxChannels = 512;
};