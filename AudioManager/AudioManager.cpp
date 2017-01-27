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
	}

	myAudioSystem->update();
}

void AudioManager::LoadAudioBank(const std::string & aAudioBankName)
{
	std::string bankName = aAudioBankName;
	std::string stringBankName = aAudioBankName;
	bankName += ".bank";
	stringBankName += ".strings.bank";

	if (myUsedBanks < myMaxBanks)
	{
		myTryResults = myAudioSystem->loadBankFile(bankName.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &myBanks[myUsedBanks].myAudioBank);
		assert(myTryResults == FMOD_OK && "Error loading bank file. Bad name?");

		myAudioSystem->loadBankFile(stringBankName.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &myBanks[myUsedBanks].myStringBank);
		myUsedBanks++;
	}
}

void AudioManager::LoadAudioFile(const std::string& aAudioName)
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
		myUsedAudioFiles++;
	}
}

void AudioManager::Play(const std::string& aAudioName, bool aNewInstance, bool aShouldRepeat, float aVolumePercentage)
{
	if (aNewInstance)
	{
		LoadAudioFile(aAudioName);
		mySounds[myUsedAudioFiles - 1].mySoundInstances->setVolume(aVolumePercentage / 100.f);
		mySounds[myUsedAudioFiles - 1].mySoundInstances->start();
	}
	else
	{
		for (unsigned int i = 0; i < (myUsedAudioFiles); ++i)
		{
			if (mySounds[i].myName == aAudioName)
			{
				mySounds[i].myIsRepeating = aShouldRepeat;
				mySounds[i].mySoundInstances->setVolume(aVolumePercentage / 100.f);
				mySounds[i].mySoundInstances->start();
				break;
			}
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
