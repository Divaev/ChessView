//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#pragma hdrstop

#include "MainWindow.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainChessForm *MainChessForm;
//---------------------------------------------------------------------------
__fastcall TMainChessForm::TMainChessForm(TComponent* Owner):TForm(Owner) {
	prevSect = NULL;
	prevChosenSect = NULL;
	chDskPaint = new chessDeskPaint(MainChessForm->chessGraphic);         //�������������� ������ ��� ��������� ����� � �����
	possMovesPtr = NULL;
	possTakesPtr = NULL;
	currTurn = WHITE;                                      //����� ����� �������
	blackChecked = false;
	whiteChecked = false;
	firstMoveMade = false;
	sameDesksCount = 1;
	replayTimer->Enabled = false;

	//------------������ ������ ������� � �������, �������� ��������� ������� (��� ���������� ������� 3-� ������������� �������)----
    map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPaint->getChDsk()->getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;
	int counter = 0;
	deskNote dNote;
	for(tmpIt = tmpMap->begin(); tmpIt != tmpMap->end(); tmpIt++) {
		dNote.chColor[counter] = (*tmpIt).second->getChessColor();
		dNote.chType[counter] = (*tmpIt).second->getChessType();
		counter++;
	}

	dNote.blackShortCasAble = false;
	dNote.blackLongCasAble = false;
	dNote.whiteShortCasAble = false;
	dNote.whiteLongCasAble = false;
	dNote.whiteEpAble = false;
	dNote.blackEpAble = false;
	dNote.lastTurn = BLACK;
	deskNotes.push_back(dNote);
	//---------------------------------------------------------------------------------------------------------------------------

	//-----------------------�������������� ��������� �������--------------------------------------------------------------
	tOpt = new timerOptions;
	whiteTimer->Enabled = false;
	blackTimer->Enabled = false;
	try {
		tOpt->readTimerOptions();
	}
	catch (timerOptions::timerReadError tre) {
		if(tre.getErrType() == "INCORR") {
			ShowMessage("Timer options read error! Incorrect timer options file. Opening default timer options file...");
			try {
				tOpt->loadDefaultOptions();
			}
			catch(timerOptions::timerReadError tre) {
				if(tre.getErrType() == "DEFINCORR")
					ShowMessage("Default timer options read error! Timer is turned off.");
				else if(tre.getErrType() == "DEFREADERR")
					ShowMessage("Can't open default timer options file! Timer is turned off.");
				else if(tre.getErrType() == "DEFINVAL")
					ShowMessage("Default timer options read error! Invalid default timer options value. Timer is turned off.");
				menuTimerSwitch->Checked = false;
				menuTimerSwitch->Enabled = false;
				tOpt->timerOn = false;
			}
		}
		else if(tre.getErrType() == "READERR") {
			ShowMessage("Can't open timer options file! Opening default timer options file...");
            try {
				tOpt->loadDefaultOptions();
			}
			catch(timerOptions::timerReadError tre) {
				if(tre.getErrType() == "DEFINCORR")
					ShowMessage("Default timer options read error! Timer is turned off.");
				else if(tre.getErrType() == "DEFREADERR")
					ShowMessage("Can't open default timer options file! Timer is turned off.");
				else if(tre.getErrType() == "DEFINVAL")
					ShowMessage("Default timer options read error! Invalid default timer options value. Timer is turned off.");
				menuTimerSwitch->Checked = false;
				menuTimerSwitch->Enabled = false;
				tOpt->timerOn = false;
			}
		}
		else if(tre.getErrType() == "INVAL") {
			ShowMessage("Timer options read error! Invalid timer options value. Opening default timer options file...");
			try {
				tOpt->loadDefaultOptions();
			}
			catch(timerOptions::timerReadError tre) {
				if(tre.getErrType() == "DEFINCORR")
					ShowMessage("Default timer options read error! Timer is turned off.");
				else if(tre.getErrType() == "DEFREADERR")
					ShowMessage("Can't open default timer options file! Timer is turned off.");
				else if(tre.getErrType() == "DEFINVAL")
					ShowMessage("Default timer options read error! Invalid default timer options value. Timer is turned off.");
				menuTimerSwitch->Checked = false;
				menuTimerSwitch->Enabled = false;
				tOpt->timerOn = false;
			}
		}
	}
	//-------------------------------------------------------------------------------------------------------------------------



	//--------------------------���������� ����������� ��������� ����� � ��������� ��-�� ���������� � ����������� �� �������� �������----------------
	if(tOpt->timerOn == false) {
		menuTimerSwitch->Checked = false;
		menuTimerStop->Caption = "Start";
		menuTimerStop->Enabled = false;
	}

	else {
		chessGraphic->Enabled = false;
		blackTotalTime = 0;
		whiteTotalTime = 0;
		menuTimerSwitch->Checked = true;
		menuTimerStop->Caption = "Start";
	}


	//-----------------------------------------------------------------------------------------------------------------------------------------------


	//----------------------�������������� ����� ���������---------------------------------------------------------------------

	mainOpt = new configOptions;
	try {
		mainOpt->readConfig();
	} catch(configOptions::configReadError cre) {
		if(cre.getErrType() == "READERR")
			ShowMessage("Configuration file reading error!");
		else if(cre.getErrType() == "INCORR")
			ShowMessage("Incorrect configuration file! Game window is minimized");
	}
	if(mainOpt->formMinimize == true)
		menuMinimize->Checked = true;
	else if(mainOpt->formMinimize == false) {
		menuMinimize->Checked = false;
		this->Width += ADDFORMWIDTH;
	}
	//-----------------------------------------------------------------------------------------------------------------------

    //----------------------��������� �������������� ���������� ���������� ��� ������������ ���� ���������-----------------------
	blackTimerMemo = new TMemo(this);
	blackTimerMemo->Parent = this;
	blackTimerMemo->Enabled = false;
	blackTimerMemo->Left = 910;
	blackTimerMemo->Width = 270;
	blackTimerMemo->Height = 60;
	blackTimerMemo->Font->Size = 14;
	blackTimerMemo->Font->Style = TFontStyles() << fsBold;
	blackTimerMemo->Alignment = taCenter;


	whiteTimerMemo = new TMemo(this);
	whiteTimerMemo->Parent = this;
	whiteTimerMemo->Enabled = false;
	whiteTimerMemo->Left = 910;
	whiteTimerMemo->Top = 830;
	whiteTimerMemo->Width = 270;
	whiteTimerMemo->Height = 60;
	whiteTimerMemo->Font->Size = 14;
	whiteTimerMemo->Font->Style = TFontStyles() << fsBold;
	whiteTimerMemo->Alignment = taCenter;

	blackTimerStop = new TButton(this);
	blackTimerStop->Parent = this;
	blackTimerStop->Left = 910;
	blackTimerStop->Top = 65;
	blackTimerStop->Width = 80;
	blackTimerStop->Caption = "Start";
	blackTimerStop->Enabled = false;
	blackTimerStop->OnClick = bTimerStopClick;

	whiteTimerStop = new TButton(this);
	whiteTimerStop->Parent = this;
	whiteTimerStop->Left = 910;
	whiteTimerStop->Top = 800;
	whiteTimerStop->Width = 80;
	whiteTimerStop->Caption = "Start";
	whiteTimerStop->Enabled = false;
	whiteTimerStop->OnClick = wTimerStopClick;

	if(tOpt->timerOn == true)
		whiteTimerStop->Enabled = true;

	blackDrawOffer = new TButton(this);
	blackDrawOffer->Parent = this;
	blackDrawOffer->Left = 1005;
	blackDrawOffer->Top = 65;
	blackDrawOffer->Width = 80;
	blackDrawOffer->Caption = "Draw offer";
	blackDrawOffer->Enabled = false;
	blackDrawOffer->OnClick = blackDrawOfferClick;


	whiteDrawOffer = new TButton(this);
	whiteDrawOffer->Parent = this;
	whiteDrawOffer->Left = 1005;
	whiteDrawOffer->Top = 800;
	whiteDrawOffer->Width = 80;
	whiteDrawOffer->Caption = "Draw offer";
	whiteDrawOffer->Enabled = false;
	whiteDrawOffer->OnClick = whiteDrawOfferClick;

	blackConcede = new TButton(this);
	blackConcede->Parent = this;
	blackConcede->Left = 1100;
	blackConcede->Top = 65;
	blackConcede->Width = 80;
	blackConcede->Caption = "Concede";
	blackConcede->Enabled = false;
	blackConcede->OnClick = blackConcedeClick;

	whiteConcede = new TButton(this);
	whiteConcede->Parent = this;
	whiteConcede->Left = 1100;
	whiteConcede->Top = 800;
	whiteConcede->Width = 80;
	whiteConcede->Caption = "Concede";
	whiteConcede->Enabled = false;
	whiteConcede->OnClick = whiteConcedeClick;

	movesLog = new TMemo(this);
	movesLog->Parent = this;
	movesLog->Left = 910;
	movesLog->Top = 110;
	movesLog->Width = 270;
	movesLog->Height = 610;
	movesLog->ReadOnly = true;
	movesLog->ScrollBars = ssVertical;
	movesLog->Font->Size = 14;

	samePosInGame = new TLabel(this);
	samePosInGame->Parent = this;
	samePosInGame->Top = 730;
	samePosInGame->Left = 930;
	samePosInGame->Font->Size = 12;
	samePosInGame->Caption = "Same positions: 1";
	uselessMovesInGame = new TLabel(this);
	uselessMovesInGame->Parent = this;
	uselessMovesInGame->Top = 760;
	uselessMovesInGame->Left = 930;
	uselessMovesInGame->Font->Size = 12;
	uselessMovesInGame->Caption = "Reversible moves: 0";

	//-------------------------------------------------------------------------------------------------------------------------

	//-----------------------������� ��������� ���������� � ��������� ��������  � ����������� ��������� ����-----------------------------------------
	if(tOpt->timerOn == true) {
		if(tOpt->gameLimit == true) {
			blackTimerMemo->Lines->Add(AnsiString(tOpt->gameLimitTime) + " : 00");
			blackTimerMemo->Lines->Add("Time for a game.");
			whiteTimerMemo->Lines->Add(AnsiString(tOpt->gameLimitTime) + " : 00");
			whiteTimerMemo->Lines->Add("Time for a game.");
		}
		else if(tOpt->moveLimit == true) {

			blackTimerMemo->Lines->Add(AnsiString(tOpt->moveLimitTime) + " : 00");
			blackTimerMemo->Lines->Add("Time for 1 move.");
			whiteTimerMemo->Lines->Add(AnsiString(tOpt->moveLimitTime) + " : 00");
			whiteTimerMemo->Lines->Add("Time for 1 move.");
		}
		else if(tOpt->someMovesLimit == true) {
			blackTimerMemo->Lines->Add(AnsiString(tOpt->firstPerTime) + " : 00");
			blackTimerMemo->Lines->Add("Time for " + AnsiString(tOpt->firstPerMovesCount) + " moves.");
			whiteTimerMemo->Lines->Add(AnsiString(tOpt->firstPerTime) + " : 00");
			whiteTimerMemo->Lines->Add("Time for " + AnsiString(tOpt->firstPerMovesCount) + " moves.");
		}

	}
	else if(tOpt->timerOn == false) {
		blackTimerMemo->Lines->Add("Timer is off.");
		blackTimerMemo->Lines->Add("");
		whiteTimerMemo->Lines->Add("Timer is off.");
		whiteTimerMemo->Lines->Add("");
	}

	samePosInGame->Caption = "Same positions: 1";
	uselessMovesInGame->Caption = "Reversible moves: 0";

	//-----------------------------------------------------------------------------------------------------------------------------------------------

	menuSaveReplay->Enabled = false;
	menuSaveGame->Enabled = false;
	menuReplayControl->Enabled = false;
}



 __fastcall TMainChessForm::~TMainChessForm() {                             //��������� ����� fastcall  ����������� ��� �������� VCL
	 if(chDskPaint != NULL)                                                 //Visual Component Library �� Borland C++ Builder
		delete chDskPaint;                                                 //������� ������ ��������� ��������� �����
	 if(tOpt != NULL)
		delete tOpt;                                                       //������� ������ �������� �������
	 if(mainOpt != NULL)
		delete mainOpt;                                                    //������� ������ ����� ��������
	 if(repMovesBox != NULL) {
		 for(int i = 0; i < repMovesBox->GetCount(); i++)                //������� ������� ����-����� ����� ������
		   delete repMovesBox->Items->Objects[i];
	 }

 }

 void TMainChessForm::showEndForm(unsigned short keyIndex) {           //����� ����������� ����������� ���� � ������������� �������
		menuSaveReplay->Enabled = true;
		menuSaveGame->Enabled = false;
		blackTimer->Enabled = false;
		chessGraphic->Enabled = false;
		endGameForm = new TForm(this);
		endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
		endGameForm->Position = poScreenCenter;
		endGameForm->Caption = "Game over!";
		endGameForm->Width = 375;
		endGameForm->Height = 135;
		TLabel* winnerLabel = new TLabel(endGameForm);
		winnerLabel->Parent = endGameForm;
		winnerLabel->Top = 25;
		winnerLabel->Left = 15;
		winnerLabel->Font->Size = 12;
		winnerLabel->Font->Style = TFontStyles() << fsBold;
		switch(keyIndex) {
		case 5:
			winnerLabel->Caption = "Black offers draw. White accepted";
			break;
		case 6:
			winnerLabel->Caption = "White offers draw. Black accepted";
			break;
		case 7:
			winnerLabel->Caption = "Draw";
			break;
		case 8:
			winnerLabel->Caption = "Draw";
			break;
		case 9:
			winnerLabel->Caption = "Black wins";
			break;
		case 10:
			winnerLabel->Caption = "White wins";
		}

		TButton* okButt = new TButton(endGameForm);
		okButt->Parent = endGameForm;
		okButt->OnClick = parentFormClose;
		okButt->Top = 75;
		okButt->Left = 45;
		okButt->Caption = "Ok";
		TButton* newGameButt = new TButton(endGameForm);
		newGameButt->Parent = endGameForm;
		newGameButt->OnClick = parentFormClose;
		newGameButt->Top = 75;
		newGameButt->Left = 145;
		newGameButt->Caption = "New game";
		TButton* saveReplayButt = new TButton(endGameForm);          //��������� � ����������� ���������� ������ ���������� ������ �� ����� ����������� ����
		saveReplayButt->Parent = endGameForm;
		saveReplayButt->OnClick = parentFormClose;
		saveReplayButt->Top = 75;
		saveReplayButt->Left = 245;
		saveReplayButt->Caption = "Save replay";
		if(repMoveNotes.size() != 0)
			saveReplayButt->Caption = "New replay";
		endGameForm->ShowModal();
		delete endGameForm;
		return;

 }

 void configOptions::readConfig() {
	 char sym;
	 unsigned num;
	 char word[30];
	 vector<string>::iterator optIt;

	 ifstream readFileStream;
	 readFileStream.open((ExtractFilePath(Application->ExeName) + "Options_files" + "/Main_config.cfg").c_str());

	 if(!readFileStream)
		throw configReadError("READERR");
	 while(readFileStream >> word) {
		optIt = find(optionsVec.begin(), optionsVec.end(), word);
		if(optIt == optionsVec.end())
			throw configReadError("INCORR");
		if(*optIt == "FORM_MINIMIZE") {
			readFileStream >> sym;
			if(sym != '=')
				throw configReadError("INCORR");
			readFileStream >> sym;
            formMinimize = (sym == '1') ? true : false;
			if(readFileStream.fail())
				throw configReadError("INCORR");
			readFileStream >> sym;
			if(sym != ';')
				throw configReadError("INCORR");
		}
		else if(*optIt == "REPLAY_MOVEMENT_DELAY") {
			readFileStream >> sym;
			if(sym != '=')
				throw configReadError("INCORR");
			readFileStream >> num;
			if(readFileStream.fail())
				throw configReadError("INCORR");
			readFileStream >> sym;
			if(sym != ';')
				throw configReadError("INCORR");
			repMovesDelay = num;
        }
	 }
	readFileStream.close();

 }

 void configOptions::saveConfig() const {
		ofstream writeFileStream;
		writeFileStream.open((ExtractFilePath(Application->ExeName) + "Options_files" + "/Main_config.cfg").c_str(), ios::trunc);
		if(!writeFileStream) {
			ShowMessage("Writing error! Can't open configuration file!");
			writeFileStream.close();
			return;
		}
		writeFileStream << "FORM_MINIMIZE = ";
		if(formMinimize == true)
			writeFileStream << '1';
		else if(formMinimize == false)
			writeFileStream << '0';
		writeFileStream << ';' << endl;
		writeFileStream << "REPLAY_MOVEMENT_DELAY = ";
		writeFileStream << repMovesDelay;
		writeFileStream << ';';
		writeFileStream.close();
		return;

 }

void TMainChessForm::newGame() {
	prevSect = NULL;
	prevChosenSect = NULL;
	if(possMovesPtr != NULL)
		possMovesPtr->clear();
	if(possTakesPtr != NULL)
		possTakesPtr->clear();
	possMovesPtr = NULL;
	possTakesPtr = NULL;
	currTurn = WHITE;
	blackChecked = false;
	whiteChecked = false;
	firstMoveMade = false;
	resultIsShowed = false;
	chDskPaint->setChDsk();
	chDskPaint->getChDsk()->startDesk();
	moveNote::moveCounter = 0;
	moveNote::uselessCounter = 0;
	moveNotes.clear();
	repMoveNotes.clear();
	repMoveNote::moveCounter = 0;
	repMoveNote::currMove = 0;
	repMoveNote::repIsFinished = 0;
	replayTimer->Enabled = false;
	sameDesksCount = 1;
	deskNotes.clear();
	chDskPaint->paintPieces();

	blackTimer->Enabled = false;                //���� ����� ���������� ���� �������� ���������� ������� ��������� ��
	whiteTimer->Enabled = false;
	movesLog->Clear();                         //������� �������� ���� ��� ����������� �����

	if(menuReplayControl->Checked == true)
		replayControlForm->Close();



	//---------------���� �� ����� ��������������� ������, ������������� ����������� ������� ������� ����� � ����� ����----------------------
	if(repMovesBox != NULL) {
		repMovesBox->Hide();
		gotoMainButt->Hide();
		prevMainButt->Hide();
		nextMainButt->Hide();
		playMainButt->Hide();
		numberMainEdit->Hide();
		delayMainEdit->Hide();
		delayRepLabel->Hide();
		samePosLabel->Hide();
		uselessMovesLabel->Hide();

		blackTimerMemo->Show();
		whiteTimerMemo->Show();
		whiteTimerStop->Show();
		blackTimerStop->Show();
		blackDrawOffer->Show();
		whiteDrawOffer->Show();
		blackConcede->Show();
		whiteConcede->Show();
		movesLog->Show();
		samePosInGame->Show();
		uselessMovesInGame->Show();

    }
	//------------������ ������ ������� � �������, �������� ��������� ������� (��� ���������� ������� 3-� ������������� �������)----
	map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPaint->getChDsk()->getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;
	int counter = 0;
	deskNote dNote;
	for(tmpIt = tmpMap->begin(); tmpIt != tmpMap->end(); tmpIt++) {
		dNote.chColor[counter] = (*tmpIt).second->getChessColor();
		dNote.chType[counter] = (*tmpIt).second->getChessType();
		counter++;
	}
	dNote.blackShortCasAble = false;
	dNote.blackLongCasAble = false;
	dNote.whiteShortCasAble = false;
	dNote.whiteLongCasAble = false;
	dNote.whiteEpAble = false;
	dNote.blackEpAble = false;
	dNote.lastTurn = BLACK;
	deskNotes.push_back(dNote);
	//---------------------------------------------------------------------------------------------------------------------------

	//-----------------�������������� ��������� �������--------------------------------------------------------------------------
	try {
		tOpt->readTimerOptions();

	}
	catch (timerOptions::timerReadError tre) {          //������������� ���������� ��� ����, ����� ���������� ���� �� ���� �����
														//������ ������ ����� ��������
		if(tre.getErrType() == "INCORR" && tOpt->timerOn == true) {
			ShowMessage("Timer options read error! Incorrect timer options file. Opening default timer options file...");
			try {
				tOpt->loadDefaultOptions();
			}
			catch(timerOptions::timerReadError tre) {
				if(tre.getErrType() == "DEFINCORR" && tOpt->timerOn == true)
					ShowMessage("Default timer options read error! Timer is turned off.");
				else if(tre.getErrType() == "DEFREADERR" && tOpt->timerOn == true)
					ShowMessage("Can't open default timer options file! Timer is turned off.");
				else if(tre.getErrType() == "DEFINVAL" && tOpt->timerOn == true)
					ShowMessage("Default timer options read error! Invalid default timer options value. Timer is turned off.");
				menuTimerSwitch->Checked = false;
				menuTimerSwitch->Enabled = false;
				tOpt->timerOn = false;
			}
		}
		else if(tre.getErrType() == "READERR" && tOpt->timerOn == true) {
			ShowMessage("Can't open timer options file! Opening default timer options file...");
			try {
				tOpt->loadDefaultOptions();
			}
			catch(timerOptions::timerReadError tre) {
				if(tre.getErrType() == "DEFINCORR" && tOpt->timerOn == true)
					ShowMessage("Default timer options read error! Timer is turned off.");
				else if(tre.getErrType() == "DEFREADERR" && tOpt->timerOn == true)
					ShowMessage("Can't open default timer options file! Timer is turned off.");
				else if(tre.getErrType() == "DEFINVAL" && tOpt->timerOn == true)
					ShowMessage("Default timer options read error! Invalid default timer options value. Timer is turned off.");
				menuTimerSwitch->Checked = false;
				menuTimerSwitch->Enabled = false;
				tOpt->timerOn = false;
			}
		}
		else if(tre.getErrType() == "INVAL" && tOpt->timerOn == true) {
			ShowMessage("Timer options read error! Invalid timer options value. Opening default timer options file...");
			try {
				tOpt->loadDefaultOptions();
			}
			catch(timerOptions::timerReadError tre) {
				if(tre.getErrType() == "DEFINCORR" && tOpt->timerOn == true)
					ShowMessage("Default timer options read error! Timer is turned off.");
				else if(tre.getErrType() == "DEFREADERR" && tOpt->timerOn == true)
					ShowMessage("Can't open default timer options file! Timer is turned off.");
				else if(tre.getErrType() == "DEFINVAL" && tOpt->timerOn == true)
					ShowMessage("Default timer options read error! Invalid default timer options value. Timer is turned off.");
				menuTimerSwitch->Checked = false;
				menuTimerSwitch->Enabled = false;
				tOpt->timerOn = false;
			}
		}

	}
	//------------------------------------------------------------------------------------------------------------------------

	menuTimerSwitch->Enabled = true;
	if(tOpt->timerOn == true) {
		chessGraphic->Enabled = false;
		blackTotalTime = 0;
		whiteTotalTime = 0;
		menuTimerSwitch->Checked = true;
		menuTimerStop->Caption = "Start";
		menuTimerStop->Enabled = true;
		menuTimerOptions->Enabled = true;
		whiteTimerStop->Enabled = true;
		blackTimerStop->Enabled = false;
		whiteTimerStop->Caption = "Start";
		blackTimerStop->Caption = "Start";
	}
	else if(tOpt->timerOn == false) {
		chessGraphic->Enabled = true;
		menuTimerSwitch->Checked = false;
		menuTimerStop->Caption = "Start";
		menuTimerStop->Enabled = false;
		menuTimerOptions->Enabled = true;
		whiteTimerStop->Enabled = false;
		blackTimerStop->Enabled = false;
		whiteTimerStop->Caption = "Start";
        blackTimerStop->Caption = "Start";
	}
	blackTotalTime = 0;
	whiteTotalTime = 0;

    //-----------------------������� ��������� ���������� � ��������� ��������  � ����������� ��������� ����-----------------------------------------
	if(tOpt->timerOn == true) {
		if(tOpt->gameLimit == true) {
			blackTimerMemo->Lines->Strings[0] = AnsiString(tOpt->gameLimitTime) + " : 00";
			blackTimerMemo->Lines->Strings[1] = "Time for a game.";
			whiteTimerMemo->Lines->Strings[0] = AnsiString(tOpt->gameLimitTime) + " : 00";
			whiteTimerMemo->Lines->Strings[1] = "Time for a game.";
		}
		else if(tOpt->moveLimit == true) {
			blackTimerMemo->Lines->Strings[0] = AnsiString(tOpt->moveLimitTime) + " : 00";
			blackTimerMemo->Lines->Strings[1] = "Time for 1 move.";
			whiteTimerMemo->Lines->Strings[0] = AnsiString(tOpt->moveLimitTime) + " : 00";
			whiteTimerMemo->Lines->Strings[1] = "Time for 1 move.";
		}
		else if(tOpt->someMovesLimit == true) {
			blackTimerMemo->Lines->Strings[0] = AnsiString(tOpt->firstPerTime) + " : 00";
			blackTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount) + " moves.";
			whiteTimerMemo->Lines->Strings[0] = AnsiString(tOpt->firstPerTime) + " : 00";
			whiteTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount) + " moves.";
		}
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------------
	samePosInGame->Caption = "Same positions: 1";
	uselessMovesInGame->Caption = "Reversible moves: 0";

	menuSaveReplay->Enabled = false;
	menuSaveGame->Enabled = false;
	menuOpenGame->Enabled = true;
	menuOpenReplay->Enabled = true;
	menuReplayControl->Enabled = false;
	menuTimer->Enabled = true;
	menuTimerSwitch->Enabled = true;
 //	menuTimerStop->Enabled = true;
	menuTimerOptions->Enabled = true;


}


void TMainChessForm::makeMove() {
    //-----------------��������� ��������� ��������� ����� ����� ����--------------------------------------------------------
		 if(repMoveNotes.size() == 0)              //���� ���� �� �������� �������
			saveDeskNote();
		 else if(repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {        //���� ���� �������� �������� ����� ����������� ������ (�������), � ������ ��� �� ��� �������� �� �����
			 if(deskNote::currDesk == deskNotes.size()) {      //���� ���������� ����� �������� ��������� ����� ����� ����������� ������ ���������� ������������ ��������� �����

				 saveDeskNote();                                 //��������� ��������� ��������� �����
				 if(currTurn == WHITE && repMoveNote::currMove + 1 == repMoveNote::moveCounter && repMoveNotes[repMoveNote::moveCounter - 1].blackMoveSect.number == 0 ||
				 currTurn == BLACK && repMoveNote::currMove + 1 == repMoveNote::moveCounter && repMoveNotes[repMoveNote::moveCounter - 1].blackMoveSect.number != 0)
					repMoveNote::repIsFinished = true;
			 }
			 else
				deskNote::currDesk++;                           //� ��������� ������ �������������� ����� �������� ��������� �����
		 }
		 else if(repMoveNote::repIsFinished == true)          //���� ������ ����� ��� �������� �� �����
			deskNote::currDesk++;                             //������ �������������� ���������� ����� �������� ��������� �����


	 //------------------------------------------------------------------------------------------------------------------------
	chessDesk* tmpChessDesk = chDskPaint->getChDsk();            // �������������� ��������� ���������
																		// �� ������-�����
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = tmpChessDesk->getWhiteChess();      //� ����� ��������� �� ����������� ����� ������
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = tmpChessDesk->getBlackChess();     // � ������ ������
    map<deskSection*, chessPiece*, sectCompare>* tmpSectionsMap = tmpChessDesk-> getMap();           //��������� �� ����������� ���. �����
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpBlackIt;
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpSectionsIt;
	set<deskSection*, lessSectPtr>* locSetPtr;
	set<deskSection*, lessSectPtr>::iterator locSetIt;
//--------------------���������� ������ ����� ����� ������------------------------------------------------------------------
	if(currTurn == BLACK) {                                                   //���� ������ ������,
		for(tmpWhiteIt = tmpWhiteChess->begin(); tmpWhiteIt != tmpWhiteChess->end(); tmpWhiteIt++)
			if((*tmpWhiteIt).first->getChessType() == PAWN)               //�� ��� ���� ����� �������� ��������� �� ���������� �������,
				(*tmpWhiteIt).first->setPrevPos(NULL);                    //�.�. ����� ������ ����� ������ ��������������� ������ �� 1 ���
	}
	else if(currTurn == WHITE) {                                      //���������� � ��� ������, ���� ������ �����
			for(tmpBlackIt = tmpBlackChess->begin(); tmpBlackIt != tmpBlackChess->end(); tmpBlackIt++)
				if((*tmpBlackIt).first->getChessType() == PAWN)
					(*tmpBlackIt).first->setPrevPos(NULL);
	}
//---------------------------------------------------------------------------------------------------------------------------

//-----------------------���������� ���������� ����-------------------------------------------------------------------------------
	map<chessPiece*, deskSection*>::iterator whiteKingIt;                   //��������� ��������� ��������, ����������� �� ����, ������ ������� ��������
																			//������ ������� �������� ��������� �� ������ ������
	for(whiteKingIt = tmpWhiteChess->begin(); whiteKingIt != tmpWhiteChess->end(); whiteKingIt++) {
		if((*whiteKingIt).first->getChessType() == KING)                               //�������������� �������� ��������� �����������, ���� ����
			break;                                                                     //��������� �� ������ ������
	}

	map<chessPiece*, deskSection*>::iterator blackKingIt;
	for(blackKingIt = tmpBlackChess->begin(); blackKingIt != tmpBlackChess->end(); blackKingIt++) {  //���������� ��� ������� ������
		if((*blackKingIt).first->getChessType() == KING)
			break;
	}


	if(currTurn == BLACK) {                                                         //���� �������� ��� �������
		(*whiteKingIt).first->setCheck(chDskPaint->getChDsk());                     //��� ������ ������ ������������� ���� ���������� ����,
																					//��������� ��������� �� ������� ������-����� � ���-�� ���������
		whiteChecked = ((*whiteKingIt).first->getCheck()) ? true : false;          //���� ���� ���������� ���� ������ ������ ����� �������� true
																				   //�� ������������� ������� ���������� ���� ����� �������
		if(whiteChecked == true)
			moveNotes[moveNotes.size() - 1].blackCheck = true;                            //������ ��������������� ������� � ��������� �������� ������
	}

	else if(currTurn == WHITE) {                                                //���������, ���� �������� ��� ������
		(*blackKingIt).first->setCheck(chDskPaint->getChDsk());                 //���� ���������� ���� ������� ������
		 blackChecked = ((*blackKingIt).first->getCheck()) ? true : false;      //������� ���������� ���� ������ �������
		 if(blackChecked == true)
			moveNotes[moveNotes.size() - 1].whiteCheck = true;                       //������ ��������������� ������� � ��������� �������� ������
	}

//-------------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------�������� ����������� ��� ���� � ��������������� ������� ����� ���������� �������� ����--------------------------------
	if(currTurn == WHITE) {
		for(tmpBlackIt = tmpBlackChess->begin(); tmpBlackIt != tmpBlackChess->end(); tmpBlackIt++) {
			tmpBlackIt->first->setMovesTakes(tmpBlackIt->second, tmpSectionsMap);
			locSetPtr = tmpBlackIt->first->getPossMoves();
			if(!locSetPtr->empty())
				break;
			locSetPtr = tmpBlackIt->first->getPossTakes();
			if(!locSetPtr->empty())
				break;
		}

		if(tmpBlackIt == tmpBlackChess->end()) {          //���� � ����� ��� ����������� ��� ����
			if(repMoveNotes.size() == 0)                  //���� ���� �� �������� �������
				menuSaveReplay->Enabled = true;          //�������� ����� ���� ��� ���������� ������
			else
				menuSaveReplay->Enabled = false;        //��������� ����� ���� ��� ���������� ������

			menuSaveGame->Enabled = false;              //�������� ����� ���� ��� ���������� ����

			if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
			}

			whiteTimerStop->Enabled = false;            //������������ ������ ���/���� �������, ����������� ����� � �����
			blackTimerStop->Enabled = false;
			whiteConcede->Enabled = false;
			blackConcede->Enabled = false;
			whiteDrawOffer->Enabled = false;
			blackDrawOffer->Enabled = false;

			if(blackChecked == true) {
				moveNotes[moveNotes.size() - 1].whiteCheckmate = true;              //������������� ���� ���������� ���� ������ � �������� ������� - ������ ����
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� � ��������� �������� ������� moveNotes																													//� ��������� ���� ��� ������ �����
				chDskPaint->paintPieces();
				chessGraphic->Enabled = false;
				endGameForm = new TForm(this);
				endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
				resultIsShowed = true;
				endGameForm->Position = poScreenCenter;
				endGameForm->Caption = "Game over!";
				endGameForm->Width = 375;
				endGameForm->Height = 135;
				TLabel* winnerLabel = new TLabel(endGameForm);
				winnerLabel->Parent = endGameForm;
				winnerLabel->Top = 25;
				winnerLabel->Left = 10;
				winnerLabel->Font->Size = 16;
				winnerLabel->Font->Style = TFontStyles() << fsBold;
				winnerLabel->Caption = "Checkmate! White won the game";
				TButton* okButt = new TButton(endGameForm);
				okButt->Parent = endGameForm;
				okButt->OnClick = parentFormClose;
				okButt->Top = 75;
				okButt->Left = 45;
				okButt->Caption = "Ok";
				TButton* newGameButt = new TButton(endGameForm);
				newGameButt->Parent = endGameForm;
				newGameButt->OnClick = parentFormClose;
				newGameButt->Top = 75;
				newGameButt->Left = 145;
				newGameButt->Caption = "New game";
				TButton* saveReplayButt = new TButton(endGameForm);
				saveReplayButt->Parent = endGameForm;
				saveReplayButt->OnClick = parentFormClose;
				saveReplayButt->Top = 75;
				saveReplayButt->Left = 245;
				saveReplayButt->Caption = "Save replay";
				if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
					saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
				endGameForm->ShowModal();
				delete endGameForm;
				return;

			}
			else {

				moveNotes[moveNotes.size() - 1].whiteStalemate = true;                  //������������� ���� ���������� ���� ������ � �������� ������� - ������ ����
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� ������� ����� moveNotes																											  //� ��������� ���� ������ �����
				chDskPaint->paintPieces();
				chessGraphic->Enabled = false;
				endGameForm = new TForm(this);
				endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
				resultIsShowed = true;
				endGameForm->Position = poScreenCenter;
				endGameForm->Caption = "Game over!";
				endGameForm->Width = 375;
				endGameForm->Height = 135;
				TLabel* winnerLabel = new TLabel(endGameForm);
				winnerLabel->Parent = endGameForm;
				winnerLabel->Top = 25;
				winnerLabel->Left = 100;
				winnerLabel->Font->Size = 16;
				winnerLabel->Font->Style = TFontStyles() << fsBold;
				winnerLabel->Caption = "Stalemate! Draw";
				TButton* okButt = new TButton(endGameForm);
				okButt->Parent = endGameForm;
				okButt->OnClick = parentFormClose;
				okButt->Top = 75;
				okButt->Left = 45;
				okButt->Caption = "Ok";
				TButton* newGameButt = new TButton(endGameForm);
				newGameButt->Parent = endGameForm;
				newGameButt->OnClick = parentFormClose;
				newGameButt->Top = 75;
				newGameButt->Left = 145;
				newGameButt->Caption = "New game";
				TButton* saveReplayButt = new TButton(endGameForm);
				saveReplayButt->Parent = endGameForm;
				saveReplayButt->OnClick = parentFormClose;
				saveReplayButt->Top = 75;
				saveReplayButt->Left = 245;
				saveReplayButt->Caption = "Save replay";
                if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
					saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
				endGameForm->ShowModal();
				delete endGameForm;
				return;
			}


		}
	}

	else if(currTurn == BLACK) {
		for(tmpWhiteIt = tmpWhiteChess->begin(); tmpWhiteIt != tmpWhiteChess->end(); tmpWhiteIt++) {
			tmpWhiteIt->first->setMovesTakes(tmpWhiteIt->second, tmpSectionsMap);
			locSetPtr = tmpWhiteIt->first->getPossMoves();
			if(!locSetPtr->empty())
				break;
			locSetPtr = tmpWhiteIt->first->getPossTakes();
			if(!locSetPtr->empty())
				break;
		}
		if(tmpWhiteIt == tmpWhiteChess->end()) {            //���� � ����� ��� ����������� ��� ����
			if(repMoveNotes.size() == 0)                  //���� ���� �� �������� �������
				menuSaveReplay->Enabled = true;          //�������� ����� ���� ��� ���������� ������
			else
				menuSaveReplay->Enabled = false;        //��������� ����� ���� ��� ���������� ������

			menuSaveGame->Enabled = false;

			if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
				whiteTimer->Enabled = false;
				blackTimer->Enabled = false;
			}

			whiteTimerStop->Enabled = false;            //������������ ������ ���/���� �������, ����������� ����� � �����
			blackTimerStop->Enabled = false;
			whiteConcede->Enabled = false;
			blackConcede->Enabled = false;
			whiteDrawOffer->Enabled = false;
			blackDrawOffer->Enabled = false;

			if(whiteChecked == true) {                      //���� ��� ���� ����� ��� �����
				moveNotes[moveNotes.size() - 1].blackCheckmate = true;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();    //������� ��������� ��������� ������� ����� moveNotes
																															//� ��������� ���� ������ �����
				chDskPaint->paintPieces();
				chessGraphic->Enabled = false;
				endGameForm = new TForm(this);
				endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
                resultIsShowed = true;
				endGameForm->Position = poScreenCenter;
				endGameForm->Caption = "Game over!";
				endGameForm->Width = 375;
				endGameForm->Height = 135;
				TLabel* winnerLabel = new TLabel(endGameForm);
				winnerLabel->Parent = endGameForm;
				winnerLabel->Top = 25;
				winnerLabel->Left = 10;
				winnerLabel->Font->Size = 16;
				winnerLabel->Font->Style = TFontStyles() << fsBold;
				winnerLabel->Caption = "Checkmate! Black won the game";
				TButton* okButt = new TButton(endGameForm);
				okButt->Parent = endGameForm;
				okButt->OnClick = parentFormClose;
				okButt->Top = 75;
				okButt->Left = 45;
				okButt->Caption = "Ok";
				TButton* newGameButt = new TButton(endGameForm);
				newGameButt->Parent = endGameForm;
				newGameButt->OnClick = parentFormClose;
				newGameButt->Top = 75;
				newGameButt->Left = 145;
				newGameButt->Caption = "New game";
				TButton* saveReplayButt = new TButton(endGameForm);
				saveReplayButt->Parent = endGameForm;
				saveReplayButt->OnClick = parentFormClose;
				saveReplayButt->Top = 75;
				saveReplayButt->Left = 245;
				saveReplayButt->Caption = "Save replay";
				if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
					saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
				endGameForm->ShowModal();
				delete endGameForm;
				return;
			}
			else {

				moveNotes[moveNotes.size() - 1].blackStalemate = true;
				chDskPaint->paintPieces();
				chessGraphic->Enabled = false;
				endGameForm = new TForm(this);
				endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
                resultIsShowed = true;
				endGameForm->Position = poScreenCenter;
				endGameForm->Caption = "Game over!";
				endGameForm->Width = 375;
				endGameForm->Height = 135;
				TLabel* winnerLabel = new TLabel(endGameForm);
				winnerLabel->Parent = endGameForm;
				winnerLabel->Top = 25;
				winnerLabel->Left = 100;
				winnerLabel->Font->Size = 16;
				winnerLabel->Font->Style = TFontStyles() << fsBold;
				winnerLabel->Caption = "Stalemate! Draw";
				TButton* okButt = new TButton(endGameForm);
				okButt->Parent = endGameForm;
				okButt->OnClick = parentFormClose;
				okButt->Top = 75;
				okButt->Left = 45;
				okButt->Caption = "Ok";
				TButton* newGameButt = new TButton(endGameForm);
				newGameButt->Parent = endGameForm;
				newGameButt->OnClick = parentFormClose;
				newGameButt->Top = 75;
				newGameButt->Left = 145;
				newGameButt->Caption = "New game";
				TButton* saveReplayButt = new TButton(endGameForm);
				saveReplayButt->Parent = endGameForm;
				saveReplayButt->OnClick = parentFormClose;
				saveReplayButt->Top = 75;
				saveReplayButt->Left = 245;
				saveReplayButt->Caption = "Save replay";
				if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
					saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
				endGameForm->ShowModal();
				delete endGameForm;
				return;
			}

		}
	}
	//-------------------------------------------------------------------------------------------------------------------------------

	//----------------���������, ��������� �� ������� 50 �����-------------------------------------------------------------------
	static bool prevTurnIsFinished = false;            //����������� ����, ������� ��������������� � ������ ������� ��������������� ������, �� ��� ����� ���������� ��������� ����� �����
	if(repMoveNote::repIsFinished == false)           //� ������ ���� ������ �� �������� (��������, ������ �����), �� ������. ���� ���������
		prevTurnIsFinished = false;
	if(deskNote::currDesk == deskNotes.size() && prevTurnIsFinished == false) {           //���� ����� �������� ��������� ����� ����� ������� �������, ��������� ������� ��������� ����� (����� ������ ���-�� ���������),
																 //�.�. ���� ����������� ����� �� ����������� ��� ��� �������) � ������ ����� �� ��� ��������� �������������
		moveNotes[moveNotes.size() - 1].checkUselessMove();      //�������� ����� �������� ���-�� ����� ��� ����������� ���������
		uselessMovesInGame->Caption = "Reversible moves: " + AnsiString(moveNote::uselessCounter);
	}
	prevTurnIsFinished = repMoveNote::repIsFinished;         //� ������ ������� ��������������� ������ ������������� ��������� ����������� ����, �� ��� ����� ���������� �������� ��������� �����

	if(moveNotes[moveNotes.size() - 1].uselessCounter >= 50) {
			if(repMoveNotes.size() == 0)                          //���� ���� �� �������� �������
				menuSaveReplay->Enabled = true;                  //���������� ������ ���������� ������
			else
				menuSaveReplay->Enabled = false;                //� ��������� ������ ������������ ������
			menuSaveGame->Enabled = false;

			if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
			}

			whiteTimerStop->Enabled = false;            //������������ ������ ���/���� �������, ����������� ����� � �����
			blackTimerStop->Enabled = false;
			whiteConcede->Enabled = false;
			blackConcede->Enabled = false;
			whiteDrawOffer->Enabled = false;
			blackDrawOffer->Enabled = false;

			moveNotes[moveNotes.size() - 1].drawSign = true;               //� ��������� �������� ���������� ���� ��������� �����
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� ������� ����� moveNotes
                                                                                                                            //� ��������� ���� ������ �����
			chDskPaint->paintPieces();
			chessGraphic->Enabled = false;
			endGameForm = new TForm(this);
			endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
			resultIsShowed = true;
			endGameForm->Position = poScreenCenter;
			endGameForm->Caption = "Game over!";
			endGameForm->Width = 375;
			endGameForm->Height = 135;
			TLabel* winnerLabel = new TLabel(endGameForm);
			winnerLabel->Parent = endGameForm;
			winnerLabel->Top = 25;
			winnerLabel->Left = 50;
			winnerLabel->Font->Size = 16;
			winnerLabel->Font->Style = TFontStyles() << fsBold;
			winnerLabel->Caption = "50 passive moves! Draw";
			TButton* okButt = new TButton(endGameForm);
			okButt->Parent = endGameForm;
			okButt->OnClick = parentFormClose;
			okButt->Top = 75;
			okButt->Left = 45;
			okButt->Caption = "Ok";
			TButton* newGameButt = new TButton(endGameForm);
			newGameButt->Parent = endGameForm;
			newGameButt->OnClick = parentFormClose;
			newGameButt->Top = 75;
			newGameButt->Left = 145;
			newGameButt->Caption = "New game";
			TButton* saveReplayButt = new TButton(endGameForm);
			saveReplayButt->Parent = endGameForm;
			saveReplayButt->OnClick = parentFormClose;
			saveReplayButt->Top = 75;
			saveReplayButt->Left = 245;
			saveReplayButt->Caption = "Save replay";
			if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
					saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
			endGameForm->ShowModal();
			delete endGameForm;
			return;
	}
//-----------------------------------------------------------------------------------------------------------------------



//-------------------���������, ��������� �� ������� 3-� ������������� �������--------------------------------------------
	if(sameDesksCount == 3) {
		if(repMoveNotes.size() == 0)
			menuSaveReplay->Enabled = true;
		else
			menuSaveReplay->Enabled = false;
		menuSaveGame->Enabled = false;

        if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //������������ ������ ���/���� �������, ����������� ����� � �����
		blackTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		blackConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
		blackDrawOffer->Enabled = false;

		chDskPaint->paintPieces();
		moveNotes[moveNotes.size() - 1].drawSign = true;               //� ��������� �������� ���������� ���� ��������� �����
		if(currTurn == BLACK)                                   //���� ���� ����������� ����� ������
			movesLog->Lines->Delete(movesLog->Lines->Count - 1);       //������� �������� ������ ������ � �����
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� ������� �����																												   //� ��������� ���� ������ �����
		chessGraphic->Enabled = false;
		//-------------------------������� ���� ����������� ����------------------------------------------------------------
		endGameForm = new TForm(this);
		endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
        resultIsShowed = true;
		endGameForm->Position = poScreenCenter;
		endGameForm->Caption = "Game over!";
		endGameForm->Width = 375;
		endGameForm->Height = 135;
		TLabel* winnerLabel = new TLabel(endGameForm);
		winnerLabel->Parent = endGameForm;
		winnerLabel->Top = 25;
		winnerLabel->Left = 50;
		winnerLabel->Font->Size = 16;
		winnerLabel->Font->Style = TFontStyles() << fsBold;
		winnerLabel->Caption = "Same position 3 times! Draw";
		TButton* okButt = new TButton(endGameForm);
		okButt->Parent = endGameForm;
		okButt->OnClick = parentFormClose;
		okButt->Top = 75;
		okButt->Left = 45;
		okButt->Caption = "Ok";
		TButton* newGameButt = new TButton(endGameForm);
		newGameButt->Parent = endGameForm;
		newGameButt->OnClick = parentFormClose;
		newGameButt->Top = 75;
		newGameButt->Left = 145;
		newGameButt->Caption = "New game";
		TButton* saveReplayButt = new TButton(endGameForm);
		saveReplayButt->Parent = endGameForm;
		saveReplayButt->OnClick = parentFormClose;
		saveReplayButt->Top = 75;
		saveReplayButt->Left = 245;
		saveReplayButt->Caption = "Save replay";
        if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
			saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
		endGameForm->ShowModal();
		//--------------------------------------------------------------------------------------------------------------------------------
		delete endGameForm;
		return;

	}

//----------��������� ���������� �� ��������� � ����� ������-------------------------------------------------------
	bool blackMaterialEnough = false;
	bool whiteMaterialEnough = false;
	unsigned blackKnightCounter = 0;
	unsigned whiteKnightCounter = 0;
	bool blKnightOnBlack = false;
	bool blKnightOnWhite = false;
	bool wtKnightOnBlack = false;
	bool wtKnightOnWhite = false;
	for(tmpWhiteIt = tmpWhiteChess->begin(); tmpWhiteIt != tmpWhiteChess->end(); tmpWhiteIt++) {
		if((*tmpWhiteIt).first->getChessType() == PAWN || (*tmpWhiteIt).first->getChessType() == ROOK ||
		  (*tmpWhiteIt).first->getChessType() == QUEEN)
			whiteMaterialEnough = true;
		else if((*tmpWhiteIt).first->getChessType() == KNIGHT) {
			whiteKnightCounter++;
			if(whiteKnightCounter == 2)
				whiteMaterialEnough = true;
		}
		else if((*tmpWhiteIt).first->getChessType() == BISHOP) {
			if((*tmpWhiteIt).second->sectColor == WHITE)
				wtKnightOnWhite = true;
			else if((*tmpWhiteIt).second->sectColor == BLACK)
				wtKnightOnBlack = true;
			if(wtKnightOnBlack == true && wtKnightOnWhite == true)
				whiteMaterialEnough = true;
        }
	}
	for(tmpBlackIt = tmpBlackChess->begin(); tmpBlackIt != tmpBlackChess->end(); tmpBlackIt++) {
		if((*tmpBlackIt).first->getChessType() == PAWN || (*tmpBlackIt).first->getChessType() == ROOK ||
			(*tmpBlackIt).first->getChessType() == QUEEN)
				blackMaterialEnough = true;
		else if((*tmpBlackIt).first->getChessType() == KNIGHT) {
			blackKnightCounter++;
			if(blackKnightCounter == 2)
				blackMaterialEnough = true;
		}
		else if((*tmpBlackIt).first->getChessType() == BISHOP) {
			if((*tmpBlackIt).second->sectColor == BLACK)
				blKnightOnBlack = true;
			else if((*tmpBlackIt).second->sectColor == WHITE)
				blKnightOnWhite = true;
			if(blKnightOnWhite == true && blKnightOnBlack == true)
				blackMaterialEnough = true;
		}
	}
	if(blackMaterialEnough == false && whiteMaterialEnough == false) {
		if(repMoveNotes.size() == 0)
			menuSaveReplay->Enabled = true;
		else
			menuSaveReplay->Enabled = false;

		menuSaveGame->Enabled = false;

		if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //������������ ������ ���/���� �������, ����������� ����� � �����
		blackTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		blackConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
		blackDrawOffer->Enabled = false;

		chDskPaint->paintPieces();
		moveNotes[moveNotes.size() - 1].drawSign = true;               //� ��������� �������� ���������� ���� ��������� �����
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();  //������� ��������� ��������� ������� ����� moveNotes
																													 //� ��������� ���� ������ �����
		//------------------------------������� ���� ����������� ����-------------------------------------------------------------
		chessGraphic->Enabled = false;
		endGameForm = new TForm(this);
        endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
		resultIsShowed = true;
		endGameForm->Position = poScreenCenter;
		endGameForm->Caption = "Game over!";
		endGameForm->Width = 375;
		endGameForm->Height = 135;
		TLabel* winnerLabel = new TLabel(endGameForm);
		winnerLabel->Parent = endGameForm;
		winnerLabel->Top = 15;
		winnerLabel->Left = 20;
		winnerLabel->Font->Size = 12;
		winnerLabel->Font->Style = TFontStyles() << fsBold;
		winnerLabel->Caption = "Both players have not enough material!";
		TLabel* winnerAddLabel = new TLabel(endGameForm);
		winnerAddLabel->Parent = endGameForm;
		winnerAddLabel->Top = 45;
		winnerAddLabel->Left = 150;
		winnerAddLabel->Font->Size = 12;
		winnerAddLabel->Font->Style = TFontStyles() << fsBold;
		winnerAddLabel->Caption = "Draw";
		TButton* okButt = new TButton(endGameForm);
		okButt->Parent = endGameForm;
		okButt->OnClick = parentFormClose;
		okButt->Top = 75;
		okButt->Left = 45;
		okButt->Caption = "Ok";
		TButton* newGameButt = new TButton(endGameForm);
		newGameButt->Parent = endGameForm;
		newGameButt->OnClick = parentFormClose;
		newGameButt->Top = 75;
		newGameButt->Left = 145;
		newGameButt->Caption = "New game";
		TButton* saveReplayButt = new TButton(endGameForm);
		saveReplayButt->Parent = endGameForm;
		saveReplayButt->OnClick = parentFormClose;
		saveReplayButt->Top = 75;
		saveReplayButt->Left = 245;
		saveReplayButt->Caption = "Save replay";
        if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
			saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
		endGameForm->ShowModal();
		//---------------------------------------------------------------------------------------------------------------------
		delete endGameForm;
		return;
    }
 //---------------------------------------------------------------------------------------------------------------------------------


	currTurn = (currTurn == WHITE) ? BLACK : WHITE;                //������ ���� �������, ������� ����� ����, �� ���������������

	if(currTurn == WHITE && tOpt->timerOn == true) {            //� ����������� �� ����, ���� ������� ����� ����
		whiteTimerStop->Enabled = true;                        //���������� ������ ���/���� ������� �����
		whiteConcede->Enabled = true;                          //���������� ������ ����� �� ���� �����
		whiteDrawOffer->Enabled = true;                        //���������� ������ ����������� ����� �� ���� �����

		blackTimerStop->Enabled = false;                     //�������������� ������������ ������ ������
		blackConcede->Enabled = false;
		blackDrawOffer->Enabled = false;
	}
	else if(currTurn == BLACK && tOpt->timerOn == true) {        //� ��������
		blackTimerStop->Enabled = true;
		blackConcede->Enabled = true;
		blackDrawOffer->Enabled = true;
		whiteTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
	}

	if(menuTimerSwitch->Checked == true) {
		if(currTurn == WHITE) {                        //� ����������� �� ����� �������, ���������� ����� ����
			blackTimer->Enabled = false;             //������������� ������ ������
			if(tOpt->someMovesLimit == true)
				whiteTotalTime -= tOpt->addTimePerMove;    //���� ��� ��������� ������ � ����� ���� ������� �������������� �����,
														//�� ���������� ���������� ����� � ������ ������
			whiteTimer->Enabled = true;                //��������� ������ �����
			if(tOpt->moveLimit == true)
				blackTotalTime = 0;
			else if(tOpt->someMovesLimit == true) {
				if(moveNotes[moveNotes.size() - 1].moveCounter == tOpt->firstPerMovesCount)
					blackTotalTime = 0;
				else if(moveNotes[moveNotes.size() - 1].moveCounter == tOpt->firstPerMovesCount + tOpt->secondPerMovesCount)
					blackTotalTime = 0;
            }
		}
		else if(currTurn == BLACK) {
			whiteTimer->Enabled = false;           //������������� ������ �����
			if(tOpt->someMovesLimit == true)
				blackTotalTime -= tOpt->addTimePerMove; //���� ��� ��������� ����� � ����� ���� ������� �������������� �����,
														//�� ���������� ���������� ����� � ������ �����
			blackTimer->Enabled = true;            //��������� ������ ������
			if(tOpt->moveLimit == true)
				whiteTotalTime = 0;
			else if(tOpt->someMovesLimit == true) {
				if(moveNotes[moveNotes.size() - 1].moveCounter == tOpt->firstPerMovesCount)
					whiteTotalTime = 0;
				else if(moveNotes[moveNotes.size() - 1].moveCounter == tOpt->firstPerMovesCount + tOpt->secondPerMovesCount)
					whiteTotalTime = 0;
			}
		}
	}

	return;
}



void __fastcall TMainChessForm::CursorMove(TObject *Sender, TShiftState Shift,        //��������� �������� ��� �������� �����
		  int X, int Y)
{
	 chessDesk* tmpDsk = chDskPaint->getChDsk();          //���������� ��������� ������-�����, ��������������

	 static pair<deskSection*, chessPiece*> pR;                       //��������� ��������� ���� <deskSection*, chessPiece*> ��� ����������
																	  //������������� ��������, ���������� �� �������� (��� static)

	if(prevSect != NULL)
		if(X >= prevSect->pixelX && X < prevSect->pixelX + SQRSIZE      //���� ������� ���������� ������� ��������� � �������� ����� ���������� ������
		&& Y >= prevSect->pixelY && Y < prevSect->pixelY + SQRSIZE)
			return;                                                    //��������� ������� ���������


	pR = tmpDsk->findSection(X, Y);                            //����������� ��������� �� ��������� ���� pR
																//�������� ����, ������ ������� �������� ���������� X, Y

	static Graphics::TBitmap* tempBit = chDskPaint->getCursorOn();

	if(pR.first->number != 0) {          //���� ������ �������� �� ���� �� ������
		if(pR.second->getChessType() != NOPIECE && pR.first->selected == false && pR.second->getChessColor() == currTurn) {                          //���� �� ���� ������ ���� �����-���� ������
																										  //� ��� ������ ��� �� ������� (�.�. �� ���� �����, �� ������ � ����������)

				if(possMovesPtr != NULL || possTakesPtr != NULL) {              //���� ���������� ��������� ��� ���� ��� ������ ���� ��������� ��������� ������
					possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //��������� ���������� ����� ����������� ���������  � ����� ��������� ��� ����
																																		 //� ���������� ���������� ����
					possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));         //��������� ���������� ����� ����������� ���������  � ����� ��������� ��� ������
					if(possMovesIt == possMovesPtr->end() && possTakesIt == possTakesPtr->end())                 //���� ��� ���������� �����������
						delSelection();                             //������� ���������� ���������																													 //� ���������� ���������� ����

					possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), pR.first));       //����� ��������� ��� ������ ����� ����. �������
																										//���� ��, �� ������� ��������� ������
					if(possTakesIt == possTakesPtr->end())                                        //���� ������� ���
						chessGraphic->Canvas->Draw(pR.first->pixelX, pR.first->pixelY, tempBit);     //������������ ������ ��������� ����
				}
				else {
					delSelection();
					chessGraphic->Canvas->Draw(pR.first->pixelX, pR.first->pixelY, tempBit);
				}

		}
		else if(pR.second->getChessType() == NOPIECE || pR.first->selected == true) {         //���� �� ���� ����������� ������ ��� ���� ��� �������

			if(possMovesPtr != NULL || possTakesPtr != NULL) {              //���� ���������� ��������� ��� ���� ��� ������ ���� ��������� ��������� ������
					possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //��������� ���������� ����� ����������� ���������  � ����� ��������� ��� ����
																																		 //� ���������� ���������� ����
					possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));         //��������� ���������� ����� ����������� ���������  � ����� ��������� ��� ������
																																		 //� ���������� ���������� ����
					if(possMovesIt == possMovesPtr->end() && possTakesIt == possTakesPtr->end())                   //���� ��� ���������� �����������
						delSelection();                    //������� ���������� ���������
			}
			else
				delSelection();

		}
		else if(pR.second->getChessType() != NOPIECE && pR.first->selected == false && pR.second->getChessColor() != currTurn) {  //���� ������ ��������� �� ����, �� ������� ����������� �����������
																															  //��� ���� ������
			pair<deskSection*, chessPiece*> prevPr;                      //��������� ��������� ����, ��������������� ���������� ���������� ������
			if(prevSect != NULL) {                                      //���� ����. ���������� ������ ����������
				prevPr = tmpDsk->findSection(prevSect->pixelX, prevSect->pixelY);        //�������������� ���� ��� ������ ������ ������ ���� �� ����������� ������
				if(prevPr.second->getChessColor() == currTurn)                           //���� ���������� ���������� ���� ��������� ������, ��������� �� ������ ����
					delSelection();                                                     //������� ���������
			}
		}
        prevSect = pR.first;              //��������� prevSect ����������� �������� ��������� ��
													//���������� (��������) ������
	}
	else {                               //���� ������ ������� �� ������� ���������� ����
		if(possMovesPtr != NULL || possTakesPtr != NULL) {
			possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //��������� ���������� ����� ����������� ���������  � ����� ��������� ��� ����
																																			 //� ���������� ���������� ����
			possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));         //��������� ���������� ����� ����������� ���������  � ����� ��������� ��� ������
			if(possMovesIt == possMovesPtr->end() && possTakesIt == possTakesPtr->end())                 //���� ��� ���������� �����������
				delSelection();                             //������� ���������� ���������
		}
		else
			delSelection();
		prevSect = pR.first;
	}

}



void __fastcall TMainChessForm::SectionClick(TObject *Sender)         //���������� ������� ����� �� ������ ���.����
{

		setSectClick();                                 //�������� �����, ���������� ������ �� �����



}


void TMainChessForm::delSelection() {                         //�������� ��������� ���� (������� �����) ��� �������� �������
															//��������� ������� ����� ��� ����������� ���� ����� � ����� ��. ��������������
		//----------------������� ���������� ����� ��������� ������---------------------------------------------------------------
		if(prevSect != NULL) {                                                   //  ���� ��������. �������. ����������

				if(prevSect->sectColor == BLACK && prevSect->selected == false)                      //���� ������ ������, � ������ �� �������
					chessGraphic->Canvas->Draw(prevSect->pixelX, prevSect->pixelY, chDskPaint->getNoCursorBlack());     //������������ ������ �������� ��� ������ ������
				else if(prevSect->sectColor == WHITE && prevSect->selected == false)                 //���� �����
					chessGraphic->Canvas->Draw(prevSect->pixelX, prevSect->pixelY, chDskPaint->getNoCursorWhite());     //��� ����� ������
		}
	 //---------------------------------------------------------------------------------------------------------------------------
}

//---------------------��������� ������� ��� ����� �� ������ ���������� ����-------------------------------------------------------
void TMainChessForm::setSectClick() {
	chessDesk* tmpDsk = chDskPaint->getChDsk();                 //������� ��������� ������-�����
	pair<deskSection*, chessPiece*> pR;                            //�������  ��������� ���� pR, ���


 //----------------------�������� ����������� ��������� ��� ����������� ���� �����------------------------------------------------------------------------
	if(prevChosenSect != NULL && prevChosenSect->selected == true) {                               //���� ���������� ��������� ��������� ���� � ����, �� ������� ���������� ���������, ��� �� �������
			pR = tmpDsk->findSection(prevChosenSect->pixelX, prevChosenSect->pixelY);      //����������� ��������� pR �� ��������� ����
																						//�������� ��������� �� ���� �� ����������� sectionsMap
																					   //���� ������� (��. �� ������ deskSection)
																						//������������� ����� ��������� ������
			Graphics::TBitmap* localBmp = new Graphics::TBitmap;               //��������� ��������� ��������� �� ������, �������� ��� ���� ������

			if(pR.first->sectColor == WHITE)                                 //� ����������� �� ����� ������, � �������� ��������� ���������
				localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/" + pR.second->getPicName().c_str());   //��������� ����������� ��� ������ �� ����� ������
			else if(pR.first->sectColor == BLACK)
				localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/" + pR.second->getPicName().c_str());   //��������� ����������� ��� ������ �� ������ ������
			chessGraphic->Canvas->Draw(pR.first->pixelX, pR.first->pixelY, localBmp);



			if(prevSect != prevChosenSect)                          //���� ������� ������ �� ��������� � ����� ���������
				prevChosenSect->selected = false;                   //������� ���� ������ ������
			delete localBmp;
	}

 //------------------------------------------------------------------------------------------------------------------------
	pR = tmpDsk->findSection(prevSect->pixelX, prevSect->pixelY);           //����������� ���� <deskSection*, chessPiece*> pR
																		//��������, ��������������� �������� ����������� ����
																	   // (�� ������� ���������� ������)

   if(pR.second->getChessType() == NOPIECE) {                      //���� ������� ���������� ���� ������

		setSectMoves(pR);                                     //�������� ����� ��������� ��������� ��� ���� �����,
															//������� � ������ ������ ������ ������ ������� � ����� ���
															//���������� ��������� ������

		if(prevChosenSect != NULL && possMovesPtr != NULL) {                //���� ���������� ��������� ������ ���������� � ���������� ��������� ��� ���� ����
			possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //� ��������� possMovesSet
																															//���� ������� ������ ��������� �� ����������
																															//���� prevSect
																															//��� ����� ���������� �������� find_f,
																															//���������������� ������� equal_to<>(T, T)
																															//� ������� �������. � stl �������  bind2nd
																															//��������� ��������� prevSect �� ������ ���������� ��������

	   //����� �������, �� ���������, �������� �� ����, �� �������� ������������ �������, ��������� ��� ���� ���������� ��������� ������
			if(possMovesIt != possMovesPtr->end()) {                  //���� ��������
				movePiece(prevSect);                               //������������ ���
				prevChosenSect = prevSect;                      //���������� ���������� ���� ����������� �������� ���� �� ������� ��������� � ������ ������ ������
				return;                                         //�������� �����, �.�. ����� �� ��������� ���, �� ������������
			}                                                   //��������� ��� ���� ���� ��� �� �����

	   }



   }


   else {                                                      //���� ������� ���������� ���� ��������
		if(prevChosenSect == prevSect) {           //���� ���������� ��������� ���� ��������� � ������� (�� ������� ����. ������),
			setSectMoves(pR);                    //�������� ����� ��������� ��������� ���  ���� �����, ������� � ������ ������ ������ ������ ����������
			if(prevChosenSect->selected == true) {
				prevChosenSect->selected = false;
				return;
			}


		}
		if(prevChosenSect != NULL && possTakesPtr != NULL) {             // ���� ���������� ��������� ������ ���������� � ���������� ��������� ��� ������ ����
			possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));
			if(possTakesIt != possTakesPtr->end()) {
					takePiece(prevSect);
					prevChosenSect = prevSect;
					chDskPaint->paintPieces();                                           //������������ ������ �� ����� ��������
					return;
			}
		}
		if(pR.second->getChessColor() != currTurn) {         //���� ���� ������, �� ������� ��� ����������� ����, ���������� �� ������ ����
				setSectMoves(pR);                                  //�������� ����� ��������� ��������� ��� ���� �����,
				return;											//������� � ������ ������ ������ ������ ������� � ����� ���
		}	   														//���������� ��������� ������
   }


//----------------------------------��������� ������ ������� ��������� ����-------------------------------------------------------
	if(prevSect->selected == false && pR.second->getChessColor() == currTurn) {          //���� ������, �� ������� ����������� ����, ��� �� �������
		map<deskSection*, chessPiece*, sectCompare>* mapPtr = tmpDsk->getMap();

		if((*mapPtr)[prevSect]->getChessType() != NOPIECE) {             //���� �� ����, �� �������� ����������� ����, ���� �����-���� ������
				delSelection();                      //������� ���������� ��������� ������ (�����)

				BLENDFUNCTION bf;                    //��������� ������ ���� BLENDFUNCTION ��� ���������� ��������������� �������
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 120;
				bf.AlphaFormat = 0;
				static Graphics::TBitmap* localBmp = chDskPaint->getSelectedSect();       //�������� �������������� ��������� ������
																							//��� �������� ������ �������
				::AlphaBlend(chessGraphic->Canvas->Handle, prevSect->pixelX, prevSect->pixelY,
							SQRSIZE,
							SQRSIZE,
							localBmp->Canvas->Handle, 0, 0,
							localBmp->Width,
							localBmp->Height, bf);


				setSectMoves(pR);                      //�������� ����� ��������� ��������� ��� ���� �����
				setSectTakes(pR);                       //�������� ����� ��������� ��������� ��� ������ �����

				if(prevSect != prevChosenSect)
					prevChosenSect = prevSect;
				prevSect->selected = true;

		}

	}



}
//--------------------------------------------------------------------------------------------------------------------------------------------
void TMainChessForm::setSectMoves(pair<deskSection*, chessPiece*> pR) {
		chessDesk* tmpDsk = chDskPaint->getChDsk();     //����������  ��������� �� ��������� ������-�����
		map<deskSection*, chessPiece*, sectCompare>* tmpMap = tmpDsk->getMap();     //���������� ��������� �� �����������,
																			 //�������� ���� deskSection*, chessPiece*
																		   //�������������� ��� ������� ���������� �������-�����
		Graphics::TBitmap* localBmp = new Graphics::TBitmap;

//--------------------������� ������� � ���������� ��������� ��� ���� �����-----------------------------------------------------
		if(possMovesPtr != NULL && !possMovesPtr->empty()) {
				for(possMovesIt = possMovesPtr->begin(); possMovesIt != possMovesPtr->end(); possMovesIt++) {
					if((*possMovesIt)->sectColor == BLACK)
						chessGraphic->Canvas->Draw((*possMovesIt)->pixelX, (*possMovesIt)->pixelY, chDskPaint->getFreeBlack());
					else if((*possMovesIt)->sectColor == WHITE)
						chessGraphic->Canvas->Draw((*possMovesIt)->pixelX, (*possMovesIt)->pixelY, chDskPaint->getFreeWhite());
				}
				for(possMovesIt = possMovesPtr->begin(); possMovesIt != possMovesPtr->end(); possMovesIt++) {       //���� ����� ��������� ��� ���� �����
						if((*possMovesIt) == prevSect)                                                              //��� ����, �� ������� � ������ ������ ��������� ������ (������ ���� �������)
							break;
				}
				if(possMovesIt == possMovesPtr->end())
					possMovesPtr->clear();                                                                         //, �� ������� ��������� ��������� ��� ���� ����� (�.�. ���� ����� �� ������� ����,
																												   //������������ ���  ����)


		}
//----------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------������� ������� � ���������� ��������� ��� ������ �����---------------------------------------------------
	if(possTakesPtr != NULL && !possTakesPtr->empty()) {
			for(possTakesIt = possTakesPtr->begin(); possTakesIt != possTakesPtr->end(); possTakesIt++) {
				if((*possTakesIt)->sectColor == WHITE)
						chessGraphic->Canvas->Draw((*possTakesIt)->pixelX, (*possTakesIt)->pixelY, chDskPaint->getFreeWhite());
				else if((*possTakesIt)->sectColor == BLACK)
						chessGraphic->Canvas->Draw((*possTakesIt)->pixelX, (*possTakesIt)->pixelY, chDskPaint->getFreeBlack());

			}

	}
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------������������ ������� ����� �� ���������� ��������� ��� ������ �����----------------------------
	if(possTakesPtr != NULL && !possTakesPtr->empty()) {
			localBmp->Transparent = true;
			for(possTakesIt = possTakesPtr->begin(); possTakesIt != possTakesPtr->end(); possTakesIt++) {       //���������� ���������� ��������� ��� ������ ����

				if((*possTakesIt)->sectColor == WHITE)                //���� ���� ���������� ��� ������ ���� - �����
					localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/" + (*tmpMap)[(*possTakesIt)]->getPicName().c_str());   //��������� ����������� ������ ��� ����� ������
				else if((*possTakesIt)->sectColor == BLACK)
					localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/" + (*tmpMap)[(*possTakesIt)]->getPicName().c_str());   //��������� ����������� ������ ��� ������ ������
				chessGraphic->Canvas->Draw((*possTakesIt)->pixelX, (*possTakesIt)->pixelY, localBmp);                  //������������ ������ � ������� ������������ ����������� chessGraphic
			}
			possTakesPtr->clear();

	}
//-------------------------------------------------------------------------------------------------------------------------------


//-----------------------------������������ ������� ��������� ��������� ��� ���� ����� ��� �������� ����---------------------------------------

		if(pR.first->number != 0 && pR.second->getChessType() != NOPIECE
		&& pR.first->selected == false && pR.second->getChessColor() == currTurn) {           //���� ���� ���������� ����� �������� ����������, � ��� ��������, � �� �������,
			if(possMovesPtr != NULL)                                                          //� �� ��� ����������� ��������� �� ������� ����  ������
				possMovesPtr->clear();                                     //������� ��������� ��������� ��� ���� ����� ����. ���������� ������
			pR.second->setMovesTakes(pR.first, tmpMap);                 //������ ������ � ��������� ��������� ��� ���� � ��������� ��� ������ �����
			possMovesPtr = pR.second->getPossMoves();                   //�������� ��� ������ ������ ��������� ��������� ��� ���� �����
		}
		else															  //� ��������� ������ ��������� �����
			return;

		if(!possMovesPtr->empty()) {
				BLENDFUNCTION bf;                    //��������� ������ ���� BLENDFUNCTION ��� ���������� ��������������� �������
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 100;
				bf.AlphaFormat = 0;
				localBmp = chDskPaint->getMovesSect();       // �������������� ��������� ������
																							//��� �������� ������ �������

				for(possMovesIt = possMovesPtr->begin(); possMovesIt != possMovesPtr->end(); possMovesIt++) {
						::AlphaBlend(chessGraphic->Canvas->Handle, (*possMovesIt)->pixelX, (*possMovesIt)->pixelY,
									SQRSIZE,
									SQRSIZE,
									localBmp->Canvas->Handle, 0, 0,
									localBmp->Width,
									localBmp->Height, bf);
				}

		}


}

void TMainChessForm::setSectTakes(pair<deskSection*, chessPiece*> pR) {
	Graphics::TBitmap* localBmp;



//--------------------------------������������ ������� ��������� ��������� ��� ������ ����� ��� �������� ����--------------------------------------------------------------

	chessDesk* tmpDsk = chDskPaint->getChDsk();                       //�������������� ��������� �� ��������� �����
	map<deskSection*, chessPiece*, sectCompare>* tmpMap = tmpDsk->getMap();      //�������������� ��������� �� ����������� sectionsMap ������ chessDesk

	if(pR.first->number != 0 && pR.second->getChessType() != NOPIECE                         //���� ���� ����������, ��� ��������,
		&& pR.first->selected == false && pR.second->getChessColor() == currTurn) {         //��� ��� �� ��������, � ��� ����������� ������ ��������� �� ������� ����
		if(possTakesPtr != NULL)
			possTakesPtr->clear();                               //������� ��������� ��������� ��� ������ ����� ����. ��������� ������
		pR.second->setMovesTakes(pR.first, tmpMap);                 //������ ������ � ��������� ��������� ��� ���� � ��������� ��� ������ �����
		possTakesPtr = pR.second->getPossTakes();             //�������� ��� ������ ���� � ������ ��������� ��������� ��� ������ �����
	}
	else {
		delete localBmp;
		return;
	}

	if(!possTakesPtr->empty()) {
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 120;
			bf.AlphaFormat = 0;
			localBmp = chDskPaint->getTakesSect();
			for(possTakesIt = possTakesPtr->begin(); possTakesIt != possTakesPtr->end(); possTakesIt++) {
					::AlphaBlend(chessGraphic->Canvas->Handle, (*possTakesIt)->pixelX, (*possTakesIt)->pixelY,
								SQRSIZE,
								SQRSIZE,
								localBmp->Canvas->Handle, 0, 0,
								localBmp->Width,
								localBmp->Height, bf);

            }

	}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


}

void TMainChessForm::movePiece(deskSection* dsPtr) {

	chessDesk* tmpDesk = chDskPaint->getChDsk();                  //��������� �  �������������� ��������� �� ������-�����
	chessDesk deskCopy = *(chDskPaint->getChDsk());                 //�������� ������ ����� �� ���������� ����� �� �� �� ���� ����������� (�����. ��� ���������� ����)
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess  = chDskPaint->getChDsk()->getWhiteChess();
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = chDskPaint->getChDsk()->getBlackChess();
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpBlackIt;
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;
	map<deskSection*, chessPiece*, sectCompare>* tmpSectionsMap = chDskPaint->getChDsk()->getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpSectionsIt;
	chessPiece* tmpPiecePtr =(*tmpSectionsMap)[prevChosenSect];											//��������� ��������� ��������� �� ������ ������, �������������
																										//��������� ��������� ����

//-----------��������� ������ � ���� � ������ ������� �����-------------------------------------------------------------------------
	if(tmpPiecePtr->getChessColor() == BLACK) {
		if(repMoveNotes.size() == 0 || repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {       //���� ���� �� �������� ������� ��� ��������, �� ������ ��� �� ���� ��������� �� �������������
			moveNotes[moveNotes.size() - 1].blackPiece.first = tmpPiecePtr->getChessType();
			moveNotes[moveNotes.size() - 1].blackPiece.second = *prevChosenSect;
			moveNotes[moveNotes.size() - 1].blackMoveSect = *dsPtr;
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();
			movesLog->Lines->Add("");
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] += "";
		}

	}
	else if(tmpPiecePtr->getChessColor() == WHITE) {
		if(moveNotes.size() == 0) {                                          //���� ������� ��� - ������
			firstMoveMade = true;
			menuSaveReplay->Enabled = false;
			if(whiteTimer->Enabled == false && repMoveNotes.size() == 0)          //���� ������ ����� �� �������, � ���� �� �������� �������
				menuSaveGame->Enabled = true;
			menuTimerSwitch->Enabled = false;
		}
		if(repMoveNotes.size() == 0 || repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {          //���� ���� �� �������� ������� ��� ��������, �� ������ ��� �� ���� ��������� �� �������������
			moveNote newNote;
			newNote.whitePiece.first = tmpPiecePtr->getChessType();
			newNote.whitePiece.second = *prevChosenSect;
			newNote.whiteMoveSect = *dsPtr;
			moveNotes.push_back(newNote);
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (newNote.getTextMove()).c_str();
		}

	}
//------------------------------------------------------------------------------------------------------------------------------------------------

	if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == BLACK) {                     //���������� � ��-�� �������������� �������,
		tmpBlackIt  = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(tmpPiecePtr));  //��������� ������������ tmpSectionsMap
																						 //���� ������ �� ��������� ��������� ���� ������

		if(tmpBlackIt != tmpBlackChess->end())                                          //����������� ��-�� �������������� ������� tmpBlackChess
																				//�������� ��������� �� ����, �� ������� ����������� ���
			(*tmpBlackChess)[tmpPiecePtr] = dsPtr;
	}


	else if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == WHITE) {      //���������� ��� ������� ����� ��������� ������
		tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));      //���� ����� ����� �����, ���� ������, �������������
		 if(tmpWhiteIt != tmpWhiteChess->end())                                                               //�� ��������� ��������� ����

			(*tmpWhiteChess)[tmpPiecePtr] = dsPtr;                           //���������� ����� ��������������� �� ���� � �����������

	}


	(*tmpSectionsMap)[prevChosenSect] = tmpDesk->getNopiece();                       //����������� ������ ������ ���������� ���������� ����
	(*tmpSectionsMap)[dsPtr] = tmpPiecePtr;                                  //����������� ����, �� ������� ������ ���, ��������� �� ������, �����
																			//������������ �� ��������� ��������� ����

	//--------------------���������� ������ ��������� ����� ����� ������--------------------------------------------------------------
	 if(tmpPiecePtr->getChessType() == PAWN) {                             //���� ������ �� ��������� ��������� ���� ���� ������,
		 tmpPiecePtr->setPrevPos(prevChosenSect);                      //�� ��������� ��� ��� ��������� �� ���������� �������,
																		//������� ����� ��������� �� ��������� ��������� ����
		if(prevChosenSect->symbol != dsPtr->symbol) {                     //���� ��������� ��� ���� ����� ��������� �� �� ����� ������������ ����� �
																		//�����, �� ������� �������������� ���
			if(tmpPiecePtr->getChessColor() == BLACK) {                   //��� ������ �����
				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), pieceCoordEqual(dsPtr->symbol, dsPtr->number + 1));       //� ����������� ����� ����� ������� �����,
																																			 //������� ������� �� �������

				tmpWhiteChess->erase(tmpWhiteIt);                            //������� �� �� �����������
				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual(dsPtr->symbol, dsPtr->number + 1));     //� ����������� ����� ���������� ����,
																																				//������� ����, ���� ������� ����� ��������� �� ����,
																																				//� �������� ���� ������� ������
				tmpSectionsIt->second = tmpDesk->getNopiece();                                                                              //�������� ������� ��-�� ����������� (������ ����� ����)
																																			//����������� ��������� �� ������ ������
			   //----------------������������� ������ ����------------------------------------------------------------
			   moveNotes[moveNotes.size() - 1].blackEp = true;
			   moveNotes[moveNotes.size() - 1].blackTake = true;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();      //����� ��������� � ��������� ���� ������ �����
			   //------------------------------------------------------------------------------------------------------
			}
			else if(tmpPiecePtr->getChessColor() == WHITE) {           //���������� ��� �����
					tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), pieceCoordEqual(dsPtr->symbol, dsPtr->number - 1));
					tmpBlackChess->erase(tmpBlackIt);
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual(dsPtr->symbol, dsPtr->number - 1));   //������� ������� �� �������
					tmpSectionsIt->second = tmpDesk->getNopiece();
					//-----------------------------������������� ������ ����-----------------------------------------
					moveNotes[moveNotes.size()- 1].whiteEp = true;
					moveNotes[moveNotes.size() - 1].whiteTake = true;
					movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
					//-----------------------------------------------------------------------------------------------
			}
		}

	 }

	//--------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------���������� ��������� (����������� �����, �.�. �����. ������ ����������� � ����� ����� (��. ����))------------------------------------------------------------------------------------------------

	if(tmpPiecePtr->getChessType() == KING && tmpPiecePtr->getChessColor() == WHITE                         //��� ������ ������ �� �������� �������
		&& prevChosenSect->symbol == 'e' && prevChosenSect->number == 1) {
		   //---------------------------------��� ������������ ������-----------------------------------
			if(dsPtr->symbol == 'g' && dsPtr->number == 1) {                                            //���� ����, �� ������� ����������� ���, ���������� ���� ��� ���������
																										  //�� ������������ ������


				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), pieceCoordEqual('h', 1));         //�������� ��������� �� ���� pair<chessPiece*, deskSection*>,
																													 //���� ������� ����� ��������� �� ����� h1

				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 1));    //�������� ��������� �� ���� pair<deskSection*, chessPiece*>,
																													//���� ������� ����� ����, �� ������� ������������ ����� ����� ���������
				(*tmpWhiteChess)[(*tmpWhiteIt).first] = (*tmpSectionsIt).first;                                    //��-�� �����. �������, �����. ������������ ����� ����������� ����� ����.
																												//(��������� �� ����, ���������� ����� �����������)
				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('h', 1));    //��-�� �����. �������, �������. ����, � �������� �����. �����
				(*tmpSectionsIt).second = tmpDesk->getNopiece();                                                     //����������� ����. - ��������� �� ������ ������
				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 1));   //��-�� �����. �������, �����. ����, �� ������� �����. �����
				(*tmpSectionsMap)[(*tmpSectionsIt).first] =  (*tmpWhiteIt).first;                                     //�����. ����. - ��������� �� �����.

				moveNotes[moveNotes.size() - 1].whiteShortCas = true;                                   //������������� ���� �������� ��������� � ������ ����
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
			}
			//--------------------------------------------------------------------------------------------

			//-----------------------------------��� ��������� ������--------------------------------------------------
			else if(dsPtr->symbol == 'c' && dsPtr->number == 1) {
					tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), pieceCoordEqual('a', 1));
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 1));
					(*tmpWhiteChess)[(*tmpWhiteIt).first] = (*tmpSectionsIt).first;
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('a', 1));
					(*tmpSectionsIt).second = tmpDesk->getNopiece();
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 1));
					(*tmpSectionsIt).second = (*tmpWhiteIt).first;

					moveNotes[moveNotes.size() - 1].whiteLongCas = true;                             // ������������� ���� ������� ��������� � ������ ����
					movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
			}
		   //---------------------------------------------------------------------------------------------------------
	}

	else if(tmpPiecePtr->getChessType() == KING && tmpPiecePtr->getChessColor() == BLACK
			&& prevChosenSect->symbol == 'e' && prevChosenSect->number == 8) {
			//---------------------------------��� ������������ ������---------------------------------------------------------
			if(dsPtr->symbol == 'g' &&  dsPtr->number == 8) {

					tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), pieceCoordEqual('h', 8));
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 8));
					(*tmpBlackChess)[(*tmpBlackIt).first] = (*tmpSectionsIt).first;
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('h', 8));
					(*tmpSectionsIt).second = tmpDesk->getNopiece();
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 8));
					(*tmpSectionsMap)[(*tmpSectionsIt).first] = (*tmpBlackIt).first;

					moveNotes[moveNotes.size() - 1].blackShortCas = true;                          //������������� ���� �������� ��������� � ������ ����

			}
			//-------------------------------------------------------------------------------------------------------------------
			//------------------------------------���  ��������� ������-----------------------------------------------------------
			else if(dsPtr->symbol == 'c' && dsPtr->number == 8) {
					tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), pieceCoordEqual('a', 8));
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 8));
					(*tmpBlackChess)[(*tmpBlackIt).first] =(*tmpSectionsIt).first;
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('a', 8));
					(*tmpSectionsIt).second = tmpDesk->getNopiece();
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 8));
					(*tmpSectionsIt).second = (*tmpBlackIt).first;
					moveNotes[moveNotes.size() - 1].blackLongCas = true;                        //������������� ���� ������� ��������� � ������ ����
					movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
			}
			//-------------------------------------------------------------------------------------------------------------------

	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	//-----------------------------���������� ����������� �����-------------------------------------------------------------------------------------
	if(tmpPiecePtr->getChessType() == PAWN) {                  //���� ��������� ������ - �����
		chessPiece* pPtr;                                     //��������� ��������� ��������� �� ������-���. ������
		if(tmpPiecePtr->getChessColor() == WHITE) {           //���� ��������� ������ �����
			if(dsPtr->number == 8) {                          //���� ����, �� ������� ��������������� ��� ����������� �� 8-�� �����������
				if(repMoveNote::moveCounter == 0)           	//���� ���� �� �������� �������
					pawnTransform();                              //�������� �����, �������������� ���� ������ ������
				else if(repMoveNote::moveCounter > 0)           //���� ������� ������
					choosePiece = repMoveNotes[repMoveNote::currMove].whiteTransPiece;     //����������, �������� ��� ��������� ������, ���������� ��� ��������� ��������
                                                                                            //��� �������� ���� ������
				//----------------������������� ������ ����-------------------------------------------------------------
				moveNotes[moveNotes.size() - 1].whiteTrans = true;                            //������������� ���� ����������� ����� �����
				moveNotes[moveNotes.size() - 1].whiteTransPiece = choosePiece;                //��������� ��� ������, ���������� ����� �����������
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
				//--------------------------------------------------------------------------------------------------------
				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));      //������� ��������� ����� � ����������� ����� �����
				tmpWhiteChess->erase(tmpWhiteIt);             //�������� �������, ������ �������� �������� ��������� �����, �� �����������
				int i = 1;                                    //�.� ����� ����� �� ���������� 8-�� ����������� ����������������
				while(true) {                                 //� �����
					   pPtr = chDskPaint->getChDsk()->pieceFromArr(WHITE, choosePiece, i);     //� ������� ����� ���� ������, ��������������� ��������� ����� �����
																							   //��� ���� � ����� �������������� ����� ������
					   if(pPtr->getChessType() != NOPIECE) {                      //���� ������ � ����� ������� ������� � �������
						   tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(pPtr));      //���� �� ����� ��� ��������� � ����������� ����� �����
						   if(tmpWhiteIt == tmpWhiteChess->end()) {                     //���� � ����������� ����� ����� �� ���,
								tmpWhiteChess->insert(make_pair(pPtr, dsPtr));         //��������� �� � �����������
								(*tmpSectionsMap)[dsPtr] = pPtr;                      //� ����������� ��������� �����, ���� �� ������� ����������� ��� �����������
								break;                                                //��������� ������
						   }
					   }
					   else if(pPtr->getChessType() == NOPIECE)                    //���� ������ � �������� ������� ��� � �����. �������
							break;                                                 //��������� ����
						i++;                                                       //�������������� ����� ������
				}
				if(tmpWhiteIt != tmpWhiteChess->end()) {                          //���� ������ �� ������� ��� ������������ � ����������� ����� �����
					chessPiece* piecePtr;                                        //��������� ��������� ��������� �� ������

					switch(choosePiece) {                                    //� ����������� �� ���� ��������� ������
					case KNIGHT: {                                          //���� ��� ����
						while(true) {                                      //� �����
							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)) {    //��������� ���� �� ������ � �������� ������� � �����. ������ ����. �� ���. ������
								piecePtr = new Knight(WHITE, i);                //���� ����� ������ � ������ ���, �� �������� ��� ��� ������, ��������������
								chDskPaint->getChDsk()->addPiece(piecePtr);     //c ������� ������������ ������ ��������� ��������� �� ����� ������ �
																				//������ ���������� �� ���. ������
								break;
							}
							i++;
						}
						break;
					}
					case BISHOP: {                                           //���������� ��� ��������� �����
						while(true) {
							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)) {
								piecePtr = new Bishop(WHITE, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case ROOK: {
						while(true) {
							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)){
								piecePtr = new Rook(WHITE, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}

						break;
					}
					case QUEEN: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)) {
								piecePtr = new Queen(WHITE, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					default:
						break;
					}
					tmpWhiteChess->insert(make_pair(piecePtr, dsPtr));               //������, ����������� � ������ ���. �����, ����� ��������� � � �����������
																					//����� �����
					(*tmpSectionsMap)[dsPtr] = piecePtr;                            //����, �� ������� ��� ����������� ���. ����������� ����������� ����� ������
				}


			}
		}

		if(tmpPiecePtr->getChessColor() == BLACK) {                             //���������� ��� ������ �����
			if(dsPtr->number == 1) {
				if(repMoveNote::moveCounter == 0)                          //���� ���� �� �������� �������
					pawnTransform();                                       //�������� ����� ��������� ���� ������ ������, � ������� ���������������� �����
				else if(repMoveNote::moveCounter > 0)                      //���� ������� ������
					choosePiece = repMoveNotes[repMoveNote::currMove].blackTransPiece;       //����������, �������� ��� ������, � ���. ������������ �����, ����������� ��� ��������� ��������
																							 //��� �������� ���� ������
				//---------------------������������� ������ ����-----------------------------------------------------
				moveNotes[moveNotes.size() - 1].blackTrans = true;                  //������������� ���� ����������� ������ �����
				moveNotes[moveNotes.size() - 1].blackTransPiece = choosePiece;      //��������� ��� ������, ���������� ����� �����������
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
				//-------------------------------------------------------------------------------------------------------
				tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(tmpPiecePtr));
				tmpBlackChess->erase(tmpBlackIt);
				int i = 1;
				while(true) {
						bool flag1 = (choosePiece == KNIGHT) ? true : false;
						bool flag2 = (choosePiece == BISHOP) ? true : false;
					   pPtr = chDskPaint->getChDsk()->pieceFromArr(BLACK, choosePiece, i);
					   if(pPtr->getChessType() != NOPIECE) {
						   tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(pPtr));
						   if(tmpBlackIt == tmpBlackChess->end()) {
								tmpBlackChess->insert(make_pair(pPtr, dsPtr));
								(*tmpSectionsMap)[dsPtr] = pPtr;
								break;
						   }
					   }
					   else if(pPtr->getChessType() == NOPIECE)
							break;
						i++;
				}
				if(tmpBlackIt != tmpBlackChess->end()) {
					chessPiece* piecePtr;

					switch(choosePiece) {
					case KNIGHT: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Knight(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case BISHOP: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Bishop(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case ROOK: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Rook(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case QUEEN: {
						while(true) {
							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Queen(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
						}
					}
					default:
						break;
					}
					tmpBlackChess->insert(make_pair(piecePtr, dsPtr));
					(*tmpSectionsMap)[dsPtr] = piecePtr;
				}


			}
		}
	}

 //-------------------------------------------------------------------------------------------------------------------------------------
	if(tmpPiecePtr->getChessType() == ROOK || tmpPiecePtr->getChessType() == KING)                 //���� ������, ������� ����������� ���, �������� ������ ��� �������,
		tmpPiecePtr->setIsMoved(true);                                                             //������������� ���� ���������� ���� � ��������� true;


	dsPtr->selected = false;                                    //����� ���������� ���� ������� ���� ��������� � ������, �� ������� ��� ������ ���
	makeMove();                                                 //�������� ����� ���������� ���� (����� ������ TMainChessForm)
	if(possMovesPtr != NULL)
		possMovesPtr->clear();                                   //������� ��������� ��������� �����
	if(possTakesPtr != NULL)
		possTakesPtr->clear();								//������� ��������� ��������� ��� ������ �����
	chDskPaint->paintPieces();                                           //������������ ����� ������� �����


}

void TMainChessForm::takePiece(deskSection* dsPtr) {

		chessDesk* tmpDesk = chDskPaint->getChDsk();
		chessDesk deskCopy = *tmpDesk;
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = chDskPaint->getChDsk()->getWhiteChess();
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = chDskPaint->getChDsk()->getBlackChess();
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpBlackIt;
		map<deskSection*, chessPiece*, sectCompare>* tmpSectionsMap = chDskPaint->getChDsk()->getMap();
		chessPiece* tmpPiecePtr = (*tmpSectionsMap)[prevChosenSect];                      //��������� ��������� �� ������, ������������� �� ���������� ��������� ����

 //-----------��������� ������ � ���� � ������ ������� �����-------------------------------------------------------------------------
	if(tmpPiecePtr->getChessColor() == BLACK) {
		if(repMoveNotes.size() == 0  || repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {
			moveNotes[moveNotes.size() - 1].blackPiece.first = tmpPiecePtr->getChessType();
			moveNotes[moveNotes.size() - 1].blackPiece.second = *prevChosenSect;
			moveNotes[moveNotes.size() - 1].blackMoveSect = *dsPtr;
			moveNotes[moveNotes.size() - 1].blackTake = true;
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();
			movesLog->Lines->Add("");
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] += "";
		}
	}
	else if(tmpPiecePtr->getChessColor() == WHITE) {
		if(repMoveNotes.size() == 0 || repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {      //���� ���� �� �������� ������� ��� ��������, �� ������ ��� �� ���� �� ��� ��������� �������������
			moveNote newNote;
			newNote.whitePiece.first = tmpPiecePtr->getChessType();
			newNote.whitePiece.second = *prevChosenSect;
			newNote.whiteMoveSect = *dsPtr;
			newNote.whiteTake = true;
			moveNotes.push_back(newNote);
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = newNote.getTextMove().c_str();
		}

	}
//------------------------------------------------------------------------------------------------------------------------------------------------


		if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == BLACK) {                 //���� ������ �� ��������� ��������� ���� ������

			(*tmpBlackChess)[tmpPiecePtr] = dsPtr;                                     //� ����������� ������ ����� ���������� �� �� ����, �� ������� ��������.
																						//������ ���������������  ������
			 tmpPiecePtr = (*tmpSectionsMap)[prevSect];                                //��������� ��������� �� ������, ������ ��������� ��� �� ������
			 tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));
			 if(tmpWhiteIt != tmpWhiteChess->end())                                                           //���� ������� ���� �� ��������� ����� �����
				tmpWhiteChess->erase(tmpWhiteIt);            //�� ����������� ����� ����� ������� ����, ���� �������, �������������
																						//����, �� ������� ���� ������

		}
		else if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == WHITE) {           //����������, ���� ������ �� ���������  ��������� ���� �����
			(*tmpWhiteChess)[tmpPiecePtr] = dsPtr;
			tmpPiecePtr = (*tmpSectionsMap)[prevSect];
			tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(tmpPiecePtr));
			if(tmpBlackIt != tmpBlackChess->end())
				tmpBlackChess->erase(tmpBlackIt);
		}
		 tmpPiecePtr = (*tmpSectionsMap)[prevChosenSect];                         //��������� ��������� ����� �� ���������� ��������� ������
		(*tmpSectionsMap)[prevChosenSect] = tmpDesk->getNopiece();
		(*tmpSectionsMap)[dsPtr] = tmpPiecePtr;

//-----------------------------���������� ����������� �����-------------------------------------------------------------------------------------
	if(tmpPiecePtr->getChessType() == PAWN) {                  //���� ��������� ��������� ��������������� � ������, ����������� ��� �������
		chessPiece* pPtr;
		if(tmpPiecePtr->getChessColor() == WHITE) {
			if(dsPtr->number == 8) {
				if(repMoveNote::moveCounter == 0)             //���� ���� �� �������� �������
					pawnTransform();                          //�������� ����� ������ ������, � ������� ������������ �����
				else if(repMoveNote::moveCounter > 0)          //���� ���� �������� �������
					choosePiece = repMoveNotes[repMoveNote::currMove].whiteTransPiece;        //� �������� ��������� ������ ����������� ����� ��������� ������, ��� ������� ������� � �������-���� ������
				//------------------------������������� ������ ����-------------------------------------------------------------------------
				moveNotes[moveNotes.size() - 1].whiteTrans = true;
				moveNotes[moveNotes.size() - 1].whiteTransPiece = choosePiece;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
				//---------------------------------------------------------------------------------------------------------------------------
				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));
				tmpWhiteChess->erase(tmpWhiteIt);
				int i = 1;
				//---------------���������, ���� �� � ������� �������� ��������� ����� ��������� ������ (�� ��������� � ����������� � ������������ ���. �����)
				while(true) {
					   pPtr = chDskPaint->getChDsk()->pieceFromArr(WHITE, choosePiece, i);
					   if(pPtr->getChessType() != NOPIECE) {
						   tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(pPtr));
						   if(tmpWhiteIt == tmpWhiteChess->end()) {
								tmpWhiteChess->insert(make_pair(pPtr, dsPtr));
								(*tmpSectionsMap)[dsPtr] = pPtr;
								break;
						   }
					   }
					   else if(pPtr->getChessType() == NOPIECE)
							break;
						i++;
				}
				if(tmpWhiteIt != tmpWhiteChess->end()) {
					chessPiece* piecePtr;

					switch(choosePiece) {
					case KNIGHT: {
						while(true) {
							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)) {
								piecePtr = new Knight(WHITE, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case BISHOP: {
						while(true) {
							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)) {
								piecePtr = new Bishop(WHITE, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case ROOK: {
						while(true) {
							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)){
								piecePtr = new Rook(WHITE, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}

						break;
					}
					case QUEEN: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)) {
								piecePtr = new Queen(WHITE, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					default:
						break;
					}
					tmpWhiteChess->insert(make_pair(piecePtr, dsPtr));
					(*tmpSectionsMap)[dsPtr] = piecePtr;
				}
                //-----------------------------------------------------------------------------------------------------------------------------------------

			}
		}

		if(tmpPiecePtr->getChessColor() == BLACK) {
			if(dsPtr->number == 1) {
				if(repMoveNote::moveCounter == 0)
					pawnTransform();
				else if(repMoveNote::moveCounter > 0)
                    choosePiece = repMoveNotes[repMoveNote::currMove].blackTransPiece;
				//----------------------------------������������� ������ ����------------------------------------------------------
				moveNotes[moveNotes.size() - 1].blackTrans = true;
				moveNotes[moveNotes.size() - 1].blackTransPiece = choosePiece;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
                //---------------------------------------------------------------------------------------------------------------
				tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(tmpPiecePtr));
				tmpBlackChess->erase(tmpBlackIt);
				int i = 1;
				while(true) {
						bool flag1 = (choosePiece == KNIGHT) ? true : false;
						bool flag2 = (choosePiece == BISHOP) ? true : false;
					   pPtr = chDskPaint->getChDsk()->pieceFromArr(BLACK, choosePiece, i);
					   if(pPtr->getChessType() != NOPIECE) {
						   tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(pPtr));
						   if(tmpBlackIt == tmpBlackChess->end()) {
								tmpBlackChess->insert(make_pair(pPtr, dsPtr));
								(*tmpSectionsMap)[dsPtr] = pPtr;
								break;
						   }
					   }
					   else if(pPtr->getChessType() == NOPIECE)
							break;
						i++;
				}
				if(tmpBlackIt != tmpBlackChess->end()) {
					chessPiece* piecePtr;

					switch(choosePiece) {
					case KNIGHT: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Knight(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case BISHOP: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Bishop(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case ROOK: {
						while(true) {

							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Rook(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
							i++;
						}
						break;
					}
					case QUEEN: {
						while(true) {
							if(!chDskPaint->getChDsk()->addNumberExist(i, BLACK)) {
								piecePtr = new Queen(BLACK, i);
								chDskPaint->getChDsk()->addPiece(piecePtr);
								break;
							}
						}
					}
					default:
						break;
					}
					tmpBlackChess->insert(make_pair(piecePtr, dsPtr));
					(*tmpSectionsMap)[dsPtr] = piecePtr;
				}


			}
		}
	}

 //-------------------------------------------------------------------------------------------------------------------------------------

		chDskPaint->getChDsk()->clearAddPiece();                      //������� ������, ���������� �������������� ������� (�����
																	// �����. �����), ���� ��� ���� �����

		if(tmpPiecePtr->getChessType() == ROOK || tmpPiecePtr->getChessType() == KING)                 //���� ������, ������� ����������� ���, �������� ������ ��� �������,
			tmpPiecePtr->setIsMoved(true);                                                             //������������� ���� ���������� ���� � ��������� true;

		dsPtr->selected = false;
		makeMove();
		if(possMovesPtr != NULL)
			possMovesPtr->clear();                  //�.�. �� ���� ������ �� �������
		if(possTakesPtr != NULL)
			possTakesPtr->clear();
		chDskPaint->paintPieces();


}

void TMainChessForm::pawnTransform() {                        //���������� ������ ����������� ����� � ������ ������

	pieceChooseForm = new TForm(this);                //��������� � �������������� ��������� �� ����� ��� ������ ������
															//���������� ����� �������� ������� ����� TMainChessForm, ��������� ��
															//��� ���������� � �������� ��������� ������������
	pieceChooseForm->Position = poScreenCenter;

	TBorderIcons tempBI = pieceChooseForm->BorderIcons;       //������������ ������ ������ �����: �������, �������� � �.�.
	tempBI >> biSystemMenu;
	pieceChooseForm->BorderIcons = tempBI;

	pieceChooseImage = new TImage(pieceChooseForm);    //��������� � �������������� ��������� �� ��������� ��� ������ �����������
															   //��������� �� �������-��������� �������� � �������� ��������� �����������

	pieceChooseImage->Parent = pieceChooseForm;            //��� ����������� ����������� ����������� ����������� ���������� ���������
															//������������ ���������
	Graphics::TBitmap *fieldBit = new Graphics::TBitmap;    //��������� � ���������� ������ ���� (��������� ����)

	Graphics::TBitmap *pieceBit = new Graphics::TBitmap;    //��������� � ���������� ������ ������
	fieldBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "choose_piece.bmp");             //��������� ����������� ��� ������� ����
	pieceChooseImage->Left = 12;                           //������ ��������� ���������� TImage
	pieceChooseImage->Top = 12;
	pieceChooseImage->Enabled = true;
	pieceChooseImage->Visible = true;
	pieceChooseImage->Width = 400;
	pieceChooseImage->Height = 100;



	pieceChooseForm->Caption = "Choose new piece";
	pieceChooseForm->Width = 450;
	pieceChooseForm->Height = 150;
	pieceChooseImage->Canvas->Draw(0, 0, fieldBit);
	if(currTurn == WHITE) {                                   //���� ������������� ����� �����
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Knight_wt.bmp");        //��������� �������. ����������� ����� ����� ��� ������� ������
		pieceChooseImage->Canvas->Draw(0, 0, pieceBit);         //������������ ������� ����� ����� � ������ ������
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Bishop_wt.bmp");
		pieceChooseImage->Canvas->Draw(100, 0, pieceBit);
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Rook_wt.bmp");
		pieceChooseImage->Canvas->Draw(200, 0, pieceBit);
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Queen_wt.bmp");
		pieceChooseImage->Canvas->Draw(300, 0, pieceBit);
	}
	else if(currTurn == BLACK) {                              //����������, ���� ������������� ����� ������
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Knight_bl.bmp");
			pieceChooseImage->Canvas->Draw(0, 0, pieceBit);
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Bishop_bl.bmp");
			pieceChooseImage->Canvas->Draw(100, 0, pieceBit);
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Rook_bl.bmp");
			pieceChooseImage->Canvas->Draw(200, 0, pieceBit);
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Queen_bl.bmp");
			pieceChooseImage->Canvas->Draw(300, 0, pieceBit);

    }



	pieceChooseImage->OnMouseMove = transCursorMove;               //������� ������������ ������� �� ���������� ����������� �������� ����������� ���������. �����
	pieceChooseImage->OnClick = transCursorClick;                 //������� ����� �� ���������� ����������� �������� ����������� ���������. �����


	pieceChooseForm->ShowModal();


	delete pieceChooseForm;
	delete fieldBit;
	delete pieceBit;
	pieceChooseForm = NULL;

}



void __fastcall TMainChessForm::transCursorMove(TObject* Sender, TShiftState Shift, int X, int Y) {     //�����, ������������ ��� ������� ������������ ������� �� ���������� ����������� ��������
	static chessType oldPiece = NOPIECE;                                         //��������� � �������������� ��������� ���������� ���� chessType
	static Graphics::TBitmap* selected = chDskPaint->getCursorOn();               //�������� ��������� � �������������� ��������� ��������� �� �����. �������
	static Graphics::TBitmap* unsel_wt = chDskPaint->getNoCursorWhite();
	static Graphics::TBitmap* unsel_bl = chDskPaint->getNoCursorBlack();
	if(X >= 0 && X < 100)                                  //� ����������� �� ����������  � ������� ������������� �������� ���� choosePiece  ������  this
		choosePiece = KNIGHT;
	else if(X >= 100 && X < 200)
		choosePiece = BISHOP;
	else if(X >= 200 && X < 300)
		choosePiece = ROOK;
	else if(X >= 300 && X < 400)
		choosePiece = QUEEN;
	if(choosePiece != oldPiece) {                      //���� ������� ������ �� ����� ����������

		 switch(oldPiece) {                          //� ����������� �� ���� ���������� ������
		case KNIGHT:
			pieceChooseImage->Canvas->Draw(0, 0, unsel_wt);     //����������� ����������. ������ �������� ���������
			break;
		case BISHOP:
			pieceChooseImage->Canvas->Draw(100, 0, unsel_bl);
			break;
		case ROOK:
			pieceChooseImage->Canvas->Draw(200, 0, unsel_wt);
			break;
		case QUEEN:
			pieceChooseImage->Canvas->Draw(300, 0, unsel_bl);
			break;
		default:
			break;
		}


		switch(choosePiece) {                          //� ����������� �� ���� ������� ������
		case KNIGHT:
			pieceChooseImage->Canvas->Draw(0, 0, selected);     //����������� ����������. ������ ���������
			break;
		case BISHOP:
			pieceChooseImage->Canvas->Draw(100, 0, selected);
			break;
		case ROOK:
			pieceChooseImage->Canvas->Draw(200, 0, selected);
			break;
		case QUEEN:
			pieceChooseImage->Canvas->Draw(300, 0, selected);
		default:
			break;

		}
		oldPiece = choosePiece;

	}


}

void __fastcall TMainChessForm::transCursorClick(TObject* Sender) {        //�����, ������������ ��� ������� ����� �� ���������� ����������� ��������
   pieceChooseForm->Close();
}




void __fastcall TMainChessForm::menuNewGameClick(TObject *Sender)
{
	newGame();
    return;
}
//---------------------------------------------------------------------------

void __fastcall TMainChessForm::menuCloseGameClick(TObject *Sender)
{
	MainChessForm->Close();
    return;
}
//---------------------------------------------------------------------------

void __fastcall TMainChessForm::parentFormClose(TObject* Sender) {
	TButton* locButtPtr = dynamic_cast<TButton*>(Sender);          //����������� �������� ����� ��� ���������� � ���� ������
	if(locButtPtr->Caption == "New game")                   //���� ��� ������ ������ ����� ����
		newGame();                                        //�������� ����� ������ ����� ����
	else if(locButtPtr->Caption == "Save replay")            //���� ��� ������ ���������� ����
		menuSaveReplayClick(menuSaveReplay);                           //�������� �����, ������������� ��� ������ ������ SaveGame ��������� ����
	else if(locButtPtr->Caption == "New replay")          //���� ��� ������ �������� ������ ������
		menuOpenReplayClick(menuOpenReplay);
	TForm* locFormPtr = dynamic_cast<TForm*> (locButtPtr->Parent);    //����������� �������� ����� ��� ����������-��������
																	  //� ���� �����
	locFormPtr->Close();                                              //��������� �����
	return;
}


void TMainChessForm::saveDeskNote() {
	map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPaint->getChDsk()->getMap();
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = chDskPaint->getChDsk()->getBlackChess();
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = chDskPaint->getChDsk()->getWhiteChess();
	set<deskSection*, lessSectPtr>* tmpPossSet;

	map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpChessIt;
	set<deskSection*, lessSectPtr>::iterator tmpPossIt;


	int counter = 0;
	unsigned char maxSameDesks = 1;
	deskNote dNote;
	for(tmpIt = tmpMap->begin(); tmpIt != tmpMap->end(); tmpIt++) {
		dNote.chColor[counter] = (*tmpIt).second->getChessColor();
		dNote.chType[counter] = (*tmpIt).second->getChessType();
		if((*tmpIt).first->number == 4 && dNote.chType[counter] == PAWN && dNote.chColor[counter] == BLACK) {
			tmpPossSet = (*tmpIt).second->getPossMoves();
			for(tmpPossIt = tmpPossSet->begin(); tmpPossIt != tmpPossSet->end(); tmpPossIt++) {
				if((*tmpPossIt)->symbol != (*tmpIt).first->symbol)
					dNote.blackEpAble = true;
			}
			tmpPossSet->clear();
		}
		if((*tmpIt).first->number == 5 && dNote.chType[counter] == PAWN && dNote.chColor[counter] == WHITE) {
			tmpPossSet = (*tmpIt).second->getPossMoves();
			for(tmpPossIt = tmpPossSet->begin(); tmpPossIt != tmpPossSet->end(); tmpPossIt++) {
				if((*tmpPossIt)->symbol != (*tmpIt).first->symbol)
					dNote.whiteEpAble = true;
			}
			tmpPossSet->clear();

		}
		if((*tmpIt).first->number == 1 && (*tmpIt).first->symbol == 'e' &&
			(*tmpIt).second->getChessType() == KING && (*tmpIt).second->getChessColor() == WHITE) {
				(*tmpIt).second->setMovesTakes((*tmpIt).first, tmpMap);
				tmpPossSet = (*tmpIt).second->getPossMoves();
				for(tmpPossIt = tmpPossSet->begin(); tmpPossIt != tmpPossSet->end(); tmpPossIt++) {
					if((*tmpPossIt)->number == 1 && (*tmpPossIt)->symbol == 'g')
						dNote.whiteShortCasAble = true;
					else if((*tmpPossIt)->number == 1 && (*tmpPossIt)->symbol == 'c')
						dNote.whiteLongCasAble = true;
				}
				tmpPossSet->clear();
				tmpPossSet = (*tmpIt).second->getPossTakes();
				tmpPossSet->clear();
		}
		if((*tmpIt).first->number == 8 && (*tmpIt).first->symbol == 'e' &&
			(*tmpIt).second->getChessType() == KING && (*tmpIt).second->getChessColor() == BLACK) {
				(*tmpIt).second->setMovesTakes((*tmpIt).first, tmpMap);
				tmpPossSet = (*tmpIt).second->getPossMoves();
				for(tmpPossIt = tmpPossSet->begin(); tmpPossIt != tmpPossSet->end(); tmpPossIt++) {
						if((*tmpPossIt)->number == 8 && (*tmpPossIt)->symbol == 'g')
							dNote.blackShortCasAble = true;
						else if((*tmpPossIt)->number == 8 && (*tmpPossIt)->symbol == 'c')
							dNote.blackLongCasAble = true;
				}
				tmpPossSet->clear();
				tmpPossSet = (*tmpIt).second->getPossTakes();
				tmpPossSet->clear();
		}

		counter++;

	}
	dNote.lastTurn = currTurn;
	deskNotes.push_back(dNote);
	unsigned char tmpMax = 1;
	for(int i = 0; i < deskNotes.size(); i++) {

		for(int j = i + 1; j < deskNotes.size(); j++) {
			if(deskNotes[i] == deskNotes[j])
				tmpMax++;
		}
		if(tmpMax > maxSameDesks)
			maxSameDesks = tmpMax;
		tmpMax = 1;

	}
	sameDesksCount = maxSameDesks;
  //	blackTimerMemo->Lines->Strings[2] = "Same positions: " + AnsiString(sameDesksCount);
  //	whiteTimerMemo->Lines->Strings[2] = "Same positions: " + AnsiString(sameDesksCount);
	samePosInGame->Caption = "Same positions: " + AnsiString(sameDesksCount);


}

void TMainChessForm::restoreDeskNote(deskNote dN) {             	//�����, ����������������� ����� ����������� ��������� ��������� �����
	map<deskSection*, chessPiece*, sectCompare>* locMap = chDskPaint->getChDsk()->getMap();
	map<chessPiece*, deskSection*, lessPiecePtr>* locBlackChess = chDskPaint->getChDsk()->getBlackChess();
	map<chessPiece*, deskSection*, lessPiecePtr>* locWhiteChess = chDskPaint->getChDsk()->getWhiteChess();

	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator locBlackIt;
	map<chessPiece*, deskSection, lessPiecePtr>::iterator locWhiteIt;

	char sym = 'a';               //��������� �������� �������� ����� ���. �����
	int num = 1;

	locMap->clear();                     //������� ����������� ���. �����, ����� ����� � ������ ����� ����� ����������� ��������� ���. �����
	locBlackChess->clear();
	locWhiteChess->clear();
	chDskPaint->getChDsk()->clearAddPiece();         //������� ������ �������������� ����� � �������������� ������ �����

	unsigned short blPawnCount = 0, blKnightCount = 0, blBishopCount = 0, blRookCount = 0, blQueenCount = 0;           //����������, �������� ���-�� ������ ����� ������������� ����
	unsigned short wtPawnCount = 0, wtKnightCount = 0, wtBishopCount = 0, wtRookCount = 0, wtQueenCount = 0;
	//-----------��������� ������� ���� deskNote, �������� ��������� ���. ����� ����� ������� ����, ��������������� ����������� ��������� � ������������ ���------------
	for(int i = 0; i < SECTCOUNT; i++) {                  //��� ������� �� 64 ���. �����
		if(dN.chColor[i] == BLACK) {                    //���� ���� ������, ������������� �� ����, ������
			switch(dN.chType[i]) {                     //� ����������� �� ���� ������
			case PAWN: {
				blPawnCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(BLACK, PAWN, blPawnCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				num++;
				if(num > 8) {
					num = 1;
					sym++;
                }

				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locBlackChess->insert(locPiecePair);
				break;
			}
			case KNIGHT: {
				blKnightCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(BLACK, KNIGHT, blKnightCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Knight(BLACK, blKnightCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
                }
				num++;
				if(num > 8) {
					num = 1;
					sym++;
                }

				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locBlackChess->insert(locPiecePair);
				break;
			}
			case BISHOP: {
				blBishopCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(BLACK, BISHOP, blBishopCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Bishop(BLACK, blBishopCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
				}
				num++;
				if(num > 8) {
					num = 1;
					sym++;
				}
				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*>locPiecePair = make_pair(locPiece, locSect);
				locBlackChess->insert(locPiecePair);
				break;
			}
			case ROOK: {
				blRookCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(BLACK, ROOK, blRookCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Rook(BLACK, blRookCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
                }
				num++;
				if(num > 8) {
					num = 1;
					sym++;
				}
				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece* ,deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locBlackChess->insert(locPiecePair);
				break;
			}
			case QUEEN: {
				blQueenCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(BLACK, QUEEN, 1);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Queen(BLACK, blQueenCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
                }
				num++;
				if(num > 8) {
					num = 1;
					sym++;
                }

				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locBlackChess->insert(locPiecePair);
				break;
			}
			case KING: {
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(BLACK, KING, 1);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				num++;
				if(num > 8) {
					num = 1;
					sym++;
				}
				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locBlackChess->insert(locPiecePair);
				break;
			}
			default:
				ShowMessage("Desk restore error!");
				return;
			}
		}
		else if(dN.chColor[i] == WHITE) {
			switch(dN.chType[i]) {
			case PAWN: {
				wtPawnCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(WHITE, PAWN, wtPawnCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				num++;
				if(num > 8) {
					num = 1;
					sym++;
				}
				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locWhiteChess->insert(locPiecePair);
				break;
			}
			case KNIGHT: {
				wtKnightCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(WHITE, KNIGHT, wtKnightCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Knight(WHITE, wtKnightCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
                }
				num++;
				if(num > 8) {
					num = 1;
					sym++;
				}
				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locWhiteChess->insert(locPiecePair);
				break;
			}
			case BISHOP: {
				wtBishopCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(WHITE, BISHOP, wtBishopCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Bishop(WHITE, wtBishopCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
                }
				num++;
				if(num > 8) {
					num = 1;
					sym++;
				}
				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*>locPiecePair = make_pair(locPiece, locSect);
				locWhiteChess->insert(locPiecePair);
				break;
			}
			case ROOK: {
				wtRookCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(WHITE, ROOK, wtRookCount);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Rook(WHITE, wtRookCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
                }
				num++;
				if(num > 8) {
					num = 1;
					sym++;
				}
				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece* ,deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locWhiteChess->insert(locPiecePair);
				break;
			}
			case QUEEN: {
				wtQueenCount++;
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(WHITE, QUEEN, 1);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				if(locPiece->getChessType() == NOPIECE) {
					locPiece = new Queen(WHITE, wtQueenCount);
					chDskPaint->getChDsk()->addPiece(locPiece);
                }
				num++;
				if(num > 8) {
					num = 1;
					sym++;
                }

				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locWhiteChess->insert(locPiecePair);
				break;
			}
			case KING: {
				chessPiece* locPiece = chDskPaint->getChDsk()->pieceFromArr(WHITE, KING, 1);
				deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
				num++;
				if(num > 8) {
					num = 1;
					sym++;
                }

				pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
				locMap->insert(locSectPair);
				pair<chessPiece*, deskSection*> locPiecePair = make_pair(locPiece, locSect);
				locWhiteChess->insert(locPiecePair);
				break;

            }

			default:
				ShowMessage("Desk restore error!");
				return;
			}

		}
		else if(dN.chColor[i] == NOCOLOR && dN.chType[i] == NOPIECE) {
			chessPiece* locPiece = chDskPaint->getChDsk()->getNopiece();
			deskSection* locSect = chDskPaint->getChDsk()->sectFromArr(sym, num);
			num++;
			if(num > 8) {
				num = 1;
				sym++;
			}
			pair<deskSection*, chessPiece*> locSectPair = make_pair(locSect, locPiece);
			locMap->insert(locSectPair);
        }
	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------
}

moveNote::moveNote() {
	deskSection dSect;
	blackPiece = make_pair(NOPIECE, dSect);        //����, ��������� �� ���� ������ ������ chessType � ��������� �� �� ��������� ����
													//��������� chessPiece* � �������� ������� ����� ���� ������������ ������, �.�.
													 //��� ������ ������ �� ������ ���. ����� ��������������� ������ ��������� � �������������� ������
	whitePiece = make_pair(NOPIECE, dSect);       //����, ��������� �� ���� ����� ������ chessType � ��������� �� �� ��������� ����
	blackShortCas = false;                               //���� �������� ��������� ������
	blackLongCas = false;                                //���� ������� ��������� ������
	whiteShortCas = false;                               //���� �������� ��������� �����
	whiteLongCas = false;                                //���� ������� ��������� �����
	blackTake = false;                                   //���� ������ ������ ������
	whiteTake = false;                                   //���� ������ ������ �������
	whiteEp = false;                                     //������ �� ������� ����� ������
	blackEp = false;                                     //������ �� ������� ������ ������
	whiteTrans = false;                                  //���� ����������� ����� �����
	blackTrans = false;                                  //���� ����������� ������ �����
	blackCheck = false;                                 //���� ���������� ���� �������
	whiteCheck = false;                                 //���� ���������� ���� ������
	blackCheckmate = false;                             //���� ���������� ���� �������
	whiteCheckmate =false;                             //���� ���������� ���� ������
	whiteStalemate = false;                            //���� ���������� ���� ������
	blackStalemate = false;                            //���� ���������� ���� �������
	whiteDrawOffer = false;
	blackDrawOffer = false;
	drawSign = false;
	whiteWinSign = false;
	blackWinSign = false;
	blackTransPiece = NOPIECE;                      //��� ������ ������ ����� �����������

	whiteTransPiece = NOPIECE;                     //��� ����� ������ ����� �����������
    moveNumber = 0;
	moveNumber = ++moveCounter;                    //����������� ������� �������� �������� ��� ����, ����� ��������� ���������� ����� ����

}

unsigned moveNote::moveCounter = 0;                       //�������������� ����������� ���� - ������� �����
unsigned moveNote::uselessCounter = 0;

void moveNote::checkUselessMove() {
	//----------���� ��� ������ ��� �� ��������--------------------------------------------------------------
	if(whitePiece.first != PAWN && whiteTake == false && blackPiece.first == NOPIECE)      //���� ��� ����� �������� �� ������, ��� ������ �����
		return;                                                                //��������� ������� ����� ��� ��������� ��������� � ������� ���������
	else if(blackPiece.first == NOPIECE)                 //� ��������� ������ �������� (���������� �� ���� ������ ������� ������������, �.�. �� ������� ���� ��������������� ����� 50 ����� ��� ��������� ��������� �� ������ �������)
		uselessCounter = 0;
	//-------------���� ��� ������ ��� ��������--------------------------------------------------------------
	else if(blackPiece.first != PAWN && whitePiece.first != PAWN &&           //���� �� ������, �� ����� �� ������ �������,
		blackTake == false && whiteTake == false)                       //� � ����� ������ �� ���� ������,
		uselessCounter++;                                                 //�� �������������� ������� ��������� �����
	else                                                                //� ��������� ������
		uselessCounter = 0;                                             //�������� ������� ��������� �����
	return;
}


char moveNote::getPieceSym(chessType chType) const {
	char pieceSym;
	switch(chType) {
	case PAWN:
		pieceSym = 'p';
		break;
	case KNIGHT:
		pieceSym = 'N';
		break;
	case BISHOP:
		pieceSym = 'B';
		break;
	case ROOK:
		pieceSym = 'R';
		break;
	case QUEEN:
		pieceSym = 'Q';
		break;
	case KING:
		pieceSym = 'K';
		break;
	default:
		break;
	}
	return pieceSym;

}

string moveNote::getTextMove() {
	string moveStr;
	ostringstream moveStream;
	char blackPieceSym, whitePieceSym;
	blackPieceSym = getPieceSym(blackPiece.first);
	whitePieceSym = getPieceSym(whitePiece.first);

	 moveStream << moveNumber << ". ";
	if(whiteShortCas == true)
		moveStream << "0-0";

	else if(whiteLongCas == true)
		moveStream << "0-0-0";

	else if(whitePiece.first == PAWN) {
		moveStream << whitePieceSym << whitePiece.second.symbol <<
		whitePiece.second.number;
		if(whiteTake == false)
			moveStream << " - " << whiteMoveSect.symbol <<
			whiteMoveSect.number;
		else if(whiteTake == true) {
			moveStream << " x " << whiteMoveSect.symbol <<
			whiteMoveSect.number;
			if(whiteEp == true)
					moveStream << " e.p.";
		}
        if(whiteTrans == true)
			moveStream << ' ' << getPieceSym(whiteTransPiece);
	}
	else {
		if(whitePiece.second.number != 0 && whitePiece.second.symbol != 0) {
			moveStream << whitePieceSym << whitePiece.second.symbol <<
			whitePiece.second.number;
			if(whiteTake == false)
				moveStream << " - " << whiteMoveSect.symbol <<
				whiteMoveSect.number;
			else if(whiteTake == true)
				moveStream << " x " << whiteMoveSect.symbol <<
				whiteMoveSect.number;
		}
	}


	if(whiteCheck == true && whiteCheckmate == false)
		moveStream << '+';
	else if(whiteCheck == true && whiteCheckmate == true)
		moveStream << '#';
	if(whiteDrawOffer == true)
		moveStream << " (=)";
	if(whiteStalemate == true)
		moveStream << " 1/2-1/2";


	if(blackPiece.first != NOPIECE) {
			moveStream << ' ';
			if(blackShortCas == true)
				moveStream << "0-0";
			else if(blackLongCas == true)
				moveStream << "0-0-0";

			else if(blackPiece.first == PAWN) {
				moveStream << blackPieceSym << blackPiece.second.symbol <<
				blackPiece.second.number;
				if(blackTake == false)
					moveStream << " - " << blackMoveSect.symbol <<
					blackMoveSect.number;
				else if(blackTake == true) {
					moveStream << " x " << blackMoveSect.symbol <<
					blackMoveSect.number;
					if(blackEp == true)
							moveStream << " e.p.";
				}
                if(blackTrans == true)
					moveStream << ' ' << getPieceSym(blackTransPiece);
			}
			else {
				if(blackPiece.second.number != 0 && blackPiece.second.symbol != 0) {
					moveStream << blackPieceSym << blackPiece.second.symbol <<
					blackPiece.second.number;
					if(blackTake == false)
						moveStream << " - " << blackMoveSect.symbol <<
						blackMoveSect.number;
					else if(blackTake == true)
						moveStream << " x " << blackMoveSect.symbol <<
						blackMoveSect.number;
				}
			}
			if(blackCheck == true && blackCheckmate == false)
				moveStream << '+';
			else if(blackCheck == true && blackCheckmate == true)
				moveStream << '#';
			if(blackDrawOffer == true)
				moveStream << " (=)";
			if(blackStalemate == true)
				moveStream << " 1/2-1/2";

	}
	if(drawSign == true)
		moveStream << " 1/2-1/2";
	if(whiteWinSign == true)
		moveStream << " 1-0";
	else if(blackWinSign == true)
		moveStream << " 0-1";

	moveStr = moveStream.str();
	return moveStr;

}

repMoveNote::repMoveNote() {
	blackPiece.first = NOPIECE;
	whitePiece.first = NOPIECE;
    blackWinSign = false;
	whiteWinSign = false;
	whiteDrawSign = false;
	blackDrawSign = false;
	whiteDrawOffer = false;
	blackDrawOffer = false;
	whiteTake = false;
	blackTake = false;
	blackEp = false;
	whiteEp = false;
	whiteTransPiece = NOPIECE;
	blackTransPiece = NOPIECE;
	moveCounter++;
	moveNumber = moveCounter;

}
unsigned repMoveNote::moveCounter = 0;
unsigned repMoveNote::currMove = 0;
bool repMoveNote::repIsFinished = false;

void repMoveNote::setMoveText(string moveStr) {
	char sym;                   //��������� ��������� ���������� ��� ������������� ������ �� ���������� �����
	int num;                   //��������� ��������� ���������� ��� ������ ����� �� ���������� �����
	char word[15];              //��������� ���������� ������ ��� ������ ���� �� ���������� �����
	char pieceSymbols[] = {'p', 'N', 'B', 'R', 'Q', 'K'};
	char* symPtr;
	unsigned short keyIndex = 0;            //������ ��������� �����������
	vector<string> keyVector;       //������ �������� ����������� "�������������" �������� � ������
	vector<string>::iterator keyIterator;
	string notationKeys[] = {"0-0", "0-0-0", "1-0", "0-1", "(=)", "1/2-1/2"};


//--------------������� ��������� ����������� "�������������" ����� � ��������------------------------------------------------
	int keysCount = sizeof(notationKeys) / sizeof(string);         //���������� �������� ����������� � ������� �. �.
	size_t foundKey;                               //����� ������� � ������, � �������� ���������� ��������� � ���  �.�.
	for(int i = 0; i < keysCount; i++) {           //���������� ��� ��������� ������ � ��������� �������������
		if((foundKey = moveStr.find(notationKeys[i])) != string::npos)        //���� �������� ����������� ������� � ������ ����
			keyVector.push_back(notationKeys[i]);                       //��������� ��� � ������ �������� ����������� ������
	}
//----------------------------------------------------------------------------------------------------------------------------



	istringstream is(moveStr);       //������� ��������� �����, ��������������� �� �������� � ����� �������
	is >> sym;                      //������ ������� �� ������
	if(!isdigit(sym)) {             //���� ������ ������ �� �������� ������ (����� ����)
		ShowMessage("Replay file reading error! Incorrect replay file.1");          //������� ��������� � ������������ ����� ������
		return;
	}
	while(isdigit(sym))             //������ ������� �� ��� ���, ���� �� ��������� ���������� (����� ������ ����� ���� 2-� � ����� -�������)
		is >> sym;
	if(sym != '.') {               //���� ������ ���������� ������ �� �������� ������
		ShowMessage("Replay file reading error! Incorrect replay file.2");          //������� ��������� � ������������ ����� ������
		return;
	}

	is >> sym;                                             //������ ��������� ������
	//----------------------------------��������� ������� �.�. ������������� �������� �� ������ ���� �����--------------------------------------------------------
	symPtr = find(pieceSymbols, pieceSymbols + 6, sym);      //��������� �������� find() ���� ����������� ������ ����� �������� ���. �����
	if(symPtr == pieceSymbols + 6) {                           //���� ����� ����� ������ ���� �� ��������� �������� ������ (�� ����������. ������� ��������� ������)
		//-----------------������ �����, ������� �������� �������� �.�. ������������� ��������-----------------------------------------------------
		int n = 0;
		word[0] = sym;                 //�������� �������� ��������� ����� ����������� ��� ����������� ������
		while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //�� ��� ���, ���� �� ����� �������� ������
			word[++n] = sym;             //���������� � ����� ����������� �������
		word[++n] = '\0';                 //��������� ����� �������� ����� ������
		//----------------------------------------------------------------------------------------------------------------------------------------
		if(keyVector.empty() == true) {                  //���� ������ �.�. ������������� �������� ��� ������� ������ ����
			ShowMessage("Replay file reading error! Incorrect replay file.3");            //������� ��������� �� ������
			return;                                                              //���������� ���������� ������
		 }
		 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //���� ����������� ����� � ������� �������� �����������
		 if(keyIterator == keyVector.end()) {                                             //���� ����� ��� � ������� �.�. ��� ������� ������, �� ����� ������� ��������� �� ������
			ShowMessage("Replay file reading error! Incorrect replay file.4");            //������� ��������� �� ������
			return;                                                              //���������� ���������� ������
		 }
		 else if((*keyIterator) == "0-0") {                  //���������� ������ ��������� ��������� �����
			 whitePiece.first = KING;                        //����������� ��������� ��������-���� ������ ����������. ��������
			 whitePiece.second.symbol = 'e';
			 whitePiece.second.number = 1;
			 whiteMoveSect.symbol = 'g';
			 whiteMoveSect.number = 1;
		 }
		 else if((*keyIterator) == "0-0-0") {              //���������� ������� ������� ��������� �����
			 whitePiece.first = KING;
			 whitePiece.second.symbol = 'e';
			 whitePiece.second.number = 1;
			 whiteMoveSect.symbol = 'b';
			 whiteMoveSect.number = 1;
		 }

		 else if((*keyIterator) != "0-0" && (*keyIterator) != "0-0-0") {         //���� ��������� �.�. �� �������� �.�. ������� ��������� ��� �.�. �������� ���������
			 ShowMessage("Replay file reading error! Incorrect replay file.5");            //������� ��������� �� ������
				return;                                                              //���������� ���������� ������, �.�. ��������� �.�. ����� ���� ����������� ������ ����� ������ ���� ����� ��� ������ ���� ������
		 }
		 is >> sym;                 //������ ��������� ������
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	else {                             //� ��������� ������, ���� �������� ������, ������� ������ � ������ �������� - �������� ��������� �����
		switch(*symPtr) {               //� ����������� �� ����, ������ ����� ������ ��������
		case 'p':
			whitePiece.first = PAWN;       //�������������� �������� �������� ������� - ���� ������
			break;
		case 'N':
			whitePiece.first = KNIGHT;
			break;
		case 'B':
			whitePiece.first = BISHOP;
		case 'R':
			whitePiece.first = ROOK;
			break;
		case 'Q':
			whitePiece.first = QUEEN;
			break;
		case 'K':
			whitePiece.first = KING;
			break;
		default:
			return;
		}
		is >> sym;                     //����� ������ ������
		if(sym < 'a' || sym > 'h') {   //���� �� �� ������ � �������� �������� - ��������� �������� ��������� �����
			ShowMessage("Replay file reading error! Incorrect replay file.6");       //������� ��������� �� ������
			return;                                             //���������� ���������� ������
		}
		whitePiece.second.symbol = sym;          //��������� �������� ����, � �������� ����������� ��� �����. ��������� ����������� ������
		is >> num;                               //����� ������ ��������� ������ ���������� ����
		if(num < 0 || num > 8) {                 //���� �� �� ��������� � ��������� ���������� �������� ���. �����
			ShowMessage("Replay file reading error! Incorrect replay file.7");       //������� ��������� �� ������
			return;
		}
		whitePiece.second.number = num;          //��������� �������� ����, � �������� ����������� ��� �����, ��������� ����������� �����
		is >> sym;                               //������ ��������� ������
		if(sym != 'x' && sym != '-') {           //���� �� �� �������� �������� ���� '-' ��� ������� ������ ������ 'x'
			ShowMessage("Replay file reading error! Incorrect replay file.8");         //������� ��������� �� ������
			return;
		}
		if(sym == 'x')                          //���� �������� ������ ������
			whiteTake = true;                   //������������� ���� ������ ������
		is >> sym;                             //������ ��������� �����
		if(sym < 'a' || sym > 'h') {           //���� �� �� ����� �������� ��������� �������� ���. ����
			ShowMessage("Replay file reading error! Incorrect replay file.9");         //������� ��������� �� ������
			return;
		}
		whiteMoveSect.symbol = sym;           //��������� �������� ����, �� ������� �������������� ��� ������, ��������� ����������� ������
		is >> num;                            //������ �����
		if(num < 0 || num > 8) {              //���� ����� �� ����� ���� ��������� �������� ���. ����
			ShowMessage("Replay file reading error! Incorrect replay file.10");          //������� ��������� �� ������
			return;
		}
		whiteMoveSect.number = num;           //��������� �������� ����, �� ������� �������������� ��� ������, ��������� ����������� �����

		is >> sym;                     //������ ��������� ������

		if(sym == '+')                  //���� ��������� ���, � ������� ���������� ������ '+'
			is >> sym;
		else if(sym == '#')            //���� ��������� ���, ��������� ���������� ������
			return;
	}
	symPtr = find(pieceSymbols, pieceSymbols + 6, sym);         //���� ���������� ������ ����� �������� ����� �����
	if(symPtr == pieceSymbols + 6 && sym != 'e') {                            //���� ��������� ����� �� �������� �������� ���� ������ (������) � �� �������� �������� 'e'

		//-----------------������ �����, ������� �������� �������� �.�. ������������� ��������-----------------------------------------------------
		int n = 0;
		word[0] = sym;                 //�������� �������� ��������� ����� ����������� ��� ����������� ������
		while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //�� ��� ���, ���� �� ����� �������� ������ ��� ����� ������ ��� ����� �����
			word[++n] = sym;             //���������� � ����� ����������� �������
		word[++n] = '\0';                 //��������� ����� �������� ����� ������
		//----------------------------------------------------------------------------------------------------------------------------------------
		if(keyVector.empty() == true) {                  //���� ������ �.�. ������������� �������� ��� ������� ������ ����
			ShowMessage("Replay file reading error! Incorrect replay file.11");            //������� ��������� �� ������
			return;                                                              //���������� ���������� ������
		 }
		 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //���� ����������� ����� � ������� �������� �����������
		 if(keyIterator == keyVector.end()) {                                             //���� ����� ��� � ������� �.�. ��� ������� ������, �� ����� ������� ��������� �� ������
			ShowMessage("Replay file reading error! Incorrect replay file.12");            //������� ��������� �� ������
			return;                                                              //���������� ���������� ������
		 }
		 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //��������� � ������� �.�. ���� � ������� ���� �.�.
		 int notKeyNumber = notatKeyPtr - notationKeys;             //����������� ����� �������� ������� ���� �.�., �������� ������������� ������� ������ �.�., ���������� � �������
		 switch(notKeyNumber) {              //� ����������� �� ����, ����� �.�. ������� � �������
		  case 0:                          //� ������� �.�. ������� � ����� �������� ����� "0-0"
			blackPiece.first = KING;          //��������� �������-���� ������ ����������� ��������, ��������������� �������� ��������� ������
			blackPiece.second.symbol = 'e';
			blackPiece.second.number = 8;
			blackMoveSect.symbol = 'g';
			blackMoveSect.number = 8;
			break;
		  case 1:                           //� ������� �.�. ������� � ����� �������� ����� "0-0-0"
			blackPiece.first = KING;               //��������� �������-���� ������ ����������� ��������, ��������������� ������� ��������� ������
			blackPiece.second.symbol = 'e';
			blackPiece.second.number = 8;
			blackMoveSect.symbol = 'b';
			blackMoveSect.number = 8;
			break;
		 case 2:                          //� ������� �.�. ������� � ����� �������� ����� "1-0"
			whiteWinSign = true;          //������������� ���� ������ ����� (�� ����� ���� ������)
			break;
		 case 4:                        //� ������� �.�. ������� � ����� �������� ����� "(=)"
			blackDrawOffer = true;       //������������� ���� ����������� ����� �� ������� ������
			break;
		 case 5:                         //� ������� �.�. ������� � ����� �������� ����� "1/2-1/2"
			blackDrawSign = true;         //������������� ���� ����� �� ����� ���� ������
			break;
		 default:                                                           //���� ����� ��������� ���-�� ����
			ShowMessage("Replay file reading error! Incorrect replay file.13");            //������� ��������� �� ������
			return;                                                              //���������� ���������� ������
		 }

	}

	else {
		if(sym == 'e') {         //���� 'e' - ����������� ������
			//--------------------������ �.�. ������ ����� �� ������� "e.p.", ���� ������� �������------------------------------------
			is >> sym;
			if(sym == '.') {
				is >> sym;
				if(sym == 'p') {
					is >> sym;
					if(sym != '.') {               //���� �.�., �� ���� ������������������ �������� "e.p." ��������� �� ���������, ��
							ShowMessage("Replay file reading error! Incorrect replay file.14");       //������� ��������� �� ������
							return;
					}
					whiteEp = true;
				}
			}
			//--------------------------------------------------------------------------------------------------------------------
			is >> sym;
			//--------------------------����� �.�. ������ �� ������� ����� ����� ���� �.�. ������ ������������� ��������------------------------------
			symPtr = find(pieceSymbols, pieceSymbols + 6, sym);
			if(symPtr == pieceSymbols + 6) {
					//-----------------������ �����, ������� �������� �������� �.�. ������������� ��������-----------------------------------------------------
					int n = 0;
					word[0] = sym;                 //�������� �������� ��������� ����� ����������� ��� ����������� ������
					while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //�� ��� ���, ���� �� ����� �������� ������
						word[++n] = sym;             //���������� � ����� ����������� �������
					word[++n] = '\0';                 //��������� ����� �������� ����� ������
					//----------------------------------------------------------------------------------------------------------------------------------------
					if(keyVector.empty() == true) {                  //���� ������ �.�. ������������� �������� ��� ������� ������ ����
						ShowMessage("Replay file reading error! Incorrect replay file.15");            //������� ��������� �� ������
						return;                                                              //���������� ���������� ������
					 }
					 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //���� ����������� ����� � ������� �������� �����������
					 if(keyIterator == keyVector.end()) {                                             //���� ����� ��� � ������� �.�. ��� ������� ������, �� ����� ������� ��������� �� ������
						ShowMessage("Replay file reading error! Incorrect replay file.16");            //������� ��������� �� ������
						return;                                                              //���������� ���������� ������
					 }
					 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //��������� � ������� �.�. ���� � ������� ���� �.�.
					 int notKeyNumber = notatKeyPtr - notationKeys;             //����������� ����� �������� ������� ���� �.�., �������� ������������� ������� ������ �.�., ���������� � �������
					 switch(notKeyNumber) {              //� ����������� �� ����, ����� �.�. ������� � �������
					  case 0:                          //� ������� �.�. ������� � ����� �������� ����� "0-0"
						blackPiece.first = KING;          //��������� �������-���� ������ ����������� ��������, ��������������� �������� ��������� ������
						blackPiece.second.symbol = 'e';
						blackPiece.second.number = 8;
						blackMoveSect.symbol = 'g';
						blackMoveSect.number = 8;
						break;
					  case 1:                           //� ������� �.�. ������� � ����� �������� ����� "0-0-0"
						blackPiece.first = KING;               //��������� �������-���� ������ ����������� ��������, ��������������� ������� ��������� ������
						blackPiece.second.symbol = 'e';
						blackPiece.second.number = 8;
						blackMoveSect.symbol = 'b';
						blackMoveSect.number = 8;
						break;
					 case 2:                          //� ������� �.�. ������� � ����� �������� ����� "1-0"
						whiteWinSign = true;          //������������� ���� ������ ����� (�� ����� ���� ������)
						break;
					 case 4:                        //� ������� �.�. ������� � ����� �������� ����� "(=)"
						blackDrawOffer = true;       //������������� ���� ����������� ����� �� ������� ������
						break;
					 case 5:                         //� ������� �.�. ������� � ����� �������� ����� "1/2-1/2"
						blackDrawSign = true;         //������������� ���� ����� �� ����� ���� ������
						break;
					 default:                                                           //���� ����� ��������� ���-�� ����
						ShowMessage("Replay file reading error! Incorrect replay file.17");            //������� ��������� �� ������
						return;                                                              //���������� ���������� ������
					 }
			}
			//-----------------------------------------------------------------------------------------------------------------------------------
		}

		switch(*symPtr) {                   //���� ����� ������ ���� ����� ������� ���������� ������ ���.������, � ����������� �� �������� ������������ ������� (�������)
		case 'p':
			blackPiece.first = PAWN;       //������������� � �������� ���� ���. ������, ������� �������� ��� �������, �������� PAWN
			break;
		case 'N':
			blackPiece.first = KNIGHT;
			break;
		case 'B':
			blackPiece.first = BISHOP;
            break;
		case 'R':
			blackPiece.first = ROOK;
			break;
		case 'Q':
			blackPiece.first = QUEEN;
			break;
		case 'K':
			blackPiece.first = KING;
			break;
		default:
			return;
		}
		//----------------------���� ������ �������, ���������������� ���� ������, ���������� ����� �������������� �����-------------------------------------------------------
		is >> sym;
		if(sym == '+')         //����� ������� ������, ���������� �� �����, ����� ���� �.�. ���� - '+'
			is >> sym;         //� ����� ������ ������ ��������� ������
		symPtr = find(pieceSymbols, pieceSymbols + 6, sym);        //���� �� ������� ���� ���� ����������� ������, �� � ������ ����� ��� �������, ����������. ���� ����� ������
		if(symPtr != pieceSymbols + 6) {
			whiteTransPiece = blackPiece.first;                    //� ������ ��������� ������� ������� ������ ���������� ����� ������, � ������� ������������������ �����
			switch(*symPtr) {
			case 'p':
				blackPiece.first = PAWN;
				break;
			case 'N':
				blackPiece.first = KNIGHT;                       //������ � ���� ������� ������������� ���� ������, ������������ �� ������� ����.
				break;
			case 'B':
				blackPiece.first = BISHOP;
				break;
			case 'R':
				blackPiece.first = ROOK;
				break;
			case 'Q':
				blackPiece.first = QUEEN;
				break;
			case 'K':
				blackPiece.first = KING;
				break;
			default:
				return;
			}
			is >> sym;                             //������ ��������� ������
		}

		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if(sym == '+')
			is >> sym;
		else if(sym == '#') {
								 //���� ����� ��������� ������� ���. ������ �� ������� ���������� ������ ������, � ������� ������ ����
			whiteTransPiece  = blackPiece.first;      //������ ������ ��������� ��� ������, ���������� ����� �������������� �����
			blackPiece.first = NOPIECE;             //�������� ����� ����������� ��� ������ ������ ��� ������� ����
			return;
        }
		//---------------����� ������ ���� ������, � ������� ������������ �����, �������� ��������� ���� ���� ����� ����, ���� ����� ������������� ��������----------
		if(sym <'a' || sym > 'h') {              //���� ����������� ������ �� ����� ���� ��������� �������� ���������� ����

            //-----------------������ �����, ������� �������� �������� �.�. ������������� ��������-----------------------------------------------------
			int n = 0;
			word[0] = sym;                 //�������� �������� ��������� ����� ����������� ��� ����������� ������
			while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //�� ��� ���, ���� �� ����� �������� ������
				word[++n] = sym;             //���������� � ����� ����������� �������
			word[++n] = '\0';                 //��������� ����� �������� ����� ������
			//----------------------------------------------------------------------------------------------------------------------------------------
			if(keyVector.empty() == true) {                  //���� ������ �.�. ������������� �������� ��� ������� ������ ����
				ShowMessage("Replay file reading error! Incorrect replay file.15");            //������� ��������� �� ������
				return;                                                              //���������� ���������� ������
			 }
			 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //���� ����������� ����� � ������� �������� �����������
			 if(keyIterator == keyVector.end()) {                                             //���� ����� ��� � ������� �.�. ��� ������� ������, �� ����� ������� ��������� �� ������
				ShowMessage("Replay file reading error! Incorrect replay file.16");            //������� ��������� �� ������
				return;                                                              //���������� ���������� ������
			 }
			 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //��������� � ������� �.�. ���� � ������� ���� �.�.
			 int notKeyNumber = notatKeyPtr - notationKeys;             //����������� ����� �������� ������� ���� �.�., �������� ������������� ������� ������ �.�., ���������� � �������
			 switch(notKeyNumber) {              //� ����������� �� ����, ����� �.�. ������� � �������
											   //0 � 1 ������������� �.�. ������� � ������� ���������, �� � ���� ����� �� �������� ���� �� �����
			 case 2:                          //� ������� �.�. ������� � ����� �������� ����� "1-0"
				whiteWinSign = true;          //������������� ���� ������ ����� (�� ����� ���� ������)
				whiteTransPiece = blackPiece.first;
				blackPiece.first = NOPIECE;
				break;
			 case 4:                        //� ������� �.�. ������� � ����� �������� ����� "(=)"
				blackDrawOffer = true;       //������������� ���� ����������� ����� �� ������� ������
				whiteTransPiece = blackPiece.first;
				blackPiece.first = NOPIECE;
				break;
			 case 5:                         //� ������� �.�. ������� � ����� �������� ����� "1/2-1/2"
				blackDrawSign = true;         //������������� ���� ����� �� ����� ���� ������
				whiteTransPiece = blackPiece.first;
                blackPiece.first = NOPIECE;
				break;
			 default:                                                           //���� ����� ��������� ���-�� ����
				ShowMessage("Replay file reading error! Incorrect replay file.17");            //������� ��������� �� ������
				return;                                                              //���������� ���������� ������
			 }

			return;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------------------
		blackPiece.second.symbol = sym;           //� �������� ���������� ������� ����, � �������� ��� �������� ��� �������, ��������� ����������� ������
		is >> num;                           //������ �����
		if(num < 0 || num > 8) {            //���� ����������� ����� �� �������� ���������� �������� ���. ����
			ShowMessage("Replay file reading error! Incorrect replay file.19");          //������� ��������� �� ������
			return;
		}
		blackPiece.second.number = num;          //� �������� ���������� ������� ����, � �������� ��� �������� ��� �������, ��������� ����������� �����
		is >> sym;                              //������ ������
		if(sym != 'x' && sym != '-') {           //���� ����������� ������ �� �������� �������� ���� '-' ��� �������� ������ 'x'
			ShowMessage("Replay  file reading error! Incorrect replay file.20");       //������� ��������� �� ������
			return;
		}
		if(sym == 'x')                              //���� ����������� ������ - ������ ������ ������
			blackTake = true;                       //�� ������������� ���� ������ �������
		is >> sym;                                  //������ ������
		if(sym < 'a' || sym > 'h') {               //���� ������ �� ����� �������� ��������� �������� ���. ����
			ShowMessage("Replay file reading error! Incorrect replay file.21");       //������� ��������� ��������
			return;
		}
		blackMoveSect.symbol = sym;          //� �������� ���������� ������� ����, �� ������� ����������� ��� �������, ��������� ����������� ������

		is >> num;                  //������ �����
		if(num < 0 || num > 8) {        //���� ����������� ����� �� ����� �������� ��������� �������� ���. ����
			ShowMessage("Replay file reading error! Incorrect replay file.22");         //������� ��������� �� ������
			return;
		}
		blackMoveSect.number = num;          //� �������� ���������� ������� ����, �� ������� ����������� ��� ������� ��������� ����������� �����
		if(is >> sym) {                      //������ ��������� ������, ���� �� ��������� ����� ����� (��� �� ��������� �����-������ ������ ����)
			symPtr = find(pieceSymbols, pieceSymbols + 6, sym);
			//--------------------------������ �������, ���������������� ���� ������, ����� �������������� �����-------------------------------------------
			if(symPtr != pieceSymbols + 6) {                    //���� ������ ������������� ������� ������. ������
				switch(*symPtr) {
				case 'N':
					blackTransPiece = KNIGHT;            //����������� ����, ��������� ��� ������, � ������� ������������������ �����, �������� � ����������� �� ������������ �������
					break;
				case 'B':
					blackTransPiece = BISHOP;
					break;
				case 'R':
					blackTransPiece = ROOK;
					break;
				case 'Q':
					blackTransPiece = QUEEN;
					break;
				default:
					break;
				}
				is >> sym;              //������ ��������� ������
			}
			//---------------------------------------------------------------------------------------------------------------------------------------------
			else if(sym == 'e') {         //���� 'e' - ����������� ������
				//--------------------������ �.�. ������ ����� �� ������� "e.p.", ���� ������� �������------------------------------------
				is >> sym;
				if(sym == '.') {
					is >> sym;
					if(sym == 'p') {
						is >> sym;
						if(sym != '.') {               //���� �.�., �� ���� ������������������ �������� "e.p." ��������� �� ���������, ��
								ShowMessage("Replay file reading error! Incorrect replay file.23");       //������� ��������� �� ������
								return;
						}
						blackEp = true;
					}
					is >> sym;          //������ ��������� ������
				}
				//-----------------------------------------------------------------------------------------------------------------------
			}
			if(sym == '+' || sym == '#')                      //���� ������ ������� ���� � ����, �� ���������� � ������ ����� ������ (����� ���� �������� ������� ����� �����)
				is >> sym;
			if(is) {                    //���� �� ������� ������ ��� ��� ��� ������ (��������, �� ��������� ����� �����)
				symPtr = find(pieceSymbols, pieceSymbols + 6, sym);
				if(symPtr == pieceSymbols + 6) {
						//-----------------������ �����, ������� �������� �������� �.�. ������������� ��������-----------------------------------------------------
						int n = 0;
						word[0] = sym;                 //�������� �������� ��������� ����� ����������� ��� ����������� ������
						while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //�� ��� ���, ���� �� ����� �������� ������
							word[++n] = sym;             //���������� � ����� ����������� �������
						word[++n] = '\0';                 //��������� ����� �������� ����� ������
						//----------------------------------------------------------------------------------------------------------------------------------------
						if(keyVector.empty() == true) {                  //���� ������ �.�. ������������� �������� ��� ������� ������ ����
							ShowMessage("Replay file reading error! Incorrect replay file.24");            //������� ��������� �� ������
							return;                                                              //���������� ���������� ������
						 }
						 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //���� ����������� ����� � ������� �������� �����������
						 if(keyIterator == keyVector.end()) {                                             //���� ����� ��� � ������� �.�. ��� ������� ������, �� ����� ������� ��������� �� ������
							ShowMessage("Replay file reading error! Incorrect replay file.25");            //������� ��������� �� ������
							return;                                                              //���������� ���������� ������
						 }
						 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //��������� � ������� �.�. ���� � ������� ���� �.�.
						 int notKeyNumber = notatKeyPtr - notationKeys;             //����������� ����� �������� ������� ���� �.�., �������� ������������� ������� ������ �.�., ���������� � �������
						 switch(notKeyNumber) {              //� ����������� �� ����, ����� �.�. ������� � �������
						 case 0:                          //� ������� �.�. ������� � ����� �������� ����� "0-0"
								break;                     //�.�. ����� ������ ������ �.�. ��������� �������������  �� �����, ������� �� �����
						 case 1:                           //� ������� �.�. ������� � ����� �������� ����� "0-0-0"
								break;
						 case 3:                          //� ������� �.�. ������� � ����� �������� ����� "0-1"
							blackWinSign = true;          //������������� ���� ������ ������ (�� ����� ���� �����)
							break;
						 case 4:                        //� ������� �.�. ������� � ����� �������� ����� "(=)"
							whiteDrawOffer = true;       //������������� ���� ����������� ����� �� ������� �����
							break;
						 case 5:                         //� ������� �.�. ������� � ����� �������� ����� "1/2-1/2"
							whiteDrawSign = true;         //������������� ���� ����� �� ����� ���� �����
							break;
						 default:                                                           //���� ����� ��������� ���-�� ����
							ShowMessage("Replay file reading error! Incorrect replay file.26");            //������� ��������� �� ������
							return;                                                              //���������� ���������� ������
						 }
				}
				//-----------------------------------------------------------------------------------------------------------------------------------
			}

		}

	}

}

void TMainChessForm::makeRepMove() {

	repMoveNote::currMove = (deskNote::currDesk - 1) / 2;        //����� ����������� ���� � ������ � ����������� �� �������� ��������� �����
																 //���������� ����� ��� �� ������� ������ ���� �������� ���������,
																 //��������, ��� ���-�� ����� ������������� � 2 ���� ������ ���-�� ���������
	if((deskNote::currDesk - 1) % 2)                   //���� ������� �� ������� ���-�� ��������� (���. �����) ����� 1,
		currTurn = BLACK;                             //�� ������� ��� ������ - ��� ������
	else                                               //���� ����� 0,
		currTurn = WHITE;                              //�� ������� ��� ������ - ��� �����

	if(currTurn == WHITE) {						   //���� ������� ��� - ��� �����

		deskSection* sourceDS = &repMoveNotes[repMoveNote::currMove].whitePiece.second;         //�� �������, ��������� ���������� �� ����� �� ����� ������, ������ �������� ������� ����� ������
		deskSection* targetDS = &repMoveNotes[repMoveNote::currMove].whiteMoveSect;             //�� �������, ��������� ���������� �� ����� �� ����� ������, ������ ������� ����� ������
		prevChosenSect = sourceDS;                          //��� ���������� ���� �� ������ ���������� ������, ����������� ��� ���������� ������� �����
		prevSect = targetDS;                                //��� ����� ���������, �������� ���������� �������������� ������������ � ���������� (��� ������� ����),
															//����������� ��������, ����������� �� ���������� ����� (� ����� ����������� � ������� ���� repMoveNote


		if(targetDS->number != 0) {                         //���� ������� ���� ����������
			if(repMoveNotes[repMoveNote::currMove].whiteTake == true) {      //���� ���������� ���� ������ ������
				if(repMoveNotes[repMoveNote::currMove].whitePiece.first == PAWN && sourceDS->symbol != targetDS->symbol && repMoveNotes[repMoveNote::currMove].whiteEp == true)     //��� ������ ����� �� �������
					movePiece(targetDS);                           //� ������������ � ������� ������ ��������� ���������� ����� ���������� ���� (������������ � � ������� ����)
				else
					takePiece(targetDS);                         //��� ����� ������ ������ ���������� ����� ������ ���. ������ (����� ������������ � � ������� ����)
            }

			else                                       //���� ���� ������ ������ �� ����������
				movePiece(targetDS);                  //�������� ����� ���������� ����
		}
		repMoveNotes[repMoveNote::currMove].samePosesWhite = sameDesksCount;                    //��� �������� ������� - ���� ������ ��������� ���������� ���������� ������� � ����������. ����
		repMoveNotes[repMoveNote::currMove].uselessMoves = moveNote::uselessCounter;      //��� �������� ������� - ���� ������ ��������� ���-�� ����� ��� ������ � ������������ ����� � �����. ���� (����� ���� �����)

	}
	else if(currTurn == BLACK) {                     //���������� ��� ������, ���� ������� ��� - ������
		deskSection* sourceDS = &repMoveNotes[repMoveNote::currMove].blackPiece.second;
		deskSection* targetDS = &repMoveNotes[repMoveNote::currMove].blackMoveSect;
		prevChosenSect = sourceDS;
		prevSect = targetDS;
		if(targetDS->number != 0) {
			if(repMoveNotes[repMoveNote::currMove].blackTake == true) {
				if(repMoveNotes[repMoveNote::currMove].blackPiece.first == PAWN && sourceDS->symbol != targetDS->symbol && repMoveNotes[repMoveNote::currMove].blackEp == true)
					movePiece(targetDS);
				else
					takePiece(targetDS);
			}
			else
				movePiece(targetDS);
		}
		repMoveNotes[repMoveNote::currMove].samePosesBlack = sameDesksCount;                    //��� �������� ������� - ���� ������ ��������� ���������� ���������� ������� � ����������. ���� (����� ���� ������)
		repMoveNotes[repMoveNote::currMove].uselessMoves = moveNote::uselessCounter;      //��� �������� ������� - ���� ������ ��������� ���-�� ����� ��� ������ � ������������ ����� � �����. ���� (����� ���� ������)
	}

	//----------------���������� ���� ��������� ���� ��� ������������� ������� (�����, �������� ���������, ���������� ������� � �.�.)--------------------------
	if(resultIsShowed == false) {
		if(repMoveNotes[repMoveNote::currMove].whiteDrawSign == true || repMoveNotes[repMoveNote::currMove].blackDrawSign == true ||  repMoveNotes[repMoveNote::currMove].whiteDrawOffer == true ||
		repMoveNotes[repMoveNote::currMove].blackDrawOffer == true || repMoveNotes[repMoveNote::currMove].whiteWinSign == true || repMoveNotes[repMoveNote::currMove].blackWinSign == true) {
			if(deskNote::currDesk - 1 == repMoveNotes.size() * 2 && repMoveNotes[repMoveNotes.size() - 1].blackPiece.first != NOPIECE ||          //���� ������� ��� ��������� (��� ����� �������:
				deskNote::currDesk - 1 == repMoveNotes.size() * 2 - 1 && repMoveNotes[repMoveNotes.size() - 1].blackPiece.first == NOPIECE) {      //���� ������������� ����� ������, ���� ������������� ����� �����)
				menuSaveReplay->Enabled = true;
				menuSaveGame->Enabled = false;
				blackTimer->Enabled = false;
				chessGraphic->Enabled = false;
				endGameForm = new TForm(this);
				endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
				endGameForm->Position = poScreenCenter;
				endGameForm->Caption = "Game over!";
				endGameForm->Width = 375;
				endGameForm->Height = 135;
				TLabel* winnerLabel = new TLabel(endGameForm);

				winnerLabel->Parent = endGameForm;
				winnerLabel->Top = 25;
				winnerLabel->Left = 20;
				winnerLabel->Alignment = taCenter;
				winnerLabel->Font->Size = 12;
				winnerLabel->Font->Style = TFontStyles() << fsBold;
				if(repMoveNotes[repMoveNote::currMove].whiteDrawOffer == true)
					winnerLabel->Caption = "White offers draw. Black accepted";
				else if(repMoveNotes[repMoveNote::currMove].blackDrawOffer == true)
					winnerLabel->Caption = "Black offers draw. White accepted";
				else if(repMoveNotes[repMoveNote::currMove].whiteDrawSign == true)
					winnerLabel->Caption = "Draw";
				else if(repMoveNotes[repMoveNote::currMove].blackDrawSign == true)
					winnerLabel->Caption = "Draw";
				else if(repMoveNotes[repMoveNote::currMove].whiteWinSign == true)
					winnerLabel->Caption = "White wins!";
				else if(repMoveNotes[repMoveNote::currMove].blackWinSign == true)
					winnerLabel->Caption = "Black wins!";

				TButton* okButt = new TButton(endGameForm);
				okButt->Parent = endGameForm;
				okButt->OnClick = parentFormClose;
				okButt->Top = 75;
				okButt->Left = 45;
				okButt->Caption = "Ok";
				TButton* newGameButt = new TButton(endGameForm);
				newGameButt->Parent = endGameForm;
				newGameButt->OnClick = parentFormClose;
				newGameButt->Top = 75;
				newGameButt->Left = 145;
				newGameButt->Caption = "New game";
				TButton* saveReplayButt = new TButton(endGameForm);
				saveReplayButt->Parent = endGameForm;
				saveReplayButt->OnClick = parentFormClose;
				saveReplayButt->Top = 75;
				saveReplayButt->Left = 245;
				saveReplayButt->Caption = "Save replay";
				if(repMoveNotes.size() != 0)                    //���� ���� �������� �������
					saveReplayButt->Caption = "New replay";            //��������� ������ ���������� ������ �� ����� ����������� ����
				endGameForm->ShowModal();
				delete endGameForm;
			}
		}
	}
	//----------------------------------------------------------------------------------------------------------------------------
	if(currTurn == WHITE) {                             //���� ����� ���������� ���� ������ ����� ���� ������� ����� (���� ������ ����������� ����� �����, ����� ���� �������� � �����),
													   //����� ������ ����������� ����� �����, ���� �� ����� ���� ����� ��������� ������������� �������� (����� ������� ��� ���������� �����, � ������ �������)
		repMoveNote::currMove++;                      //�������������� ����� �������� ���� ������
		//---------------� ����-����� ����������� ����� ������ �������� ���������� ��� ��� ����������� (������� ����� ������� � ��������������� ������)---------------------------------------------
		if(repMoveNote::currMove == (deskNotes.size() - 1) / 2) {                             //���� ����������� ����� �� ����������� ��� (��������� ��� ����� �����������)
			AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //���������� ���� AnsiString ����������� ������ ���������������� �������� ����-�����
			TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //��������� � �������������� ��������� �� ������ ���� TListBoxInfo
																																	 //����������� ��� ��������� �� ������ �����. ��-�� ����-�����
			if(listItemInfo != NULL)                               //���� ������������� ������������ ������������� ������ �������, � ��������� ���������
				listItemInfo->moveIsMade = true;                   //����������� �������� true ����� ���������� ����(���� ������� ��-�� ����-�����)
			repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //��� ����, ����� ������� ����-����� ��� ����������� �����������, ������� ���
			repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //�� ����� ���������� ��������� ����� ������� ����-�����
		}
		else if(repMoveNote::currMove == (deskNotes.size() / 2) && repMoveNote::currMove == repMoveNote::moveCounter &&      //��� ������, ���� ���� ����������� ����� �����
		repMoveNotes[repMoveNotes.size() - 1].blackMoveSect.number == 0) {               //���� else if ��������� ��������� ���� else, ������ ��� �������� ������
			AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //���������� ���� AnsiString ����������� ������ ���������������� �������� ����-�����
			TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //��������� � �������������� ��������� �� ������ ���� TListBoxInfo
																																	 //����������� ��� ��������� �� ������ �����. ��-�� ����-�����
			if(listItemInfo != NULL)                               //���� ������������� ������������ ������������� ������ �������, � ��������� ���������
				listItemInfo->moveIsMade = true;                   //����������� �������� true ����� ���������� ����(���� ������� ��-�� ����-�����)
			repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //��� ����, ����� ������� ����-����� ��� ����������� �����������, ������� ���
			repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //�� ����� ���������� ��������� ����� ������� ����-�����
		}
		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	}
	else if(currTurn == BLACK && deskNotes[deskNotes.size() - 1].lastTurn == BLACK) { 		//��� ������, ���� ������ ����������� ����� ������ (����� ���� �� ������������ �����, �������� ��� �������� �������)
		repMoveNote::currMove++;                      //�������������� ����� �������� ���� ������
		//---------------� ����-����� ����������� ����� ������ �������� ���������� ��� ��� ����������� (������� ����� ������� � ��������������� ������)---------------------------------------------
		if(repMoveNote::currMove == (deskNotes.size() - 1) / 2) {                             //���� ����������� ����� �� ����������� ��� (��������� ��� ����� �����������)
			AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //���������� ���� AnsiString ����������� ������ ���������������� �������� ����-�����
			TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //��������� � �������������� ��������� �� ������ ���� TListBoxInfo
																																	 //����������� ��� ��������� �� ������ �����. ��-�� ����-�����
			if(listItemInfo != NULL)                               //���� ������������� ������������ ������������� ������ �������, � ��������� ���������
				listItemInfo->moveIsMade = true;                   //����������� �������� true ����� ���������� ����(���� ������� ��-�� ����-�����)
			repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //��� ����, ����� ������� ����-����� ��� ����������� �����������, ������� ���
			repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //�� ����� ���������� ��������� ����� ������� ����-�����
		}
		//------------------------------------------------------------------------------------------------------------------------------------------------------------
	}
	else if(currTurn == BLACK && deskNotes[deskNotes.size() - 1].lastTurn == WHITE) {   //��� ������, ���� ������ ����������� ������������� ��������� �� ���� ������ (�����, ����������� �����)
																					//�� ����������� ��������� ��� ����������� ������ ������
		if(deskNote::currDesk - 1 == repMoveNotes.size() * 2 - 1 && repMoveNotes[repMoveNotes.size() - 1].blackPiece.first == NOPIECE) {              //���� ���������������� ���������
            repMoveNote::currMove++;                      //�������������� ����� �������� ���� ������
			//---------------� ����-����� ����������� ����� ������ �������� ���������� ��� ��� ����������� (������� ����� ������� � ��������������� ������)---------------------------------------------
			if(repMoveNote::currMove == (deskNotes.size()) / 2) {                             //���� ����������� ����� �� ����������� ��� (��������� ��� ����� �����������)
				AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //���������� ���� AnsiString ����������� ������ ���������������� �������� ����-�����
				TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //��������� � �������������� ��������� �� ������ ���� TListBoxInfo
																																		 //����������� ��� ��������� �� ������ �����. ��-�� ����-�����
				if(listItemInfo != NULL)                               //���� ������������� ������������ ������������� ������ �������, � ��������� ���������
					listItemInfo->moveIsMade = true;                   //����������� �������� true ����� ���������� ����(���� ������� ��-�� ����-�����)
				repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //��� ����, ����� ������� ����-����� ��� ����������� �����������, ������� ���
				repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //�� ����� ���������� ��������� ����� ������� ����-�����
			}
			//------------------------------------------------------------------------------------------------------------------------------------------------------------


		}
	}

	chDskPaint->paintPieces();
	return;

}




void __fastcall TMainChessForm::menuSaveReplayClick(TObject *Sender)
{
	AnsiString saveFileName;

	if(saveReplayDialog->Execute())
		saveFileName = saveReplayDialog->FileName;
	else return;

	ofstream saveFileStream(saveFileName.c_str());
	if(!saveFileStream) {
		ShowMessage("Error! Can not open file!");
		return;
	}

	//------------------------��������� ������, ���������� � �������� ���� moveNote � ��������� ������-------------------------
	for(int i = 0; i < moveNotes.size(); i++) {

		saveFileStream << moveNotes[i].getTextMove();

		saveFileStream << endl;
	}
	//-------------------------------------------------------------------------------------------------------------------------
	moveNotes.clear();          //������� ������, �������� �������-������ �����


	return;
}


deskNote::deskNote() {
	for(int i = 0; i < SECTCOUNT; i++) {
		chType[i] = NOPIECE;
		chColor[i] = NOCOLOR;
	}
	blackShortCasAble = false;
	blackLongCasAble = false;
	whiteShortCasAble = false;
	whiteLongCasAble = false;
	blackEpAble = false;
	whiteEpAble = false;
	lastTurn = NOCOLOR;
	currDesk++;
}
unsigned deskNote::currDesk = 0;



bool operator==(deskNote dN1, deskNote dN2) {
	for(int i = 0; i < SECTCOUNT; i++)
		if(dN1.chType[i] != dN2.chType[i])
			return false;
	for(int i = 0; i < SECTCOUNT; i++)
		if(dN1.chColor[i] != dN2.chColor[i])
			return false;
	if(dN1.blackShortCasAble != dN2.blackShortCasAble)
		return false;
	if(dN1.blackLongCasAble != dN2.blackLongCasAble)
		return false;
	if(dN1.whiteShortCasAble != dN2.whiteShortCasAble)
		return false;
	if(dN1.whiteLongCasAble != dN2.whiteLongCasAble)
		return false;
	if(dN1.whiteEpAble != dN2.whiteEpAble)
		return false;
	if(dN1.blackEpAble != dN2.blackEpAble)
		return false;
	if(dN1.lastTurn != dN2.lastTurn)
		return false;
	return true;

}

void __fastcall TMainChessForm::whiteTimerTimer(TObject *Sender) {
	whiteTotalTime++;                            //�������������� ������� ������� ����� � ��������
	//----------------------------� ����������� �� ������ ������ ������� ������������ ���������� ����� � ������� mm : ss -------------------------
	if(tOpt->gameLimit == true) {
		int minLeft = (tOpt->gameLimitTime * 60 - whiteTotalTime) / 60;
		int secLeft = tOpt->gameLimitTime * 60 - whiteTotalTime -  minLeft * 60;
		whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
		if(secLeft > 9)
			whiteTimerMemo->Lines->Strings[0] +=  IntToStr(secLeft);
		else if(secLeft >= 0 && secLeft <= 9)
			whiteTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
		whiteTimerMemo->Lines->Strings[1] = "Time for a game.";

	}
	else if(tOpt->moveLimit == true) {
		int minLeft = (tOpt->moveLimitTime * 60 - whiteTotalTime) / 60;
		int secLeft = tOpt->moveLimitTime * 60 - whiteTotalTime -  minLeft * 60;
		whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
		if(secLeft > 9)
			whiteTimerMemo->Lines->Strings[0] += IntToStr(secLeft);
		else if(secLeft >= 0 && secLeft <= 9)
			whiteTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
		whiteTimerMemo->Lines->Strings[1] = "Time for 1 move.";
	}
	else if(tOpt->someMovesLimit == true) {
		int minLeft, secLeft;
		if(moveNote::moveCounter < tOpt->firstPerMovesCount) {
			minLeft = (tOpt->firstPerTime * 60 - whiteTotalTime) / 60;
			secLeft = tOpt->firstPerTime * 60 - whiteTotalTime - minLeft * 60;
			whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
			if(secLeft > 9)
				whiteTimerMemo->Lines->Strings[0] += IntToStr(secLeft);
			else if(secLeft >= 0 && secLeft <= 9)
				whiteTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
			blackTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount - moveNote::moveCounter) + " moves.";
		}
		else if(moveNote::moveCounter >= tOpt->firstPerMovesCount && moveNote::moveCounter < tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			minLeft = (tOpt->secondPerTime * 60 - whiteTotalTime) / 60;
			secLeft = tOpt->secondPerTime * 60 - whiteTotalTime - minLeft * 60;
			whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
			if(secLeft > 9)
				whiteTimerMemo->Lines->Strings[0] +=  IntToStr(secLeft);
			else if(secLeft >= 0 && secLeft <= 9)
				whiteTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
			blackTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount + tOpt->secondPerMovesCount - moveNote::moveCounter) + " moves.";
		}
		else if(moveNote::moveCounter >= tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			minLeft = (tOpt->finalPerTime * 60 - whiteTotalTime) / 60;
			secLeft = tOpt->finalPerTime * 60 - whiteTotalTime - minLeft * 60;
			whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
			if(secLeft > 9)
				whiteTimerMemo->Lines->Strings[0] += IntToStr(secLeft);
			else if(secLeft >= 0 && secLeft <= 9)
				whiteTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
			whiteTimerMemo->Lines->Strings[1] = "Time for a game.";
		}

	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------

	//-----------���� ������������ �������� ��������� ������� (� ����������� �� ������ ������ �������) �� ����������, �� ��������� ���������� ������-------------------
	if(tOpt->gameLimit == true) {
		if(whiteTotalTime != tOpt->gameLimitTime * 60)
			return;
	}
	else if(tOpt->moveLimit == true) {
		if(whiteTotalTime != tOpt->moveLimitTime * 60)
			return;
	}
	else if(tOpt->someMovesLimit == true) {
		if(moveNotes[moveNotes.size() - 1].moveCounter < tOpt->firstPerMovesCount) {
			if(whiteTotalTime != tOpt->firstPerTime * 60)
				return;
		}
		else if(moveNotes[moveNotes.size() - 1].moveCounter >= tOpt->firstPerMovesCount &&
		moveNotes[moveNotes.size() - 1].moveCounter < tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			if(whiteTotalTime != tOpt->secondPerTime * 60)
				return;
		}
		else if(moveNotes[moveNotes.size() - 1].moveCounter >= tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			if(whiteTotalTime != tOpt->finalPerTime * 60)
				return;
		}
	}

	else return;
	//-------------------------------------------------------------------------------------------------------------------------------------------

   //---------------------� ������ ��������� ������� �����, ���������, ���������� �� ��������� � ������---------------------------
	 bool blackMaterialEnough = false;
	 unsigned char blackKnightCount = 0;
	 bool blBishopOnBlack = false;
	 bool blBishopOnWhite = false;
	 map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPaint->getChDsk()->getMap();
	 map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;

	 for(tmpIt = tmpMap->begin(); tmpIt != tmpMap->end(); tmpIt++) {

		if((*tmpIt).second->getChessColor() == BLACK) {
			if((*tmpIt).second->getChessType() == PAWN || (*tmpIt).second->getChessType() == ROOK ||
			(*tmpIt).second->getChessType() == QUEEN)
				blackMaterialEnough = true;
			else if((*tmpIt).second->getChessType() == KNIGHT) {
				blackKnightCount++;
				if(blackKnightCount == 2)
				blackMaterialEnough = true;
			}
			else if((*tmpIt).second->getChessType() == BISHOP) {
				if((*tmpIt).first->sectColor == BLACK)
					blBishopOnBlack = true;
				else if((*tmpIt).first->sectColor == WHITE)
					blBishopOnWhite = true;
				if(blBishopOnBlack == true && blBishopOnWhite == true)
					blackMaterialEnough = true;
			}
		}

	 }

	//------------------------------------------------------------------------------------------------------------------------

	//------------------������� ���� ����������� ������-----------------------------------------------------------------------
	if(currTurn == WHITE) {

		if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //������������ ������ ���/���� �������, ����������� ����� � �����
		blackTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		blackConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
		blackDrawOffer->Enabled = false;

		menuSaveReplay->Enabled = true;
		menuSaveGame->Enabled = false;

		chDskPaint->paintPieces();
		chessGraphic->Enabled = false;
		endGameForm = new TForm(this);
		endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
		endGameForm->Position = poScreenCenter;
		endGameForm->Caption = "Game over!";
		endGameForm->Width = 375;
		endGameForm->Height = 135;
		TLabel* winnerLabel = new TLabel(endGameForm);
		winnerLabel->Parent = endGameForm;
		winnerLabel->Top = 25;
		winnerLabel->Left = 15;
		winnerLabel->Font->Size = 12;
		winnerLabel->Font->Style = TFontStyles() << fsBold;
		if(blackMaterialEnough == true) {
			winnerLabel->Caption = "White's time is over! Black won the game";
			if(!moveNotes.empty()) {
				moveNotes[moveNotes.size() - 1].blackWinSign = true;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
			}

			else {
				moveNote mNote;
				mNote.blackWinSign = true;
				moveNotes.push_back(mNote);
				movesLog->Lines->Add((mNote.getTextMove()).c_str());
			}
		}
		else if(blackMaterialEnough == false) {
			  winnerLabel->Font->Size = 14;
			  winnerLabel->Top = 15;
			  winnerLabel->Left = 85;
			  winnerLabel->Caption = "White's time is over!";
			  TLabel* winnerAddLabel = new TLabel(endGameForm);
			  winnerAddLabel->Parent = endGameForm;
			  winnerAddLabel->Top = 45;
			  winnerAddLabel->Left = 40;
			  winnerAddLabel->Font->Size = 10;
			  winnerAddLabel->Font->Style = TFontStyles() << fsBold;
			  winnerAddLabel->Caption = "But black do not have enough material. Draw!";

			  moveNotes[moveNotes.size()-1].drawSign = true;
		}
		TButton* okButt = new TButton(endGameForm);
		okButt->Parent = endGameForm;
		okButt->OnClick = parentFormClose;
		okButt->Top = 75;
		okButt->Left = 45;
		okButt->Caption = "Ok";
		TButton* newGameButt = new TButton(endGameForm);
		newGameButt->Parent = endGameForm;
		newGameButt->OnClick = parentFormClose;
		newGameButt->Top = 75;
		newGameButt->Left = 145;
		newGameButt->Caption = "New game";
		TButton* saveReplayButt = new TButton(endGameForm);
		saveReplayButt->Parent = endGameForm;
		saveReplayButt->OnClick = parentFormClose;
		saveReplayButt->Top = 75;
		saveReplayButt->Left = 245;
		saveReplayButt->Caption = "Save replay";
		endGameForm->ShowModal();
		delete endGameForm;
		return;
	}
	//---------------------------------------------------------------------------------------------------------------------
}



//---------------------------------------------------------------------------

void __fastcall TMainChessForm::blackTimerTimer(TObject *Sender)
{
	blackTotalTime++;                   //�������������� �������� �������� ������� ������

	//----------------------------� ����������� �� ������ ������ ������� ������������ ���������� ����� � ������� mm : ss -------------------------
	if(tOpt->gameLimit == true) {
		int minLeft = (tOpt->gameLimitTime * 60 - blackTotalTime) / 60;
		int secLeft = tOpt->gameLimitTime * 60 - blackTotalTime -  minLeft * 60;
		blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
		if(secLeft > 9)
			blackTimerMemo->Lines->Strings[0] +=  IntToStr(secLeft);
		else if(secLeft >= 0 && secLeft <= 9)
			blackTimerMemo->Lines->Strings[0] +=  "0" + IntToStr(secLeft);
		blackTimerMemo->Lines->Strings[1] = "Time for a game.";

	}
	else if(tOpt->moveLimit == true) {
		int minLeft = (tOpt->moveLimitTime * 60 - blackTotalTime) / 60;
		int secLeft = tOpt->moveLimitTime * 60 - blackTotalTime -  minLeft * 60;
		blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " ;
		if(secLeft > 9)
			blackTimerMemo->Lines->Strings[0] += IntToStr(secLeft);
		else if(secLeft >= 0 && secLeft <= 9)
			blackTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
		blackTimerMemo->Lines->Strings[1] = "Time for 1 move.";
	}
	else if(tOpt->someMovesLimit == true) {
		int minLeft, secLeft;
		if(moveNote::moveCounter < tOpt->firstPerMovesCount) {
			minLeft = (tOpt->firstPerTime * 60 - blackTotalTime) / 60;
			secLeft = tOpt->firstPerTime * 60 - blackTotalTime - minLeft * 60;
			blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
			if(secLeft > 9)
				blackTimerMemo->Lines->Strings[0] += IntToStr(secLeft);
			else if(secLeft >= 0 && secLeft <= 9)
				blackTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
			whiteTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount - moveNote::moveCounter) + " moves.";
		}
		else if(moveNote::moveCounter >= tOpt->firstPerMovesCount && moveNote::moveCounter < tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			minLeft = (tOpt->secondPerTime * 60 - blackTotalTime) / 60;
			secLeft = tOpt->secondPerTime * 60 - blackTotalTime - minLeft * 60;
			blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
			if(secLeft > 9)
				blackTimerMemo->Lines->Strings[0] += IntToStr(secLeft) ;
			else if(secLeft >= 0 && secLeft <= 9)
				blackTimerMemo->Lines->Strings[0] += "0" + IntToStr(secLeft);
			whiteTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount + tOpt->secondPerMovesCount - moveNote::moveCounter) + " moves.";
		}
		else if(moveNote::moveCounter >= tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			minLeft = (tOpt->finalPerTime * 60 - blackTotalTime) / 60;
			secLeft = tOpt->finalPerTime * 60 - blackTotalTime - minLeft * 60;
			blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : ";
			if(secLeft > 9)
				blackTimerMemo->Lines->Strings[0] += IntToStr(secLeft) ;
			else if(secLeft >= 0 && secLeft <= 9)
                blackTimerMemo->Lines->Strings[0] += IntToStr(secLeft);
			blackTimerMemo->Lines->Strings[1] = "Time for a game.";
		}

	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------

	//-----------���� ������������ �������� ��������� ������� (� ����������� �� ������ ������ �������) �� ����������, �� ��������� ���������� ������-------------------
	if(tOpt->gameLimit == true) {
		if(blackTotalTime != tOpt->gameLimitTime * 60)
			return;
	}
	else if(tOpt->moveLimit == true) {
		if(blackTotalTime != tOpt->moveLimitTime * 60)
			return;
	}
	else if(tOpt->someMovesLimit == true) {
		if(moveNotes[moveNotes.size() - 1].moveCounter < tOpt->firstPerMovesCount) {
			if(blackTotalTime != tOpt->firstPerTime * 60)
				return;

		}
		else if(moveNotes[moveNotes.size() - 1].moveCounter >= tOpt->firstPerMovesCount &&
		moveNotes[moveNotes.size() - 1].moveCounter < tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			if(blackTotalTime != tOpt->secondPerTime * 60)
				return;
		}
		else if(moveNotes[moveNotes.size() - 1].moveCounter >= tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
			if(blackTotalTime != tOpt->finalPerTime * 60)
				return;
        }
	}
	else return;
	//--------------------------------------------------------------------------------------------------------------------------------------------

	//----------------------� ������ ��������� ������� ������, ���������, ���������� �� ��������� � �����---------------------
    bool whiteMaterialEnough = false;
	unsigned char whiteKnightCount = 0;
	bool wtBishopOnBlack = false;
	bool wtBishopOnWhite = false;
    map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPaint->getChDsk()->getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;

	for(tmpIt = tmpMap->begin(); tmpIt != tmpMap->end(); tmpIt++) {

		if((*tmpIt).second->getChessColor() == WHITE) {
			if((*tmpIt).second->getChessType() == PAWN || (*tmpIt).second->getChessType() == ROOK ||
			(*tmpIt).second->getChessType() == QUEEN)
				whiteMaterialEnough = true;
			else if((*tmpIt).second->getChessType() == KNIGHT) {
				whiteKnightCount++;
				if(whiteKnightCount == 2)
				whiteMaterialEnough = true;
			}
			else if((*tmpIt).second->getChessType() == BISHOP) {
				if((*tmpIt).first->sectColor == BLACK)
					wtBishopOnBlack = true;
				else if((*tmpIt).first->sectColor == WHITE)
					wtBishopOnWhite = true;
				if(wtBishopOnBlack == true && wtBishopOnWhite == true)
					whiteMaterialEnough = true;
			}
		}

	}
	//---------------------------------------------------------------------------------------------------------------------------

	//---------------------------������� ���� ����������� ����--------------------------------------------------------------
	if(currTurn == BLACK) {

        	if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //������������ ������ ���/���� �������, ����������� ����� � �����
		blackTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		blackConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
		blackDrawOffer->Enabled = false;

		menuSaveReplay->Enabled = true;
		menuSaveGame->Enabled = false;
		blackTimer->Enabled = false;
		chDskPaint->paintPieces();
		chessGraphic->Enabled = false;
		endGameForm = new TForm(this);
        endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
		endGameForm->Position = poScreenCenter;
		endGameForm->Caption = "Game over!";
		endGameForm->Width = 375;
		endGameForm->Height = 135;
		TLabel* winnerLabel = new TLabel(endGameForm);
		winnerLabel->Parent = endGameForm;
		winnerLabel->Top = 25;
		winnerLabel->Left = 15;
		winnerLabel->Font->Size = 12;
		winnerLabel->Font->Style = TFontStyles() << fsBold;
		if(whiteMaterialEnough == true) {
			winnerLabel->Caption = "Black's time is over! White won the game";
			if(!moveNotes.empty()) {
				moveNotes[moveNotes.size() - 1].whiteWinSign = true;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
			}
			else {
				moveNote mNote;
				mNote.whiteWinSign = true;
				moveNotes.push_back(mNote);
				movesLog->Lines->Add((mNote.getTextMove()).c_str());
			}
		}
		else if(whiteMaterialEnough == false) {
			winnerLabel->Font->Size = 14;
			winnerLabel->Top = 15;
			winnerLabel->Left = 85;
			winnerLabel->Caption = "Black's time is over!";
			TLabel* winnerAddLabel = new TLabel(endGameForm);
			winnerAddLabel->Parent = endGameForm;
			winnerAddLabel->Top = 45;
			winnerAddLabel->Left = 40;
			winnerAddLabel->Font->Size = 10;
			winnerAddLabel->Font->Style = TFontStyles() << fsBold;
			winnerAddLabel->Caption = "But white do not have enough material. Draw!";
			moveNotes[moveNotes.size() - 1].drawSign = true;
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //����� ��������� � ��������� ���� ������ �����
		}
		TButton* okButt = new TButton(endGameForm);
		okButt->Parent = endGameForm;
		okButt->OnClick = parentFormClose;
		okButt->Top = 75;
		okButt->Left = 45;
		okButt->Caption = "Ok";
		TButton* newGameButt = new TButton(endGameForm);
		newGameButt->Parent = endGameForm;
		newGameButt->OnClick = parentFormClose;
		newGameButt->Top = 75;
		newGameButt->Left = 145;
		newGameButt->Caption = "New game";
		TButton* saveReplayButt = new TButton(endGameForm);
		saveReplayButt->Parent = endGameForm;
		saveReplayButt->OnClick = parentFormClose;
		saveReplayButt->Top = 75;
		saveReplayButt->Left = 245;
		saveReplayButt->Caption = "Save replay";
		endGameForm->ShowModal();
		delete endGameForm;
		return;
	}
	//-------------------------------------------------------------------------------------------------------------------------

}

void TMainChessForm::blackTimerMemoSet(int strNum, AnsiString str) {

	blackTimerMemo->Lines->Strings[strNum] = str;
}

void TMainChessForm::whiteTimerMemoSet(int strNum, AnsiString str) {

	whiteTimerMemo->Lines->Strings[strNum] = str;
}
//---------------------------------------------------------------------------
configOptions::configOptions() {
	string Option = "FORM_MINIMIZE";
	optionsVec.push_back(Option);
	Option = "REPLAY_MOVEMENT_DELAY";
	optionsVec.push_back(Option);
}

timerOptions::timerOptions() {
	timerOn = false;
	gameLimit = false;
	moveLimit = false;
	someMovesLimit = false;
	firstPerMovesCount = 0;
	secondPerMovesCount = 0;
	firstPerTime = 0;
	secondPerTime = 0;
	finalPerTime = 0;
	string Option = "TIMER_ON";
	optionsVec.push_back(Option);
	Option = "GAME_LIMIT";
	optionsVec.push_back(Option);
	Option = "MOVE_LIMIT";
	optionsVec.push_back(Option);
	Option = "GAME_LIMIT_TIME";
	optionsVec.push_back(Option);
	Option = "MOVE_LIMIT_TIME";
	optionsVec.push_back(Option);
	Option = "SOME_MOVES_LIMIT";
	optionsVec.push_back(Option);
	Option = "FIRST_PER_MOVES_COUNT";
	optionsVec.push_back(Option);
	Option = "SECOND_PER_MOVES_COUNT";
	optionsVec.push_back(Option);
	Option = "FIRST_PER_TIME";
	optionsVec.push_back(Option);
	Option = "SECOND_PER_TIME";
	optionsVec.push_back(Option);
	Option = "FINAL_PER_TIME";
	optionsVec.push_back(Option);
	Option = "ADD_TIME_PER_MOVE";
	optionsVec.push_back(Option);

}

void timerOptions::readTimerOptions() {

	ifstream readFileStream((ExtractFilePath(Application->ExeName) + "Options_files" + "/Timer_options.cfg").c_str());

	char sym;
	char word[30];
	unsigned num;
	vector<string>::iterator optIt;

	if(!readFileStream)
		throw timerReadError("READERR");

	while(readFileStream >> word) {

	  //while(!readFileStream.eof()) - ��������� �����������, �.�. �������� ����� ����� ����� ������������
	  //����� ������� ���������� �������� �����, � ������, �������� ������ � ������� ��������� "������"
	  //� �� �� ����� ���� �������� ������ �� ����� ����� ���������� 0 ��� ��������� ������, ��� ��������� �� ������������
	  //� �������� ��������� ��������� ����� while()

	  //	readFileStream >> word;
		optIt = find(optionsVec.begin(), optionsVec.end(), word);
		if(optIt == optionsVec.end())
			throw timerReadError("INCORR");
		else {
			if(*optIt == "TIMER_ON") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("INCORR");
				timerOn = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");

			}
			else if(*optIt == "GAME_LIMIT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("INCORR");
				gameLimit = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "MOVE_LIMIT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("INCORR");
				moveLimit = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "GAME_LIMIT_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> gameLimitTime;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "MOVE_LIMIT_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> moveLimitTime;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");

			}
			else if(*optIt == "SOME_MOVES_LIMIT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("INCORR");
				someMovesLimit = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "FIRST_PER_MOVES_COUNT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> firstPerMovesCount;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "SECOND_PER_MOVES_COUNT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> secondPerMovesCount;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "FIRST_PER_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> firstPerTime;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "SECOND_PER_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> secondPerTime;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "FINAL_PER_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> finalPerTime;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}
			else if(*optIt == "ADD_TIME_PER_MOVE") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("INCORR");
				readFileStream >> addTimePerMove;
				if(readFileStream.fail())
					throw timerReadError("INCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("INCORR");
			}

		}

	}

	if(gameLimit == true && (moveLimit == true || someMovesLimit == true))
		throw timerReadError("INVAL");
	else if(moveLimit == true && (gameLimit == true || someMovesLimit == true))
		throw timerReadError("INVAL");
	else if(someMovesLimit == true && (gameLimit == true || moveLimit == true))
		throw timerReadError("INVAL");
	else if(gameLimit == true && gameLimitTime == 0)
		throw timerReadError("INVAL");
	else if(moveLimit == true && moveLimitTime == 0)
		throw timerReadError("INVAL");
	else if(someMovesLimit == true && (firstPerMovesCount == 0 || firstPerTime == 0 ||
			secondPerMovesCount == 0 || secondPerTime == 0 || finalPerTime == 0))
			throw timerReadError("INVAL");



}

void timerOptions::loadDefaultOptions() {
	ifstream readFileStream((ExtractFilePath(Application->ExeName) + "Options_files" + "/Default_timer_opt.cfg").c_str());
	char sym;
	char word[30];
	unsigned num;
	vector<string>::iterator optIt;

	if(!readFileStream)
		throw timerReadError("DEFREADERR");

	while(readFileStream >> word) {

	  //while(!readFileStream.eof()) - ��������� �����������, �.�. �������� ����� ����� ����� ������������
	  //����� ������� ���������� �������� �����, � ������, �������� ������ � ������� ��������� "������"
	  //� �� �� ����� ���� �������� ������ �� ����� ����� ���������� 0 ��� ��������� ������, ��� ��������� �� ������������
	  //� �������� ��������� ��������� ����� while()

	  //	readFileStream >> word;
		optIt = find(optionsVec.begin(), optionsVec.end(), word);
		if(optIt == optionsVec.end())
			throw timerReadError("DEFINCORR");
		else {
			if(*optIt == "TIMER_ON") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("DEFINCORR");
				timerOn = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");

			}
			else if(*optIt == "GAME_LIMIT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("DEFINCORR");
				gameLimit = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "MOVE_LIMIT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("DEFINCORR");
				moveLimit = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "GAME_LIMIT_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> gameLimitTime;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "MOVE_LIMIT_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> moveLimitTime;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");

			}
			else if(*optIt == "SOME_MOVES_LIMIT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != '1' && sym != '0')
					throw timerReadError("DEFINCORR");
				someMovesLimit = sym - '0';
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "FIRST_PER_MOVES_COUNT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> firstPerMovesCount;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "SECOND_PER_MOVES_COUNT") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> secondPerMovesCount;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "FIRST_PER_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> firstPerTime;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "SECOND_PER_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> secondPerTime;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "FINAL_PER_TIME") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> finalPerTime;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}
			else if(*optIt == "ADD_TIME_PER_MOVE") {
				readFileStream >> sym;
				if(sym != '=')
					throw timerReadError("DEFINCORR");
				readFileStream >> addTimePerMove;
				if(readFileStream.fail())
					throw timerReadError("DEFINCORR");
				readFileStream >> sym;
				if(sym != ';')
					throw timerReadError("DEFINCORR");
			}

		}

	}
	if(timerOn == true) {
		if(gameLimit == true && (moveLimit == true || someMovesLimit == true))
			throw timerReadError("DEFINVAL");
		else if(moveLimit == true && (gameLimit == true || someMovesLimit == true))
			throw timerReadError("DEFINVAL");
		else if(someMovesLimit == true && (gameLimit == true || moveLimit == true))
			throw timerReadError("DEFINVAL");
		else if(gameLimit == true && gameLimitTime == 0)
			throw timerReadError("DEFINVAL");
		else if(moveLimit == true && moveLimitTime == 0)
			throw timerReadError("DEFINVAL");
		else if(someMovesLimit == true && (firstPerMovesCount == 0 || firstPerTime == 0 ||
				secondPerMovesCount == 0 || secondPerTime == 0 || finalPerTime == 0))
				throw timerReadError("DEFINVAL");
	}

}

void __fastcall timerOptions::saveTimerOptions(TObject* calledBy) {
		timerOptions tmpTimerOpt;       //���������� ��������� ������ �������� �������
		//---------------------��������� ������, ��������� ������������� � ���� �����, � ��������� �� �� ��������� ������� �������� �������
		TButton* saveButt = (TButton*) calledBy;
		TForm* tOptForm = (TForm*)(saveButt->Parent);
		for(int i=0; i < tOptForm->ControlCount; i++)
				if(tOptForm->Controls[i]->ClassNameIs("TGroupBox")) {
					if(static_cast<TGroupBox*>(tOptForm->Controls[i])->Name == "Game_Limit_GroupBox") {
						TGroupBox* groupBox = static_cast<TGroupBox*>(tOptForm->Controls[i]);
						for(int j = 0; j < groupBox->ControlCount; j++) {
							if(groupBox->Controls[j]->ClassNameIs("TCheckBox")) {
								if(static_cast<TCheckBox*>(groupBox->Controls[j])->Name == "Game_Limit_CheckBox") {
									tmpTimerOpt.gameLimit = (static_cast<TCheckBox*>(groupBox->Controls[j])->Checked) ? 1 : 0;
								}

							}
							else if(groupBox->Controls[j]->ClassNameIs("TEdit")) {
									if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "Game_Limit_Edit") {
										try {
											tmpTimerOpt.gameLimitTime = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);
										}
										catch(EConvertError&) {
											static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
											ShowMessage("Game limit time must be integer value > 0!");
											return;
										}

									}
                            }
						}
					}
					else if(static_cast<TGroupBox*>(tOptForm->Controls[i])->Name == "Move_Limit_GroupBox") {
						TGroupBox* groupBox = static_cast<TGroupBox*>(tOptForm->Controls[i]);
						for(int j = 0; j < groupBox->ControlCount; j++) {
							if(groupBox->Controls[j]->ClassNameIs("TCheckBox")) {
								if(static_cast<TCheckBox*>(groupBox->Controls[j])->Name == "Move_Limit_CheckBox") {
									tmpTimerOpt.moveLimit = (static_cast<TCheckBox*>(groupBox->Controls[j])->Checked) ? 1 : 0;
								}
							}
							else if(groupBox->Controls[j]->ClassNameIs("TEdit")) {
									if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "Move_Limit_Edit") {
										try {
											tmpTimerOpt.moveLimitTime = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);

										}
										catch(EConvertError&) {
											static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
											ShowMessage("Move limit time must be integer value > 0!");
											return;
                                        }

									}
							}
                        }
					}
					else if(static_cast<TGroupBox*>(tOptForm->Controls[i])->Name == "Some_Moves_Limit_GroupBox") {
						TGroupBox* groupBox = static_cast<TGroupBox*>(tOptForm->Controls[i]);
						for(int j = 0; j < groupBox->ControlCount; j++) {
								if(groupBox->Controls[j]->ClassNameIs("TCheckBox")) {
										if(static_cast<TCheckBox*>(groupBox->Controls[j])->Name == "Some_Moves_Limit_CheckBox") {
											tmpTimerOpt.someMovesLimit = (static_cast<TCheckBox*>(groupBox->Controls[j])->Checked) ? 1 : 0;
										}
								}
								else if(groupBox->Controls[j]->ClassNameIs("TEdit")) {
										if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "First_Per_Moves_Edit") {
											try {
												tmpTimerOpt.firstPerMovesCount = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);
											}
											catch(EConvertError&) {
												static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
												ShowMessage("First period moves count must be integer value > 0");
												return;
											}


										}
										else if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "First_Per_Time_Edit") {
											try {
												tmpTimerOpt.firstPerTime = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);
											}
											catch(EConvertError&) {
													static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
													ShowMessage("First period time must be integer value > 0");
													return;
                                            }

										}
										else if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "Second_Per_Moves_Edit") {
											try {
												tmpTimerOpt.secondPerMovesCount = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);
											}
											catch(EConvertError&) {
												static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
												ShowMessage("Second period moves count must be integer value > 0");
												return;
                                            }

										}
										else if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "Second_Per_Time_Edit") {
											try {
												tmpTimerOpt.secondPerTime = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);
											}
											catch(EConvertError&) {
													static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
													ShowMessage("Second period time must be integer value > 0");
													return;
                                            }


										}
										else if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "Final_Time_Edit") {
											try {
												tmpTimerOpt.finalPerTime = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);

											}
											catch(EConvertError&) {
												static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
												ShowMessage("Final period time must be integer value > 0");
												return;
                                            }


										}
										else if(static_cast<TEdit*>(groupBox->Controls[j])->Name == "Add_Time_Edit") {
											try {
												tmpTimerOpt.addTimePerMove = StrToInt(static_cast<TEdit*>(groupBox->Controls[j])->Text);
											}
											catch(EConvertError&) {
													static_cast<TEdit*>(groupBox->Controls[j])->Text = "0";
													ShowMessage("Additional time per move must be integer value > 0");
													return;
                                            }
										}
								}
						}
					}
				}
        tmpTimerOpt.timerOn = tmpTimerOpt.moveLimit || tmpTimerOpt.gameLimit || tmpTimerOpt.someMovesLimit;         //���� �� ���� �� ������� ������� �� ������, ������� ���� ����������� ������� �� ��������� ������� ��������

		//----------------------------------------------------------------------------------------------------------------------

		//--------------------------��������� ������������ ������, ��������� ������������� ����� �����---------------------------
		if(tmpTimerOpt.gameLimit == true && tmpTimerOpt.gameLimitTime == 0) {
				ShowMessage("You enter incorrect game limit time! Try again.");
				return;
		}
		if(tmpTimerOpt.moveLimit == true && tmpTimerOpt.moveLimitTime == 0) {
				ShowMessage("You enter incorrect move limit time! Try again.");
				return;
		}
		if(tmpTimerOpt.someMovesLimit == true && tmpTimerOpt.firstPerMovesCount == 0) {
			ShowMessage("You entered incorrect first period moves count! Try again.");
			return;
		}
		if(tmpTimerOpt.someMovesLimit == true && tmpTimerOpt.firstPerTime == 0) { 
			ShowMessage("You entered incorrect first period time! Try again.");
			return;
		}
		if(tmpTimerOpt.someMovesLimit == true && tmpTimerOpt.secondPerMovesCount == 0) {
			ShowMessage("You entered incorrect second period moves count! Try again.");
			return;
			
		}
		if(tmpTimerOpt.someMovesLimit == true && tmpTimerOpt.secondPerTime == 0) {
			ShowMessage("You entered incorrect second period time! Try again.");
			return;
		}
		if(tmpTimerOpt.someMovesLimit == true && tmpTimerOpt.finalPerTime == 0) {
			ShowMessage("You entered incorrct final period time! Try again.");
			return;
		}
		//------------------------------------------------------------------------------------------------------------------------

		//--------------------��������� ������ �� ���������� ������� �������� ������� � ��������� ���� ��������------------------
		ofstream outFileStream;
		outFileStream.open((ExtractFilePath(Application->ExeName) + "Options_files" + "/Timer_options.cfg").c_str(), ios::out | ios::trunc);
		if(!outFileStream) {
			ShowMessage("Can't open timer options file!");
			outFileStream.close();
			return;
		}
		outFileStream << "TIMER_ON = ";
		outFileStream << tmpTimerOpt.timerOn << ';' << endl;
		outFileStream << "GAME_LIMIT = ";
		outFileStream << tmpTimerOpt.gameLimit << ';' << endl;
		outFileStream << "MOVE_LIMIT = ";
		outFileStream << tmpTimerOpt.moveLimit << ';' <<  endl;
		outFileStream << "GAME_LIMIT_TIME = ";
		outFileStream << tmpTimerOpt.gameLimitTime << ';' << endl;
		outFileStream << "MOVE_LIMIT_TIME = ";
		outFileStream << tmpTimerOpt.moveLimitTime << ';' << endl;
		outFileStream << "SOME_MOVES_LIMIT = ";
		outFileStream << tmpTimerOpt.someMovesLimit << ';' << endl;
		outFileStream << "FIRST_PER_MOVES_COUNT = ";
		outFileStream << tmpTimerOpt.firstPerMovesCount << ';' << endl;
		outFileStream << "SECOND_PER_MOVES_COUNT = ";
		outFileStream << tmpTimerOpt.secondPerMovesCount << ';' << endl;
		outFileStream << "FIRST_PER_TIME = ";
		outFileStream << tmpTimerOpt.firstPerTime << ';' << endl;
		outFileStream << "SECOND_PER_TIME = ";
		outFileStream << tmpTimerOpt.secondPerTime << ';' << endl;
		outFileStream << "FINAL_PER_TIME = ";
		outFileStream << tmpTimerOpt.finalPerTime << ';' << endl;
		outFileStream << "ADD_TIME_PER_MOVE = ";
		outFileStream << tmpTimerOpt.addTimePerMove << ';' << endl;
		tOptForm->Close();
		//--------------------------------------------------------------------------------------------------------------------------------
		//-----------------��������� ���������� � ������� � ��������� ����� ������� �����-------------------------------------------------------------------------

		TMainChessForm* tMainForm = (TMainChessForm*) (tOptForm->Owner);

		if(tmpTimerOpt.timerOn == false) {                            //���� �� ��������� ������� �������� ������� ������� ����������� ������� �� ����������,
			if(tMainForm->menuTimerSwitch->Checked == true)           //� ���� ������ �� ����� ��� �������
				tMainForm->menuTimerSwitchClick(tMainForm->menuTimerSwitch);      //�������� ������� �� ����� �� ����� ����,  ���������� �� ���������\���������� �������,
																				 //����� �������, ��������� ������
			tMainForm->blackTimerMemoSet(0, "Timer is off.");
			tMainForm->blackTimerMemoSet(1, "");
			tMainForm->whiteTimerMemoSet(0, "Timer is off.");
			tMainForm->whiteTimerMemoSet(1, "");

		}
		else if(tmpTimerOpt.gameLimit == true) {                 	//���� �� ��������� ������� �������� ������� ���������� ������� ������ �� ����,
			if(tMainForm->menuTimerSwitch->Checked == true) {     	//� ���� ������ �� ����� ��� �������
				tMainForm->blackTimerMemoSet(0, AnsiString(tmpTimerOpt.gameLimitTime) + " : 00");      //� ��������������� ��������� ����� ��������� ���������� � �������
				tMainForm->blackTimerMemoSet(1, "Time for a game.");
				tMainForm->whiteTimerMemoSet(0, AnsiString(tmpTimerOpt.gameLimitTime) + " : 00");
				tMainForm->whiteTimerMemoSet(1, "Time for a game.");
			}
		}
		else if(tmpTimerOpt.moveLimit == true) {
			if(tMainForm->menuTimerSwitch->Checked == true) {
				tMainForm->blackTimerMemoSet(0, AnsiString(tmpTimerOpt.moveLimitTime) + " : 00");
				tMainForm->blackTimerMemoSet(1, "Time for 1 move.");
				tMainForm->whiteTimerMemoSet(0, AnsiString(tmpTimerOpt.moveLimitTime) + " : 00");
				tMainForm->whiteTimerMemoSet(1, "Time for 1 move.");
			}
		}
		else if(tmpTimerOpt.someMovesLimit == true) {
			if(tMainForm->menuTimerSwitch->Checked == true) {
				tMainForm->blackTimerMemoSet(0, AnsiString(tmpTimerOpt.firstPerTime) + " : 00");
				tMainForm->blackTimerMemoSet(1, "Time for " + AnsiString(tmpTimerOpt.firstPerMovesCount) + " moves.");
				tMainForm->whiteTimerMemoSet(0, AnsiString(tmpTimerOpt.firstPerTime) + " : 00");
				tMainForm->whiteTimerMemoSet(1, "Time for " + AnsiString(tmpTimerOpt.firstPerMovesCount) + " moves.");
			}
		}

		//------------------------------------------------------------------------------------------------------------------------


		*this = tmpTimerOpt;                     //��������� ������� �������� ������� ����������� ��������� ������

}
void __fastcall TMainChessForm::menuTimerStopClick(TObject *Sender)                   //������� ��� ����� ������ "Stop" ���� "Timer"
{
	if(menuTimerStop->Caption == "Stop") {                             //���� ������� ��������(�������) ������ ���� - "Stop", �.�. ������ � ������ ������ ��������
		blackTimer->Enabled = false;                                   //����� ������� ������������� ������ ������
		whiteTimer->Enabled = false;                                   //������������� ������ �����
		menuTimerStop->Caption = "Continue";                           //�������� ������ ������ �� "Continue"
		if(currTurn == WHITE)
			whiteTimerStop->Caption = "Continue";
		else if(currTurn == BLACK)
			blackTimerStop->Caption = "Continue";
		chessGraphic->Enabled = false;                                 //��������� ����������� ������ - ��������� ����
		menuSaveGame->Enabled = true;
	}
	else if(menuTimerStop->Caption == "Start") {                      //���� ������� ��������(�������) ������ ���� - "Start", �.�. ������ � ������ ������ �� ��������,
																	  //� �� ���������� ����� � ������� ������
		if(currTurn == WHITE) {      			//���� ������� ��� - ��� �����
			if(moveNote::moveCounter == 0 && tOpt->someMovesLimit == true)        //���� ������� ��� �������� ������
				whiteTotalTime -= tOpt->addTimePerMove;                    //�� ����� ����� ������ ���� (� �� ����� ������� ���� �����!) � ������ ������� ���������� �������� �������������  ��������
																		   //�������� ������� �������� ������� ������, �������� � ���� (�.�. �������� �� ������ ���������� ������� �����)
			whiteTimer->Enabled = true;                                  //�������� ������ �����
			menuTimerSwitch->Enabled = false;
        }

		else if(currTurn == BLACK)
			blackTimer->Enabled = true;
		menuTimerStop->Caption = "Stop";                             //�������� ������ ������ �� "Stop"
		menuTimerOptions->Enabled = false;                           //��������� ��������� �������� ������� �� ����� ��� ������
		menuSaveGame->Enabled = false;                                //��������� ���������� ���� �� ����� ������ �������
		chessGraphic->Enabled = true;                                //�������� ����. ������ - ��������� ����

	}
	else if(menuTimerStop->Caption == "Continue") {                 //���� ������� ��������(�������) ������ ���� - "Continue", �.�. ������ � ������ ������ �� ��������
	   if(currTurn == WHITE) {                                         //� ����������� �� ����, ����� ������� ��� �� ������� �� ������ ����, ���� �����
			whiteTimer->Enabled = true;                             //������������ ������ ������� �����
			whiteTimerStop->Caption = "Stop";
	   }
	   else if(currTurn == BLACK) {                                    //���� ������
			blackTimer->Enabled = true;                             //������������ ������ ������� ������
			blackTimerStop->Caption = "Stop";
	   }
	   menuTimerStop->Caption = "Stop";                             //�������� ������ ������ �� "Stop"
	   chessGraphic->Enabled = true;                                //�������� ����. ������ - ��������� ����
	   menuSaveGame->Enabled = false;
	}

}
//---------------------------------------------------------------------------

void __fastcall TMainChessForm::menuTimerOptionsClick(TObject *Sender)
{
	tOptionsForm = new TForm(this);                      //�������� ������ � �������������� �����
    tOptionsForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
	tOptionsForm->Position = poScreenCenter;
	tOptionsForm->BorderStyle = bsSingle;
	tOptionsForm->BorderIcons >> biMaximize;

	tOptionsForm->Caption = "Timer Options";
	tOptionsForm->Name = "Timer_Options_Form";
	tOptionsForm->Width = 400;
	tOptionsForm->Height = 405;
	TButton* okButt = new TButton(tOptionsForm);          //������� ������ "��"
	okButt->Parent = tOptionsForm;
	okButt->OnClick = tOpt->saveTimerOptions;
	okButt->Caption = "Save";
	okButt->Name = "Save_Timer_Options_Button";
	okButt->Top = 345;
	okButt->Left = 205;
	TButton* cancelButt = new TButton(tOptionsForm);       //������� ������ "Cancel"
	cancelButt->Parent = tOptionsForm;
	cancelButt->OnClick = parentFormClose;
	cancelButt->Caption = "Cancel";
	cancelButt->Name = "Cancel_Timer_Options_Button";
	cancelButt->Top = 345;
	cancelButt->Left = 300;


	TGroupBox* gameLimitGroup = new TGroupBox(tOptionsForm);          //������� ����������� �������� ������� �� ����
	gameLimitGroup->Parent = tOptionsForm;
	gameLimitGroup->Caption = "Game limit timer";
	gameLimitGroup->Name = "Game_Limit_GroupBox";
	gameLimitGroup->Left = 5;
	gameLimitGroup->Top = 7;
	gameLimitGroup->Width = 180;
	TGroupBox* moveLimitGroup = new TGroupBox(tOptionsForm);        //������� ����������� �������� ������� �� ���
	moveLimitGroup->Parent = tOptionsForm;
	moveLimitGroup->Caption = "Move limit timer";
	moveLimitGroup->Name = "Move_Limit_GroupBox";
	moveLimitGroup->Left = 205;
	moveLimitGroup->Top = 7;
	TGroupBox* sMovesLimitGroup = new TGroupBox(tOptionsForm);      //������� ����������� �������� ������� �� ��������
	sMovesLimitGroup->Parent = tOptionsForm;
	sMovesLimitGroup->Caption = "Some moves limit timer";
	sMovesLimitGroup->Name = "Some_Moves_Limit_GroupBox";
	sMovesLimitGroup->Left = 7;
	sMovesLimitGroup->Top = 130;
	sMovesLimitGroup->Width = 382;
	sMovesLimitGroup->Height = 205;


 //-----------------��������� �������� � �����������------------------------------------------------------------------
	gameLimitCheck = new TCheckBox(gameLimitGroup);
	gameLimitCheck->Parent = gameLimitGroup;
	gameLimitCheck->Caption = "Switch on game limit timer";
	gameLimitCheck->Name = "Game_Limit_CheckBox";
	gameLimitCheck->Top = 20;
	gameLimitCheck->Left = 15;
	gameLimitCheck->Width = 160;
	gameLimitCheck->OnClick = gameLimChboxClick;


	moveLimitCheck = new TCheckBox(moveLimitGroup);
	moveLimitCheck->Parent = moveLimitGroup;
	moveLimitCheck->Caption = "Switch on move limit timer";
	moveLimitCheck->Name = "Move_Limit_CheckBox";
	moveLimitCheck->Top = 20;
	moveLimitCheck->Left = 15;
	moveLimitCheck->Width = 160;
	moveLimitCheck->OnClick = moveLimChboxClick;


	sMovesLimitCheck = new TCheckBox(sMovesLimitGroup);
	sMovesLimitCheck->Parent = sMovesLimitGroup;
	sMovesLimitCheck->Caption = "Switch on some moves limit timer";
	sMovesLimitCheck->Name = "Some_Moves_Limit_CheckBox";
	sMovesLimitCheck->Top = 20;
	sMovesLimitCheck->Left = 15;
	sMovesLimitCheck->Width = 200;
	sMovesLimitCheck->OnClick = sMovesLimChboxClick;

//--------------------------------------------------------------------------------------------------------------------------

//--------------------��������� ��������� ���� � �����������-----------------------------------------------------------------
	gameLimitEdit = new TEdit(gameLimitGroup);
	gameLimitEdit->Parent = gameLimitGroup;
	gameLimitEdit->Text = "0";
	gameLimitEdit->Name = "Game_Limit_Edit";
	gameLimitEdit->Top = 60;
	gameLimitEdit->Left = 100;
	gameLimitEdit->Width = 50;
	gameLimitLabel = new TLabel(gameLimitGroup);
	gameLimitLabel->Parent = gameLimitGroup;
	gameLimitLabel->Caption = "Time (min.):";
	gameLimitLabel->Name = "Game_Limit_Label";
	gameLimitLabel->Top = 60;
	gameLimitLabel->Left = 20;
	gameLimitLabel->Width = 100;


	moveLimitEdit = new TEdit(moveLimitGroup);
	moveLimitEdit->Parent = moveLimitGroup;
	moveLimitEdit->Text = "0";
	moveLimitEdit->Name = "Move_Limit_Edit";
	moveLimitEdit->Top = 60;
	moveLimitEdit->Left = 100;
	moveLimitEdit->Width = 50;
	moveLimitLabel = new TLabel(moveLimitGroup);
	moveLimitLabel->Parent = moveLimitGroup;
	moveLimitLabel->Caption = "Time (min.):";
	moveLimitLabel->Name = "Move_Limit_Label";
	moveLimitLabel->Top = 60;
	moveLimitLabel->Left = 20;
	moveLimitLabel->Width = 100;


	per1MovesEdit = new TEdit(sMovesLimitGroup);
	per1MovesEdit->Parent = sMovesLimitGroup;
	per1MovesEdit->Text = "0";
	per1MovesEdit->Name = "First_Per_Moves_Edit";
	per1MovesEdit->Top = 50;
	per1MovesEdit->Left = 110;
	per1MovesEdit->Width = 50;
	per1MovesLabel = new TLabel(sMovesLimitGroup);
	per1MovesLabel->Parent = sMovesLimitGroup;
	per1MovesLabel->Caption = "Per. 1 moves:";
	per1MovesLabel->Name = "First_Per_Moves_Label";
	per1MovesLabel->Top = 50;
	per1MovesLabel->Left = 20;
	per1MovesLabel->Width = 100;


	per1TimeEdit = new TEdit(sMovesLimitGroup);
	per1TimeEdit->Parent = sMovesLimitGroup;
	per1TimeEdit->Text = "0";
	per1TimeEdit->Name = "First_Per_Time_Edit";
	per1TimeEdit->Top = 50;
	per1TimeEdit->Left = 310;
	per1TimeEdit->Width = 50;
	per1TimeLabel = new TLabel(sMovesLimitGroup);
	per1TimeLabel->Parent = sMovesLimitGroup;
	per1TimeLabel->Caption = "Per. 1 time (min.):";
	per1TimeLabel->Name = "First_Per_Time_Label";
	per1TimeLabel->Top = 50;
	per1TimeLabel->Left = 200;
	per1TimeLabel->Width = 100;


	per2MovesEdit = new TEdit(sMovesLimitGroup);
	per2MovesEdit->Parent = sMovesLimitGroup;
	per2MovesEdit->Text = "0";
	per2MovesEdit->Name = "Second_Per_Moves_Edit";
	per2MovesEdit->Top = 100;
	per2MovesEdit->Left = 110;
	per2MovesEdit->Width = 50;
	per2MovesLabel = new TLabel(sMovesLimitGroup);
	per2MovesLabel->Parent = sMovesLimitGroup;
	per2MovesLabel->Caption = "Per. 2 moves:";
	per2MovesLabel->Name = "Second_Per_Moves_Label";
	per2MovesLabel->Top = 100;
	per2MovesLabel->Left = 20;
	per2MovesLabel->Width = 100;


	per2TimeEdit = new TEdit(sMovesLimitGroup);
	per2TimeEdit->Parent = sMovesLimitGroup;
	per2TimeEdit->Text = "0";
	per2TimeEdit->Name = "Second_Per_Time_Edit";
	per2TimeEdit->Top = 100;
	per2TimeEdit->Left = 310;
	per2TimeEdit->Width = 50;
	per2TimeLabel = new TLabel(sMovesLimitGroup);
	per2TimeLabel->Parent = sMovesLimitGroup;
	per2TimeLabel->Caption = "Per. 2 time (min.):";
	per2TimeLabel->Name = "Second_Per_Time_Label";
	per2TimeLabel->Top = 100;
	per2TimeLabel->Left = 200;
	per2TimeLabel->Width = 100;


	finalTimeEdit = new TEdit(sMovesLimitGroup);
	finalTimeEdit->Parent = sMovesLimitGroup;
	finalTimeEdit->Text = "0";
	finalTimeEdit->Name = "Final_Time_Edit";
	finalTimeEdit->Top = 140;
	finalTimeEdit->Left = 225;
	finalTimeEdit->Width = 50;
	finalTimeLabel = new TLabel(sMovesLimitGroup);
	finalTimeLabel->Parent = sMovesLimitGroup;
	finalTimeLabel->Caption = "Final period time (min.):";
	finalTimeLabel->Name = "Final_Per_Time_Label";
	finalTimeLabel->Top = 140;
	finalTimeLabel->Left = 100;
	finalTimeLabel->Width = 125;

	addTimeEdit = new TEdit(sMovesLimitGroup);
	addTimeEdit->Parent = sMovesLimitGroup;
	addTimeEdit->Text = "0";
	addTimeEdit->Name = "Add_Time_Edit";
	addTimeEdit->Top = 170;
	addTimeEdit->Left = 225;
	addTimeEdit->Width = 50;
	addTimeLabel = new TLabel(sMovesLimitGroup);
	addTimeLabel->Parent = sMovesLimitGroup;
	addTimeLabel->Caption = "Additional time per. move (sec.):";
	addTimeLabel->Name = "Add_Time_Move";
	addTimeLabel->Top = 170;
	addTimeLabel->Left = 55;
	addTimeLabel->Width = 155;


//---------� ����������� �� ��������, ����������� �� �����, ���������� ����������� ���������� �����������--------------------
	if(tOpt->gameLimit == true) {
		gameLimitCheck->Checked = true;
		gameLimitEdit->Enabled = true;
		gameLimitLabel->Enabled = true;
		moveLimitCheck->Checked = false;
		moveLimitEdit->Enabled = false;
		moveLimitLabel->Enabled = false;
		sMovesLimitCheck->Checked = false;
		per1MovesEdit->Enabled = false;
		per1TimeEdit->Enabled = false;
		per2MovesEdit->Enabled = false;
		per2TimeEdit->Enabled = false;
		finalTimeEdit->Enabled = false;
		addTimeEdit->Enabled = false;
		per1MovesLabel->Enabled = false;
		per1TimeLabel->Enabled = false;
		per2MovesLabel->Enabled = false;
		per2TimeLabel->Enabled = false;
		finalTimeLabel->Enabled = false;
		addTimeLabel->Enabled = false;

	}
	else if(tOpt->moveLimit == true) {
		moveLimitCheck->Checked = true;
		moveLimitEdit->Enabled = true;
		moveLimitLabel->Enabled = true;
		gameLimitCheck->Checked = false;
		gameLimitEdit->Enabled = false;
		gameLimitLabel->Enabled = false;
		sMovesLimitCheck->Checked = false;
		per1MovesEdit->Enabled = false;
		per1TimeEdit->Enabled = false;
		per2MovesEdit->Enabled = false;
		per2TimeEdit->Enabled = false;
		finalTimeEdit->Enabled = false;
		addTimeEdit->Enabled = false;
		per1MovesLabel->Enabled = false;
		per1TimeLabel->Enabled = false;
		per2MovesLabel->Enabled = false;
		per2TimeLabel->Enabled = false;
		finalTimeLabel->Enabled = false;
		addTimeLabel->Enabled = false;

	}
	else if(tOpt->someMovesLimit == true) {
		sMovesLimitCheck->Checked = true;
		per1MovesEdit->Enabled = true;
		per1TimeEdit->Enabled = true;
		per2MovesEdit->Enabled = true;
		per2TimeEdit->Enabled = true;
		finalTimeEdit->Enabled = true;
		addTimeEdit->Enabled = true;
		per1MovesLabel->Enabled = true;
		per1TimeLabel->Enabled = true;
		per2MovesLabel->Enabled = true;
		per2TimeLabel->Enabled = true;
		finalTimeLabel->Enabled = true;
		addTimeLabel->Enabled  = true;

		gameLimitCheck->Checked = false;
		gameLimitEdit->Enabled = false;
		gameLimitLabel->Enabled = false;
		moveLimitCheck->Checked = false;
		moveLimitEdit->Enabled = false;
		moveLimitLabel->Enabled = false;

	}
	else if(tOpt->timerOn == false) {
		gameLimitCheck->Checked = false;
		gameLimitEdit->Enabled = false;
		gameLimitLabel->Enabled = false;
		moveLimitCheck->Checked = false;
		moveLimitEdit->Enabled = false;
		moveLimitLabel->Enabled = false;
		sMovesLimitCheck->Checked = false;
		per1MovesEdit->Enabled = false;
		per1TimeEdit->Enabled = false;
		per2MovesEdit->Enabled = false;
		per2TimeEdit->Enabled = false;
		finalTimeEdit->Enabled = false;
		addTimeEdit->Enabled = false;
		per1MovesLabel->Enabled = false;
		per1TimeLabel->Enabled = false;
		per2MovesLabel->Enabled = false;
		per2TimeLabel->Enabled = false;
		finalTimeLabel->Enabled = false;
		addTimeLabel->Enabled = false;
	}
    gameLimitEdit->Text = tOpt->gameLimitTime;
	moveLimitEdit->Text = tOpt->moveLimitTime;
	per1MovesEdit->Text = tOpt->firstPerMovesCount;
	per1TimeEdit->Text = tOpt->firstPerTime;
	per2MovesEdit->Text = tOpt->secondPerMovesCount;
	per2TimeEdit->Text = tOpt->secondPerTime;
	finalTimeEdit->Text = tOpt->finalPerTime;
	addTimeEdit->Text = tOpt->addTimePerMove;
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	tOptionsForm->ShowModal();
	delete tOptionsForm;


}

void __fastcall TMainChessForm::gameLimChboxClick(TObject* Sender) {
	if(gameLimitCheck->Checked == true) {
		gameLimitEdit->Enabled = true;
		gameLimitLabel->Enabled = true;
	}
	else {
		gameLimitEdit->Enabled = false;
		gameLimitLabel->Enabled =false;
		return;
	}

	moveLimitCheck->Checked = false;
	moveLimitEdit->Enabled = false;
	moveLimitLabel->Enabled = false;
	sMovesLimitCheck->Checked = false;
	per1MovesEdit->Enabled = false;
	per1TimeEdit->Enabled = false;
	per2MovesEdit->Enabled = false;
	per2TimeEdit->Enabled = false;
	finalTimeEdit->Enabled = false;
	addTimeEdit->Enabled = false;
	per1MovesLabel->Enabled = false;
	per1TimeLabel->Enabled = false;
	per2MovesLabel->Enabled = false;
	per2TimeLabel->Enabled = false;
	finalTimeLabel->Enabled = false;
	addTimeLabel->Enabled = false;
	return;

}

void __fastcall TMainChessForm::moveLimChboxClick(TObject* Sender) {
	if(moveLimitCheck->Checked == true) {
		moveLimitEdit->Enabled = true;
		moveLimitLabel->Enabled = true;
	}
	else {
		moveLimitEdit->Enabled = false;
		moveLimitLabel->Enabled = false;
		return;
	}

	gameLimitCheck->Checked = false;
	gameLimitEdit->Enabled = false;
	gameLimitLabel->Enabled = false;
	sMovesLimitCheck->Checked = false;
	per1MovesEdit->Enabled = false;
	per1TimeEdit->Enabled = false;
	per2MovesEdit->Enabled = false;
	per2TimeEdit->Enabled = false;
	finalTimeEdit->Enabled = false;
	addTimeEdit->Enabled = false;
	per1MovesLabel->Enabled = false;
	per1TimeLabel->Enabled = false;
	per2MovesLabel->Enabled = false;
	per2TimeLabel->Enabled = false;
	finalTimeLabel->Enabled = false;
	addTimeLabel->Enabled = false;
	return;

}

void __fastcall TMainChessForm::sMovesLimChboxClick(TObject* Sender) {
	if(sMovesLimitCheck->Checked == true) {
		per1MovesEdit->Enabled = true;
		per1TimeEdit->Enabled = true;
		per2MovesEdit->Enabled = true;
		per2TimeEdit->Enabled = true;
		finalTimeEdit->Enabled = true;
		addTimeEdit->Enabled = true;
		per1MovesLabel->Enabled = true;
		per1TimeLabel->Enabled = true;
		per2MovesLabel->Enabled = true;
		per2TimeLabel->Enabled = true;
		finalTimeLabel->Enabled = true;
		addTimeLabel->Enabled = true;
	}
	else {

		per1MovesEdit->Enabled = false;
		per1TimeEdit->Enabled = false;
		per2MovesEdit->Enabled = false;
		per2TimeEdit->Enabled = false;
		finalTimeEdit->Enabled = false;
		addTimeEdit->Enabled = false;
		per1MovesLabel->Enabled = false;
		per1TimeLabel->Enabled = false;
		per2MovesLabel->Enabled = false;
		per2TimeLabel->Enabled = false;
		finalTimeLabel->Enabled = false;
		addTimeLabel->Enabled = false;
		return;
	}


	gameLimitCheck->Checked = false;
	gameLimitEdit->Enabled = false;
	gameLimitLabel->Enabled = false;
	moveLimitCheck->Checked = false;
	moveLimitEdit->Enabled = false;
	moveLimitLabel->Enabled = false;
	return;
}

void __fastcall TMainChessForm::bTimerStopClick(TObject* Sender) {
	menuTimerStopClick(menuTimerStop);
	TButton* locButt = (TButton*) Sender;
	locButt->Caption = menuTimerStop->Caption;
	whiteTimerStop->Caption = "Stop";
	return;
}

void __fastcall TMainChessForm::wTimerStopClick(TObject* Sender) {
	menuTimerStopClick(menuTimerStop);
	TButton* locButt = (TButton*) Sender;
	locButt->Caption = menuTimerStop->Caption;
	blackTimerStop->Caption = "Stop";
	return;
}

void __fastcall TMainChessForm::whiteConcedeClick(TObject* Sender) {             //�����, ���������� �� ������� �� ������� �� ������ ����� �����
	chessGraphic->Enabled = false;
	moveNotes[moveNotes.size() - 1].blackWinSign = true;               //� ��������� �������� ���������� ���� ��������� ������ ������
	movesLog->Lines->Delete(movesLog->Lines->Count - 1);             //� ������ � ������� �� ������� ����� ����� ������� ��������� ������ ������
	movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� ������� �����

	if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
		whiteTimer->Enabled = false;
		blackTimer->Enabled = false;
	}
	//----------------------��������� ����(�����) ����������� ����----------------------------------------------------------------------
	endGameForm = new TForm(this);
	endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
	endGameForm->Position = poScreenCenter;
	endGameForm->Caption = "Game over!";
	endGameForm->Width = 375;
	endGameForm->Height = 135;
	TLabel* winnerLabel = new TLabel(endGameForm);
	winnerLabel->Parent = endGameForm;
	winnerLabel->Top = 25;
	winnerLabel->Left = 15;
	winnerLabel->Font->Size = 13;
	winnerLabel->Font->Style = TFontStyles() << fsBold;
	winnerLabel->Caption = "White conceded! Black won the game";
	TButton* okButt = new TButton(endGameForm);
	okButt->Parent = endGameForm;
	okButt->OnClick = parentFormClose;
	okButt->Top = 75;
	okButt->Left = 45;
	okButt->Caption = "Ok";
	TButton* newGameButt = new TButton(endGameForm);
	newGameButt->Parent = endGameForm;
	newGameButt->OnClick = parentFormClose;
	newGameButt->Top = 75;
	newGameButt->Left = 145;
	newGameButt->Caption = "New game";
	TButton* saveReplayButt = new TButton(endGameForm);
	saveReplayButt->Parent = endGameForm;
	saveReplayButt->OnClick = parentFormClose;
	saveReplayButt->Top = 75;
	saveReplayButt->Left = 245;
	saveReplayButt->Caption = "Save replay";
	endGameForm->ShowModal();
	delete endGameForm;
	//--------------------------------------------------------------------------------------------------------------------------------------------
	menuSaveReplay->Enabled = true;
	menuSaveGame->Enabled = false;
}

void __fastcall TMainChessForm::blackConcedeClick(TObject* Sender) {
	chessGraphic->Enabled = false;
	moveNotes[moveNotes.size() - 1].whiteWinSign = true;               //� ��������� �������� ���������� ���� ��������� ������ ������
	movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� ������� �����

	if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
		whiteTimer->Enabled = false;
		blackTimer->Enabled = false;
	}
	//----------------------��������� ����(�����) ����������� ����----------------------------------------------------------------------
	endGameForm = new TForm(this);
    endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
	endGameForm->Position = poScreenCenter;
	endGameForm->Caption = "Game over!";
	endGameForm->Width = 375;
	endGameForm->Height = 135;
	TLabel* winnerLabel = new TLabel(endGameForm);
	winnerLabel->Parent = endGameForm;
	winnerLabel->Top = 25;
	winnerLabel->Left = 15;
	winnerLabel->Font->Size = 13;
	winnerLabel->Font->Style = TFontStyles() << fsBold;
	winnerLabel->Caption = "Black conceded! White won the game";
	TButton* okButt = new TButton(endGameForm);
	okButt->Parent = endGameForm;
	okButt->OnClick = parentFormClose;
	okButt->Top = 75;
	okButt->Left = 45;
	okButt->Caption = "Ok";
	TButton* newGameButt = new TButton(endGameForm);
	newGameButt->Parent = endGameForm;
	newGameButt->OnClick = parentFormClose;
	newGameButt->Top = 75;
	newGameButt->Left = 145;
	newGameButt->Caption = "New game";
	TButton* saveReplayButt = new TButton(endGameForm);
	saveReplayButt->Parent = endGameForm;
	saveReplayButt->OnClick = parentFormClose;
	saveReplayButt->Top = 75;
	saveReplayButt->Left = 245;
	saveReplayButt->Caption = "Save replay";
	endGameForm->ShowModal();
	delete endGameForm;
	//--------------------------------------------------------------------------------------------------------------------------------------------
	menuSaveReplay->Enabled = true;
	menuSaveGame->Enabled = false;
}

void __fastcall TMainChessForm::whiteDrawOfferClick(TObject* Sender) {
	if(MessageBox(0, L"Do you accept it?",  L"Draw offer from white", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		chessGraphic->Enabled = false;
		moveNotes[moveNotes.size() - 1].drawSign = true;               //� ��������� �������� ���������� ���� ��������� ������ ������
		movesLog->Lines->Delete(movesLog->Lines->Count - 1);             //� ������ � ������� �� ������� ����� ����� ������� ��������� ������ ������
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� ������� �����

		if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
			whiteTimer->Enabled = false;
			blackTimer->Enabled = false;
		}
		//----------------------��������� ����(�����) ����������� ����----------------------------------------------------------------------
		endGameForm = new TForm(this);
		endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
		endGameForm->Position = poScreenCenter;
		endGameForm->Caption = "Game over!";
		endGameForm->Width = 375;
		endGameForm->Height = 135;
		TLabel* winnerLabel = new TLabel(endGameForm);
		winnerLabel->Parent = endGameForm;
		winnerLabel->Top = 25;
		winnerLabel->Left = 10;
		winnerLabel->Font->Size = 12;
		winnerLabel->Font->Style = TFontStyles() << fsBold;
		winnerLabel->Caption = "White offered draw and black accepted it!";
		TButton* okButt = new TButton(endGameForm);
		okButt->Parent = endGameForm;
		okButt->OnClick = parentFormClose;
		okButt->Top = 75;
		okButt->Left = 45;
		okButt->Caption = "Ok";
		TButton* newGameButt = new TButton(endGameForm);
		newGameButt->Parent = endGameForm;
		newGameButt->OnClick = parentFormClose;
		newGameButt->Top = 75;
		newGameButt->Left = 145;
		newGameButt->Caption = "New game";
		TButton* saveReplayButt = new TButton(endGameForm);
		saveReplayButt->Parent = endGameForm;
		saveReplayButt->OnClick = parentFormClose;
		saveReplayButt->Top = 75;
		saveReplayButt->Left = 245;
		saveReplayButt->Caption = "Save replay";
		endGameForm->ShowModal();
		delete endGameForm;
		//--------------------------------------------------------------------------------------------------------------------------------------------
		menuSaveReplay->Enabled = true;
		menuSaveGame->Enabled = false;
	}

}

void __fastcall TMainChessForm::blackDrawOfferClick(TObject* Sender) {
	if(MessageBox(0, L"Do you accept it?", L"Draw offer from black", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		chessGraphic->Enabled = false;
		moveNotes[moveNotes.size() - 1].drawSign = true;               //� ��������� �������� ���������� ���� ��������� ������ ������
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //������� ��������� ��������� ������� �����

		if(tOpt->timerOn == true) {                     //���� ������ �������, ������������� ������� ������ � �����
			whiteTimer->Enabled = false;
			blackTimer->Enabled = false;
		}
		//----------------------��������� ����(�����) ����������� ����----------------------------------------------------------------------
		endGameForm = new TForm(this);
		endGameForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
		endGameForm->Position = poScreenCenter;
		endGameForm->Caption = "Game over!";
		endGameForm->Width = 375;
		endGameForm->Height = 135;
		TLabel* winnerLabel = new TLabel(endGameForm);
		winnerLabel->Parent = endGameForm;
		winnerLabel->Top = 25;
		winnerLabel->Left = 10;
		winnerLabel->Font->Size = 12;
		winnerLabel->Font->Style = TFontStyles() << fsBold;
		winnerLabel->Caption = "Black offered draw and white accepted it!";
		TButton* okButt = new TButton(endGameForm);
		okButt->Parent = endGameForm;
		okButt->OnClick = parentFormClose;
		okButt->Top = 75;
		okButt->Left = 45;
		okButt->Caption = "Ok";
		TButton* newGameButt = new TButton(endGameForm);
		newGameButt->Parent = endGameForm;
		newGameButt->OnClick = parentFormClose;
		newGameButt->Top = 75;
		newGameButt->Left = 145;
		newGameButt->Caption = "New game";
		TButton* saveReplayButt = new TButton(endGameForm);
		saveReplayButt->Parent = endGameForm;
		saveReplayButt->OnClick = parentFormClose;
		saveReplayButt->Top = 75;
		saveReplayButt->Left = 245;
		saveReplayButt->Caption = "Save replay";
		endGameForm->ShowModal();
		delete endGameForm;
		//--------------------------------------------------------------------------------------------------------------------------------------------
		menuSaveReplay->Enabled = true;
		menuSaveGame->Enabled = false;
	}
}


void __fastcall TMainChessForm::menuTimerSwitchClick(TObject *Sender)
{

	if(menuTimerSwitch->Checked == true) {
		chessGraphic->Enabled = true;
		menuTimerStop->Enabled = false;
		menuTimerStop->Caption = "Start";
		menuTimerOptions->Enabled = true;
		menuTimerSwitch->Checked = false;
		blackTimer->Enabled = false;
		whiteTimer->Enabled = false;

		whiteTimerStop->Enabled = false;
		blackTimerStop->Enabled = false;
		whiteTimerStop->Caption = "Start";
		blackTimerStop->Caption = "Start";

		tOpt->timerOn = false;
		blackTimerMemo->Lines->Strings[0] = "Timer is off.";
		blackTimerMemo->Lines->Strings[1] = "";
		whiteTimerMemo->Lines->Strings[0] = "Timer is off.";
		whiteTimerMemo->Lines->Strings[1] = "";
	}
	else if(menuTimerSwitch->Checked == false) {
		chessGraphic->Enabled = false;
		menuTimerStop->Caption = "Start";
		menuTimerStop->Enabled = true;
		menuTimerSwitch->Checked = true;
		menuTimerOptions->Enabled = true;

		whiteTimerStop->Enabled = true;
		blackTimerStop->Enabled = false;
		whiteTimerStop->Caption = "Start";
		blackTimerStop->Caption = "Start";

		tOpt->timerOn = true;
		if(tOpt->gameLimit == true) {
			blackTimerMemo->Lines->Strings[0] = AnsiString(tOpt->gameLimitTime) + ": 00";
			blackTimerMemo->Lines->Strings[1] = "Time for a game.";
			whiteTimerMemo->Lines->Strings[0] = AnsiString(tOpt->gameLimitTime) + ": 00";
			whiteTimerMemo->Lines->Strings[1] = "Time for a game.";
		}
		else if(tOpt->moveLimit == true) {
			blackTimerMemo->Lines->Strings[0] = AnsiString(tOpt->moveLimitTime) + ": 00";
			blackTimerMemo->Lines->Strings[1] = "Time for 1 move.";
			whiteTimerMemo->Lines->Strings[0] = AnsiString(tOpt->moveLimitTime) + ": 00";
			whiteTimerMemo->Lines->Strings[1] = "Time for 1 move.";

		}
		else if(tOpt->someMovesLimit == true) {
			blackTimerMemo->Lines->Strings[0] = AnsiString(tOpt->firstPerTime) + ": 00";
			blackTimerMemo->Lines->Strings[1]= "Time for " + AnsiString(tOpt->firstPerMovesCount) + " moves.";
			whiteTimerMemo->Lines->Strings[0] = AnsiString(tOpt->firstPerTime) + ": 00";
			whiteTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount) + " moves.";

        }
	}

	//-----------��������� ��������� ������������� ������� Switch On � ��������� ����� ��������-----------------------------------
	ofstream outFileStream;
	outFileStream.open((ExtractFilePath(Application->ExeName) + "Options_files" + "/Timer_options.cfg").c_str(), ios::trunc);
	if(!outFileStream)
		return;
	outFileStream << "TIMER_ON = ";
	outFileStream << tOpt->timerOn << ';' << endl;
	outFileStream << "GAME_LIMIT = ";
	outFileStream << tOpt->gameLimit << ';' << endl;
	outFileStream << "MOVE_LIMIT = ";
	outFileStream << tOpt->moveLimit << ';' <<  endl;
	outFileStream << "GAME_LIMIT_TIME = ";
	outFileStream << tOpt->gameLimitTime << ';' << endl;
	outFileStream << "MOVE_LIMIT_TIME = ";
	outFileStream << tOpt->moveLimitTime << ';' << endl;
	outFileStream << "SOME_MOVES_LIMIT = ";
	outFileStream << tOpt->someMovesLimit << ';' << endl;
	outFileStream << "FIRST_PER_MOVES_COUNT = ";
	outFileStream << tOpt->firstPerMovesCount << ';' << endl;
	outFileStream << "SECOND_PER_MOVES_COUNT = ";
	outFileStream << tOpt->secondPerMovesCount << ';' << endl;
	outFileStream << "FIRST_PER_TIME = ";
	outFileStream << tOpt->firstPerTime << ';' << endl;
	outFileStream << "SECOND_PER_TIME = ";
	outFileStream << tOpt->secondPerTime << ';' << endl;
	outFileStream << "FINAL_PER_TIME = ";
	outFileStream << tOpt->finalPerTime << ';' << endl;
	outFileStream << "ADD_TIME_PER_MOVE = ";
	outFileStream << tOpt->addTimePerMove << ';' << endl;
	//-------------------------------------------------------------------------------------------------------------------
	outFileStream.close();
}
//---------------------------------------------------------------------------

void __fastcall TMainChessForm::menuSaveGameClick(TObject *Sender)
{
	bool errorIsShowed = false;
    AnsiString saveFileName;
	if(saveGameDialog->Execute())
		saveFileName = saveGameDialog->FileName;
	else return;
	ofstream saveFileStream;
	saveFileStream.open(saveFileName.c_str(), ios::binary | ios::trunc);
	if(!saveFileStream) {
		ShowMessage("Can't open file to save!");
		return;
	}


	struct saveGameSect {
		deskSection prevPosition;          //���������� ������� ������ (����������� ��� �����)
		bool isMoved;                      //���� ������������ ������ (��������������� ��� ������ ��� �����, ���� ��� ���������� ���� �� 1 ���)
		bool isChecked;                   //���� ���������� ���� (��������������� ��� ������, �������� ��������� ���)
		char coordX;                    //�-���������� ���������� ����
		int coordY;                     //Y-���������� ���������� ����
		chessColor chColor;
		chessType chType;
		int chNumber;

	};
	saveGameSect* saveDesk = new saveGameSect[SECTCOUNT];         //��������� ���������� ���� ����������� �������� �������� saveGameSect,
												//���� �������� �� �������� ����������, ����� ���� ����������� ���������� ���� � �������� ����
	map<deskSection*, chessPiece*, sectCompare>* locMap;
	locMap = chDskPaint->getChDsk()->getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	int i;
	for(i = 0, locMapIt = locMap->begin(); i < SECTCOUNT && locMapIt != locMap->end(); i++, locMapIt++) {
		if(locMapIt->second->getChessType() == NOPIECE) {
			saveDesk[i].isMoved = false;
			saveDesk[i].isChecked = false;
			saveDesk[i].chColor = NOCOLOR;
			saveDesk[i].chType = NOPIECE;
			saveDesk[i].coordX = locMapIt->first->symbol;
			saveDesk[i].coordY = locMapIt->first->number;
			saveDesk[i].chNumber = 0;


		}
		else if(locMapIt->second->getChessType() == PAWN) {
			saveDesk[i].isMoved = false;
			saveDesk[i].isChecked = false;
			if(locMapIt->second->getPrevPos() != NULL)
				saveDesk[i].prevPosition = *locMapIt->second->getPrevPos();
			saveDesk[i].chColor = locMapIt->second->getChessColor();
			saveDesk[i].chNumber = locMapIt->second->getChessNumber();
			saveDesk[i].chType = PAWN;
			saveDesk[i].coordX = locMapIt->first->symbol;
			saveDesk[i].coordY = locMapIt->first->number;

		}
		else if(locMapIt->second->getChessType()  == ROOK) {
			saveDesk[i].isMoved = locMapIt->second->getIsMoved();
			saveDesk[i].isChecked = false;
			saveDesk[i].chColor = locMapIt->second->getChessColor();
			saveDesk[i].chNumber = locMapIt->second->getChessNumber();
			saveDesk[i].chType = ROOK;
			saveDesk[i].coordX = locMapIt->first->symbol;
			saveDesk[i].coordY = locMapIt->first->number;

		}
		else if(locMapIt->second->getChessType() == KING) {
			saveDesk[i].isMoved = locMapIt->second->getIsMoved();
			saveDesk[i].isChecked = locMapIt->second->getCheck();
			saveDesk[i].chColor = locMapIt->second->getChessColor();
			saveDesk[i].chNumber = locMapIt->second->getChessNumber();
			saveDesk[i].chType = KING;
			saveDesk[i].coordX = locMapIt->first->symbol;
			saveDesk[i].coordY = locMapIt->first->number;
		}
		else {
			saveDesk[i].isMoved = false;
			saveDesk[i].isChecked = false;
			saveDesk[i].chColor = locMapIt->second->getChessColor();
			saveDesk[i].chType = locMapIt->second->getChessType();
			saveDesk[i].chNumber = locMapIt->second->getChessNumber();
            saveDesk[i].coordX = locMapIt->first->symbol;
			saveDesk[i].coordY = locMapIt->first->number;
		}
	}



	saveFileStream.write(reinterpret_cast<char*>(saveDesk), sizeof(saveGameSect) * SECTCOUNT);
	if(!saveFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Writing error!");
			errorIsShowed = true;
		}

	unsigned* movesCounterPtr = new unsigned;                      //�������� ������ ��� �����, ������ ���������� �����
	*movesCounterPtr = moveNotes[moveNotes.size() - 1].moveCounter;                 //�������������� ��������� ����������. ����� �������� ������� vecto<moveNote> moveNotes
	saveFileStream.write(reinterpret_cast<char*>(movesCounterPtr), sizeof(unsigned));        //��������� ���������� ����� � �������� ����
	if(!saveFileStream)
		ShowMessage("Writing error!");

	for(int i = 0; i < *movesCounterPtr ; i++) {
		moveNote* moveNotePtr = &moveNotes[i];
		saveFileStream.write(reinterpret_cast<char*>(moveNotePtr), sizeof(moveNote));
		if(!saveFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Writing error!");
				errorIsShowed = true;
			}
	}
	unsigned* uselessCountPtr = new unsigned;
	*uselessCountPtr = moveNote::uselessCounter;
	saveFileStream.write(reinterpret_cast<char*>(uselessCountPtr), sizeof(unsigned));
	if(!saveFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Writing error!");
			errorIsShowed = true;
		}

	unsigned* desksCounterPtr = new unsigned;
	*desksCounterPtr = deskNotes.size();
	saveFileStream.write(reinterpret_cast<char*>(desksCounterPtr), sizeof(unsigned));
	if(!saveFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Writing error!");
			errorIsShowed = true;
		}

	for(int i = 0; i < *desksCounterPtr; i ++) {
		deskNote* deskNotePtr = &deskNotes[i];
		saveFileStream.write(reinterpret_cast<char*>(deskNotePtr), sizeof(deskNote));
		if(!saveFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Writing error!");
				errorIsShowed = true;
			}
	}

	chessColor* currTurnPtr = new chessColor;
	*currTurnPtr = currTurn;
	saveFileStream.write(reinterpret_cast<char*>(currTurnPtr), sizeof(chessColor));
	if(!saveFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Writing error!");
			errorIsShowed = true;
		}




	char* timerOnPtr = new char;
	*timerOnPtr = tOpt->timerOn;
	saveFileStream.write(timerOnPtr, 1);
	if(!saveFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Writing error!");
			errorIsShowed = true;
		}

	//-------��������� � ���������� ��������������� ��������� timerOptSave ��� ����������� ���������� ����� ������� � ��������� ���� � �������� ����--------
	struct timerOptSave {
		bool timerOn;
		bool gameLimit;
		bool moveLimit;
		unsigned gameLimitTime;
		unsigned moveLimitTime;
		bool someMovesLimit;
		unsigned firstPerMovesCount;
		unsigned secondPerMovesCount;
		unsigned firstPerTime;
		unsigned secondPerTime;
		unsigned finalPerTime;
		unsigned addTimePerMove;
		timerOptSave() {

		}
		timerOptSave(timerOptions locOpt) {
			timerOn = locOpt.timerOn;
			gameLimit = locOpt.gameLimit;
			moveLimit = locOpt.moveLimit;
			gameLimitTime = locOpt.gameLimitTime;
			moveLimitTime = locOpt.moveLimitTime;
			someMovesLimit = locOpt.someMovesLimit;
			firstPerMovesCount = locOpt.firstPerMovesCount;
			secondPerMovesCount = locOpt.secondPerMovesCount;
			firstPerTime = locOpt.firstPerTime;
			secondPerTime = locOpt.secondPerTime;
			finalPerTime = locOpt.finalPerTime;
			addTimePerMove = locOpt.addTimePerMove;

		}
		operator timerOptions() {
			timerOptions locOpt;
			locOpt.timerOn = timerOn;
			locOpt.gameLimit = gameLimit;
			locOpt.moveLimit = moveLimit;
			locOpt.gameLimitTime = gameLimitTime;
			locOpt.moveLimitTime = moveLimitTime;
			locOpt.someMovesLimit = someMovesLimit;
			locOpt.firstPerMovesCount = firstPerMovesCount;
			locOpt.secondPerMovesCount = secondPerMovesCount;
			locOpt.firstPerTime = firstPerTime;
			locOpt.secondPerTime = secondPerTime;
			locOpt.finalPerTime = finalPerTime;
			locOpt.addTimePerMove = addTimePerMove;
			return locOpt;
		}
	};
   //-----------------------------------------------------------------------------------------------------------------------
	timerOptSave* tOptSave = new timerOptSave;
	*tOptSave = *tOpt;

	if(tOpt->timerOn == true) {

		int* blackTimePtr = new int;
		int* whiteTimePtr = new int;
		*blackTimePtr = blackTotalTime;
		*whiteTimePtr = whiteTotalTime;
		saveFileStream.write(reinterpret_cast<char*>(whiteTimePtr), sizeof(unsigned));
		if(!saveFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Writing error!");
				errorIsShowed = true;
			}
		saveFileStream.write(reinterpret_cast<char*>(blackTimePtr), sizeof(unsigned));
		if(!saveFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Writing error!");
				errorIsShowed = true;
			}

		saveFileStream.write(reinterpret_cast<char*>(tOptSave), sizeof(timerOptions));
		if(!saveFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Writing error!");
				errorIsShowed = true;
			}

        delete blackTimePtr;
		delete whiteTimePtr;
		delete timerOnPtr;
	}

	delete[] saveDesk;
	delete movesCounterPtr;
	delete uselessCountPtr;
	delete desksCounterPtr;
	delete currTurnPtr;
	delete tOptSave;

	saveFileStream.close();
	return;
}
//---------------------------------------------------------------------------

void __fastcall TMainChessForm::menuOpenGameClick(TObject *Sender)
{

	blackTimer->Enabled = false;
	whiteTimer->Enabled = false;
	firstMoveMade = true;
	resultIsShowed = false;


	bool  errorIsShowed = false;
	AnsiString openFileName;
	if(openGameDialog->Execute())
		openFileName = openGameDialog->FileName;
	else return;                                          //����  ���������� ���� �������� ����� ������� ��� ������ ����������� �����, ��������� ���������� ������

	//-----------------------� ������� ���������� �������� ����� ����������� ���������� ����������  ��� ������ ���� (���� �� ����� ��� ����� ���������. ������)-----------------------------
	if(repMovesBox != NULL) {               //���� ���������� ����-���� ����� ������ (�.�. ����� ���������� ������)
		if(repMovesBox->Visible == true) {      //���� ����-���� ����� ������ ������������

			repMovesBox->Hide();                //�������� ���������� ����������, ������������ � ������ ���������. ������
			nextMainButt->Hide();
			prevMainButt->Hide();
			playMainButt->Hide();
			gotoMainButt->Hide();
			numberMainEdit->Hide();
			delayMainEdit->Hide();
			delayRepLabel->Hide();
			samePosLabel->Hide();
			uselessMovesLabel->Hide();

			blackTimerMemo->Show();                 //������� ���������� ����������, ������������ � ������ ����
			whiteTimerMemo->Show();
			whiteTimerStop->Show();
			blackTimerStop->Show();
			whiteDrawOffer->Show();
			blackDrawOffer->Show();
			blackConcede->Show();
			whiteConcede->Show();
			movesLog->Show();
			samePosInGame->Show();
			uselessMovesInGame->Show();
		}
    }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	ifstream openFileStream;
	openFileStream.open(openFileName.c_str(), ios::binary);
	if(!openFileStream) {
		ShowMessage("Can't open file!");
		return;
	}

	struct saveGameSect {
		deskSection prevPosition;          //���������� ������� ������ (����������� ��� �����)
		bool isMoved;                      //���� ������������ ������ (��������������� ��� ������ ��� �����, ���� ��� ���������� ���� �� 1 ���)
		bool isChecked;                   //���� ���������� ���� (��������������� ��� ������, �������� ��������� ���)
		char coordX;                    //�-���������� ���������� ����
		int coordY;                     //Y-���������� ���������� ����
		chessColor chColor;
		chessType chType;
		int chNumber;
		saveGameSect() : isMoved(false) {
			chColor = NOCOLOR;
			chType = NOPIECE;
        }

	};

	saveGameSect* saveDesk = new saveGameSect[SECTCOUNT];         //��������� ���������� ���� ����������� �������� �������� saveGameSect,
																	//���� �������� �� �������� ����������, ����� ���� ����������� ���������� ���� � �������� ����
	openFileStream.read(reinterpret_cast<char*>(saveDesk), sizeof(saveGameSect) * SECTCOUNT);
	if(!openFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Reading error!");
			errorIsShowed = true;
		}

	unsigned* movesCounterPtr = new unsigned;
	openFileStream.read(reinterpret_cast<char*>(movesCounterPtr), sizeof(unsigned));
	if(!openFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Reading error!");
			errorIsShowed = true;
		}
	moveNote::moveCounter = *movesCounterPtr;

	moveNote* moveNotePtr = new moveNote;
	moveNote::moveCounter--;
	moveNotes.clear();
	movesLog->Clear();
	for(int i = 0; i < *movesCounterPtr; i++) {
		openFileStream.read(reinterpret_cast<char*>(moveNotePtr), sizeof(moveNote));
		if(!openFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Reading error!");
				errorIsShowed = true;
			}
		moveNotes.push_back(*moveNotePtr);
		movesLog->Lines->Add((*moveNotePtr).getTextMove().c_str());
	}

	unsigned* uselessCountPtr = new unsigned;
	openFileStream.read(reinterpret_cast<char*>(uselessCountPtr), sizeof(unsigned));
	if(!openFileStream)
		 if(errorIsShowed == false) {
			ShowMessage("Reading error!");
			errorIsShowed = true;
		 }
	moveNote::uselessCounter = *uselessCountPtr;

	unsigned* desksCounterPtr = new unsigned;
	openFileStream.read(reinterpret_cast<char*>(desksCounterPtr), sizeof(unsigned));
	if(!openFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Reading error!");
			errorIsShowed = true;
		}


	deskNote* deskNotePtr = new deskNote;
	deskNotes.clear();
    deskNote::currDesk = 0;
	for(int i = 0; i < *desksCounterPtr; i++) {
		openFileStream.read(reinterpret_cast<char*>(deskNotePtr), sizeof(deskNote));
		if(!openFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Reading error!");
				errorIsShowed = true;
			}
		deskNotes.push_back(*deskNotePtr);
	}


	chessColor* currTurnPtr = new chessColor;
	openFileStream.read(reinterpret_cast<char*>(currTurnPtr), sizeof(chessColor));
	if(!openFileStream)
		if(errorIsShowed == false) {
			ShowMessage("Reading error!");
			errorIsShowed = true;
		}

	currTurn = *currTurnPtr;

	if(currTurn == BLACK)           //���� ������� ��� - ��� ������
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] += "";         //���������� ������ �� ��������� ��������� ����� � ��������� ���� movesLog
	else if(currTurn == WHITE) {      //�����, ���� ������� ��� - ��� �����
		movesLog->Lines->Add("");    //��������� ������ �� ����� ������� ��������� �������
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] += "";        //��������� ������ �� ��������� (���������) ������
	}

	char* timerOnPtr = new char;                  //��������� � ���������� ��������� ��������� �� ������� ������ �������
	openFileStream.read(timerOnPtr, 1);           //������ ������� ������ ������� �� ��������� �����
	if(!openFileStream)                           //���� ��������� ���� �� �������,
		if(errorIsShowed == false) {             //� � �������� �� ������ ����� �� ����������
			ShowMessage("Reading error!");        //������� ��������� �� ������
			errorIsShowed = true;                 //������������� ���� ����������� ��������� �� ������
		}
	tOpt->timerOn = *timerOnPtr;                 //�������� �������� ������� �������� ������� (���� ������ �������) ����������� ��������, �� ������� ��������� ��������� ����.

		//-------��������� � ���������� ��������������� ��������� timerOptSave ��� ����������� ������ ����� ������� �� ���������� ����� � �������� ����--------
	struct timerOptSave {
		bool timerOn;
		bool gameLimit;
		bool moveLimit;
		unsigned gameLimitTime;
		unsigned moveLimitTime;
		bool someMovesLimit;
		unsigned firstPerMovesCount;
		unsigned secondPerMovesCount;
		unsigned firstPerTime;
		unsigned secondPerTime;
		unsigned finalPerTime;
		unsigned addTimePerMove;
		timerOptSave() {

		}
		timerOptSave(timerOptions locOpt) {
			timerOn = locOpt.timerOn;
			gameLimit = locOpt.gameLimit;
			moveLimit = locOpt.moveLimit;
			gameLimitTime = locOpt.gameLimitTime;
			moveLimitTime = locOpt.moveLimitTime;
			someMovesLimit = locOpt.someMovesLimit;
			firstPerMovesCount = locOpt.firstPerMovesCount;
			secondPerMovesCount = locOpt.secondPerMovesCount;
			firstPerTime = locOpt.firstPerTime;
			secondPerTime = locOpt.secondPerTime;
			finalPerTime = locOpt.finalPerTime;
			addTimePerMove = locOpt.addTimePerMove;
		}
		operator timerOptions() {
			timerOptions locOpt;
			locOpt.timerOn = timerOn;
			locOpt.gameLimit = gameLimit;
			locOpt.moveLimit = moveLimit;
			locOpt.gameLimitTime = gameLimitTime;
			locOpt.moveLimitTime = moveLimitTime;
			locOpt.someMovesLimit = someMovesLimit;
			locOpt.firstPerMovesCount = firstPerMovesCount;
			locOpt.secondPerMovesCount = secondPerMovesCount;
			locOpt.firstPerTime = firstPerTime;
			locOpt.secondPerTime = secondPerTime;
			locOpt.finalPerTime = finalPerTime;
			locOpt.addTimePerMove = addTimePerMove;
			return locOpt;
		}
	};
   //-----------------------------------------------------------------------------------------------------------------------


   //----------------------������ ��������� ������� �� ��������� ���������� ����� � ��������� �� � ������� ������� �������� �������-------------------------------------------------------
	if(tOpt->timerOn) {
		menuTimerStop->Caption = "Continue";
		menuTimerStop->Enabled = true;

		int* blackTimePtr = new int;
		int* whiteTimePtr = new int;

		openFileStream.read(reinterpret_cast<char*>(whiteTimePtr), sizeof(unsigned));
		if(!openFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Reading error!");
				errorIsShowed = true;
			}

		whiteTotalTime = *whiteTimePtr;

		openFileStream.read(reinterpret_cast<char*>(blackTimePtr), sizeof(unsigned));
		if(!openFileStream)
			if(errorIsShowed == false) {
				ShowMessage("Reading error!");
				errorIsShowed = true;
			}

		blackTotalTime = *blackTimePtr;


		timerOptSave* tOptSave = new timerOptSave;
		openFileStream.read(reinterpret_cast<char*>(tOptSave), sizeof(timerOptSave));
		if(!openFileStream)
			if(errorIsShowed == true) {
				ShowMessage("Reading error!");
				errorIsShowed = true;
			}
		*tOpt = *tOptSave;

		delete blackTimePtr;
		delete whiteTimePtr;
		delete tOptSave;
	//-------------------------------------------------------------------------------------------------------------------------------------------------


	//----------������� � ��������� ���� ������� � ������ �������� �������� � �� ��������� ��������� ����� �������� ����----------------------
		 if(tOpt->gameLimit == true) {
			 int minLeft = (tOpt->gameLimitTime * 60 - blackTotalTime) / 60;
			 int secLeft = tOpt->gameLimitTime * 60 - blackTotalTime - minLeft * 60;
			 blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
			 blackTimerMemo->Lines->Strings[1] = "Time for a game.";
			 minLeft = (tOpt->gameLimitTime * 60 - whiteTotalTime) / 60;
			 secLeft = tOpt->gameLimitTime * 60 - whiteTotalTime - minLeft * 60;
			 whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
			 whiteTimerMemo->Lines->Strings[1] = "Time for a game.";
		 }
		 else if(tOpt->moveLimit == true) {
			 int minLeft = (tOpt->moveLimitTime * 60 - blackTotalTime) / 60;
			 int secLeft = tOpt->moveLimitTime * 60 - blackTotalTime - minLeft * 60;
			 blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
			 blackTimerMemo->Lines->Strings[1] = "Time for 1 move.";
			 minLeft = (tOpt->moveLimitTime * 60 - whiteTotalTime) / 60;
			 secLeft = tOpt->moveLimitTime * 60 - whiteTotalTime - minLeft * 60;
			 whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
			 whiteTimerMemo->Lines->Strings[1] = "Time for 1 move.";
		 }
		 else if(tOpt->someMovesLimit == true) {
				if(moveNote::moveCounter < tOpt->firstPerMovesCount) {
					int minLeft = (tOpt->firstPerTime * 60 - blackTotalTime) / 60;
					int secLeft = tOpt->firstPerTime * 60 - blackTotalTime - minLeft * 60;
					blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
					if(currTurn == WHITE)
						blackTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount - moveNote::moveCounter) + " moves.";
					else if(currTurn == BLACK)
						blackTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount - (moveNote::moveCounter - 1)) + " moves.";
					minLeft = (tOpt->firstPerTime * 60 - whiteTotalTime) /60;
					secLeft = tOpt->firstPerTime * 60 - whiteTotalTime - minLeft * 60;
					whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
					whiteTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount - moveNote::moveCounter) + " moves.";


				}
				else if(moveNote::moveCounter >= tOpt->firstPerMovesCount && moveNote::moveCounter < tOpt->secondPerMovesCount) {
					int minLeft = (tOpt->secondPerTime * 60 - blackTotalTime) / 60;
					int secLeft = tOpt->secondPerTime * 60 - blackTotalTime - minLeft * 60;
					blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
					if(currTurn == WHITE)
						blackTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount + tOpt->secondPerMovesCount - moveNote::moveCounter) + " moves.";
					else if(currTurn == BLACK)
						blackTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount + tOpt->secondPerMovesCount - (moveNote::moveCounter - 1)) + " moves.";
					minLeft = (tOpt->secondPerTime * 60 - whiteTotalTime) / 60;
					secLeft = tOpt->secondPerTime * 60 - whiteTotalTime - minLeft * 60;
					whiteTimerMemo->Lines->Strings [0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
					whiteTimerMemo->Lines->Strings[1] = "Time for " + AnsiString(tOpt->firstPerMovesCount + tOpt->secondPerMovesCount - moveNote::moveCounter) + " moves.";
				}
				else if(moveNote::moveCounter >= tOpt->firstPerMovesCount + tOpt->secondPerMovesCount) {
					int minLeft = (tOpt->finalPerTime * 60 - blackTotalTime) / 60;
					int secLeft = tOpt->finalPerTime * 60 - blackTotalTime - minLeft * 60;
					blackTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
					blackTimerMemo->Lines->Strings[1] = "Time for a game.";
					minLeft = (tOpt->finalPerTime * 60 - whiteTotalTime) / 60;
					secLeft = tOpt->finalPerTime * 60 - whiteTotalTime - minLeft * 60;
					whiteTimerMemo->Lines->Strings[0] = IntToStr(minLeft) + " : " + IntToStr(secLeft);
					whiteTimerMemo->Lines->Strings[1] = "Time for a game.";
				}
		 }
		 //--------------------------------------------------------------------------------------------------------------------------------------------
		 //--------------���������� ��������� � ����������� ��������� ���������� � ��������� ����� ����� �������� ����---------------------------------
		 chessGraphic->Enabled = false;
		 menuTimerSwitch->Checked = true;
		 menuTimerSwitch->Enabled = false;
		 menuTimerOptions->Enabled = false;               //���� � ����������� ���� ������ ������������, ��������� ������ � ��� ����������
		 menuTimerStop->Enabled = true;
		 menuTimerStop->Caption = "Continue";


		 if(currTurn == WHITE) {
			 whiteTimerStop->Enabled = true;
			 whiteTimerStop->Caption = "Continue";
			 blackTimerStop->Enabled = false;
			 blackTimerStop->Caption = "Continue";
		 }
		 else if(currTurn == BLACK) {
			 blackTimerStop->Enabled = true;
			 blackTimerStop->Caption = "Continue";
			 whiteTimerStop->Enabled = false;
			 whiteTimerStop->Caption = "Continue";
		 }

		 //--------------------------------------------------------------------------------------------------------------------------------------------

	}
	else {
        chessGraphic->Enabled = true;
		menuTimerStop->Enabled = false;
		menuTimerStop->Caption = "Start";
		menuTimerSwitch->Checked = false;
        menuTimerSwitch->Enabled = false;
		menuTimerOptions->Enabled = true;                      //���� � ����������� ���� ������ �� ������������, ��������� ������ � ��� ����������
		blackTimerStop->Enabled = false;
		blackTimerStop->Caption = "Start";
		whiteTimerStop->Enabled = false;
		whiteTimerStop->Caption = "Start";
		blackTimer->Enabled = false;
		whiteTimer->Enabled = false;
		//-------������� � ��������� ���� ������� � ������ �������� �������� � �� ��������� ��������� ����� �������� ����--------------
		blackTimerMemo->Lines->Strings[0] = "Timer is off.";
		blackTimerMemo->Lines->Delete(1);
		whiteTimerMemo->Lines->Strings[0] = "Timer is off.";
		whiteTimerMemo->Lines->Delete(1);
		//--------------------------------------------------------------------------------------------------------------------------------
	}

    menuOpenGame->Enabled = true;
	menuOpenReplay->Enabled = true;
	menuReplayControl->Enabled = false;
	menuSaveReplay->Enabled = false;
	menuTimer->Enabled = true;



	//--------------�������������� �������� �������� ��������  ���������� �� ����������� ������ (�������� ��� ��������)---------------------------------

	static chessDesk localDesk;
	chDskPaint->setChDsk(&localDesk);
	map<deskSection*, chessPiece*, sectCompare>* locMap = localDesk.getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	map<chessPiece*, deskSection*, lessPiecePtr>* locBlackChess = localDesk.getBlackChess();
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator locBlackIt;
	map<chessPiece*, deskSection*, lessPiecePtr>* locWhiteChess = localDesk.getWhiteChess();
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator locWhiteIt;

	 locMap->clear();
	 locBlackChess->clear();
     locWhiteChess->clear();

	chDskPaint->getChDsk()->clearAddPiece();                      //������� ������ �������������� ����� ���������� ����

	chessPiece* tmpPiecePtr;
	deskSection* tmpSectPtr;
	for(int i = 0; i < SECTCOUNT; i++) {
		switch(saveDesk[i].chType) {
		case PAWN:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, PAWN, saveDesk[i].chNumber);          //���������� ��������� �� ������� ������� ���.�����
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);      //���������� ��������� �� ��-� ������� ���. �����
			if(saveDesk[i].prevPosition.number != 0 && saveDesk[i].prevPosition.symbol != 0)     //���� ���������� ���������� ������� �����
				tmpPiecePtr->setPrevPos(localDesk.sectFromArr(saveDesk[i].prevPosition.symbol, saveDesk[i].prevPosition.number));     //������ ��������������� ���� �������-�����
			else tmpPiecePtr->setPrevPos(NULL);                                          //� ��������� ������ ����������� ���������������� ���� ����. �������� ����.

			locMap->insert(make_pair(tmpSectPtr, tmpPiecePtr));                        //��������� ������������������ ��������� �� ������-������ � ������ ���. ���� � �����������
			if(saveDesk[i].chColor == BLACK)
				locBlackChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			else if(saveDesk[i].chColor == WHITE)
				locWhiteChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			break;
		case KNIGHT:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, KNIGHT, saveDesk[i].chNumber);
			if(tmpPiecePtr->getChessType() == NOPIECE) {                                   //���� � ������� �������� � ����� ������� ���,
				tmpPiecePtr = new Knight(saveDesk[i].chColor, saveDesk[i].chNumber);        //�������� ��� ����� ������� ������
				localDesk.addPiece(tmpPiecePtr);                                         //��������� �� ���. ������ ��������� � ������ ���. �����
			}
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);
			locMap->insert(make_pair(tmpSectPtr, tmpPiecePtr));
			if(saveDesk[i].chColor == BLACK)
				locBlackChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			else if(saveDesk[i].chColor == WHITE)
				locWhiteChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			break;
		case BISHOP:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, BISHOP, saveDesk[i].chNumber);
			if(tmpPiecePtr->getChessType() == NOPIECE){
				tmpPiecePtr = new Bishop(saveDesk[i].chColor, saveDesk[i].chNumber);
				localDesk.addPiece(tmpPiecePtr);
			}
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);
			locMap->insert(make_pair(tmpSectPtr, tmpPiecePtr));
			if(saveDesk[i].chColor == BLACK)
				locBlackChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			else if(saveDesk[i].chColor == WHITE)
				locWhiteChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			break;
		case ROOK:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, ROOK, saveDesk[i].chNumber);
			if(tmpPiecePtr->getChessType() == NOPIECE) {
				tmpPiecePtr = new Rook(saveDesk[i].chColor, saveDesk[i].chNumber);
				localDesk.addPiece(tmpPiecePtr);
			}
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);
			locMap->insert(make_pair(tmpSectPtr, tmpPiecePtr));
			if(saveDesk[i].chColor == WHITE)
				locWhiteChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			else if(saveDesk[i].chColor == BLACK)
				locBlackChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			break;
		case QUEEN:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, QUEEN, saveDesk[i].chNumber);
			if(tmpPiecePtr->getChessType() == NOPIECE) {
				tmpPiecePtr = new Queen(saveDesk[i].chColor, saveDesk[i].chNumber);
				localDesk.addPiece(tmpPiecePtr);
			}
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);
			locMap->insert(make_pair(tmpSectPtr, tmpPiecePtr));
			if(saveDesk[i].chColor == WHITE)
				locWhiteChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			else if(saveDesk[i].chColor == BLACK)
				locBlackChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			break;
		case KING:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, KING, saveDesk[i].chNumber);
			if(saveDesk[i].isMoved == true)
				tmpPiecePtr->setIsMoved(true);
			if(saveDesk[i].isChecked == true)
				tmpPiecePtr->setCheck(true);
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);
			locMap->insert(make_pair(tmpSectPtr, tmpPiecePtr));
			if(saveDesk[i].chColor == WHITE)
				locWhiteChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			else if(saveDesk[i].chColor == BLACK)
				locBlackChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			break;


		default:
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);
			locMap->insert(make_pair(tmpSectPtr, localDesk.getNopiece()));
			break;
		}


    }

	delete[] saveDesk;
	delete movesCounterPtr;
	delete moveNotePtr;
	delete uselessCountPtr;
	delete desksCounterPtr;
	delete deskNotePtr;

	delete currTurnPtr;
	openFileStream.close();
	chDskPaint->paintPieces();
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
}


void __fastcall TMainChessForm::menuMinimizeClick(TObject *Sender)
{
	if(menuMinimize->Checked == true) {
		menuMinimize->Checked = false;
		mainOpt->formMinimize = false;
		mainOpt->saveConfig();

		this->Width += ADDFORMWIDTH;
	}
	else if(menuMinimize->Checked == false) {
		menuMinimize->Checked = true;
		mainOpt->formMinimize = true;
		mainOpt->saveConfig();
		this->Width -= ADDFORMWIDTH;
	}
}
//---------------------------------------------------------------------------






void __fastcall TMainChessForm::menuOpenReplayClick(TObject *Sender)
{
    AnsiString openReplayFile;
	if(openReplayDialog->Execute())
		openReplayFile = openReplayDialog->FileName;
	else return;

//----------�������� ��������� ����� � �������� ���������----------------------------------------------------------------------
	prevSect = NULL;
	prevChosenSect = NULL;
	if(possMovesPtr != NULL)
		possMovesPtr->clear();
	if(possTakesPtr != NULL)
		possTakesPtr->clear();
	possMovesPtr = NULL;
	possTakesPtr = NULL;
	currTurn = WHITE;
	blackChecked = false;
	whiteChecked = false;
	firstMoveMade = false;
	resultIsShowed = false;
	chDskPaint->setChDsk();
	chDskPaint->getChDsk()->startDesk();
	moveNote::moveCounter = 0;
	moveNote::uselessCounter = 0;
	moveNotes.clear();
	repMoveNote::currMove = 0;
	repMoveNote::moveCounter = 0;
	repMoveNote::repIsFinished = false;
	repMoveNotes.clear();
	replayTimer->Enabled = false;
	sameDesksCount = 1;
	deskNotes.clear();
	deskNote::currDesk = 0;
	chDskPaint->paintPieces();

	blackTimer->Enabled = false;                //���� ����� ���������� ���� �������� ���������� ������� ��������� ��
	whiteTimer->Enabled = false;
	movesLog->Clear();                         //������� �������� ���� ��� ����������� �����
	samePosLabel->Caption = "Same positions: 1";            //������ ��������� �������� ����� ������������� ������� ������
	uselessMovesLabel->Caption = "Reversible moves: 0";    //������ ��������� �������� ����� ��� ����������� ���������
	//-------------------------------------------------------------------------------------------------------------------------



	//------------������ ������ ������� � �������, �������� ��������� ������� (��� ���������� ������� 3-� ������������� �������)----
	map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPaint->getChDsk()->getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;
	int counter = 0;
	deskNote dNote;
	for(tmpIt = tmpMap->begin(); tmpIt != tmpMap->end(); tmpIt++) {
		dNote.chColor[counter] = (*tmpIt).second->getChessColor();
		dNote.chType[counter] = (*tmpIt).second->getChessType();
		counter++;
	}
	dNote.blackShortCasAble = false;
	dNote.blackLongCasAble = false;
	dNote.whiteShortCasAble = false;
	dNote.whiteLongCasAble = false;
	dNote.whiteEpAble = false;
	dNote.blackEpAble = false;
	dNote.lastTurn = BLACK;
	deskNotes.push_back(dNote);
	//---------------------------------------------------------------------------------------------------------------------------
	chessGraphic->Enabled = false;
//-------------------------------------------------------------------------------------------------------------------------------

	//-------------------����������� ������� �������� ����� ������������� ��� ����� ��������������� ������---------------------
	blackTimerMemo->Hide();        //�������� ����������, ������������ � ����������� ������� ��� ������� ����
	whiteTimerMemo->Hide();
	whiteTimerStop->Hide();
	blackTimerStop->Hide();
	blackDrawOffer->Hide();
	whiteDrawOffer->Hide();
	blackConcede->Hide();
	whiteConcede->Hide();
	movesLog->Hide();
	samePosInGame->Hide();
    uselessMovesInGame->Hide();

	if(repMovesBox == NULL) {                 //���� ���������� ����������� ������� ������� ����� ��� �� ���� ������� �����
		repMovesBox = new TListBox(this);
		repMovesBox->Parent = this;

		repMovesBox->Left = 910;
		repMovesBox->Top = 5;
		repMovesBox->Width = 270;
		repMovesBox->Height = 700;
		repMovesBox->ItemHeight = 30;
		repMovesBox->OnClick = repMovesBoxClick;



		repMovesBox->Style = lbOwnerDrawFixed;             //������������� ����� ���������

		repMovesBox->OnDrawItem = repMovesBoxDraw;        //� ������� ��������� �������� ����-����� ����������� ����������� �����

		playMainButt = new TButton(this);
		playMainButt->Parent = this;
		playMainButt->Top = 840;
		playMainButt->Left = 910;
		playMainButt->Height = 55;
		playMainButt->Width = 65;
		playMainButt->Images = repPlayerButtons;
		playMainButt->ImageIndex = 0;
		playMainButt->ImageAlignment = iaCenter;
		playMainButt->Enabled = true;
		playMainButt->OnClick = playRepButtClick;

		prevMainButt = new TButton(this);
		prevMainButt->Parent = this;
		prevMainButt->Top = 840;
		prevMainButt->Left = 980;
		prevMainButt->Height = 55;
		prevMainButt->Width = 65;
		prevMainButt->Images = repPlayerButtons;
		prevMainButt->ImageIndex = 1;
		prevMainButt->ImageAlignment = iaCenter;
		prevMainButt->OnClick = prevRepButtClick;
		if(deskNote::currDesk == 1)
			prevMainButt->Enabled = false;

		nextMainButt = new TButton(this);
		nextMainButt->Parent = this;
		nextMainButt->Top = 840;
		nextMainButt->Left = 1050;
		nextMainButt->Height = 55;
		nextMainButt->Width = 65;
		nextMainButt->Images = repPlayerButtons;
		nextMainButt->ImageIndex = 2;
		nextMainButt->ImageAlignment = iaCenter;
		nextMainButt->OnClick = nextRepButtClick;
		nextMainButt->Enabled = true;

		gotoMainButt = new TButton(this);
		gotoMainButt->Parent = this;
		gotoMainButt->Top = 871;
		gotoMainButt->Left = 1120;
		gotoMainButt->Height = 23;
		gotoMainButt->Width = 65;
		gotoMainButt->Images = repPlayerButtons;
		gotoMainButt->ImageIndex = 3;
		gotoMainButt->ImageAlignment = iaCenter;
		gotoMainButt->OnClick = gotoRepButtClick;
		gotoMainButt->Enabled = true;
		gotoMainButt->Name = "gotoMain";
		gotoMainButt->Caption = "";

		numberMainEdit = new TEdit(this);
		numberMainEdit->Parent = this;
		numberMainEdit->Top = 840;
		numberMainEdit->Left = 1120;
		numberMainEdit->Width = 65;
		numberMainEdit->Text = repMoveNote::currMove + 1;
		numberMainEdit->Alignment = taCenter;
		numberMainEdit->Font->Size = 12;
		numberMainEdit->Font->Style = TFontStyles() << fsBold;

		delayMainEdit = new TEdit(this);
		delayMainEdit->Parent = this;
		delayMainEdit->Top = 800;
		delayMainEdit->Left = 1120;
		delayMainEdit->Width = 65;
		delayMainEdit->Font->Size = 12;
		delayMainEdit->Font->Style = TFontStyles() << fsBold;
		delayMainEdit->Text = UIntToStr(mainOpt->repMovesDelay);
		delayMainEdit->Alignment = taCenter;

		delayRepLabel = new TLabel(this);
		delayRepLabel->Parent = this;
		delayRepLabel->Top = 800;
		delayRepLabel->Left = 950;
		delayRepLabel->Caption = "Play movement delay:";
		delayRepLabel->Font->Size = 12;
	   //	delayRepLabel->Caption += UIntToStr(mainOpt->repMovesDelay);

		samePosLabel = new TLabel(this);
		samePosLabel->Parent = this;
		samePosLabel->Top = 770;
		samePosLabel->Left = 950;
		samePosLabel->Font->Size = 12;
		samePosLabel->Caption = "Same positions: ";
		samePosLabel->Caption += UIntToStr(sameDesksCount);

		uselessMovesLabel = new TLabel(this);
		uselessMovesLabel->Parent = this;
		uselessMovesLabel->Top = 740;
		uselessMovesLabel->Left = 950;
		uselessMovesLabel->Font->Size = 12;
		uselessMovesLabel->Caption = "Reversible moves: ";
		uselessMovesLabel->Caption += UIntToStr(moveNote::uselessCounter);
	}
	else {
		repMovesBox->Clear();
		prevMainButt->Enabled = false;
		nextMainButt->Enabled = true;
		playMainButt->ImageIndex = 0;
		playMainButt->Enabled = true;
		numberMainEdit->Text = repMoveNote::currMove + 1;

		repMovesBox->Show();
		nextMainButt->Show();
		prevMainButt->Show();
		playMainButt->Show();
		gotoMainButt->Show();
		numberMainEdit->Show();
		delayMainEdit->Show();
		delayRepLabel->Show();
		samePosLabel->Show();
		uselessMovesLabel->Show();
		if(menuReplayControl->Checked == true) {
			prevRepButt->Enabled = false;
			nextRepButt->Enabled = true;
			playRepButt->ImageIndex = 0;
			playRepButt->Enabled = true;
			numberRepEdit->Text = repMoveNote::currMove + 1;
        }

	}

	//-----------------------------------------------------------------------------------------------------------------------
	if(menuMinimize->Checked == true)           //���� ��� �������� ������ �������� ����� ������� ��� ����������� �������
		menuReplayControlClick(menuReplayControl);            //��������� ������ ���������� �������

	//---------------------------��������� � ��������� ������ ���� ������--------------------------------------------------------
	ifstream openFileStream;
	openFileStream.open(openReplayFile.c_str());
	if(!openFileStream) {
		ShowMessage("Error! Can't open replay file.");
		return;
	}
	string moveStr;

	while(getline(openFileStream, moveStr)) {           //��������� ������ �������� ��������� ����
		repMoveNote rpNote;                             //��������� ������ ���� reMoveNote (��� ������)
		rpNote.setMoveText(moveStr);                    //� ������� ������, �������������� �������� ���� � ���. ������� (�����), ����������� �������� ��������� ����� ���������� �������
		repMoveNotes.push_back(rpNote);                //��������� ��������� ������ � ��������������� ������
		TListBoxItemInfo* itemInfo = new TListBoxItemInfo;           //��������� � ���������� ������ � ������� � �������� ����-����� repMovesBox
		itemInfo->moveIsMade = false;
		repMovesBox->AddItem(AnsiString(moveStr.c_str()), itemInfo);

	}

	//--------------------------------------------------------------------------------------------------------------------------
	repMovesBox->ItemIndex = 0;                      //���������� ��������� ��-�� ����-����� ������������� ������ ��-�
	openFileStream.close();

	menuReplayControl->Enabled = true;
	menuSaveGame->Enabled = false;
	menuSaveReplay->Enabled = false;
	menuOpenGame->Enabled = true;
	menuOpenReplay->Enabled = true;
	menuTimer->Enabled = false;


}

void __fastcall TMainChessForm::repMovesBoxDraw(TWinControl* Control, int Index, const TRect& Rect, TOwnerDrawState State) {

	TListBox *pListBox = dynamic_cast<TListBox*>(Control);
	TListBoxItemInfo* pListInfo = dynamic_cast<TListBoxItemInfo*>(pListBox->Items->Objects[Index]);
	TCanvas *pCanvas = pListBox->Canvas;
	pCanvas->Font->Size = 15;
	if(pListInfo->moveIsMade == false) {


		pCanvas->Brush->Color = StringToColor("0xC0C0C0");

	}
	if(State.Contains(odSelected)) {
		pCanvas->Brush->Color = StringToColor("0xff9900");
		pCanvas->Font->Color = clBlack;
	}


	pCanvas->FillRect(Rect);
	pCanvas->TextRect(Rect, Rect.Left,Rect.Top, pListBox->Items->Strings[Index]);

}

void __fastcall TMainChessForm::repMovesBoxClick(TObject* Sender) {
	if(repMovesBox->ItemIndex * 2 <= deskNotes.size() - 1) {             //���� ��������� ������������� ������ � ����-����� ������������� ��� ������������ ����
		unsigned deskIndex = repMovesBox->ItemIndex * 2;     //���������� ��������� ���������� - ������ ������� ��������� ���. ����� � �������������� �� �� ������� ���������� ��������
															 //� ����-����� ����� ������ (���-�� ��������� = ���-�� ����� * 2 + 1, ������� ��������� ����-����� ���������� � ����)
		restoreDeskNote(deskNotes[deskIndex]);
		deskNote::currDesk = deskIndex + 1;
		repMoveNote::currMove = (deskNote::currDesk - 1) / 2;
		if(replayControlForm != NULL) {
			nextRepButt->Enabled = true;                         //����� �������� �� �������� ��� �� ����� ������������ ���������� ������ ���������� ����
			playRepButt->Enabled = true;
		}
		if(nextMainButt != NULL) {
			nextMainButt->Enabled = true;
			playMainButt->Enabled = true;
		}
		if(repMovesBox->ItemIndex != 0) {                     //���� ������� ����������� �� �� ������ ���
			if(replayControlForm != NULL)
				prevRepButt->Enabled = true;                   //���������� ������ ����������� ���� �� ������ ���������� �������
			if(prevMainButt != NULL)
				prevMainButt->Enabled = true;                  //���������� ������ ����������� ���� � ������� ����������
		}
		else {
			if(replayControlForm != NULL)
				prevRepButt->Enabled = false;
			if(prevMainButt != NULL)
				prevMainButt->Enabled = false;
		}
		if(menuReplayControl->Checked == true) {
			if(replayControlForm != NULL)
				numberRepEdit->Text = repMovesBox->ItemIndex + 1;    //� ��������� ���� ������ ���������� ������� ���������� ����� ����, � �������� ������� ������������
			if(numberMainEdit != NULL)
				numberMainEdit->Text = repMovesBox->ItemIndex + 1;
		}
		else if(menuReplayControl->Checked == false) {
			if(numberMainEdit != NULL)
				numberMainEdit->Text = repMovesBox->ItemIndex + 1;
		}

		if(repMoveNote::currMove > 0) {
			uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].uselessMoves);
			samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].samePosesBlack);
		}
		else {
			uselessMovesLabel->Caption  = "Reversible moves: 0";
			samePosLabel->Caption = "Same positions: 1";
		}

	}
	else {                                                  //� ��������� ������, ���� ������ ��������� ����� ����� ���������� �������������������, ��������� ��� ���� ������ �� ���������� ������������
		if(deskNote::currDesk < deskNotes.size()) {
			restoreDeskNote(deskNotes[deskNotes.size() - 1]);
			deskNote::currDesk = deskNotes.size();
		}


		unsigned mCount;                //���������� �����, ������� ���������� ��������� (�������� ������� � ������) ��� �������� � ���������� ������������� ����
		mCount = repMovesBox->ItemIndex * 2 - ((deskNotes.size() - 1));         //������������ ���-�� �����, ������� ���������� ��������� (����� ��������� ������ ���� * 2 - ���-�� ��������� ���. ����� - 1)
		static bool repPanelIsOpen = true;            //���������� ��������� ����, ��������������� ��� �������� ������ ���������� �������
		//---------�������� ���������� ���������� ������ ���������� �������---------------------------------------
		if(menuReplayControl->Checked == false) {          //���� ������ ���������� ������� �� ���� ������� �������������
			repPanelIsOpen = false;             //������������� ����, ����������� ����������� ��������� ������ ���. ������� (�������\�������)
			menuReplayControlClick(menuReplayControl);       //�������� �����, ����������� ������, ��� ����� ���������� ��� ���������� ����������, ����������� �� ���
		}
		nextRepButt->Hide();
		prevRepButt->Hide();
		gotoRepButt->Hide();
		playRepButt->Hide();
		numberRepEdit->Hide();

		//---------------------------------------------------------------------------------------------------------
		//-----------------���������� ���������� ����������, �������������� � �������� ���������� ����� (��������-��� � ��.)-------
		repMovesProgress = new TProgressBar(replayControlForm);
		repMovesProgLabel = new TLabel(replayControlForm);
		repMovesProgLabel->Parent = replayControlForm;
		repMovesProgLabel->Left = 20;
		repMovesProgLabel->Top = 5;
		repMovesProgLabel->Caption = "Making replay moves...";
		repMovesProgress->Parent = replayControlForm;
		repMovesProgress->Height = 35;
		repMovesProgress->Width = 275;
		repMovesProgress->Top = 25;
		repMovesProgress->Left = 10;
		repMovesProgress->Max = 100;		   //���������� ��������� ��������-����
		//-----------------------------------------------------------------------------------------------------------

		if(mCount > 0) {                      //���� ���������� ����� ������ 0
			int movesProgStep = 100 / mCount;        //������������ ��� ��������-����
			for(int i = 0; i < mCount; i++) {
				makeRepMove();                                             //�������� ����� ���������� ���� ������ ������������ ����� ���
				repMovesProgress->StepBy(movesProgStep);
			}
			repMovesProgress->StepBy(movesProgStep);                              //��������� ��������� ������� � ��������-����
		}


		repMovesProgress->Hide();                                 //�������� ��������-���, ������������ ������� ���������� ����� ����� ����� �� ��� �� ������������ ����
		repMovesProgLabel->Hide();                               //�������� �����, ���������� ��������� ��������-����
		delete repMovesProgress;                                 //����������� ������, ����������� �������� ��������-����
		delete repMovesProgLabel;                               //����������� ������, ���������� �������� �����
		nextRepButt->Show();                                    //���������� ���������� ��������� �������� ������ ������ ���������� �������
		prevRepButt->Show();
		gotoRepButt->Show();
		playRepButt->Show();
		numberRepEdit->Show();
        if(repPanelIsOpen == false && replayControlForm != NULL)
			replayControlForm->Close();

		if(replayControlForm != NULL)
			nextRepButt->Enabled = true;
		if(nextMainButt != NULL)
			nextMainButt->Enabled = true;
		if(repMovesBox->ItemIndex != 0) {
			prevRepButt->Enabled = true;
			prevMainButt->Enabled = true;
		}
		else {
			prevRepButt->Enabled = false;
            prevMainButt->Enabled = false;
		}
		if(replayControlForm != NULL)
			numberRepEdit->Text = repMovesBox->ItemIndex + 1;      //� ��������� ���� ������ ���������� ������� ���������� ����� ����, � �������� ������� ������������
		if(numberMainEdit != NULL)
			numberMainEdit->Text = repMovesBox->ItemIndex + 1;
        samePosLabel->Caption = "Same positions: " + UIntToStr(sameDesksCount);
		uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(moveNote::uselessCounter);
	}

	repMoveNote::currMove = repMovesBox->ItemIndex;
	chDskPaint->paintPieces();                        //������������ ������ ����� �������� �� �������� ���
}



void __fastcall TMainChessForm::menuReplayControlClick(TObject *Sender) {

	if(menuReplayControl->Checked == false) {
		 menuReplayControl->Checked = true;

		replayControlForm = new TForm(this);
		replayControlForm->OnClose = replayControlClose;            //������� OnClose ����������� �����, �������������� ������, ���������� ������
																	//����� ���������� ����������, �.�. ����� replayControlForm ����� ���� ������� � ����� ������
		replayControlForm->Width = 300;
		replayControlForm->Height = 90;
		replayControlForm->BorderStyle = bsToolWindow;
		replayControlForm->Caption = "Replay control";
        replayControlForm->Position = poScreenCenter;
		replayControlForm->Show();


		playRepButt = new TButton(replayControlForm);
		playRepButt->Parent = replayControlForm;
		playRepButt->Top = 3;
		playRepButt->Left = 3;
		playRepButt->Height = 60;
		playRepButt->Width = 70;
		playRepButt->Images = repPlayerButtons;
		if(playMainButt != NULL) {                           //���� �������� ������ ��������������� ������ (� ����������� ������� ������� �����) ��� ����������,
			playRepButt->Enabled = playMainButt->Enabled;     //�������� �� ���������
			if(playMainButt->ImageIndex == 0)
				playRepButt->ImageIndex = 0;
			else if(playMainButt->ImageIndex == 4)
				playRepButt->ImageIndex = 4;
		}
		else
			playRepButt->ImageIndex = 0;
		playRepButt->ImageAlignment = iaCenter;
		playRepButt->OnClick = playRepButtClick;

		prevRepButt = new TButton(replayControlForm);
		prevRepButt->Parent = replayControlForm;
		prevRepButt->Top = 3;
		prevRepButt->Left = 76;
		prevRepButt->Height = 60;
		prevRepButt->Width = 70;
		prevRepButt->Images = repPlayerButtons;
		prevRepButt->ImageIndex = 1;
		prevRepButt->ImageAlignment = iaCenter;
		prevRepButt->OnClick = prevRepButtClick;
		if(deskNote::currDesk == 1)
			prevRepButt->Enabled = false;
		if(prevMainButt != NULL)                             //���� �������� ������ �������� � ����������� ���� ��� ����������, �������� �� ���������
			prevRepButt->Enabled = prevMainButt->Enabled;

		nextRepButt = new TButton(replayControlForm);
		nextRepButt->Parent = replayControlForm;
		nextRepButt->Top = 3;
		nextRepButt->Left = 149;
		nextRepButt->Height = 60;
		nextRepButt->Width = 70;
		nextRepButt->Images = repPlayerButtons;
		nextRepButt->ImageIndex = 2;
		nextRepButt->ImageAlignment = iaCenter;
		nextRepButt->OnClick = nextRepButtClick;
		nextRepButt->Enabled = true;
		if(nextMainButt != NULL)
			nextRepButt->Enabled = nextMainButt->Enabled;

		gotoRepButt = new TButton(replayControlForm);
		gotoRepButt->Parent = replayControlForm;
		gotoRepButt->Top = 39;
		gotoRepButt->Left = 222;
		gotoRepButt->Height = 24;
		gotoRepButt->Width = 70;
		gotoRepButt->Images = repPlayerButtons;
		gotoRepButt->ImageIndex = 3;
		gotoRepButt->ImageAlignment = iaCenter;
		gotoRepButt->OnClick = gotoRepButtClick;
		gotoRepButt->Enabled = true;
		gotoRepButt->Name = "gotoRep";
        gotoRepButt->Caption = "";

		numberRepEdit = new TEdit(replayControlForm);
		numberRepEdit->Parent = replayControlForm;
		numberRepEdit->Top = 3;
		numberRepEdit->Left = 222;
		numberRepEdit->Width = 70;
		if(repMoveNote::currMove < repMoveNote::moveCounter)
			numberRepEdit->Text = repMoveNote::currMove + 1;
		else
			numberRepEdit->Text = repMoveNote::moveCounter;
		numberRepEdit->Alignment = taCenter;
		numberRepEdit->Font->Size = 16;
		numberRepEdit->Font->Style = TFontStyles() << fsBold;
	}
	else if(menuReplayControl->Checked == true) {
		replayControlForm->Close();
		menuReplayControl->Checked = false;
	}


}

void __fastcall TMainChessForm::replayControlClose(TObject* Sender, TCloseAction& Action) {       //�����, ���������� �� ������� OnClose ����� replayControlForm
	Action = caFree;                           //������������� ������, ���������� ������, ��� �� ��������
	menuReplayControl->Checked = false;
    replayControlForm = NULL;
	return;

}

void __fastcall TMainChessForm::playRepButtClick(TObject* Sender) {

	if(playMainButt->ImageIndex == 0) {              //���� �� ������ ��������������� ����������� ����������� "�����"
		if(delayRepLabel != NULL) {
			if(delayMainEdit->Text.Length() < 4) {
				int num = StrToInt(delayMainEdit->Text);
				if(num > 0 && num < 1000) {
					replayTimer->Interval = num * 1000;
					mainOpt->repMovesDelay = num;
					mainOpt->saveConfig();
				}
				else {
					ShowMessage("Incorrect movement delay! Please enter value 0..999");
					return;
				}
			}
			else {
				ShowMessage("Incorrect movement delay! Please enter value 0..999");
				return;
			}
		}
		playMainButt->Images = repPlayerButtons;
		playMainButt->ImageIndex = 4;
		playMainButt->ImageAlignment = iaCenter;
		if(replayControlForm != NULL) {
			playRepButt->Images = repPlayerButtons;
			playRepButt->ImageIndex = 4;
			playRepButt->ImageAlignment = iaCenter;
        }
		replayTimer->Enabled = true;
	}
	else if(playMainButt->ImageIndex == 4) {        //���� �� ������ ��������������� ����������� ����������� "�����"
		replayTimer->Enabled = false;
		playMainButt->ImageIndex = 0;
		if(replayControlForm != NULL)
			playRepButt->ImageIndex = 0;
    }
}

void __fastcall TMainChessForm::nextRepButtClick(TObject* Sender) {
	 if(deskNote::currDesk <= deskNotes.size() - 1 && (repMoveNote::currMove < repMoveNote::moveCounter - 1 ||                //���� ��� ����� ���������� � �� �������� ���������
	 repMoveNote::currMove == repMoveNote::moveCounter - 1 && repMoveNotes[repMoveNote::currMove].blackPiece.first != NOPIECE && deskNotes[deskNote::currDesk - 1].lastTurn == BLACK)) {     //��� �������� ���������, �� ���� ��� (��� ���. �������) ������� �� ���� ����� � ������, � ������ ��� ������ ����� (�.�. ����� ���� ��� ����� ��� ����������)
		restoreDeskNote(deskNotes[deskNote::currDesk]);                    //��������������� ��������� ��������� �����, �����. ����, ��� ������ ���� ����� ���������� ����
		repMoveNote::currMove = (deskNote::currDesk - 1) / 2;              //�� ������ �������� ��������� ���. ����� ���������� ����� �������� ����
		currTurn = ((deskNote::currDesk - 1) % 2) ? BLACK : WHITE;
		//-------------���������� ���������� ������������� ������� � ����� ��� ������ �������� ��������� �� �����------------------------------------------------
		if(samePosLabel != NULL && uselessMovesLabel != NULL) {
			if(currTurn == WHITE) {                        //���� ��������� ��� ��� ����� (����� ���� ��� �� ������� ��������������� �������)
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove].samePosesWhite);
				if(repMoveNotes[repMoveNote::currMove].uselessMoves == 0 && repMoveNotes[repMoveNote::currMove].whiteTake == false && repMoveNotes[repMoveNote::currMove].whitePiece.first != PAWN)
					uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove].uselessMoves);   //���� �� ������� ���� ������� ����� ��� ����������� ��������� ������������,
																																	//�� ����� ���� �����, ���� �� ������� �������, ������� ��� ����� ���������
			}
			else if(currTurn == BLACK) {                   //���� ��������� ��� ��� ������ (����� ���� ��� �� ������� ��������������� �������)
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove].samePosesBlack);
				uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove].uselessMoves);
			}
        }
		//----------------------------------------------------------------------------------------------------------------------------------------------------------
		deskNote::currDesk++;                                               //�������������� ������� ��������� ��������� �����
		repMoveNote::currMove = (deskNote::currDesk - 1) / 2;              //�� ������ �������� ��������� ���. ����� ���������� ����� �������� ����
		currTurn = ((deskNote::currDesk - 1) % 2) ? BLACK : WHITE;         //�� ������ �������� �������. ���. ����� ���������� ����� ������� ������� ����� ����
		chDskPaint->paintPieces();                                          //����� �������������� ���. ����� ������������ ��

	}
	else if(deskNote::currDesk == deskNotes.size() - 1 && repMoveNote::currMove == repMoveNote::moveCounter - 1) {    //���� ����. ��� ���������� ����� � �� �������� ���������
		makeRepMove();
		if(deskNotes.size() == 1)              //���� ����� ���������� ���� ������, ���-�� ��������� ���. ����� ������� �������� 1 (�.�. �� ���� ��������� �� ������ ����),
			return;                            //�� ��� ������, ��� ������������ ����� ����� ���� ��� ������, ������������� ���������� ��������� ������ �����
		if(samePosLabel != NULL && uselessMovesLabel != NULL) {
			if(deskNotes[deskNote::currDesk - 1].lastTurn == BLACK)         //� ������ ������� �������� ��������� ����������� ��� ����� �������� ����� ����
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].samePosesBlack);

			else if(deskNotes[deskNote::currDesk - 1].lastTurn == WHITE)     //� ������ ������� �������� ��������� ����������� ��� ����� �������� ����� ����
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].samePosesWhite);
			uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].uselessMoves);
		}
	}
	else {                       //� ��������� ������: ���� ��� �� ���������� (��������� ��� ���������� ���������, ���� ���� �� ��� �������� �����, �.�. �� ������� ���� ��������� ����)
		makeRepMove();
        if(deskNotes.size() == 1)              //���� ����� ���������� ���� ������, ���-�� ��������� ���. ����� ������� �������� 1 (�.�. �� ���� ��������� �� ������ ����),
			return;                            //�� ��� ������, ��� ������������ ����� ����� ���� ��� ������, ������������� ���������� ��������� ������ �����
		if(samePosLabel != NULL && uselessMovesLabel != NULL) {
			samePosLabel->Caption = "Same positions: " + UIntToStr(sameDesksCount);
			uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(moveNote::uselessCounter);
		}
	}
	//--------------���������� ��������� ���������� ����������� ������� ���������� �������� ����� ���������------------------------
	if(repMoveNote::currMove == repMoveNote::moveCounter) {       //���� ����� �������� ���� ������ ����� ������ ���-�� �����
		if(replayControlForm != NULL) {
			nextRepButt->Enabled = false;
			playRepButt->ImageIndex = 0;
			playRepButt->Enabled = false;
		}
		if(nextMainButt != NULL) {
			nextMainButt->Enabled = false;
			playMainButt->ImageIndex = 0;
			playMainButt->Enabled = false;
		}

		return;
	}

	if(currTurn == BLACK && repMoveNote::currMove == 0) {       //���� �������� ������ ��� (��� �����)
		if(replayControlForm != NULL)
			prevRepButt->Enabled = true;                          //���������� ������ �������� �� ���������� ���
		if(prevMainButt != NULL)
			prevMainButt->Enabled = true;
	}
	else if(currTurn == WHITE)
		repMovesBox->ItemIndex = repMoveNote::currMove;               //����� ���� ������ (����� ����� ���� ������� �����) ������� ���������� ��-�� ����������� �������� ���. ����

	if(replayControlForm != NULL)
		numberRepEdit->Text = UIntToStr((deskNote::currDesk + 1) / 2);               //� ����������� ��������� ���� ��������� ����� ����, �������
																//�������������� ��� ���������� ����. ���. ����� + 1 (�.�. ���������� ����������
																//����� ������������ ����), �������� �� 2
	if(numberMainEdit != NULL)
		numberMainEdit->Text = UIntToStr((deskNote::currDesk + 1) / 2);
    //-------------------------------------------------------------------------------------------------------------------------------
	return;
}



void __fastcall TMainChessForm::prevRepButtClick(TObject* Sender) {          //�����, ����������� ��� ������� ������ ����. ����
																			//������ ���������� �������
	deskNote::currDesk--;
	if(deskNote::currDesk == 1) {
		if(replayControlForm != NULL)
			prevRepButt->Enabled = false;
		if(prevMainButt !=NULL)
			prevMainButt->Enabled = false;
	}

	restoreDeskNote(deskNotes[deskNote::currDesk - 1]);
	currTurn = ((deskNote::currDesk - 1) % 2) ? BLACK : WHITE;       //����������, ���� ����������� ����� ���� ����� ���������� �������� ��������� ���. �����
	repMoveNote::currMove = (deskNote::currDesk - 1) / 2;
	if(deskNote::currDesk   < deskNotes.size()) {            //���� ���������� ����� ������� ��������� ����� ������ ������� ������� ���. �����
		if(replayControlForm != NULL) {
			nextRepButt->Enabled = true;
			playRepButt->Enabled = true;
		}
		if(nextMainButt != NULL) {
			nextMainButt->Enabled = true;
			playMainButt->Enabled = true;
		}
	}

	if(replayControlForm != NULL)
		numberRepEdit->Text = (deskNote::currDesk + 1) / 2;
	if(numberMainEdit != NULL)
		numberMainEdit->Text = (deskNote::currDesk + 1) / 2;
	if(samePosLabel != NULL && uselessMovesLabel != NULL) {
		if(currTurn == WHITE) {
		   samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove].samePosesWhite);
		   if(repMoveNote::currMove > 0)
				uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].uselessMoves);
		   else
				uselessMovesLabel->Caption = "Reversible moves: 0";

		}
		else if(currTurn == BLACK) {
			samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove].samePosesBlack);
			if(repMoveNotes[repMoveNote::currMove].uselessMoves != 0) {         //���� ���������� ����� ��� ����������� �������� ��� ������� �������-���� ������ = 0
				if(repMoveNote::currMove > 0)             //���� ������ ��� ������ - �� ������
					uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].uselessMoves);
				else                              //���� ��� ������ - ������
					uselessMovesLabel->Caption = "Reversible moves: 0";
			}
			else
                uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove].uselessMoves);
		}

	}
	repMovesBox->ItemIndex = (deskNote::currDesk + 1) / 2 - 1;
	chDskPaint->paintPieces();
	return;

}

void __fastcall TMainChessForm::gotoRepButtClick(TObject* Sender) {
	AnsiString numberText;
	TComponent* locComponent = dynamic_cast<TComponent*>(Sender);
	if(locComponent != NULL) {
		if(locComponent->Name == "gotoRep") {
			numberText = numberRepEdit->Text;
			if(numberMainEdit != NULL)
				numberMainEdit->Text = numberText;
		}
		else if(locComponent->Name == "gotoMain") {
			numberText = numberMainEdit->Text;
			if(replayControlForm != NULL)
				numberRepEdit->Text = numberText;
		}
	}

	bool repPanelIsOpen = true;            		//���������� ��������� ����, ��������������� ��� �������� ������ ���������� �������
	if(menuReplayControl->Checked == false)             //���� ����� ������� ������ ���������� ������� �� ������� �������� (������ �������)
		repPanelIsOpen = false;                        //������� ���� �������� ������ ���������� �������
	else
		repPanelIsOpen = true;

	if(StrToInt(numberText) > repMoveNote::moveCounter) {                       //���� ������������ � ��������� ���� � ������ ���������� ������� ������ �����
																						//������������� ���-�� ����� � ������ ������
		ShowMessage(AnsiString("Invalid move number. This game has ") + UIntToStr(repMoveNote::moveCounter)  + AnsiString(" moves"));    //������� ��������� �� ������
		if(replayControlForm != NULL)
			numberRepEdit->Text = (deskNote::currDesk + 1) / 2;                             //� �������� ���� ������� ����� �������� ����
		if(numberMainEdit != NULL)
			numberMainEdit->Text = (deskNote::currDesk + 1) / 2;
		return;
	}
	else if((StrToInt(numberText) - 1) * 2 <= deskNotes.size() - 1) {               //���� �������������� ������������� ��� ������ ���-�� ����������� �����
		int deskIndex = (StrToInt(numberText) - 1) * 2;
		restoreDeskNote(deskNotes[deskIndex]);
		deskNote::currDesk = deskIndex + 1;
		repMoveNote::currMove = (deskNote::currDesk - 1) / 2 ;
		if(repMoveNote::currMove > 0) {
			samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].samePosesBlack);
			uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].uselessMoves);
		}
		else {
			samePosLabel->Caption = "Same positions: 1";
			uselessMovesLabel->Caption = "Reversible moves: 0";
		}

	}
	else if((StrToInt(numberText) - 1) * 2 > deskNotes.size() - 1) {                //���� ������������� ������������� ��� ������ ���-�� �����, ����������� �� �����,
																					//�� ��� ���� ������ ���-�� ����������� ����� (���-�� ��������, �������� �� 2)

		if(deskNote::currDesk < deskNotes.size()) {                               //������� ��������������� ��������� ����������� ����� ��� (��������� ��������� ���. �����)
			restoreDeskNote(deskNotes[deskNotes.size() - 1]);                     //�� ��������� ����� ������� ����� � ������������� (��������, ������� - 3, ������������� - 10,
			deskNote::currDesk = deskNotes.size();                                //����� ���� ��������� 5 �����, ��������������� ��������� ����� 5-�� ����, � ��� ����� ��������� ����� ����
		}                                                                         //� 6 �� 10)

		if(menuReplayControl->Checked == false)
			menuReplayControlClick(menuReplayControl);


		nextRepButt->Hide();
		prevRepButt->Hide();
		gotoRepButt->Hide();
		playRepButt->Hide();
		numberRepEdit->Hide();

		repMovesProgress = new TProgressBar(replayControlForm);
		repMovesProgLabel = new TLabel(replayControlForm);
		repMovesProgLabel->Parent = replayControlForm;
		repMovesProgLabel->Left = 20;
		repMovesProgLabel->Top = 5;
		repMovesProgLabel->Caption = "Making replay moves...";
		repMovesProgress->Parent = replayControlForm;
		repMovesProgress->Height = 35;
		repMovesProgress->Width = 275;
		repMovesProgress->Top = 25;
		repMovesProgress->Left = 10;
		repMovesProgress->Max = 100;               //���������� ��������� ��������-����
		int movesProgStep = 100 / ((StrToInt(numberText) * 2 - 1) - deskNotes.size());        //������������ ��� ��������-����

		for(int i = deskNotes.size(); i < StrToInt(numberText) * 2 - 1; i++) {   //��������� ������� �����, ������� ���������� ������� ���� ����������
			makeRepMove();                                                              //���� ������� - 1 (�.�. ������ ������������ ����� ������������ ����)
			repMovesProgress->StepBy(movesProgStep);                            //��������� �������� � ��������-���� ��������������� ����������� �����
		}
		repMovesProgress->StepBy(movesProgStep);                              //��������� ��������� ������� � ��������-����

		repMovesProgress->Hide();
		repMovesProgLabel->Hide();
		delete repMovesProgress;
		delete repMovesProgLabel;
		nextRepButt->Show();
		prevRepButt->Show();
		gotoRepButt->Show();
		playRepButt->Show();
		numberRepEdit->Show();
	}
	deskNote::currDesk = StrToInt(numberText) * 2 - 1;

	if(deskNote::currDesk == 1) {
		if(replayControlForm != NULL)
			prevRepButt->Enabled = false;
		if(prevMainButt != NULL)
			prevMainButt->Enabled = false;
	}
	else {
		if(replayControlForm != NULL)
			prevRepButt->Enabled = true;;
		if(prevMainButt != NULL)
			prevMainButt->Enabled = true;
    }

	if(repMoveNote::currMove <= repMoveNote::moveCounter) {
		if(replayControlForm != NULL) {
			nextRepButt->Enabled = true;
			playRepButt->Enabled = true;
		}
		if(nextMainButt != NULL) {
			nextMainButt->Enabled = true;
			playMainButt->Enabled = true;
		}
	}
	if(repPanelIsOpen == false && menuReplayControl->Checked == true)
		replayControlForm->Close();

	repMovesBox->ItemIndex = repMoveNote::currMove;
	chDskPaint->paintPieces();
	return;
}


//---------------------------------------------------------------------------

void __fastcall TMainChessForm::replayTimerTimer(TObject *Sender)             //������ ��������������� ��������������� ������
{
	static bool repIsOver = false;
	if(repMoveNote::currMove == repMoveNote::moveCounter - 1 && repMoveNotes[repMoveNote::currMove].blackPiece.first == NOPIECE || repIsOver == true) {
		replayTimer->Enabled = false;
		nextRepButtClick(nextMainButt);
		repIsOver = false;
		return;
	}
	else if(repMoveNote::currMove == repMoveNote::moveCounter - 1)
		repIsOver = true;

	nextRepButtClick(nextMainButt);


}
//---------------------------------------------------------------------------



