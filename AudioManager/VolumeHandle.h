#pragma once
#include "AudioEnums.h"

class VolumeHandle
{
public:
	VolumeHandle(const VolumeHandle& aVolumeHandle) = delete;
	VolumeHandle();
	float TranslateToUsedVolume(AudioChannel aChannel, float aVolume);
	void SetChannelVolume(AudioChannel aChannel, float aVolume);
	float GetChannelVolume(AudioChannel aChannel);
private:
	float myMasterVolume;
	float myFXVolume;
	float myMusicVolume;
	float myRawFXVolume;
	float myRawMusicVolume;
};
