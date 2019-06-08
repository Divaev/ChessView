// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
// ---------------------------------------------------------------------------
USEFORM("MainWindow.cpp", MainChessForm);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	try {
		Application->Initialize();
		Application->Title = "ChessView";
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TMainChessForm), &MainChessForm);
		Application->Run();
	}
	catch (Exception &exception) {
		Application->ShowException(&exception);
	}
	catch (...) {
		try {
			throw Exception("");
		}
		catch (Exception &exception) {
			Application->ShowException(&exception);
		}
	}
	return 0;
}
// ---------------------------------------------------------------------------
