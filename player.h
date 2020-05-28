
#ifndef __PLAYER_H__
#define __PLAYER_H__

// Includes
#include <stdio.h>
#include "sndfile.h"
//#include <sndfile.h>
#include "portaudio.h"


// Defines
//typedef float SAMPLE;

typedef struct
{
	int frameindex;
	int maxframeindex;
	int numInputChannels;
	int numOutputChannels;
	float *samples;
	//SAMPLE *samples;
}Pa_data;


//static int playCallback(void *inputbuffer, void *outputbuffer, 
//						unsigned long framesperbuffer, 
//      				PaTimestamp outTime, void *userdata){};

int playStream(float *fptr,SF_INFO sfinfo );

int playPartStream(float *fptr, int channelsIn, int channelsOut, int size, int sampleRate );

#endif
