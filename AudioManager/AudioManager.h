#pragma once
#include "fmod_studio.hpp"
#include <assert.h>
#include <string>

struct Sound
{
	FMOD::Studio::EventInstance* mySoundInstances;
	std::string myName;
	bool myIsRepeating;
};

struct Bank
{
	FMOD::Studio::Bank* myAudioBank;
	FMOD::Studio::Bank* myStringBank;
	std::string myName;
};

class AudioManager
{
public:
	AudioManager(const AudioManager& aAudioManager) = delete;
	static AudioManager& GetInstance();
	void Init();
	void Update();
	void LoadAudioBank(const std::string& aBankName, bool aIsMaster = false);
	void LoadAudioFile(const std::string& aAudioName);
	void Play(const std::string& aAudioName, bool aNewInstance = false, bool aShouldRepeat = false, float aVolumePercentage = 100);
	void Stop(const std::string& aAudioName);
	void StopAll();
private:
	AudioManager();
	~AudioManager();
	
	static const int myMaxAudioFiles = 100;
	static const int myMaxBanks = 50;
	const int myMaxChannels = 512;
	unsigned int myUsedAudioFiles = 0;
	unsigned int myUsedBanks = 0;

	Sound mySounds[myMaxAudioFiles];
	Bank myBanks[myMaxBanks];
	
	FMOD_RESULT myTryResults;
	FMOD::Studio::System* myAudioSystem = nullptr;
	FMOD::Studio::Bank* myAudioBank = nullptr;
	FMOD::Studio::Bank* myStringBank = nullptr;
};