#include "AudioManager.h"

AudioManager & AudioManager::GetInstance()
{
	static AudioManager instance;

	return instance;
}

void AudioManager::Init(const std::string& aBankPath)
{
	myTryResults = FMOD::Studio::System::create(&myAudioSystem);
	assert(myTryResults == FMOD_OK && "Error creating audio system");
	
	myTryResults = myAudioSystem->initialize(myMaxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
	assert(myTryResults == FMOD_OK && "Error initializing audio system");
	
	myTryResults = myAudioSystem->loadBankFile(aBankPath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &myAudioBank);
	assert(myTryResults == FMOD_OK && "Error loading bank file. Bad path?");

	myAudioSystem->loadBankFile("Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myStringBank);
	
}

void AudioManager::Update()
{
	myAudioSystem->update();
}

void AudioManager::LoadAudioFile(const std::string& aAudioName)
{
	std::string eventName = "event:/";
	eventName += aAudioName;

	if (myEventCount < myMaxAudioFiles)
	{
		FMOD::Studio::EventDescription* tempDescription = nullptr;
		myAudioSystem->getEvent(eventName.c_str(), &tempDescription);

		FMOD::Studio::EventInstance* tempInstance = nullptr;
		tempDescription->createInstance(&tempInstance);

		myAudioFiles[myEventCount].mySoundInstances = tempInstance;
		myAudioFiles[myEventCount].myName = aAudioName;
		myEventCount++;
	}
}

void AudioManager::Play(const std::string& aAudioName)
{
	for (unsigned int i = 0; i < myMaxAudioFiles; ++i)
	{
		if (myAudioFiles[i].myName == aAudioName)
		{
			myAudioFiles[i].mySoundInstances->start();
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
