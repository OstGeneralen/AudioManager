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

VolumeHandle & AudioManager::GetVolumeHandle()
{
	return myVolumeHandle;
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

void AudioManager::LoadAudioFile(const std::string& aAudioName, AudioChannel aChannel)
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
		mySounds[myUsedAudioFiles].myChannel = aChannel;
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
			aVolumePercentage = myVolumeHandle.TranslateToUsedVolume(mySounds[i].myChannel, aVolumePercentage);
			mySounds[i].myIsRepeating = aShouldRepeat;
			mySounds[i].mySoundInstances->setVolume(aVolumePercentage);
			mySounds[i].mySoundInstances->start();
			break;
		}
	}
}

void AudioManager::PlayNewInstance(const std::string & aAudioName, AudioChannel aChannel, bool aShouldRepeat, float aVolumePercentage)
{
	LoadAudioFile(aAudioName, aChannel);

	mySounds[myUsedAudioFiles - 1].mySoundInstances->setVolume(myVolumeHandle.TranslateToUsedVolume(aChannel, aVolumePercentage));
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

	float volumeToReach = myVolumeHandle.GetChannelVolume(mySounds[aIndex].myChannel);

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
