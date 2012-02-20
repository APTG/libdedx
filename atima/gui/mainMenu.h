#ifndef DEDXGUI_MAINMENU_INCLUDED
#define DEDXGUI_MAINMENU_INCLUDED
#include <wx/wx.h>

enum
{
    ID_Quit = 1,
    ID_About,
};

class MainMenu: public wxMenuBar
{
	public:
		MainMenu();
};
#endif
