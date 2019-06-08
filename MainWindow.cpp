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
	chDskPaint = new chessDeskPaint(MainChessForm->chessGraphic);         //инициализируем объект для отрисовки доски и фигур
	possMovesPtr = NULL;
	possTakesPtr = NULL;
	currTurn = WHITE;                                      //белые ходят первыми
	blackChecked = false;
	whiteChecked = false;
	firstMoveMade = false;
	sameDesksCount = 1;
	replayTimer->Enabled = false;

	//------------Задаем первый элемент в векторе, хранящий шахматные позиции (для реализации правила 3-х повторяющихся позиций)----
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

	//-----------------------Инициализируем настройки таймера--------------------------------------------------------------
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



	//--------------------------Определяем доступность шахматной доски и состояние эл-ов управления в зависимости от настроек таймера----------------
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


	//----------------------Инициализируем общие настройки---------------------------------------------------------------------

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

    //----------------------Добавляем дополнительные визуальные компоненты для расширенного окна программы-----------------------
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

	//-----------------------Выводим начальную информацию о состоянии таймеров  в специальные текстовые поля-----------------------------------------
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



 __fastcall TMainChessForm::~TMainChessForm() {                             //служебное слово fastcall  обязательно для объектов VCL
	 if(chDskPaint != NULL)                                                 //Visual Component Library от Borland C++ Builder
		delete chDskPaint;                                                 //удаляем объект отрисовки шахматной доски
	 if(tOpt != NULL)
		delete tOpt;                                                       //удаляем объект настроек таймера
	 if(mainOpt != NULL)
		delete mainOpt;                                                    //удаляем объект общих настроек
	 if(repMovesBox != NULL) {
		 for(int i = 0; i < repMovesBox->GetCount(); i++)                //удаляем объекты лист-бокса ходов реплея
		   delete repMovesBox->Items->Objects[i];
	 }

 }

 void TMainChessForm::showEndForm(unsigned short keyIndex) {           //метод отображения результатов игры в нестандартных случаях
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
		TButton* saveReplayButt = new TButton(endGameForm);          //объявляем и динамически определяем кнопку сохранения реплея на форме результатов игры
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

	blackTimer->Enabled = false;                //если после предыдущей игры остались работающие таймеры выключаем их
	whiteTimer->Enabled = false;
	movesLog->Clear();                         //очищаем тестовое поле для отображения ходов

	if(menuReplayControl->Checked == true)
		replayControlForm->Close();



	//---------------Если до этого воспроизводился реплей, перестраиваем расширенную область главной формы в режим игры----------------------
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
	//------------Задаем первый элемент в векторе, хранящий шахматные позиции (для реализации правила 3-х повторяющихся позиций)----
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

	//-----------------Инициализируем настройки таймера--------------------------------------------------------------------------
	try {
		tOpt->readTimerOptions();

	}
	catch (timerOptions::timerReadError tre) {          //перехватываем исключение для того, чтобы определить один из трех видов
														//ошибок чтения файла настроек
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

    //-----------------------Выводим начальную информацию о состоянии таймеров  в специальные текстовые поля-----------------------------------------
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
    //-----------------Сохраняем состояние шахматной доски после хода--------------------------------------------------------
		 if(repMoveNotes.size() == 0)              //если игра не является реплеем
			saveDeskNote();
		 else if(repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {        //если игра является повтором ранее сохраненной партии (реплеем), и реплей еще не был выполнен до конца
			 if(deskNote::currDesk == deskNotes.size()) {      //если порядковый номер текущего состояния доски равен порядковому номеру последнего прочитанного состояния доски

				 saveDeskNote();                                 //сохраняем состояние шахматной доски
				 if(currTurn == WHITE && repMoveNote::currMove + 1 == repMoveNote::moveCounter && repMoveNotes[repMoveNote::moveCounter - 1].blackMoveSect.number == 0 ||
				 currTurn == BLACK && repMoveNote::currMove + 1 == repMoveNote::moveCounter && repMoveNotes[repMoveNote::moveCounter - 1].blackMoveSect.number != 0)
					repMoveNote::repIsFinished = true;
			 }
			 else
				deskNote::currDesk++;                           //в противном случае инкрементируем номер текущего состояния доски
		 }
		 else if(repMoveNote::repIsFinished == true)          //если реплей ранее был выполнен до конца
			deskNote::currDesk++;                             //только инкрементируем порядковый номер текущего состояния доски


	 //------------------------------------------------------------------------------------------------------------------------
	chessDesk* tmpChessDesk = chDskPaint->getChDsk();            // инициализируем локальный указатель
																		// на объект-доску
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = tmpChessDesk->getWhiteChess();      //а также указатели на отображение белых шахмат
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = tmpChessDesk->getBlackChess();     // и черных шахмат
    map<deskSection*, chessPiece*, sectCompare>* tmpSectionsMap = tmpChessDesk-> getMap();           //указатель на отображение шах. полей
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpBlackIt;
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpSectionsIt;
	set<deskSection*, lessSectPtr>* locSetPtr;
	set<deskSection*, lessSectPtr>::iterator locSetIt;
//--------------------Реализация взятия пешки через проход------------------------------------------------------------------
	if(currTurn == BLACK) {                                                   //если ходили черные,
		for(tmpWhiteIt = tmpWhiteChess->begin(); tmpWhiteIt != tmpWhiteChess->end(); tmpWhiteIt++)
			if((*tmpWhiteIt).first->getChessType() == PAWN)               //то для всех пешек обнуляем указатель на предыдущую позицию,
				(*tmpWhiteIt).first->setPrevPos(NULL);                    //т.к. право взятия через проход предоставляется только на 1 ход
	}
	else if(currTurn == WHITE) {                                      //аналогично в том случае, если ходили белые
			for(tmpBlackIt = tmpBlackChess->begin(); tmpBlackIt != tmpBlackChess->end(); tmpBlackIt++)
				if((*tmpBlackIt).first->getChessType() == PAWN)
					(*tmpBlackIt).first->setPrevPos(NULL);
	}
//---------------------------------------------------------------------------------------------------------------------------

//-----------------------Реализация постановки шаха-------------------------------------------------------------------------------
	map<chessPiece*, deskSection*>::iterator whiteKingIt;                   //объявляем локальный итератор, ссылающийся на пару, ключом которой является
																			//ключом которой является указатель на белого короля
	for(whiteKingIt = tmpWhiteChess->begin(); whiteKingIt != tmpWhiteChess->end(); whiteKingIt++) {
		if((*whiteKingIt).first->getChessType() == KING)                               //инициализируем итератор элементом отображения, ключ пары
			break;                                                                     //указывает на белого короля
	}

	map<chessPiece*, deskSection*>::iterator blackKingIt;
	for(blackKingIt = tmpBlackChess->begin(); blackKingIt != tmpBlackChess->end(); blackKingIt++) {  //аналогично для черного короля
		if((*blackKingIt).first->getChessType() == KING)
			break;
	}


	if(currTurn == BLACK) {                                                         //если совершен ход черными
		(*whiteKingIt).first->setCheck(chDskPaint->getChDsk());                     //для белого короля устанавливаем флаг постановки шаха,
																					//передавая указатель на текущий объект-доску в кач-ве аргумента
		whiteChecked = ((*whiteKingIt).first->getCheck()) ? true : false;          //если флаг постановки шаха белого короля имеет значение true
																				   //то устанавливаем признак постановки шаха белой стороне
		if(whiteChecked == true)
			moveNotes[moveNotes.size() - 1].blackCheck = true;                            //вносим соответствующую пометку в текстовое описание партии
	}

	else if(currTurn == WHITE) {                                                //аналогичо, если совершен ход белыми
		(*blackKingIt).first->setCheck(chDskPaint->getChDsk());                 //флаг постановки шаха черному королю
		 blackChecked = ((*blackKingIt).first->getCheck()) ? true : false;      //признак постановки шаха черной стороне
		 if(blackChecked == true)
			moveNotes[moveNotes.size() - 1].whiteCheck = true;                       //вносим соответствующую пометку в текстовое описание партии
	}

//-------------------------------------------------------------------------------------------------------------------------------------------
	//--------------------------Проверка возможности для хода с противоположной стороны после завершения текущего хода--------------------------------
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

		if(tmpBlackIt == tmpBlackChess->end()) {          //если у белых НЕТ возможности для хода
			if(repMoveNotes.size() == 0)                  //если игра не является реплеем
				menuSaveReplay->Enabled = true;          //включаем пункт меню для сохранения реплея
			else
				menuSaveReplay->Enabled = false;        //выключаем пункт меню для сохранения реплея

			menuSaveGame->Enabled = false;              //включаем пункт меню для сохранения игры

			if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
			}

			whiteTimerStop->Enabled = false;            //деактивируем кнопки вкл/выкл таймера, предложения ничьи и сдачи
			blackTimerStop->Enabled = false;
			whiteConcede->Enabled = false;
			blackConcede->Enabled = false;
			whiteDrawOffer->Enabled = false;
			blackDrawOffer->Enabled = false;

			if(blackChecked == true) {
				moveNotes[moveNotes.size() - 1].whiteCheckmate = true;              //устанавливаем флаг постановки мата белыми в свойстве объекта - записи хода
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения в последнем элементе вектора moveNotes																													//в текстовое поле для записи ходов
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
				if(repMoveNotes.size() != 0)                    //если игра является реплеем
					saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
				endGameForm->ShowModal();
				delete endGameForm;
				return;

			}
			else {

				moveNotes[moveNotes.size() - 1].whiteStalemate = true;                  //устанавливаем флаг постановки пата белыми в свойстве объекта - записи хода
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения вектора ходов moveNotes																											  //в текстовое поле записи ходов
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
                if(repMoveNotes.size() != 0)                    //если игра является реплеем
					saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
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
		if(tmpWhiteIt == tmpWhiteChess->end()) {            //если у белых нет возможности для хода
			if(repMoveNotes.size() == 0)                  //если игра не является реплеем
				menuSaveReplay->Enabled = true;          //включаем пункт меню для сохранения реплея
			else
				menuSaveReplay->Enabled = false;        //выключаем пункт меню для сохранения реплея

			menuSaveGame->Enabled = false;

			if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
				whiteTimer->Enabled = false;
				blackTimer->Enabled = false;
			}

			whiteTimerStop->Enabled = false;            //деактивируем кнопки вкл/выкл таймера, предложения ничьи и сдачи
			blackTimerStop->Enabled = false;
			whiteConcede->Enabled = false;
			blackConcede->Enabled = false;
			whiteDrawOffer->Enabled = false;
			blackDrawOffer->Enabled = false;

			if(whiteChecked == true) {                      //если при этом белые под шахом
				moveNotes[moveNotes.size() - 1].blackCheckmate = true;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();    //выводим последние изменения вектора ходов moveNotes
																															//в текстовое поле записи ходов
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
				if(repMoveNotes.size() != 0)                    //если игра является реплеем
					saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
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
				if(repMoveNotes.size() != 0)                    //если игра является реплеем
					saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
				endGameForm->ShowModal();
				delete endGameForm;
				return;
			}

		}
	}
	//-------------------------------------------------------------------------------------------------------------------------------

	//----------------Проверяем, применимо ли правило 50 ходов-------------------------------------------------------------------
	static bool prevTurnIsFinished = false;            //статический флаг, который устанавливается в случае полного воспроизведения реплея, НО уже ПОСЛЕ выполнения выражений этого блока
	if(repMoveNote::repIsFinished == false)           //в случае если реплей не выполнен (например, открыт новый), то вышеук. флаг снимается
		prevTurnIsFinished = false;
	if(deskNote::currDesk == deskNotes.size() && prevTurnIsFinished == false) {           //если номер текущего состояния доски равен размеру вектора, хранящего объекты состояния доски (равен общему кол-ву состояний),
																 //т.е. если выполняется ранее не выполненный ход для реплеев) и реплей ранее не был полностью воспроизведен
		moveNotes[moveNotes.size() - 1].checkUselessMove();      //вызываем метод проверки кол-ва ходов без необратимых изменений
		uselessMovesInGame->Caption = "Reversible moves: " + AnsiString(moveNote::uselessCounter);
	}
	prevTurnIsFinished = repMoveNote::repIsFinished;         //в случае полного воспроизведения реплея устанавливаем локальный статический флаг, но уже после выполнения основных выражений блока

	if(moveNotes[moveNotes.size() - 1].uselessCounter >= 50) {
			if(repMoveNotes.size() == 0)                          //если игра не является реплеем
				menuSaveReplay->Enabled = true;                  //активируем кнопку сохранения реплея
			else
				menuSaveReplay->Enabled = false;                //в противном случае деактивируем реплей
			menuSaveGame->Enabled = false;

			if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
			}

			whiteTimerStop->Enabled = false;            //деактивируем кнопки вкл/выкл таймера, предложения ничьи и сдачи
			blackTimerStop->Enabled = false;
			whiteConcede->Enabled = false;
			blackConcede->Enabled = false;
			whiteDrawOffer->Enabled = false;
			blackDrawOffer->Enabled = false;

			moveNotes[moveNotes.size() - 1].drawSign = true;               //в текстовом описании последнего хода фиксируем ничью
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения вектора ходов moveNotes
                                                                                                                            //в текстовое поле записи ходов
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
			if(repMoveNotes.size() != 0)                    //если игра является реплеем
					saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
			endGameForm->ShowModal();
			delete endGameForm;
			return;
	}
//-----------------------------------------------------------------------------------------------------------------------



//-------------------Проверяем, применимо ли правило 3-х повторяющихся позиций--------------------------------------------
	if(sameDesksCount == 3) {
		if(repMoveNotes.size() == 0)
			menuSaveReplay->Enabled = true;
		else
			menuSaveReplay->Enabled = false;
		menuSaveGame->Enabled = false;

        if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //деактивируем кнопки вкл/выкл таймера, предложения ничьи и сдачи
		blackTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		blackConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
		blackDrawOffer->Enabled = false;

		chDskPaint->paintPieces();
		moveNotes[moveNotes.size() - 1].drawSign = true;               //в текстовом описании последнего хода фиксируем ничью
		if(currTurn == BLACK)                                   //если игра закончилась ходом черных
			movesLog->Lines->Delete(movesLog->Lines->Count - 1);       //удаляем ненужную пустую строку в конце
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения вектора ходов																												   //в текстовое поле записи ходов
		chessGraphic->Enabled = false;
		//-------------------------Выводим окно результатов игры------------------------------------------------------------
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
        if(repMoveNotes.size() != 0)                    //если игра является реплеем
			saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
		endGameForm->ShowModal();
		//--------------------------------------------------------------------------------------------------------------------------------
		delete endGameForm;
		return;

	}

//----------Проверяем достаточно ли материала у обоих сторон-------------------------------------------------------
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

		if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //деактивируем кнопки вкл/выкл таймера, предложения ничьи и сдачи
		blackTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		blackConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
		blackDrawOffer->Enabled = false;

		chDskPaint->paintPieces();
		moveNotes[moveNotes.size() - 1].drawSign = true;               //в текстовом описании последнего хода фиксируем ничью
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();  //выводим последние изменения вектора ходов moveNotes
																													 //в текстовое поле записи ходов
		//------------------------------Выводим окно результатов игры-------------------------------------------------------------
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
        if(repMoveNotes.size() != 0)                    //если игра является реплеем
			saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
		endGameForm->ShowModal();
		//---------------------------------------------------------------------------------------------------------------------
		delete endGameForm;
		return;
    }
 //---------------------------------------------------------------------------------------------------------------------------------


	currTurn = (currTurn == WHITE) ? BLACK : WHITE;                //меняем цвет стороны, имеющей право хода, на противоположный

	if(currTurn == WHITE && tOpt->timerOn == true) {            //в зависимости от того, кому перешло право хода
		whiteTimerStop->Enabled = true;                        //активируем кнопку вкл/выкл таймера белых
		whiteConcede->Enabled = true;                          //активируем кнопку сдачи на ходе белых
		whiteDrawOffer->Enabled = true;                        //активируем кнопку предложения ничьи на ходе белых

		blackTimerStop->Enabled = false;                     //соответственно деактивируем кнопки черных
		blackConcede->Enabled = false;
		blackDrawOffer->Enabled = false;
	}
	else if(currTurn == BLACK && tOpt->timerOn == true) {        //и наоборот
		blackTimerStop->Enabled = true;
		blackConcede->Enabled = true;
		blackDrawOffer->Enabled = true;
		whiteTimerStop->Enabled = false;
		whiteConcede->Enabled = false;
		whiteDrawOffer->Enabled = false;
	}

	if(menuTimerSwitch->Checked == true) {
		if(currTurn == WHITE) {                        //в зависимости от цыета стороны, получившей право хода
			blackTimer->Enabled = false;             //останавливаем таймер черных
			if(tOpt->someMovesLimit == true)
				whiteTotalTime -= tOpt->addTimePerMove;    //если ход совершили черные и право хода перешло соответственно белым,
														//то прибавляем добавочное время к лимиту черных
			whiteTimer->Enabled = true;                //запускаем таймер белых
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
			whiteTimer->Enabled = false;           //останавливаем таймер белых
			if(tOpt->someMovesLimit == true)
				blackTotalTime -= tOpt->addTimePerMove; //если ход совершили белые и право хода перешло соответственно белым,
														//то прибавляем добавочное время к лимиту белых
			blackTimer->Enabled = true;            //запускаем таймер черных
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



void __fastcall TMainChessForm::CursorMove(TObject *Sender, TShiftState Shift,        //отрисовка спрайтов при движении мышки
		  int X, int Y)
{
	 chessDesk* tmpDsk = chDskPaint->getChDsk();          //определяем локальный объект-доску, инициализируем

	 static pair<deskSection*, chessPiece*> pR;                       //объявляем локальную пару <deskSection*, chessPiece*> для сохранения
																	  //возвращаемого значения, определяем ее единожды (как static)

	if(prevSect != NULL)
		if(X >= prevSect->pixelX && X < prevSect->pixelX + SQRSIZE      //если текущие координаты курсора находятся в пределах ранее выделенной клетки
		&& Y >= prevSect->pixelY && Y < prevSect->pixelY + SQRSIZE)
			return;                                                    //завершаем функцию отрисовки


	pR = tmpDsk->findSection(X, Y);                            //присваиваем указателю на локальную пару pR
																//значение пары, клетка которой содержит координаты X, Y

	static Graphics::TBitmap* tempBit = chDskPaint->getCursorOn();

	if(pR.first->number != 0) {          //если курсор попадает на одну из клеток
		if(pR.second->getChessType() != NOPIECE && pR.first->selected == false && pR.second->getChessColor() == currTurn) {                          //если на этой клетке есть какая-либо фигура
																										  //и эта клетка еще не ВЫБРАНА (т.е. не было клика, не путать с ВЫДЕЛЕНИЕМ)

				if(possMovesPtr != NULL || possTakesPtr != NULL) {              //если существуют доступные для хода или взятия поля последней выбранной ячейки
					possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //проверяем совпадения полей предыдущего выделения  и полей доступных для хода
																																		 //с последнего выбранного поля
					possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));         //проверяем совпадения полей предыдущего выделения  и полей доступных для взятия
					if(possMovesIt == possMovesPtr->end() && possTakesIt == possTakesPtr->end())                 //если оба совпадения отсутствуют
						delSelection();                             //удаляем предыдущее выделение																													 //с последнего выбранного поля

					possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), pR.first));       //среди доступных для взятия полей пред. фигурой
																										//ищем те, на которые указывает курсор
					if(possTakesIt == possTakesPtr->end())                                        //исли таковых нет
						chessGraphic->Canvas->Draw(pR.first->pixelX, pR.first->pixelY, tempBit);     //отрисовываем спрайт выделения поля
				}
				else {
					delSelection();
					chessGraphic->Canvas->Draw(pR.first->pixelX, pR.first->pixelY, tempBit);
				}

		}
		else if(pR.second->getChessType() == NOPIECE || pR.first->selected == true) {         //если на поле отсутствует фигура или поле уже выбрано

			if(possMovesPtr != NULL || possTakesPtr != NULL) {              //если существуют доступные для хода или взятия поля последней выбранной ячейки
					possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //проверяем совпадения полей предыдущего выделения  и полей доступных для хода
																																		 //с последнего выбранного поля
					possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));         //проверяем совпадения полей предыдущего выделения  и полей доступных для взятия
																																		 //с последнего выбранного поля
					if(possMovesIt == possMovesPtr->end() && possTakesIt == possTakesPtr->end())                   //если оба совпадения отсутствуют
						delSelection();                    //удаляем предыдущее выделение
			}
			else
				delSelection();

		}
		else if(pR.second->getChessType() != NOPIECE && pR.first->selected == false && pR.second->getChessColor() != currTurn) {  //Если курсор перемещен на поле, на котором расположена недоступная
																															  //для хода фигура
			pair<deskSection*, chessPiece*> prevPr;                      //объявляем локальную пару, соответствующую предыдущей выделенной ячейке
			if(prevSect != NULL) {                                      //если пред. выделенная ячейка существует
				prevPr = tmpDsk->findSection(prevSect->pixelX, prevSect->pixelY);        //инициализируем пару при помощи метода поиска пары по координатам ячейки
				if(prevPr.second->getChessColor() == currTurn)                           //если предыдущая выделенное поле содержало фигуру, доступную на данном ходу
					delSelection();                                                     //удаляем выделение
			}
		}
        prevSect = pR.first;              //указателю prevSect присваиваем значение указателя на
													//выделенную (спрайтом) клетку
	}
	else {                               //если курсор выходит за пределы шахматного поля
		if(possMovesPtr != NULL || possTakesPtr != NULL) {
			possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //проверяем совпадения полей предыдущего выделения  и полей доступных для хода
																																			 //с последнего выбранного поля
			possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));         //проверяем совпадения полей предыдущего выделения  и полей доступных для взятия
			if(possMovesIt == possMovesPtr->end() && possTakesIt == possTakesPtr->end())                 //если оба совпадения отсутствуют
				delSelection();                             //удаляем предыдущее выделение
		}
		else
			delSelection();
		prevSect = pR.first;
	}

}



void __fastcall TMainChessForm::SectionClick(TObject *Sender)         //обработчик события клика по клетке шах.поля
{

		setSectClick();                                 //вызываем метод, выделяющий клетку по клику



}


void TMainChessForm::delSelection() {                         //удаление выделения поля (зеленой рамки) при движении курсора
															//позволяет удалить рамку без перерисовки всей доски в целях ув. быстродействия
		//----------------Удаляем предыдущую рамку выделения клетки---------------------------------------------------------------
		if(prevSect != NULL) {                                                   //  если предыдущ. выделен. существует

				if(prevSect->sectColor == BLACK && prevSect->selected == false)                      //цвет клетки черный, и клетка не выбрана
					chessGraphic->Canvas->Draw(prevSect->pixelX, prevSect->pixelY, chDskPaint->getNoCursorBlack());     //использовать спрайт удаления для черной клетки
				else if(prevSect->sectColor == WHITE && prevSect->selected == false)                 //если белый
					chessGraphic->Canvas->Draw(prevSect->pixelX, prevSect->pixelY, chDskPaint->getNoCursorWhite());     //для белой клетки
		}
	 //---------------------------------------------------------------------------------------------------------------------------
}

//---------------------Отрисовка графики при клике по клетке щахматного поля-------------------------------------------------------
void TMainChessForm::setSectClick() {
	chessDesk* tmpDsk = chDskPaint->getChDsk();                 //создаем локальный объект-доску
	pair<deskSection*, chessPiece*> pR;                            //создаем  локальную пару pR, для


 //----------------------Удаление предыдущего выделения без перерисовки всей доски------------------------------------------------------------------------
	if(prevChosenSect != NULL && prevChosenSect->selected == true) {                               //если существует последнее выбранное поле и поле, на котором расположен указатель, еще не выбрано
			pR = tmpDsk->findSection(prevChosenSect->pixelX, prevChosenSect->pixelY);      //присваиваем указателю pR на локальную пару
																						//значение указателя на пару из отображения sectionsMap
																					   //ключ которой (ук. на объект deskSection)
																						//соответствует ранее выбранной ячейке
			Graphics::TBitmap* localBmp = new Graphics::TBitmap;               //объявляем локальный указатель на спрайт, выделяем под него память

			if(pR.first->sectColor == WHITE)                                 //в зависимости от цвета ячейки, с которого удаляется выделение
				localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/" + pR.second->getPicName().c_str());   //загружаем изображение для фигуры на белоя ячейке
			else if(pR.first->sectColor == BLACK)
				localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/" + pR.second->getPicName().c_str());   //загружаем изображение для фигуры на черной ячейке
			chessGraphic->Canvas->Draw(pR.first->pixelX, pR.first->pixelY, localBmp);



			if(prevSect != prevChosenSect)                          //если текущая клетка не совпадает с ранее выбранной
				prevChosenSect->selected = false;                   //снимаем флаг выбора клетки
			delete localBmp;
	}

 //------------------------------------------------------------------------------------------------------------------------
	pR = tmpDsk->findSection(prevSect->pixelX, prevSect->pixelY);           //присваиваем паре <deskSection*, chessPiece*> pR
																		//значение, соответствующее текущему выделенному полю
																	   // (на котором расположен курсор)

   if(pR.second->getChessType() == NOPIECE) {                      //если текущее выделенное поле пустое

		setSectMoves(pR);                                     //вызываем метод отрисовки доступных для хода полей,
															//который в данном случае только удалит спрайты с полей для
															//предыдущей выбранной фигуры

		if(prevChosenSect != NULL && possMovesPtr != NULL) {                //если предыдущая выбранная фигура существует и существуют доступные для хода поля
			possMovesIt = find_if(possMovesPtr->begin(), possMovesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));       //в множестве possMovesSet
																															//ищем элемент равный указателю на выделенное
																															//поле prevSect
																															//для этого используем алгоритм find_f,
																															//предопределенный функтор equal_to<>(T, T)
																															//с помощью предопр. в stl функции  bind2nd
																															//связываем указатель prevSect со вторым аргументом функтора

	   //таким образом, мы проверили, является ли поле, по которому осуществлено нажатие, возможным для хода предыдущий выбранной фигуры
			if(possMovesIt != possMovesPtr->end()) {                  //если является
				movePiece(prevSect);                               //осуществляем ход
				prevChosenSect = prevSect;                      //последнему выбранному полю присваиваем значение поля на которое указывает в данный момент курсор
				return;                                         //заершаем метод, т.к. еесли мы выполнили ход, то отрисовывать
			}                                                   //доступные для хода поля уже не нужно

	   }



   }


   else {                                                      //если текущее выделенное поле непустое
		if(prevChosenSect == prevSect) {           //если предыдущее выбранное поле совпадает с текущим (на котором расп. курсор),
			setSectMoves(pR);                    //вызываем метод отрисовки доступных для  хода полей, который в данном случае только удалит предыдущие
			if(prevChosenSect->selected == true) {
				prevChosenSect->selected = false;
				return;
			}


		}
		if(prevChosenSect != NULL && possTakesPtr != NULL) {             // если предыдущая выбранная фигура существует и существуют доступные для взятия поля
			possTakesIt = find_if(possTakesPtr->begin(), possTakesPtr->end(), bind2nd(equal_to<deskSection*>(), prevSect));
			if(possTakesIt != possTakesPtr->end()) {
					takePiece(prevSect);
					prevChosenSect = prevSect;
					chDskPaint->paintPieces();                                           //отрисовываем фигуры на новых позициях
					return;
			}
		}
		if(pR.second->getChessColor() != currTurn) {         //если цвет фигуры, по которой был осуществлен клик, недоступен на данном ходу
				setSectMoves(pR);                                  //вызываем метод отрисовки доступных для хода полей,
				return;											//который в данном случае только удалит спрайты с полей для
		}	   														//предыдущей выбранной фигуры
   }


//----------------------------------Отрисовка нового спрайта выделения поля-------------------------------------------------------
	if(prevSect->selected == false && pR.second->getChessColor() == currTurn) {          //если клетка, по которой осуществлен клик, еще не выбрана
		map<deskSection*, chessPiece*, sectCompare>* mapPtr = tmpDsk->getMap();

		if((*mapPtr)[prevSect]->getChessType() != NOPIECE) {             //если на поле, по которому осуществлен клик, есть какая-либо фигура
				delSelection();                      //удаляем предыдущее выделение клетки (рамку)

				BLENDFUNCTION bf;                    //объявляем объект типа BLENDFUNCTION для реализации полупрозрачного спрайта
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 120;
				bf.AlphaFormat = 0;
				static Graphics::TBitmap* localBmp = chDskPaint->getSelectedSect();       //единожды инициализируем локальный спрайт
																							//для удобства вызова методов
				::AlphaBlend(chessGraphic->Canvas->Handle, prevSect->pixelX, prevSect->pixelY,
							SQRSIZE,
							SQRSIZE,
							localBmp->Canvas->Handle, 0, 0,
							localBmp->Width,
							localBmp->Height, bf);


				setSectMoves(pR);                      //вызываем метод отрисовки доступных для хода полей
				setSectTakes(pR);                       //вызываем метод отрисовки доступных для взятия полей

				if(prevSect != prevChosenSect)
					prevChosenSect = prevSect;
				prevSect->selected = true;

		}

	}



}
//--------------------------------------------------------------------------------------------------------------------------------------------
void TMainChessForm::setSectMoves(pair<deskSection*, chessPiece*> pR) {
		chessDesk* tmpDsk = chDskPaint->getChDsk();     //определяем  указатель на локальный объект-доску
		map<deskSection*, chessPiece*, sectCompare>* tmpMap = tmpDsk->getMap();     //определяем указатель на отображение,
																			 //хранящее пары deskSection*, chessPiece*
																		   //инициализируем его методом локального объекта-доски
		Graphics::TBitmap* localBmp = new Graphics::TBitmap;

//--------------------Удаляем заливку с предыдущих доступных для хода полей-----------------------------------------------------
		if(possMovesPtr != NULL && !possMovesPtr->empty()) {
				for(possMovesIt = possMovesPtr->begin(); possMovesIt != possMovesPtr->end(); possMovesIt++) {
					if((*possMovesIt)->sectColor == BLACK)
						chessGraphic->Canvas->Draw((*possMovesIt)->pixelX, (*possMovesIt)->pixelY, chDskPaint->getFreeBlack());
					else if((*possMovesIt)->sectColor == WHITE)
						chessGraphic->Canvas->Draw((*possMovesIt)->pixelX, (*possMovesIt)->pixelY, chDskPaint->getFreeWhite());
				}
				for(possMovesIt = possMovesPtr->begin(); possMovesIt != possMovesPtr->end(); possMovesIt++) {       //Если среди доступных для хода полей
						if((*possMovesIt) == prevSect)                                                              //нет поля, на которое в данный момент указывает курсор (начало хода фигурой)
							break;
				}
				if(possMovesIt == possMovesPtr->end())
					possMovesPtr->clear();                                                                         //, то очищаем множество доступных для хода полей (т.к. клик будет по пустому полю,
																												   //недоступному для  хода)


		}
//----------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------Удаляем заливку с предыдущих доступных для взятия полей---------------------------------------------------
	if(possTakesPtr != NULL && !possTakesPtr->empty()) {
			for(possTakesIt = possTakesPtr->begin(); possTakesIt != possTakesPtr->end(); possTakesIt++) {
				if((*possTakesIt)->sectColor == WHITE)
						chessGraphic->Canvas->Draw((*possTakesIt)->pixelX, (*possTakesIt)->pixelY, chDskPaint->getFreeWhite());
				else if((*possTakesIt)->sectColor == BLACK)
						chessGraphic->Canvas->Draw((*possTakesIt)->pixelX, (*possTakesIt)->pixelY, chDskPaint->getFreeBlack());

			}

	}
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------Отрисовываем спрайты фигур на предыдущих доступных для взятия полях----------------------------
	if(possTakesPtr != NULL && !possTakesPtr->empty()) {
			localBmp->Transparent = true;
			for(possTakesIt = possTakesPtr->begin(); possTakesIt != possTakesPtr->end(); possTakesIt++) {       //перебираем предыдущие доступные для взятия поля

				if((*possTakesIt)->sectColor == WHITE)                //если цвет доступного для взятия поля - белый
					localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/" + (*tmpMap)[(*possTakesIt)]->getPicName().c_str());   //загружаем изображение фигуры для белой ячейки
				else if((*possTakesIt)->sectColor == BLACK)
					localBmp->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/" + (*tmpMap)[(*possTakesIt)]->getPicName().c_str());   //загружаем изображение фигуры для черной ячейки
				chessGraphic->Canvas->Draw((*possTakesIt)->pixelX, (*possTakesIt)->pixelY, localBmp);                  //отрисовываем спрайт в объекте графического изображения chessGraphic
			}
			possTakesPtr->clear();

	}
//-------------------------------------------------------------------------------------------------------------------------------


//-----------------------------Отрисовываем спрайты выделения доступных для хода полей для текущего поля---------------------------------------

		if(pR.first->number != 0 && pR.second->getChessType() != NOPIECE
		&& pR.first->selected == false && pR.second->getChessColor() == currTurn) {           //если поле полученное через аргумент существует, и оно непустое, и не выбрано,
			if(possMovesPtr != NULL)                                                          //и на нем расположена доступная на текущем ходу  фигура
				possMovesPtr->clear();                                     //очищаем множество доступных для хода полей пред. выделенной клетки
			pR.second->setMovesTakes(pR.first, tmpMap);                 //вносим данные в множества доступных для хода и доступных для взятия полей
			possMovesPtr = pR.second->getPossMoves();                   //получаем для данной фигуры множество доступных для хода полей
		}
		else															  //в противном случае завершаем метод
			return;

		if(!possMovesPtr->empty()) {
				BLENDFUNCTION bf;                    //объявляем объект типа BLENDFUNCTION для реализации полупрозрачного спрайта
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 100;
				bf.AlphaFormat = 0;
				localBmp = chDskPaint->getMovesSect();       // инициализируем локальный спрайт
																							//для удобства вызова методов

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



//--------------------------------Отрисовываем спрайты выделения доступных для взятия полей для текущего поля--------------------------------------------------------------

	chessDesk* tmpDsk = chDskPaint->getChDsk();                       //инициализируем указатель на шахматную доску
	map<deskSection*, chessPiece*, sectCompare>* tmpMap = tmpDsk->getMap();      //инициализируем указатель на отображение sectionsMap класса chessDesk

	if(pR.first->number != 0 && pR.second->getChessType() != NOPIECE                         //если поле существует, оно непустое,
		&& pR.first->selected == false && pR.second->getChessColor() == currTurn) {         //оно уже не выделено, и нем расположена фигура доступная на текущем ходу
		if(possTakesPtr != NULL)
			possTakesPtr->clear();                               //очищаем множество доступных для взятия полей пред. выбранной клетки
		pR.second->setMovesTakes(pR.first, tmpMap);                 //вносим данные в множества доступных для хода и доступных для взятия полей
		possTakesPtr = pR.second->getPossTakes();             //получаем для данных поля и фигуры множество доступных для взятия полей
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

	chessDesk* tmpDesk = chDskPaint->getChDsk();                  //объявляем и  инициализируем указатель на объект-доску
	chessDesk deskCopy = *(chDskPaint->getChDsk());                 //копируем объект доску до совершения каких бы то ни было манипуляций (необъ. для реализации шаха)
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess  = chDskPaint->getChDsk()->getWhiteChess();
	map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = chDskPaint->getChDsk()->getBlackChess();
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpBlackIt;
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;
	map<deskSection*, chessPiece*, sectCompare>* tmpSectionsMap = chDskPaint->getChDsk()->getMap();
	map<deskSection*, chessPiece*, sectCompare>::iterator tmpSectionsIt;
	chessPiece* tmpPiecePtr =(*tmpSectionsMap)[prevChosenSect];											//объявляем временный указатель на объект фигуры, расположенной
																										//последнем выбранном поле

//-----------Добавляем запись о ходе в вектор записей ходов-------------------------------------------------------------------------
	if(tmpPiecePtr->getChessColor() == BLACK) {
		if(repMoveNotes.size() == 0 || repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {       //если игра не является реплеем или является, но реплей еще ни разу полностью не воспроизведен
			moveNotes[moveNotes.size() - 1].blackPiece.first = tmpPiecePtr->getChessType();
			moveNotes[moveNotes.size() - 1].blackPiece.second = *prevChosenSect;
			moveNotes[moveNotes.size() - 1].blackMoveSect = *dsPtr;
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();
			movesLog->Lines->Add("");
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] += "";
		}

	}
	else if(tmpPiecePtr->getChessColor() == WHITE) {
		if(moveNotes.size() == 0) {                                          //если текущий ход - первый
			firstMoveMade = true;
			menuSaveReplay->Enabled = false;
			if(whiteTimer->Enabled == false && repMoveNotes.size() == 0)          //если таймер белых не включен, и игра не является реплеем
				menuSaveGame->Enabled = true;
			menuTimerSwitch->Enabled = false;
		}
		if(repMoveNotes.size() == 0 || repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {          //если игра не является реплеем или является, но реплей еще ни разу полностью не воспроизведен
			moveNote newNote;
			newNote.whitePiece.first = tmpPiecePtr->getChessType();
			newNote.whitePiece.second = *prevChosenSect;
			newNote.whiteMoveSect = *dsPtr;
			moveNotes.push_back(newNote);
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (newNote.getTextMove()).c_str();
		}

	}
//------------------------------------------------------------------------------------------------------------------------------------------------

	if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == BLACK) {                     //обращаемся к эл-ту ассоциативного массива,
		tmpBlackIt  = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(tmpPiecePtr));  //заданному отображением tmpSectionsMap
																						 //если фигура на последнем выбранном поле черная

		if(tmpBlackIt != tmpBlackChess->end())                                          //присваиваем эл-ту ассоциативного массива tmpBlackChess
																				//значение указателя на поле, на которое совершается ход
			(*tmpBlackChess)[tmpPiecePtr] = dsPtr;
	}


	else if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == WHITE) {      //аналогично для объекта белой шахматной фигуры
		tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));      //если среди белых фигур, есть фигура, расположенная
		 if(tmpWhiteIt != tmpWhiteChess->end())                                                               //на последнем выбранном поле

			(*tmpWhiteChess)[tmpPiecePtr] = dsPtr;                           //установить новую соответствующее ей поле в отображении

	}


	(*tmpSectionsMap)[prevChosenSect] = tmpDesk->getNopiece();                       //присваиваем пустую фигуру последнему выбранному полю
	(*tmpSectionsMap)[dsPtr] = tmpPiecePtr;                                  //присваиваем полю, на которое сделан ход, указатель на фигуру, ранее
																			//находившуюся на последнем выбранном поле

	//--------------------Реализация взятия вражеской пешки через проход--------------------------------------------------------------
	 if(tmpPiecePtr->getChessType() == PAWN) {                             //если фигура на последнем выбранном поле была пешкой,
		 tmpPiecePtr->setPrevPos(prevChosenSect);                      //то сохраняем для нее указатель на предыдущую позицию,
																		//который равен указателю на последнее выбранное поле
		if(prevChosenSect->symbol != dsPtr->symbol) {                     //если доступная для хода пешка находится не на одной вертикальной линии с
																		//полем, на которое осуществляется ход
			if(tmpPiecePtr->getChessColor() == BLACK) {                   //для черной пешки
				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), pieceCoordEqual(dsPtr->symbol, dsPtr->number + 1));       //в отображении белых фигур находим пешку,
																																			 //которая берется на проходе

				tmpWhiteChess->erase(tmpWhiteIt);                            //удаляем ее из отображения
				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual(dsPtr->symbol, dsPtr->number + 1));     //в отображении полей шахматного поля,
																																				//находим пару, ключ которой равен указателю на поле,
																																				//с которого была удалена фигура
				tmpSectionsIt->second = tmpDesk->getNopiece();                                                                              //значению данного эл-та отображения (второй часть пары)
																																			//присваиваем указатель на пустую фигуру
			   //----------------Корректировка записи хода------------------------------------------------------------
			   moveNotes[moveNotes.size() - 1].blackEp = true;
			   moveNotes[moveNotes.size() - 1].blackTake = true;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();      //вывод изменений в текстовое поле записи ходов
			   //------------------------------------------------------------------------------------------------------
			}
			else if(tmpPiecePtr->getChessColor() == WHITE) {           //аналогично для белой
					tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), pieceCoordEqual(dsPtr->symbol, dsPtr->number - 1));
					tmpBlackChess->erase(tmpBlackIt);
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual(dsPtr->symbol, dsPtr->number - 1));   //которая берется на проходе
					tmpSectionsIt->second = tmpDesk->getNopiece();
					//-----------------------------Корректировка записи хода-----------------------------------------
					moveNotes[moveNotes.size()- 1].whiteEp = true;
					moveNotes[moveNotes.size() - 1].whiteTake = true;
					movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
					//-----------------------------------------------------------------------------------------------
			}
		}

	 }

	//--------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------Реализация рокировки (перемещение ладьи, т.к. перем. короля реализовано в общем блоке (см. выше))------------------------------------------------------------------------------------------------

	if(tmpPiecePtr->getChessType() == KING && tmpPiecePtr->getChessColor() == WHITE                         //для белого короля на исходной позиции
		&& prevChosenSect->symbol == 'e' && prevChosenSect->number == 1) {
		   //---------------------------------Для королевского фланга-----------------------------------
			if(dsPtr->symbol == 'g' && dsPtr->number == 1) {                                            //если поле, на которое совершается ход, соотствует полю для рокировки
																										  //по королевскому флангу


				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), pieceCoordEqual('h', 1));         //получаем указатель на пару pair<chessPiece*, deskSection*>,
																													 //ключ которой равен указателю на ладью h1

				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 1));    //получаем указатель на пару pair<deskSection*, chessPiece*>,
																													//ключ которой равен полю, на которое перемещается ладья после рокировки
				(*tmpWhiteChess)[(*tmpWhiteIt).first] = (*tmpSectionsIt).first;                                    //эл-ту ассоц. массива, соотв. перемещаемой ладье присваиваем новое знач.
																												//(указатель на поле, занимаемое после перемещения)
				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('h', 1));    //эл-ту ассоц. массива, соответ. полю, с которого перем. ладья
				(*tmpSectionsIt).second = tmpDesk->getNopiece();                                                     //присваиваем знач. - указатель на пустую фигуру
				tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 1));   //эл-ту ассоц. массива, соотв. полю, на которое перем. ладья
				(*tmpSectionsMap)[(*tmpSectionsIt).first] =  (*tmpWhiteIt).first;                                     //присв. знач. - указатель на ладью.

				moveNotes[moveNotes.size() - 1].whiteShortCas = true;                                   //устанавливаем флаг короткой рокировки в записи хода
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
			}
			//--------------------------------------------------------------------------------------------

			//-----------------------------------Для ферзевого фланга--------------------------------------------------
			else if(dsPtr->symbol == 'c' && dsPtr->number == 1) {
					tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), pieceCoordEqual('a', 1));
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 1));
					(*tmpWhiteChess)[(*tmpWhiteIt).first] = (*tmpSectionsIt).first;
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('a', 1));
					(*tmpSectionsIt).second = tmpDesk->getNopiece();
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 1));
					(*tmpSectionsIt).second = (*tmpWhiteIt).first;

					moveNotes[moveNotes.size() - 1].whiteLongCas = true;                             // устанавливаем флаг длинной рокировки в записи хода
					movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
			}
		   //---------------------------------------------------------------------------------------------------------
	}

	else if(tmpPiecePtr->getChessType() == KING && tmpPiecePtr->getChessColor() == BLACK
			&& prevChosenSect->symbol == 'e' && prevChosenSect->number == 8) {
			//---------------------------------Для королевского фланга---------------------------------------------------------
			if(dsPtr->symbol == 'g' &&  dsPtr->number == 8) {

					tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), pieceCoordEqual('h', 8));
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 8));
					(*tmpBlackChess)[(*tmpBlackIt).first] = (*tmpSectionsIt).first;
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('h', 8));
					(*tmpSectionsIt).second = tmpDesk->getNopiece();
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('f', 8));
					(*tmpSectionsMap)[(*tmpSectionsIt).first] = (*tmpBlackIt).first;

					moveNotes[moveNotes.size() - 1].blackShortCas = true;                          //устанавливаем флаг короткой рокировки в записи хода

			}
			//-------------------------------------------------------------------------------------------------------------------
			//------------------------------------Для  ферзевого фланга-----------------------------------------------------------
			else if(dsPtr->symbol == 'c' && dsPtr->number == 8) {
					tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), pieceCoordEqual('a', 8));
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 8));
					(*tmpBlackChess)[(*tmpBlackIt).first] =(*tmpSectionsIt).first;
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('a', 8));
					(*tmpSectionsIt).second = tmpDesk->getNopiece();
					tmpSectionsIt = find_if(tmpSectionsMap->begin(), tmpSectionsMap->end(), sectCoordEqual('d', 8));
					(*tmpSectionsIt).second = (*tmpBlackIt).first;
					moveNotes[moveNotes.size() - 1].blackLongCas = true;                        //устанавливаем флаг длинной рокировки в записи хода
					movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
			}
			//-------------------------------------------------------------------------------------------------------------------

	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------

	//-----------------------------Реализация превращения пешки-------------------------------------------------------------------------------------
	if(tmpPiecePtr->getChessType() == PAWN) {                  //если выбранная фигура - решка
		chessPiece* pPtr;                                     //объявляем локальный указатель на объект-шах. фигуру
		if(tmpPiecePtr->getChessColor() == WHITE) {           //если выбранная фигура белая
			if(dsPtr->number == 8) {                          //если поле, на которое осуществаляется ход расположено на 8-ой горизонтали
				if(repMoveNote::moveCounter == 0)           	//если игра не является реплеем
					pawnTransform();                              //вызываем метод, отрисовывающий окно выбора фигуры
				else if(repMoveNote::moveCounter > 0)           //если запущен реплей
					choosePiece = repMoveNotes[repMoveNote::currMove].whiteTransPiece;     //переменной, хранящей тип выбранной фигуры, присваивам уже известное значение
                                                                                            //без открытия окна выбора
				//----------------Корректировка записи хода-------------------------------------------------------------
				moveNotes[moveNotes.size() - 1].whiteTrans = true;                            //устанавливаем флаг превращения белой пешки
				moveNotes[moveNotes.size() - 1].whiteTransPiece = choosePiece;                //сохраняем тип фигуры, полученной после превращения
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
				//--------------------------------------------------------------------------------------------------------
				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));      //находим выбранную пешку в отображении белых фигур
				tmpWhiteChess->erase(tmpWhiteIt);             //вырезаем элемент, ключом которого является выбранная пешка, из отображения
				int i = 1;                                    //т.к белая пешка по достижению 8-ой горизонтали трансформируется
				while(true) {                                 //в цикле
					   pPtr = chDskPaint->getChDsk()->pieceFromArr(WHITE, choosePiece, i);     //в массиве фигур ищем фигуру, соответствующую выбранной через форму
																							   //при этом в цикле инкрементируем номер фигуры
					   if(pPtr->getChessType() != NOPIECE) {                      //если фигура с таким номером имеется в массиве
						   tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(pPtr));      //ищем ее среди уже имеющихся в отображении белых фигур
						   if(tmpWhiteIt == tmpWhiteChess->end()) {                     //если в отображении белых фигур ее нет,
								tmpWhiteChess->insert(make_pair(pPtr, dsPtr));         //вставляем ее в отображение
								(*tmpSectionsMap)[dsPtr] = pPtr;                      //в отображении шахматных полей, полю на которое осуществлен ход присваиваем
								break;                                                //выбранную фигуру
						   }
					   }
					   else if(pPtr->getChessType() == NOPIECE)                    //если фигуры с заданным номером нет в соотв. массиве
							break;                                                 //прерываем цикл
						i++;                                                       //инкрементируем номер фигуры
				}
				if(tmpWhiteIt != tmpWhiteChess->end()) {                          //если фигура из массива уже присутствует в отображении белых фигур
					chessPiece* piecePtr;                                        //объявляем локальный указатель на фигуру

					switch(choosePiece) {                                    //в зависимости от типа выбранной фигуры
					case KNIGHT: {                                          //если это конь
						while(true) {                                      //в цикле
							if(!chDskPaint->getChDsk()->addNumberExist(i, WHITE)) {    //проверяем есть ли фигура с заданным номером в соотв. списке указ. на доп. фигуры
								piecePtr = new Knight(WHITE, i);                //если такой фигуры в списке нет, то выделяем под нее память, инициализируем
								chDskPaint->getChDsk()->addPiece(piecePtr);     //c помощью специального метода добавляем указатель на новую фигуру в
																				//список указателей на доп. фигуры
								break;
							}
							i++;
						}
						break;
					}
					case BISHOP: {                                           //аналогично для остальных фигур
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
					tmpWhiteChess->insert(make_pair(piecePtr, dsPtr));               //фигуру, добавленную в список доп. фигур, также добавляем и в отображение
																					//белых фигур
					(*tmpSectionsMap)[dsPtr] = piecePtr;                            //полю, на которое был осуществлен ход. присваиваем добавленную ранее фигуру
				}


			}
		}

		if(tmpPiecePtr->getChessColor() == BLACK) {                             //аналогично для черной пешки
			if(dsPtr->number == 1) {
				if(repMoveNote::moveCounter == 0)                          //если игра не является реплеем
					pawnTransform();                                       //вызываем метод отрисовки окна выбора фигуры, в которую трансформируется пешка
				else if(repMoveNote::moveCounter > 0)                      //если запущен реплей
					choosePiece = repMoveNotes[repMoveNote::currMove].blackTransPiece;       //переменной, хранящий тип фигуры, в кот. превращается пешка, присваиваем уже известное значение
																							 //без открытия окна выбора
				//---------------------Корректировка записи хода-----------------------------------------------------
				moveNotes[moveNotes.size() - 1].blackTrans = true;                  //устанавливаем флаг превращения черной пешки
				moveNotes[moveNotes.size() - 1].blackTransPiece = choosePiece;      //сохраняем тип фигуры, полученной после превращения
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
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
	if(tmpPiecePtr->getChessType() == ROOK || tmpPiecePtr->getChessType() == KING)                 //если фигура, которой совершается ход, является ладьей или королем,
		tmpPiecePtr->setIsMoved(true);                                                             //устанавливаем флаг совершения хода в положение true;


	dsPtr->selected = false;                                    //после выполнения хода снимаем флаг выделения с ячейки, на которую был сделан ход
	makeMove();                                                 //вызываем метод завершения хода (метод класса TMainChessForm)
	if(possMovesPtr != NULL)
		possMovesPtr->clear();                                   //очищаем множество доступных ходов
	if(possTakesPtr != NULL)
		possTakesPtr->clear();								//очищаем множество доступных для взятия полей
	chDskPaint->paintPieces();                                           //отрисовываем новые позиции фигур


}

void TMainChessForm::takePiece(deskSection* dsPtr) {

		chessDesk* tmpDesk = chDskPaint->getChDsk();
		chessDesk deskCopy = *tmpDesk;
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = chDskPaint->getChDsk()->getWhiteChess();
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = chDskPaint->getChDsk()->getBlackChess();
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpBlackIt;
		map<deskSection*, chessPiece*, sectCompare>* tmpSectionsMap = chDskPaint->getChDsk()->getMap();
		chessPiece* tmpPiecePtr = (*tmpSectionsMap)[prevChosenSect];                      //временный указатель на фигуру, расположенную на предыдущем выбранном поле

 //-----------Добавляем запись о ходе в вектор записей ходов-------------------------------------------------------------------------
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
		if(repMoveNotes.size() == 0 || repMoveNotes.size() > 0 && repMoveNote::repIsFinished == false) {      //если игра не является реплеем или является, но реплей еще ни разу не был полностью воспроизведен
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


		if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == BLACK) {                 //если фигура на последнем выбранном поле черная

			(*tmpBlackChess)[tmpPiecePtr] = dsPtr;                                     //в отображении черных фигур перемещаем ее на поле, на котором осуществ.
																						//взятие противоположной  фигуры
			 tmpPiecePtr = (*tmpSectionsMap)[prevSect];                                //временный указатель на фигуру, котрая удаляется при ее взятии
			 tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));
			 if(tmpWhiteIt != tmpWhiteChess->end())                                                           //если таковая есть во множестве белых фигур
				tmpWhiteChess->erase(tmpWhiteIt);            //из отображения белых фигур удаляем пару, ключ который, соответствует
																						//полю, на котором было взятие

		}
		else if((*tmpSectionsMap)[prevChosenSect]->getChessColor() == WHITE) {           //аналогично, если фигура на последнем  выбранном поле белая
			(*tmpWhiteChess)[tmpPiecePtr] = dsPtr;
			tmpPiecePtr = (*tmpSectionsMap)[prevSect];
			tmpBlackIt = find_if(tmpBlackChess->begin(), tmpBlackChess->end(), piecePairEqual(tmpPiecePtr));
			if(tmpBlackIt != tmpBlackChess->end())
				tmpBlackChess->erase(tmpBlackIt);
		}
		 tmpPiecePtr = (*tmpSectionsMap)[prevChosenSect];                         //временный указатель снова на предыдущую выбранную фигуру
		(*tmpSectionsMap)[prevChosenSect] = tmpDesk->getNopiece();
		(*tmpSectionsMap)[dsPtr] = tmpPiecePtr;

//-----------------------------Реализация превращения пешки-------------------------------------------------------------------------------------
	if(tmpPiecePtr->getChessType() == PAWN) {                  //блок полностью повторяет соответствующий в методе, выполняющем ход фигурой
		chessPiece* pPtr;
		if(tmpPiecePtr->getChessColor() == WHITE) {
			if(dsPtr->number == 8) {
				if(repMoveNote::moveCounter == 0)             //если игра не является реплеем
					pawnTransform();                          //вызываем метод выбора фигуры, в которую превращается пешка
				else if(repMoveNote::moveCounter > 0)          //если игра является реплеем
					choosePiece = repMoveNotes[repMoveNote::currMove].whiteTransPiece;        //в качестве выбранной фигуры превращения будет выступать фигура, тип которой записан в объекте-ходе реплея
				//------------------------Корректировка записи хода-------------------------------------------------------------------------
				moveNotes[moveNotes.size() - 1].whiteTrans = true;
				moveNotes[moveNotes.size() - 1].whiteTransPiece = choosePiece;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
				//---------------------------------------------------------------------------------------------------------------------------
				tmpWhiteIt = find_if(tmpWhiteChess->begin(), tmpWhiteChess->end(), piecePairEqual(tmpPiecePtr));
				tmpWhiteChess->erase(tmpWhiteIt);
				int i = 1;
				//---------------Проверяем, есть ли в массиве объектов шахматных фигур свободные фигуры (не связанные с указателями в отображениях шах. досок)
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
				//----------------------------------Корректировка записи хода------------------------------------------------------
				moveNotes[moveNotes.size() - 1].blackTrans = true;
				moveNotes[moveNotes.size() - 1].blackTransPiece = choosePiece;
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
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

		chDskPaint->getChDsk()->clearAddPiece();                      //очищаем память, занимаемую дополнительной фигурой (после
																	// превр. пешки), если она была взята

		if(tmpPiecePtr->getChessType() == ROOK || tmpPiecePtr->getChessType() == KING)                 //если фигура, которой совершается ход, является ладьей или королем,
			tmpPiecePtr->setIsMoved(true);                                                             //устанавливаем флаг совершения хода в положение true;

		dsPtr->selected = false;
		makeMove();
		if(possMovesPtr != NULL)
			possMovesPtr->clear();                  //т.к. ни одна фигура не выбрана
		if(possTakesPtr != NULL)
			possTakesPtr->clear();
		chDskPaint->paintPieces();


}

void TMainChessForm::pawnTransform() {                        //реализация метода превращения пешки в другую фигуру

	pieceChooseForm = new TForm(this);                //объявляем и инициализируем указатель на форму для выбора фигуры
															//владельцем формы является главная форма TMainChessForm, указатель на
															//нее передается в качестве параметра конструктора
	pieceChooseForm->Position = poScreenCenter;

	TBorderIcons tempBI = pieceChooseForm->BorderIcons;       //деактивируем иконки границ формы: закрыть, свернуть и т.п.
	tempBI >> biSystemMenu;
	pieceChooseForm->BorderIcons = tempBI;

	pieceChooseImage = new TImage(pieceChooseForm);    //объявляем и инициализируем указатель на компонент для вывода изображений
															   //указатель на объекта-владелеца передаем в качестве аргумента конструтора

	pieceChooseImage->Parent = pieceChooseForm;            //для отображения динамически создаваемых компонентов необходимо указывать
															//родительский компонент
	Graphics::TBitmap *fieldBit = new Graphics::TBitmap;    //объявляем и определяем спрайт фона (шахматные поля)

	Graphics::TBitmap *pieceBit = new Graphics::TBitmap;    //объявляем и определяем спрайт фигуры
	fieldBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "choose_piece.bmp");             //загружаем изображение для спрайта фона
	pieceChooseImage->Left = 12;                           //задаем параметры компонента TImage
	pieceChooseImage->Top = 12;
	pieceChooseImage->Enabled = true;
	pieceChooseImage->Visible = true;
	pieceChooseImage->Width = 400;
	pieceChooseImage->Height = 100;



	pieceChooseForm->Caption = "Choose new piece";
	pieceChooseForm->Width = 450;
	pieceChooseForm->Height = 150;
	pieceChooseImage->Canvas->Draw(0, 0, fieldBit);
	if(currTurn == WHITE) {                                   //если преобразуемая пешка белая
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Knight_wt.bmp");        //загружаем соответ. изображения белых фигур для спрайта фигуры
		pieceChooseImage->Canvas->Draw(0, 0, pieceBit);         //отрисовываем спрайты белых фигур в нужных местах
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Bishop_wt.bmp");
		pieceChooseImage->Canvas->Draw(100, 0, pieceBit);
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Rook_wt.bmp");
		pieceChooseImage->Canvas->Draw(200, 0, pieceBit);
		pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Queen_wt.bmp");
		pieceChooseImage->Canvas->Draw(300, 0, pieceBit);
	}
	else if(currTurn == BLACK) {                              //аналогично, если преобразуемая пешка черная
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Knight_bl.bmp");
			pieceChooseImage->Canvas->Draw(0, 0, pieceBit);
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Bishop_bl.bmp");
			pieceChooseImage->Canvas->Draw(100, 0, pieceBit);
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/Rook_bl.bmp");
			pieceChooseImage->Canvas->Draw(200, 0, pieceBit);
			pieceBit->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/Queen_bl.bmp");
			pieceChooseImage->Canvas->Draw(300, 0, pieceBit);

    }



	pieceChooseImage->OnMouseMove = transCursorMove;               //событию передвижения курсора по компоненту отображения рисунков присваиваем пользоват. метод
	pieceChooseImage->OnClick = transCursorClick;                 //событию клика по компоненту отображения рисунков присваиваем пользоват. метод


	pieceChooseForm->ShowModal();


	delete pieceChooseForm;
	delete fieldBit;
	delete pieceBit;
	pieceChooseForm = NULL;

}



void __fastcall TMainChessForm::transCursorMove(TObject* Sender, TShiftState Shift, int X, int Y) {     //метод, используемый для события передвижения курсора по компоненту отображения рисунков
	static chessType oldPiece = NOPIECE;                                         //объявляем и инициализируем локальную переменную типа chessType
	static Graphics::TBitmap* selected = chDskPaint->getCursorOn();               //единожды объявляем и инициализируем локальные указатели на необх. спрайты
	static Graphics::TBitmap* unsel_wt = chDskPaint->getNoCursorWhite();
	static Graphics::TBitmap* unsel_bl = chDskPaint->getNoCursorBlack();
	if(X >= 0 && X < 100)                                  //в зависимости от координаты  Х курсора устанавливаем значение поля choosePiece  класса  this
		choosePiece = KNIGHT;
	else if(X >= 100 && X < 200)
		choosePiece = BISHOP;
	else if(X >= 200 && X < 300)
		choosePiece = ROOK;
	else if(X >= 300 && X < 400)
		choosePiece = QUEEN;
	if(choosePiece != oldPiece) {                      //если текущая фигура не равна предыдущей

		 switch(oldPiece) {                          //в зависимости от типа предыдущей фигуры
		case KNIGHT:
			pieceChooseImage->Canvas->Draw(0, 0, unsel_wt);     //накладываем соответств. спрайт удаления выделения
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


		switch(choosePiece) {                          //в зависимости от типа текущей фигуры
		case KNIGHT:
			pieceChooseImage->Canvas->Draw(0, 0, selected);     //накладываем соответств. спрайт выделения
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

void __fastcall TMainChessForm::transCursorClick(TObject* Sender) {        //метод, используемый для события клика по компоненту отображения рисунков
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
	TButton* locButtPtr = dynamic_cast<TButton*>(Sender);          //динамически приводим общий тип компонента к типу кнопки
	if(locButtPtr->Caption == "New game")                   //если это кнопка начала новой игры
		newGame();                                        //вызываем метод начала новой игры
	else if(locButtPtr->Caption == "Save replay")            //если это кнопка сохранения игры
		menuSaveReplayClick(menuSaveReplay);                           //вызываем метод, выполняющийся при выборе пункта SaveGame основного меню
	else if(locButtPtr->Caption == "New replay")          //если это кнопка открытия нового реплея
		menuOpenReplayClick(menuOpenReplay);
	TForm* locFormPtr = dynamic_cast<TForm*> (locButtPtr->Parent);    //динамически приводим общий тип компонента-родителя
																	  //к типу формы
	locFormPtr->Close();                                              //закрываем форму
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

void TMainChessForm::restoreDeskNote(deskNote dN) {             	//метод, восстанавливающий ранее сохраненное состояние шахматной доски
	map<deskSection*, chessPiece*, sectCompare>* locMap = chDskPaint->getChDsk()->getMap();
	map<chessPiece*, deskSection*, lessPiecePtr>* locBlackChess = chDskPaint->getChDsk()->getBlackChess();
	map<chessPiece*, deskSection*, lessPiecePtr>* locWhiteChess = chDskPaint->getChDsk()->getWhiteChess();

	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	map<chessPiece*, deskSection*, lessPiecePtr>::iterator locBlackIt;
	map<chessPiece*, deskSection, lessPiecePtr>::iterator locWhiteIt;

	char sym = 'a';               //начальные значения индексов полей шах. доски
	int num = 1;

	locMap->clear();                     //очищаем отображения шах. полей, белых фигур и черных фигур после предыдущего состояния шах. доски
	locBlackChess->clear();
	locWhiteChess->clear();
	chDskPaint->getChDsk()->clearAddPiece();         //очищаем списки дополнительных белых и дополнительных черных фигур

	unsigned short blPawnCount = 0, blKnightCount = 0, blBishopCount = 0, blRookCount = 0, blQueenCount = 0;           //переменные, хранящие кол-во черных фигур определенного типа
	unsigned short wtPawnCount = 0, wtKnightCount = 0, wtBishopCount = 0, wtRookCount = 0, wtQueenCount = 0;
	//-----------Используя объекты типа deskNote, хранящие состояния шах. доски после каждого хода, восстанавливаем необходимое состояние и отрисовываем его------------
	for(int i = 0; i < SECTCOUNT; i++) {                  //для каждого из 64 шах. полей
		if(dN.chColor[i] == BLACK) {                    //если цвет фигуры, расположенной на поле, черный
			switch(dN.chType[i]) {                     //в зависимости от типа фигуры
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
	blackPiece = make_pair(NOPIECE, dSect);        //пара, состоящая из типа черной фигуры chessType и указателю на ее начальное поле
													//указатель chessPiece* в качестве первого члена пары использовать нельзя, т.к.
													 //при взятии фигуры из списка доп. фигур соответствующий объект удаляется с высвобождением памяти
	whitePiece = make_pair(NOPIECE, dSect);       //пара, состоящая из типа белой фигуры chessType и указателю на ее начальное поле
	blackShortCas = false;                               //флаг короткой рокировки черных
	blackLongCas = false;                                //флаг длинной рокировки черных
	whiteShortCas = false;                               //флаг короткой рокировки белых
	whiteLongCas = false;                                //флаг длинной рокировки белых
	blackTake = false;                                   //флаг взятия фигуры белыми
	whiteTake = false;                                   //флаг взятия фигуры черными
	whiteEp = false;                                     //взятие на проходе белой пешкой
	blackEp = false;                                     //взятие на проходе черной пешкой
	whiteTrans = false;                                  //флаг превращения белой пешки
	blackTrans = false;                                  //флаг превращения черной пешки
	blackCheck = false;                                 //флаг постановки шаха черными
	whiteCheck = false;                                 //флаг постановки мата белыми
	blackCheckmate = false;                             //флаг постановки мата черными
	whiteCheckmate =false;                             //флаг постановки мата белыми
	whiteStalemate = false;                            //флаг постановки пата белыми
	blackStalemate = false;                            //флаг постановки пата черными
	whiteDrawOffer = false;
	blackDrawOffer = false;
	drawSign = false;
	whiteWinSign = false;
	blackWinSign = false;
	blackTransPiece = NOPIECE;                      //тип черной фигуры после превращения

	whiteTransPiece = NOPIECE;                     //тип белой фигуры после превращения
    moveNumber = 0;
	moveNumber = ++moveCounter;                    //присваиваем объекту значение счетчика для того, чтобы сохранить порядковый номер хода

}

unsigned moveNote::moveCounter = 0;                       //инициализируем статическое поле - счетчик ходов
unsigned moveNote::uselessCounter = 0;

void moveNote::checkUselessMove() {
	//----------Если ход черных еще не выполнен--------------------------------------------------------------
	if(whitePiece.first != PAWN && whiteTake == false && blackPiece.first == NOPIECE)      //если ход белых выполнен не пешкой, без взятия фигур
		return;                                                                //оставляем счетчик ходов без обратимых изменений в прежнем состоянии
	else if(blackPiece.first == NOPIECE)                 //в противном случае обнуляем (независимо от хода черных счетчик сбрасывается, т.к. по правилу игра останавливается после 50 ходов без обратимых изменений ЗА КАЖДУЮ сторону)
		uselessCounter = 0;
	//-------------Если ход черных уже выполнен--------------------------------------------------------------
	else if(blackPiece.first != PAWN && whitePiece.first != PAWN &&           //если ни черные, ни белые не ходили пешками,
		blackTake == false && whiteTake == false)                       //и с обеих сторон не было взятий,
		uselessCounter++;                                                 //то инкрементируем счетчик пассивных ходов
	else                                                                //в противном случае
		uselessCounter = 0;                                             //обнуляем счетчик пассивных ходов
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
	char sym;                   //временный локальная переменная для посимвольного чтения из текстового файла
	int num;                   //временная локальная переменная для чтения чисел из текстового файла
	char word[15];              //временный локкальный массив для чтения слов из текстового файла
	char pieceSymbols[] = {'p', 'N', 'B', 'R', 'Q', 'K'};
	char* symPtr;
	unsigned short keyIndex = 0;            //индекс условного обозначения
	vector<string> keyVector;       //вектор условных обозначений "нестандартных" ситуаций в строке
	vector<string>::iterator keyIterator;
	string notationKeys[] = {"0-0", "0-0-0", "1-0", "0-1", "(=)", "1/2-1/2"};


//--------------Сначала считываем обозначения "нестандартных" ходов и ситуаций------------------------------------------------
	int keysCount = sizeof(notationKeys) / sizeof(string);         //количество условных обозначений в массиве у. о.
	size_t foundKey;                               //номер символа в строке, с которого начинается найденное в ней  у.о.
	for(int i = 0; i < keysCount; i++) {           //перебираем все возможные строки с условными обозначениями
		if((foundKey = moveStr.find(notationKeys[i])) != string::npos)        //если условное обозначение найдено в строке хода
			keyVector.push_back(notationKeys[i]);                       //добавляем его в вектор условных обозначений строки
	}
//----------------------------------------------------------------------------------------------------------------------------



	istringstream is(moveStr);       //создаем строковый поток, ассоциированный со входящей в метод строкой
	is >> sym;                      //читаем символы из потока
	if(!isdigit(sym)) {             //если первый символ не является цифрой (номер хода)
		ShowMessage("Replay file reading error! Incorrect replay file.1");          //выводим сообщение о некорректном файле реплея
		return;
	}
	while(isdigit(sym))             //читаем символы до тех пор, пока не попадется нечисловой (номер строки может быть 2-х и более -значным)
		is >> sym;
	if(sym != '.') {               //если первый нечисловой символ не является точкой
		ShowMessage("Replay file reading error! Incorrect replay file.2");          //выводим сообщение о некорректном файле реплея
		return;
	}

	is >> sym;                                             //читаем следующий символ
	//----------------------------------Проверяем наличие у.о. нестандартных ситуаций до записи хода белых--------------------------------------------------------
	symPtr = find(pieceSymbols, pieceSymbols + 6, sym);      //используя алгоритм find() ищем прочитанный символ среди индексов шах. фигур
	if(symPtr == pieceSymbols + 6) {                           //если сразу после номера хода мы прочитали неверный символ (не соответств. символу шахматной фигуры)
		//-----------------Читаем слово, которое вероятно является у.о. нестандартной ситуации-----------------------------------------------------
		int n = 0;
		word[0] = sym;                 //нулевому элементу читаемого слова присваиваем уже прочитанный символ
		while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //до тех пор, пока не будет прочитан пробел
			word[++n] = sym;             //прибавляем к слову прочитанные символы
		word[++n] = '\0';                 //завершаем слово символом конца строки
		//----------------------------------------------------------------------------------------------------------------------------------------
		if(keyVector.empty() == true) {                  //если вектор у.о. нестандартных ситуаций для текущей строки пуст
			ShowMessage("Replay file reading error! Incorrect replay file.3");            //выводим сообщение об ошибке
			return;                                                              //прекращаем выполнение метода
		 }
		 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //ищем прочитанное слово в векторе условных обозначений
		 if(keyIterator == keyVector.end()) {                                             //если слова нет в векторе у.о. для текущей строки, то также выводим сообщение об ошибке
			ShowMessage("Replay file reading error! Incorrect replay file.4");            //выводим сообщение об ошибке
			return;                                                              //прекращаем выполнение метода
		 }
		 else if((*keyIterator) == "0-0") {                  //распознаем запись корроткой рокировки белых
			 whitePiece.first = KING;                        //присваиваем свойствам оьбхекта-хода реплея соответств. значения
			 whitePiece.second.symbol = 'e';
			 whitePiece.second.number = 1;
			 whiteMoveSect.symbol = 'g';
			 whiteMoveSect.number = 1;
		 }
		 else if((*keyIterator) == "0-0-0") {              //распознаем запиись длинной рокировки белых
			 whitePiece.first = KING;
			 whitePiece.second.symbol = 'e';
			 whitePiece.second.number = 1;
			 whiteMoveSect.symbol = 'b';
			 whiteMoveSect.number = 1;
		 }

		 else if((*keyIterator) != "0-0" && (*keyIterator) != "0-0-0") {         //если найденное у.о. не является у.о. длинной рокировки или у.о. короткой рокировки
			 ShowMessage("Replay file reading error! Incorrect replay file.5");            //выводим сообщение об ошибке
				return;                                                              //прекращаем выполнение метода, т.к. остальные у.о. могут быть расположены только после записи хода белых или записи хода черных
		 }
		 is >> sym;                 //читаем следующий символ
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	else {                             //в противном случае, если прочитан символ, который входит в массив символов - индексов шахматный фигур
		switch(*symPtr) {               //в зависимости от того, индекс какой фигуры прочитан
		case 'p':
			whitePiece.first = PAWN;       //инициализируем свойства текущего объекта - хода реплея
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
		is >> sym;                     //далее читаем символ
		if(sym < 'a' || sym > 'h') {   //если он не входит в диапазон символов - буквенных индексов шахматных полей
			ShowMessage("Replay file reading error! Incorrect replay file.6");       //выводим сообщение об ошибке
			return;                                             //прекращаем выполнение метода
		}
		whitePiece.second.symbol = sym;          //буквенным индексом поля, С КОТОРОГО выполняется ход белых. назначаем прочитанный символ
		is >> num;                               //далее читаем численный индекс шахматного поля
		if(num < 0 || num > 8) {                 //если он не находится в диапазлне допустимых индексов шах. полей
			ShowMessage("Replay file reading error! Incorrect replay file.7");       //выводим сообщение об ошибке
			return;
		}
		whitePiece.second.number = num;          //численным индексом поля, С КОТОРОГО выполняется ход белых, назначаем прочитанное число
		is >> sym;                               //читаем следующий символ
		if(sym != 'x' && sym != '-') {           //если он не является символом хода '-' или сиволом взятия фигуры 'x'
			ShowMessage("Replay file reading error! Incorrect replay file.8");         //выводим сообщение об ошибке
			return;
		}
		if(sym == 'x')                          //если прочитан символ взятия
			whiteTake = true;                   //устанавливаем флаг взятия белыми
		is >> sym;                             //читаем следующий сивол
		if(sym < 'a' || sym > 'h') {           //если он не может являться буквенным индексом шах. поля
			ShowMessage("Replay file reading error! Incorrect replay file.9");         //выводим сообщение об ошибке
			return;
		}
		whiteMoveSect.symbol = sym;           //буквенным индексом поля, НА КОТОРОЕ осуществляется ход черных, назначаем прочитанный символ
		is >> num;                            //читаем число
		if(num < 0 || num > 8) {              //если число не может быть численным индексом шах. поля
			ShowMessage("Replay file reading error! Incorrect replay file.10");          //выводим сообщение об ошибке
			return;
		}
		whiteMoveSect.number = num;           //численным индексом поля, НА КОТОРОЕ осуществляется ход черных, назначаем прочитанное число

		is >> sym;                     //читаем следующий символ

		if(sym == '+')                  //если поставлен шах, в нотации пропускаем символ '+'
			is >> sym;
		else if(sym == '#')            //если поставлен мат, завершаем выполнение метода
			return;
	}
	symPtr = find(pieceSymbols, pieceSymbols + 6, sym);         //ищем полученный символ среди индексов типов фигур
	if(symPtr == pieceSymbols + 6 && sym != 'e') {                            //если следующий сивол не является индексом типа фигуры (черным) и не является символом 'e'

		//-----------------Читаем слово, которое вероятно является у.о. нестандартной ситуации-----------------------------------------------------
		int n = 0;
		word[0] = sym;                 //нулевому элементу читаемого слова присваиваем уже прочитанный символ
		while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //до тех пор, пока не будет прочитан пробел или конец строки или конец файла
			word[++n] = sym;             //прибавляем к слову прочитанные символы
		word[++n] = '\0';                 //завершаем слово символом конца строки
		//----------------------------------------------------------------------------------------------------------------------------------------
		if(keyVector.empty() == true) {                  //если вектор у.о. нестандартных ситуаций для текущей строки пуст
			ShowMessage("Replay file reading error! Incorrect replay file.11");            //выводим сообщение об ошибке
			return;                                                              //прекращаем выполнение метода
		 }
		 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //ищем прочитанное слово в векторе условных обозначений
		 if(keyIterator == keyVector.end()) {                                             //если слова нет в векторе у.о. для текущей строки, то также выводим сообщение об ошибке
			ShowMessage("Replay file reading error! Incorrect replay file.12");            //выводим сообщение об ошибке
			return;                                                              //прекращаем выполнение метода
		 }
		 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //найденное в векторе у.о. ищем в массиве всех у.о.
		 int notKeyNumber = notatKeyPtr - notationKeys;             //рассчитывам номер элемента массива всех у.о., которому соответствует элемент равный у.о., найденному в векторе
		 switch(notKeyNumber) {              //в зависимости от того, какое у.о. найдено в векторе
		  case 0:                          //в массиве у.о. элемент с таким индексом равен "0-0"
			blackPiece.first = KING;          //свойствам объекта-хода реплея присваиваем значения, соответствующие короткой рокировке черных
			blackPiece.second.symbol = 'e';
			blackPiece.second.number = 8;
			blackMoveSect.symbol = 'g';
			blackMoveSect.number = 8;
			break;
		  case 1:                           //в массиве у.о. элемент с таким индексом равен "0-0-0"
			blackPiece.first = KING;               //свойствам объекта-хода реплея присваиваем значения, соответствующие длинной рокировке черных
			blackPiece.second.symbol = 'e';
			blackPiece.second.number = 8;
			blackMoveSect.symbol = 'b';
			blackMoveSect.number = 8;
			break;
		 case 2:                          //в массиве у.о. элемент с таким индексом равен "1-0"
			whiteWinSign = true;          //устанавливаем флаг победы белых (во время хода черных)
			break;
		 case 4:                        //в массиве у.о. элемент с таким индексом равен "(=)"
			blackDrawOffer = true;       //устанавливаем флаг предложения ничьи со стороны черных
			break;
		 case 5:                         //в массиве у.о. элемент с таким индексом равен "1/2-1/2"
			blackDrawSign = true;         //устанавливаем флаг ничьи во время хода черных
			break;
		 default:                                                           //если вдруг прочитано что-то иное
			ShowMessage("Replay file reading error! Incorrect replay file.13");            //выводим сообщение об ошибке
			return;                                                              //прекращаем выполнение метода
		 }

	}

	else {
		if(sym == 'e') {         //если 'e' - прочитанный символ
			//--------------------Читаем у.о. взятия пешки на проходе "e.p.", если таковое имеется------------------------------------
			is >> sym;
			if(sym == '.') {
				is >> sym;
				if(sym == 'p') {
					is >> sym;
					if(sym != '.') {               //если у.о., то есть последовательность символов "e.p." полностью не прочитана, то
							ShowMessage("Replay file reading error! Incorrect replay file.14");       //выводим сообщение об ошибке
							return;
					}
					whiteEp = true;
				}
			}
			//--------------------------------------------------------------------------------------------------------------------
			is >> sym;
			//--------------------------После у.о. взятия на проходе также может быть у.о. другой нестандартной ситуации------------------------------
			symPtr = find(pieceSymbols, pieceSymbols + 6, sym);
			if(symPtr == pieceSymbols + 6) {
					//-----------------Читаем слово, которое вероятно является у.о. нестандартной ситуации-----------------------------------------------------
					int n = 0;
					word[0] = sym;                 //нулевому элементу читаемого слова присваиваем уже прочитанный символ
					while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //до тех пор, пока не будет прочитан пробел
						word[++n] = sym;             //прибавляем к слову прочитанные символы
					word[++n] = '\0';                 //завершаем слово символом конца строки
					//----------------------------------------------------------------------------------------------------------------------------------------
					if(keyVector.empty() == true) {                  //если вектор у.о. нестандартных ситуаций для текущей строки пуст
						ShowMessage("Replay file reading error! Incorrect replay file.15");            //выводим сообщение об ошибке
						return;                                                              //прекращаем выполнение метода
					 }
					 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //ищем прочитанное слово в векторе условных обозначений
					 if(keyIterator == keyVector.end()) {                                             //если слова нет в векторе у.о. для текущей строки, то также выводим сообщение об ошибке
						ShowMessage("Replay file reading error! Incorrect replay file.16");            //выводим сообщение об ошибке
						return;                                                              //прекращаем выполнение метода
					 }
					 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //найденное в векторе у.о. ищем в массиве всех у.о.
					 int notKeyNumber = notatKeyPtr - notationKeys;             //рассчитывам номер элемента массива всех у.о., которому соответствует элемент равный у.о., найденному в векторе
					 switch(notKeyNumber) {              //в зависимости от того, какое у.о. найдено в векторе
					  case 0:                          //в массиве у.о. элемент с таким индексом равен "0-0"
						blackPiece.first = KING;          //свойствам объекта-хода реплея присваиваем значения, соответствующие короткой рокировке черных
						blackPiece.second.symbol = 'e';
						blackPiece.second.number = 8;
						blackMoveSect.symbol = 'g';
						blackMoveSect.number = 8;
						break;
					  case 1:                           //в массиве у.о. элемент с таким индексом равен "0-0-0"
						blackPiece.first = KING;               //свойствам объекта-хода реплея присваиваем значения, соответствующие длинной рокировке черных
						blackPiece.second.symbol = 'e';
						blackPiece.second.number = 8;
						blackMoveSect.symbol = 'b';
						blackMoveSect.number = 8;
						break;
					 case 2:                          //в массиве у.о. элемент с таким индексом равен "1-0"
						whiteWinSign = true;          //устанавливаем флаг победы белых (во время хода черных)
						break;
					 case 4:                        //в массиве у.о. элемент с таким индексом равен "(=)"
						blackDrawOffer = true;       //устанавливаем флаг предложения ничьи со стороны черных
						break;
					 case 5:                         //в массиве у.о. элемент с таким индексом равен "1/2-1/2"
						blackDrawSign = true;         //устанавливаем флаг ничьи во время хода черных
						break;
					 default:                                                           //если вдруг прочитано что-то иное
						ShowMessage("Replay file reading error! Incorrect replay file.17");            //выводим сообщение об ошибке
						return;                                                              //прекращаем выполнение метода
					 }
			}
			//-----------------------------------------------------------------------------------------------------------------------------------
		}

		switch(*symPtr) {                   //если после записи хода белых получен корректный индекс шах.фигуры, в зависимости от значения прочитанного символа (индекса)
		case 'p':
			blackPiece.first = PAWN;       //устанавливаем в качестве типа шах. фигуры, которой выполнен ход черными, значение PAWN
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
		//----------------------Блок чтения символа, соответствующего типу фигуры, полученной после преобразования пешки-------------------------------------------------------
		is >> sym;
		if(sym == '+')         //после индекса фигуры, полученной из пешки, может быть у.о. шаха - '+'
			is >> sym;         //в таком случае читаем следующий символ
		symPtr = find(pieceSymbols, pieceSymbols + 6, sym);        //если на текущем ходе было превращение фигуры, то в строке будут два символа, соответств. типу фигур подряд
		if(symPtr != pieceSymbols + 6) {
			whiteTransPiece = blackPiece.first;                    //В случае прочтения второго символа первый становится типом фигуры, в которую трансформировалась пешка
			switch(*symPtr) {
			case 'p':
				blackPiece.first = PAWN;
				break;
			case 'N':
				blackPiece.first = KNIGHT;                       //Второй в свою очередь присваивается типу фигуры, перемещенной на текущем ходу.
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
			is >> sym;                             //читаем следующий символ
		}

		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if(sym == '+')
			is >> sym;
		else if(sym == '#') {
								 //если после получения индекса шах. фигуры не следует символьный индекс ячейки, а следует символ мата
			whiteTransPiece  = blackPiece.first;      //данный индекс сохраняем как фигуру, полученную после преобразования пешки
			blackPiece.first = NOPIECE;             //обнуляем ранее присвоенный тип черной фигуры для данного хода
			return;
        }
		//---------------После записи типа фигуры, в которую превратилась пешка, возможно окончание игры либо после мата, либо после нестандартной ситуации----------
		if(sym <'a' || sym > 'h') {              //если прочитанный символ не может быть буквенным индексом шахматного поля

            //-----------------Читаем слово, которое вероятно является у.о. нестандартной ситуации-----------------------------------------------------
			int n = 0;
			word[0] = sym;                 //нулевому элементу читаемого слова присваиваем уже прочитанный символ
			while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //до тех пор, пока не будет прочитан пробел
				word[++n] = sym;             //прибавляем к слову прочитанные символы
			word[++n] = '\0';                 //завершаем слово символом конца строки
			//----------------------------------------------------------------------------------------------------------------------------------------
			if(keyVector.empty() == true) {                  //если вектор у.о. нестандартных ситуаций для текущей строки пуст
				ShowMessage("Replay file reading error! Incorrect replay file.15");            //выводим сообщение об ошибке
				return;                                                              //прекращаем выполнение метода
			 }
			 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //ищем прочитанное слово в векторе условных обозначений
			 if(keyIterator == keyVector.end()) {                                             //если слова нет в векторе у.о. для текущей строки, то также выводим сообщение об ошибке
				ShowMessage("Replay file reading error! Incorrect replay file.16");            //выводим сообщение об ошибке
				return;                                                              //прекращаем выполнение метода
			 }
			 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //найденное в векторе у.о. ищем в массиве всех у.о.
			 int notKeyNumber = notatKeyPtr - notationKeys;             //рассчитывам номер элемента массива всех у.о., которому соответствует элемент равный у.о., найденному в векторе
			 switch(notKeyNumber) {              //в зависимости от того, какое у.о. найдено в векторе
											   //0 и 1 соответствуют у.о. коротой и длинной рокировок, но в этом месте их записаей быть не может
			 case 2:                          //в массиве у.о. элемент с таким индексом равен "1-0"
				whiteWinSign = true;          //устанавливаем флаг победы белых (во время хода черных)
				whiteTransPiece = blackPiece.first;
				blackPiece.first = NOPIECE;
				break;
			 case 4:                        //в массиве у.о. элемент с таким индексом равен "(=)"
				blackDrawOffer = true;       //устанавливаем флаг предложения ничьи со стороны черных
				whiteTransPiece = blackPiece.first;
				blackPiece.first = NOPIECE;
				break;
			 case 5:                         //в массиве у.о. элемент с таким индексом равен "1/2-1/2"
				blackDrawSign = true;         //устанавливаем флаг ничьи во время хода черных
				whiteTransPiece = blackPiece.first;
                blackPiece.first = NOPIECE;
				break;
			 default:                                                           //если вдруг прочитано что-то иное
				ShowMessage("Replay file reading error! Incorrect replay file.17");            //выводим сообщение об ошибке
				return;                                                              //прекращаем выполнение метода
			 }

			return;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------------------
		blackPiece.second.symbol = sym;           //в качестве буквенного индекса поля, С КОТОРОГО был выполнен ход черными, назначаем прочитанный символ
		is >> num;                           //читаем число
		if(num < 0 || num > 8) {            //если прочитанное число не является допустимым индексом щах. поля
			ShowMessage("Replay file reading error! Incorrect replay file.19");          //выводим сообщение об ошибке
			return;
		}
		blackPiece.second.number = num;          //в качестве численного индекса поля, С КОТОРОГО был выполнен ход черными, назначаем прочитанное число
		is >> sym;                              //читаем символ
		if(sym != 'x' && sym != '-') {           //если прочитанный символ не является символом хода '-' или символом взятия 'x'
			ShowMessage("Replay  file reading error! Incorrect replay file.20");       //выводим сообщение об ошибке
			return;
		}
		if(sym == 'x')                              //если прочитанный символ - символ взятия фигуры
			blackTake = true;                       //то устанавливаем флаг взятия черными
		is >> sym;                                  //читаем символ
		if(sym < 'a' || sym > 'h') {               //если символ не может являться буквенным индексом шах. поля
			ShowMessage("Replay file reading error! Incorrect replay file.21");       //выводим сообщение обошибке
			return;
		}
		blackMoveSect.symbol = sym;          //в качестве буквенного индекса поля, НА КОТОРОЕ осуществлен ход черными, назначаем прочитанный символ

		is >> num;                  //читаем число
		if(num < 0 || num > 8) {        //если прочитанное число не может являться численным индексом шах. поля
			ShowMessage("Replay file reading error! Incorrect replay file.22");         //выводим сообщение об ошибке
			return;
		}
		blackMoveSect.number = num;          //в качестве численного индекса поля, НА КОТОРОЕ осуществлен ход черными назначаем прочитанное число
		if(is >> sym) {                      //читаем следующий символ, если не достигнут конец файла (или не произошел какой-гибудь другой сбой)
			symPtr = find(pieceSymbols, pieceSymbols + 6, sym);
			//--------------------------Чтение символа, соответствующего типу фигуры, после преобразования пешки-------------------------------------------
			if(symPtr != pieceSymbols + 6) {                    //если символ соответствует индексу шахмат. фигуры
				switch(*symPtr) {
				case 'N':
					blackTransPiece = KNIGHT;            //присваиваем полю, хранящему тип фигуры, в которую трансформировалась пешка, щначение в зависимости от прочитанного символа
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
				is >> sym;              //читаем следующий символ
			}
			//---------------------------------------------------------------------------------------------------------------------------------------------
			else if(sym == 'e') {         //если 'e' - прочитанный символ
				//--------------------Читаем у.о. взятия пешки на проходе "e.p.", если таковое имеется------------------------------------
				is >> sym;
				if(sym == '.') {
					is >> sym;
					if(sym == 'p') {
						is >> sym;
						if(sym != '.') {               //если у.о., то есть последовательность символов "e.p." полностью не прочитана, то
								ShowMessage("Replay file reading error! Incorrect replay file.23");       //выводим сообщение об ошибке
								return;
						}
						blackEp = true;
					}
					is >> sym;          //читаем следующий символ
				}
				//-----------------------------------------------------------------------------------------------------------------------
			}
			if(sym == '+' || sym == '#')                      //если прочли символы шаха и мата, то пропускаем и читаем новый символ (после мата получаем признак конца файла)
				is >> sym;
			if(is) {                    //если во входном потоке все еще нет ошибок (например, не достигнут конец файла)
				symPtr = find(pieceSymbols, pieceSymbols + 6, sym);
				if(symPtr == pieceSymbols + 6) {
						//-----------------Читаем слово, которое вероятно является у.о. нестандартной ситуации-----------------------------------------------------
						int n = 0;
						word[0] = sym;                 //нулевому элементу читаемого слова присваиваем уже прочитанный символ
						while((sym = is.get()) != ' ' && sym != '\n' && !is.eof())   //до тех пор, пока не будет прочитан пробел
							word[++n] = sym;             //прибавляем к слову прочитанные символы
						word[++n] = '\0';                 //завершаем слово символом конца строки
						//----------------------------------------------------------------------------------------------------------------------------------------
						if(keyVector.empty() == true) {                  //если вектор у.о. нестандартных ситуаций для текущей строки пуст
							ShowMessage("Replay file reading error! Incorrect replay file.24");            //выводим сообщение об ошибке
							return;                                                              //прекращаем выполнение метода
						 }
						 keyIterator = find(keyVector.begin(), keyVector.end(), word);          //ищем прочитанное слово в векторе условных обозначений
						 if(keyIterator == keyVector.end()) {                                             //если слова нет в векторе у.о. для текущей строки, то также выводим сообщение об ошибке
							ShowMessage("Replay file reading error! Incorrect replay file.25");            //выводим сообщение об ошибке
							return;                                                              //прекращаем выполнение метода
						 }
						 string* notatKeyPtr = find(notationKeys, notationKeys + 6, *keyIterator);     //найденное в векторе у.о. ищем в массиве всех у.о.
						 int notKeyNumber = notatKeyPtr - notationKeys;             //рассчитывам номер элемента массива всех у.о., которому соответствует элемент равный у.о., найденному в векторе
						 switch(notKeyNumber) {              //в зависимости от того, какое у.о. найдено в векторе
						 case 0:                          //в массиве у.о. элемент с таким индексом равен "0-0"
								break;                     //т.к. после записи черных у.о. рокировки располагаться  не может, выходим из блока
						 case 1:                           //в массиве у.о. элемент с таким индексом равен "0-0-0"
								break;
						 case 3:                          //в массиве у.о. элемент с таким индексом равен "0-1"
							blackWinSign = true;          //устанавливаем флаг победы черных (во время хода белых)
							break;
						 case 4:                        //в массиве у.о. элемент с таким индексом равен "(=)"
							whiteDrawOffer = true;       //устанавливаем флаг предложения ничьи со стороны белых
							break;
						 case 5:                         //в массиве у.о. элемент с таким индексом равен "1/2-1/2"
							whiteDrawSign = true;         //устанавливаем флаг ничьи во время хода белых
							break;
						 default:                                                           //если вдруг прочитано что-то иное
							ShowMessage("Replay file reading error! Incorrect replay file.26");            //выводим сообщение об ошибке
							return;                                                              //прекращаем выполнение метода
						 }
				}
				//-----------------------------------------------------------------------------------------------------------------------------------
			}

		}

	}

}

void TMainChessForm::makeRepMove() {

	repMoveNote::currMove = (deskNote::currDesk - 1) / 2;        //перед выполнением хода в реплее в зависимости от текущего состояния доски
																 //определяем какой ход из вектора должен быть выполнен следующим,
																 //учитывая, что кол-во ходов прблизительно в 2 раза больше кол-ва состояний
	if((deskNote::currDesk - 1) % 2)                   //если остаток от деления кол-ва состояний (шах. доски) равен 1,
		currTurn = BLACK;                             //то текущий ход реплея - ход черных
	else                                               //если равен 0,
		currTurn = WHITE;                              //то текущий ход реплея - ход белых

	if(currTurn == WHITE) {						   //если текущий ход - ход белых

		deskSection* sourceDS = &repMoveNotes[repMoveNote::currMove].whitePiece.second;         //из объекта, хранящего информацию об одном из ходов реплея, читаем исходную позицию белой фигуры
		deskSection* targetDS = &repMoveNotes[repMoveNote::currMove].whiteMoveSect;             //из объекта, хранящего информацию об одном из ходов реплея, читаем целевую белой фигуры
		prevChosenSect = sourceDS;                          //для выполнения хода из реплея используем методы, применяемые для выполнения обычных ходов
		prevSect = targetDS;                                //для этого свойствам, хранящим результаты взаимодействия пользователя с интерфесом (при обычной игре),
															//присваиваем значения, прочитанные из текстового файла (и ранее сохраненные в объекты типа repMoveNote


		if(targetDS->number != 0) {                         //если целевое поле существует
			if(repMoveNotes[repMoveNote::currMove].whiteTake == true) {      //если установлен флаг взятия фигуры
				if(repMoveNotes[repMoveNote::currMove].whitePiece.first == PAWN && sourceDS->symbol != targetDS->symbol && repMoveNotes[repMoveNote::currMove].whiteEp == true)     //для взятия пешки на проходе
					movePiece(targetDS);                           //в соответствии с логикой работы программы используем метод выполнения хода (используемый и в обычной игре)
				else
					takePiece(targetDS);                         //для любых других взятий используем метод взятия шах. фигуры (также используемый и в обычной игре)
            }

			else                                       //если флаг взятия фигуры не установлен
				movePiece(targetDS);                  //вызываем метод выполнения хода
		}
		repMoveNotes[repMoveNote::currMove].samePosesWhite = sameDesksCount;                    //для текщуего объекта - хода реплея сохраняем количество одинаковых позиций в соответств. поле
		repMoveNotes[repMoveNote::currMove].uselessMoves = moveNote::uselessCounter;      //для текущего объекта - хода реплея сохраняем кол-во ходов без взятия и передвижения пешек в соотв. поле (после хода белых)

	}
	else if(currTurn == BLACK) {                     //аналогично для случая, если текущий ход - черный
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
		repMoveNotes[repMoveNote::currMove].samePosesBlack = sameDesksCount;                    //для текщуего объекта - хода реплея сохраняем количество одинаковых позиций в соответств. поле (после хода черных)
		repMoveNotes[repMoveNote::currMove].uselessMoves = moveNote::uselessCounter;      //для текущего объекта - хода реплея сохраняем кол-во ходов без взятия и передвижения пешек в соотв. поле (после хода черных)
	}

	//----------------Отображаем окно окончания игры для нестандартных случаев (сдача, нехватка материала, недостаток времени и т.п.)--------------------------
	if(resultIsShowed == false) {
		if(repMoveNotes[repMoveNote::currMove].whiteDrawSign == true || repMoveNotes[repMoveNote::currMove].blackDrawSign == true ||  repMoveNotes[repMoveNote::currMove].whiteDrawOffer == true ||
		repMoveNotes[repMoveNote::currMove].blackDrawOffer == true || repMoveNotes[repMoveNote::currMove].whiteWinSign == true || repMoveNotes[repMoveNote::currMove].blackWinSign == true) {
			if(deskNote::currDesk - 1 == repMoveNotes.size() * 2 && repMoveNotes[repMoveNotes.size() - 1].blackPiece.first != NOPIECE ||          //если текущий ход последний (для обоих случаев:
				deskNote::currDesk - 1 == repMoveNotes.size() * 2 - 1 && repMoveNotes[repMoveNotes.size() - 1].blackPiece.first == NOPIECE) {      //игра заканчивается ходом черных, игра заканчивается ходом белых)
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
				if(repMoveNotes.size() != 0)                    //если игра является реплеем
					saveReplayButt->Caption = "New replay";            //отключаем кнопку сохранения реплея на форме результатов игры
				endGameForm->ShowModal();
				delete endGameForm;
			}
		}
	}
	//----------------------------------------------------------------------------------------------------------------------------
	if(currTurn == WHITE) {                             //если после выполнения хода реплея право хода перешло белым (если партия завершилась ходом белых, право хода остается у белых),
													   //также партия завершается ходом белых, если во время хода белых случилась нестандартная ситуация (белые сдались или предложили ничью, а черные приняли)
		repMoveNote::currMove++;                      //инкрементируем номер текущего хода реплея
		//---------------В лист-боксе прочитанных ходов реплея отмечаем предыдущий ход как выполненный (удаляем серую заливку с соответствующей строки)---------------------------------------------
		if(repMoveNote::currMove == (deskNotes.size() - 1) / 2) {                             //если выполняется ранее не выполненный ход (последний ход может повторяться)
			AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //Переменной типа AnsiString присваиваем строку соответствующего элемента лист-бокса
			TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //объявляем и инициализируем указатель на объект типа TListBoxInfo
																																	 //присваиваем ему указатель на объект соотв. эл-та лист-бокса
			if(listItemInfo != NULL)                               //если вышеуказанная динамическая идентификация прошла успешно, и указатель ненулевой
				listItemInfo->moveIsMade = true;                   //присваиваем значение true флагу выполнения хода(полю объекта эл-та лист-бокса)
			repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //для того, чтобы элемент лист-бокса был моментально перерисован, удаляем его
			repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //на месте удаленного вставляем новый элемент лист-бокса
		}
		else if(repMoveNote::currMove == (deskNotes.size() / 2) && repMoveNote::currMove == repMoveNote::moveCounter &&      //для случая, если игра завершилась ходом белых
		repMoveNotes[repMoveNotes.size() - 1].blackMoveSect.number == 0) {               //блок else if полностью повторяет блок else, введен для удобства чтения
			AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //Переменной типа AnsiString присваиваем строку соответствующего элемента лист-бокса
			TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //объявляем и инициализируем указатель на объект типа TListBoxInfo
																																	 //присваиваем ему указатель на объект соотв. эл-та лист-бокса
			if(listItemInfo != NULL)                               //если вышеуказанная динамическая идентификация прошла успешно, и указатель ненулевой
				listItemInfo->moveIsMade = true;                   //присваиваем значение true флагу выполнения хода(полю объекта эл-та лист-бокса)
			repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //для того, чтобы элемент лист-бокса был моментально перерисован, удаляем его
			repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //на месте удаленного вставляем новый элемент лист-бокса
		}
		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	}
	else if(currTurn == BLACK && deskNotes[deskNotes.size() - 1].lastTurn == BLACK) { 		//для случая, если партия завершилась ходом черных (право хода не передавалось белым, поледний ход совершен черными)
		repMoveNote::currMove++;                      //инкрементируем номер текущего хода реплея
		//---------------В лист-боксе прочитанных ходов реплея отмечаем предыдущий ход как выполненный (удаляем серую заливку с соответствующей строки)---------------------------------------------
		if(repMoveNote::currMove == (deskNotes.size() - 1) / 2) {                             //если выполняется ранее не выполненный ход (последний ход может повторяться)
			AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //Переменной типа AnsiString присваиваем строку соответствующего элемента лист-бокса
			TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //объявляем и инициализируем указатель на объект типа TListBoxInfo
																																	 //присваиваем ему указатель на объект соотв. эл-та лист-бокса
			if(listItemInfo != NULL)                               //если вышеуказанная динамическая идентификация прошла успешно, и указатель ненулевой
				listItemInfo->moveIsMade = true;                   //присваиваем значение true флагу выполнения хода(полю объекта эл-та лист-бокса)
			repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //для того, чтобы элемент лист-бокса был моментально перерисован, удаляем его
			repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //на месте удаленного вставляем новый элемент лист-бокса
		}
		//------------------------------------------------------------------------------------------------------------------------------------------------------------
	}
	else if(currTurn == BLACK && deskNotes[deskNotes.size() - 1].lastTurn == WHITE) {   //для случая, сели партия завершилась нестандартной ситуацией на ходу черных (сдача, предложение ничьи)
																					//но фактический последний ход осуществлен именно белыми
		if(deskNote::currDesk - 1 == repMoveNotes.size() * 2 - 1 && repMoveNotes[repMoveNotes.size() - 1].blackPiece.first == NOPIECE) {              //если ходдействительно последний
            repMoveNote::currMove++;                      //инкрементируем номер текущего хода реплея
			//---------------В лист-боксе прочитанных ходов реплея отмечаем предыдущий ход как выполненный (удаляем серую заливку с соответствующей строки)---------------------------------------------
			if(repMoveNote::currMove == (deskNotes.size()) / 2) {                             //если выполняется ранее не выполненный ход (последний ход может повторяться)
				AnsiString moveStr = repMovesBox->Items->Strings[repMoveNote::currMove - 1];                  //Переменной типа AnsiString присваиваем строку соответствующего элемента лист-бокса
				TListBoxItemInfo* listItemInfo = dynamic_cast<TListBoxItemInfo*>(repMovesBox->Items->Objects[repMoveNote::currMove - 1]);    //объявляем и инициализируем указатель на объект типа TListBoxInfo
																																		 //присваиваем ему указатель на объект соотв. эл-та лист-бокса
				if(listItemInfo != NULL)                               //если вышеуказанная динамическая идентификация прошла успешно, и указатель ненулевой
					listItemInfo->moveIsMade = true;                   //присваиваем значение true флагу выполнения хода(полю объекта эл-та лист-бокса)
				repMovesBox->Items->Delete(repMoveNote::currMove - 1);     //для того, чтобы элемент лист-бокса был моментально перерисован, удаляем его
				repMovesBox->Items->InsertObject(repMoveNote::currMove - 1, moveStr, listItemInfo);      //на месте удаленного вставляем новый элемент лист-бокса
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

	//------------------------Переводим данные, хранящиеся в объектах типа moveNote в текстовый формат-------------------------
	for(int i = 0; i < moveNotes.size(); i++) {

		saveFileStream << moveNotes[i].getTextMove();

		saveFileStream << endl;
	}
	//-------------------------------------------------------------------------------------------------------------------------
	moveNotes.clear();          //очищаем вектор, хранящий объекты-записи ходов


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
	whiteTotalTime++;                            //инкрементируем счетчик времени белых в секундах
	//----------------------------В зависимости от режима работы таймера рассчитываем оставшееся время в формате mm : ss -------------------------
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

	//-----------Если определенное значение счетчиков времени (в зависимости от режима работы таймера) не достигнуто, то прерываем выполнение метода-------------------
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

   //---------------------В случае истечения времени белых, проверяем, достаточно ли материала у черных---------------------------
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

	//------------------Выводим окно результатов партии-----------------------------------------------------------------------
	if(currTurn == WHITE) {

		if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //деактивируем кнопки вкл/выкл таймера, предложения ничьи и сдачи
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
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
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
	blackTotalTime++;                   //инкрементируем значение счетчика времени черных

	//----------------------------В зависимости от режима работы таймера рассчитываем оставшееся время в формате mm : ss -------------------------
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

	//-----------Если определенное значение счетчиков времени (в зависимости от режима работы таймера) не достигнуто, то прерываем выполнение метода-------------------
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

	//----------------------В случае истечения времени черных, проверяем, достаточно ли материала у белых---------------------
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

	//---------------------------Выводим окно результатов игры--------------------------------------------------------------
	if(currTurn == BLACK) {

        	if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
					whiteTimer->Enabled = false;
					blackTimer->Enabled = false;
		}

		whiteTimerStop->Enabled = false;            //деактивируем кнопки вкл/выкл таймера, предложения ничьи и сдачи
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
				movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
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
			movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //вывод изменений в текстовое поле записи ходов
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

	  //while(!readFileStream.eof()) - неудачная конструкция, т.к. проверка конца файла будет осуществлена
	  //после полного выполнения итерации цикла, а значит, возможно чтение и попытка обработки "мусора"
	  //в то же время сама операция чтения из файла может возвращать 0 при неудачном чтении, что позволяет ее использовать
	  //в качестве условного выражения цикла while()

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

	  //while(!readFileStream.eof()) - неудачная конструкция, т.к. проверка конца файла будет осуществлена
	  //после полного выполнения итерации цикла, а значит, возможно чтение и попытка обработки "мусора"
	  //в то же время сама операция чтения из файла может возвращать 0 при неудачном чтении, что позволяет ее использовать
	  //в качестве условного выражения цикла while()

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
		timerOptions tmpTimerOpt;       //определяем временный объект настроек таймера
		//---------------------Считываем данные, введенные пользователем в поля формы, и сохраняем их во временном объекте настроек таймера
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
        tmpTimerOpt.timerOn = tmpTimerOpt.moveLimit || tmpTimerOpt.gameLimit || tmpTimerOpt.someMovesLimit;         //если ни один из режимов таймера не выбран, снимаем флаг включенного таймера во временном объекте настроек

		//----------------------------------------------------------------------------------------------------------------------

		//--------------------------Проверяем правильность данных, введенных пользователем через форму---------------------------
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

		//--------------------Сохраняем данные из временного объекта настроек таймера в текстовый файл настроек------------------
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
		//-----------------Обновляем информацию о таймере в текстовых полях главной формы-------------------------------------------------------------------------

		TMainChessForm* tMainForm = (TMainChessForm*) (tOptForm->Owner);

		if(tmpTimerOpt.timerOn == false) {                            //если во временном объекте настроек таймера признак включенного таймера не установлен,
			if(tMainForm->menuTimerSwitch->Checked == true)           //и если таймер до этого был включен
				tMainForm->menuTimerSwitchClick(tMainForm->menuTimerSwitch);      //вызываем событие по клику на пункт меню,  отвечающий за включение\выключение таймера,
																				 //таким образом, выключаем таймер
			tMainForm->blackTimerMemoSet(0, "Timer is off.");
			tMainForm->blackTimerMemoSet(1, "");
			tMainForm->whiteTimerMemoSet(0, "Timer is off.");
			tMainForm->whiteTimerMemoSet(1, "");

		}
		else if(tmpTimerOpt.gameLimit == true) {                 	//если во временном объекте настроек таймера установлен признак лимита на игру,
			if(tMainForm->menuTimerSwitch->Checked == true) {     	//и если таймер до этого был включен
				tMainForm->blackTimerMemoSet(0, AnsiString(tmpTimerOpt.gameLimitTime) + " : 00");      //в соответствующих текстовых полях обновляем информацию о таймере
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


		*this = tmpTimerOpt;                     //основному объекту настроек таймера присваиваем временный объект

}
void __fastcall TMainChessForm::menuTimerStopClick(TObject *Sender)                   //событие при клике пункта "Stop" меню "Timer"
{
	if(menuTimerStop->Caption == "Stop") {                             //если текущее название(надпись) пункта меню - "Stop", т.е. таймер в данный момент работает
		blackTimer->Enabled = false;                                   //после нажатия останавливаем таймер черных
		whiteTimer->Enabled = false;                                   //останавливаем таймер белых
		menuTimerStop->Caption = "Continue";                           //название пункта меняем на "Continue"
		if(currTurn == WHITE)
			whiteTimerStop->Caption = "Continue";
		else if(currTurn == BLACK)
			blackTimerStop->Caption = "Continue";
		chessGraphic->Enabled = false;                                 //Отключаем графический объект - шахматное поле
		menuSaveGame->Enabled = true;
	}
	else if(menuTimerStop->Caption == "Start") {                      //если текущее название(надпись) пункта меню - "Start", т.е. таймер в данный момент не работает,
																	  //и НЕ запускался ранее в течение партии
		if(currTurn == WHITE) {      			//если текущий ход - ход белых
			if(moveNote::moveCounter == 0 && tOpt->someMovesLimit == true)        //если текущий ход является первым
				whiteTotalTime -= tOpt->addTimePerMove;                    //то сразу после старта игры (а не после первого хода белых!) к лимиту времени прибавляем зажанное пользователем  значение
																		   //согласно системе контроля времени Фишера, принятой в ФИДЕ (т.е. вычитаем из общего прошедшего времени белых)
			whiteTimer->Enabled = true;                                  //включаем таймер белых
			menuTimerSwitch->Enabled = false;
        }

		else if(currTurn == BLACK)
			blackTimer->Enabled = true;
		menuTimerStop->Caption = "Stop";                             //название пункта меняем на "Stop"
		menuTimerOptions->Enabled = false;                           //запрещаем изменение настроек таймера во время его работы
		menuSaveGame->Enabled = false;                                //запрещаем сохранение игры вл время работы таймера
		chessGraphic->Enabled = true;                                //Включаем граф. объект - шахматное поле

	}
	else if(menuTimerStop->Caption == "Continue") {                 //если текущее название(надпись) пункта меню - "Continue", т.е. таймер в данный момент не работает
	   if(currTurn == WHITE) {                                         //в зависимости от того, какая сторона еще не сыграла на данном ходу, если белые
			whiteTimer->Enabled = true;                             //возобновляем работу таймера белых
			whiteTimerStop->Caption = "Stop";
	   }
	   else if(currTurn == BLACK) {                                    //если черные
			blackTimer->Enabled = true;                             //возобновляем работу таймера черных
			blackTimerStop->Caption = "Stop";
	   }
	   menuTimerStop->Caption = "Stop";                             //название пункта меняем на "Stop"
	   chessGraphic->Enabled = true;                                //Включаем граф. объект - шахматное поле
	   menuSaveGame->Enabled = false;
	}

}
//---------------------------------------------------------------------------

void __fastcall TMainChessForm::menuTimerOptionsClick(TObject *Sender)
{
	tOptionsForm = new TForm(this);                      //выделяем память и инициализтоуем форму
    tOptionsForm->Icon->LoadFromFile(ExtractFilePath(Application->ExeName) + "main_icon.ico");
	tOptionsForm->Position = poScreenCenter;
	tOptionsForm->BorderStyle = bsSingle;
	tOptionsForm->BorderIcons >> biMaximize;

	tOptionsForm->Caption = "Timer Options";
	tOptionsForm->Name = "Timer_Options_Form";
	tOptionsForm->Width = 400;
	tOptionsForm->Height = 405;
	TButton* okButt = new TButton(tOptionsForm);          //создаем кнопку "Ок"
	okButt->Parent = tOptionsForm;
	okButt->OnClick = tOpt->saveTimerOptions;
	okButt->Caption = "Save";
	okButt->Name = "Save_Timer_Options_Button";
	okButt->Top = 345;
	okButt->Left = 205;
	TButton* cancelButt = new TButton(tOptionsForm);       //Создаем кнопку "Cancel"
	cancelButt->Parent = tOptionsForm;
	cancelButt->OnClick = parentFormClose;
	cancelButt->Caption = "Cancel";
	cancelButt->Name = "Cancel_Timer_Options_Button";
	cancelButt->Top = 345;
	cancelButt->Left = 300;


	TGroupBox* gameLimitGroup = new TGroupBox(tOptionsForm);          //Создаем объединение счетчика времени на игру
	gameLimitGroup->Parent = tOptionsForm;
	gameLimitGroup->Caption = "Game limit timer";
	gameLimitGroup->Name = "Game_Limit_GroupBox";
	gameLimitGroup->Left = 5;
	gameLimitGroup->Top = 7;
	gameLimitGroup->Width = 180;
	TGroupBox* moveLimitGroup = new TGroupBox(tOptionsForm);        //Создаем объединение счетчика времени на ход
	moveLimitGroup->Parent = tOptionsForm;
	moveLimitGroup->Caption = "Move limit timer";
	moveLimitGroup->Name = "Move_Limit_GroupBox";
	moveLimitGroup->Left = 205;
	moveLimitGroup->Top = 7;
	TGroupBox* sMovesLimitGroup = new TGroupBox(tOptionsForm);      //Создаем объединение счетчика времени по периодам
	sMovesLimitGroup->Parent = tOptionsForm;
	sMovesLimitGroup->Caption = "Some moves limit timer";
	sMovesLimitGroup->Name = "Some_Moves_Limit_GroupBox";
	sMovesLimitGroup->Left = 7;
	sMovesLimitGroup->Top = 130;
	sMovesLimitGroup->Width = 382;
	sMovesLimitGroup->Height = 205;


 //-----------------Добавляем чекбоксы к группбоксам------------------------------------------------------------------
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

//--------------------Добавляем текстовые поля к группбоксам-----------------------------------------------------------------
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


//---------В зависимости от настроек, прочитанных из файла, определяем доступность физуальных компонентов--------------------
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

void __fastcall TMainChessForm::whiteConcedeClick(TObject* Sender) {             //метод, завязанный на событие по нажатию на кнопку сдачи белых
	chessGraphic->Enabled = false;
	moveNotes[moveNotes.size() - 1].blackWinSign = true;               //в текстовом описании последнего хода фиксируем победу черных
	movesLog->Lines->Delete(movesLog->Lines->Count - 1);             //в случае с методом по событию сдачи белых удаляем последнюю пустую строку
	movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения вектора ходов

	if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
		whiteTimer->Enabled = false;
		blackTimer->Enabled = false;
	}
	//----------------------Открываем окно(форму) рузультатов игры----------------------------------------------------------------------
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
	moveNotes[moveNotes.size() - 1].whiteWinSign = true;               //в текстовом описании последнего хода фиксируем победу черных
	movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения вектора ходов

	if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
		whiteTimer->Enabled = false;
		blackTimer->Enabled = false;
	}
	//----------------------Открываем окно(форму) рузультатов игры----------------------------------------------------------------------
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
		moveNotes[moveNotes.size() - 1].drawSign = true;               //в текстовом описании последнего хода фиксируем победу черных
		movesLog->Lines->Delete(movesLog->Lines->Count - 1);             //в случае с методом по событию сдачи белых удаляем последнюю пустую строку
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения вектора ходов

		if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
			whiteTimer->Enabled = false;
			blackTimer->Enabled = false;
		}
		//----------------------Открываем окно(форму) рузультатов игры----------------------------------------------------------------------
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
		moveNotes[moveNotes.size() - 1].drawSign = true;               //в текстовом описании последнего хода фиксируем победу черных
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] = (moveNotes[moveNotes.size() - 1].getTextMove()).c_str();     //выводим последние изменения вектора ходов

		if(tOpt->timerOn == true) {                     //если таймер включен, останавливаем таймеры черных и белых
			whiteTimer->Enabled = false;
			blackTimer->Enabled = false;
		}
		//----------------------Открываем окно(форму) рузультатов игры----------------------------------------------------------------------
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

	//-----------Сохраняем состояние переключателя таймера Switch On в текстовом файле настроек-----------------------------------
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
		deskSection prevPosition;          //предыдущая позиция фигуры (заполняется для пешек)
		bool isMoved;                      //флаг передвижения фигуры (устанавливается для короля или ладьи, если ими совершался хотя бы 1 ход)
		bool isChecked;                   //флаг постановки шаха (устанавливается для короля, которому поставлен шах)
		char coordX;                    //Х-координата шахматного поля
		int coordY;                     //Y-координата шахматного поля
		chessColor chColor;
		chessType chType;
		int chNumber;

	};
	saveGameSect* saveDesk = new saveGameSect[SECTCOUNT];         //состояние шахматного поля описывается массивом объектов saveGameSect,
												//поля которого не содержат указателей, чтобы была возможность сохранения поля в бинарный файл
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

	unsigned* movesCounterPtr = new unsigned;                      //выделяем память под число, равное количеству ходов
	*movesCounterPtr = moveNotes[moveNotes.size() - 1].moveCounter;                 //инициализируем указатель соответств. полем элемента вектора vecto<moveNote> moveNotes
	saveFileStream.write(reinterpret_cast<char*>(movesCounterPtr), sizeof(unsigned));        //сохраняем количество ходов в бинарный файл
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

	//-------Объявляем и определяем вспомогательную структуру timerOptSave для корректного сохранения опций таймера в текстовый файл в бинарном виде--------
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
	else return;                                          //если  диалоговое окно открытия файла закрыто без выбора конкретного файла, завершаем выполнение метода

	//-----------------------В области расширения основной формы Оотображаем визуальные компоненты  для режима игры (если до этого был режим воспроизв. реплея)-----------------------------
	if(repMovesBox != NULL) {               //если существует лист-бокс ходов реплея (т.е. ранее открывался реплей)
		if(repMovesBox->Visible == true) {      //если лист-бокс ходов реплея отображается

			repMovesBox->Hide();                //скрываем визуальные компоненты, отображаемые в режиме воспроизв. реплея
			nextMainButt->Hide();
			prevMainButt->Hide();
			playMainButt->Hide();
			gotoMainButt->Hide();
			numberMainEdit->Hide();
			delayMainEdit->Hide();
			delayRepLabel->Hide();
			samePosLabel->Hide();
			uselessMovesLabel->Hide();

			blackTimerMemo->Show();                 //выводим визуальные компоненты, отображаемые в режиме игры
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
		deskSection prevPosition;          //предыдущая позиция фигуры (заполняется для пешек)
		bool isMoved;                      //флаг передвижения фигуры (устанавливается для короля или ладьи, если ими совершался хотя бы 1 ход)
		bool isChecked;                   //флаг постановки шаха (устанавливается для короля, которому поставлен шах)
		char coordX;                    //Х-координата шахматного поля
		int coordY;                     //Y-координата шахматного поля
		chessColor chColor;
		chessType chType;
		int chNumber;
		saveGameSect() : isMoved(false) {
			chColor = NOCOLOR;
			chType = NOPIECE;
        }

	};

	saveGameSect* saveDesk = new saveGameSect[SECTCOUNT];         //состояние шахматного поля описывается массивом объектов saveGameSect,
																	//поля которого не содержат указателей, чтобы была возможность сохранения поля в бинарный файл
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

	if(currTurn == BLACK)           //если текцщий ход - ход черных
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] += "";         //возвращаем курсор на последнюю созданную линию в текстовом поле movesLog
	else if(currTurn == WHITE) {      //иначе, если текущий ход - ход белых
		movesLog->Lines->Add("");    //добавляем строку на месте текщего положения курсора
		movesLog->Lines->Strings[movesLog->Lines->Count - 1] += "";        //переводим курсор на последнюю (созданную) строку
	}

	char* timerOnPtr = new char;                  //объявляем и определяем временный указатель на признак работы таймера
	openFileStream.read(timerOnPtr, 1);           //читаем признак работы таймера из бинарного файла
	if(!openFileStream)                           //если прочитать файл не удалось,
		if(errorIsShowed == false) {             //и с ообщение об ошибке ранее не выводилось
			ShowMessage("Reading error!");        //выводим сообщение об ошибке
			errorIsShowed = true;                 //устанавливаем флаг выведенного сообщения об ошибке
		}
	tOpt->timerOn = *timerOnPtr;                 //свойству текущего объекта настроек таймера (флаг работы таймера) присваиваем значение, на которое ссылается временный указ.

		//-------Объявляем и определяем вспомогательную структуру timerOptSave для корректного чтения опций таймера из текстового файла в бинарном виде--------
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


   //----------------------Читаем настройки таймера из бинарного текстового файла и сохраняем их в текущем объекте настроек таймера-------------------------------------------------------
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


	//----------Выводим в текстовые поля черного и белого таймеров сведения о их начальном состоянии после загрузки игры----------------------
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
		 //--------------Определяем состояние и доступность элементов управления и шахматной доски после загрузки игры---------------------------------
		 chessGraphic->Enabled = false;
		 menuTimerSwitch->Checked = true;
		 menuTimerSwitch->Enabled = false;
		 menuTimerOptions->Enabled = false;               //если в загруженной игре таймер используется, запрещаем доступ к его настройкам
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
		menuTimerOptions->Enabled = true;                      //если в загруженной игре таймер не используется, разрешаем доступ к его настройкам
		blackTimerStop->Enabled = false;
		blackTimerStop->Caption = "Start";
		whiteTimerStop->Enabled = false;
		whiteTimerStop->Caption = "Start";
		blackTimer->Enabled = false;
		whiteTimer->Enabled = false;
		//-------Выводим в текстовые поля черного и белого таймеров сведения о их начальном состоянии после загрузки игры--------------
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



	//--------------Инициализируем свойства основных объектов  значениями из загруженных данных (объектов для хранения)---------------------------------

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

	chDskPaint->getChDsk()->clearAddPiece();                      //очищаем список дополнительных фигур предыдущей игры

	chessPiece* tmpPiecePtr;
	deskSection* tmpSectPtr;
	for(int i = 0; i < SECTCOUNT; i++) {
		switch(saveDesk[i].chType) {
		case PAWN:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, PAWN, saveDesk[i].chNumber);          //возвращаем указатель на элемент массива шах.фигур
			tmpSectPtr = localDesk.sectFromArr(saveDesk[i].coordX, saveDesk[i].coordY);      //возвращаем указатель на эл-т массива шах. полей
			if(saveDesk[i].prevPosition.number != 0 && saveDesk[i].prevPosition.symbol != 0)     //если существует предыдущая позиция пешки
				tmpPiecePtr->setPrevPos(localDesk.sectFromArr(saveDesk[i].prevPosition.symbol, saveDesk[i].prevPosition.number));     //меняем соответствующее поле объекта-пешки
			else tmpPiecePtr->setPrevPos(NULL);                                          //в противном случае присваиваем соответствующему полю знач. нулевого указ.

			locMap->insert(make_pair(tmpSectPtr, tmpPiecePtr));                        //добавляем инициализированные указатели на объект-фигуру и объект шах. поле в отображение
			if(saveDesk[i].chColor == BLACK)
				locBlackChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			else if(saveDesk[i].chColor == WHITE)
				locWhiteChess->insert(make_pair(tmpPiecePtr, tmpSectPtr));
			break;
		case KNIGHT:
			tmpPiecePtr = localDesk.pieceFromArr(saveDesk[i].chColor, KNIGHT, saveDesk[i].chNumber);
			if(tmpPiecePtr->getChessType() == NOPIECE) {                                   //если в массиве элемента с таким номером нет,
				tmpPiecePtr = new Knight(saveDesk[i].chColor, saveDesk[i].chNumber);        //выделяем под такой элемент память
				localDesk.addPiece(tmpPiecePtr);                                         //указатель на доп. фигуру добавляем к списку доп. фигур
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

//----------Приводим шахматную доску в исходное состояние----------------------------------------------------------------------
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

	blackTimer->Enabled = false;                //если после предыдущей игры остались работающие таймеры выключаем их
	whiteTimer->Enabled = false;
	movesLog->Clear();                         //очищаем тестовое поле для отображения ходов
	samePosLabel->Caption = "Same positions: 1";            //задаем начальное значение метки повторяющихся позиций реплея
	uselessMovesLabel->Caption = "Reversible moves: 0";    //задаем начальное значение ходов без необратимых изменений
	//-------------------------------------------------------------------------------------------------------------------------



	//------------Задаем первый элемент в векторе, хранящий шахматные позиции (для реализации правила 3-х повторяющихся позиций)----
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

	//-------------------Расширенную область основной формы перестраиваем под режим воспроизведения реплея---------------------
	blackTimerMemo->Hide();        //скрываем компоненты, используемые в расширенной области при обычной игре
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

	if(repMovesBox == NULL) {                 //если компоненты расширенной области главной формы еще не были созданы ранее
		repMovesBox = new TListBox(this);
		repMovesBox->Parent = this;

		repMovesBox->Left = 910;
		repMovesBox->Top = 5;
		repMovesBox->Width = 270;
		repMovesBox->Height = 700;
		repMovesBox->ItemHeight = 30;
		repMovesBox->OnClick = repMovesBoxClick;



		repMovesBox->Style = lbOwnerDrawFixed;             //устанавливаем режим отрисовки

		repMovesBox->OnDrawItem = repMovesBoxDraw;        //к событию отрисовки элемента лист-бокса привязываем собственный метод

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
	if(menuMinimize->Checked == true)           //если при открытии реплея основная форма открыта без расширенной области
		menuReplayControlClick(menuReplayControl);            //открываем панель управления реплеем

	//---------------------------Открываем и построчно читаем файл реплея--------------------------------------------------------
	ifstream openFileStream;
	openFileStream.open(openReplayFile.c_str());
	if(!openFileStream) {
		ShowMessage("Error! Can't open replay file.");
		return;
	}
	string moveStr;

	while(getline(openFileStream, moveStr)) {           //построчно читаем открытый текстовый файл
		repMoveNote rpNote;                             //объявляем объект типа reMoveNote (ход реплея)
		rpNote.setMoveText(moveStr);                    //с помощью метода, преобразующего описание хода в шах. нотации (текст), присваиваем значения свойствам ранее созданного объекта
		repMoveNotes.push_back(rpNote);                //добавляем созданные объект в соответствующий вектор
		TListBoxItemInfo* itemInfo = new TListBoxItemInfo;           //объявляем и определяем объект с данными о элементе лист-бокса repMovesBox
		itemInfo->moveIsMade = false;
		repMovesBox->AddItem(AnsiString(moveStr.c_str()), itemInfo);

	}

	//--------------------------------------------------------------------------------------------------------------------------
	repMovesBox->ItemIndex = 0;                      //изначально выбранным эл-ом лист-бокса устанавливаем первый эл-т
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
	if(repMovesBox->ItemIndex * 2 <= deskNotes.size() - 1) {             //Если выбранная пользователем строка в лист-боксе соответствует уже выполненному ходу
		unsigned deskIndex = repMovesBox->ItemIndex * 2;     //определяем локальную переменную - индекс вектора состояний шах. доски и инициализируем ее по индексу выбранного элемента
															 //в лист-боксе ходов реплея (кол-во состояний = кол-во ходов * 2 + 1, индексы элементов лист-бокса начинаются с нуля)
		restoreDeskNote(deskNotes[deskIndex]);
		deskNote::currDesk = deskIndex + 1;
		repMoveNote::currMove = (deskNote::currDesk - 1) / 2;
		if(replayControlForm != NULL) {
			nextRepButt->Enabled = true;                         //после перехода на заданный ход по клику пользователя активируем кнопку следующего хода
			playRepButt->Enabled = true;
		}
		if(nextMainButt != NULL) {
			nextMainButt->Enabled = true;
			playMainButt->Enabled = true;
		}
		if(repMovesBox->ItemIndex != 0) {                     //если переход осуществлен не на первый ход
			if(replayControlForm != NULL)
				prevRepButt->Enabled = true;                   //активируем кнопку предыдущего хода на панели управления реплеем
			if(prevMainButt != NULL)
				prevMainButt->Enabled = true;                  //активируем кнопку предыдущего хода в области расширения
		}
		else {
			if(replayControlForm != NULL)
				prevRepButt->Enabled = false;
			if(prevMainButt != NULL)
				prevMainButt->Enabled = false;
		}
		if(menuReplayControl->Checked == true) {
			if(replayControlForm != NULL)
				numberRepEdit->Text = repMovesBox->ItemIndex + 1;    //в текстовое поле панели управления реплеем записываем номер хода, к которому перешел пользователь
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
	else {                                                  //в противном случае, если теущее состояние доски равно последнему инициализированному, выполняем все ходы вплоть до выбранного включительно
		if(deskNote::currDesk < deskNotes.size()) {
			restoreDeskNote(deskNotes[deskNotes.size() - 1]);
			deskNote::currDesk = deskNotes.size();
		}


		unsigned mCount;                //количество ходов, которые необходимо выполнить (суммарно черными и белыми) для перехода к выбранному пользователем ходу
		mCount = repMovesBox->ItemIndex * 2 - ((deskNotes.size() - 1));         //рассчитываем кол-во ходов, которое необходимо выполнить (номер выбранной строки хода * 2 - кол-во состояний шах. доски - 1)
		static bool repPanelIsOpen = true;            //определяем локальный флаг, устанавливаемый при открытой панели управления реплеем
		//---------Скрываем визуальные компоненты панели управления реплеем---------------------------------------
		if(menuReplayControl->Checked == false) {          //если панель управления реплеем не была открыта пользователем
			repPanelIsOpen = false;             //устанавливаем флаг, сохраняющий изначальное состояние панели упр. реплеем (открыта\закрыта)
			menuReplayControlClick(menuReplayControl);       //вызываем метод, открывающий панель, тем самым определяем все визуальные компоненты, размещаемые на ней
		}
		nextRepButt->Hide();
		prevRepButt->Hide();
		gotoRepButt->Hide();
		playRepButt->Hide();
		numberRepEdit->Hide();

		//---------------------------------------------------------------------------------------------------------
		//-----------------Определяем визуальные компоненты, отображающиеся в процессе выполнения ходов (прогресс-бар и др.)-------
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
		repMovesProgress->Max = 100;		   //количество сегментов прогресс-бара
		//-----------------------------------------------------------------------------------------------------------

		if(mCount > 0) {                      //если количество ходов больше 0
			int movesProgStep = 100 / mCount;        //рассчитываем шаг прогресс-бара
			for(int i = 0; i < mCount; i++) {
				makeRepMove();                                             //вызываем метод выполнения хода реплея рассчитанное число раз
				repMovesProgress->StepBy(movesProgStep);
			}
			repMovesProgress->StepBy(movesProgStep);                              //добавляем последний сегмент к прогресс-бару
		}


		repMovesProgress->Hide();                                 //скрываем прогресс-бар, отображающий процесс выполнения ходов после клика по еще не выполненному ходу
		repMovesProgLabel->Hide();                               //скрываем метку, поясняющую состояние прогресс-бара
		delete repMovesProgress;                                 //освобождаем память, заниммаемую объектом прогресс-бара
		delete repMovesProgLabel;                               //освобождаем память, занимаемую объектом метки
		nextRepButt->Show();                                    //отображаем визуальные компонеты штатного режима панели управления реплеем
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
			numberRepEdit->Text = repMovesBox->ItemIndex + 1;      //в текстовое поле панели управления реплеем записываем номер хода, к которому перешел пользователь
		if(numberMainEdit != NULL)
			numberMainEdit->Text = repMovesBox->ItemIndex + 1;
        samePosLabel->Caption = "Same positions: " + UIntToStr(sameDesksCount);
		uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(moveNote::uselessCounter);
	}

	repMoveNote::currMove = repMovesBox->ItemIndex;
	chDskPaint->paintPieces();                        //отрисовываем фигуры после перехода на заданный ход
}



void __fastcall TMainChessForm::menuReplayControlClick(TObject *Sender) {

	if(menuReplayControl->Checked == false) {
		 menuReplayControl->Checked = true;

		replayControlForm = new TForm(this);
		replayControlForm->OnClose = replayControlClose;            //событию OnClose присваиваем метод, высвобождающий память, занимаемую формой
																	//такая реализация необходима, т.к. форма replayControlForm может быть закрыта в любой момент
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
		if(playMainButt != NULL) {                           //если основная кнопка воспроизведения реплея (в расширенной области главной формы) уже существует,
			playRepButt->Enabled = playMainButt->Enabled;     //копируем ее состояние
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
		if(prevMainButt != NULL)                             //если основная кнопка перехода к предыдущему ходу уже существует, копируем ее состояние
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

void __fastcall TMainChessForm::replayControlClose(TObject* Sender, TCloseAction& Action) {       //метод, вызываемый по событию OnClose формы replayControlForm
	Action = caFree;                           //высвобождение памяти, занимаемой формой, при ее закрытии
	menuReplayControl->Checked = false;
    replayControlForm = NULL;
	return;

}

void __fastcall TMainChessForm::playRepButtClick(TObject* Sender) {

	if(playMainButt->ImageIndex == 0) {              //Если на кнопке воспроизведения расположено изображение "старт"
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
	else if(playMainButt->ImageIndex == 4) {        //если на кнопке воспроизведения расположено изображение "пауза"
		replayTimer->Enabled = false;
		playMainButt->ImageIndex = 0;
		if(replayControlForm != NULL)
			playRepButt->ImageIndex = 0;
    }
}

void __fastcall TMainChessForm::nextRepButtClick(TObject* Sender) {
	 if(deskNote::currDesk <= deskNotes.size() - 1 && (repMoveNote::currMove < repMoveNote::moveCounter - 1 ||                //если ход ранее выполнялся и не является последним
	 repMoveNote::currMove == repMoveNote::moveCounter - 1 && repMoveNotes[repMoveNote::currMove].blackPiece.first != NOPIECE && deskNotes[deskNote::currDesk - 1].lastTurn == BLACK)) {     //или является последним, но весь ход (под опр. номером) состоит из хода белых и черных, и только что ходили белые (т.е. ранее этот ход белых уже выполнялся)
		restoreDeskNote(deskNotes[deskNote::currDesk]);                    //восстанавливаем состояние шахматной доски, соотв. тому, что должно быть после выполнения хода
		repMoveNote::currMove = (deskNote::currDesk - 1) / 2;              //по номеру текущего состояния шах. доски определяем номер текущего хода
		currTurn = ((deskNote::currDesk - 1) % 2) ? BLACK : WHITE;
		//-------------Определяем количество повторяющихся позиций и ходов без взятий фигурами отличными от пешки------------------------------------------------
		if(samePosLabel != NULL && uselessMovesLabel != NULL) {
			if(currTurn == WHITE) {                        //если последний ход был белым (право хода еще не перешло противоположной стороне)
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove].samePosesWhite);
				if(repMoveNotes[repMoveNote::currMove].uselessMoves == 0 && repMoveNotes[repMoveNote::currMove].whiteTake == false && repMoveNotes[repMoveNote::currMove].whitePiece.first != PAWN)
					uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove].uselessMoves);   //если на текущем ходе счетчик ходов без необратимых изменений сбрасывается,
																																	//то после хода белых, если он явлется таковым, счетчик уже можно обновлять
			}
			else if(currTurn == BLACK) {                   //если последний ход был черным (право хода еще не перешло противоположной стороне)
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove].samePosesBlack);
				uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove].uselessMoves);
			}
        }
		//----------------------------------------------------------------------------------------------------------------------------------------------------------
		deskNote::currDesk++;                                               //инкрементируем текущее состояние шахматной доски
		repMoveNote::currMove = (deskNote::currDesk - 1) / 2;              //по номеру текущего состояния шах. доски определяем номер текущего хода
		currTurn = ((deskNote::currDesk - 1) % 2) ? BLACK : WHITE;         //по номеру текущего состоян. шах. доски определяем какой стороне перешло право хода
		chDskPaint->paintPieces();                                          //после восстановления шах. доски отрисовываем ее

	}
	else if(deskNote::currDesk == deskNotes.size() - 1 && repMoveNote::currMove == repMoveNote::moveCounter - 1) {    //если след. ход выполнялся ранее и он является последним
		makeRepMove();
		if(deskNotes.size() == 1)              //если после выполнения хода реплея, кол-во состояний шах. доски приняло значение 1 (т.е. не было совершено ни одного хода),
			return;                            //то это значит, что пользователь начал новую игру или реплей, следовательно необходимо завершить данный метод
		if(samePosLabel != NULL && uselessMovesLabel != NULL) {
			if(deskNotes[deskNote::currDesk - 1].lastTurn == BLACK)         //в данном случаем оператор сравнения выполняется уже после передачи права хода
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].samePosesBlack);

			else if(deskNotes[deskNote::currDesk - 1].lastTurn == WHITE)     //в данном случаем оператор сравнения выполняется уже после передачи права хода
				samePosLabel->Caption = "Same positions: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].samePosesWhite);
			uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].uselessMoves);
		}
	}
	else {                       //в противном случае: если ход не выполнялся (последний ход необходимо выполнять, даже если он был выполнен ранее, т.к. он вызывет окно окончания игры)
		makeRepMove();
        if(deskNotes.size() == 1)              //если после выполнения хода реплея, кол-во состояний шах. доски приняло значение 1 (т.е. не было совершено ни одного хода),
			return;                            //то это значит, что пользователь начал новую игру или реплей, следовательно необходимо завершить данный метод
		if(samePosLabel != NULL && uselessMovesLabel != NULL) {
			samePosLabel->Caption = "Same positions: " + UIntToStr(sameDesksCount);
			uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(moveNote::uselessCounter);
		}
	}
	//--------------Определяем состояние визуальных компонентов области расширения основной формы программы------------------------
	if(repMoveNote::currMove == repMoveNote::moveCounter) {       //если номер текущего хода больше равен общему кол-ву ходов
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

	if(currTurn == BLACK && repMoveNote::currMove == 0) {       //если выполнен первый ход (ход белых)
		if(replayControlForm != NULL)
			prevRepButt->Enabled = true;                          //активируем кнопку возврата на предыдущий ход
		if(prevMainButt != NULL)
			prevMainButt->Enabled = true;
	}
	else if(currTurn == WHITE)
		repMovesBox->ItemIndex = repMoveNote::currMove;               //после хода черных (когда право хода перешло белым) индексу выбранного эл-та присваиваем значение тек. хода

	if(replayControlForm != NULL)
		numberRepEdit->Text = UIntToStr((deskNote::currDesk + 1) / 2);               //в специальном текстовом поле оторажаем номер хода, который
																//рассчитывается как количество сост. шах. доски + 1 (т.к. необходимо отобразить
																//номер предстоящего хода), деленное на 2
	if(numberMainEdit != NULL)
		numberMainEdit->Text = UIntToStr((deskNote::currDesk + 1) / 2);
    //-------------------------------------------------------------------------------------------------------------------------------
	return;
}



void __fastcall TMainChessForm::prevRepButtClick(TObject* Sender) {          //метод, выполняемый при нажатии кнопки пред. хода
																			//панели управления реплеем
	deskNote::currDesk--;
	if(deskNote::currDesk == 1) {
		if(replayControlForm != NULL)
			prevRepButt->Enabled = false;
		if(prevMainButt !=NULL)
			prevMainButt->Enabled = false;
	}

	restoreDeskNote(deskNotes[deskNote::currDesk - 1]);
	currTurn = ((deskNote::currDesk - 1) % 2) ? BLACK : WHITE;       //определяем, кому принадлежит право хода после декремента текущего состояния шах. доски
	repMoveNote::currMove = (deskNote::currDesk - 1) / 2;
	if(deskNote::currDesk   < deskNotes.size()) {            //если порядковый номер текущей шахматной доски меньше размера вектора шах. досок
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
			if(repMoveNotes[repMoveNote::currMove].uselessMoves != 0) {         //если количество ходов без необратимых изменеий для данного объекта-хода реплея = 0
				if(repMoveNote::currMove > 0)             //если данный ход реплея - не первый
					uselessMovesLabel->Caption = "Reversible moves: " + UIntToStr(repMoveNotes[repMoveNote::currMove - 1].uselessMoves);
				else                              //если ход реплея - первый
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

	bool repPanelIsOpen = true;            		//определяем локальный флаг, устанавливаемый при открытой панели управления реплеем
	if(menuReplayControl->Checked == false)             //если пункт отрытия панели управления реплеем не выделен галочкой (панель закрыта)
		repPanelIsOpen = false;                        //снимаем флаг открытий панели управления реплеем
	else
		repPanelIsOpen = true;

	if(StrToInt(numberText) > repMoveNote::moveCounter) {                       //если пользователь в текстовое поле в панели управления реплеем вводит число
																						//превосходящее кол-во ходов в записи реплея
		ShowMessage(AnsiString("Invalid move number. This game has ") + UIntToStr(repMoveNote::moveCounter)  + AnsiString(" moves"));    //выводим сообщение об ошибке
		if(replayControlForm != NULL)
			numberRepEdit->Text = (deskNote::currDesk + 1) / 2;                             //в тектовое поле выводим номер текущего хода
		if(numberMainEdit != NULL)
			numberMainEdit->Text = (deskNote::currDesk + 1) / 2;
		return;
	}
	else if((StrToInt(numberText) - 1) * 2 <= deskNotes.size() - 1) {               //если заправшиваемый пользователем ход меньше кол-ва выполненных ходов
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
	else if((StrToInt(numberText) - 1) * 2 > deskNotes.size() - 1) {                //если запрашиваемый пользователем ход меньше кол-ва ходов, прочитанных из файла,
																					//но при этом больше кол-ва выполненных ходов (кол-во сотояний, деленное на 2)

		if(deskNote::currDesk < deskNotes.size()) {                               //сначала восстанавливаем последний выполненный ранее ход (последнее состояние шах. доска)
			restoreDeskNote(deskNotes[deskNotes.size() - 1]);                     //из диапазоны между текущим ходом и запрашиваемым (например, текущий - 3, запрашиваемый - 10,
			deskNote::currDesk = deskNotes.size();                                //ранее было выполнено 5 ходом, восстанавливаем состояние после 5-го хода, а уже потом выполняем новые ходы
		}                                                                         //с 6 по 10)

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
		repMovesProgress->Max = 100;               //количество сегментов прогресс-бара
		int movesProgStep = 100 / ((StrToInt(numberText) * 2 - 1) - deskNotes.size());        //рассчитываем шаг прогресс-бара

		for(int i = deskNotes.size(); i < StrToInt(numberText) * 2 - 1; i++) {   //выполняем столько ходов, сколько составляет разница двух упомянутых
			makeRepMove();                                                              //выше величин - 1 (т.к. всегда отображается номер предстоящего хода)
			repMovesProgress->StepBy(movesProgStep);                            //добавляем сегменты к прогресс-бару пропорционально выполненным ходам
		}
		repMovesProgress->StepBy(movesProgStep);                              //добавляем последний сегмент к прогресс-бару

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

void __fastcall TMainChessForm::replayTimerTimer(TObject *Sender)             //таймер автоматического воспроизведения реплея
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



