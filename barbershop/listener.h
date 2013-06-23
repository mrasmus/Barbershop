#ifndef _LISTENER_H
#define _LISTENER_H

#pragma warning(disable : 4244)
#pragma warning(disable : 4305)

#include "portaudio.h"

struct MicData
{
	int device;
	float frequency;
	float amplitude;
	PaStream *stream;
};

extern MicData playerData[4];

int initListener();

#endif