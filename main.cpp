/*******************************************************************************************
 *                                                                                         *
 * Main Program File                                                                       *
 *                                                                                         *
 *                                                                                         *
 * File: main.cpp                                                                          *
 *                                                                                         *
 *                                                                                         *
 * Author: Daniel Levitan                                                                  *
 *                                                                                         *
 *******************************************************************************************/


// Contastantes
#define INI_HEIGHT 170



// wxHeaders 

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/file.h"
#include "wx/colour.h"
#include "wx/dialog.h"
//#include "wx/clipbrd.h"

// std header
#include <stdlib.h>

// auxiliary libs headers
#include "WalkGraphic.h"
#include "sndfile.h"
//#include "soundEffects.h"
#include "track.h"
#include "cursor.h"
#include "player.h"
#include "encoder.h"
//#include "decoder.h"
#include "mad.h"


// toolbar headers
#include "wx/toolbar.h"
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/playSel.xpm"
#include "bitmaps/expand.xpm"
#include "bitmaps/fit.xpm"
#include "bitmaps/zoomIn.xpm"
#include "bitmaps/zoomOut.xpm"
#include "bitmaps/teste.xpm"




int decode(const char *filename, SF_INFO &info);
#define INPUT_BUFFER_SIZE 65535

struct private_data {
                FILE                    *file;
                unsigned char			*inputBuffer;
                int                     numChannels;
                int                     samplerate;
                int                     len;
                float                   *sample;
				//signed int              *sample;
                int                     icount,
                                        ocount;
                bool                    firstRun;
                //void                  *userData; //useless
};

/* The MAD callbacks */
enum mad_flow input_cb (void *_data, struct mad_stream *stream);
enum mad_flow output_cb(void *_data, struct mad_header const *header, struct mad_pcm *pcm);
enum mad_flow error_cb (void *_data, struct mad_stream *stream, struct mad_frame *frame);




//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// MyApp  MyFrame  MyCanvas  ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////


// ---------------------------------------- MyApp ----------------------------------------------------
class MyApp: public wxApp
{
public:
    MyApp();
    ~MyApp();

    bool OnInit();
};

class MyFrame;

// ------------------------------------- MyCanvas ----------------------------------------------------
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(MyFrame* parent);
	~MyCanvas();

	// FileMenu
	void newFile(int samplerate);
	void newFileStereo(int samplerate);
	void openFile(const char *filename);				// Preciso carregar abrindo arquivo
	void saveFile();
	void saveFileAs(const char *filename);

	void exportMp3As(const char *filename);

	void closeFile();

	//Effect Menu
	void effectNull();
	void effectReverseNull();
	void gain();
	void fadeIn();
	void fadeOut();
	void crossOver();
	void toStereo();
	void toMono();
	
	// View Menu
	void zoomHorIn();
	void zoomHorOut();

	void ams();
	void temp();
	void maxAmp();
	void separate();


	// Edit Menu
	void copy();
	void paste();
	void cut();
	void selectAll();

	// not directly used functions
	void drawOpenedFile(float *fptr,int size);
	void drawOpenedFileSelect(float *fptr,int size, int start, int end);

	void drawOpenedFileStereo(float *fptrLeft,float *fptrRight,int size);
	void drawOpenedFileSelectStereo(float *fptrLeft,float *fptrRight,int size, int start, int end);
	
	void clearArea();									  // Clean screen through myCanvas

	// event handlers
	void OnPaint(wxPaintEvent &event);

	void OnMouseButtonDown(wxMouseEvent &event);		  // On the event button down - Start Selection
	void OnMouseButtonUp(wxMouseEvent &event);			  // On the event button up   - End Selection
	void OnMouseMove(wxMouseEvent &event);
	void OnMouseDoubleClick(wxMouseEvent &event);

	// Player
	void play();
	void playSelected();
	void expand();
	void fit();

	void teste();

	// Variables
	WalkGraphic		*m_walkGraphic;
	WalkGraphic		*m_walkGraphicLeft;
	WalkGraphic		*m_walkGraphicRight;

	Track			*m_track;

	wxCursor		*mSelectCursor;
	float			*m_clipboard;
	float			*m_clipboardLeft,*m_clipboardRight;
	float			m_startClipboard, m_endClipboard;
	int				num, cont;

	bool			showAms; // View, sample
	bool			showTemp; // View, time

	bool			showSeparate;
	bool			selectedL, selectedR;


private:
	MyFrame		*m_owner;
	wxBitmap	*m_buffer;
	wxMemoryDC	m_bufferDC;
	int			m_width,m_height;
	bool		selected;


	DECLARE_EVENT_TABLE()
};

// ------------------------------------- MyFrame -----------------------------------------------------
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	// FileMenu
	void OnNewFile(wxCommandEvent &);
	void OnOpenFile(wxCommandEvent &);
	//void OnCloseFile(wxCommandEvent &);
	void OnClose(wxCommandEvent &);
	void OnSaveFile(wxCommandEvent &);
	void OnSaveFileAs(wxCommandEvent &);
	void OnExportMp3As(wxCommandEvent &);
	void OnQuit (wxCommandEvent &);

	// Effect Menu
	void OnNull(wxCommandEvent &);
	void OnReverseNull(wxCommandEvent &);
	void OnGain(wxCommandEvent &);
	
	void OnFadeIn(wxCommandEvent &);
	void OnFadeOut(wxCommandEvent &);
	void OnCrossOver(wxCommandEvent &);
	void OnToStereo(wxCommandEvent &);
	void OnToMono(wxCommandEvent &);


	// Edit Menu
	void OnCopy(wxCommandEvent &);
	void OnPaste(wxCommandEvent &);
	void OnCut(wxCommandEvent &);
	void OnSelectAll(wxCommandEvent &);

	// View Menu
	void OnZoomHorizIn(wxCommandEvent &);
	void OnZoomHorizOut(wxCommandEvent &);
	//void OnZoomVertIn(wxCommandEvent &);
	//void OnZoomVertOut(wxCommandEvent &);
	void OnAms(wxCommandEvent &);
	void OnTemp(wxCommandEvent &);
	void OnMaxAmp(wxCommandEvent &);
	void OnSeparate(wxCommandEvent &);
	

	// Player
	void OnPlay(wxCommandEvent &);
	void OnPlaySelected(wxCommandEvent &);
	void OnExpand(wxCommandEvent &);
	void OnFit(wxCommandEvent &);


	
	void OnTeste(wxCommandEvent &);




	// variables
	wxMenuBar		*menuBar;
	wxMenu			*fileMenu;
	wxMenu			*effectMenu;
	wxMenu			*editMenu;
	wxMenu			*viewMenu;
	wxMenu			*helpMenu;
	wxMenu			*exibitionMenu; 
	wxToolBar		*toolBar;

	bool			fileOpened;
	//bool			isSample;
	
	
	
private:
	MyCanvas *m_canvas;

	DECLARE_EVENT_TABLE()
};

enum // event IDs
{
	ID_NEW,			//FileMenu
	ID_OPEN,		
	ID_CLOSE,
	ID_SAVE,
	ID_SAVE_AS,
	ID_EXPORTMP3_AS,
	ID_QUIT,
	ID_COPY,		//EditMenu
	ID_PASTE,
	ID_CUT,
	ID_ALL,
	ID_ZOOM_H_IN,	//ViewMenu
	ID_ZOOM_H_OUT,
	//ID_ZOOM_V_IN,
	//ID_ZOOM_V_OUT,
	ID_MAX_AMP,
	ID_AMS,
	ID_TEMP,
	ID_PLAY,
	ID_PLAY_SEL,
	ID_EXPAND,
	ID_SEPARATE,
	ID_FIT,
	ID_NULL,		//EffectMenu
	ID_R_NULL,
	ID_GAIN,
	ID_FADE_OUT,
	ID_FADE_IN,
	ID_CROSS_OVER,
	ID_TO_STEREO,
	ID_TO_MONO,
	ID_TESTE

};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU (ID_NEW, MyFrame::OnNewFile)
	EVT_MENU (ID_OPEN, MyFrame::OnOpenFile)
	EVT_MENU (ID_CLOSE, MyFrame::OnClose)
	EVT_MENU (ID_SAVE, MyFrame::OnSaveFile)
	EVT_MENU (ID_SAVE_AS, MyFrame::OnSaveFileAs)
	EVT_MENU (ID_EXPORTMP3_AS, MyFrame::OnExportMp3As)

	EVT_MENU (ID_QUIT, MyFrame::OnQuit)
	
	EVT_MENU (ID_NULL, MyFrame::OnNull)
	EVT_MENU (ID_R_NULL, MyFrame::OnReverseNull)
	EVT_MENU (ID_GAIN, MyFrame::OnGain)
	EVT_MENU (ID_FADE_IN, MyFrame::OnFadeIn)
	EVT_MENU (ID_FADE_OUT, MyFrame::OnFadeOut)
	EVT_MENU (ID_FADE_OUT, MyFrame::OnFadeOut)
	EVT_MENU (ID_CROSS_OVER, MyFrame::OnCrossOver)
	EVT_MENU (ID_TO_MONO, MyFrame::OnToMono)
	EVT_MENU (ID_TO_STEREO, MyFrame::OnToStereo)
	
	EVT_MENU (ID_COPY, MyFrame::OnCopy)
	EVT_MENU (ID_PASTE, MyFrame::OnPaste)
	EVT_MENU (ID_CUT, MyFrame::OnCut)
	EVT_MENU (ID_ALL, MyFrame::OnSelectAll)
	
	EVT_MENU (ID_ZOOM_H_IN, MyFrame::OnZoomHorizIn)
	EVT_MENU (ID_ZOOM_H_OUT, MyFrame::OnZoomHorizOut)
	//EVT_MENU (ID_ZOOM_V_IN, MyFrame::OnZoomVertIn)
	//EVT_MENU (ID_ZOOM_V_OUT, MyFrame::OnZoomVertOut)
	EVT_MENU (ID_AMS, MyFrame::OnAms)
	EVT_MENU (ID_TEMP, MyFrame::OnTemp)
	
	EVT_MENU (ID_MAX_AMP, MyFrame::OnMaxAmp)
	EVT_MENU (ID_SEPARATE, MyFrame::OnSeparate)
	
	

	EVT_MENU (ID_PLAY, MyFrame::OnPlay)
	EVT_MENU (ID_PLAY_SEL, MyFrame::OnPlaySelected)
	EVT_MENU (ID_EXPAND, MyFrame::OnExpand)
	EVT_MENU (ID_FIT, MyFrame::OnFit)

	EVT_MENU (ID_TESTE, MyFrame::OnTeste)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
	EVT_LEFT_DOWN(MyCanvas::OnMouseButtonDown)
	EVT_LEFT_UP(MyCanvas::OnMouseButtonUp)
	EVT_LEFT_DCLICK(MyCanvas::OnMouseDoubleClick)
	EVT_MOUSE_EVENTS(MyCanvas::OnMouseMove)


END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)


//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  Implementation  ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------- MyApp ----------------------------------------------------
MyApp::MyApp()
{
}

MyApp::~MyApp()
{
}

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame ("Editor de Arquivos de Audio Multiplataforma",wxPoint(100,100), wxSize(600,500));
	

	frame->Show (TRUE);
    SetTopWindow (frame);

	return TRUE;
}


// ------------------------------------- MyFrame -----------------------------------------------------
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& 
size)
    : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    m_canvas = new MyCanvas(this);

	//  -------------------- menu --------------------
	menuBar = new wxMenuBar();
	fileMenu = new wxMenu();
	fileMenu->Append(ID_NEW, "&New", "New File");
	fileMenu->Append(ID_OPEN, "&Open ...", "Open File");
	fileMenu->Append(ID_SAVE, "&Save", "Save File");
	fileMenu->Append(ID_SAVE_AS, "Save &As...", "Save File As");
	fileMenu->Append(ID_EXPORTMP3_AS, "Export &Mp3 As...", "Export Mp3 As");
	fileMenu->Append(ID_CLOSE, "&Close", "Close File");
	fileMenu->AppendSeparator();
	fileMenu->Append(ID_QUIT, "E&xit\tAlt-X", "Leave the program");
	menuBar->Append(fileMenu, "&File");								// File Menu


	editMenu = new wxMenu();
	editMenu->Append(ID_COPY, "&Copy\tCTRL-C", "Copy");
	editMenu->Append(ID_PASTE, "&Paste\tCTRL-V", "Paste");
	editMenu->Append(ID_CUT, "&Cut\tCTRL-X", "Cut");
	editMenu->Append(ID_ALL, "&Select All\tCTRL-A", "Select All");

	menuBar->Append(editMenu, "&Edit");						// /Edit Menu

	viewMenu = new wxMenu();

	//viewMenu->Append(ID_TEMP_AMS, "Tempo / Amostra X Amplitude", "Tempo / Amostra X Amplitude");
    //viewMenu->AppendCheckItem(ID_TEMP_AMS, _T("Tempo/Amostra\tCtrl-M"));
	
	exibitionMenu = new wxMenu();
	exibitionMenu ->AppendCheckItem(ID_AMS, _T("Sample"));
	exibitionMenu ->AppendCheckItem(ID_TEMP, _T("Time"));
	
	viewMenu->Append(-2,"Exibition",exibitionMenu);


	viewMenu->AppendSeparator();
	viewMenu->Append(ID_ZOOM_H_IN, "Zoom Horizontal In", "Zoom Horizontal In");
	viewMenu->Append(ID_ZOOM_H_OUT, "Zoom Horizontal Out", "Zoom Horizontal Out");
	//viewMenu->AppendSeparator();
	//viewMenu->Append(ID_ZOOM_V_IN, "Zoom Vertical In", "Zoom Vertical In");
	//viewMenu->Append(ID_ZOOM_V_OUT, "Zoom Vertical Out", "Zoom Vertical Out");
	viewMenu->AppendSeparator();
	viewMenu->Append(ID_FIT, "Fit", "Fit into the screen");
	viewMenu->Append(ID_EXPAND, "Expand", "Expand the selected part to the whole screen");

	viewMenu->AppendSeparator();
	viewMenu->Append(ID_MAX_AMP, "Max. Amplitude", "Max. Amplitude");
	//viewMenu->Append(ID_SEPARATE, "Separate", "Separate Stereo Channels");
	viewMenu->AppendCheckItem(ID_SEPARATE, "Separate", "Separate Stereo Channels");

	menuBar->Append(viewMenu, "&View");						// View Menu

	effectMenu = new wxMenu();
	effectMenu->Append(ID_NULL, "&Null", "Null");
	effectMenu->Append(ID_R_NULL, "&Reverse Null", "Reverse Null");
	effectMenu->AppendSeparator();
	effectMenu->Append(ID_GAIN, "&Gain", "Gain");
	effectMenu->Append(ID_FADE_IN, "Fade &In", "Fade In");
	effectMenu->Append(ID_FADE_OUT, "Fade &Out", "Fade Out");
	effectMenu->Append(ID_CROSS_OVER, "Cross Over", "Cross Over");
	effectMenu->Append(ID_TO_STEREO, "To &Stereo", "To Stereo");
	effectMenu->Append(ID_TO_MONO, "To &Mono", "To Mono");
	menuBar->Append(effectMenu, "Effe&ct");						// Effect Menu

	helpMenu = new wxMenu();
	helpMenu->Append(ID_TESTE, "Teste\tCTRL-D", "Teste");
	menuBar->Append(helpMenu, "&Help");						// Help Menu
	


    SetMenuBar(menuBar);

	//  -------------------- Tool BAR --------------------
	toolBar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT);

	wxBitmap toolBarBitmaps;

	toolBarBitmaps = wxBitmap( new_xpm );
	toolBar->AddTool(ID_NEW, _T("New"), toolBarBitmaps, _T("New file"));

	toolBarBitmaps = wxBitmap( open_xpm );
	toolBar->AddTool(ID_OPEN, _T("Open"), toolBarBitmaps, _T("Open file"));

	toolBarBitmaps = wxBitmap( save_xpm );
	toolBar->AddTool(ID_SAVE, _T("Save"), toolBarBitmaps, _T("Save file"));

	toolBar->AddSeparator();

	toolBarBitmaps = wxBitmap( copy_xpm );
	toolBar->AddTool(ID_COPY, _T("Copy"), toolBarBitmaps, _T("Copy file"));

	toolBarBitmaps = wxBitmap( paste_xpm );
	toolBar->AddTool(ID_PASTE, _T("Paste"), toolBarBitmaps, _T("Paste file"));

	toolBarBitmaps = wxBitmap( cut_xpm );
	toolBar->AddTool(ID_CUT, _T("Cut"), toolBarBitmaps, _T("Cut file"));

	toolBar->AddSeparator();

	toolBarBitmaps = wxBitmap( play_xpm );
	toolBar->AddTool(ID_PLAY, _T("Play"), toolBarBitmaps, _T("Play file"));

	toolBarBitmaps = wxBitmap( play_sel_xpm );
	toolBar->AddTool(ID_PLAY_SEL, _T("Play Selected"), toolBarBitmaps, _T("Play Selected "));

	toolBar->AddSeparator();
	toolBarBitmaps = wxBitmap( expand_xpm );
	toolBar->AddTool(ID_EXPAND, _T("Expand"), toolBarBitmaps, _T("Expand"));
	toolBarBitmaps = wxBitmap( fit_xpm );
	toolBar->AddTool(ID_FIT, _T("Fit"), toolBarBitmaps, _T("Fit"));

	toolBar->AddSeparator();
	toolBarBitmaps = wxBitmap( zoomIn_xpm );
	toolBar->AddTool(ID_ZOOM_H_IN, _T("Zoom In"), toolBarBitmaps, _T("Zoom In"));
	
	toolBarBitmaps = wxBitmap( zoomOut_xpm );
	toolBar->AddTool(ID_ZOOM_H_OUT, _T("Zoom Out"), toolBarBitmaps, _T("Zoom Out"));

	toolBarBitmaps = wxBitmap( teste_xpm );
	toolBar->AddTool(ID_TESTE, _T("teste"), toolBarBitmaps, _T("Teste"));




	SetToolBar(toolBar);
	toolBar->Realize();

	// -------------------- statusBar --------------------
	CreateStatusBar(3);

	fileOpened = false;
	//isSample = true;
	

	// MenuBar
	menuBar->Enable(ID_CLOSE, false);   // File Menu
	menuBar->Enable(ID_SAVE, false);   
	menuBar->Enable(ID_SAVE_AS, false);   
	menuBar->Enable(ID_EXPORTMP3_AS, false);   


	menuBar->Enable(ID_COPY, false);	// Edit Menu
	//menuBar->Enable(ID_PASTE, false);
	menuBar->Enable(ID_CUT, false);
	
	menuBar->Enable(ID_ZOOM_H_IN, false);   // View Menu
	menuBar->Enable(ID_ZOOM_H_OUT, false);
	//menuBar->Enable(ID_ZOOM_V_IN, false);
	//menuBar->Enable(ID_ZOOM_V_OUT, false);
	menuBar->Enable(ID_FIT, false);
	menuBar->Enable(ID_EXPAND, false);
	menuBar->Enable(ID_SEPARATE, false);

	menuBar->Enable(ID_AMS,false);
	menuBar->Enable(ID_TEMP,false);
	menuBar->Enable(ID_MAX_AMP, false);
	
	menuBar->Enable(ID_NULL, false);       //Effect Menu
	menuBar->Enable(ID_R_NULL, false);
	menuBar->Enable(ID_GAIN, false);
	menuBar->Enable(ID_FADE_IN, false);
	menuBar->Enable(ID_FADE_OUT, false);
	menuBar->Enable(ID_CROSS_OVER, false);
	menuBar->Enable(ID_TO_MONO, false);
	menuBar->Enable(ID_TO_STEREO, false);


	// ToolBar
	toolBar->EnableTool(ID_SAVE,false);

	toolBar->EnableTool(ID_PLAY,false);
	toolBar->EnableTool(ID_PLAY_SEL,false);

	toolBar->EnableTool(ID_EXPAND,false);
	toolBar->EnableTool(ID_FIT,false);

	toolBar->EnableTool(ID_ZOOM_H_IN, false);
	toolBar->EnableTool(ID_ZOOM_H_OUT, false);

	toolBar->EnableTool(ID_COPY, false);
	//toolBar->EnableTool(ID_PASTE, false);
	toolBar->EnableTool(ID_CUT, false);

}



// ---- FILE MENU


void MyFrame::OnNewFile(wxCommandEvent &)
{

      const wxString choices[] = { _T("Mono"), _T("Stereo")} ;
      wxString		str;
      int		samplerate = 0;	
    	
      str = wxGetTextFromUser(_T("Set the samplerate"),
								_T("Sample Rate:"),
								_T("44100"),
								this,
								1, 1, TRUE);

	samplerate = atoi(str);

      wxSingleChoiceDialog dialog(this,
                                _T("Escolha o novo tipo de arquivo"),
								_T("Novo Arquivo"),
								WXSIZEOF(choices), 
								choices);

	
    dialog.SetSelection(2);

    if (dialog.ShowModal() == wxID_OK)
    {
		
		if (dialog.GetStringSelection() == "Mono")
			m_canvas->newFile(samplerate);
				
		if (dialog.GetStringSelection() == "Stereo"){
			m_canvas->newFileStereo(samplerate);
			menuBar->Enable(ID_SEPARATE, true);
		}
			
		//wxMessageBox( dialog.GetStringSelection() );
        //wxMessageDialog dialog2(this, dialog.GetStringSelection(), _T("Got string"));
        //dialog2.ShowModal();
		fileOpened = true;

		// Menu Bar
		menuBar->Enable(ID_NEW, false);   // File Menu
		menuBar->Enable(ID_OPEN, false);   
		menuBar->Enable(ID_SAVE, true);
		menuBar->Enable(ID_SAVE_AS, true); 
		menuBar->Enable(ID_EXPORTMP3_AS, true); 
		menuBar->Enable(ID_CLOSE, true);
		
		menuBar->Enable(ID_ZOOM_H_IN, true);  // View Menu
		menuBar->Enable(ID_ZOOM_H_OUT, true);
		menuBar->Enable(ID_FIT, true);
		menuBar->Enable(ID_EXPAND, true);
		menuBar->Enable(ID_AMS,true);
		menuBar->Enable(ID_TEMP,true);
		menuBar->Enable(ID_MAX_AMP, true);


		// Toolbar
		toolBar->EnableTool(ID_NEW,false);
		toolBar->EnableTool(ID_OPEN,false);
		toolBar->EnableTool(ID_SAVE,true);

		toolBar->EnableTool(ID_ZOOM_H_IN,true);
		toolBar->EnableTool(ID_ZOOM_H_OUT,true);
		toolBar->EnableTool(ID_EXPAND,true);
		toolBar->EnableTool(ID_FIT,true);

		toolBar->EnableTool(ID_FIT,true);
		toolBar->EnableTool(ID_PLAY,true);

		
    }


}

void MyFrame::OnOpenFile(wxCommandEvent &)
{
	static wxString s_extDef;
	
	wxString path = wxFileSelector(
                                    _T("Select the file to load"),
                                    _T(""), _T(""),
                                    s_extDef,
                                    _T("Waveform (*.wav)|*.wav|Compressed (*.mp3)|*.mp3|All files (*.*)|*.*"),
                                    wxCHANGE_DIR,
                                    this
                                   );

    	s_extDef = path.AfterLast(_T('.'));

	
	
	if (!path) {
		//wxMessageBox("No path");
	}
	else
	{
		
		SetTitle(path);
		fileOpened = true;
		// Menu Bar
		menuBar->Enable(ID_NEW, false);			// File Menu
		menuBar->Enable(ID_OPEN, false);   
		menuBar->Enable(ID_SAVE, true);   
		menuBar->Enable(ID_SAVE_AS, true); 
		menuBar->Enable(ID_EXPORTMP3_AS, true); 
		menuBar->Enable(ID_CLOSE, true);
		
		menuBar->Enable(ID_ZOOM_H_IN, true);	// View Menu
		menuBar->Enable(ID_ZOOM_H_OUT, true);
		menuBar->Enable(ID_FIT, true);
		menuBar->Enable(ID_EXPAND, false);
		menuBar->Enable(ID_AMS,true);
		menuBar->Enable(ID_TEMP,true);
		menuBar->Enable(ID_MAX_AMP, true);


		// Toolbar
		toolBar->EnableTool(ID_NEW,false);
		toolBar->EnableTool(ID_OPEN,false);
		toolBar->EnableTool(ID_SAVE,true);

		toolBar->EnableTool(ID_ZOOM_H_IN,true);
		toolBar->EnableTool(ID_ZOOM_H_OUT,true);
		toolBar->EnableTool(ID_EXPAND,false);
		toolBar->EnableTool(ID_FIT,true);

		toolBar->EnableTool(ID_FIT,true);
		toolBar->EnableTool(ID_PLAY,true);

		m_canvas->openFile( path );
	}
}


void MyFrame::OnSaveFile(wxCommandEvent &)
{
	m_canvas->saveFile();
}

void MyFrame::OnSaveFileAs(wxCommandEvent &)
{
	//wxMessageBox("To dentro do save file as\n");
	
	wxFileDialog dialog(this,
						_T("Choose a file"), 
						_T(""),  
						_T(""), 
						_T("*.wav"), 
						wxSAVE|wxOVERWRITE_PROMPT);
	
	//dialog.SetFilterIndex(1);
	
	if (dialog.ShowModal() == wxID_OK)
    	{
        	//wxLogMessage(_T("%s"),dialog.GetFilename().c_str());
		m_canvas->saveFileAs( dialog.GetFilename().c_str() );
		
    	}

	SetTitle(dialog.GetFilename().c_str());
	
}

void MyFrame::OnExportMp3As(wxCommandEvent &)
{
	wxFileDialog dialog(this,
						_T("Choose a file"), 
						_T(""),  
						_T(""), 
						_T("*.mp3"), 
						wxSAVE|wxOVERWRITE_PROMPT);
	//dialog.SetFilterIndex(1);
	
	if (dialog.ShowModal() == wxID_OK)
    	{
        	//wxLogMessage(_T("%s"),dialog.GetFilename().c_str());
		m_canvas->exportMp3As( dialog.GetFilename().c_str() );
		
    	}

	SetTitle(dialog.GetFilename().c_str());

}

void MyFrame::OnClose(wxCommandEvent &)
{

	SetTitle("Editor de Arquivos de Audio Multiplataforma");
	fileOpened = false;
	
	menuBar->Enable(ID_CLOSE, false);	// File Menu
	menuBar->Enable(ID_SAVE,false);
	menuBar->Enable(ID_SAVE_AS,false);
	menuBar->Enable(ID_EXPORTMP3_AS,false);
	menuBar->Enable(ID_NEW, true);
	menuBar->Enable(ID_OPEN, true);
	

	menuBar->Enable(ID_COPY, false);	// Edit Menu
	menuBar->Enable(ID_PASTE, false);
	menuBar->Enable(ID_CUT, false);
	
	menuBar->Enable(ID_ZOOM_H_IN, false);   // View Menu
	menuBar->Enable(ID_ZOOM_H_OUT, false);
	menuBar->Enable(ID_FIT, false);
	menuBar->Enable(ID_EXPAND, false);
	menuBar->Enable(ID_AMS,false);
	menuBar->Enable(ID_TEMP,false);
	menuBar->Enable(ID_MAX_AMP, false);
	menuBar->Enable(ID_SEPARATE, false);
	
	menuBar->Enable(ID_NULL, false);       //Effect Menu
	menuBar->Enable(ID_R_NULL, false);
	menuBar->Enable(ID_GAIN, false);
	menuBar->Enable(ID_FADE_IN, false);
	menuBar->Enable(ID_FADE_OUT, false);
	menuBar->Enable(ID_CROSS_OVER, false);
	menuBar->Enable(ID_TO_MONO, false);
	menuBar->Enable(ID_TO_STEREO, false);


	// ToolBar
	toolBar->EnableTool(ID_NEW,true);
	toolBar->EnableTool(ID_OPEN,true);
	toolBar->EnableTool(ID_SAVE,false);

	toolBar->EnableTool(ID_PLAY,false);
	toolBar->EnableTool(ID_PLAY_SEL,false);


	toolBar->EnableTool(ID_EXPAND,false);
	toolBar->EnableTool(ID_FIT,false);

	toolBar->EnableTool(ID_ZOOM_H_IN, false);
	toolBar->EnableTool(ID_ZOOM_H_OUT, false);
	toolBar->EnableTool(ID_FIT, false);
	toolBar->EnableTool(ID_EXPAND, false);



	toolBar->EnableTool(ID_COPY, false);
	toolBar->EnableTool(ID_PASTE, false);
	toolBar->EnableTool(ID_CUT, false);
	
	m_canvas->closeFile();
	//m_canvas->clearArea();
	//m_canvas->m_track->m_startPoint = m_canvas->m_track->m_endPoint = 0;
	SetStatusText( "", 0 );
	SetStatusText( "", 1 );
	SetStatusText( "", 2 );
}

void MyFrame::OnQuit (wxCommandEvent &)
{
    Close (TRUE);
}

void MyFrame::OnNull(wxCommandEvent &)
{
	m_canvas->effectNull();
}

void MyFrame::OnReverseNull(wxCommandEvent &)
{
	m_canvas->effectReverseNull();
}

void MyFrame::OnGain(wxCommandEvent &)
{
	m_canvas->gain();
}

void MyFrame::OnFadeIn(wxCommandEvent &)
{
	m_canvas->fadeIn();
}

void MyFrame::OnFadeOut(wxCommandEvent &)
{
	m_canvas->fadeOut();
}

void MyFrame::OnCrossOver(wxCommandEvent &)
{
	m_canvas->crossOver();
}

void MyFrame::OnToStereo(wxCommandEvent &)
{
	m_canvas->toStereo();
}

void MyFrame::OnToMono(wxCommandEvent &)
{
	m_canvas->toMono();
}

void MyFrame::OnCopy(wxCommandEvent &)
{
	m_canvas->copy();
}

void MyFrame::OnPaste(wxCommandEvent &)
{
	m_canvas->paste();
}

void MyFrame::OnCut(wxCommandEvent &)
{
	m_canvas->cut();
}

void MyFrame::OnSelectAll(wxCommandEvent &)
{
	m_canvas->selectAll();
}


void MyFrame::OnPlay(wxCommandEvent &)
{
	m_canvas->play();
}

void MyFrame::OnPlaySelected(wxCommandEvent &)
{
	m_canvas->playSelected();
}

void MyFrame::OnZoomHorizIn(wxCommandEvent &)
{
	m_canvas->zoomHorIn();
}

void MyFrame::OnZoomHorizOut(wxCommandEvent &)
{
	m_canvas->zoomHorOut();
}

/*
void MyFrame::OnZoomVertIn(wxCommandEvent &)
{
	wxMessageBox("Zoom Vertical In\n","Zoom Vertical In", wxICON_INFORMATION | wxOK );
}

void MyFrame::OnZoomVertOut(wxCommandEvent &)
{
	wxMessageBox("Zoom Vertical Out\n","Zoom Vertical Out", wxICON_INFORMATION | wxOK );
}
*/

void MyFrame::OnAms(wxCommandEvent &)
{
	m_canvas->ams();
}

void MyFrame::OnTemp(wxCommandEvent &)
{
	m_canvas->temp();
}

void MyFrame::OnMaxAmp(wxCommandEvent &)
{
	m_canvas->maxAmp();
}

void MyFrame::OnSeparate(wxCommandEvent &)
{
	m_canvas->separate();
}



void MyFrame::OnExpand(wxCommandEvent &)
{
	m_canvas->expand();
}

void MyFrame::OnFit(wxCommandEvent &)
{
	m_canvas->fit();
}



void MyFrame::OnTeste(wxCommandEvent &)
{
	/*
	const wxString choices[] = { _T("Mono"), _T("Stereo")} ;

    wxSingleChoiceDialog dialog(this,
                                
                                _T("Escolha o novo tipo de arquivo"),
								_T("Novo Arquivo"),
                                WXSIZEOF(choices), choices);

    dialog.SetSelection(2);

    if (dialog.ShowModal() == wxID_OK)
    {
        wxMessageDialog dialog2(this, dialog.GetStringSelection(), _T("Got string"));
        dialog2.ShowModal();
    }
	*/
	m_canvas->teste();
}

// ------------------------------------- MyCanvas ----------------------------------------------------
MyCanvas::MyCanvas(MyFrame* parent):
wxScrolledWindow(parent, -1,wxDefaultPosition, wxDefaultSize, wxHSCROLL)

{
	m_owner  = parent;
	m_width  = m_height = -10;
	m_buffer = NULL;
	m_track  = new Track();
	num		 = 0;
	cont     = 0;
	
	showAms  = false;
	showTemp = false;
	
	selected = false; 
	showSeparate = false;
	selectedL = false;
	selectedR = false;

	wxColour backgroundColour(172, 168, 153); 
	SetBackgroundColour(backgroundColour);

	mSelectCursor  = MakeCursor( wxCURSOR_IBEAM, IBeamCursorXpm,   17, 16);
	
	SetScrollRate( 10, 10 );
        SetVirtualSize( 0, 0 );

}


MyCanvas::~MyCanvas()
{
	if (m_track->trackInfo.channels == 2){
		if (m_walkGraphicLeft) free(m_walkGraphicLeft);
		if (m_walkGraphicRight) free(m_walkGraphicRight);
	}
	if (m_track->trackInfo.channels == 1)
		if (m_walkGraphic) free(m_walkGraphic);

	if (m_track) free(m_track);
}

/*	
    ------------------MyCanvas::OnPaint(wxPaintEvent &event)-----------------------
	Objetivo : Responsável por redesenhar a tela quando necessário
	Recebe   : wxPaintEvent
	Devolve  : void

	Goal       : Redraw the screen when necessary
	Parameters : wxPaintEvent
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------------------------------------
*/
void MyCanvas::OnPaint(wxPaintEvent &event)
{
	wxString	str;
	wxPen		pen;
	wxBrush		brush;
	wxColor		color;
	wxColour	colour;
	int		width,
       			height;
	int		x,
			y;

	// get device context
	wxPaintDC dc(this);
	PrepareDC (dc);

	GetViewStart(&x, &y);

	if (m_owner->fileOpened){
		
		if ( (m_track->trackInfo.channels == 1)  && (m_buffer)){
		
			GetClientSize(&width,&height);
		
			if ( (m_width != width) || (m_height!=height) ){
				m_width = m_track->ini_width;
				m_height = INI_HEIGHT;
			}			
			
			if(m_buffer) delete m_buffer;
			m_buffer = new wxBitmap(m_width, INI_HEIGHT);
			

			m_track->trackArea = m_walkGraphic->setArea(10,10,m_width-10,INI_HEIGHT-10);
			m_bufferDC.SelectObject(*m_buffer);
			
			m_walkGraphic->setDC(&m_bufferDC);

			m_walkGraphic->setGridBackgroundColor(wxColour(192, 192, 192));
			m_walkGraphic->setCurveColor(wxColour(50,  50, 200));

			colour.Set(172, 168, 153); // backGround colour
			
			pen.SetColour(colour);
			brush.SetColour(colour);
			m_bufferDC.SetPen(pen);
			m_bufferDC.SetBrush(brush);
			m_bufferDC.DrawRectangle(0,0,m_width,m_height);
			
			if (selected)
				if (m_track->m_startPoint <= m_track->m_endPoint)
					m_walkGraphic->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->border);
				else
					m_walkGraphic->plotSelect(m_track->m_startPoint, m_track->m_endPoint-1, m_track->border);
			else
				m_walkGraphic->plot();

			//m_bufferDC.SelectObject( wxNullBitmap );
			//wxPaintDC dc(this);
			//dc.DrawBitmap (*m_buffer, 0, 0);
			
			
			m_bufferDC.SelectObject( wxNullBitmap ); // necessary
			wxClientDC screen_dc(this);
			screen_dc.DrawBitmap(*m_buffer,-x*10,0);
			//screen_dc.DrawBitmap(*m_buffer,0,0);


		}

		
		if ( (m_track->trackInfo.channels == 2) && (m_buffer) ) { 
			
			GetClientSize(&width,&height);
			
			if ( (m_width != width) || (m_height!=height) ){
				m_width = m_track->ini_width;
				m_height = INI_HEIGHT*2 + 20;
			}

			if(m_buffer) delete m_buffer;
			m_buffer = new wxBitmap(m_width,INI_HEIGHT*2 + 20);

			m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,INI_HEIGHT-10);
			m_track->trackAreaRight = m_walkGraphicRight->setArea(10,INI_HEIGHT-20,m_width-10,300);  
			
			m_bufferDC.SelectObject(*m_buffer);

			m_walkGraphicLeft->setDC(&m_bufferDC);
			m_walkGraphicRight->setDC(&m_bufferDC);

			m_walkGraphicLeft->setGridBackgroundColor(wxColour(192, 192, 192));
			m_walkGraphicLeft->setCurveColor(wxColour(50,  50, 200));

			m_walkGraphicRight->setGridBackgroundColor(wxColour(192, 192, 192));
			m_walkGraphicRight->setCurveColor(wxColour(50,  50, 200));

			
			// --- clean area ---
			colour.Set(172, 168, 153); // backGround colour
			
			pen.SetColour(colour);
			brush.SetColour(colour);
			m_bufferDC.SetPen(pen);
			m_bufferDC.SetBrush(brush);
			m_bufferDC.DrawRectangle(0,0,m_width,m_height);
			
			
			//if ( (m_track->m_startPoint != 0) & (m_track->m_endPoint !=0) ){
			//	m_track->m_startPoint = int ((float) (m_track->m_startSample*width) / (m_track->trackInfo.frames));
			//	m_track->m_endPoint = int ((float) (m_track->m_endSample*width) / (m_track->trackInfo.frames));
			//	m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint,m_track->trackArea.GetX());
			//	m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint,m_track->trackArea.GetX());
			//}
			//else{
			//	m_walkGraphicLeft->plot();
			//	m_walkGraphicRight->plot();
			//}
			
			//m_walkGraphicLeft->plot();
			//m_walkGraphicRight->plot();
			if (selected)
			{
				if (!showSeparate){
					//m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->border);
					m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->border);
					m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->border);
				}
				else
					if (selectedL){
						m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->border);
						m_walkGraphicRight->plot();	
					}
					if (selectedR){
						m_walkGraphicLeft->plot();
						m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->border);
					}
			}
			else
			{
				m_walkGraphicLeft->plot();
				m_walkGraphicRight->plot();
			}
			m_bufferDC.SelectObject( wxNullBitmap ); // necessary
			wxClientDC screen_dc(this);
			
			screen_dc.DrawBitmap(*m_buffer,-x*10,0);
			//screen_dc.DrawBitmap(*m_buffer,0,0);
			
		}


	}
	
}

/*	
    ------------------MyCanvas::newFile()---------------------------------------------
	Objetivo : Criar um arquivo novo
	Recebe   : void
	Devolve  : void

	Goal       : Create a new file
	Parameters : void
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------------------------------------
*/
void MyCanvas::newFile(int samplerate)
{
	
	m_track->infile = NULL;

	m_track->trackInfo.channels = 1;
	m_track->trackInfo.format = 65538;
	m_track->trackInfo.frames = 1000;
	m_track->trackInfo.samplerate = samplerate;
	m_track->trackInfo.sections = 1;
	m_track->trackInfo.seekable = 1;

	if ((m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames*m_track->trackInfo.channels)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
	}

	for (int i = 0; i < m_track->trackInfo.frames; i++)
		m_track->trackFptr[i] = 0;

	m_walkGraphic = new WalkGraphic( m_track->trackInfo.frames );

	m_track->readcntf = m_track->trackInfo.frames;

	clearArea();
	drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);

	m_track->border = m_walkGraphic->getMaxStringLen();
	
}

/*	
    ------------------MyCanvas::newFileStereo()---------------------------------------
	Objetivo : Criar um arquivo novo
	Recebe   : void
	Devolve  : void

	Goal       : Create a new file
	Parameters : void
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------------------------------------
*/
void MyCanvas::newFileStereo(int samplerate)
{
	
	m_track->infile = NULL;

	m_track->trackInfo.channels = 2;
	m_track->trackInfo.format = 65538;
	m_track->trackInfo.frames = 1000;
	m_track->trackInfo.samplerate = samplerate;
	m_track->trackInfo.sections = 1;
	m_track->trackInfo.seekable = 1;

	if ((m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames*m_track->trackInfo.channels)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
	}

	if ((m_track->trackFptrLeft = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
	}

	if ((m_track->trackFptrRight = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
	}


	for (int i = 0; i < m_track->trackInfo.frames*m_track->trackInfo.channels; i++)
		m_track->trackFptr[i] = 0;

	for (i = 0; i < m_track->trackInfo.frames; i++)
	{
		m_track->trackFptrLeft[i] = 0;
		m_track->trackFptrRight[i] = 0;
	}


	m_walkGraphicLeft = new WalkGraphic( m_track->trackInfo.frames );
	m_walkGraphicRight = new WalkGraphic( m_track->trackInfo.frames );

	m_track->readcntf = m_track->trackInfo.frames;

	clearArea();
	drawOpenedFileStereo(m_track->trackFptrLeft, m_track->trackFptrRight, m_track->trackInfo.frames);
	//drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);

	m_track->border = m_walkGraphicLeft->getMaxStringLen();
	
}



/*	
    ------------------MyCanvas::openFile(const char *filename)-----------------------
	Objetivo : Carregar os valores do arquivo da música na memória e exibi-lo em tela
	Recebe   : Um ponteiro para char (que indica o nome do arquivo)
	Devolve  : void

	Goal       : Load to memory the file samples and show in the screen
	Parameters : Char pointer, file's name
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------------------------------------
*/
void MyCanvas::openFile(const char *filename)
{
	wxString	str;
	int		k = 0;
	int		maxL, 
			maxR;
	wxString 	path = filename;
	static wxString s_extDef;

	s_extDef = path.AfterLast(_T('.'));
		
	//selected = false;
	maxL = maxR = 0;

	//  *** Open the file *** 
	//  SNDFILE pointer and header to m_track
	

        if (strcmp(s_extDef,"mp3") == 0)// eh mp3
        {

			decode(filename, m_track->trackInfo);
		
			if (m_track->trackInfo.channels == 1)
			{
				if ((m_track->infile = sf_open("outstep.wav", SFM_READ, &m_track->trackInfo)) == NULL ){
					wxMessageBox("FILE ERROR\n","File Opening", wxICON_INFORMATION | wxOK );
								sf_close(m_track->infile);
				}

				if ((m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames*m_track->trackInfo.channels)*sizeof(float))) == NULL ){
								wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
								exit(1);
				}

				m_owner->menuBar->Enable(ID_TO_STEREO, true);

				m_walkGraphic = new WalkGraphic( m_track->trackInfo.frames );
				m_track->readcntf = sf_read_float(m_track->infile, m_track->trackFptr, m_track->trackInfo.frames*m_track->trackInfo.channels);
				drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
				m_track->border = m_walkGraphic->getMaxStringLen();
			}

			if (m_track->trackInfo.channels == 2)
			{
				if ((m_track->infile = sf_open("outstep.wav", SFM_READ, &m_track->trackInfo)) == NULL ){
					wxMessageBox("FILE ERROR\n","File Opening", wxICON_INFORMATION | wxOK );
					sf_close(m_track->infile);
				}

				if ((m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames*m_track->trackInfo.channels)*sizeof(float))) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
					exit(1);
				}
				m_walkGraphicLeft  = new WalkGraphic( m_track->trackInfo.frames );
				m_walkGraphicRight = new WalkGraphic( m_track->trackInfo.frames );
				m_track->readcntf  = sf_read_float(m_track->infile, m_track->trackFptr, m_track->trackInfo.frames*m_track->trackInfo.channels);
				if ((m_track->trackFptrLeft = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
				   wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
				   exit(1);
				}
				if ((m_track->trackFptrRight = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
				   wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
				   exit(1);
				}

				// Split left and right
				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptrLeft[k] = m_track->trackFptr[lef];
					k++;                       
				}
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptrRight[k] = m_track->trackFptr[rig];
					k++;
				}

				drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
				maxL = m_walkGraphicLeft->getMaxStringLen();
				maxR = m_walkGraphicRight->getMaxStringLen();

				if (maxL > maxR)
					m_track->border = maxL;
				else
					m_track->border = maxR;
			} // Stereo
		} // mp3



        if (strcmp(s_extDef,"wav") == 0)// eh wav
		{
			if ((m_track->infile = sf_open(filename, SFM_READ, &m_track->trackInfo)) == NULL ){
				wxMessageBox("FILE ERROR\n","File Opening", wxICON_INFORMATION | wxOK );
				sf_close(m_track->infile);
			}

			if ( m_track->trackInfo.channels == 1)     // Mono
				m_owner->menuBar->Enable(ID_TO_STEREO, true);
			else{
				m_owner->menuBar->Enable(ID_TO_MONO, true);
				m_owner->menuBar->Enable(ID_SEPARATE, true);
			}
			//  *** Read the file and paint the screen ***
			//  Samples to m_track, m_walkGraphic receives m_track
			if ( m_track->trackInfo.channels == 1){     // Mono
				if ((m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames*m_track->trackInfo.channels)*sizeof(float))) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
					exit(1);
				}
				//m_walkGraphic = new WalkGraphic( m_track->trackInfo.frames );
			
				m_track->readcntf = sf_read_float(m_track->infile, m_track->trackFptr, m_track->trackInfo.frames*m_track->trackInfo.channels);
				//fprintf(stderr, "m_track->trackFptr: %f, m_track->trackInfo.frames: %d", *m_track->trackFptr, m_track->trackInfo.frames);
				drawOpenedFile(m_track->trackFptr, m_track->trackInfo.frames);
				//m_track->border = m_walkGraphic->getMaxStringLen();
			}
			else{  // Stereo
				if ((m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames*m_track->trackInfo.channels)*sizeof(float))) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
					exit(1);
				}
				m_walkGraphicLeft = new WalkGraphic( m_track->trackInfo.frames );
				m_walkGraphicRight = new WalkGraphic( m_track->trackInfo.frames );

				m_track->readcntf = sf_read_float(m_track->infile, m_track->trackFptr, m_track->trackInfo.frames*m_track->trackInfo.channels);
			
				if ((m_track->trackFptrLeft = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
					exit(1);
				}
				if ((m_track->trackFptrRight = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
					exit(1);
				}
		
				//m_track->m_startPoint = m_track->m_endPoint = 0;

				// Split left and right 
				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptrLeft[k] = m_track->trackFptr[lef];
					k++;
				}
			
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptrRight[k] = m_track->trackFptr[rig];
					k++;
				}

				drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
			
				maxL = m_walkGraphicLeft->getMaxStringLen();
				maxR = m_walkGraphicRight->getMaxStringLen();

				if (maxL > maxR)
					m_track->border = maxL;
				else
					m_track->border = maxR;
			
		} // mono
		sf_close(m_track->infile);
	}  //wave
}


/*
	------------------MyCanvas::saveFile()--------------------------
	Objetivo : Gravar as modificações sofridas pelo arquivo no mesmo 
	Recebe   : void
	Devolve  : void

	Goal       : Save the modifications to the file
	Parameters : void
	Return	   : void
	Should Return : Error Code

	Obs: Atualmente estou gravando o resultado da modificação em um 
		 arquivo chamado result.wav
	-----------------------------------------------------------------
*/
void MyCanvas::saveFile()
{
	sf_count_t	writeCnt;
	SNDFILE		*outfile;
	char		*filename = "result.wav";
	sf_count_t	readcntf;
	SF_INFO		info;


	/* Procedimento de delecao e criacao de um novo arquivo
	if _WINDOWS_
		//wxString str;
		//str.Printf("del %s",filename);
		 Deletar o arquivo, criar outro
	else _LINUX_
		// Deletar o arquivo, criar outro
	else _MAC_
		// Deletar o arquivo, criar outro
	end if
	*/

	
	// *** Opening the file for writing ***
	readcntf = m_track->trackInfo.frames*m_track->trackInfo.channels;
	info = m_track->trackInfo;

	if ((outfile  = sf_open(filename, SFM_WRITE, &info)) == NULL ){
		wxMessageBox("Erro abrindo o arquivo para escrita\n");
		sf_close(outfile);
	}


	writeCnt = sf_write_float(outfile, m_track->trackFptr, readcntf);
	sf_close(outfile);	
}


/*
	------------------MyCanvas::saveFileAs(const char *filename)------------
	Objetivo : Gravar as modificações sofridas pelo arquivo em outro arquivo 
	Recebe   : Um ponteiro para char (que indica o nome do novo arquivo)
	Devolve  : void

	Goal       : Save the modifications to the new file
	Parameters : Char pointer, file's name
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------
*/
void MyCanvas::saveFileAs(const char *filename)
{
	sf_count_t	writeCnt;
	SNDFILE		*outfile;
	sf_count_t	readcntf;
	SF_INFO		info;
	
	readcntf = m_track->trackInfo.frames*m_track->trackInfo.channels;

	//wxString str;
	//str.Printf(" %d \n",readcntf);
	//wxMessageBox(str);
	
	info = m_track->trackInfo;

	if ((outfile  = sf_open(filename, SFM_WRITE, &info)) == NULL ){
		wxMessageBox("Erro abrindo o arquivo para escrita\n");
		sf_close(outfile);
	}
	
	writeCnt = sf_write_float(outfile, m_track->trackFptr, readcntf);

	sf_close(outfile);
	

}


/*
	------------------MyCanvas::exportMp3As(const char *filename)------------
	Objetivo : Gravar as modificações sofridas pelo arquivo em um arquivo mp3
	Recebe   : Um ponteiro para char (que indica o nome do novo arquivo)
	Devolve  : void

	Goal       : Save the mp3 version in another file
	Parameters : Char pointer, file's name
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------
*/
void MyCanvas::exportMp3As(const char *filename)
{
	int ret = encode( m_track->trackFptr, m_track->trackInfo, filename);
}



/*
	------------------MyCanvas::closeFile()-----------------------------------
	Objetivo : Zerar as variáveis utilizadas quando um arquivo é aberto
	Recebe   : void
	Devolve  : void

	Goal       : Reset the variables that are set when a file is opened
	Parameters : void
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------
*/
void MyCanvas::closeFile()
{
	int		channels;

	clearArea();
	
	m_track->infile = NULL;

	m_width,m_height = 0;
	

	channels = m_track->trackInfo.channels;
	m_track->trackInfo.channels = 0;
	m_track->trackInfo.format = 0;
	m_track->trackInfo.frames = 0;
	m_track->trackInfo.samplerate = 0;
	m_track->trackInfo.sections = 0;
	m_track->trackInfo.seekable = 0;

	if (channels == 1){
		if (m_track->trackFptr) free(m_track->trackFptr);
		if (m_walkGraphic) free(m_walkGraphic);
	}
	else{
		if (m_track->trackFptrLeft) free(m_track->trackFptrLeft);
		if (m_track->trackFptrRight) free(m_track->trackFptrRight);

		if (m_walkGraphicLeft) free(m_walkGraphicLeft);
		if (m_walkGraphicRight) free(m_walkGraphicRight);

		
	}
	
	m_track->readcntf = 0;
	m_track->border = 0;
	m_track->m_startPoint = m_track->m_endPoint = -1;
	m_track->m_validPressed = false;
	m_track->ini_width = 0;

}

/*
	------------------drawOpenedFile(float *fptr,int size)---------------------------
	Objetivo : Exibir as amostras do float* em forma de onda na tela  
	Recebe   : Um ponteiro para float (amostras) e um inteiro representando o tamanho
	Devolve  : void

	Goal       : Paint on the screen the samples in wave form
	Parameters : float pointer(samples), int(size)
	Return	   : void
	Should Return : Error Code
	---------------------------------------------------------------------------------
*/
void MyCanvas::drawOpenedFile(float *fptr,int size)
{
	wxString	str;
	
	wxColour	colour;
	wxPen		pen;
	wxBrush		brush;

	int		width,
			height;
	int		xx,
			yy;

	// *** Getting the size of the window 'client area' which may be drawn
	// *** Getting the position at which the visible portion of the window starts.
	GetViewStart(&xx, &yy);
	if ( m_track->ini_width  == 0 ){
		GetClientSize(&width,&height);
		m_width  = width;
		m_track->ini_width  = m_width;
		SetVirtualSize( m_track->ini_width  ,  0 );
	}
	else{
		m_width	= m_track->ini_width;
	}

	m_height = INI_HEIGHT;
	m_track->ini_height = m_height;
	
	// *** Setting the WalkGraphic object, the wxBitmap and the Device Context
	if (m_walkGraphic) free(m_walkGraphic);
	
	m_walkGraphic = new WalkGraphic( size );

	
	if(m_buffer) delete m_buffer;
	m_buffer = new wxBitmap(m_width,m_height);
	
	m_track->trackArea = m_walkGraphic->setArea(10,10,m_width-10,m_height-10);
	
	m_bufferDC.SelectObject(*m_buffer);
	m_bufferDC.Clear();
	m_walkGraphic->setDC(&m_bufferDC);
	

	m_walkGraphic->setGridBackgroundColor(wxColour(192, 192, 192));
	m_walkGraphic->setCurveColor(wxColour(50,  50, 200));
	
	// --- clean area ---
	//colour.Set(0xA0,0xA0,0xA0); // backGround colour
	colour.Set(172, 168, 153); // backGround colour

	pen.SetColour(colour);
	brush.SetColour(colour);
	m_bufferDC.SetPen(pen);
	m_bufferDC.SetBrush(brush);
	m_bufferDC.DrawRectangle(0,0,m_width,m_height);


	// *** Adding samples to WalkGraphic and drawing the audio wave
	for (int index = 0; index < size; index++){
		m_walkGraphic->newPoint(fptr[index],str, false);
	}
		//m_walkGraphic->newPoint(m_track->trackFptr[index],str,( (index*m_track->trackInfo.frames) % 10000)==0 ? true : false);
	m_walkGraphic->plot();
	
	// *** Showing on the Device Context (screen)
	m_bufferDC.SelectObject( wxNullBitmap ); // necessary
	wxClientDC screen_dc(this);
	screen_dc.DrawBitmap(*m_buffer,-xx*10,0);
}


/*
	------------------drawOpenedFileSelect(float *fptr,int size, int start, int end)---------
	Objetivo : Exibir a forma de onda na tela  incluindo o trecho marcado pelo usuário
	Recebe   : Um ponteiro para float (amostras), um inteiro representando o tamanho
			   um inteiro representando o ponto de início da marcação e um representando 
			   o fim
	Devolve  : void

	Goal       : Paint on the screen the samples in wave form including the selected slice 
	Parameters : float pointer(samples), int(size), int (start selection), int (end selection)
	Return	   : void
	Should Return : Error Code

	Obs: Verificar se ainda utilizo esta função
	-----------------------------------------------------------------------------------------
*/
void MyCanvas::drawOpenedFileSelect(float *fptr,int size, int start, int end)
{
	wxString   str;
	int 	   width,
		   height;
	wxColour   colour;
	wxPen	   pen;
	wxBrush    brush;

	GetClientSize(&width,&height);

	if ( m_track->ini_width  == 0 ){
		GetClientSize(&width,&height);
		m_width  = width;
		m_track->ini_width  = m_width;
	}
	else
	m_width	= m_track->ini_width;	

	m_height = INI_HEIGHT;

	if(m_buffer) delete m_buffer;
	m_buffer = new wxBitmap(m_width,m_height);
	
	m_track->trackArea = m_walkGraphic->setArea(10,10,m_width-10,m_height-10);
	
	m_bufferDC.SelectObject(*m_buffer);
	m_bufferDC.Clear();
	m_walkGraphic->setDC(&m_bufferDC);
	
	m_walkGraphic->setGridBackgroundColor(wxColour(192, 192, 192));
	m_walkGraphic->setCurveColor(wxColour(50,  50, 200));

	// --- clean area ---
	//colour.Set(148, 148, 170); // backGround colour
	colour.Set(172, 168, 153); // backGround colour
	pen.SetColour(colour);
	brush.SetColour(colour);
	m_bufferDC.SetPen(pen);
	m_bufferDC.SetBrush(brush);
	m_bufferDC.DrawRectangle(0,0,m_width,m_height);

	for (int index = 0; index < size; index++) 
		m_walkGraphic->newPoint(fptr[index],str, false);
	

	m_walkGraphic->plotSelect(start, end, m_track->trackArea.GetX());
        /*
	if (start < end)
		m_walkGraphic->plotSelect(start, end, m_track->trackArea.GetX());
	else
		m_walkGraphic->plotSelect(start, end+1, m_track->trackArea.GetX());
	*/
	//m_walkGraphic->plotSelect(start+1, end-1, m_track->trackArea.GetX());
	
	m_bufferDC.SelectObject( wxNullBitmap ); // necessary
	wxClientDC screen_dc(this);
	screen_dc.DrawBitmap(*m_buffer,0,0);
	//screen_dc.DrawBitmap(*m_buffer,0,0);
	
}


/*
	------------------drawOpenedFileStereo(float *fptrLeft,float *fptrRight,int size)-------------
	Objetivo : Exibir as amostras que caracterizam o arquivo estéreo em tela, para isso utilizando
			   dois retângulos
	Recebe   : Dois ponteiro para float (amostras) e um inteiro representando o tamanho
	Devolve  : void

	Goal       : Paint on the screen the stereo samples in wave form
	Parameters : two float pointers(samples), int(size)
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------
*/
void MyCanvas::drawOpenedFileStereo(float *fptrLeft,float *fptrRight,int size)
{
	wxString	str;
	int		width,
			height;
	wxColour	colour;
	wxPen		pen;
	wxBrush		brush;
	int		xx,
			yy;


	// *** Getting the size of the window 'client area' which may be drawn
	// *** Getting the position at which the visible portion of the window starts.
	GetViewStart(&xx, &yy);
	if ( m_track->ini_width  == 0 ){
		GetClientSize(&width,&height);
		m_width  = width;
		m_track->ini_width  = m_width;
		SetVirtualSize( m_track->ini_width  ,  0 );
	}
	else
		m_width	= m_track->ini_width;	

	m_height = 2*INI_HEIGHT +20;
	m_track->ini_height = m_height;
	
	
	// *** Setting the WalkGraphic object, the wxBitmap and the Device Context
	if (m_walkGraphicLeft) free(m_walkGraphicLeft);
	m_walkGraphicLeft = new WalkGraphic( size );
	
	if (m_walkGraphicRight) free(m_walkGraphicRight);
	m_walkGraphicRight = new WalkGraphic( size );


	if(m_buffer) delete m_buffer;
	m_buffer = new wxBitmap(m_width,m_height);

	m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,INI_HEIGHT-10);
	m_track->trackAreaRight = m_walkGraphicRight->setArea(10,INI_HEIGHT-20,m_width-10,300);  
	
	m_bufferDC.SelectObject(*m_buffer);
	m_bufferDC.Clear();
	
	m_walkGraphicLeft->setDC(&m_bufferDC);
	m_walkGraphicRight->setDC(&m_bufferDC);
	
	m_walkGraphicLeft->setGridBackgroundColor(wxColour(192, 192, 192));
	m_walkGraphicLeft->setCurveColor(wxColour(50,  50, 200));

	m_walkGraphicRight->setGridBackgroundColor(wxColour(192, 192, 192));
	m_walkGraphicRight->setCurveColor(wxColour(50,  50, 200));
	
	// --- clean area ---
	//colour.Set(0xA0,0xA0,0xA0); // backGround colour
	colour.Set(172, 168, 153); // backGround colour
	pen.SetColour(colour);
	brush.SetColour(colour);

	m_bufferDC.SetBrush(brush);
	m_bufferDC.SetPen(pen);
	
	m_bufferDC.DrawRectangle(0,0,m_width,m_height);

	// *** Adding samples to WalkGraphic and drawing the audio wave
	for (int index = 0; index < size; index++){
		str.Printf("%.2f",(float) index/m_track->trackInfo.samplerate);
		m_walkGraphicLeft->newPoint(m_track->trackFptrLeft[index],str, false);
		m_walkGraphicRight->newPoint(m_track->trackFptrRight[index],str, false);
	}
	
	m_walkGraphicLeft->plot();
	m_walkGraphicRight->plot();


	// *** Showing on the Device Context (screen)
	m_bufferDC.SelectObject( wxNullBitmap ); // necessary
	wxClientDC screen_dc(this);
	screen_dc.DrawBitmap(*m_buffer,-xx*10,0);
}


/*
	------------------drawOpenedFileSelectStereo(float *fptr,int size, int start, int end)---
	Objetivo : Exibir a forma de onda na tela  incluindo o trecho marcado pelo usuário
	Recebe   : Um ponteiro para float (amostras), um inteiro representando o tamanho
			   um inteiro representando o ponto de início da marcação e um representando 
			   o fim
	Devolve  : void

	Goal       : Paint on the screen the samples in wave form including the selected slice 
	Parameters : float pointer(samples), int(size), int (start selection), int (end selection)
	Return	   : void
	Should Return : Error Code

	Obs: Verificar se ainda utilizo esta função
	-----------------------------------------------------------------------------------------
*/
void MyCanvas::drawOpenedFileSelectStereo(float *fptrLeft,float *fptrRight,int size, int start, int end)
{

	wxString	str;
	int		width,
			height;
	wxColour	colour;
	wxPen		pen;
	wxBrush		brush;
	int		x,
			y;


	GetClientSize(&width,&height);
	GetViewStart(&x, &y);
	/*		
	if ( (m_width != width) || (m_height!=height) ){
		m_width = m_track->ini_width;
		m_height = INI_HEIGHT*2 + 20;
	}*/

	if ( m_track->ini_width  == 0 ){
		GetClientSize(&width,&height);
		m_width  = width;
		m_track->ini_width  = m_width;
		SetVirtualSize( m_track->ini_width  ,  0 );
	}
	else
		m_width	= m_track->ini_width;	

	m_height = 2*INI_HEIGHT +20;
	m_track->ini_height = m_height;
	

	if(m_buffer) delete m_buffer;
	m_buffer = new wxBitmap(m_width,INI_HEIGHT*2 + 20);

	m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,INI_HEIGHT-10);
	m_track->trackAreaRight = m_walkGraphicRight->setArea(10,INI_HEIGHT-20,m_width-10,300);  
	
	m_bufferDC.SelectObject(*m_buffer);

	m_walkGraphicLeft->setDC(&m_bufferDC);
	m_walkGraphicRight->setDC(&m_bufferDC);

	m_walkGraphicLeft->setGridBackgroundColor(wxColour(192, 192, 192));
	m_walkGraphicLeft->setCurveColor(wxColour(50,  50, 200));

	m_walkGraphicRight->setGridBackgroundColor(wxColour(192, 192, 192));
	m_walkGraphicRight->setCurveColor(wxColour(50,  50, 200));

	
	// --- clean area ---
	colour.Set(172, 168, 153); // backGround colour
	
	pen.SetColour(colour);
	brush.SetColour(colour);
	m_bufferDC.SetPen(pen);
	m_bufferDC.SetBrush(brush);
	m_bufferDC.DrawRectangle(0,0,m_width,m_height);
	
	for (int index = 0; index < size; index++){
		str.Printf("%.2f",(float) index/m_track->trackInfo.samplerate);
		m_walkGraphicLeft->newPoint(m_track->trackFptrLeft[index],str, false);
		m_walkGraphicRight->newPoint(m_track->trackFptrRight[index],str, false);
	}

	if (!showSeparate){
		//m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1, m_track->trackAreaLeft.GetX());
		//m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1, m_track->trackAreaLeft.GetX());
		m_walkGraphicLeft->plotSelect(m_track->m_startPoint , m_track->m_endPoint, m_track->trackAreaLeft.GetX());
		m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->trackAreaLeft.GetX());
	}
	else{
		if (selectedL){
			
			m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->trackAreaLeft.GetX());
			m_walkGraphicRight->plot();	
		}
		if (selectedR){
			m_walkGraphicLeft->plot();
			m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->trackAreaRight.GetX());
		}
	}
	
	m_bufferDC.SelectObject( wxNullBitmap ); // necessary
	wxClientDC screen_dc(this);
	screen_dc.DrawBitmap(*m_buffer,-x*10,0);
	//screen_dc.DrawBitmap(*m_buffer,0,0);
}

/*	
    ------------------MyCanvas::OnMouseButtonDown(wxMouseEvent &event)-----------------------
	Objetivo : Salvar a posição na qual o mouse estiver quando o botão direito for 
			   apertado
	Recebe   : wxPaintEvent
	Devolve  : void

	Goal       : Save the position where the mouse was pressed
	Parameters : wxPaintEvent
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------------------------------------
*/
void MyCanvas::OnMouseButtonDown(wxMouseEvent &event)
{
	wxString str;
    wxClientDC dc(this);
    PrepareDC(dc);
    m_owner->PrepareDC(dc);
	wxColour colour;
	wxPen pen;
	wxBrush brush;
	int width, height;

	int grid_x1, grid_x2, grid_y1, grid_y2;
	int	xx,yy;
	//int sample;

	if ( m_owner->fileOpened ){

		GetClientSize(&width,&height);
		wxPoint pos = event.GetPosition();
		long x = dc.DeviceToLogicalX( pos.x );
		long y = dc.DeviceToLogicalY( pos.y );
	
			
		

		if ( (m_track->trackInfo.channels == 1) && ( m_track->trackArea.Inside(x,y)) ){
			
			SetCursor(*mSelectCursor);	
			drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
			m_walkGraphic->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);
			
			GetViewStart(&xx, &yy);

			// Desenhando a linha vertical 
			m_bufferDC.SelectObject(*m_buffer);
			colour.Set(148,  148, 170);
			
			pen.SetColour(colour);
			brush.SetColour(colour);
			m_bufferDC.SetPen(pen);
			m_bufferDC.SetBrush(brush);
			m_bufferDC.DrawLine((int)x, grid_y1 ,(int)x, grid_y2);
			
			m_bufferDC.SelectObject( wxNullBitmap ); // necessary
			wxClientDC screen_dc(this);

			GetViewStart(&xx, &yy);
			screen_dc.DrawBitmap(*m_buffer,-xx*10,0);


			m_track->m_buttonPressed = true;
			m_track->m_startPoint = int(x) - m_track->trackArea.GetX();

				
			m_track->m_startSample = ( m_track->m_startPoint )*(m_track->trackInfo.frames) / m_track->trackArea.GetWidth();
			m_track->m_startSample = (int) m_track->m_startSample;

		}	
		
		if (m_track->trackInfo.channels == 2)
		{
			if (!showSeparate){
				if (( m_track->trackAreaLeft.Inside(x,y)) || ( m_track->trackAreaRight.Inside(x,y))){

					SetCursor(*mSelectCursor);	
					drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
					
					// Desenhando a linha vertical 
					m_walkGraphicLeft->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);
					m_bufferDC.SelectObject(*m_buffer);
					colour.Set(148, 148, 170);

					pen.SetColour(colour);
					brush.SetColour(colour);
					m_bufferDC.SetPen(pen);
					m_bufferDC.SetBrush(brush);
					m_bufferDC.DrawLine((int)x, grid_y1+1 ,(int)x, grid_y2);

					m_walkGraphicRight->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);
					
					m_bufferDC.SelectObject(*m_buffer);
					colour.Set(172, 168, 153); // backGround colour
					brush.SetColour(colour);
					colour.Set(148, 148, 170);
					pen.SetColour(colour);
					m_bufferDC.SetPen(pen);
					m_bufferDC.SetBrush(brush);
					m_bufferDC.DrawLine((int)x, grid_y1+1 ,(int)x, grid_y2);
					
					m_bufferDC.SelectObject( wxNullBitmap ); // necessary
					wxClientDC screen_dc(this);

					GetViewStart(&xx, &yy);
					screen_dc.DrawBitmap(*m_buffer,-xx*10,0);

					m_walkGraphicLeft->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);

					m_track->m_buttonPressed = true;
					m_track->m_startPoint = int(x) - m_track->trackAreaLeft.GetX();

					m_track->m_startSample = ( m_track->m_startPoint )*(m_track->trackInfo.frames) / m_track->trackAreaLeft.GetWidth();
					m_track->m_startSample = (int) m_track->m_startSample;	
				}

			}
			else{
				if (m_track->trackAreaLeft.Inside(x,y)){  //Left
					
					
					SetCursor(*mSelectCursor);	
					drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
					
					// Desenhando a linha vertical 
					m_walkGraphicLeft->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);
					m_bufferDC.SelectObject(*m_buffer);
					colour.Set(148, 148, 170);
					pen.SetColour(colour);
					brush.SetColour(colour);
					m_bufferDC.SetPen(pen);
					m_bufferDC.SetBrush(brush);
					m_bufferDC.DrawLine((int)x, grid_y1+1 ,(int)x, grid_y2);
					m_walkGraphicRight->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);
					
					m_bufferDC.SelectObject( wxNullBitmap ); // necessary
					wxClientDC screen_dc(this);

					GetViewStart(&xx, &yy);
					screen_dc.DrawBitmap(*m_buffer,-xx*10,0);

					m_walkGraphicLeft->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);

					m_track->m_buttonPressed = true;
					m_track->m_startPoint = int(x) - m_track->trackAreaLeft.GetX();

					m_track->m_startSample = ( m_track->m_startPoint )*(m_track->trackInfo.frames) / m_track->trackAreaLeft.GetWidth();
					m_track->m_startSample = (int) m_track->m_startSample;	
				}

				if (m_track->trackAreaRight.Inside(x,y)){  //Right
				
					SetCursor(*mSelectCursor);	
					drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
					
					m_walkGraphicRight->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);
					
					m_bufferDC.SelectObject(*m_buffer);
					colour.Set(172, 168, 153); // backGround colour
					brush.SetColour(colour);
					colour.Set(148, 148, 170);
					pen.SetColour(colour);
					m_bufferDC.SetPen(pen);
					m_bufferDC.SetBrush(brush);
					m_bufferDC.DrawLine((int)x, grid_y1+1 ,(int)x, grid_y2);
					
					m_bufferDC.SelectObject( wxNullBitmap ); // necessary
					wxClientDC screen_dc(this);

					GetViewStart(&xx, &yy);
					screen_dc.DrawBitmap(*m_buffer,-xx*10,0);

					m_walkGraphicLeft->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);

					m_track->m_buttonPressed = true;
					m_track->m_startPoint = int(x) - m_track->trackAreaLeft.GetX();

					m_track->m_startSample = ( m_track->m_startPoint )*(m_track->trackInfo.frames) / m_track->trackAreaLeft.GetWidth();
					m_track->m_startSample = (int) m_track->m_startSample;	
				}
			}
		}

		// So se estiver dentro da area
		//selected = false;

		if (m_track->isInside(x,y)){
			m_track->m_validPressed = true;
			//m_owner->SetStatusText( "", 1 );
		}
		else
			m_track->m_validPressed = false;
		

		
		

	}
	
}

/*	
    ------------------void MyCanvas::OnMouseButtonUp(wxMouseEvent &event)---------
	Objetivo : Salvar a posição na qual o mouse estiver quando o botão direito for 
			   solto
	Recebe   : wxPaintEvent
	Devolve  : void

	Goal       : Save the position where the mouse was released
	Parameters : wxPaintEvent
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------------
*/
void MyCanvas::OnMouseButtonUp(wxMouseEvent &event)
{
	wxString str;
	int		 width, height;

	wxClientDC dc(this);
    PrepareDC(dc);
    m_owner->PrepareDC(dc);
	

	m_track->m_buttonPressed = false;

	wxPoint pos = event.GetPosition();
    long x = dc.DeviceToLogicalX( pos.x );
	long y = dc.DeviceToLogicalY( pos.y );

	
	if ( m_owner->fileOpened){
		if (m_track->trackInfo.channels == 1){
		
			m_track->m_endPoint = int(x) - m_track->trackArea.GetX();

			//if (m_track->m_endPoint > m_track->trackArea.GetWidth())  m_track->m_endPoint = m_track->trackArea.GetWidth();
			//if (int(x) < m_track->trackArea.GetX() ) m_track->m_endPoint = 2;

			GetClientSize(&width,&height);
			m_track->m_endSample   = (float) (m_track->m_endPoint*m_track->trackInfo.frames)/(m_track->trackArea.GetWidth());
			m_track->m_endSample = (int) m_track->m_endSample;
		}

		if (m_track->trackInfo.channels == 2){
			m_track->m_endPoint = int(x) - m_track->trackAreaLeft.GetX() ;
			//if (m_track->m_endPoint > m_track->trackAreaLeft.GetWidth()) m_track->m_endPoint = m_track->trackAreaLeft.GetWidth();
			//if (int(x) < m_track->trackAreaLeft.GetX() ) m_track->m_endPoint = 2;
			GetClientSize(&width,&height);
			m_track->m_endSample   = (float) (m_track->m_endPoint*m_track->trackInfo.frames)/(m_track->trackAreaLeft.GetWidth());
			m_track->m_endSample = (int) m_track->m_endSample;

			if ( (m_track->m_validPressed) && (showSeparate) ){
				if (m_track->trackAreaLeft.Inside(x,y)){
					selectedL = true;
					selectedR = false;

				}

				if (m_track->trackAreaRight.Inside(x,y)){
					selectedR = true;
					selectedL = false;
				}


			}

			
		}
		
		// If it is a valid choice
		if (m_track->m_validPressed){

			m_owner->menuBar->Enable(ID_NULL, true);
			m_owner->menuBar->Enable(ID_R_NULL, true);

			m_owner->menuBar->Enable(ID_COPY, true);
			m_owner->menuBar->Enable(ID_CUT, true);
			m_owner->menuBar->Enable(ID_PASTE, true);

			m_owner->menuBar->Enable(ID_EXPAND, true);

			m_owner->menuBar->Enable(ID_GAIN, true);
			m_owner->menuBar->Enable(ID_FADE_IN, true);
			m_owner->menuBar->Enable(ID_FADE_OUT, true);

			m_owner->menuBar->Enable(ID_CROSS_OVER, true);
			m_owner->toolBar->EnableTool(ID_EXPAND,true);
			m_owner->toolBar->EnableTool(ID_PLAY_SEL,true);


			m_owner->toolBar->EnableTool(ID_COPY, true);
			m_owner->toolBar->EnableTool(ID_CUT, true);

			selected = true;
		}

		// Sera q eh isso mesmo ?!?!
		if (m_track->m_startPoint == m_track->m_endPoint )
		{
			//m_track->m_startPoint = m_track->m_endPoint = m_track->m_startSample = m_track->m_endSample = -1;
			m_owner->menuBar->Enable(ID_NULL, false);
			m_owner->menuBar->Enable(ID_R_NULL, false);
			m_owner->menuBar->Enable(ID_COPY, false);
			m_owner->menuBar->Enable(ID_CUT, false);
			//m_owner->menuBar->Enable(ID_PASTE, false);
			m_owner->menuBar->Enable(ID_EXPAND, false);
			m_owner->menuBar->Enable(ID_GAIN, false);
			m_owner->menuBar->Enable(ID_FADE_IN, false);
			m_owner->menuBar->Enable(ID_FADE_OUT, false);
			m_owner->menuBar->Enable(ID_CROSS_OVER, false);

			m_owner->toolBar->EnableTool(ID_COPY,false);
			m_owner->toolBar->EnableTool(ID_CUT,false);
			m_owner->toolBar->EnableTool(ID_EXPAND,false);
			m_owner->toolBar->EnableTool(ID_PLAY_SEL,false);

			selected = false;
		}
			
		
	}	// fileOpened
}


/*	
    ------------------void MyCanvas::OnMouseMove(wxMouseEvent &event)----------
	Objetivo : Desenhar a seleção do trecho de arquivo	
			   De acordo com as variáveis selecionadas pode exibir o tempo 
			   correspondente ao ponto por onde o mouse passa ou exibir o valor 
			   dos trechos onde ocorrem a marcação
	Recebe   : wxPaintEvent
	Devolve  : void

	Goal       : Draw the selected portion of the wave
				 According to the enabled variables may show the time where the 
				 mouse is passing by, or the selected points
	Parameters : wxPaintEvent
	Return	   : void
	Should Return : Error Code
	---------------------------------------------------------------------------
*/
void MyCanvas::OnMouseMove(wxMouseEvent &event)
{
	wxString	str;
	wxColour	colour;
	wxPen		pen;
	wxBrush		brush;
	int			xx,yy;

    wxClientDC dc(this);
    PrepareDC(dc);
    m_owner->PrepareDC(dc);


	GetViewStart(&xx, &yy);

	wxPoint pos = event.GetPosition();
    long x = dc.DeviceToLogicalX( pos.x );
    long y = dc.DeviceToLogicalY( pos.y );
	
	/*
	if (m_track->trackInfo.channels == 1)
		m_track->m_endPoint = int(x) - m_track->trackArea.GetX();
	else
		m_track->m_endPoint = int(x) - m_track->trackAreaLeft.GetX();
	*/
	//str.Printf("Start: %d   End: %d", m_track->m_startPoint, m_track->m_endPoint);
	//m_owner->SetStatusText( str, 2 );
	
	//if  (( m_owner->fileOpened ) && (m_track->m_buttonPressed) ) {

	if( m_owner->fileOpened ){
		

		if ((m_track->trackInfo.channels == 1) && (m_track->m_buttonPressed) )
			m_track->m_endPoint = int(x) - m_track->trackArea.GetX();
			/*
			if (m_track->m_startPoint < m_track->m_endPoint)
				m_track->m_endPoint = int(x) - m_track->trackArea.GetX();
			else
				m_track->m_endPoint = int(x) - m_track->trackArea.GetX();
			*/
				
				
		if ((m_track->trackInfo.channels == 2) && (m_track->m_buttonPressed))
			m_track->m_endPoint = int(x) - m_track->trackAreaLeft.GetX();
			/*
			if (m_track->m_startPoint < m_track->m_endPoint)
				m_track->m_endPoint = int(x) - m_track->trackAreaLeft.GetX();
				//m_track->m_endPoint = int(x) - m_track->trackAreaLeft.GetX()-1;
			else
				m_track->m_endPoint = int(x) - m_track->trackAreaLeft.GetX();
			*/

		if	(( m_track->trackInfo.channels == 1 ) && (m_track->trackArea.Inside(x,y))){
			
			SetCursor(*mSelectCursor);	
			if ( (m_track->m_startPoint != -1) && (m_track->m_buttonPressed) ){
				
				m_bufferDC.SelectObject(*m_buffer);
				//colour.Set(0xA0,0xA0,0xA0); // backGround colour
				colour.Set(172, 168, 153); // backGround colour
				
				pen.SetColour(colour);
				brush.SetColour(colour);
				m_bufferDC.SetPen(pen);
				m_bufferDC.SetBrush(brush);
				m_bufferDC.DrawRectangle(0,0,m_width,m_height);

				m_walkGraphic->setDC(&m_bufferDC);
				m_walkGraphic->setArea(10,10,m_width-10,m_height-10);

				//m_walkGraphic->plotSelect(m_track->m_startPoint, m_track->m_endPoint - m_track->m_startPoint -m_track->border ,m_track->trackArea.GetX());
				m_walkGraphic->plotSelect(m_track->m_startPoint, m_track->m_endPoint, m_track->trackArea.GetX());
				
				m_bufferDC.SelectObject( wxNullBitmap ); // necessary
				wxClientDC screen_dc(this);
				
				screen_dc.DrawBitmap(*m_buffer,-xx*10,0);
					
			}
			else
				SetCursor(wxNullCursor);
		}
		else
			SetCursor(wxNullCursor);


		if ( m_track->trackInfo.channels == 2){ 
			if(!showSeparate){
				if ( (m_track->trackAreaLeft.Inside(x,y)) || (m_track->trackAreaRight.Inside(x,y)) ){

					SetCursor(*mSelectCursor);
					if ( (m_track->m_startPoint != -1) && (m_track->m_buttonPressed) ){
						m_bufferDC.SelectObject(*m_buffer);
	
						colour.Set(172, 168, 153); // backGround colour

						pen.SetColour(colour);
						brush.SetColour(colour);
						m_bufferDC.SetPen(pen);
						m_bufferDC.SetBrush(brush);
						m_bufferDC.DrawRectangle(0,0,m_width,m_height);

						m_walkGraphicLeft->setDC(&m_bufferDC);
						m_walkGraphicRight->setDC(&m_bufferDC);
						
						//m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,m_height-10);
						//m_track->trackAreaRight = m_walkGraphicRight->setArea(10,m_height+10,m_width-10,m_height-10);  

						m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,INI_HEIGHT-10);
						m_track->trackAreaRight = m_walkGraphicRight->setArea(10,INI_HEIGHT-20,m_width-10,300);  

						
						//m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaLeft.GetX());
						//m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaRight.GetX());

						m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint,m_track->trackAreaLeft.GetX());
						m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint,m_track->trackAreaRight.GetX());

						m_bufferDC.SelectObject( wxNullBitmap ); // necessary
						wxClientDC screen_dc(this);
						screen_dc.DrawBitmap(*m_buffer,-xx*10,0);
					}
				}
				else
					SetCursor(wxNullCursor);
			}
			else{
				if ( m_track->trackAreaLeft.Inside(x,y) ){
					
					SetCursor(*mSelectCursor);
					if ( (m_track->m_startPoint != -1) && (m_track->m_buttonPressed) ){
						m_bufferDC.SelectObject(*m_buffer);
	
						colour.Set(172, 168, 153); // backGround colour

						pen.SetColour(colour);
						brush.SetColour(colour);
						m_bufferDC.SetPen(pen);
						m_bufferDC.SetBrush(brush);
						m_bufferDC.DrawRectangle(0,0,m_width,m_height);

						m_walkGraphicLeft->setDC(&m_bufferDC);
						m_walkGraphicRight->setDC(&m_bufferDC);
						
						//m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,m_height-10);
						//m_track->trackAreaRight = m_walkGraphicRight->setArea(10,m_height+10,m_width-10,m_height-10);  

						m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,INI_HEIGHT-10);
						m_track->trackAreaRight = m_walkGraphicRight->setArea(10,INI_HEIGHT-20,m_width-10,300);  

						
						m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint,m_track->trackAreaLeft.GetX());
						//m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaLeft.GetX());
						//m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaRight.GetX());
						m_walkGraphicRight->plot();

						m_bufferDC.SelectObject( wxNullBitmap ); // necessary
						wxClientDC screen_dc(this);
						screen_dc.DrawBitmap(*m_buffer,-xx*10,0);
					}

				}

				if (m_track->trackAreaRight.Inside(x,y)){
							SetCursor(*mSelectCursor);
					if ( (m_track->m_startPoint != -1) && (m_track->m_buttonPressed) ){
						m_bufferDC.SelectObject(*m_buffer);
	
						colour.Set(172, 168, 153); // backGround colour

						pen.SetColour(colour);
						brush.SetColour(colour);
						m_bufferDC.SetPen(pen);
						m_bufferDC.SetBrush(brush);
						m_bufferDC.DrawRectangle(0,0,m_width,m_height);

						m_walkGraphicLeft->setDC(&m_bufferDC);
						m_walkGraphicRight->setDC(&m_bufferDC);
						
						//m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,m_height-10);
						//m_track->trackAreaRight = m_walkGraphicRight->setArea(10,m_height+10,m_width-10,m_height-10);  

						m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,INI_HEIGHT-10);
						m_track->trackAreaRight = m_walkGraphicRight->setArea(10,INI_HEIGHT-20,m_width-10,300);  

						
						//m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaLeft.GetX());
						m_walkGraphicLeft->plot();
						m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint,m_track->trackAreaRight.GetX());
						//m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaRight.GetX());
						//m_walkGraphicRight->plot();

						m_bufferDC.SelectObject( wxNullBitmap ); // necessary
						wxClientDC screen_dc(this);
						screen_dc.DrawBitmap(*m_buffer,-xx*10,0);
					}
				}
			}
		}
		/*
		if ( m_track->trackInfo.channels == 2){ 
			if ( (m_track->trackAreaLeft.Inside(x,y)) || (m_track->trackAreaRight.Inside(x,y)) ){

				SetCursor(*mSelectCursor);
				if ( (m_track->m_startPoint != -1) && (m_track->m_buttonPressed) ){

					m_bufferDC.SelectObject(*m_buffer);

					//colour.Set(0xA0,0xA0,0xA0); // backGround colour
					colour.Set(172, 168, 153); // backGround colour

					pen.SetColour(colour);
					brush.SetColour(colour);
					m_bufferDC.SetPen(pen);
					m_bufferDC.SetBrush(brush);
					m_bufferDC.DrawRectangle(0,0,m_width,m_height);

					m_walkGraphicLeft->setDC(&m_bufferDC);
					m_walkGraphicRight->setDC(&m_bufferDC);
					
					//m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,m_height-10);
					//m_track->trackAreaRight = m_walkGraphicRight->setArea(10,m_height+10,m_width-10,m_height-10);  

					m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,INI_HEIGHT-10);
					m_track->trackAreaRight = m_walkGraphicRight->setArea(10,INI_HEIGHT-20,m_width-10,300);  

					
					m_walkGraphicLeft->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaLeft.GetX());
					m_walkGraphicRight->plotSelect(m_track->m_startPoint, m_track->m_endPoint+1,m_track->trackAreaRight.GetX());

					m_bufferDC.SelectObject( wxNullBitmap ); // necessary
					wxClientDC screen_dc(this);
					screen_dc.DrawBitmap(*m_buffer,-xx*10,0);
				}
			}
			else
				SetCursor(wxNullCursor);
		}
		*/

		if ( (m_owner->fileOpened) && (m_track->isInside(x,y) )){
			wxString	str;
			int			sample = -1;

			if (m_track->trackInfo.channels==1){
						
				sample = ( int(x) - m_track->trackArea.GetX() )*(m_track->trackInfo.frames) / m_track->trackArea.GetWidth();
				str.Printf("Amostra: %d   Ponto: %d\n",sample,int(x) - m_track->trackArea.GetX());
				m_owner->SetStatusText( str, 0 );
				

				//str.Printf("Start: %d   End: %d", (int) m_track->m_startSample, (int) m_track->m_endSample);
				if (showAms){ 
					if (showTemp){ // Se amostra e tempo
						float	start,end;
						start = end = 0;
						
						start = m_track->m_startSample/m_track->trackInfo.samplerate;
						end   = m_track->m_endSample/m_track->trackInfo.samplerate;
						
						str.Printf("Start: %d   End: %d  Start: %f  End: %f\n" , (int) m_track->toSample(m_track->m_startPoint), (int) m_track->toSample(m_track->m_endPoint), start, end);
						m_owner->SetStatusText( str, 1 );
					}
					else{	// Se amostra e nao tempo
						str.Printf("Start: %d   End: %d\n", (int) m_track->toSample(m_track->m_startPoint), (int) m_track->toSample(m_track->m_endPoint));
						m_owner->SetStatusText( str, 1 );
					}
				}
				else{
					if (showTemp){ // Se nao amostra e tempo
						float	start,end;
						start = end = 0;
						
						start = m_track->m_startSample/m_track->trackInfo.samplerate;
						//end   = m_track->m_endSample/m_track->trackInfo.samplerate;
						end   = m_track->toSample(int(x)) /m_track->trackInfo.samplerate;
						str.Printf("Start: %f   End: %f\n", start, end);

						m_owner->SetStatusText( str, 1 );
					}
					else{   // Se nao amostra nem tempo (Default amostra)
						str.Printf("Start: %d   End: %d\n", m_track->m_startPoint, m_track->m_endPoint);
						m_owner->SetStatusText( str , 1 );
						//m_owner->SetStatusText( "", 1 );
					}
				}


				sample = ( int(x) - m_track->trackArea.GetX() )*(m_track->trackInfo.frames) / m_track->trackArea.GetWidth();
				str.Printf("Tempo: %f\n",(float) sample/m_track->trackInfo.samplerate);
				m_owner->SetStatusText( str, 2 );
			}


			if (m_track->trackInfo.channels==2){
						
				if (m_track->trackAreaLeft.Inside(x,y) ){
					sample = ( int(x) - m_track->trackAreaLeft.GetX() )*(m_track->trackInfo.frames) / m_track->trackAreaLeft.GetWidth();
					str.Printf("Amostra: %d   Ponto: %d\n",sample,int(x) - m_track->trackAreaLeft.GetX());
					m_owner->SetStatusText( str, 0 );

					str.Printf("Start: %d   End: %d", m_track->m_startPoint, m_track->m_endPoint);
					m_owner->SetStatusText( str, 1 );

					sample = ( int(x) - m_track->trackAreaLeft.GetX() )*(m_track->trackInfo.frames) / m_track->trackAreaLeft.GetWidth();
					str.Printf("Tempo: %f\n",(float) sample/m_track->trackInfo.samplerate);
					m_owner->SetStatusText( str, 2 );
				}

				if (m_track->trackAreaRight.Inside(x,y) ){
					sample = ( int(x) - m_track->trackAreaRight.GetX() )*(m_track->trackInfo.frames) / m_track->trackAreaRight.GetWidth();
					str.Printf("Amostra: %d   Ponto: %d\n",sample,int(x) - m_track->trackAreaRight.GetX());
					m_owner->SetStatusText( str, 0 );

					str.Printf("Start: %d   End: %d", m_track->m_startPoint, m_track->m_endPoint);
					m_owner->SetStatusText( str, 1 );

					sample = ( int(x) - m_track->trackAreaRight.GetX() )*(m_track->trackInfo.frames) / m_track->trackAreaRight.GetWidth();
					str.Printf("Tempo: %f\n",(float) sample/m_track->trackInfo.samplerate);
					m_owner->SetStatusText( str, 2 );
				}
			}

		}


		/*

		if ( (m_owner->fileOpened) & ( showAms ) & (m_track->trackArea.Inside(x,y)) ){
			wxString	str;
			int			sample;

			sample = ( int(x) - m_track->trackArea.GetX() )*(m_track->trackInfo.frames) / m_track->trackArea.GetWidth();
			str.Printf("Amostra: %d   Ponto: %d\n",sample,int(x) - m_track->trackArea.GetX());
			m_owner->SetStatusText( str, 0 );
		}

		if (m_track->trackInfo.channels == 2){
			if ( (m_owner->fileOpened) & ( showAms ) &  (m_track->isInside(x,y)) ){
				wxString	str;
				int			sample = -1;

				//wxPoint pos = event.GetPosition();
				//long x = dc.DeviceToLogicalX( pos.x );

				if (m_track->trackAreaLeft.Inside(x,y) ){
					sample = ( int(x) - m_track->trackAreaLeft.GetX() )*(m_track->trackInfo.frames) / m_track->trackAreaLeft.GetWidth();
					str.Printf("Amostra: %d   Ponto: %d\n",sample,int(x) - m_track->trackAreaLeft.GetX());
				}

				if (m_track->trackAreaRight.Inside(x,y) ){
					sample = ( int(x) - m_track->trackAreaRight.GetX() )*(m_track->trackInfo.frames) / m_track->trackAreaRight.GetWidth();
					str.Printf("Amostra: %d   Ponto: %d\n",sample,int(x) - m_track->trackAreaRight.GetX());
				}

				//str.Printf("Amostra: %d   Ponto: %d\n",sample,int(x) - m_track->trackArea.GetX());
				m_owner->SetStatusText( str, 0 );
			}
		}


		
		if ( (m_owner->fileOpened) & ( showTemp ) & (m_track->trackArea.Inside(x,y)) ){
			wxString	str;
			int sample;

			wxPoint pos = event.GetPosition();
			long x = dc.DeviceToLogicalX( pos.x );
			
			//str.Printf("%f\n", (float) m_track->trackInfo.frames/m_track->trackInfo.samplerate);
			sample = ( int(x) - m_track->trackAreaLeft.GetX() )*(m_track->trackInfo.frames) / m_track->trackArea.GetWidth();

			str.Printf("Tempo: %f\n",(float) sample/m_track->trackInfo.samplerate);
			m_owner->SetStatusText( str, 2 );
		}
		*/

	}
}

void MyCanvas::OnMouseDoubleClick(wxMouseEvent &event)
{
	//wxMessageBox("dC");
   	wxString str;
    	wxClientDC dc(this);
    	PrepareDC(dc);
    	m_owner->PrepareDC(dc);
	wxColour colour;
	wxPen pen;
	wxBrush brush;
	int width, height;

	int grid_x1, grid_x2, grid_y1, grid_y2;
	int	xx,yy;
	//int sample;

	if ( m_owner->fileOpened ){

		GetClientSize(&width,&height);
		wxPoint pos = event.GetPosition();
		long x = dc.DeviceToLogicalX( pos.x );
		long y = dc.DeviceToLogicalY( pos.y );
		selected = false;
			

		if ( (m_track->trackInfo.channels == 1) && ( m_track->trackArea.Inside(x,y)) ){
				
			SetCursor(*mSelectCursor);	
			drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
			m_walkGraphic->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);
			
			GetViewStart(&xx, &yy);

			//m_track->m_buttonPressed = true;
			//m_track->m_startPoint = int(x) - m_track->trackArea.GetX();
			m_track->m_startSample = -1;
			m_track->m_endSample = -1;
		}
		

		if ( (m_track->trackInfo.channels == 2) && 
			 (  ( m_track->trackAreaLeft.Inside(x,y)) || ( m_track->trackAreaRight.Inside(x,y)) ) ){
			
			SetCursor(*mSelectCursor);	
			drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
			
			m_walkGraphicLeft->getArea(&grid_x1, &grid_y1, &grid_x2, &grid_y2);


			//m_track->m_buttonPressed = true;
			//m_track->m_startPoint = int(x) - m_track->trackAreaLeft.GetX();
			m_track->m_startSample = -1;
			m_track->m_endSample = -1;

		}

   	/*
		// So se estiver dentro da area
		//selected = false;
		if (m_track->isInside(x,y)){

			m_track->m_validPressed = true;

			m_owner->menuBar->Enable(ID_NULL, true);
			m_owner->menuBar->Enable(ID_R_NULL, true);

			m_owner->menuBar->Enable(ID_COPY, true);
			m_owner->menuBar->Enable(ID_CUT, true);
			m_owner->menuBar->Enable(ID_PASTE, true);

			m_owner->menuBar->Enable(ID_EXPAND, true);

			m_owner->menuBar->Enable(ID_GAIN, true);
			m_owner->menuBar->Enable(ID_FADE_IN, true);
			m_owner->menuBar->Enable(ID_FADE_OUT, true);


			m_owner->toolBar->EnableTool(ID_COPY, true);
			m_owner->toolBar->EnableTool(ID_CUT, true);

			m_owner->SetStatusText( "", 1 );

		}
		else 
			m_track->m_validPressed = false;

		*/
		
	}

}
/*
	------------------void MyCanvas::copy()-------------------------------------
	Objetivo : Copiar para um buffer de memória as amostras previamente marcadas
	Recebe   : void
	Devolve  : void

	Goal       : Copy to a memory buffer the samples previously marked
	Parameters : void
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------------------------------
*/
void MyCanvas::copy()
{
	int		k;
	int		size;

	
	if (m_track->m_startSample < m_track->m_endSample){
		m_startClipboard = m_track->m_startSample;
		m_endClipboard   = m_track->m_endSample;
	}
	else{
		m_startClipboard = m_track->m_endSample;
		m_endClipboard   = m_track->m_startSample;
	}

	size = (int) (m_endClipboard-m_startClipboard);
	
	// Mono
	if (m_track->trackInfo.channels == 1){
		
		if ( (m_clipboard = (float*)malloc( size*sizeof(float) )) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
			exit(1);
		}

		k = (int) m_startClipboard;
		for (int i = 0; i < size; i++ ){
			m_clipboard[i] = m_track->trackFptr[k];
			k++;
		}

		
		// PARA O ARQUIVO DE MEMORIA 
		FILE	*tmpFile;
		tmpFile =  fopen("trans.txt","wb");

		int counter = 0;
		char sz[101];
		//char tmp;
		

		sprintf(sz, "%i",size);
		fwrite(sz,strlen(sz),1,tmpFile);
		fwrite("\r\n",1,1,tmpFile);

		while (counter < size)
		{
			sprintf(sz, "%G",m_clipboard[counter]);
			fwrite(sz,strlen(sz),1,tmpFile);
			fwrite("\r\n",1,1,tmpFile);
			counter++;
		}
		fclose(tmpFile);
		


	}

	// Stereo
	if (m_track->trackInfo.channels == 2){
		
		if (!showSeparate){
			if ( (m_clipboardLeft = (float*)malloc( size*sizeof(float) )) == NULL ){
				wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
				exit(1);
			}
			if ( (m_clipboardRight = (float*)malloc( size*sizeof(float) )) == NULL ){
				wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
				exit(1);
			}

			k = (int)m_startClipboard;
			for (int i = 0; i < size; i++ ){
				m_clipboardLeft[i] = m_track->trackFptrLeft[k];
				m_clipboardRight[i] = m_track->trackFptrRight[k];

				k++;
			}

			// PARA O ARQUIVO DE MEMORIA 
			FILE	*tmpFile;
			tmpFile =  fopen("trans.txt","wb");

			int counter = 0;
			char sz[101];
			//char tmp;
			

			sprintf(sz, "%i",size);
			fwrite(sz,strlen(sz),1,tmpFile);
			fwrite("\r\n",1,1,tmpFile);

			while (counter < size)
			{
				sprintf(sz, "%G",m_clipboardLeft[counter]);
				fwrite(sz,strlen(sz),1,tmpFile);
				fwrite("\r\n",1,1,tmpFile);
				counter++;
			}

			counter = 0;
			while (counter < size)
			{
				sprintf(sz, "%G",m_clipboardRight[counter]);
				fwrite(sz,strlen(sz),1,tmpFile);
				fwrite("\r\n",1,1,tmpFile);
				counter++;
			}

			fclose(tmpFile);
		}
		else{
			if (selectedL){
				
				if ( (m_clipboardLeft = (float*)malloc( size*sizeof(float) )) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
					exit(1);
				}
				if ( (m_clipboardRight = (float*)malloc( size*sizeof(float) )) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
					exit(1);
				}

				k = (int) m_startClipboard;
				for (int i = 0; i < size; i++ ){
					m_clipboardLeft[i] = m_track->trackFptrLeft[k];
					m_clipboardRight[i] = 0;

					k++;
				}

				// PARA O ARQUIVO DE MEMORIA 
				FILE	*tmpFile;
				tmpFile =  fopen("trans.txt","wb");

				int counter = 0;
				char sz[101];
				//char tmp;
				

				sprintf(sz, "%i",size);
				fwrite(sz,strlen(sz),1,tmpFile);
				fwrite("\r\n",1,1,tmpFile);

				while (counter < size)
				{
					sprintf(sz, "%G",m_clipboardLeft[counter]);
					fwrite(sz,strlen(sz),1,tmpFile);
					fwrite("\r\n",1,1,tmpFile);
					counter++;
				}

				counter = 0;
				while (counter < size)
				{
					sprintf(sz, "%G",m_clipboardRight[counter]);
					fwrite(sz,strlen(sz),1,tmpFile);
					fwrite("\r\n",1,1,tmpFile);
					counter++;
				}

				fclose(tmpFile);

			}

			if (selectedR){
				
				if ( (m_clipboardLeft = (float*)malloc( size*sizeof(float) )) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
					exit(1);
				}
				if ( (m_clipboardRight = (float*)malloc( size*sizeof(float) )) == NULL ){
					wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
					exit(1);
				}

				k = (int) m_startClipboard;
				for (int i = 0; i < size; i++ ){
					m_clipboardLeft[i] = 0;
					m_clipboardRight[i] = m_track->trackFptrRight[k];

					k++;
				}

				// PARA O ARQUIVO DE MEMORIA 
				FILE	*tmpFile;
				tmpFile =  fopen("trans.txt","wb");

				int counter = 0;
				char sz[101];
				//char tmp;
				

				sprintf(sz, "%i",size);
				fwrite(sz,strlen(sz),1,tmpFile);
				fwrite("\r\n",1,1,tmpFile);

				while (counter < size)
				{
					sprintf(sz, "%G",m_clipboardLeft[counter]);
					fwrite(sz,strlen(sz),1,tmpFile);
					fwrite("\r\n",1,1,tmpFile);
					counter++;
				}

				counter = 0;
				while (counter < size)
				{
					sprintf(sz, "%G",m_clipboardRight[counter]);
					fwrite(sz,strlen(sz),1,tmpFile);
					fwrite("\r\n",1,1,tmpFile);
					counter++;
				}

				fclose(tmpFile);
			}
		}

	}

	m_owner->menuBar->Enable(ID_PASTE, true);
	m_owner->toolBar->EnableTool(ID_PASTE, true);


}


/*
	------------------void MyCanvas::paste()-------------------------------------------
	Objetivo : Copiar para o buffer desenhado na tela, as amostras copiadas da memória
	Recebe   : void
	Devolve  : void

	Goal       : Copy to the display buffer, the samples that were in the memory buffer
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::paste()
{
	int		clipSize;
	int		start,end;
	int		k,i;
	int     	trackSize;
	int		width, height;
	float		*rasc;			//rasc do arq mono
	float		*rascL,*rascR;  // rascs do arq stereo
	wxString 	str;
	
	GetClientSize(&width,&height);

	trackSize = (m_track->trackInfo.frames);

	start = (int) (m_track->m_startSample);
	end = (int) (m_track->m_endSample);
	
	clipSize =(int) (m_endClipboard - m_startClipboard);


	// Mono
	if ((m_track->trackInfo.channels == 1) ) {
		
		rasc = new float[trackSize];

		

		// DO ARQUIVO DE MEMORIA 
		// ------------------------------------------------------------------
		FILE	*tmpFile;
		char    buffer[50];
		int		size;
		float	*sClipboard;


		if ( (tmpFile =  fopen("trans.txt","rb")) == NULL)
		{
			wxMessageBox("Nao foi possivel abrir o arquivo para transferencia");
			exit(1);
		}

		fscanf (tmpFile, "%s", buffer);
		size = atoi(buffer);
		clipSize = size;

		if ( (sClipboard = (float*)malloc( clipSize*sizeof(float) )) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
			exit(1);
		}


		for (i=0; i < size ;i++)
		{
			fscanf(tmpFile, "%s", buffer);
			sClipboard[i] = atof(buffer);
		}

		fclose(tmpFile);
	
		
		// Guardando no rascunho o conteudo de Fptr
		for (i = 0; i < trackSize; i++) rasc[i] = m_track->trackFptr[i];

		if (m_track->trackFptr) free(m_track->trackFptr);
		
		if ((m_track->trackFptr = (float*)malloc((trackSize+clipSize)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		
		
		// Zerando Fptr novo
		for (i = 0; i < trackSize + clipSize; i++) m_track->trackFptr[i] = 0;
		
		
		// Colando ate o ponto de corte
		for (i = 0; i < start; i++) m_track->trackFptr[i] = rasc[i];

		// Colando o trecho copiado
		k = start;
		for (i = 0; i < clipSize; i++){
			//m_track->trackFptr[k] = m_clipboard[i];
			m_track->trackFptr[k] = sClipboard[i];
			k++;
		}

		// Colando do ponto de corte em diante
		for (i = start; i < trackSize; i++){
			m_track->trackFptr[k] = rasc[i];
			k++;
		}
		
		
		//m_track->trackInfo.frames = trackSize + clipSize;
		
		m_track->ini_width = m_track->ini_width*(clipSize + m_track->trackInfo.frames)/m_track->trackInfo.frames;
		m_track->trackInfo.frames = trackSize + clipSize;

		SetVirtualSize(m_track->ini_width, 0 );

		m_track->m_startPoint = (int)  ( (float) ( start * m_track->ini_width) / (m_track->trackInfo.frames)) ;
		m_track->m_endPoint = (int) ((float) ( (start+clipSize) * m_track->ini_width) / (m_track->trackInfo.frames));
		
		
		Clear();
		drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		fit();
		//drawOpenedFileSelect(m_track->trackFptr, m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
		//drawOpenedFile(sClipboard,size);
	}


	// Stereo
	if (m_track->trackInfo.channels == 2){
	

		
		rascL = new float[trackSize];
		rascR = new float[trackSize];

			// DO ARQUIVO DE MEMORIA 
		// ------------------------------------------------------------------
		FILE	*tmpFile;
		char    buffer[50];
		int		size;
		float	*sClipboardLeft,*sClipboardRight;


		if ( (tmpFile =  fopen("trans.txt","rb")) == NULL)
		{
			wxMessageBox("Nao foi possivel abrir o arquivo para transferencia");
			exit(1);
		}

		fscanf (tmpFile, "%s", buffer);
		size = atoi(buffer);
		clipSize = size;

		if ( (sClipboardLeft = (float*)malloc( clipSize*sizeof(float) )) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
			exit(1);
		}

		
		if ( (sClipboardRight = (float*)malloc( clipSize*sizeof(float) )) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK);
			exit(1);
		}


		for (i=0; i < size ;i++)
		{
			fscanf(tmpFile, "%s", buffer);
			sClipboardLeft[i] = atof(buffer);
		}

		for (i=0; i < size ;i++)
		{
			fscanf(tmpFile, "%s", buffer);
			sClipboardRight[i] = atof(buffer);
		}

		fclose(tmpFile);


		
		// Guardando no rascunho o conteudo de Fptr
		for (i = 0; i < trackSize; i++) rascL[i] = m_track->trackFptrLeft[i];
		for (i = 0; i < trackSize; i++) rascR[i] = m_track->trackFptrRight[i];
	
		
		if (m_track->trackFptrLeft) free(m_track->trackFptrLeft);
		if (m_track->trackFptrRight) free(m_track->trackFptrRight);

		if ((m_track->trackFptrLeft = (float*)malloc((trackSize+clipSize)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		if ((m_track->trackFptrRight = (float*)malloc((trackSize+clipSize)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		
		// Zerando Fptr novo
		for (i = 0; i < trackSize + clipSize; i++){
			m_track->trackFptrLeft[i] = 0;
			m_track->trackFptrRight[i] = 0;
		}
		
		for (i = 0; i < trackSize; i++){
			m_track->trackFptrLeft[i] = rascL[i];
			m_track->trackFptrRight[i] = rascR[i];
		}

		
		// Colando ate o ponto de corte
		for (i = 0; i < start; i++){
			m_track->trackFptrLeft[i] = rascL[i];
			m_track->trackFptrRight[i] = rascR[i];
		}

		
		// Colando o trecho copiado
		k = start;
		for (i = 0; i < clipSize; i++){
			m_track->trackFptrLeft[k] = sClipboardLeft[i];
			m_track->trackFptrRight[k] = sClipboardRight[i];

			//m_track->trackFptrLeft[k] = m_clipboardLeft[i];
			//m_track->trackFptrRight[k] = m_clipboardRight[i];
			k++;
		}

		
		// Colando do ponto de corte em diante
		for (i = start; i < trackSize; i++){
			m_track->trackFptrLeft[k] = rascL[i];
			m_track->trackFptrRight[k] = rascR[i];
			k++;
		}
		
		
		
		m_track->ini_width = m_track->ini_width*(clipSize + m_track->trackInfo.frames)/m_track->trackInfo.frames;
		m_track->trackInfo.frames = trackSize + clipSize;
		SetVirtualSize(m_track->ini_width, 0 );

		m_track->m_startPoint = (int) ( (float) ( start * m_track->ini_width) / (m_track->trackInfo.frames));
		m_track->m_endPoint = (int) ((float) ( (start+clipSize) * m_track->ini_width) / (m_track->trackInfo.frames));
		
		
		Clear();
		drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
		//drawOpenedFileStereo(rascL,rascR,m_track->trackInfo.frames);
		
	}
}


 /*
	------------------void MyCanvas::cut()-----------------------------------------------
	Objetivo : Copiar para o buffer de memoria, as amostras retiradas do buffer desenhado 
	Recebe   : void
	Devolve  : void

	Goal       : Copy to the memory buffer, the samples that were cutted from the 
				 displayed buffer
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::cut()
{
	int		start,end;
	int		i,k;
	int     trackSize, size;
	int		width, height;
	float	*rasc;
	float   *rascL,*rascR;
	

	GetClientSize(&width,&height);
	trackSize = (m_track->trackInfo.frames);

	if (m_track->m_startSample < m_track->m_endSample){
		start = (int) m_track->m_startSample;
		end   = (int) m_track->m_endSample;
	}
	else{
		start = (int) m_track->m_endSample;
		end   = (int) m_track->m_startSample;
	}

	size = end - start;

	
	if (m_track->trackInfo.channels == 1){
		
		rasc = new float[trackSize - size];
		m_clipboard = new float[size];

		
		// --- Copying the samples to clipoard
		k = start;
		for (i = 0; i < size; i++ ){
			m_clipboard[i] = m_track->trackFptr[k];
			k++;
		}

		// PARA O ARQUIVO DE MEMORIA 
		FILE	*tmpFile;
		tmpFile =  fopen("trans.txt","wb");

		int counter = 0;
		char sz[101];
		//char tmp;
		

		sprintf(sz, "%i",size);
		fwrite(sz,strlen(sz),1,tmpFile);
		fwrite("\r\n",1,1,tmpFile);

		while (counter < size)
		{
			sprintf(sz, "%G",m_clipboard[counter]);
			fwrite(sz,strlen(sz),1,tmpFile);
			fwrite("\r\n",1,1,tmpFile);
			counter++;
		}
		fclose(tmpFile);


		// --- Mantaining the other samples in the track
		for (i = 0; i < start; i++) rasc[i] = m_track->trackFptr[i];
		k = start;
		for (i = end; i < trackSize; i++){
			rasc[k] = m_track->trackFptr[i];
			k++;
		}


		
		if (m_track->trackFptr) free(m_track->trackFptr);
		if ((m_track->trackFptr = (float*)malloc((trackSize - size)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		
		// Zerando Fptr novo
		for (i = 0; i < trackSize - size; i++) m_track->trackFptr[i] = 0;
		for (i = 0; i < trackSize - size; i++) m_track->trackFptr[i] = rasc[i];
		
		
		m_track->trackInfo.frames = trackSize - size;
		m_track->m_startPoint = m_track->m_endPoint = 0;
		drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
	}


	if (m_track->trackInfo.channels == 2){

		rascL = new float[trackSize - size];
		rascR = new float[trackSize - size];
		m_clipboardLeft = new float[size];
		m_clipboardRight = new float[size];

		
		// --- Copying the samples to clipoard
		k = start;
		for (i = 0; i < size; i++ ){
			m_clipboardLeft[i] = m_track->trackFptrLeft[k];
			m_clipboardRight[i] = m_track->trackFptrRight[k];
			k++;
		}

		// PARA O ARQUIVO DE MEMORIA 
		FILE	*tmpFile;
		tmpFile =  fopen("trans.txt","wb");

		int counter = 0;
		char sz[101];
		//char tmp;
		

		sprintf(sz, "%i",size);
		fwrite(sz,strlen(sz),1,tmpFile);
		fwrite("\r\n",1,1,tmpFile);

		while (counter < size)
		{
			sprintf(sz, "%G",m_clipboardLeft[counter]);
			fwrite(sz,strlen(sz),1,tmpFile);
			fwrite("\r\n",1,1,tmpFile);
			counter++;
		}

		counter = 0;
		while (counter < size)
		{
			sprintf(sz, "%G",m_clipboardRight[counter]);
			fwrite(sz,strlen(sz),1,tmpFile);
			fwrite("\r\n",1,1,tmpFile);
			counter++;
		}

		fclose(tmpFile);

		// Ajeitando as amostras
		for (i = 0; i < start; i++){
			rascL[i] = m_track->trackFptrLeft[i];
			rascR[i] = m_track->trackFptrRight[i];
		}

		k = start;
		for (i = end; i < trackSize; i++){
			rascL[k] = m_track->trackFptrLeft[i];
			rascR[k] = m_track->trackFptrRight[i];
			k++;
		}

		if (m_track->trackFptrLeft) free(m_track->trackFptrLeft);
		if (m_track->trackFptrRight) free(m_track->trackFptrRight);

		if ((m_track->trackFptrLeft = (float*)malloc((trackSize - size)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}

		if ((m_track->trackFptrRight = (float*)malloc((trackSize - size)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}

		// Zerando Fptr novo
		for (i = 0; i < trackSize - size; i++){
			m_track->trackFptrLeft[i] = 0;
			m_track->trackFptrRight[i] = 0;
		}
		
		for (i = 0; i < trackSize - size; i++){
			m_track->trackFptrLeft[i] = rascL[i];
			m_track->trackFptrRight[i] = rascR[i];
		}

		m_track->trackInfo.frames = trackSize - size;

		m_track->m_startPoint = m_track->m_endPoint = 0;

		//drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);

	}
	
	if (selected) selected = false;

	
}

 /*
	------------------void MyCanvas::selectAll()---------------------------------------
	Objetivo : Copiar para o buffer de memoria, as amostras retiradas do buffer desenhado 
	Recebe   : void
	Devolve  : void

	Goal       : Copy to the memory buffer, the samples that were cutted from the 
				 displayed buffer
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::selectAll()
{
	
	int		start, end, size;
	start = end = 0;

	
	start = m_track->m_startPoint = 0;	
	if (m_track->trackInfo.channels == 1)
		end	  = m_track->m_endPoint = m_track->trackArea.width;
	
	if (m_track->trackInfo.channels == 2)
		end	  = m_track->m_endPoint = m_track->trackAreaLeft.width;
	
	m_track->m_startSample = m_track->toSample(start);
	m_track->m_endSample = m_track->toSample(end);

	size  = m_track->trackInfo.frames;


	if (m_track->trackInfo.channels == 1)
			drawOpenedFileSelect(m_track->trackFptr ,size, start, end);

	if (m_track->trackInfo.channels == 2){
		if (!showSeparate)
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,size,start,end);
		/*
		else{
			if (selectedL){
			}

			if (selectedR){
			}
		}
		*/
			
	}




	m_owner->menuBar->Enable(ID_NULL, true);
	m_owner->menuBar->Enable(ID_R_NULL, true);

	m_owner->menuBar->Enable(ID_COPY, true);
	m_owner->menuBar->Enable(ID_CUT, true);
	m_owner->menuBar->Enable(ID_PASTE, true);

	m_owner->menuBar->Enable(ID_EXPAND, true);

	m_owner->menuBar->Enable(ID_GAIN, true);
	m_owner->menuBar->Enable(ID_FADE_IN, true);
	m_owner->menuBar->Enable(ID_FADE_OUT, true);

	m_owner->menuBar->Enable(ID_CROSS_OVER, true);
	m_owner->toolBar->EnableTool(ID_EXPAND,true);
	m_owner->toolBar->EnableTool(ID_PLAY_SEL,true);


	m_owner->toolBar->EnableTool(ID_COPY, true);
	m_owner->toolBar->EnableTool(ID_CUT, true);


	selected = true;


}

/*	Objetivo : Exeutar o arquivo que está na memória
	Recebe   : void
	Devolve  : void
 */
void MyCanvas::play()
{
	int	ret;
	
	ret = playStream(m_track->trackFptr, m_track->trackInfo);
	if (ret !=0)
		wxMessageBox("Erro no play\n");
}

/*	Objetivo : Exeutar a parcela selecionada do arquivo que está na memória
	Recebe   : void
	Devolve  : void
 */
void MyCanvas::playSelected()
{
	// Se o arquivo foi aberto e executado, entao so precisamos passar trackFptr, pois mesmo sendo 
	// estereo, ele ja esta ordenado
	/*
	int		size = 0;
	float	*fptr;
	int		start,end;
	

	if (m_track->trackInfo.channels == 1){
		if (m_track->m_startSample < m_track->m_endSample){
			start = m_track->m_startSample;
			end   = m_track->m_endSample;
		}
		else{
			start = m_track->m_endSample;
			end   = m_track->m_startSample;
		}
		size =  end - start;
		if ((fptr = (float*)malloc((size)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		int k = 0;
		for (int i = start; i < end; i++){
			fptr[k] = m_track->trackFptr[i];
			k++;
		}
		playPartStream(fptr, m_track->trackInfo.channels, m_track->trackInfo.channels, size, m_track->trackInfo.samplerate );
		free(fptr);
	}
		
	if (m_track->trackInfo.channels == 2){
		if (m_track->m_startSample < m_track->m_endSample){
			start = m_track->m_startSample;
			end   = m_track->m_endSample;
		}
		else{
			start = m_track->m_endSample;
			end   = m_track->m_startSample;
		}
		size =  end - start;
		if ((fptr = (float*)malloc((size)*m_track->trackInfo.channels*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		int k = 0;
		for (int i = start; i < 2*size; i++){
			fptr[k] = m_track->trackFptr[i];
			k++;
		}
		playPartStream(fptr, m_track->trackInfo.channels, m_track->trackInfo.channels, size, m_track->trackInfo.samplerate );
		free(fptr);
	}
	*/
}


 /*
	------------------void MyCanvas::expand()-----------------------------------------------
	Objetivo : Redimensionar a tela de exibição ajustando o trecho selecionado para corresponder
			   ao total da tela
	Recebe   : void
	Devolve  : void

	Goal       : Resize the drawing to fit the hole selection on the screen
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::expand()
{
	int		width, 
			height;
	int		startScroll;
	//int		grid_x1, grid_y1, grid_x2, grid_y2;
	int		xx,
			yy;
	int		newSize;
	int		oldWidth;
	int		start, 
			end;

	xx		= 0;
	startScroll 	= 0;
	start		= m_track->m_startPoint;
	end		= m_track->m_endPoint;
	oldWidth	= m_track->ini_width;
	
	GetClientSize(&width,&height);
	GetViewStart(&xx, &yy);

	
	if (start < end){
		newSize = (int) (m_track->m_endSample - m_track->m_startSample) ;
		m_track->ini_width = (int) (( width*m_track->trackInfo.frames ) / ( m_track->m_endSample - m_track->m_startSample ));
	}
	else{
		newSize = (int) (m_track->m_startSample - m_track->m_endSample) ;
		m_track->ini_width = (int) (( width*m_track->trackInfo.frames ) / ( m_track->m_startSample  - m_track->m_endSample));
	}
	 
	SetVirtualSize(m_track->ini_width , 0 );
	
	// BORDER !!!
	m_track->border = m_track->trackArea.GetX()/10 + 40;
	
	if (m_track->trackInfo.channels == 1) {
		
		m_track->m_startPoint = (int) (m_track->m_startSample*m_track->ini_width / (m_track->trackInfo.frames));
		m_track->m_endPoint   = (int) (m_track->m_endSample*m_track->ini_width / (m_track->trackInfo.frames));

		if ( start < end){
			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint , m_track->m_endPoint);
			Scroll( (m_track->m_startPoint + m_track->trackArea.GetX())/10 ,0);	
		}

		if (start > end){
			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_endPoint , m_track->m_startPoint);
			Scroll( (m_track->m_endPoint + m_track->trackArea.GetX())/10 ,0);	
		}

		
	}
	
	if (m_track->trackInfo.channels == 2) {
		m_track->m_startPoint = (int) (m_track->m_startSample*m_track->ini_width / (m_track->trackInfo.frames));
		m_track->m_endPoint   = (int) (m_track->m_endSample*m_track->ini_width / (m_track->trackInfo.frames));
				
		if ( start < end){
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			//drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
			Scroll( (m_track->m_startPoint + m_track->trackAreaLeft.GetX())/10 ,0);
		}

		if ( start > end){
			//drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_endPoint+2,m_track->m_startPoint );
			Scroll( (m_track->m_endPoint + m_track->trackAreaLeft.GetX())/10 ,0);
		}


	}
	
}

 /*
	------------------void MyCanvas::fit()-----------------------------------------------
	Objetivo : Redimensionar a tela de exibição ajustando todo o arquivo ao espaço 
  			   disponível em tela

	Recebe   : void
	Devolve  : void

	Goal       : Resize the drawing to fit the hole file on the screen
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::fit()
{
	int width, height;

	GetClientSize(&width,&height);
	m_track->ini_width = width;
	
	SetVirtualSize(m_track->ini_width,0);
	m_track->zoomFactor = 1;

	

	if (!selected){
		
		m_track->m_startPoint = m_track->m_endPoint = -1;
		m_track->m_startSample = m_track->m_endSample = -1;

		if (m_track->trackInfo.channels == 1)
			drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		if (m_track->trackInfo.channels == 2)
			drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
	}
	else{
		if (m_track->trackInfo.channels == 1){

			m_track->m_startPoint =(int) (m_track->m_startSample*(m_track->ini_width - m_track->trackArea.GetX() )/ (m_track->trackInfo.frames));
			m_track->m_endPoint = (int) (m_track->m_endSample*(m_track->ini_width - m_track->trackArea.GetX() )/ (m_track->trackInfo.frames));

			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint , m_track->m_endPoint);
			//Scroll ( - m_track->m_startPoint*10, 0 );
		}
			
		if (m_track->trackInfo.channels == 2){
			
			m_track->m_startPoint = (int) (m_track->m_startSample*(m_track->ini_width - m_track->trackAreaLeft.GetX() )/ (m_track->trackInfo.frames));
			m_track->m_endPoint = (int) (m_track->m_endSample*(m_track->ini_width - m_track->trackAreaLeft.GetX() )/ (m_track->trackInfo.frames));
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_endPoint+2,m_track->m_startPoint );
		}
		
	}
}

void MyCanvas::teste()
{
	wxString	str;	
	str.Printf("Point %d      %d",m_track->m_startPoint,m_track->m_endPoint);
	wxMessageBox(str);

	//str.Printf("Samples %f      %f",m_track->m_startSample,m_track->m_endSample);
	//wxMessageBox(str);


	//float result = m_track->toSample(m_track->m_startPoint);
	//str.Printf("Sample %f",result);
	//wxMessageBox(str);
	

	

	/*
	int		start, end, size;

	start = m_track->m_startPoint = 1;	
	end	  = m_track->trackArea.width;
	size  = m_track->trackInfo.frames;

	drawOpenedFileSelect(m_track->trackFptr ,size, start, end);
	*/

	/*
	wxPanel *panel = new wxPanel(this);
	wxCheckBox *m_check;
	m_check = new wxCheckBox(panel, -1, _T("show welcome message box at startup"),
                           wxPoint(10, 70), wxSize(300, 20));
	*/

	/*
	if (wxTheClipboard->Open()){
				
		wxCustomDataObject data;
		//float *ptr;
		wxTheClipboard->GetData( data );

		int size = data.GetDataSize();
		str.Printf("Size%d\n", size);
		wxMessageBox(str,"Size", wxICON_INFORMATION | wxOK );
			
		wxTheClipboard->Close();
	}
	*/	

	

	//str.Printf("Samples %d  readcntf %d\n ", m_track->trackInfo.frames, (int) m_track->readcntf);
	//wxMessageBox(str,"Selected", wxICON_INFORMATION | wxOK );
	
	//str.Printf("frames %f  readcntf %f\n ", (float) m_track->trackInfo.frames/m_track->trackInfo.samplerate,(float) m_track->readcntf/m_track->trackInfo.samplerate);
	//wxMessageBox(str,"Selected", wxICON_INFORMATION | wxOK );
		
	
	/*
	str.Printf("Channels %d\n Format %d\n Frames %d\n", 
		m_track->trackInfo.channels,
		m_track->trackInfo.format,
		m_track->trackInfo.frames
	);
	wxMessageBox(str,"Selected", wxICON_INFORMATION | wxOK );

	str.Printf("Samplerate %d\n Sections %d\n Seekable %d\n", 
		m_track->trackInfo.samplerate,
		m_track->trackInfo.sections,
		m_track->trackInfo.seekable
	);
	wxMessageBox(str,"Selected", wxICON_INFORMATION | wxOK );
	*/

	//str.Printf("%d\n ", m_track->trackInfo.samplerate);
	//str.Printf("Channels %d\n Format %d\n Frames %d\n Samplerate %f\n", m_track->trackInfo.channels, m_track->trackInfo.format,m_track->trackInfo.frames,m_track->trackInfo.samplerate);
	//wxMessageBox(str,"Selected", wxICON_INFORMATION | wxOK );

	/*
	str.Printf("Samplerate %d\n Channels %d\n Frames %d\n ", m_track->trackInfo.samplerate, m_track->trackInfo.channels,m_track->trackInfo.frames);
	wxMessageBox(str,"Selected", wxICON_INFORMATION | wxOK );
	
	str.Printf("Format %d\n Seekable %d\n Sections %d\n", m_track->trackInfo.format,m_track->trackInfo.seekable,m_track->trackInfo.sections);
	wxMessageBox(str,"Selected", wxICON_INFORMATION | wxOK );
	*/

	/*
	SNDFILE		*infile;
	SNDFILE		*outfile;
	char		*filename = "result.wav";
	SF_INFO		info;
	float		*samples;
	sf_count_t	readcntf;
	sf_count_t	writecnt;
	int			i,ret,frames,k;
	float		*saida,*left,*right;
	ret = 0;

	printf("Teste da biblioteca libSndFile\n");

	if ((infile = sf_open("weapon2.wav", SFM_READ, &info)) == NULL ){
		//printf("Erro abrindo o arquivo para leitura\n");
		wxMessageBox("Erro abrindo o arquivo para leitura\n");
		sf_close(infile);
	}

	if ((samples = (float*)malloc((info.frames*info.channels)*sizeof(float))) == NULL ){
			//printf("Erro de alocação de memória\n");
			wxMessageBox("Erro de alocação de memória\n");
			exit(1);
	}
	
	readcntf = sf_read_float(infile, samples, info.frames*info.channels);

	sf_close(infile);

	
	//printf("\n\nExibindo as informacoes do header\n\n");
	//printf("Channels    %d\n",info.channels);
	//printf("Formato     %d\n",info.format);
	//printf("Frames      %d\n",info.frames);
	//printf("Sample Rate %d\n",info.samplerate);
	//printf("Seções      %d\n",info.sections);
	//printf("Seekable    %d\n",info.seekable);
	
	//printf("\n\nExibindo as 5 primeiras amostras\n\n");
	//for (int i = 0; i < 6; i ++)
	//	printf("Amostra %d - % f\n",i,samples[i]);
	
	frames = info.frames;

	if ((saida = (float*)malloc((info.frames)*sizeof(float))) == NULL ){
			//printf("Erro de alocação de memória\n");
			wxMessageBox("Erro de alocação de memória\n");
			exit(1);
	}

	if ((left = (float*)malloc((info.frames)*sizeof(float))) == NULL ){
			//printf("Erro de alocação de memória\n");
			wxMessageBox("Erro de alocação de memória\n");
			exit(1);
	}


	if ((right = (float*)malloc((info.frames)*sizeof(float))) == NULL ){
			//printf("Erro de alocação de memória\n");
			wxMessageBox("Erro de alocação de memória\n");
			exit(1);
	}


	//printf("Alocou\n\n");
	//int lixo = getchar();

	k = 0;
	for (int lef = 0; lef < readcntf;lef +=2){
			left[k] = samples[lef];
			k++;
	}
		
	k = 0;
	for (int rig = 1; rig < readcntf; rig +=2){
		right[k] = samples[rig];
		k++;
	}
	
	for (i=0; i < info.frames; i++) saida[i] = (left[i] + right[i])/2;
	

	info.channels = 1;


		
	//printf("\n\n");

	//printf("Salvando ... \n\n");

	
	if ((outfile  = sf_open(filename, SFM_WRITE, &info)) == NULL ){
		printf("Erro abrindo o arquivo para escrita\n");
		wxMessageBox("Erro abrindo o arquivo para escrita\n");
		sf_close(outfile);
	}
	
	info.frames = frames;
	writecnt = sf_write_float(outfile, saida, info.frames);
	//writecnt = sf_write_float(outfile, right, info.frames);

	sf_close(outfile);

	free(samples);
	free(saida);
	free(left);
	free(right);
	*/
	/*
	str.Printf("Channels    %d\n Formato     %d\n Frames      %d\n",m_track->trackInfo.channels,m_track->trackInfo.format,m_track->trackInfo.frames);
	wxMessageBox(str);
	
	str.Printf("Sample Rate %d\n Seções      %d\n Seekable    %d\n",m_track->trackInfo.samplerate,m_track->trackInfo.sections,m_track->trackInfo.seekable);
	wxMessageBox(str);
	*/

	 
	
	

	
	//str.Printf("%d",selected);
	//wxMessageBox(str);

	
	
	//printf("Formato     %d\n",info.format);
	//printf("Frames      %d\n",info.frames);
	//printf("Sample Rate %d\n",info.samplerate);
	//printf("Seções      %d\n",info.sections);
	//printf("Seekable    %d\n",info.seekable);

}


/*
	------------------void MyCanvas::effectNull()-----------------------
	Objetivo : Tornar o valor das amostras delimitadas pela seleção nulo
	Recebe   : void
	Devolve  : void

	Goal       : Reset the sample values between the selection points
	Parameters : void
	Return	   : void
	Should Return : Error Code
	--------------------------------------------------------------------
*/
void MyCanvas::effectNull()
{
	wxString str;
	float	 start,end;

	if (m_track->m_startSample < m_track->m_endSample){
		start = m_track->m_startSample;
		end   = m_track->m_endSample;
	}
	else{
		start = m_track->m_endSample;
		end   = m_track->m_startSample;
	}
	

	if (selected){
		if (m_track->trackInfo.channels == 1){

			for (int i = (int) start; i < end; i++) m_track->trackFptr[i] = 0;
			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			
		}

		if (m_track->trackInfo.channels == 2){
					
			if (!showSeparate){
				for (int i = (int) start; i < end; i++){
						m_track->trackFptrLeft[i] = 0;
						m_track->trackFptrRight[i] = 0;
				}

				// interleaving
				int k;
				k = 0;

				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
					k++;
				}
				
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptr[rig] = m_track->trackFptrRight[k];
					k++;
				}
				//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint-2);
				drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
				
					
				
			}	
			else{
				
				if (selectedL){
					for (int i = (int)start; i < end; i++) m_track->trackFptrLeft[i] = 0;

					int k;
					k = 0;

					for (int lef = 0; lef < m_track->readcntf;lef +=2){
						m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
						k++;
					}
					
					k = 0;
					for (int rig = 1; rig < m_track->readcntf; rig +=2){
						m_track->trackFptr[rig] = m_track->trackFptrRight[k];
						k++;
					}
					drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
				}
				
				if (selectedR){
					for (int i = (int) start; i < end; i++) m_track->trackFptrRight[i] = 0;

					int k;
					k = 0;

					for (int lef = 0; lef < m_track->readcntf;lef +=2){
						m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
						k++;
					}
					
					k = 0;
					for (int rig = 1; rig < m_track->readcntf; rig +=2){
						m_track->trackFptr[rig] = m_track->trackFptrRight[k];
						k++;
					}
					drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
				}
				
				
				
			}
		}
	}
			
}


/*
	------------------void MyCanvas::effectReverseNull()--------------------
	Objetivo : Tornar o valor das amostras fora dos limites de seleção nulas
	Recebe   : void
	Devolve  : void

	Goal       : Reset the sample values outside the selection points
	Parameters : void
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------
*/
void MyCanvas::effectReverseNull()
{
	wxString str;
	int		 i;
	float	 start,end;

	if (m_track->m_startSample < m_track->m_endSample){
		start = m_track->m_startSample;
		end   = m_track->m_endSample;
	}
	else{
		start = m_track->m_endSample;
		end   = m_track->m_startSample;
	}

	if (selected){
		if (m_track->trackInfo.channels == 1){
			for (i = 0; i < start; i++) m_track->trackFptr[i] = 0;
			for (i = (int) end; i < m_track->trackInfo.frames; i++) m_track->trackFptr[i] = 0;
			//drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
			//drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint-1, m_track->m_endPoint+1);
			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
		}

		if (m_track->trackInfo.channels == 2){
			if (!showSeparate){
				for (i = 0; i < start; i++){
					m_track->trackFptrLeft[i] = 0;
					m_track->trackFptrRight[i] = 0;
				}	

				for (i = (int) end; i < m_track->trackInfo.frames; i++){
					m_track->trackFptrLeft[i] = 0;
					m_track->trackFptrRight[i] = 0;
				}
				// interleaving
				
				int k;
				k = 0;

				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
					k++;
				}
				
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptr[rig] = m_track->trackFptrRight[k];
					k++;
				}

				//drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
				//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
				drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			}
			else
				if (selectedL){
					for (i = 0; i < start; i++) m_track->trackFptrLeft[i] = 0;
					for (i = (int) end; i < m_track->trackInfo.frames; i++) m_track->trackFptrLeft[i] = 0;
						
					// interleaving
					int k;
					k = 0;

					for (int lef = 0; lef < m_track->readcntf;lef +=2){
						m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
						k++;
					}
					
					k = 0;
					for (int rig = 1; rig < m_track->readcntf; rig +=2){
						m_track->trackFptr[rig] = m_track->trackFptrRight[k];
						k++;
					}
					//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
					drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
				}

				if (selectedR){
					for (i = 0; i < start; i++) m_track->trackFptrRight[i] = 0;
						for (i = (int) end; i < m_track->trackInfo.frames; i++) m_track->trackFptrRight[i] = 0;
							
						// interleaving
						int k;
						k = 0;

						for (int lef = 0; lef < m_track->readcntf;lef +=2){
							m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
							k++;
						}
						
						k = 0;
						for (int rig = 1; rig < m_track->readcntf; rig +=2){
							m_track->trackFptr[rig] = m_track->trackFptrRight[k];
							k++;
						}
							drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
							//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
				}

	
		}
	

		//m_track->m_startSample = m_track->m_endSample = 0;
		//m_track->m_startPoint = m_track->m_endPoint = 0;
	}
	
}


/*
	------------------void MyCanvas::zoomHorIn()-------------------------------------------
	Objetivo : Essa função aumenta o retângulo onde a onda é desenhada, em conseqüência
			   a distância horizontal entre um ponto e seu consequente é aumentada 
			   permitindo assim uma visão mais detalhada do arquivo
	Recebe   : void
	Devolve  : void

	Goal       : This function raise the rectangule where the wave is drawn, in consequence
				 the distance between two consecutive points is raised and let us see the 
				 file in detail
	Parameters : void
	Return	   : void
	Should Return : Error Code
	---------------------------------------------------------------------------------------
*/
void MyCanvas::zoomHorIn()
{
	int		width,
			height;
	wxString	str;
	int		x,
			y;
	float		xf;
			

	/*
	GetViewStart(&x, &y);
	x = x*1.1;
	
	GetClientSize(&width,&height);
	m_track->ini_width = m_track->ini_width*1.1;
	m_track->zoomFactor = m_track->zoomFactor + 1;
	SetVirtualSize(m_track->ini_width, 0 );
	*/
	
	//m_track->m_startPoint = m_track->toPoint(m_track->m_startSample);
	//m_track->m_endPoint = m_track->toPoint(m_track->m_endSample);

	//m_track->m_startPoint = m_track->m_startPoint*1.1;
	//m_track->m_endPoint = m_track->m_endPoint*1.1;
	
	//m_track->m_startPoint = m_track->m_endPoint = -1;  // Para usar quando not selected

	if (!selected)
	{
		GetViewStart(&x, &y);
		xf = x*1.1;
		x = (int) xf;
		
		GetClientSize(&width,&height);

		xf = m_track->ini_width*1.1;
		m_track->ini_width = (int) xf;
		m_track->zoomFactor = m_track->zoomFactor + 1;
	
		SetVirtualSize(m_track->ini_width, 0 );

		m_track->m_startPoint = m_track->m_endPoint = -1;

		if (m_track->trackInfo.channels == 1) 
			drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		
		if (m_track->trackInfo.channels == 2)
			drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);

		Scroll( x ,0);	
	}
	else
	{
		GetViewStart(&x, &y);
		xf = x*1.1;
		x = (int) xf;

		GetClientSize(&width,&height);

		xf = m_track->ini_width*1.1;
		m_track->ini_width = (int) xf;
		m_track->zoomFactor = m_track->zoomFactor + 1;
	

		m_track->m_startPoint = (int) (m_track->m_startSample*m_track->ini_width / (m_track->trackInfo.frames));
		m_track->m_endPoint = (int) (m_track->m_endSample*m_track->ini_width / (m_track->trackInfo.frames));

		
		if (m_track->trackInfo.channels == 1) 
			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			//drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint+1, m_track->m_endPoint+1);

		
		if (m_track->trackInfo.channels == 2)
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
			//drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);


		SetVirtualSize(m_track->ini_width, 0 );
		Scroll( x ,0);	
		
	}
	


	/*
	if (m_track->trackInfo.channels == 1){
		if ( m_clipboard ) free( m_clipboard );
	}
	if (m_track->trackInfo.channels == 2){
		if ( m_clipboardLeft ) free( m_clipboardLeft );
		if ( m_clipboardRight ) free( m_clipboardRight );
	}
	*/
	m_owner->menuBar->Enable(ID_PASTE, false);
	m_owner->toolBar->EnableTool(ID_PASTE, false);
	//str.Printf("Zoom Factor: %d", m_track->ini_width);
	//m_owner->SetStatusText(str,2);
	//str.Printf()
}


/*
	------------------void MyCanvas::zoomHorOut()-------------------------------------------
	Objetivo : Essa função diminui o retângulo onde a onda é desenhada, em conseqüência
			   a distância horizontal entre um ponto e seu consequente é diminuida 
			   permitindo assim uma visão mais ampla do arquivo
	Recebe   : void
	Devolve  : void

	Goal       : This function decrease the rectangule where the wave is drawn, in consequence
				 the distance between two consecutive points is decreased and let us see the 
				 file in a wide way
	Parameters : void
	Return	   : void
	Should Return : Error Code
	---------------------------------------------------------------------------------------
*/
void MyCanvas::zoomHorOut()
{
	int		width, 
			height;
	int		x,
			y;
	float		xf;

	GetClientSize(&width,&height);

	
	//GetViewStart(&x, &y);
	//x = x*0.9;
	//m_track->ini_width = m_track->ini_width*0.9;
	//m_track->zoomFactor = m_track->zoomFactor - 1;

	//SetVirtualSize(m_track->ini_width, 0 );
	//m_track->m_startPoint = m_track->m_endPoint = 0;
	
	/*
	if (!selected){
		if (m_track->trackInfo.channels == 1)
			drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);

		if (m_track->trackInfo.channels == 2)
			drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
	}
	Scroll( x ,0);
	*/

	clearArea();

	if (!selected)
	{
		GetViewStart(&x, &y);
		xf = x*0.9;
		x = (int) xf;
		
		GetClientSize(&width,&height);

		xf = m_track->ini_width*0.9;
		m_track->ini_width = (int) xf;
		m_track->zoomFactor = m_track->zoomFactor - 1;
	
		SetVirtualSize(m_track->ini_width, 0 );

		m_track->m_startPoint = m_track->m_endPoint = -1;

		if (m_track->trackInfo.channels == 1) 
			drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		
		if (m_track->trackInfo.channels == 2)
			drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);

		Scroll( x ,0);	
	}
	else
	{
		
		GetViewStart(&x, &y);
		xf = x*0.9;
		x  = (int) xf;
	
		GetClientSize(&width,&height);

		xf = m_track->ini_width*0.9;
		m_track->ini_width = (int) xf;
		m_track->zoomFactor = m_track->zoomFactor - 1;
		
		SetVirtualSize(m_track->ini_width, 0 );

		m_track->m_startPoint = (int) (m_track->m_startSample*m_track->ini_width / (m_track->trackInfo.frames));
		m_track->m_endPoint = (int) (m_track->m_endSample*m_track->ini_width / (m_track->trackInfo.frames));

		
		if (m_track->trackInfo.channels == 1) 
			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			//drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint+1, m_track->m_endPoint+1);

		if (m_track->trackInfo.channels == 2)
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
			//drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);	
		
		//drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);



		SetVirtualSize(m_track->ini_width, 0 );
		Scroll( x ,0);	
	}
	
}

	
		

/*
	------------------void MyCanvas::ams()-------------------------------------------
	Objetivo : Ativar ou desativar a exibição do valor do ponto de amostras e do ponto 
			   correspondente na tela por onde o mouse passa
	Recebe   : void
	Devolve  : void

	Goal       : Enable or disable the exhibition of the sample point or the correspodent
				 screen phisycal point where the mouse moves over
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::ams()
{
	showAms = !showAms;
}


/*
	------------------void MyCanvas::temp()-----------------------------------------
	Objetivo : Ativar ou desativar a exibição do tempo total do arquivo e permitir a 
			   exibição de que ponto corresponde ao local onde o mouse esta passando
				
	Recebe   : void
	Devolve  : void

	Goal       : Enable or disable the exhibition of the total time and exhibits the
				 corresponding where the mouse moves over
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::temp()
{
	wxString	str;	
	
	showTemp = !showTemp;
	
	if (showTemp){
		str.Printf("%f\n", (float) m_track->trackInfo.frames/m_track->trackInfo.samplerate);
		m_owner->SetStatusText(str,2);
	}
	else
		m_owner->SetStatusText("",2);
}



	


/*
	------------------void MyCanvas::maxAmp()-------------------------------------
	Objetivo : Exibe através de uma janela a maior amplitude encontrada no arquivo 
	Recebe   : void
	Devolve  : void

	Goal       : Show the major amplitude in the file by a message window
				 corresponding where the mouse moves over
	Parameters : void
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------------
*/
void MyCanvas::maxAmp()
{
	float	maxAmp,maxAmpL,maxAmpR;
	wxString	str;
	int		i;

	maxAmp = maxAmpL = maxAmpR = 0;

	// Guardando no rascunho o conteudo de Fptr

	if (!selected)
	{
		/*
		for (i = 0; i < m_track->trackInfo.frames; i++) {
			if ( fabs(m_track->trackFptr[i])  > maxAmp) 
				maxAmp = fabs(m_track->trackFptr[i]);
		}
		*/


		if (m_track->trackInfo.channels == 1)
		{
	
			for (i = 0; i < (int) m_track->trackInfo.frames; i++) {
				if ( fabs(m_track->trackFptr[i])  > maxAmp) 
					maxAmp = fabs(m_track->trackFptr[i]);
			}
		}

		if (m_track->trackInfo.channels == 2)
		{
			for (i = 0; i < m_track->trackInfo.frames; i++) {
				if ( fabs(m_track->trackFptrLeft[i])  > maxAmpL) 
					maxAmpL = fabs(m_track->trackFptrLeft[i]);
			}

			for (i = 0; i < m_track->trackInfo.frames; i++) {
				if ( fabs(m_track->trackFptrRight[i])  > maxAmpR) 
					maxAmpR = fabs(m_track->trackFptrRight[i]);
			}
	
			if (maxAmpL > maxAmpR)
				maxAmp = maxAmpL;
			else
				maxAmp = maxAmpR;
		}


	}
	else
	{
		
		if (m_track->trackInfo.channels == 1)
		{
			for (i = (int) m_track->m_startSample; i < (int) m_track->m_endSample; i++) {
				if ( fabs(m_track->trackFptr[i])  > maxAmp) 
					maxAmp = fabs(m_track->trackFptr[i]);
			}
		}
		else
		{
			if (!showSeparate){
				for (i = (int) m_track->m_startSample; i < (int) m_track->m_endSample; i++) {
					if ( fabs(m_track->trackFptrLeft[i])  > maxAmpL) 
						maxAmpL = fabs(m_track->trackFptrLeft[i]);
				}

				for (i = (int) m_track->m_startSample; i < (int) m_track->m_endSample; i++) {
					if ( fabs(m_track->trackFptrRight[i])  > maxAmpR) 
						maxAmpR = fabs(m_track->trackFptrRight[i]);
				}

				if (maxAmpL > maxAmpR)
					maxAmp = maxAmpL;
				else
					maxAmp = maxAmpR;
			}
			else{
				if (selectedL){
					for (i = (int) m_track->m_startSample; i < (int) m_track->m_endSample; i++) {
						if ( fabs(m_track->trackFptrLeft[i])  > maxAmp) 
							maxAmp = fabs(m_track->trackFptrLeft[i]);
					}		
				}

				if (selectedR){
					for (i = (int) m_track->m_startSample; i < (int) m_track->m_endSample; i++) {
						if ( fabs(m_track->trackFptrRight[i])  > maxAmp) 
							maxAmp = fabs(m_track->trackFptrRight[i]);
					}		
				
				}
			

			}

		}

	}

	str.Printf("%f \n",maxAmp);
	wxMessageBox(str,"Maxima Amplitude", wxICON_INFORMATION | wxOK );

}

/*
	------------------void MyCanvas::separate()-------------------------------------
	Objetivo : Separa o tratamento dos arquivos Estéreo em dois canais 
	Recebe   : void
	Devolve  : void

	Goal       : Distinguish the treatment on the stereo files between two independet
				 channels
	Parameters : void
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------------
*/
void MyCanvas::separate()
{
	if (m_track->trackInfo.channels == 2){
		showSeparate = !showSeparate;
		selectedL = false;
		selectedR = false;
		drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
	}
	
	m_track->m_endPoint    = -1;  
	m_track->m_startPoint  = -1;
	m_track->m_endSample   = -1;
	m_track->m_startSample = -1;
	

}


void MyCanvas::gain()
{
	int i;
	

	wxString str;
	str = wxGetTextFromUser(_T("Set the linear gain"),
								_T("Gain:"),
								_T("1.0"),
								this,
								1, 1, TRUE);

	//if (str = "-1")
	float ganho = atof(str);

	
	//str.Printf("%f",ganho);
	//wxMessageBox(str,"Gain", wxICON_INFORMATION | wxOK );

	if (m_track->trackInfo.channels == 1){
		if ( (m_track->m_startSample !=0) & (m_track->m_endSample != 0) ){
			
			if (m_track->m_endSample > m_track->m_startSample){

				for (i = (int)m_track->m_startSample; i < m_track->m_endSample; i++){
	
					m_track->trackFptr[i] *= ganho;
					if ( m_track->trackFptr[i] > 1) m_track->trackFptr[i] = 0.99;
					if ( m_track->trackFptr[i] < -1 ) m_track->trackFptr[i] = -1;
				}
			}
			else{

				for (i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){

					m_track->trackFptr[i] *= ganho;
					if ( m_track->trackFptr[i] > 1)  m_track->trackFptr[i] = 0.99;
					if ( m_track->trackFptr[i] < -1 ) m_track->trackFptr[i] = -1;

				}
			}

			drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint-1, m_track->m_endPoint+1);
		}
	}

	if (m_track->trackInfo.channels == 2){
		if ( (m_track->m_startSample !=0) && (m_track->m_endSample != 0) ){
			
			if (! showSeparate){
				for (i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){

					m_track->trackFptrLeft[i] *= ganho;
					if ( m_track->trackFptrLeft[i] > 1)  m_track->trackFptrLeft[i] = 0.99;
					if ( m_track->trackFptrLeft[i] < -1 ) m_track->trackFptrLeft[i] = -1;

					m_track->trackFptrRight[i] *= ganho;
					if ( m_track->trackFptrRight[i] > 1)  m_track->trackFptrRight[i] = 0.99;
					if ( m_track->trackFptrRight[i] < -1 ) m_track->trackFptrRight[i] = -1;
				}	

				for (i = (int) m_track->m_endSample ; i < m_track->m_startSample; i++){
					m_track->trackFptrLeft[i] *= ganho;
					if ( m_track->trackFptrLeft[i] > 1)  m_track->trackFptrLeft[i] = 0.99;
					if ( m_track->trackFptrLeft[i] < -1 ) m_track->trackFptrLeft[i] = -1;
					
					m_track->trackFptrRight[i] *= ganho;
					if ( m_track->trackFptrRight[i] > 1)  m_track->trackFptrRight[i] = 0.99;
					if ( m_track->trackFptrRight[i] < -1 ) m_track->trackFptrRight[i] = -1;

				}

				// interleaving
				int k;
				k = 0;

				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
					k++;
				}
				
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptr[rig] = m_track->trackFptrRight[k];
					k++;
				}
				drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
			}
			else{
				if (selectedL){
					for (i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
						
						m_track->trackFptrLeft[i] *= ganho;
						if ( m_track->trackFptrLeft[i] > 1)  m_track->trackFptrLeft[i] = 0.99;
						if ( m_track->trackFptrLeft[i] < -1 ) m_track->trackFptrLeft[i] = -1;
					}
					for (i = (int) m_track->m_endSample ; i < m_track->m_startSample; i++){
						m_track->trackFptrLeft[i] *= ganho;
						if ( m_track->trackFptrLeft[i] > 1)  m_track->trackFptrLeft[i] = 0.99;
						if ( m_track->trackFptrLeft[i] < -1 ) m_track->trackFptrLeft[i] = -1;
					}
						
					// interleaving
					int k;
					k = 0;

					for (int lef = 0; lef < m_track->readcntf;lef +=2){
						m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
						k++;
					}
					
					k = 0;
					for (int rig = 1; rig < m_track->readcntf; rig +=2){
						m_track->trackFptr[rig] = m_track->trackFptrRight[k];
						k++;
					}
					drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
				}

				if (selectedR){
					for (i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
					
						m_track->trackFptrRight[i] *= ganho;
						if ( m_track->trackFptrRight[i] > 1)  m_track->trackFptrRight[i] = 0.99;
						if ( m_track->trackFptrRight[i] < -1 ) m_track->trackFptrRight[i] = -1;

					}
					for (i = (int) m_track->m_endSample ; i < m_track->m_startSample; i++){
						m_track->trackFptrRight[i] *= ganho;
						if ( m_track->trackFptrRight[i] > 1)  m_track->trackFptrRight[i] = 0.99;
						if ( m_track->trackFptrRight[i] < -1 ) m_track->trackFptrRight[i] = -1;
					}
					
					// interleaving
					int k;
					k = 0;

					for (int lef = 0; lef < m_track->readcntf;lef +=2){
						m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
						k++;
					}
										
					k = 0;
					for (int rig = 1; rig < m_track->readcntf; rig +=2){
						m_track->trackFptr[rig] = m_track->trackFptrRight[k];
						k++;
					}
					drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
				}
			}
		}
	}

	//m_track->m_startSample = m_track->m_endSample = 0;
	//m_track->m_startPoint = m_track->m_endPoint = 0;
	//for (int i = 0; i < m_track->readcntf; i++) m_track->trackFptr[i] *= ganho;
	//drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);

}


/*
	------------------void MyCanvas::fadeIn()-------------------------------------
	Objetivo : Aumenta gradativamente o valor das amostras
	Recebe   : void
	Devolve  : void

	Goal       : Increases smoothly the samples values 
	Parameters : void
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------------
*/
void MyCanvas::fadeIn()
{
	wxString str;
	int		 k = 0;


	if ( m_track->trackInfo.channels == 1 ){
		float	y_max,y_min;
		float	a,b;

		m_walkGraphic->getMaxYPoint(&y_max);
		m_walkGraphic->getMinYPoint(&y_min);

		
		if (m_track->m_endSample > m_track->m_startSample){
			a =  (float) abs ((int)y_max) / (m_track->m_endSample - m_track->m_startSample);
			b = 0;
			for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
				m_track->trackFptr[i] = (float) m_track->trackFptr[i]*(a*k + b);
				k++;
			}
		}
		else{
			a =  (float) abs((int)y_max)/ (m_track->m_startSample - m_track->m_endSample) ;
			b = 0;
			for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
				m_track->trackFptr[i] = (float) m_track->trackFptr[i]*(a*k + b) ;
				k++;
			}
		}
		//drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		//drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint-1, m_track->m_endPoint+1);
		drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
		

	}

	if ( m_track->trackInfo.channels == 2 ){
		float	y_maxLeft,y_maxRight;
		float	y_minLeft,y_minRight;
		float	aLeft,bLeft, aRight,bRight;
	
		m_walkGraphicLeft->getMaxYPoint(&y_maxLeft);
		m_walkGraphicRight->getMaxYPoint(&y_maxRight);

		m_walkGraphicLeft->getMinYPoint(&y_minLeft);
		m_walkGraphicRight->getMinYPoint(&y_minRight);

		if (!showSeparate){
			if ( m_track->m_endSample > m_track->m_startSample ){
			
				aLeft = (float) abs ( (int) y_maxLeft) / (m_track->m_endSample - m_track->m_startSample);
				bLeft = 0;

				aRight = (float) abs( (int) y_maxRight) / (m_track->m_endSample - m_track->m_startSample);
				bRight = 0;
				k = 0;
				for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
					m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
					m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
					k++;
				}
			}
			else{
				aLeft = (int) ((float) y_maxLeft / (m_track->m_startSample - m_track->m_endSample));
				bLeft = 0; 

				aRight =(int) ((float) y_maxRight / (m_track->m_startSample - m_track->m_endSample));
				bRight = 0;

				k = 0;
				for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
					m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
					m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
					k++;
				}
			}
			// interleaving
			k = 0;
			for (int lef = 0; lef < m_track->readcntf;lef +=2){
				m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
				k++;
			}
			
			k = 0;
			for (int rig = 1; rig < m_track->readcntf; rig +=2){
				m_track->trackFptr[rig] = m_track->trackFptrRight[k];
				k++;
			}
			//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
		}
		else{
			if (selectedL){
				if ( m_track->m_endSample > m_track->m_startSample ){
			
					aLeft = (float) abs ( (int) y_maxLeft) / (m_track->m_endSample - m_track->m_startSample);
					bLeft = 0;
					k = 0;
					for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
						m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
						k++;
					}
				}
				else{
					aLeft = (float) y_maxLeft / (m_track->m_startSample - m_track->m_endSample);
					bLeft = 0; 

					k = 0;
					for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
						m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
						k++;
					}
				}
				// interleaving
				k = 0;
				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
					k++;
				}
				
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptr[rig] = m_track->trackFptrRight[k];
					k++;
				}
				//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
				drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			}

			if (selectedR){
				if ( m_track->m_endSample > m_track->m_startSample ){
					aRight = (float) abs((int)y_maxRight) / (m_track->m_endSample - m_track->m_startSample);
					bRight = 0;
					k = 0;
					for (int i = (int)m_track->m_startSample; i < m_track->m_endSample; i++){
						m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
						k++;
					}
				}
				else{
					aRight = (float) y_maxRight / (m_track->m_startSample - m_track->m_endSample);
					bRight = 0;

					k = 0;
					for (int i = (int)m_track->m_endSample; i < m_track->m_startSample; i++){
						m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
						k++;
					}
				}
				// interleaving
				k = 0;
				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
					k++;
				}
				
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptr[rig] = m_track->trackFptrRight[k];
					k++;
				}
				drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
				//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
			}
		} 

	}

}


/*
	------------------void MyCanvas::fadeOut()-------------------------------------
	Objetivo : Diminui gradativamente o valor das amostras
	Recebe   : void
	Devolve  : void

	Goal       : Decreases smoothly the samples values 
	Parameters : void
	Return	   : void
	Should Return : Error Code
	------------------------------------------------------------------------------
*/
void MyCanvas::fadeOut()
{
	wxString str;
	float	 start,end;

	if (m_track->m_startSample < m_track->m_endSample){
		start = m_track->m_startSample;
		end   = m_track->m_endSample;
	}
	else{
		start = m_track->m_endSample;
		end   = m_track->m_startSample;
	}
	
	int		k = 0;
	
	
	if ( m_track->trackInfo.channels == 1 ){
		float	y_max,y_min;
		float	a,b;

		m_walkGraphic->getMaxYPoint(&y_max);
		m_walkGraphic->getMinYPoint(&y_min);

		a = (float) y_max / (m_track->m_endSample - m_track->m_startSample);
		b = abs ( (int) y_min);
	

		if (m_track->m_endSample > m_track->m_startSample){
		
			a = (float) y_max / (m_track->m_endSample - m_track->m_startSample);
			b = abs ( (int)y_min);
	
			
			k = 0;
			for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
				m_track->trackFptr[i] = (float) m_track->trackFptr[i]*(-a*k + b) ;
				k++;
			}
		}
		else{

			a = (float) y_max / (m_track->m_startSample - m_track->m_endSample);
			b = abs ( (int) y_min);
	
			k = 0;
			for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
				m_track->trackFptr[i] = (float) m_track->trackFptr[i]*(-a*k + b) ;
				k++;
			}
		}
		//drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		//drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint-1, m_track->m_endPoint+1);
		drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);

	}

	if ( m_track->trackInfo.channels == 2 ){
		float	y_maxLeft,y_maxRight;
		float	y_minLeft,y_minRight;
		float	aLeft,bLeft, aRight,bRight;
	
		m_walkGraphicLeft->getMaxYPoint(&y_maxLeft);
		m_walkGraphicRight->getMaxYPoint(&y_maxRight);

		m_walkGraphicLeft->getMinYPoint(&y_minLeft);
		m_walkGraphicRight->getMinYPoint(&y_minRight);


		if (!showSeparate){

			if ( m_track->m_endSample > m_track->m_startSample ){
			
				aLeft = (float) y_maxLeft / (m_track->m_endSample - m_track->m_startSample);
				bLeft = abs ( (int) y_minLeft ); 

				aRight = (float) y_maxRight / (m_track->m_endSample - m_track->m_startSample);
				bRight = abs ( (int) y_minRight );

				for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
					m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
					m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
					k++;
				}
			}
			else{
				aLeft = (float) y_maxLeft / (m_track->m_startSample - m_track->m_endSample);
				bLeft = abs ( (int) y_minLeft ); 

				aRight = (float) y_maxRight / (m_track->m_startSample - m_track->m_endSample);
				bRight = abs ( (int) y_minRight );

				for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
					m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
					m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
					k++;
				}
			}

			// interleaving
			int k;
			k = 0;

			for (int lef = 0; lef < m_track->readcntf;lef +=2){
				m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
				k++;
			}
			
			k = 0;
			for (int rig = 1; rig < m_track->readcntf; rig +=2){
				m_track->trackFptr[rig] = m_track->trackFptrRight[k];
				k++;
			}
			
			//m_track->m_startSample = m_track->m_endSample = 0;
			//m_track->m_startPoint = m_track->m_endPoint = 0;

			//drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
		}
		else{
			if (selectedL){
					if ( m_track->m_endSample > m_track->m_startSample ){
					
						aLeft = (float) y_maxLeft / (m_track->m_endSample - m_track->m_startSample);
						bLeft = abs ( (int) y_minLeft ); 

						for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
							m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
							k++;
						}
					}
					else{
						aLeft = (float) y_maxLeft / (m_track->m_startSample - m_track->m_endSample);
						bLeft = abs ( (int) y_minLeft ); 

						for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
							m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
							k++;
						}
					}

					// interleaving
					int k;
					k = 0;

					for (int lef = 0; lef < m_track->readcntf;lef +=2){
						m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
						k++;
					}
					
					k = 0;
					for (int rig = 1; rig < m_track->readcntf; rig +=2){
						m_track->trackFptr[rig] = m_track->trackFptrRight[k];
						k++;
					}
					drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
			}

			if (selectedR){
			
					if ( m_track->m_endSample > m_track->m_startSample ){
					
						aRight = (float) y_maxRight / (m_track->m_endSample - m_track->m_startSample);
						bRight = abs ( (int) y_minRight );

						for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
							m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
							k++;
						}
					}
					else{
						aLeft = (float) y_maxLeft / (m_track->m_startSample - m_track->m_endSample);
						bLeft = abs ( (int) y_minLeft ); 

						aRight = (float) y_maxRight / (m_track->m_startSample - m_track->m_endSample);
						bRight = abs ( (int) y_minRight );

						for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
							m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
							k++;
						}
					}

					// interleaving
					int k;
					k = 0;

					for (int lef = 0; lef < m_track->readcntf;lef +=2){
						m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
						k++;
					}
					
					k = 0;
					for (int rig = 1; rig < m_track->readcntf; rig +=2){
						m_track->trackFptr[rig] = m_track->trackFptrRight[k];
						k++;
					}

					drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
			}

		}
	}
}


/*
	------------------void MyCanvas::crossOver()---------------------------------------
	Objetivo : Faz a diminuição e posteriormente o aumento gradativos do trecho marcado
	Recebe   : void
	Devolve  : void

	Goal       : Decrease and increase smoothly the samples values of the selected slice
	Parameters : void
	Return	   : void
	Should Return : Error Code
	-----------------------------------------------------------------------------------
*/
void MyCanvas::crossOver()
{
	wxString str;
	int		 k = 0;
	
	
	if ( m_track->trackInfo.channels == 1 ){
		float	y_max,y_min;
		float	a,b;

		m_walkGraphic->getMaxYPoint(&y_max);
		m_walkGraphic->getMinYPoint(&y_min);

		if (m_track->m_endSample > m_track->m_startSample){
		
			a = (float) abs ( (int) (y_max - y_min)) / (m_track->m_endSample - m_track->m_startSample);
			b = abs( (int) y_min);
			
			k = 0;
			for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
				m_track->trackFptr[i] = (float) m_track->trackFptr[i]*(-a*k + b) ;
				k++;
			}
		}
		else{

			a = (float) abs ( (int) (y_max - y_min)) / (m_track->m_endSample - m_track->m_startSample);
			b = y_min;
	
			k = 0;
			for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
				m_track->trackFptr[i] = (float) m_track->trackFptr[i]*(-a*k + b) ;
				k++;
			}
		}
		//drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		//drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint-1, m_track->m_endPoint+1);
		drawOpenedFileSelect(m_track->trackFptr,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
	}

	if ( m_track->trackInfo.channels == 2 ){
		float	y_maxLeft,y_maxRight;
		float	y_minLeft,y_minRight;
		float	aLeft,bLeft, aRight,bRight;
	
		m_walkGraphicLeft->getMaxYPoint(&y_maxLeft);
		m_walkGraphicRight->getMaxYPoint(&y_maxRight);

		m_walkGraphicLeft->getMinYPoint(&y_minLeft);
		m_walkGraphicRight->getMinYPoint(&y_minRight);

		if (!showSeparate){
			if ( m_track->m_endSample > m_track->m_startSample ){
			
				aLeft = (float) abs((int) (y_maxLeft - y_minLeft)) / (m_track->m_endSample - m_track->m_startSample);
				bLeft = abs ( (int) (y_minLeft) ); 

				aRight = (float) abs( (int) (y_maxRight - y_minRight)) / (m_track->m_endSample - m_track->m_startSample);
				bRight = abs ( (int) (y_minRight) );

				for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
					m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
					m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
					k++;
				}
			}
			else{
				aLeft = (float) abs( (int) (y_maxLeft - y_minLeft)) / (m_track->m_startSample - m_track->m_endSample);
				bLeft = abs ((int) (y_minLeft)); 

				aRight = (float) abs((int) (y_maxLeft - y_minLeft)) / (m_track->m_startSample - m_track->m_endSample);
				bRight = abs( (int) y_minRight);

				for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
					m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
					m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
					k++;
				}
			}

			// interleaving
			int k;
			k = 0;

			for (int lef = 0; lef < m_track->readcntf;lef +=2){
				m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
				k++;
			}
			
			k = 0;
			for (int rig = 1; rig < m_track->readcntf; rig +=2){
				m_track->trackFptr[rig] = m_track->trackFptrRight[k];
				k++;
			}
			drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
		}
		else{

			if (selectedL){
				if ( m_track->m_endSample > m_track->m_startSample ){
			
					aLeft = (float) abs( (int) (y_maxLeft - y_minLeft)) / (m_track->m_endSample - m_track->m_startSample);
					bLeft = abs ( (int) y_minLeft ); 

					for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
						m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
						k++;
					}
				}
				else{
					aLeft = (float) abs( (int) (y_maxLeft - y_minLeft)) / (m_track->m_startSample - m_track->m_endSample);
					bLeft = abs ( (int) y_minLeft); 

					for (int i = (int) m_track->m_endSample; i < m_track->m_startSample; i++){
						m_track->trackFptrLeft[i] = (float) m_track->trackFptrLeft[i]*(-aLeft*k + bLeft) ;
						k++;
					}
				}

				// interleaving
				int k;
				k = 0;

				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
					k++;
				}
				
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptr[rig] = m_track->trackFptrRight[k];
					k++;
				}
				//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
				drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
				
			}

			if (selectedR){
				if ( m_track->m_endSample > m_track->m_startSample ){
				
					aRight = (float) abs( (int) (y_maxRight - y_minRight)) / (m_track->m_endSample - m_track->m_startSample);
					bRight = abs ( (int) y_minRight );

					for (int i = (int) m_track->m_startSample; i < m_track->m_endSample; i++){
						m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
						k++;
					}
				}
				else{
					aRight = (float) abs( (int) (y_maxLeft - y_minLeft)) / (m_track->m_startSample - m_track->m_endSample);
					bRight = abs((int) y_minRight);

					for (int i = (int)  m_track->m_endSample; i < m_track->m_startSample; i++){
						m_track->trackFptrRight[i] = (float) m_track->trackFptrRight[i]*(-aRight*k + bRight) ;
						k++;
					}
				}

				// interleaving
				int k;
				k = 0;

				for (int lef = 0; lef < m_track->readcntf;lef +=2){
					m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
					k++;
				}
				
				k = 0;
				for (int rig = 1; rig < m_track->readcntf; rig +=2){
					m_track->trackFptr[rig] = m_track->trackFptrRight[k];
					k++;
				}
				//drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint+2, m_track->m_endPoint);
				drawOpenedFileSelectStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames, m_track->m_startPoint, m_track->m_endPoint);
			}
		}
		
	}
}




/*
	------------------void MyCanvas::toStereo()---------
	Objetivo : Transforma o arquivo de mono para estéreo
	Recebe   : void
	Devolve  : void

	Goal       : Changes the file from mono to stereo
	Parameters : void
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------
*/
void MyCanvas::toStereo()
{
	if ( (m_owner->fileOpened) && (m_track->trackInfo.channels == 1)) {

		float   *tempFptrLeft,*tempFptrRight;		
		int	i;
		SF_INFO info;
		
		float	tempRead =  m_track->readcntf*2; 

		m_walkGraphicLeft  = new WalkGraphic( m_track->trackInfo.frames );
		m_walkGraphicRight = new WalkGraphic( m_track->trackInfo.frames );
	
		
		if ((tempFptrLeft = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		if ((tempFptrRight = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		
		for (i = 0; i < m_track->trackInfo.frames; i++){
			tempFptrLeft[i] = m_track->trackFptr[i];
			tempFptrRight[i] = m_track->trackFptr[i];
		}
		
		info = m_track->trackInfo;
		info.channels = 2;
		

		free(m_track);
		m_track = new Track();
		m_track->readcntf = (sf_count_t) tempRead;
		m_track->trackInfo = info;
		
		if (( m_track->trackFptrLeft = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}

		if (( m_track->trackFptrRight = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		
		m_track->trackFptrLeft = tempFptrLeft;
		m_track->trackFptrRight = tempFptrRight;

		
		free(m_track->trackFptr);
		if (( m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames)*(m_track->trackInfo.channels)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}

		// Interleaving
		int k;
		k = 0;

		
		for (int lef = 0; lef < m_track->readcntf;lef +=2){
			m_track->trackFptr[lef] = m_track->trackFptrLeft[k];
			k++;
		}
		
		k = 0;
		for (int rig = 1; rig < m_track->readcntf; rig +=2){
			m_track->trackFptr[rig] = m_track->trackFptrRight[k];
			k++;
		}

		clearArea();
		
		drawOpenedFileStereo(m_track->trackFptrLeft,m_track->trackFptrRight,m_track->trackInfo.frames);

		m_owner->menuBar->Enable(ID_TO_STEREO, false);
		m_owner->menuBar->Enable(ID_TO_MONO, true);
		m_owner->menuBar->Enable(ID_SEPARATE, true);
	}
	else

		wxMessageBox("Arquivo ja eh Stereo\n","To Stereo", wxICON_INFORMATION | wxOK );
}


/*
	------------------void MyCanvas::toMono()-----------
	Objetivo : Transforma o arquivo de estéreo para mono
	Recebe   : void
	Devolve  : void

	Goal       : Changes the file from stereo to mono
	Parameters : void
	Return	   : void
	Should Return : Error Code
	----------------------------------------------------
*/
void MyCanvas::toMono()
{
	if ( (m_owner->fileOpened) && (m_track->trackInfo.channels == 2)) {

		int		i;
		float   *tempFptr;
		SF_INFO info;
		
		//float	tempRead =  m_track->readcntf/2; 
		float	tempRead =  m_track->trackInfo.frames; 

		m_walkGraphic = new WalkGraphic( m_track->trackInfo.frames );

		if (( tempFptr = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}

		for (i = 0; i < m_track->trackInfo.frames; i++)
			tempFptr[i] = (m_track->trackFptrLeft[i] + m_track->trackFptrRight[i])/2;
			//tempFptr[i] = (m_track->trackFptrLeft[i] + m_track->trackFptrRight[i]);
			
			
			
			
		info = m_track->trackInfo;
		info.channels = 1;
	
		clearArea();

		free(m_track);
		m_track = new Track();
		m_track->readcntf = (sf_count_t) tempRead;
		m_track->trackInfo = info;
		
		if (( m_track->trackFptr = (float*)malloc((m_track->trackInfo.frames)*sizeof(float))) == NULL ){
			wxMessageBox("MEM ERROR\n","Alocating Memory", wxICON_INFORMATION | wxOK );
			exit(1);
		}
		
		m_track->trackFptr = tempFptr;
		//for (i = 0; i < m_track->trackInfo.frames; i++)
		//	m_track->trackFptr[i] = tempFptr[i];
		
		drawOpenedFile(m_track->trackFptr,m_track->trackInfo.frames);
		
		m_owner->menuBar->Enable(ID_TO_STEREO, true);
		m_owner->menuBar->Enable(ID_TO_MONO, false);
	}
	else
		wxMessageBox("Arquivo ja eh Mono\n","To Mono", wxICON_INFORMATION | wxOK );
	//wxMessageBox("To Mono\n","To Mono", wxICON_INFORMATION | wxOK );

	selected = false;
}



/*
	------------------void MyCanvas::celarArea()-------------------------------------------
	Objetivo : Limpa o conteúdo desenhado na tela e disponibiliza um bitmap limpo
	Recebe   : void
	Devolve  : void

	Goal       : Clear the bitmap displayed on the screen
	Parameters : void
	Return	   : void
	Should Return : Error Code
	---------------------------------------------------------------------------------------
*/
void MyCanvas::clearArea()
{
	wxString str;
	int width,height;
	wxColour colour;
	wxPen pen;
	wxBrush brush;

	GetClientSize(&width,&height);
	m_width  = width;
	m_height = height;

	if(m_buffer) delete m_buffer;
	m_buffer = new wxBitmap(m_width,m_height);

	if (m_track->trackInfo.channels == 1 )
	{
		m_track->trackArea = m_walkGraphic->setArea(10,10,m_width-10,m_height-10);

		m_bufferDC.SelectObject(*m_buffer);
		m_bufferDC.Clear();

		m_walkGraphic->setDC(&m_bufferDC);
		m_walkGraphic->setGridBackgroundColor(wxColour(192, 192, 192));
		m_walkGraphic->setCurveColor(wxColour(50,  50, 200));

		// --- clean area ---
		//colour.Set(0xA0,0xA0,0xA0); // backGround colour
		colour.Set(172, 168, 153); // backGround colour
		
		pen.SetColour(colour);
		brush.SetColour(colour);
		m_bufferDC.SetPen(pen);
		m_bufferDC.SetBrush(brush);
		m_bufferDC.DrawRectangle(0,0,m_width,m_height);
	}


	if (m_track->trackInfo.channels == 2 )
	{
		//m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,m_height-10);
		//m_track->trackAreaRight = m_walkGraphicRight->setArea(10,10,m_width-10,m_height-10);
		
		m_track->trackAreaLeft = m_walkGraphicLeft->setArea(10,10,m_width-10,m_height-10);
		m_track->trackAreaRight = m_walkGraphicRight->setArea(10,m_height+10,m_width-10,m_height+10);  

		m_bufferDC.SelectObject(*m_buffer);
		m_bufferDC.Clear();

		m_walkGraphicLeft->setDC(&m_bufferDC);
		m_walkGraphicRight->setDC(&m_bufferDC);

		m_walkGraphicLeft->setGridBackgroundColor(wxColour(192, 192, 192));
		m_walkGraphicLeft->setCurveColor(wxColour(50,  50, 200));

		m_walkGraphicRight->setGridBackgroundColor(wxColour(192, 192, 192));
		m_walkGraphicRight->setCurveColor(wxColour(50,  50, 200));

		// --- clean area ---
		//colour.Set(0xA0,0xA0,0xA0); // backGround colour
		colour.Set(172, 168, 153); // backGround colour
		pen.SetColour(colour);
		brush.SetColour(colour);
		m_bufferDC.SetPen(pen);
		m_bufferDC.SetBrush(brush);
		m_bufferDC.DrawRectangle(0,0,m_width,m_height);
	}


	m_bufferDC.SelectObject( wxNullBitmap ); // necessary
	wxClientDC screen_dc(this);
	screen_dc.DrawBitmap(*m_buffer,0,0);

	// Text in the bottom most
	//str.Printf("Sample Rate: %d",m_track->trackInfo.samplerate);
	//m_owner->SetStatusText(str,1);

	//str.Printf("Length: %.2f",(float)m_track->trackInfo.frames/m_track->trackInfo.samplerate);
	//m_owner->SetStatusText(str,2);

	//str.Printf("Frames: %d",m_track->trackInfo.frames);
	//m_owner->SetStatusText(str,3);
}


inline float scale(mad_fixed_t sample)
{
   return (float) (sample / (float) (1L << MAD_F_FRACBITS));
}

/*
inline signed int scale(mad_fixed_t sample)
{
   return (signed int) (sample / (signed int) (1L << MAD_F_FRACBITS));
}
*/


//int decode(struct private_data mPrivateData, const char *filename, float *sample,     SF_INFO info)
int decode(const char *filename, SF_INFO &info)
{
        struct private_data				mPrivateData;
        mad_decoder                     mDecoder;
        //int                           chn;
        SNDFILE                         *outfile;
        //SF_INFO                       info;

        mPrivateData.len         = 0;
        mPrivateData.file        = fopen(filename,"rb");
        mPrivateData.inputBuffer = new unsigned char [INPUT_BUFFER_SIZE];
        mPrivateData.numChannels = 0;
        mPrivateData.samplerate  = 0;
        mPrivateData.icount		 = 0;
		mPrivateData.ocount		 = 0;
        mPrivateData.firstRun	 = true;

        mad_decoder_init(&mDecoder, &mPrivateData, input_cb, 0, 0, output_cb, error_cb, 0);

        if(mad_decoder_run(&mDecoder, MAD_DECODER_MODE_SYNC) == 0)
        {
                /* success */
                //wxMessageBox("Success\n");

                info.samplerate = mPrivateData.samplerate;
                info.channels   = mPrivateData.numChannels;
				info.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
                info.sections   = 1;
                info.seekable   = 1;
                info.frames     = 0;

                outfile = sf_open("outstep.wav" , SFM_WRITE , &info);
                sf_write_float(outfile, mPrivateData.sample , mPrivateData.len) ;
				//sf_write_int(outfile, mPrivateData.sample , mPrivateData.len) ;
                sf_close(outfile);

                // Se tiver successo copio de um ponteiro para o outro

                //sample = mPrivateData.sample;

                mad_decoder_finish(&mDecoder);



    }
        else {
                /* failure */
                //wxMessageBox("Failure\n");
            mad_decoder_finish(&mDecoder);
   }

        /* delete everything */
        //printf("Input %d   Output %d\n",mPrivateData.icount, mPrivateData.ocount);

    delete mPrivateData.inputBuffer;
        fclose(mPrivateData.file);

        return(0);
}


//
//   MAD Callbacks
//

/* The input callback is called when the decoder wants more data. */
enum mad_flow input_cb(void *_data, struct mad_stream *stream)
{
   struct private_data *data = (struct private_data *)_data;

    data->icount += 1;

   if(feof(data->file))
      return MAD_FLOW_STOP;


   /* "Each time you refill your buffer, you need to preserve the data in
    *  your existing buffer from stream.next_frame to the end.
    *
    *  This usually amounts to calling memmove() on this unconsumed portion
    *  of the buffer and appending new data after it, before calling
    *  mad_stream_buffer()"
    *           -- Rob Leslie, on the mad-dev mailing list */

   unsigned int unconsumedBytes;
   if(stream->next_frame) {
      unconsumedBytes = data->inputBuffer + INPUT_BUFFER_SIZE - stream->next_frame;
      memmove(data->inputBuffer, stream->next_frame, unconsumedBytes);
   }
   else
      unconsumedBytes = 0;

   int read = fread(data->inputBuffer + unconsumedBytes,
                                        1,
                                        INPUT_BUFFER_SIZE - unconsumedBytes,
                                        data->file);

   mad_stream_buffer(stream, data->inputBuffer, read + unconsumedBytes);
}                                                                               



/* The output callback is called every time the decoder has finished decoding
 * a frame, allowing us to use the decoded data */
enum mad_flow output_cb(void *_data,
                        struct mad_header const *header,
                        struct mad_pcm *pcm)
{

        int             channels,
                        samplerate;
		float			samples;
        struct  private_data *data = (struct private_data *)_data;
        int             smpl,
                        cont;
        float			*bufferL,
                        *bufferR;
		//signed int		*bufferL,
        //                  *bufferR;



        samplerate= pcm->samplerate;
        channels  = pcm->channels;
        samples   = pcm->length;

    if (data->firstRun){
                data->numChannels = channels;
                data->samplerate  = samplerate;
                data->firstRun = false;
    }



    if(data->ocount == 0)
			    //data->sample = (signed int *) malloc(channels*samples*sizeof(signed int) );
				data->sample = (float *) malloc(channels*samples*sizeof(float) );
    else
                //data->sample = (signed int *) realloc (data->sample , (data->len + samples)*sizeof(signed int)*channels );
				data->sample = (float *) realloc (data->sample , (data->len + samples)*sizeof(float)*channels );

        data->ocount += 1;

        if (!data->sample){
                //printf("NAo alocou memo\n");
                exit(1);
        }

        //signed int *buffer = new int[channels*samples];
		float *buffer = new float[channels*samples];

        if (channels == 1){
                for(smpl = 0; smpl < samples; smpl++)
                         buffer[smpl] = scale(pcm->samples[0][smpl]);
						 //buffer[smpl] = 32767*scale(pcm->samples[0][smpl]);
        }

        if (channels == 2){
                bufferL = new float[samples];
                bufferR = new float[samples];

                for(smpl = 0; smpl < samples; smpl++){
                         bufferL[smpl] = scale(pcm->samples[0][smpl]);
                         bufferR[smpl] = scale(pcm->samples[1][smpl]);
                }

                //Interleaving
                cont = 0;
                while (cont < samples){
                        buffer[cont] = bufferL[cont];
                        cont++;
                        buffer[cont] = bufferR[cont];
                        cont++;
                }
        }


        memcpy(data->sample + data->len ,buffer ,channels*samples);
        data->len +=channels*samples;


        if (channels == 2){
                delete [] bufferL;
                delete [] bufferR;
        }
        delete [] buffer;

   return MAD_FLOW_CONTINUE;
}


enum mad_flow error_cb(void *_data, struct mad_stream *stream,
                       struct mad_frame *frame)
{
   return MAD_FLOW_CONTINUE;
}




























