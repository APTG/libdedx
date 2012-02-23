#include "target.h"
#include "dedx.h"
#include <wx/wx.h>
#include <wx/combobox.h>

DropDownTarget::DropDownTarget(wxWindow* parent):wxComboBox(parent,-1, _("Pick Target"))
{

	this->Append( _("Water") );
}
