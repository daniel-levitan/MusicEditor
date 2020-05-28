
#ifndef __TRACK_H__
#define __TRACK_H__

#include "wx/image.h"
#include "wx/file.h"
#include "wx/colour.h"


//#include <sndfile.h>
#include "sndfile.h"



class Track
{
public:
	
	// Header Info
	SF_INFO		trackInfo;
	
	wxRect	trackArea;

	wxRect	trackAreaLeft;
	wxRect	trackAreaRight;
	
	// Sample values
	float	*trackFptr;

	float	*trackFptrLeft;
	float	*trackFptrRight;


	// Select points
	int	m_startPoint,m_endPoint;
	float	m_startSample,m_endSample;

	bool	m_buttonPressed;
	bool	m_validPressed;

	// File Name
	SNDFILE		*infile;

	// Samples read
	sf_count_t	readcntf;

	// Zoom 
	int	ini_height;
	int	ini_width;
	
	float	zoomFactor;

	// Label border
	int	border;

	// functions
	int	toPoint(float sample);
	float	toSample(int point);
	bool	isInside(float x, float y);

	Track();
	~Track();
};

#endif
