#include <cstdlib>
#include <wx/string.h>
#include <wx/wx.h>

class MyApp : public wxAppConsole
{
public:
    bool OnInit() override;
};

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxString name("John");

    wxPrintf("Hello, %s!\n", name);

    exit(0);
}
