#pragma once
#define AM AudioManager::GetInstance()

#include "FMOD/fmod_studio.hpp"
#include <assert.h>
#include <string>
#include "VolumeHandle.h"

struct Sound
{
	FMOD::Studio::EventInstance* mySoundInstances;
	std::string myName;
	AudioChannel myChannel;
	bool myShouldBeFreed = false;
	bool myIsRepeating;
	bool myIsCrossFading = false;
	bool myIsCrossFadingUp = false;
	float myCrossFadeScalar = 0.f;
	float myInternalVolume = 1.f;
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
	VolumeHandle& GetVolumeHandle();
	
	static AudioManager& GetInstance();
	void Init();
	void Update();
	void LoadAudioBank(const std::string& aBankName, bool aIsMaster = false);
	void LoadAudioFile(const std::string& aAudioName, AudioChannel aChannel);
	void UnloadAudioFile(const std::string& aAudioName);
	void CrossFade(const std::string& aAudioOne, const std::string& aAudioTwo, float aCrossFadeScalar);
	void Play(const std::string& aAudioName, bool aShouldRepeat = false, float aVolumePercentage = 100);
	void PlayNewInstance(const std::string& aAudioName, AudioChannel aChannel, bool aShouldRepeat = false, float aVolumePercentage = 100);
	void Stop(const std::string& aAudioName);
	void StopAll();
	void SetRepeat(const std::string& aAudioName, bool aRepeatState);
private:
	AudioManager();
	~AudioManager();
	
	void CrossFadeUpdate(unsigned int aIndex);

	static const int myMaxAudioFiles = 100;
	static const int myMaxBanks = 50;
	const int myMaxChannels = 512;
	unsigned int myUsedAudioFiles = 0;
	unsigned int myUsedBanks = 0;

	VolumeHandle myVolumeHandle;

	Sound mySounds[myMaxAudioFiles];
	Bank myBanks[myMaxBanks];
	
	FMOD_RESULT myTryResults;
	FMOD::Studio::System* myAudioSystem = nullptr;
};