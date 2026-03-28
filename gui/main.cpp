/*
 * hworld.cpp
 */

#include "mainframe.h"
#include "wx/wx.h"

class MyApp : public wxApp {
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    MainFrame *frame = new MainFrame(_("dEdx"), wxPoint(200, 200), wxSize(450, 340));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}
