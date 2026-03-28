#ifndef DEDXGUI_DROPDOWN_PROGRAM_INCLUDED
#define DEDXGUI_DROPDOWN_PROGRAM_INCLUDED

#include <wx/combobox.h>
#include <wx/wx.h>
class DropDownProgram : public wxComboBox {
  public:
    DropDownProgram(wxWindow *parent);
};
#endif
