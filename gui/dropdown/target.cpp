#include "target.h"

#include <wx/combobox.h>
#include <wx/wx.h>

#include "dedx.h"

DropDownTarget::DropDownTarget(wxWindow *parent) : wxComboBox(parent, -1, _("Pick Target")) {

    this->Append(_("Water"));
}
