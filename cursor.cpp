#include "cursor.h"

/*
static const char * IBeamCursorXpm[] = {
"32 32 3 1",
".	c #FF0000", // mask color = RGB:255,0,0
"#	c #000000",
"+	c #FFFFFF",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
".............####.###...........",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
"................##..............",
".............####.###...........",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................",
"................................"};

*/
// Use CursorId as a fallback.
wxCursor * MakeCursor( int CursorId, const char * pXpm[36],  int HotX, int HotY )
{
   wxCursor * pCursor;

#ifdef CURSORS_SIZE32
   const int HotAdjust =0;
#else
   const int HotAdjust =8;
#endif

#if 0
   pCursor = new wxCursor(CursorId);
#endif

   wxImage Image = wxImage(wxBitmap(pXpm).ConvertToImage());   
   Image.SetMaskColour(255,0,0);
   Image.SetMask();// Enable mask.

#ifdef __WXGTK__
   //
   // Kludge: the wxCursor Image constructor is broken in wxGTK.
   // This code, based loosely on the broken code from the wxGTK source,
   // works around the problem by constructing a 1-bit bitmap and
   // calling the other custom cursor constructor.
   //
   // -DMM
   //

   unsigned char *rgbBits = Image.GetData();
   int w = Image.GetWidth() ;
   int h = Image.GetHeight();
   int imagebitcount = (w*h)/8;
   
   unsigned char *bits = new unsigned char [imagebitcount];
   unsigned char *maskBits = new unsigned char [imagebitcount];

   int i, j, i8;
   unsigned char cMask;
   for (i=0; i<imagebitcount; i++) {
      bits[i] = 0;
      i8 = i * 8;
        
      cMask = 1;
      for (j=0; j<8; j++) {
         if (rgbBits[(i8+j)*3+2] < 127)
            bits[i] = bits[i] | cMask;
         cMask = cMask * 2;
      }
   }

   for (i=0; i<imagebitcount; i++) {
      maskBits[i] = 0x0;
      i8 = i * 8;
      
      cMask = 1;
      for (j=0; j<8; j++) {
         if (rgbBits[(i8+j)*3] < 127 || rgbBits[(i8+j)*3+1] > 127)
            maskBits[i] = maskBits[i] | cMask;
         cMask = cMask * 2;
      }
   }

   pCursor = new wxCursor((const char *)bits, w, h,
                          HotX-HotAdjust, HotY-HotAdjust,
                          (const char *)maskBits,
                          new wxColour(0, 0, 0),
                          new wxColour(255, 255, 255));

#else
   Image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, HotX-HotAdjust );
   Image.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, HotY-HotAdjust );
   pCursor = new wxCursor( Image );
#endif

   return pCursor;
}

