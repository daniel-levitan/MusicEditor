
#ifndef __ENCODER_H__
#define __ENCODER_H__

// Includes
// std header
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// auxiliary libs headers
#include "sndfile.h"
//#include <sndfile.h>
#include "lame.h"


int encode(float *samples, SF_INFO info, const char *outName);


//int encode(char *inName, char *outName);
//int decode(char *inName, char *outName)

#endif
