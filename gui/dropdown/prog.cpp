#include "prog.h"
extern "C"{
#include <dedx.h>
}
#include <wx/wx.h>
#include <wx/combobox.h>
#include <sstream>

DropDownProgram::DropDownProgram(wxWindow* parent):wxComboBox(parent,-1, _("Pick Target"))
{
	const int * prog_id;
	prog_id = dedx_get_program_list();
	int i = 0;
	while(prog_id[i] != -1)
	{
		wxString text = wxString() << prog_id[i];
		text << _(":\t\t");
		const char * name = dedx_get_program_name(prog_id[i]);
		wxString n = wxString(name,wxConvUTF8);
		text << n;
		this->Append(text);
		i++;
	}
}
