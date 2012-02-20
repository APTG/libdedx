#ifndef DEDXGUI_DROPDOWN_TARGET_INCLUDED
#define DEDXGUI_DROPDOWN_TARGET_INCLUDED

#include <wx/wx.h>
#include <wx/combobox.h>
class DropDownTarget: public wxComboBox
{
public:
		DropDownTarget(wxWindow* parent);
};
#endif
