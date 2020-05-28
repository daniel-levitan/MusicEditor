#include "track.h"

Track::Track()
{
	trackFptr		= NULL;
	
	//rightTrackFptr	= NULL;
	//leftTrackFptr	= NULL;
	
	m_buttonPressed = false;
	m_validPressed = false;
	
	m_startPoint	= -1;
	m_endPoint		= -1;
	
	m_startSample	= -1;
	m_endSample		= -1;
	
	zoomFactor		= 0;

	ini_height      = 0;
	ini_width		= 0;
}

Track::~Track()
{
	
}




int Track::toPoint(float sample)
{
	int		point;

	if (trackInfo.channels == 1)
		point = (int) sample*trackArea.GetWidth() / (trackInfo.frames);
	if (trackInfo.channels == 2)
		point = (int) sample*trackAreaLeft.GetWidth() / (trackInfo.frames);

	return point;	
}

float Track::toSample(int point)
{
	float	sample;

	if (trackInfo.channels == 1)
		sample = ( point )*(trackInfo.frames) / trackArea.GetWidth();

	if (trackInfo.channels == 2)
		sample = ( point )*(trackInfo.frames) / trackAreaLeft.GetWidth();
	
	return sample;
}

bool Track::isInside(float x, float y)
{
	bool	isInside = false;

	if ( (trackInfo.channels == 1) && ( trackArea.Inside((int)x,(int)y) ) )
		isInside = true;
	
	if ( (trackInfo.channels == 2) && (( trackAreaLeft.Inside( (int)x, (int)y) ) ||  (trackAreaRight.Inside( (int)x, (int)y))) )
		isInside = true;

	return isInside;
}
