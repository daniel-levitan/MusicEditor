
#ifndef __DECODER_H__
#define __DECODER_H__

// Includes
// std header
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



// auxiliary libs headers
#include "sndfile.h"
//#include <sndfile.h>
#include "mad.h"

#define INPUT_BUFFER_SIZE 65535

struct private_data {
		FILE			*file;
		unsigned char	*inputBuffer;
		int 			numChannels;
		int				samplerate;
		int				len;
		float 			*sample;
		int				icount,
						ocount;
		bool			firstRun;
		//void 			*userData; //useless
};

/* The MAD callbacks */
enum mad_flow input_cb (void *_data, struct mad_stream *stream);
enum mad_flow output_cb(void *_data, struct mad_header const *header, struct mad_pcm *pcm);
enum mad_flow error_cb (void *_data, struct mad_stream *stream, struct mad_frame *frame);


//int decode(struct private_data mPrivateData, const char *filename, float *sample, SF_INFO info);
int decode(const char *filename, SF_INFO &info);



#endif
