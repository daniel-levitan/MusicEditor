#include "player.h"

static int playCallback(void *inputbuffer, void *outputbuffer, 
						unsigned long framesperbuffer, 
						PaTimestamp outTime, void *userdata)
{
	Pa_data *data = (Pa_data*)userdata;
	//SAMPLE *rptr = &data->samples[data->frameindex * data->numInputChannels];
	//SAMPLE *wptr = (SAMPLE*)outputbuffer;

	float *rptr = &data->samples[data->frameindex * data->numInputChannels];
	float *wptr = (float*)outputbuffer;

	unsigned int i;
    int finished = 0;
	//float gain = 0.3;
	float gain = 1;
    (void) inputbuffer;
    (void) outTime;

	int framesleft = data->maxframeindex - data->frameindex; 

	if(framesleft <= framesperbuffer){
		
		for (i=0;i<framesleft;i++){
			*wptr++=(*rptr++)*gain;
			if(data->numOutputChannels == 2){
				if(data->numInputChannels == 1){
					*wptr++=(*rptr) *gain; //copy the left to right
				}
				else{
					*wptr++=(*rptr++)*gain;
				}
			}
		}
		//data->frameindex = 0;
		finished = 1;
	}
	else{
		for (i=0;i<framesperbuffer;i++){
			*wptr++=(*rptr++)*gain;
			if(data->numOutputChannels == 2){
				if(data->numInputChannels == 1){
					*wptr++=(*rptr)*gain; //copy the left to right
				}
				else{
					*wptr++=(*rptr++)*gain;
				}
			}
		}
		data->frameindex += framesperbuffer;
	}
	
	//printf("frame index = %d\n",data->frameindex);
	return finished;
}



int playStream(float *fptr, SF_INFO sfinfo ){

	PortAudioStream *stream;
	PaError 	err;
	unsigned long 	framesperbuffer;
	Pa_data 	data;

	int playbackChannels = sfinfo.channels;
	
	framesperbuffer = 64;
	data.frameindex = 0;

	err = Pa_Initialize();
	if(err!=paNoError){
		Pa_Terminate();
		return(-1);
	}

	
	data.samples = fptr;
	data.maxframeindex = sfinfo.frames;
	data.numOutputChannels = playbackChannels;

	data.numInputChannels = sfinfo.channels;
	
	err = Pa_OpenStream(
			&stream,
			paNoDevice,
			0,
			paFloat32,
			NULL,
			Pa_GetDefaultOutputDeviceID(),
			playbackChannels, 
			paFloat32,
			NULL,
			(double)sfinfo.samplerate,
			framesperbuffer,
			0,
			paClipOff,
			playCallback,
			&data);

	
	if(err!=paNoError)
		Pa_Terminate();
		
	err = Pa_StartStream(stream);
	if(err!=paNoError)
		Pa_Terminate();
	
	

	//if (sfinfo.channels == 1)
	//	Pa_Sleep(1000*sfinfo.frames*2/sfinfo.samplerate);


	//if (sfinfo.channels == 2)
	//	Pa_Sleep(1000*sfinfo.frames/sfinfo.samplerate);

	Pa_Sleep((long int) (1000* (float)sfinfo.frames/sfinfo.samplerate + 150));
	// precisei somar 150, acredito q seja problema da interface grafica

	//err = Pa_StopStream( stream );

	err= Pa_CloseStream(stream);

	if(err!=paNoError)
		Pa_Terminate();
	
	Pa_Terminate();
	
	return 0;
}


int playPartStream(float *fptr, int channelsIn, int channelsOut, int size, int sampleRate){

	PortAudioStream *stream;
	PaError err;
	unsigned long framesperbuffer;
	Pa_data data;

	//int playbackChannels = sfinfo.channels;
	int playbackChannels = channelsOut;
	
	framesperbuffer = 64;
	data.frameindex = 0;

	err = Pa_Initialize();
	if(err!=paNoError)
		Pa_Terminate();


	data.samples = fptr;
	//data.maxframeindex = sfinfo.frames;
	data.maxframeindex = size;
	data.numOutputChannels = playbackChannels;

	data.numInputChannels = channelsIn;

	err = Pa_OpenStream(
			&stream,
			paNoDevice,
			0,
			paFloat32,
			NULL,
			Pa_GetDefaultOutputDeviceID(),
			playbackChannels, 
			paFloat32,
			NULL,
			(double)sampleRate,
			framesperbuffer,
			0,
			paClipOff,
			playCallback,
			&data);



	if(err!=paNoError)
		Pa_Terminate();
		
	err = Pa_StartStream(stream);
	if(err!=paNoError)
		Pa_Terminate();
	/*
	if (channelsIn == 1)
		Pa_Sleep(1000*size*playbackChannels/sampleRate);

	if (channelsIn == 2)
		Pa_Sleep(1000*size/sampleRate);
	*/

	Pa_Sleep((long int) (1000*(float)size/sampleRate + 150));

	


	err= Pa_CloseStream(stream);

	if(err!=paNoError)
		Pa_Terminate();
	
	Pa_Terminate();
	
	return 0;
}

