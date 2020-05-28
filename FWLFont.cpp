#include "FWLFont.h"

FWLFont::FWLFont()
{
 m_error=inicialize(DEFAULT_FILENAME); 
};


FWLFont::FWLFont(char *filename)
{
 m_error=inicialize(filename); 
};

FWLFont::~FWLFont()
{
	if(m_allocatedMemory) delete [] m_buffer; 
}

// 0 sucess
// 1 no memory
// 2 read disk error
// 3 invalid file
int FWLFont::inicialize(char *filename)
{
 FILE *f;
 FWLFontHeader header;

 m_allocatedMemory=0; 

 if((f=fopen(filename,"rb"))==NULL) return 2;
 fread(&header,sizeof(header),1,f);

 m_width=header.width;
 m_height=header.height;
 m_bytesPerCharacter=header.bytesPerCharacter;

 m_buffer = new unsigned char[256*m_bytesPerCharacter];
 if(m_buffer) m_allocatedMemory=true;
 else return 1;

 if(strcmp(header.ident,"GRAPHFONTFWL")) return 3;

 fread(m_buffer,256*m_bytesPerCharacter,1,f);
 fclose(f);
 return 0;
}


void FWLFont::DrawChar(int x,int y,unsigned char ch)
{
 unsigned char *aux=m_buffer+(ch*m_bytesPerCharacter);
 for(int l=0;l<m_height;l++){
     int m=1;
     for(int c=0;c<m_width;c++){
		 if((*aux)&m) m_dc->DrawPoint(x+c,y+l);
		 m*=2;
		}
     aux++;
    }
}

void FWLFont::DrawString(int x,int y,const char *str)
{
 int c=0;
 int index=0;
 while( str[index] != NULL ){
 //while( str[index] != ' ' ){
		DrawChar(x+c,y,str[index]);
		index++;
		c+=m_width;
	}
}

int FWLFont::stringWidth(const char *str)
{
	return strlen(str)*m_width;

}

int FWLFont::GetHeight()
{
	return m_height;
}
