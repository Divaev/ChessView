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
#include "ChessPaint.h"               //добавляем заголовочный файл объекта для отрисовки доски и фигур
				//Если в одном из собственных заголовочных файлов есть директива  using namespace std,
				//то добавлять этот заголовочный файл (и все файлы его включающие) нужно ПОСЛЕ СТАНДАРТНЫХ
				//ЗАГОЛОВОЧНЫХ ФАЙЛОВ (так, чтобы во всех файлах директива была после всех включений станд. заголовков!)
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
	void __fastcall replayTimerTimer(TObject *Sender);                                   //Общий таймер


private:	// User declarations
		bool whiteChecked;                //флаг, принимающий значение true, если белому королю поставлен шах
		bool blackChecked;               //флаг, принимающий значение true, если черному королю поставлен шах
		bool firstMoveMade;             //флаг первого хода партии (достаточно хода белых)
        bool resultIsShowed;            //флаг, устанавливаемый при выведении окна результата игры
		chessDeskPaint*  chDskPaint;             //указатель на объект отрисовки
		deskSection* prevSect;                                  //последняя клетка, на которую указывал курсор
                                                    //используется обработчиками событий клика и движения курсора
													//SectSelect() и CursorMove() соответственно
		deskSection* prevChosenSect;              //последняя выбранная клетка выделяемая полупрозрачным спрайтом
		set<deskSection*, lessSectPtr>* possMovesPtr;        // указатель на множество доступных для хода полей для последней выбранной ячейки
		set<deskSection*, lessSectPtr>::iterator possMovesIt;      //итератор множества доступных для хода полей
		set<deskSection*, lessSectPtr>* possTakesPtr;               //указатель на множество доступных для взятия полей последней выбранной клетки
		set<deskSection*, lessSectPtr>::iterator possTakesIt;      //итератор множество доступных для взятия полей
		chessColor currTurn;                                //Поле, определяющее, с какой стороны должен быть сделан ход (белые\черные)
		chessType choosePiece;                             //тип фигуры, полученной  после трансформации пешки
		vector<moveNote> moveNotes;                     //вектор, хранящий объекты типа moveNote, необходим для сохранения
														//партии в тектовом виде
		vector<deskNote> deskNotes;                     //вектор, хранящий состояния доски после каждого хода (черных и белых)
														// в низкоуровневом виде
		vector<repMoveNote> repMoveNotes;               //вектор, хранящий объекты - ходы повтора партии

		unsigned sameDesksCount;
		int blackTotalTime;                        //суммарное время, отсчитанное таймером черных
		int whiteTotalTime;                        //суммарное время, отсчитанное таймером белых
//---------------Визуальные компоненты, создаваемые динамически-------------------------------------------------------------------
		TImage* pieceChooseImage;                      //компонент для отображения изображений в окне выбора новой фигуры вместо пешки
		TForm* pieceChooseForm;                     //форма выбора новой фигуры вместо пешки
		TForm* endGameForm;                        //форма окончания игры
		//------------------------Компоненты окна настроек таймера----------------------------------------------------------
		TForm* tOptionsForm;                       //форма настроек таймера
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

		//------------------------Компоненты, отображаемом в расширенном режиме окна программы------------------------------------
		TMemo* blackTimerMemo;               //текстовое поле таймера черных
		TMemo* whiteTimerMemo;              //текстовое поле таймера белых
		TButton* whiteTimerStop;            //кнопка остановки таймера белых
		TButton* blackTimerStop;            //кнопка остановки таймера черных
		TButton* blackDrawOffer;            //кнопка предложения ничьи черными
		TButton* whiteDrawOffer;            //кнопка предложения ничьи белыми
		TButton* blackConcede;        //кнопка сдачи черных
		TButton* whiteConcede;        //кнопка сдачи белых
		TMemo* movesLog;                //текстовое поле для отображения записанных ходов
		TLabel* samePosInGame;          //метка для вывода количества повторяющихся позиций
		TLabel* uselessMovesInGame;      //метка для вывода количества ходов без необратимых изменений
			//------------------В режиме воспроизведения реплея------------------------------------------------------
		TListBox* repMovesBox;          //лист-бокс ходов реплея
		TButton* nextMainButt;          //кнопка след. хода реплея
		TButton* prevMainButt;          //кнопка предыдущего хода реплея
		TButton* playMainButt;          //кнопка автоматического проигрывания реплея
		TButton* gotoMainButt;         //кнопка перехода к конкретному ходу партии
		TEdit* numberMainEdit;        //текстовое поле для ввода номера хода, к которому осуществляется переход по вышеуказ. кнопке
		TEdit* delayMainEdit;            //текстовое поле для ввода задержки между ходами при автовоспроизведении реплея
		TLabel* delayRepLabel;        //текстое поле для ввода значения задержки при воспроизведении реплея
		TLabel* samePosLabel;         //метка для вывода количества повторяющихся позиций в режиме воспроизв. реплея
		TLabel* uselessMovesLabel;     //метка для вывода количества ходов без необратимых изменений в режиме воспроизв. реплея
			//------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------

		//-----------Компоненты панели управления реплеями-----------------------------------------------------
		TForm* replayControlForm;
		TButton* nextRepButt;
		TButton* prevRepButt;
		TButton* playRepButt;
		TButton* gotoRepButt;
		TEdit* numberRepEdit;
		TProgressBar* repMovesProgress;         //прогресс-бар выполнения прочитанных из файла ходов реплея
        TLabel* repMovesProgLabel;              //метка с пояснением к прогресс-бара ходов реплея
		//------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------------
		timerOptions*  tOpt;                           //объект настроек таймера
		configOptions* mainOpt;

public:		// User declarations
		__fastcall TMainChessForm(TComponent* Owner);
		void delSelection();                   //метод, удаляющий рамку выделения клетки
		void setSectClick();                   //метод, отрисовывающий полупрозрачный спрайт на клетке  по клику,
												//выделяет выбранную клетку
		void setSectMoves(pair<deskSection*, chessPiece*>);                  //выделяет доступные для хода поля (клетки) после выбора клетки
		void setSectTakes(pair<deskSection*, chessPiece*>);                 //выделяет доступные для взятия поля (клетки) после выбора клетки
		void movePiece(deskSection*);               //метод, выполняющий ход фигурой, расположенной в последнем выделенном поле prevChosenSect,
													//в поле, указатель на которое передается через аргумент
		void takePiece(deskSection*);              //метод, выполняющий взятие фигурой, расположенной в последнем выделенном поле prevChosenSect,
													//в поле, указатель на которое передается через аргумент
		void makeMove();                        //метод завершения хода, передает право хода противоположной стороне
		void makeRepMove();                     //метод, выполняющий следующий ход из вектрора ходов повтора шах. партии
		void pawnTransform();                  //метод, превращающий пешку в другую фигуру по достижению крайней горизонтали
		void newGame();                     //метод, останавливающий текущую игру и начинающий новую
		void saveDeskNote();                //метод, сохраняющий текущее состояние доски в объекте deskNote в низкоуровневом виде
		void restoreDeskNote(deskNote);     //метод восстанавливающий ранее сохраненное состояние шахматной доски
		void showEndForm(unsigned short);
	//----------------------Методы для работы с визуальными компонентами, созданными динамически-----------------------------------------
		void __fastcall transCursorMove(TObject* Sender, TShiftState, int X, int Y);
		void __fastcall transCursorClick(TObject* Sender);
		void __fastcall parentFormClose(TObject*);         //метод закрывает форму родителя компонента (например, родителя кнопки)
														   //компонент передается в метод в качестве аргумента
		//-------------Методы для работы с окном настроек таймера----------------------------------------------------------------------
		void __fastcall gameLimChboxClick(TObject*);
		void __fastcall moveLimChboxClick(TObject*);
		void __fastcall sMovesLimChboxClick(TObject*);

		//-----------------------------------------------------------------------------------------------------------------------------

		//---------------Методы для работы с панелью (формой) управления реплеем---------------------------------------------------------
		void __fastcall replayControlClose(TObject*, TCloseAction&);         //методы, вызываемый по событию OnClose формы replayControlForm
		void __fastcall playRepButtClick(TObject*);       //метод, выполняемый по событию OnClick кнопки запуска, расположенной на панели управления реплеем
		void __fastcall nextRepButtClick(TObject*);       //метод, выполняемый по событию OnClick кнопки след. хода, расположенной на панели управления реплеем
		void __fastcall prevRepButtClick(TObject*);       //метод, выполняемый по событию OnClick кнопки пред.. хода, расположенной на панели управления реплеем
		void __fastcall gotoRepButtClick(TObject*);       //метод, выполняемый по событию OnClick кнопки перехода к ходу(по номеру),
															//расположенной на панели управления реплеем
		//--------------------------------------------------------------------------------------------------------------------------------

		//--------------Методы для работы с компонентами, расположенными в области расширения основной формы------------------------------------------
        void __fastcall bTimerStopClick(TObject*);
		void __fastcall wTimerStopClick(TObject*);
		void __fastcall whiteConcedeClick(TObject*);
		void __fastcall blackConcedeClick(TObject*);
		void __fastcall whiteDrawOfferClick(TObject*);
		void __fastcall blackDrawOfferClick(TObject*);
		void __fastcall repMovesBoxDraw(TWinControl*, int, const TRect&, TOwnerDrawState);
		void __fastcall repMovesBoxClick(TObject*);
		void blackTimerMemoSet(int, AnsiString);         //метод для заполнения текстового поля blackTimerMemo запределами класса TMainChessForm
		void whiteTimerMemoSet(int, AnsiString);          //метод для заполнения текстового поля whiteTimerMemo запределами класса TMainChessForm
	//---------------------------------------------------------------------------------------------------------------------------------

		__fastcall ~TMainChessForm();                //деструктор, высвобождающий память

};

struct moveNote {
	static unsigned moveCounter;                   //статический счетчик кол-ва созданных объектов, итерируется конструктором
	unsigned moveNumber;                           //номер хода, данной переменной в констр. присваивается текущиее значение moveCounter;
	static unsigned uselessCounter;                        //счетчик 50 ходов без взятия и хода пешкой (с обеих сторон)
													//необходим для реализации правила, по которому матч считается ничейным
	pair<chessType, deskSection> blackPiece;        //пара, состоящая из типа черной фигуры chessType и ее начального поля
													//указатель chessPiece* в качестве первого члена пары использовать нельзя, т.к.
													 //при взятии фигуры из списка доп. фигур соответствующий объект удаляется с высвобождением памяти
	pair<chessType, deskSection> whitePiece;       //пара, состоящая из типа белой фигуры chessType и ее начального поля
	deskSection blackMoveSect;                       //конечное положение черной фигуры
	deskSection whiteMoveSect;                       //конечное положение белой фигуры
	bool blackShortCas;                               //флаг короткой рокировки черных
	bool blackLongCas;                                //флаг длинной рокировки черных
	bool whiteShortCas;                               //флаг короткой рокировки белых
	bool whiteLongCas;                                //флаг длинной рокировки белых
	bool blackTake;                                   //флаг взятия фигуры белыми
	bool whiteTake;                                   //флаг взятия фигуры черными
	bool whiteEp;                                     //взятие на проходе белой пешкой
	bool blackEp;                                     //взятие на проходе черной пешкой
	bool whiteTrans;                                  //флаг превращения белой пешки
	bool blackTrans;                                  //флаг превращения черной пешки

	bool blackCheck;                                 //флаг постановки шаха черными
	bool whiteCheck;                                 //флаг постановки мата белыми
	bool blackCheckmate;                             //флаг постановки мата черными
	bool whiteCheckmate;                             //флаг постановки мата белыми
	bool whiteStalemate;                            //флаг постановки пата белыми
	bool blackStalemate;                            //флаг постановки пата черными
	chessType blackTransPiece;                      //тип черной фигуры после превращения

	chessType whiteTransPiece;                     //тип белой фигуры после превращения
	bool whiteDrawOffer;                                 //флаг предложения ничьи белыми
	bool blackDrawOffer;                           //флаг предложения ничьи черными

	bool drawSign;								   //ничья в результате невозможности поставить мат вследствие
												   //нехватки материала, или после истечения врмени в случае, если у игрока с остатком времени
												   //нехватка материала, или в после 50 ходов без взятия и хода пешкой, или после 3 повторяющихся
												   //позиций
	bool blackWinSign;                           //флаг победы черных по причине, не связанной с постановкой мата (истечение времени)
	bool whiteWinSign;                           //флаг победы белых по причине, не связанной с постановкой мата (истечение времени)
	moveNote();
	string getTextMove();
	char getPieceSym(chessType) const;
	void checkUselessMove();                    //метод инурементирующий свойство uselessNumber, в том случем, если текущий ход является пассивным
												// (не является взятием и не является ходом пешкой)
};

struct repMoveNote {
	static unsigned moveCounter;
	static unsigned currMove;
    static bool repIsFinished;              //флаг, устанавливаемый при просмотре реплея до конца (выполнен посл. ход, получен результат игры)
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
	void setMoveText(string);                    //метод, принимающий описание хода в виде строки и преобразующий их в свойства объекта
	unsigned samePosesBlack;                      //количество повторяющихся позиций до текущего хода включительно (после хода черных)
	unsigned samePosesWhite;                      //количество повторяющихся позиций до текущего хода включительно (после хода белых)
	unsigned uselessMoves;                 //количество ходов без взятия фигур и передвижения пешек до текущего хода включительно (после хода черных)
	repMoveNote();
};

struct deskNote {
	static unsigned currDesk;
	chessType chType[SECTCOUNT];
	chessColor chColor[SECTCOUNT];
	bool blackShortCasAble;             //флаг доступности короткой рокировки черных
	bool blackLongCasAble;              //флаг доступности длянниой рокировки черных
	bool whiteShortCasAble;             //флаг доступности короткой рокировки белых
	bool whiteLongCasAble;              //флаг доступности длинной рокировки белых
	bool blackEpAble;                 //флаг возможности взятия на проходе  черной пешкой белую пешку
	bool whiteEpAble;                  //флаг возможности взятия на проходе белой пешкой черную пешку
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

class TListBoxItemInfo : public TObject {          //создаем собственный класс-наследник TObject для того,
	public:                                          //чтобы объекты этого класса связать со строками string и добавить Item в лист-бокс
		bool moveIsMade;                             // ходов реплея    repMovesBox
};


//---------------------------------------------------------------------------
extern PACKAGE TMainChessForm *MainChessForm;
//---------------------------------------------------------------------------
#endif
