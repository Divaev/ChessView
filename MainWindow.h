//---------------------------------------------------------------------------

#ifndef MainWindowH
#define MainWindowH
//---------------------------------------------------------------------------


#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <vector>
#include "ChessPaint.h"               //��������� ������������ ���� ������� ��� ��������� ����� � �����
				//���� � ����� �� ����������� ������������ ������ ���� ���������  using namespace std,
				//�� ��������� ���� ������������ ���� (� ��� ����� ��� ����������) ����� ����� �����������
				//������������ ������ (���, ����� �� ���� ������ ��������� ���� ����� ���� ��������� �����. ����������!)
//---------------------------------------------------------------------------
class moveNote;
class deskNote;
class repMoveNote;
struct timerOptions;
struct configOptions;



class TMainChessForm : public TForm
{
__published:	// IDE-managed Components
	TImage *chessGraphic;
	TTimer *blackTimer;
	TTimer *whiteTimer;
    TTimer *replayTimer;
	TMainMenu *GameMenu;
	TMenuItem *menuHead;
	TMenuItem *menuNewGame;
	TMenuItem *menuOpenReplay;
	TMenuItem *menuTimer;
	TMenuItem *menuTimerSwitch;
	TMenuItem *menuTimerStop;
	TMenuItem *menuTimerOptions;
	TMenuItem *N1;
	TMenuItem *menuCloseGame;
	TMenuItem *menuSaveReplay;
	TSaveDialog *saveReplayDialog;
	TMenuItem *menuSaveGame;
	TMenuItem *menuOpenGame;
	TSaveDialog *saveGameDialog;
	TOpenDialog *openGameDialog;
	TMenuItem *menuView;
	TMenuItem *menuMinimize;
	TMenuItem *menuReplayControl;
	TOpenDialog *openReplayDialog;
	TImageList *repPlayerButtons;

	void __fastcall CursorMove(TObject *Sender, TShiftState Shift, int X,
		  int Y);
	void __fastcall SectionClick(TObject *Sender);
	void __fastcall menuNewGameClick(TObject *Sender);
	void __fastcall menuCloseGameClick(TObject *Sender);
	void __fastcall menuSaveReplayClick(TObject *Sender);
	void __fastcall whiteTimerTimer(TObject *Sender);
	void __fastcall blackTimerTimer(TObject *Sender);
	void __fastcall menuTimerStopClick(TObject *Sender);
	void __fastcall menuTimerOptionsClick(TObject *Sender);
	void __fastcall menuTimerSwitchClick(TObject *Sender);
	void __fastcall menuSaveGameClick(TObject *Sender);
	void __fastcall menuOpenGameClick(TObject *Sender);
	void __fastcall menuMinimizeClick(TObject *Sender);
	void __fastcall menuOpenReplayClick(TObject *Sender);
	void __fastcall menuReplayControlClick(TObject *Sender);
	void __fastcall replayTimerTimer(TObject *Sender);                                   //����� ������


private:	// User declarations
		bool whiteChecked;                //����, ����������� �������� true, ���� ������ ������ ��������� ���
		bool blackChecked;               //����, ����������� �������� true, ���� ������� ������ ��������� ���
		bool firstMoveMade;             //���� ������� ���� ������ (���������� ���� �����)
        bool resultIsShowed;            //����, ��������������� ��� ��������� ���� ���������� ����
		chessDeskPaint*  chDskPaint;             //��������� �� ������ ���������
		deskSection* prevSect;                                  //��������� ������, �� ������� �������� ������
                                                    //������������ ������������� ������� ����� � �������� �������
													//SectSelect() � CursorMove() ��������������
		deskSection* prevChosenSect;              //��������� ��������� ������ ���������� �������������� ��������
		set<deskSection*, lessSectPtr>* possMovesPtr;        // ��������� �� ��������� ��������� ��� ���� ����� ��� ��������� ��������� ������
		set<deskSection*, lessSectPtr>::iterator possMovesIt;      //�������� ��������� ��������� ��� ���� �����
		set<deskSection*, lessSectPtr>* possTakesPtr;               //��������� �� ��������� ��������� ��� ������ ����� ��������� ��������� ������
		set<deskSection*, lessSectPtr>::iterator possTakesIt;      //�������� ��������� ��������� ��� ������ �����
		chessColor currTurn;                                //����, ������������, � ����� ������� ������ ���� ������ ��� (�����\������)
		chessType choosePiece;                             //��� ������, ����������  ����� ������������� �����
		vector<moveNote> moveNotes;                     //������, �������� ������� ���� moveNote, ��������� ��� ����������
														//������ � �������� ����
		vector<deskNote> deskNotes;                     //������, �������� ��������� ����� ����� ������� ���� (������ � �����)
														// � �������������� ����
		vector<repMoveNote> repMoveNotes;               //������, �������� ������� - ���� ������� ������

		unsigned sameDesksCount;
		int blackTotalTime;                        //��������� �����, ����������� �������� ������
		int whiteTotalTime;                        //��������� �����, ����������� �������� �����
//---------------���������� ����������, ����������� �����������-------------------------------------------------------------------
		TImage* pieceChooseImage;                      //��������� ��� ����������� ����������� � ���� ������ ����� ������ ������ �����
		TForm* pieceChooseForm;                     //����� ������ ����� ������ ������ �����
		TForm* endGameForm;                        //����� ��������� ����
		//------------------------���������� ���� �������� �������----------------------------------------------------------
		TForm* tOptionsForm;                       //����� �������� �������
		TCheckBox* gameLimitCheck;
		TCheckBox* moveLimitCheck;
		TCheckBox* sMovesLimitCheck;
		TEdit* gameLimitEdit;
		TEdit* moveLimitEdit;
		TEdit* sMovesLimitEdit;
		TEdit* per1MovesEdit;
		TEdit* per1TimeEdit;
		TEdit* per2MovesEdit;
		TEdit* per2TimeEdit;
		TEdit* finalTimeEdit;
		TEdit* addTimeEdit;
		TLabel* gameLimitLabel;
		TLabel* moveLimitLabel;
		TLabel* sMovesLimitLabel;
		TLabel* per1MovesLabel;
		TLabel* per1TimeLabel;
		TLabel* per2MovesLabel;
		TLabel* per2TimeLabel;
		TLabel* finalTimeLabel;
		TLabel* addTimeLabel;
		//-----------------------------------------------------------------------------------------------------------------------

		//------------------------����������, ������������ � ����������� ������ ���� ���������------------------------------------
		TMemo* blackTimerMemo;               //��������� ���� ������� ������
		TMemo* whiteTimerMemo;              //��������� ���� ������� �����
		TButton* whiteTimerStop;            //������ ��������� ������� �����
		TButton* blackTimerStop;            //������ ��������� ������� ������
		TButton* blackDrawOffer;            //������ ����������� ����� �������
		TButton* whiteDrawOffer;            //������ ����������� ����� ������
		TButton* blackConcede;        //������ ����� ������
		TButton* whiteConcede;        //������ ����� �����
		TMemo* movesLog;                //��������� ���� ��� ����������� ���������� �����
		TLabel* samePosInGame;          //����� ��� ������ ���������� ������������� �������
		TLabel* uselessMovesInGame;      //����� ��� ������ ���������� ����� ��� ����������� ���������
			//------------------� ������ ��������������� ������------------------------------------------------------
		TListBox* repMovesBox;          //����-���� ����� ������
		TButton* nextMainButt;          //������ ����. ���� ������
		TButton* prevMainButt;          //������ ����������� ���� ������
		TButton* playMainButt;          //������ ��������������� ������������ ������
		TButton* gotoMainButt;         //������ �������� � ����������� ���� ������
		TEdit* numberMainEdit;        //��������� ���� ��� ����� ������ ����, � �������� �������������� ������� �� ��������. ������
		TEdit* delayMainEdit;            //��������� ���� ��� ����� �������� ����� ������ ��� ������������������� ������
		TLabel* delayRepLabel;        //������� ���� ��� ����� �������� �������� ��� ��������������� ������
		TLabel* samePosLabel;         //����� ��� ������ ���������� ������������� ������� � ������ ���������. ������
		TLabel* uselessMovesLabel;     //����� ��� ������ ���������� ����� ��� ����������� ��������� � ������ ���������. ������
			//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------

		//-----------���������� ������ ���������� ��������-----------------------------------------------------
		TForm* replayControlForm;
		TButton* nextRepButt;
		TButton* prevRepButt;
		TButton* playRepButt;
		TButton* gotoRepButt;
		TEdit* numberRepEdit;
		TProgressBar* repMovesProgress;         //��������-��� ���������� ����������� �� ����� ����� ������
        TLabel* repMovesProgLabel;              //����� � ���������� � ��������-���� ����� ������
		//------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------------
		timerOptions*  tOpt;                           //������ �������� �������
		configOptions* mainOpt;

public:		// User declarations
		__fastcall TMainChessForm(TComponent* Owner);
		void delSelection();                   //�����, ��������� ����� ��������� ������
		void setSectClick();                   //�����, �������������� �������������� ������ �� ������  �� �����,
												//�������� ��������� ������
		void setSectMoves(pair<deskSection*, chessPiece*>);                  //�������� ��������� ��� ���� ���� (������) ����� ������ ������
		void setSectTakes(pair<deskSection*, chessPiece*>);                 //�������� ��������� ��� ������ ���� (������) ����� ������ ������
		void movePiece(deskSection*);               //�����, ����������� ��� �������, ������������� � ��������� ���������� ���� prevChosenSect,
													//� ����, ��������� �� ������� ���������� ����� ��������
		void takePiece(deskSection*);              //�����, ����������� ������ �������, ������������� � ��������� ���������� ���� prevChosenSect,
													//� ����, ��������� �� ������� ���������� ����� ��������
		void makeMove();                        //����� ���������� ����, �������� ����� ���� ��������������� �������
		void makeRepMove();                     //�����, ����������� ��������� ��� �� �������� ����� ������� ���. ������
		void pawnTransform();                  //�����, ������������ ����� � ������ ������ �� ���������� ������� �����������
		void newGame();                     //�����, ��������������� ������� ���� � ���������� �����
		void saveDeskNote();                //�����, ����������� ������� ��������� ����� � ������� deskNote � �������������� ����
		void restoreDeskNote(deskNote);     //����� ����������������� ����� ����������� ��������� ��������� �����
		void showEndForm(unsigned short);
	//----------------------������ ��� ������ � ����������� ������������, ���������� �����������-----------------------------------------
		void __fastcall transCursorMove(TObject* Sender, TShiftState, int X, int Y);
		void __fastcall transCursorClick(TObject* Sender);
		void __fastcall parentFormClose(TObject*);         //����� ��������� ����� �������� ���������� (��������, �������� ������)
														   //��������� ���������� � ����� � �������� ���������
		//-------------������ ��� ������ � ����� �������� �������----------------------------------------------------------------------
		void __fastcall gameLimChboxClick(TObject*);
		void __fastcall moveLimChboxClick(TObject*);
		void __fastcall sMovesLimChboxClick(TObject*);

		//-----------------------------------------------------------------------------------------------------------------------------

		//---------------������ ��� ������ � ������� (������) ���������� �������---------------------------------------------------------
		void __fastcall replayControlClose(TObject*, TCloseAction&);         //������, ���������� �� ������� OnClose ����� replayControlForm
		void __fastcall playRepButtClick(TObject*);       //�����, ����������� �� ������� OnClick ������ �������, ������������� �� ������ ���������� �������
		void __fastcall nextRepButtClick(TObject*);       //�����, ����������� �� ������� OnClick ������ ����. ����, ������������� �� ������ ���������� �������
		void __fastcall prevRepButtClick(TObject*);       //�����, ����������� �� ������� OnClick ������ ����.. ����, ������������� �� ������ ���������� �������
		void __fastcall gotoRepButtClick(TObject*);       //�����, ����������� �� ������� OnClick ������ �������� � ����(�� ������),
															//������������� �� ������ ���������� �������
		//--------------------------------------------------------------------------------------------------------------------------------

		//--------------������ ��� ������ � ������������, �������������� � ������� ���������� �������� �����------------------------------------------
        void __fastcall bTimerStopClick(TObject*);
		void __fastcall wTimerStopClick(TObject*);
		void __fastcall whiteConcedeClick(TObject*);
		void __fastcall blackConcedeClick(TObject*);
		void __fastcall whiteDrawOfferClick(TObject*);
		void __fastcall blackDrawOfferClick(TObject*);
		void __fastcall repMovesBoxDraw(TWinControl*, int, const TRect&, TOwnerDrawState);
		void __fastcall repMovesBoxClick(TObject*);
		void blackTimerMemoSet(int, AnsiString);         //����� ��� ���������� ���������� ���� blackTimerMemo ����������� ������ TMainChessForm
		void whiteTimerMemoSet(int, AnsiString);          //����� ��� ���������� ���������� ���� whiteTimerMemo ����������� ������ TMainChessForm
	//---------------------------------------------------------------------------------------------------------------------------------

		__fastcall ~TMainChessForm();                //����������, �������������� ������

};

struct moveNote {
	static unsigned moveCounter;                   //����������� ������� ���-�� ��������� ��������, ����������� �������������
	unsigned moveNumber;                           //����� ����, ������ ���������� � ������. ������������� �������� �������� moveCounter;
	static unsigned uselessCounter;                        //������� 50 ����� ��� ������ � ���� ������ (� ����� ������)
													//��������� ��� ���������� �������, �� �������� ���� ��������� ��������
	pair<chessType, deskSection> blackPiece;        //����, ��������� �� ���� ������ ������ chessType � �� ���������� ����
													//��������� chessPiece* � �������� ������� ����� ���� ������������ ������, �.�.
													 //��� ������ ������ �� ������ ���. ����� ��������������� ������ ��������� � �������������� ������
	pair<chessType, deskSection> whitePiece;       //����, ��������� �� ���� ����� ������ chessType � �� ���������� ����
	deskSection blackMoveSect;                       //�������� ��������� ������ ������
	deskSection whiteMoveSect;                       //�������� ��������� ����� ������
	bool blackShortCas;                               //���� �������� ��������� ������
	bool blackLongCas;                                //���� ������� ��������� ������
	bool whiteShortCas;                               //���� �������� ��������� �����
	bool whiteLongCas;                                //���� ������� ��������� �����
	bool blackTake;                                   //���� ������ ������ ������
	bool whiteTake;                                   //���� ������ ������ �������
	bool whiteEp;                                     //������ �� ������� ����� ������
	bool blackEp;                                     //������ �� ������� ������ ������
	bool whiteTrans;                                  //���� ����������� ����� �����
	bool blackTrans;                                  //���� ����������� ������ �����

	bool blackCheck;                                 //���� ���������� ���� �������
	bool whiteCheck;                                 //���� ���������� ���� ������
	bool blackCheckmate;                             //���� ���������� ���� �������
	bool whiteCheckmate;                             //���� ���������� ���� ������
	bool whiteStalemate;                            //���� ���������� ���� ������
	bool blackStalemate;                            //���� ���������� ���� �������
	chessType blackTransPiece;                      //��� ������ ������ ����� �����������

	chessType whiteTransPiece;                     //��� ����� ������ ����� �����������
	bool whiteDrawOffer;                                 //���� ����������� ����� ������
	bool blackDrawOffer;                           //���� ����������� ����� �������

	bool drawSign;								   //����� � ���������� ������������� ��������� ��� ����������
												   //�������� ���������, ��� ����� ��������� ������ � ������, ���� � ������ � �������� �������
												   //�������� ���������, ��� � ����� 50 ����� ��� ������ � ���� ������, ��� ����� 3 �������������
												   //�������
	bool blackWinSign;                           //���� ������ ������ �� �������, �� ��������� � ����������� ���� (��������� �������)
	bool whiteWinSign;                           //���� ������ ����� �� �������, �� ��������� � ����������� ���� (��������� �������)
	moveNote();
	string getTextMove();
	char getPieceSym(chessType) const;
	void checkUselessMove();                    //����� ���������������� �������� uselessNumber, � ��� ������, ���� ������� ��� �������� ���������
												// (�� �������� ������� � �� �������� ����� ������)
};

struct repMoveNote {
	static unsigned moveCounter;
	static unsigned currMove;
    static bool repIsFinished;              //����, ��������������� ��� ��������� ������ �� ����� (�������� ����. ���, ������� ��������� ����)
	unsigned moveNumber;
	pair<chessType, deskSection> whitePiece;
	pair<chessType, deskSection> blackPiece;
	deskSection blackMoveSect;
	deskSection whiteMoveSect;
	bool blackWinSign;
	bool whiteWinSign;
	bool whiteDrawSign;
    bool blackDrawSign;
	bool whiteDrawOffer;
	bool blackDrawOffer;
	bool whiteTake;
	bool blackTake;
	bool whiteEp;
    bool blackEp;
	chessType whiteTransPiece;
	chessType blackTransPiece;
	void setMoveText(string);                    //�����, ����������� �������� ���� � ���� ������ � ������������� �� � �������� �������
	unsigned samePosesBlack;                      //���������� ������������� ������� �� �������� ���� ������������ (����� ���� ������)
	unsigned samePosesWhite;                      //���������� ������������� ������� �� �������� ���� ������������ (����� ���� �����)
	unsigned uselessMoves;                 //���������� ����� ��� ������ ����� � ������������ ����� �� �������� ���� ������������ (����� ���� ������)
	repMoveNote();
};

struct deskNote {
	static unsigned currDesk;
	chessType chType[SECTCOUNT];
	chessColor chColor[SECTCOUNT];
	bool blackShortCasAble;             //���� ����������� �������� ��������� ������
	bool blackLongCasAble;              //���� ����������� �������� ��������� ������
	bool whiteShortCasAble;             //���� ����������� �������� ��������� �����
	bool whiteLongCasAble;              //���� ����������� ������� ��������� �����
	bool blackEpAble;                 //���� ����������� ������ �� �������  ������ ������ ����� �����
	bool whiteEpAble;                  //���� ����������� ������ �� ������� ����� ������ ������ �����
	friend bool operator ==(deskNote, deskNote);
	chessColor lastTurn;
	deskNote();

};


struct configOptions {
	class configReadError {
		string errorType;
	public:
		configReadError(string eT) : errorType(eT) {

		}
		string getErrType() const {
			return errorType;
        }

	};
	unsigned repMovesDelay;
	bool formMinimize;
	void readConfig();
	void saveConfig() const;
	vector<string> optionsVec;
    configOptions();
};




struct timerOptions {
	class timerReadError {
		string errorType;
	public:
		timerReadError(string eT) : errorType(eT) {

		}
		string getErrType() const {
			return errorType;
        }
	};
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
	timerOptions();
	void readTimerOptions();
	void __fastcall saveTimerOptions(TObject*);
	void loadDefaultOptions();
	vector<string> optionsVec;

};

class TListBoxItemInfo : public TObject {          //������� ����������� �����-��������� TObject ��� ����,
	public:                                          //����� ������� ����� ������ ������� �� �������� string � �������� Item � ����-����
		bool moveIsMade;                             // ����� ������    repMovesBox
};


//---------------------------------------------------------------------------
extern PACKAGE TMainChessForm *MainChessForm;
//---------------------------------------------------------------------------
#endif
