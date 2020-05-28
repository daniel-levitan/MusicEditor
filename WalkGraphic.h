#ifndef __WALKGRAPHIC_H__
#define __WALKGRAPHIC_H__

#include <math.h>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "FWLFont.h"


//#define DATA_SIZE 150 // number of ploted points in the graphic
//#define DATA_SIZE 12246 // number of plotted points in graphic

#define DATA_SIZE 40176
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  class RotateVector  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RotateVector
{
protected:

	RotateVector();
	~RotateVector();

	int		rotateVector_ind_vec;
    bool	rotateVector_vec_is_full;
    float	*rotateVector_data_y;
    wxString *rotateVector_data_x;
    bool	*rotateVector_data_plot_x;
    int		rotateVector_size;
    int		rotateVector_ind_give_point;
    int		rotateVector_info;
    bool	rotateVector_end_of_give_point;

	void	rotateVector_set_size(int number_of_elements);

	bool	rotateVector_is_full();
    bool	rotateVector_is_empty();
    int		rotateVector_get_size();
	void	rotateVector_new_point(float y,wxString x,bool plot_x);
	void	rotateVector_reset_give_point();
	int		rotateVector_give_point();

private:
	void	rotateVector_constructor();
	void	rotateVector_destructor();

};







////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// class WalkGraphic  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


class WalkGraphic : public RotateVector
{

public:

	WalkGraphic();
	WalkGraphic(int size);
    WalkGraphic(wxMemoryDC *dc,int x1,int y1,int x2,int y2);
	~WalkGraphic();

	void	setCurveColor(wxColour c){ m_curveColour=c; }
    void	setGridColor(wxColour c){ m_gridColour=c; }
    void	setGridLinesColor(wxColour c){ m_internalLinesColour=c; }
    void	setGridBackgroundColor(wxColour c){ m_gridBackgroundColour=c; }
	//void setArea(int x1,int y1,int x2,int y2);
	wxRect	setArea(int x1,int y1,int x2,int y2);
	void	setDC(wxMemoryDC *dc);

	void	newPoint(float y,wxString x,bool plot_x);
	//void plot(bool selected, int startPoint, int endPoint);
	void	plot();
	//void	plotSelect(int start, int end);
	void	plotSelect(int start, int end, int offset);

	void	redefineVerticalAxis();
	void	scaleVerticalAxis(float scale_factor);

	void	setSize(int size);

	void	getArea(int *x1,int *y1,int *x2,int *y2);
	void    getMaxYPoint(float *y_max);
	void    getMinYPoint(float *y_min);

	int 	getMaxStringLen();

private:

	void	walkGraphicConstructor();
	void	walkGraphicConstructorSize(int size);
	void	horizontalDottedLine(int y, int x1, int x2);
	void	verticalDottedLine(int x, int y1, int y2);
	//void showGrid(bool traceOn,bool selection, int startPoint, int endPoint);
	void	showGrid(bool traceOn);
	void	showGridSelected(bool traceOn,int startPosition, int endPosition);
	void	plotVector(bool traceOn);
	void	atualizeVerticalScale(bool forced);

	wxMemoryDC *m_dc;
	wxPen	m_pen;
	wxBrush m_brush;

	FWLFont *m_font;

	// coordinates when all the graphic should stay (indluding Strings)
    int		m_screen_x1;
    int		m_screen_y1;
    int		m_screen_x2;
    int		m_screen_y2;

    // grid coordinates
    int		m_grid_x1;
    int		m_grid_y1;
    int		m_grid_x2;
    int		m_grid_y2;

	// max and min values of the ploted graphic
	float	m_max,m_min;

	int		m_decimalChars;

	// colours
	wxColour m_curveColour;
	wxColour m_gridColour;
	wxColour m_internalLinesColour;
	wxColour m_gridBackgroundColour;

	// vertical strings
	wxString m_verticalAxisStr[11];

	//rectangle   -- Tem de fazer parte de uma classe track
	wxRect	m_track;

	//SF_INFO m_sfinfo;						
};
#endif
