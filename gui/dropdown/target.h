#ifndef DEDXGUI_DROPDOWN_TARGET_INCLUDED
#define DEDXGUI_DROPDOWN_TARGET_INCLUDED

#include <wx/combobox.h>
#include <wx/wx.h>
class DropDownTarget : public wxComboBox {
  public:
    DropDownTarget(wxWindow *parent);
};
#endif
