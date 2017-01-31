#include "AudioManager.h"

AudioManager & AudioManager::GetInstance()
{
	static AudioManager instance;

	return instance;
}

void AudioManager::Init()
{
	myTryResults = FMOD::Studio::System::create(&myAudioSystem);
	assert(myTryResults == FMOD_OK && "Error creating audio system");
	
	myTryResults = myAudioSystem->initialize(myMaxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
	assert(myTryResults == FMOD_OK && "Error initializing audio system");

	SetCategoryVolume(AudioCategory::Effect, 100);
	SetCategoryVolume(AudioCategory::Music, 100);
}

void AudioManager::Update()
{
	for (unsigned int index = 0; index < (myUsedAudioFiles); index++)
	{
		if (mySounds[index].myIsRepeating)
		{
			FMOD_STUDIO_PLAYBACK_STATE state;
			mySounds[index].mySoundInstances->getPlaybackState(&state);
			if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
			{
				mySounds[index].mySoundInstances->start();
			}
		}
		else if (mySounds[index].myShouldBeFreed)
		{
			FMOD_STUDIO_PLAYBACK_STATE state;
			mySounds[index].mySoundInstances->getPlaybackState(&state);
			if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
			{
				UnloadAudioFile(mySounds[index].myName);
			}
		}

		if (mySounds[index].myIsCrossFading)
		{
			CrossFadeUpdate(index);
		}
	}

	myAudioSystem->update();
}

void AudioManager::LoadAudioBank(const std::string & aAudioBankName, bool aIsMaster)
{
	std::string bankName = aAudioBankName;
	std::string stringBankName = aAudioBankName;
	bankName += ".bank";
	stringBankName += ".strings.bank";

	if (myUsedBanks < myMaxBanks)
	{
		myTryResults = myAudioSystem->loadBankFile(bankName.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &myBanks[myUsedBanks].myAudioBank);
		assert(myTryResults == FMOD_OK && "Error loading bank file. Bad name?");

		if (aIsMaster)
		{
			myAudioSystem->loadBankFile(stringBankName.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &myBanks[myUsedBanks].myStringBank);
		}
		myUsedBanks++;
	}
}

void AudioManager::LoadAudioFile(const std::string& aAudioName, AudioCategory aCategory)
{
	std::string eventName = "event:/";
	eventName += aAudioName;

	if (myUsedAudioFiles < myMaxAudioFiles)
	{
		FMOD::Studio::EventDescription* tempDescription = nullptr;
		myTryResults = myAudioSystem->getEvent(eventName.c_str(), &tempDescription);
		assert(myTryResults == FMOD_OK && "Error loading audio file");

		FMOD::Studio::EventInstance* tempInstance = nullptr;
		tempDescription->createInstance(&tempInstance);

		mySounds[myUsedAudioFiles].mySoundInstances = tempInstance;
		mySounds[myUsedAudioFiles].myName = aAudioName;
		mySounds[myUsedAudioFiles].myCategory = aCategory;
		myUsedAudioFiles++;
	}
}

void AudioManager::UnloadAudioFile(const std::string & aAudioName)
{
	for (unsigned int index = 0; index < myUsedAudioFiles; ++index)
	{
		if (mySounds[index].myName == aAudioName)
		{
			mySounds[index].mySoundInstances->release();
			
			if (index == myUsedAudioFiles - 1)
			{
				myUsedAudioFiles -= 1;
				return;
			}
			else
			{
				mySounds[index] = mySounds[myUsedAudioFiles - 1];
				
				myUsedAudioFiles -= 1;
				return;
			}
		}
	}

	assert(false && "Could not unload audio file. Check name.");
}

void AudioManager::CrossFade(const std::string & aAudioOne, const std::string & aAudioTwo, float aCrossFadeScalar)
{
	bool foundAudioOne = false;
	bool foundAudioTwo = false;
	for (unsigned int index = 0; index < myUsedAudioFiles; ++index)
	{
		if (!foundAudioOne)
		{
			if (!mySounds[index].myIsCrossFading)
			{
				mySounds[index].myIsCrossFading = true;
				mySounds[index].myIsCrossFadingUp = false;
				mySounds[index].myCrossFadeScalar = aCrossFadeScalar;
				foundAudioOne = true;
			}
		}
		
		if (!foundAudioTwo)
		{
				if (!mySounds[index].myIsCrossFading)
				{
					mySounds[index].myIsCrossFading = true;
					mySounds[index].myIsCrossFadingUp = true;
					mySounds[index].myCrossFadeScalar = aCrossFadeScalar;
					foundAudioTwo = true;
					Play(aAudioTwo, false, 0.f);
				}
		}

		if (foundAudioOne && foundAudioTwo)
		{
			break;
		}
	}
}

void AudioManager::Play(const std::string& aAudioName, bool aShouldRepeat, float aVolumePercentage)
{
	for (unsigned int i = 0; i < (myUsedAudioFiles); ++i)
	{
		if (mySounds[i].myName == aAudioName)
		{
			aVolumePercentage = CalculatePlayVolume(aVolumePercentage, mySounds[i].myCategory);
			mySounds[i].myIsRepeating = aShouldRepeat;
			mySounds[i].mySoundInstances->setVolume(aVolumePercentage);
			mySounds[i].mySoundInstances->start();
			break;
		}
	}
}

void AudioManager::PlayNewInstance(const std::string & aAudioName, AudioCategory aCategory, bool aShouldRepeat, float aVolumePercentage)
{
	LoadAudioFile(aAudioName);

	mySounds[myUsedAudioFiles - 1].myCategory = aCategory;

	aVolumePercentage = CalculatePlayVolume(aVolumePercentage);

	mySounds[myUsedAudioFiles - 1].mySoundInstances->setVolume(aVolumePercentage);
	mySounds[myUsedAudioFiles - 1].myShouldBeFreed = true;
	mySounds[myUsedAudioFiles - 1].mySoundInstances->start();
}

void AudioManager::Stop(const std::string & aAudioName)
{
	for (unsigned int index = 0; index < myUsedAudioFiles; ++index)
	{
		if (mySounds[index].myName == aAudioName)
		{
			myTryResults = mySounds[index].mySoundInstances->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			mySounds[index].myIsRepeating = false;
		}
	}
}

void AudioManager::StopAll()
{
	for (unsigned int index = 0; index < myUsedAudioFiles; ++index)
	{
		myTryResults = mySounds[index].mySoundInstances->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
}

void AudioManager::SetMasterVolume(float aMasterVolume)
{
	myMasterVolume = aMasterVolume / 100;
	SetCategoryVolume(AudioCategory::Effect, myEffectsVolume * 100);
	SetCategoryVolume(AudioCategory::Music, myMusicVolume * 100);

	CorrectAllVolumes();

}

void AudioManager::SetCategoryVolume(AudioCategory aCategory, float aVolumePercentage)
{
	if (myMasterVolume > 1 && aVolumePercentage > 1)
	{
		aVolumePercentage = (myMasterVolume / 100.f) * (aVolumePercentage / 100.f);
	}
	else
	{
		aVolumePercentage = myMasterVolume * (aVolumePercentage / 100.f);
	}

	switch (aCategory)
	{
	case AudioCategory::Effect:
		myEffectsVolume = aVolumePercentage;
		break;
	case AudioCategory::Music:
		myMusicVolume = aVolumePercentage;
		break;
	default:
		assert(false && "Bad AudioCategory to set volume for");
		break;
	}

	CorrectAllVolumes();
}

void AudioManager::SetRepeat(const std::string & aAudioName, bool aRepeatState)
{
	for (unsigned int index = 0; index < myUsedAudioFiles; ++index)
	{
		if (mySounds[index].myName == aAudioName)
		{
			mySounds[index].myIsRepeating = aRepeatState;
			break;
		}
	}
}

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
	myAudioSystem->release();
}

void AudioManager::CrossFadeUpdate(unsigned int aIndex)
{
	float newVolume = 0;
	mySounds[aIndex].mySoundInstances->getVolume(&newVolume);

	if (mySounds[aIndex].myIsCrossFadingUp)
	{
		newVolume += mySounds[aIndex].myCrossFadeScalar;
	}
	else if (!mySounds[aIndex].myIsCrossFadingUp)
	{
		newVolume -= mySounds[aIndex].myCrossFadeScalar;
	}

	mySounds[aIndex].mySoundInstances->setVolume(newVolume);

	float volumeToReach = myMasterVolume;

	switch (mySounds[aIndex].myCategory)
	{
	case AudioCategory::Effect:
		volumeToReach = myEffectsVolume;
		break;
	case AudioCategory::Music:
		volumeToReach = myMusicVolume;
		break;
	default:
		volumeToReach = myMasterVolume;
		break;
	}

	if (newVolume <= 0.f || newVolume >= volumeToReach)
	{
		if (!mySounds[aIndex].myIsCrossFadingUp)
		{
			Stop(mySounds[aIndex].myName);
		}

		mySounds[aIndex].myIsCrossFading = false;
		mySounds[aIndex].myIsCrossFadingUp = false;
	}
}

float AudioManager::CalculatePlayVolume(float aPercentage, AudioCategory aCategory)
{
	if (aCategory == AudioCategory::Effect)
	{
		return (aPercentage / 100.f) * myEffectsVolume;
	}
	else if (aCategory == AudioCategory::Music)
	{
		return (aPercentage / 100.f) * myMusicVolume;
	}
	else
	{
		return (aPercentage / 100.f) * (myMasterVolume);
	}
}

void AudioManager::CorrectAllVolumes()
{
	for (unsigned int index = 0; index < myUsedAudioFiles; index++)
	{
		float currentVolume = 0;
		float volumeToSetTo = 0;

		mySounds[index].mySoundInstances->getVolume(&currentVolume);
		volumeToSetTo = CalculatePlayVolume(currentVolume * 100, mySounds[index].myCategory);

		mySounds[index].mySoundInstances->setVolume(volumeToSetTo);
	}
}
