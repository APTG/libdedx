#ifndef DEDXGUI_MAINFRAME_INCLUDED
#define DEDXGUI_MAINFRAME_INCLUDED

#include <wx/wx.h>
 
class MainFrame: public wxFrame
{
public:

    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
