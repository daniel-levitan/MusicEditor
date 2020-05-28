#include "WalkGraphic.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  class RotateVector  //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


RotateVector::RotateVector()
{
	rotateVector_data_y = NULL;
	rotateVector_data_x = NULL;
	rotateVector_data_plot_x = NULL;
}

RotateVector::~RotateVector()
{
	rotateVector_destructor();
}
  
void RotateVector::rotateVector_constructor()
{
	rotateVector_size = 0;
	rotateVector_ind_vec = 0;
	rotateVector_vec_is_full = false;
	rotateVector_ind_give_point = 0;
	rotateVector_end_of_give_point = false;
	rotateVector_info = 0;
}

void RotateVector::rotateVector_destructor()
{
	if(rotateVector_data_y) delete rotateVector_data_y;
	if(rotateVector_data_x) delete rotateVector_data_x;
	if(rotateVector_data_plot_x) delete rotateVector_data_plot_x;
}


// all old information will be lost
void RotateVector::rotateVector_set_size(int number_of_elements)
{
	rotateVector_destructor();
	rotateVector_constructor();
	rotateVector_size = number_of_elements;
	
	/*
	if ( (rotateVector_data_y = (float*)malloc( number_of_elements )) == NULL ){
		exit(1);
	}

	if ( (rotateVector_data_x = (wxString*)malloc( number_of_elements )) == NULL ){
		exit(1);
	}

	if ( (rotateVector_data_plot_x = (bool*)malloc( number_of_elements )) == NULL ){
		exit(1);
	}
	*/

	rotateVector_data_y = new float [rotateVector_size];
	rotateVector_data_x = new wxString [rotateVector_size];
	rotateVector_data_plot_x = new bool [rotateVector_size];	
}

bool RotateVector::rotateVector_is_full()
{ 
	return rotateVector_vec_is_full; 
}

bool RotateVector::rotateVector_is_empty()
{
	return ((!rotateVector_vec_is_full)&&(rotateVector_ind_vec==0)); 
}

int RotateVector::rotateVector_get_size()
{
	return rotateVector_size; 
}

void RotateVector::rotateVector_new_point(float y,wxString x,bool plot_x)
{
	if(!rotateVector_vec_is_full){

	    rotateVector_data_y[rotateVector_ind_vec]=y;
	    rotateVector_data_x[rotateVector_ind_vec]=x;
	    rotateVector_data_plot_x[rotateVector_ind_vec]=plot_x;

	    rotateVector_ind_vec++;
	    if(rotateVector_ind_vec == rotateVector_size){
		rotateVector_vec_is_full=true;
		rotateVector_ind_vec=0;
	    }
	}
	else{
	    rotateVector_data_y[rotateVector_ind_vec]=y;
	    rotateVector_data_x[rotateVector_ind_vec]=x;
	    rotateVector_data_plot_x[rotateVector_ind_vec]=plot_x;

	    rotateVector_ind_vec++;
	    if(rotateVector_ind_vec==rotateVector_size) rotateVector_ind_vec=0;
	}
}

void RotateVector::rotateVector_reset_give_point()
{
	if(!rotateVector_vec_is_full) rotateVector_ind_give_point=0;
	else rotateVector_ind_give_point=rotateVector_ind_vec;
	rotateVector_end_of_give_point=false;
}

//  info = 0 -> new data ok
//  info = -1  -> vector is empty 
//  end_of_vector = 1  -> last data readed
//  end_of_vector = 2  -> end of reading 
int RotateVector::rotateVector_give_point()
{
	int ret;

	if(rotateVector_end_of_give_point){ rotateVector_info = 2; return -1; }
	if((!rotateVector_vec_is_full)&&(rotateVector_ind_vec==0)){ rotateVector_info=-1; return -1; }
	rotateVector_info=0;

	ret=rotateVector_ind_give_point;
	rotateVector_ind_give_point++;
	if(rotateVector_ind_give_point==rotateVector_size) rotateVector_ind_give_point=0;

	if(rotateVector_ind_give_point==rotateVector_ind_vec){
	    rotateVector_info=1;
	    rotateVector_end_of_give_point=true;
	    if(!rotateVector_vec_is_full) rotateVector_ind_give_point=0;
	}
	return ret;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// class WalkGraphic  ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


WalkGraphic::WalkGraphic()
{
	m_dc=NULL;
	walkGraphicConstructor();
	m_track = setArea(0,0,0,0);
}

WalkGraphic::WalkGraphic(int size)
{
	m_dc=NULL;
	walkGraphicConstructorSize(size);
	m_track = setArea(0,0,0,0);
}



WalkGraphic::WalkGraphic(wxMemoryDC *dc,int x1,int y1,int x2,int y2)
{
	m_dc=dc;
	walkGraphicConstructor();
	m_track = setArea(x1,y1,x2,y2);
}


WalkGraphic::~WalkGraphic()
{
	delete m_font;
}


void WalkGraphic::walkGraphicConstructor()
{
	rotateVector_size = DATA_SIZE;

	m_max = m_min = 0;

	m_font = new FWLFont();

	m_decimalChars = 2;

	m_curveColour.Set(0xD0,0x00,0x00);
	m_gridColour.Set(0x00,0x00,0x00);
	m_internalLinesColour.Set(0x00,0x00,0x00);
	m_gridBackgroundColour.Set(0xFF,0xFF,0xFF);
	//m_gridBackgroundColour.Set(172, 168, 153);
	
	

	//rotateVector_set_size(DATA_SIZE);
	rotateVector_set_size(rotateVector_size);
}


void WalkGraphic::walkGraphicConstructorSize(int size)
{
	m_max = m_min = 0;

	m_font = new FWLFont();

	m_decimalChars = 2;

	m_curveColour.Set(0xD0,0x00,0x00);
	m_gridColour.Set(0x00,0x00,0x00);
	m_internalLinesColour.Set(0x00,0x00,0x00);
	//m_gridBackgroundColour.Set(0xFF,0xFF,0xFF);
	m_gridBackgroundColour.Set(172, 168, 153);
	

	rotateVector_set_size(size);
}



wxRect WalkGraphic::setArea(int x1,int y1,int x2,int y2)
{
	wxRect track;
	
	m_screen_x1=x1;
	m_screen_x2=x2;
	m_screen_y1=y1;
	m_screen_y2=y2;

	m_grid_y1=m_screen_y1+5;
	m_grid_x2=m_screen_x2-15;           
	m_grid_y2=m_screen_y2-m_font->GetHeight()-6; 
	atualizeVerticalScale(true); // to calculate m_grid_x1

	track = wxRect(m_grid_x1,m_grid_y1,m_grid_x2-m_grid_x1+1,m_grid_y2-m_grid_y1+1);

	return track;
}


void WalkGraphic::setDC(wxMemoryDC *dc)
{
	m_dc = dc;
}

void WalkGraphic::newPoint(float y,wxString x,bool plot_x)
{
	if(y>m_max) m_max=(float)ceil(y);
	if(y<m_min) m_min=(float)floor(y);

	rotateVector_new_point(y,x,plot_x);
}

void WalkGraphic::horizontalDottedLine(int y, int x1, int x2)
{
	int aux = x2;
	if (x1>x2)
	{
		x2 = x1;
		x1 = aux;
	}
	for (int i = x1; i <= x2; i+=4){
		aux=i+1;
		if(aux>x2) aux=x2;
		m_dc->DrawLine(i,y,aux,y);
	}
}

void WalkGraphic::verticalDottedLine(int x, int y1, int y2)
{
	int aux = y2;
	if (y1>y2)
	{
		y2 = y1;
		y1 = aux;
	}
	for (int i = y1; i <= y2; i+=4){
	       aux=i+1;
	       if(aux>y2) aux=y2;
	       m_dc->DrawLine(x,i,x,aux);
    }
}


void WalkGraphic::showGrid(bool traceOn)
{
	int i;
	int aux;
	int len;
	float dif_y=((float)m_grid_y2/10-(float)m_grid_y1/10);
	int fontHeight = m_font->GetHeight();

	m_font->setDC(m_dc);	
	// clean grid area
	m_brush.SetColour(192, 192, 192);

	//m_brush.SetColour(m_gridBackgroundColour);
	m_pen.SetColour(m_gridColour);	

	m_dc->SetBrush(m_brush);
	m_dc->SetPen(m_pen);
	m_dc->DrawRectangle(m_grid_x1,m_grid_y1,m_grid_x2-m_grid_x1+1,m_grid_y2-m_grid_y1+1);
	
	// 3D impression
	m_dc->DrawLine(m_grid_x1, m_grid_y2+1, m_grid_x2+1, m_grid_y2+1);  
	m_dc->DrawLine(m_grid_x2+1,m_grid_y1+1,m_grid_x2+1,m_grid_y2+2);
	
	//  -------- dotted horizontal lines and vertical scale --------------	
	len=m_font->stringWidth(m_verticalAxisStr[0]);
	
	m_dc->DrawLine(m_grid_x1-3,m_grid_y1,m_grid_x1,m_grid_y1);
	
	m_font->DrawString(m_grid_x1-len-5,(int)(m_grid_y1-fontHeight/2+1),m_verticalAxisStr[0]);
	aux=m_grid_y1;
	for(i=1;i<1;i++){
		m_pen.SetColour(m_internalLinesColour);
		m_dc->SetPen(m_pen);	    
	    	if(traceOn) horizontalDottedLine((int)(aux+i*dif_y),m_grid_x1,m_grid_x2);
		m_pen.SetColour(m_gridColour);
		m_dc->SetPen(m_pen);	    	    
		len=m_font->stringWidth(m_verticalAxisStr[i]);
	    	m_dc->DrawLine(m_grid_x1-3,(int)(aux+i*dif_y),m_grid_x1,(int)(aux+i*dif_y));
	    	m_font->DrawString(m_grid_x1-len-5,(int)(aux+i*dif_y-fontHeight/2+1),m_verticalAxisStr[i]);
	}

	// median horizontal full line
	m_pen.SetColour(m_internalLinesColour);
	m_dc->SetPen(m_pen);	    	
	m_dc->DrawLine(m_grid_x1,(int)(m_grid_y1+m_grid_y2)/2,m_grid_x2,(int)(m_grid_y1+m_grid_y2)/2);

	m_pen.SetColour(m_gridColour);
	m_dc->SetPen(m_pen);	    		
	len=m_font->stringWidth(m_verticalAxisStr[5]);
	m_dc->DrawLine(m_grid_x1-3,(int)(m_grid_y1+m_grid_y2)/2,m_grid_x1,(int)(m_grid_y1+m_grid_y2)/2);
	m_font->DrawString(m_grid_x1-len-5,(int)(((m_grid_y1+m_grid_y2)/2)-fontHeight/2+1),m_verticalAxisStr[5]);

	aux=(m_grid_y2+m_grid_y1)/2;
	// Aqui eu defino os ptos do grid
	for(i=1;i<1;i++){   
		m_pen.SetColour(m_internalLinesColour);
		m_dc->SetPen(m_pen);	    
	    	if (traceOn) horizontalDottedLine((int)(aux+i*dif_y),m_grid_x1,m_grid_x2);
		m_pen.SetColour(m_gridColour);
		m_dc->SetPen(m_pen);	    	    	    
	    	len=m_font->stringWidth(m_verticalAxisStr[i+5]);
	    	m_dc->DrawLine(m_grid_x1-3,(int)(aux+i*dif_y),m_grid_x1,(int)(aux+i*dif_y));
	    	m_font->DrawString(m_grid_x1-len-5,(int)(aux+i*dif_y-fontHeight/2+1),m_verticalAxisStr[i+5]);
	}
	len=m_font->stringWidth(m_verticalAxisStr[10]);
	m_dc->DrawLine(m_grid_x1-3,m_grid_y2,m_grid_x1,m_grid_y2);
	m_font->DrawString(m_grid_x1-len-5,m_grid_y2-fontHeight/2+1,m_verticalAxisStr[10]);

}


void WalkGraphic::plotVector(bool traceOn)
{
	float	x,y;
	float	actual_x,actual_y;
	float	actual_val;
	int		data_index; // index for data in rotateVector

	float dx=(float)((float)(m_grid_x2-m_grid_x1)/(float)(rotateVector_size-1)); 

	float dy=0;

	if((m_max-m_min)!=0) dy=(m_grid_y2-m_grid_y1) / (m_max-m_min);

	rotateVector_reset_give_point();
	data_index = rotateVector_give_point();
	if(rotateVector_info!=0) return; // nothing to be ploted
	actual_val = rotateVector_data_y[data_index]; // teke the first value

	// initial coordinates	
	x = actual_x = m_grid_x1; 
	
	actual_y = (m_grid_y2 - (actual_val - m_min)*dy);
	if(actual_y < m_grid_y1) actual_y = m_grid_y1;  
	if(actual_y > m_grid_y2) actual_y = m_grid_y2;

	// set colour
	m_pen.SetColour(m_curveColour);	
	m_dc->SetPen(m_pen);


	do{
	    data_index = rotateVector_give_point();
	    actual_val = rotateVector_data_y[data_index];

	    y = (m_grid_y2-(actual_val-m_min)*dy);
	    if(y<m_grid_y1) y = m_grid_y1;  
		if(y>m_grid_y2) y = m_grid_y2;

	    x+= dx;
	    if(x > m_grid_x2-1) x = m_grid_x2-1;

	    // plot line segment between the last and actual values
	    m_dc->DrawLine((int)actual_x,(int)actual_y,(int)x,(int)y);		

	    actual_x=x;
	    actual_y=y;

		// if necessary, plot horizontal axis value and a vertical line
	    if(rotateVector_data_plot_x[data_index]){
			int str_len=m_font->stringWidth(rotateVector_data_x[data_index]);
			int str_x=(int)x-str_len/2;

			// set colour
			m_pen.SetColour(m_internalLinesColour);	
			m_dc->SetPen(m_pen);
		
			if (traceOn) verticalDottedLine((int)x,m_grid_y1,m_grid_y2);

			// set colour
			m_pen.SetColour(m_gridColour);	
			m_dc->SetPen(m_pen);				
			
			m_dc->DrawLine((int)x,m_grid_y2+1,(int)x,m_grid_y2+4);
			if((str_x>=m_screen_x1)&&((str_x+str_len)<=m_screen_x2)) // existe espaco para plotar a string
				m_font->DrawString((int)x-str_len/2,m_grid_y2+m_font->GetHeight()-2,rotateVector_data_x[data_index]);

			// restaure curve colour
			m_pen.SetColour(m_curveColour);	
			m_dc->SetPen(m_pen);

	    }

	} while(rotateVector_info==0);

}

void WalkGraphic::atualizeVerticalScale(bool forced) 
{
	wxString str;
	char aux[10];	
	static float actual_max=-1,actual_min=-1;
	int len=0; // string width in pixels
	int i;
	float dv=(float)((m_max-m_min)/10);


	// should recalculate vertical strings
	if((forced)||((actual_max!=m_max)||(actual_min!=m_min))){
	    actual_max=m_max; actual_min=m_min; // atualize values
		 
	    sprintf(aux,"_1.%df",m_decimalChars);
	    aux[0]='%';

	    // calculate the width for vertical scale
	    for(i=0;i<11;i++){
			m_verticalAxisStr[i].Printf((const char *)aux,m_max-i*dv);

			//if(m_font->stringWidth(m_verticalAxisStr[i])>len) len=m_font->stringWidth(m_verticalAxisStr[i]);
			// Esse eh o valor maluco que eu obtenho com o padrao -1.00 (preciso ver como construir valores solidos)
			if(m_font->stringWidth(m_verticalAxisStr[i])>len) len=32;

	    }
		
		len;
	    // atualize coordinate of the begin of grid
	    m_grid_x1=m_screen_x1+len+6;
	}
}


int WalkGraphic::getMaxStringLen()
{
	int max = 0;

	for(int i=0; i<11; i++){
		if (m_font->stringWidth(m_verticalAxisStr[i]) > max)
				max = m_font->stringWidth(m_verticalAxisStr[i]);
	}
	return max;
}

void WalkGraphic::plot()
{
	//atualizeVerticalScale(false);
	atualizeVerticalScale(true);
	showGrid(false); // probl aqui
	plotVector(false);
}


void WalkGraphic::plotSelect(int start, int end, int offset)
{
	start = start + offset;
	end   = end   + offset;

	atualizeVerticalScale(false);
	showGridSelected(false,start,end);
	plotVector(false);
}


void WalkGraphic::redefineVerticalAxis()
{
	int data_index;
	float y;

	m_max=m_min=0;
	rotateVector_reset_give_point();
	do{
	    data_index=rotateVector_give_point();
	    y=rotateVector_data_y[data_index];
	    if(y>m_max) m_max=(float)ceil(y);
	    if(y<m_min) m_min=(float)floor(y);

	} while(rotateVector_info==0);

	atualizeVerticalScale(true);	
}



void WalkGraphic::scaleVerticalAxis(float scale_factor)
{
	if(scale_factor>1){
	   m_max*=scale_factor;
	   m_min*=scale_factor;
	  }
    
	atualizeVerticalScale(true);	
}


void WalkGraphic::setSize(int size)
{
	rotateVector_set_size(size);
}

void WalkGraphic::getMaxYPoint(float *y_max)
{
	*y_max = m_max;
}
	
void WalkGraphic::getMinYPoint(float *y_min)
{
	*y_min = m_min;
}

void WalkGraphic::getArea(int *x1,int *y1,int *x2,int *y2)
{
	*x1 = m_grid_x1;
    *x2 = m_grid_x2;
	*y1 = m_grid_y1;
    *y2 = m_grid_y2;
}

void WalkGraphic::showGridSelected(bool traceOn,int start, int end)
{
	int		i;
	int		aux;
	int		len;
	float		dif_y = ((float)m_grid_y2/10-(float)m_grid_y1/10);
	int		fontHeight = m_font->GetHeight();
	
	m_font->setDC(m_dc);
	
	m_brush.SetColour(m_gridBackgroundColour);
	m_pen.SetColour(m_gridColour);	

	m_dc->SetBrush(m_brush);
	m_dc->SetPen(m_pen);
	m_dc->DrawRectangle(m_grid_x1,m_grid_y1,m_grid_x2-m_grid_x1+1,m_grid_y2-m_grid_y1+1);
	
	// Draw Selection

	m_brush.SetColour(148,148,170); // backGround colour
	m_dc->SetBrush(m_brush);
	m_pen.SetColour(148, 148, 170);	
	m_dc->SetPen(m_pen);

	//m_dc->DrawRectangle(start,m_grid_y1,end-start,m_grid_y2-m_grid_y1+1);
	m_dc->DrawRectangle(start,m_grid_y1+1,end-start,m_grid_y2-m_grid_y1-1);
	
	m_brush.SetColour(0xA0,0xA0,0xA0);
	m_dc->SetBrush(m_brush);
	m_pen.SetColour(m_gridColour);	
	m_dc->SetPen(m_pen);

	m_dc->DrawLine(m_grid_x1, m_grid_y2+1, m_grid_x2+1, m_grid_y2+1);  
	m_dc->DrawLine(m_grid_x2+1,m_grid_y1+1,m_grid_x2+1,m_grid_y2+2);

	//  -------- dotted horizontal lines and vertical scale --------------	
	len=m_font->stringWidth(m_verticalAxisStr[0]);
	m_dc->DrawLine(m_grid_x1-3,m_grid_y1,m_grid_x1,m_grid_y1);
	m_font->DrawString(m_grid_x1-len-5,(int)(m_grid_y1-fontHeight/2+1),m_verticalAxisStr[0]);

	aux=m_grid_y1;
	for(i=1;i<1;i++){
		m_pen.SetColour(m_internalLinesColour);
		m_dc->SetPen(m_pen);	    
	    if(traceOn) horizontalDottedLine((int)(aux+i*dif_y),m_grid_x1,m_grid_x2);

		m_pen.SetColour(m_gridColour);
		m_dc->SetPen(m_pen);	    	    
	    len=m_font->stringWidth(m_verticalAxisStr[i]);
	    m_dc->DrawLine(m_grid_x1-3,(int)(aux+i*dif_y),m_grid_x1,(int)(aux+i*dif_y));
	    m_font->DrawString(m_grid_x1-len-5,(int)(aux+i*dif_y-fontHeight/2+1),m_verticalAxisStr[i]);
   }

	// median horizontal full line
	m_pen.SetColour(m_internalLinesColour);
	m_dc->SetPen(m_pen);	    	
	m_dc->DrawLine(m_grid_x1,(int)(m_grid_y1+m_grid_y2)/2,m_grid_x2,(int)(m_grid_y1+m_grid_y2)/2);

	m_pen.SetColour(m_gridColour);
	m_dc->SetPen(m_pen);	    		
	len=m_font->stringWidth(m_verticalAxisStr[5]);
	m_dc->DrawLine(m_grid_x1-3,(int)(m_grid_y1+m_grid_y2)/2,m_grid_x1,(int)(m_grid_y1+m_grid_y2)/2);
	m_font->DrawString(m_grid_x1-len-5,(int)(((m_grid_y1+m_grid_y2)/2)-fontHeight/2+1),m_verticalAxisStr[5]);

	aux=(m_grid_y2+m_grid_y1)/2;
	for(i=1;i<1;i++){
		m_pen.SetColour(m_internalLinesColour);
		m_dc->SetPen(m_pen);	    
	    if (traceOn) horizontalDottedLine((int)(aux+i*dif_y),m_grid_x1,m_grid_x2);

		m_pen.SetColour(m_gridColour);
		m_dc->SetPen(m_pen);	    	    	    
	    len=m_font->stringWidth(m_verticalAxisStr[i+5]);
	    m_dc->DrawLine(m_grid_x1-3,(int)(aux+i*dif_y),m_grid_x1,(int)(aux+i*dif_y));
	    m_font->DrawString(m_grid_x1-len-5,(int)(aux+i*dif_y-fontHeight/2+1),m_verticalAxisStr[i+5]);
	   }

	len=m_font->stringWidth(m_verticalAxisStr[10]);
	m_dc->DrawLine(m_grid_x1-3,m_grid_y2,m_grid_x1,m_grid_y2);
	m_font->DrawString(m_grid_x1-len-5,m_grid_y2-fontHeight/2+1,m_verticalAxisStr[10]);

}


