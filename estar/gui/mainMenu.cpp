#include "mainMenu.h"
#include <wx/wx.h>
MainMenu::MainMenu():wxMenuBar()
{
	wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_About, _("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("E&xit") );


    this->Append( menuFile, _("&File") );
}
