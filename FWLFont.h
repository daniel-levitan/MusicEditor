#ifndef __FWLFONT_H__
#define __FWLFONT_H__

#include <string.h>
#include <stdio.h>


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#define HEADER_SIZE 20
#define DEFAULT_FILENAME "SMALL.FNT"

typedef unsigned char byte;

typedef struct FWLFontHeader{

  char ident[13];
  byte width;
  byte height;
  byte bytesPerCharacter;
  byte reserved[50];
  
} FWLFontHeader;


class FWLFont{

 unsigned char *m_buffer;
 int m_error;
 bool m_allocatedMemory;
 int m_width,m_height;
 int m_bytesPerCharacter;

 wxMemoryDC *m_dc;
 
 int inicialize(char *filename);

public:
 FWLFont();
 FWLFont(char *filename);
 ~FWLFont();
 int error(){ return m_error; }
 void setDC(wxMemoryDC *dc){ m_dc = dc; }
 void DrawChar(int x,int y,unsigned char ch);
 void DrawString(int x,int y,const char *str);
 int stringWidth(const char *str);
 int GetHeight();
 
};
#endif
