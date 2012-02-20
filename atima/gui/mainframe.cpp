#include "mainframe.h"
#include "mainMenu.h"
#include "dropdown/prog.h"
#include "dropdown/target.h"


BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(ID_Quit, MainFrame::OnQuit)
	EVT_MENU(ID_About, MainFrame::OnAbout)
END_EVENT_TABLE()

	MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame( NULL, -1, title, pos, size )
{
	SetMenuBar( new MainMenu() );
	wxPanel *panel = new wxPanel(this,-1);

	DropDownProgram* comboBox = new DropDownProgram(panel); 
	CreateStatusBar();
	SetStatusText( _("dEdx running") );
}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox( _("Welcome to dEdx a frontend for libdEdx \nlibdEdx was written by Jakob Toftegaard and Niels Bassler at the Dept. of Physics and Astronomy and Dept. of Experimental Clinical Oncology with support from Armin Lühr, Helge Knudsen and David C. Hansen.\nWe do not claim any correctness of the produced results, any use of these data are at own risk."),
			_("About dEdx"),
			wxOK | wxICON_INFORMATION, this);
}
