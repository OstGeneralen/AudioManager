#include "VolumeHandle.h"

VolumeHandle::VolumeHandle()
{
	myMasterVolume = 1;
	myFXVolume = 1;
	myMusicVolume = 1;
	myRawFXVolume = 100;
	myRawMusicVolume = 100;
}

float VolumeHandle::TranslateToUsedVolume(AudioChannel aChannel, float aVolume)
{
	switch (aChannel)
	{
	case AudioChannel::Master:
		return (aVolume / 100.f) * myMasterVolume;
		break;
	case AudioChannel::Effect:
		return (aVolume / 100.f) * myFXVolume;
		break;
	case AudioChannel::Music:
		return (aVolume / 100.f) * myMusicVolume;
		break;
	default:
		return -1;
		break;
	}
}

void VolumeHandle::SetChannelVolume(AudioChannel aChannel, float aVolume)
{
	switch (aChannel)
	{
	case AudioChannel::Master:
		myMasterVolume = aVolume / 100.f;
		break;
	case AudioChannel::Effect:
		myRawFXVolume = aVolume;
		break;
	case AudioChannel::Music:
		myRawMusicVolume = aVolume;
		break;
	default:
		break;
	}

	myFXVolume = (myRawFXVolume / 100.f) * myMasterVolume;
	myMusicVolume = (myRawMusicVolume / 100.f) * myMasterVolume;
}

float VolumeHandle::GetChannelVolume(AudioChannel aChannel)
{
	switch (aChannel)
	{
	case AudioChannel::Master:
		return myMasterVolume;
		break;
	case AudioChannel::Effect:
		return myFXVolume;
		break;
	case AudioChannel::Music:
		return myMusicVolume;
		break;
	default:
		break;
	}
}

