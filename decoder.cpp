
#include "decoder.h"



inline float scale(mad_fixed_t sample)
{
   return (float) (sample / (float) (1L << MAD_F_FRACBITS));
}


//int decode(struct private_data mPrivateData, const char *filename, float *sample,	SF_INFO info)
int decode(const char *filename, SF_INFO &info)
{
	struct private_data mPrivateData;
	mad_decoder			mDecoder;	
	//int					chn;
	SNDFILE				*outfile;
	//SF_INFO 			info;
	
	mPrivateData.len = 0;
	mPrivateData.file = fopen(filename,"rb");
	mPrivateData.inputBuffer = new unsigned char [INPUT_BUFFER_SIZE];
    	mPrivateData.numChannels = 0;
	mPrivateData.samplerate = 0;
	mPrivateData.icount = mPrivateData.ocount = 0;
	mPrivateData.firstRun = true;

    	mad_decoder_init(&mDecoder, &mPrivateData, input_cb, 0, 0, output_cb, error_cb, 0);

	if(mad_decoder_run(&mDecoder, MAD_DECODER_MODE_SYNC) == 0)
	{
		/* success */
		//wxMessageBox("Success\n");
		
		info.samplerate = mPrivateData.samplerate;
		info.channels   = mPrivateData.numChannels;
		info.format	= 65538;
		info.sections   = 1;
		info.seekable   = 1;
		info.frames 	= 0;
		
		outfile = sf_open("outstep.wav" , SFM_WRITE , &info);
		sf_write_float(outfile, mPrivateData.sample , mPrivateData.len) ;
		sf_close(outfile);
		
		// Se tiver successo copio de um ponteiro para o outro 

		//sample = mPrivateData.sample;
		
		mad_decoder_finish(&mDecoder);
		
		
		
    }
	else {
		/* failure */
		//wxMessageBox("Failure\n");
	    mad_decoder_finish(&mDecoder);
   }

	/* delete everything */
	//printf("Input %d   Output %d\n",mPrivateData.icount, mPrivateData.ocount);
	
    delete mPrivateData.inputBuffer;
	fclose(mPrivateData.file);

	return(0);
}





//
//   MAD Callbacks
//

/* The input callback is called when the decoder wants more data. */
enum mad_flow input_cb(void *_data, struct mad_stream *stream)
{
   struct private_data *data = (struct private_data *)_data;

    data->icount += 1;

   if(feof(data->file)) 
      return MAD_FLOW_STOP;
  

   /* "Each time you refill your buffer, you need to preserve the data in
    *  your existing buffer from stream.next_frame to the end.
    *
    *  This usually amounts to calling memmove() on this unconsumed portion
    *  of the buffer and appending new data after it, before calling
    *  mad_stream_buffer()"
    *           -- Rob Leslie, on the mad-dev mailing list */

   unsigned int unconsumedBytes;
   if(stream->next_frame) {
      unconsumedBytes = data->inputBuffer + INPUT_BUFFER_SIZE - stream->next_frame;
      memmove(data->inputBuffer, stream->next_frame, unconsumedBytes);
   }
   else
      unconsumedBytes = 0;

   int read = fread(data->inputBuffer + unconsumedBytes,
					1, 
					INPUT_BUFFER_SIZE - unconsumedBytes,
					data->file);
   
   mad_stream_buffer(stream, data->inputBuffer, read + unconsumedBytes);

   return MAD_FLOW_CONTINUE;
}


/* The output callback is called every time the decoder has finished decoding
 * a frame, allowing us to use the decoded data */
enum mad_flow output_cb(void *_data,
                        struct mad_header const *header,
                        struct mad_pcm *pcm)
{

	int		channels,
			samplerate,
			samples;
	struct	private_data *data = (struct private_data *)_data;
	int		smpl,
			cont;
	//int		chn;

	float	*bufferL,
			*bufferR;


	samplerate= pcm->samplerate;
	channels  = pcm->channels;
	samples   = pcm->length;
	
    if (data->firstRun){
		data->numChannels = channels;
		data->samplerate  = samplerate;	
		data->firstRun = false;
    }

	

    if(data->ocount == 0) 
   		data->sample = (float *) malloc(channels*samples*sizeof(float) );
    else
   		data->sample = (float *) realloc (data->sample , (data->len + samples)*sizeof(float)*channels );
    
	data->ocount += 1;

 	if (!data->sample){
   		printf("NAo alocou memo\n");
		exit(1);
	}
 		
	float *buffer = new float[channels*samples];
	
	if (channels == 1){
		for(smpl = 0; smpl < samples; smpl++)
			 buffer[smpl] = 32767*scale(pcm->samples[0][smpl]);
	}

	if (channels == 2){
		bufferL = new float[samples];
		bufferR = new float[samples];
		
		for(smpl = 0; smpl < samples; smpl++){
			 bufferL[smpl] = 32767*scale(pcm->samples[0][smpl]);
			 bufferR[smpl] = 32767*scale(pcm->samples[1][smpl]);
		}

		//Interleaving
		cont = 0; 
		while (cont < samples){
			buffer[cont] = bufferL[cont];
			cont++;
			buffer[cont] = bufferR[cont];
			cont++;
		}
	}
	
	
	memcpy(data->sample + data->len ,buffer ,channels*samples);
	data->len +=channels*samples;

	
	if (channels == 2){
		delete [] bufferL;	
		delete [] bufferR;
	}
	delete [] buffer;
   
   return MAD_FLOW_CONTINUE;
}


enum mad_flow error_cb(void *_data, struct mad_stream *stream, 
                       struct mad_frame *frame)
{
   return MAD_FLOW_CONTINUE;
}





