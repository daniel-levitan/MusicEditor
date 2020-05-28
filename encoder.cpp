
#include "encoder.h"
#include "conversor.h"

//int encode(char *inName, char *outName)
int encode(float *samples, SF_INFO info ,const char *outName)
{
	FILE		*outfile; 
	int			writecnt;
	int			frames,k;
	float	    *left,*right; 
	int			mp3buffer_size;
	int			ret_code,imp3;
	
	unsigned char		*mp3buffer;
	lame_global_flags	*gfp;
	if ( info.channels == 1){     // Mono
		if ((left = (float*)malloc((info.frames)*sizeof(float))) == NULL ) return -1; // Memory Allocation Error
		if ((right = (float*)malloc((info.frames)*sizeof(float))) == NULL ) return -1; // Memory Allocation Error
		
		// Sorting the frames samples
		k = 0;
		for (int k = 0; k < info.frames;k++){
			left[k] = samples[k];
			right[k] = samples[k];
		}

			
		mp3buffer_size = (int) 1.25*info.frames + 7200;
		if ((mp3buffer = (unsigned char*)malloc((mp3buffer_size))) == NULL ) return -1; // Memory Allocation Error
		
		gfp = lame_init();
		imp3 = lame_init_params(gfp);
		if (imp3 < 0) return -1; // Error Initializing the lame initial parameters

		ret_code = lame_set_num_channels(gfp,2);
		ret_code = lame_set_scale(gfp, 32768/2);
		//ret_code = lame_set_mode(gfp,MONO);

		ret_code = lame_set_in_samplerate(gfp,info.samplerate);
		imp3 = lame_encode_buffer_float(gfp, left, right, info.frames, mp3buffer, mp3buffer_size);
		ret_code = lame_encode_flush(gfp, mp3buffer, mp3buffer_size);  

		if ( (outfile = fopen(outName,"wb+")) == NULL ) return -1; // Error opening the out file
		writecnt = fwrite(mp3buffer, 1, imp3, outfile);
		fclose(outfile);

		lame_close(gfp);
		
		free(left);
		free(right);

	}


	
	if ( info.channels == 2){     // Stereo 

		if ((left = (float*)malloc((info.frames)*sizeof(float))) == NULL ) return -1; // Memory Allocation Error
		if ((right = (float*)malloc((info.frames)*sizeof(float))) == NULL ) return -1; // Memory Allocation Error
		
		// Sorting the frames samples
		k = 0;
		for (int lef = 0; lef < info.frames*info.channels;lef +=2){
			left[k] = samples[lef];
			k++;
		}
		
		k = 0;
		for (int rig = 1; rig < info.frames*info.channels; rig +=2){
			right[k] = samples[rig];
			k++;
		}

		
		mp3buffer_size = (int) 1.25*info.frames + 7200;
		if ((mp3buffer = (unsigned char*)malloc((mp3buffer_size))) == NULL ) return -1; // Memory Allocation Error
		
		gfp = lame_init();
		imp3 = lame_init_params(gfp);
		if (imp3 < 0) return -1; // Error Initializing the lame initial parameters

		ret_code = lame_set_num_channels(gfp,2);
		ret_code = lame_set_scale(gfp, 32768/2);
		ret_code = lame_set_in_samplerate(gfp,info.samplerate);

		imp3 = lame_encode_buffer_float(gfp, left, right, info.frames, mp3buffer, mp3buffer_size);
		ret_code = lame_encode_flush(gfp, mp3buffer, mp3buffer_size);  

		if ( (outfile = fopen(outName,"wb+")) == NULL ) return -1; // Error opening the out file
		writecnt = fwrite(mp3buffer, 1, imp3, outfile);
		fclose(outfile);

		lame_close(gfp);
		
		free(left);
		free(right);


	}
	
	return 0;
}


