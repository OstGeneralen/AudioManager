#pragma once
#include "FMOD/fmod_studio.hpp"
#include <assert.h>
#include <string>
	
enum class AudioCategory
{
	None,
	Effect,
	Music,
};

struct Sound
{
	FMOD::Studio::EventInstance* mySoundInstances;
	std::string myName;
	AudioCategory myCategory;
	bool myShouldBeFreed = false;
	bool myIsRepeating;
	bool myIsCrossFading = false;
	bool myIsCrossFadingUp = false;
	float myCrossFadeScalar = 0.f;
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
	void LoadAudioFile(const std::string& aAudioName, AudioCategory aCategory = AudioCategory::None);
	void UnloadAudioFile(const std::string& aAudioName);
	void CrossFade(const std::string& aAudioOne, const std::string& aAudioTwo, float aCrossFadeScalar);
	void Play(const std::string& aAudioName, bool aShouldRepeat = false, float aVolumePercentage = 100);
	void PlayNewInstance(const std::string& aAudioName, AudioCategory aCategory = AudioCategory::None, bool aShouldRepeat = false, float aVolumePercentage = 100);
	void Stop(const std::string& aAudioName);
	void StopAll();
	void SetMasterVolume(float aMasterVolume);
	void SetCategoryVolume(AudioCategory aCategory, float aVolume);
	void SetRepeat(const std::string& aAudioName, bool aRepeatState);
private:
	AudioManager();
	~AudioManager();
	
	void CrossFadeUpdate(unsigned int aIndex);

	float CalculatePlayVolume(float aPercentage, AudioCategory aCategory = AudioCategory::None);

	static const int myMaxAudioFiles = 100;
	static const int myMaxBanks = 50;
	const int myMaxChannels = 512;
	unsigned int myUsedAudioFiles = 0;
	unsigned int myUsedBanks = 0;

	float myMasterVolume = 100;
	float myEffectsVolume = 100;
	float myMusicVolume = 100;

	Sound mySounds[myMaxAudioFiles];
	Bank myBanks[myMaxBanks];
	
	FMOD_RESULT myTryResults;
	FMOD::Studio::System* myAudioSystem = nullptr;
	FMOD::Studio::Bank* myAudioBank = nullptr;
	FMOD::Studio::Bank* myStringBank = nullptr;
};