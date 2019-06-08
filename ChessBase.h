//---------------------------------------------------------------------------

#ifndef ChessBaseH
#define ChessBaseH

#include <map>
#include <set>
#include <list>
#include <string>
#include <algorithm>
#include <typeinfo>
#include <cstdio>

using namespace std;
enum chessColor {WHITE, BLACK, NOCOLOR};
enum chessType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NOPIECE};

extern const int DESKLEN;                   //константа, задающая ширину шахматной доски
extern const int SQRSIZE;                   //константа, задающая ширину поля (клетки)
extern const int TOPLEFTX;                 //координата X первой клетки
extern const int TOPLEFTY;                 //координата Y первой клетки
const int SECTCOUNT = 64;             //количества полей шах. доски
extern const int ADDFORMWIDTH;


struct deskSection;
class sectCompare;
class chessDesk;
class coordExistSet;
class lessSectPtr {                        //функциональный объект для сортировки указателей на объекты-клетки
											//по индексам клеток в алфавитном порядке
public:
	bool operator()(deskSection* const, deskSection* const);
};


class chessPiece {                                //Абстрактный класс шахматной фигуры
protected:
	chessType chType;                             //Тип фигуры
	chessColor chColor;                             //Цвет фигуры
	char chNumber;                               //Номер фигуры (для повторяющихся фигур)
	string picName;
	set<deskSection*, lessSectPtr> possMoves;         //множество доступных для хода фигурой клеток
	set<deskSection*, lessSectPtr> possTakes;       //множество доступных для взятия фигурой полей

public:

	chessPiece() : chType(NOPIECE), chColor(NOCOLOR),
	chNumber(0), picName("") {
	};
	chessPiece(chessColor cC, char n) : chColor(cC), chNumber(n) {
	};
	void setParameters(chessType, chessColor, char, string);
	string getPicName() const;
	chessType getChessType() const;
	chessColor getChessColor() const;
	char getChessNumber() const;
	friend bool operator==(const chessPiece&, const chessPiece&);     //перегружаем операции равенства и сравнения
	friend bool operator<(const chessPiece&, const chessPiece&);
	set<deskSection*, lessSectPtr>* getPossMoves();   // чистый виртуальный метод, возвращающий доступные для данной
																				//фигуры клетки для хода в зависимости от ее текущего положения
	set<deskSection*, lessSectPtr>* getPossTakes();
	virtual void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*) = 0;    //метод, сохраняющий доступные для хода и для взятия поля

	virtual void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*) {
	}
	void checkCheck(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);            //метод, устанавливающий попадет ли сторона под шах, если сделает ход
																							//определенной фигурой (вызывающей метод), с определенного поля (передается через аргумент),
																							//при текущем состоянии доски (передается через аргумент)
	virtual void setPrevPos(deskSection*) {

	}
	virtual deskSection* getPrevPos() const {

	}

	virtual void setIsMoved(bool) {
	}

	virtual void setCheck(chessDesk*) {
	}

	virtual void setCheck(map<deskSection*, chessPiece*, sectCompare>*) {

    }
	virtual void setCheck(bool) {

    }

	virtual bool getCheck() const {

	}

	virtual bool getIsMoved() const {

	}

	virtual ~chessPiece() {

	}

};





class Pawn : public chessPiece {
	deskSection* prevPosPtr;              //указатель на предыдущую позицию пешки (сохраняется ТОЛЬКО В ТЕЧЕНИЕ 1 ХОДА),
											//необходим для реализации взятия на проходе
	char prevPosX;                      //символьная координата предыдущей позиции пешки
	int prevPosY;                      //численная координата предыдущей позиции пешки
public:
	Pawn() : chessPiece() {
	}
	Pawn(chessColor cC, char n, char x = 0, char y = 0) : chessPiece(cC, n), prevPosX(x), prevPosY(y) {
		chType = PAWN;
		if(cC == WHITE)
			picName = "Pawn_wt.bmp";
		else if(cC == BLACK)
			picName = "Pawn_bl.bmp";
        prevPosPtr = NULL;
	}

	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);            //метод, определяющий доступные для хода и взятия
																						//поля при текущем положении фигуры и текущ. состоянием доски
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);         //метод аналогичный предыдущему, но без проверки
																					//наличия шаха после хода
	void setPrevPos(deskSection*);                              //метод, сохраняющий предыдущую позицию пешки, необходим для реализации
																//взятия через проход
	deskSection* getPrevPos() const;                            //метод, возвращающий предыдущую позицию пешки, необходим для реализации
																//взятия через проход

};

class Knight : public chessPiece {
public:
	Knight() : chessPiece() {
    }
	Knight(chessColor cC, char n) : chessPiece(cC, n) {
		chType = KNIGHT;
		if(cC == WHITE)
			picName = "Knight_wt.bmp";
		else if(cC == BLACK)
		   picName = "Knight_bl.bmp";

	}
	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
};

class Bishop : public chessPiece {
public:
	Bishop() : chessPiece() {
    }
	Bishop(chessColor cC, char n) : chessPiece(cC, n) {
		chType = BISHOP;
		if(cC == WHITE)
			picName = "Bishop_wt.bmp";
		else if(cC == BLACK)
			picName = "Bishop_bl.bmp";
	}
	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
};

class Rook : public chessPiece {
	bool isMoved;                       //флаг, принимающий значение false, если не было ни одного хода ладьей
public:                                 //и true  в противном случае (флаг необходим для реализации рокировки)
	Rook() : chessPiece() {
    }
	Rook(chessColor cC, char n) : chessPiece(cC, n) {
		chType = ROOK;
		isMoved = false;
		if(cC == WHITE)
			picName = "Rook_wt.bmp";
		else if (cC == BLACK)
			picName = "Rook_bl.bmp";

	}
	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
	void setIsMoved(bool);
	bool getIsMoved() const;

};

class Queen : public chessPiece {
public:
	Queen() : chessPiece() {
    }
	Queen(chessColor cC, char n) : chessPiece(cC, n) {
		chType = QUEEN;
		if(cC == WHITE)
			picName = "Queen_wt.bmp";
		else if(cC == BLACK)
			picName = "Queen_bl.bmp";

	}
	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
};

class King : public chessPiece {
	bool isMoved;                           //флаг, принимающий значение false, если не было ни одного хода королем,
											//и true в противном случае (флаг необходим для реализации рокировки)
	bool isChecked;                         //флаг, принимающий значение true, если королю поставлен шах
	public:
	King() : chessPiece() {

    }
	King(chessColor cC, char n) : chessPiece(cC, n) {
		chType = KING;
		isMoved = false;
        isChecked = false;
		if(cC == WHITE)
			picName = "King_wt.bmp";
		else if(cC == BLACK)
			picName = "King_bl.bmp";

	}
	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);          //метод, заполняющий множества доступных для хода
																						//и взятия полей без проверки на шах и рокировки
	void setIsMoved(bool);
	bool getIsMoved() const;
	void setCheck(chessDesk*);      //метод, устанавливающий флаг isChecked в зависимости от текущего состояния шахматной доски
												   //указатель на шахматную доску chessDesk* передается в функцию в качестве аргумента
	void setCheck(map<deskSection*, chessPiece*, sectCompare>*);         //перегрузка предыдущего метода, теперь аргуметом является отображение полей
	void setCheck(bool);                                    //перегрузка предыдущего метода с явной установкой флага isChecked в положение, передаваемое через арг.
	bool getCheck() const;


};


class Nopiece : public chessPiece {
public:
	Nopiece() : chessPiece() {

	}
	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);

};





struct deskSection {                   //структура одной ячейки поля

		char symbol;                  //символьная координата
		int number;                   //буквенная координата
		int pixelX;                   //пиксельная координата X с точкой отчета в верхнем левом углу доски
		int pixelY;                   //пиксельная координата Y с точкой отчета в верхнем левом углу доски
		chessColor sectColor;               //цвет ячейки
		bool selected;                //флаг выбора клетки (после клика)
		static const int lenX = 50;           //инициализируем статические константы длины и высоты ячеек (для всех одинаковые)
		static const int lenY = 50;          //статические константы можно инициализировать внутри класса!

		deskSection(char, int);
		deskSection() : symbol(0), number(0), sectColor(NOCOLOR), selected(false)  {

		}
 		friend bool operator ==(const deskSection, const deskSection);           //перегружаем оператор сравнения

};




class lessPiecePtr {                            //функциональный объект для сортировки по старшинству шахматных фигур
public:
	bool operator()(chessPiece* const, chessPiece* const) const;       //данный метод должен быть обязательно константным (для корректного функционирования ассоциативного массива)!

};


 class sectCompare {                                 //Создаем функциональный объект для сортировки элементов отображения sectionsMAp по ключам
public:
	bool operator()(const deskSection*, const deskSection*) const;
};

class equalSectPtr {
	deskSection* sectPtr;
public:
	equalSectPtr(deskSection* s) : sectPtr(s) {

	}
	bool operator()(const deskSection*) const;
};

class isCoordInSect {
	int coordX;
	int coordY;
public:
	isCoordInSect(int x, int y) : coordX(x), coordY(y) {

	}
	bool operator()(const pair<deskSection*, chessPiece*>) const;              //в отличие от функциональных объектов, используемых при инициализации отображения,
};                                                                       //собственные функциональные объекты, передаваемые в алгоритм,
																		// принимают в качестве аргумента всю пару, А НЕ ТОЛЬКО КЛЮЧ!


class sectEnable {                                                    //функциональный объект для поиска пары deskSection*, chessPiece*
																	//по символьной и числовой координатах deskSection, а также
																	//по факту отсутствия на данной клетке фигуры ChessPiece
		char sym;
		int num;
public:
	sectEnable(char s, int n) : sym(s), num(n) {

	}
	bool operator()(const pair<deskSection*, chessPiece*>) const;
};

class sectTakeEnable {                                           //функциональный объект для поска пары pair<deskSection*, chessPiece*>
																//по координатам ячейки deskSection* по наличию фигуры  chessPiece и по
	char sym;                                                   //ее цвету
	int num;
	chessColor chColor;
public:
	sectTakeEnable(char s, int n, chessColor c) : sym(s), num(n), chColor(c) {

	}
	bool operator()(const pair<deskSection*, chessPiece*>) const;
};

class piecePairEqual {                                          //функциональный объект поиска пары pair<chessPiece*, deskSection*>,
	chessPiece* piecePtr;                                       //ключ которой равен заданному указателю *piecePtr
public:
	piecePairEqual(chessPiece* pPtr) : piecePtr(pPtr) {

    }
	bool operator()(const pair<chessPiece*, deskSection*>) const;
};

class sectCoordEqual {                                      //функциональный объект для поиска пары pair<deskSection*, chessPiece*>
		char sym;
		int num;
public:
		sectCoordEqual(char s, int n) : sym(s), num(n) {
		}
		bool operator()(const pair<deskSection*, chessPiece*>) const;
};

class pieceCoordEqual {
	char sym;
	int num;
public:
	pieceCoordEqual(char s, int n) : sym(s), num(n) {

	}
    bool operator()(const pair<chessPiece*, deskSection*>) const;
};

class coordExistSet {
	char sym;
	int num;
public:
	coordExistSet(char s, int n) : sym(s), num(n) {

	}
    bool operator()(const deskSection*) const;
};



class chessDesk {
		Pawn pawnBlackArr[8];                           //массив объектов- черных пешек
		Knight knightBlackArr[2];                       //и т.д.
		Bishop bishopBlackArr[2];                       //данные массивы необходимы, т.к. в отображении SectionsMap работать
		Rook rookBlackArr[2];                           //проще и эффективнее с указателями на объекты, а не с самими объектами,
		Queen queenBlack;                               //а знчит, требуется их где-то хранить
		King kingBlack;

		Pawn pawnWhiteArr[8];
		Knight knightWhiteArr[2];
		Bishop bishopWhiteArr[2];
		Rook rookWhiteArr[2];
		Queen queenWhite;
		King kingWhite;

		deskSection dSect[8][8];                    //массив ячеек
		Nopiece noPiece;                            //объект "пустой" фигуры
		map<deskSection*, chessPiece*, sectCompare> sectionsMap;   //отображение, в котором ключами являются объекты-ячейки,
															   // значениями - указатели объекты-шахматы
		map<deskSection*, chessPiece*, sectCompare>::iterator sectionsIt;	//итератор отображения
		map<chessPiece*, deskSection*, lessPiecePtr> whiteChess;         //набор белых фигур, ключи - тип фигуры, сортируются по старшинству
		map<chessPiece*, deskSection*, lessPiecePtr> blackChess;        //набор черных фигур
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator piecesIt;
		list<chessPiece*> addWhitePieces;                  //список указателей на дополнительные белые фигуры (после превр. пешки)
		list<chessPiece*> addBlackPieces;                 //список указателей на дополнительные черные фигуры
		list<chessPiece*>::iterator addPiecesIt;          //итератор списака указателей на дополнительные фигуры

public:
		chessDesk();
	 	~chessDesk();
		pair<deskSection*, chessPiece*>* getSection();           //функция возвращает пару из указателя на объект-шахматную фигуру
															//и указателя на объект ячейку


		pair<deskSection*, chessPiece*> findSection(int, int);         //метод поиска ячейки с заданными координатами (попиксельно)
																	// возвращает указатель на пару из отображения
		pair<deskSection*, chessPiece*> getSection(char, int);      // метод возвращает пару по координатам шахматного поля

	   map<deskSection*, chessPiece*, sectCompare>* getMap();                  //метод для возврата указателя на отображение  sectionsMap
	   map<chessPiece*, deskSection*, lessPiecePtr>* getWhiteChess();         //метод возвращает указатель на отображение белых фигур
	   map<chessPiece*, deskSection*, lessPiecePtr>* getBlackChess();         //метод возвращает указатель на отображение черных фигур


	   Nopiece* getNopiece();                                          //метод возвращает указатель на объект-пустую фигуру
	   chessPiece* pieceFromArr(chessColor, chessType, int);                       //метод возвращающий указатель на шахматную фигуру и массива
	   deskSection* sectFromArr(char, int);
	   void clearAddPiece();                                          //метод, высвобождающий память, занимаемую дополнительными фигурами
																	//после их взятия
	   void addPiece(chessPiece*);                                 //метод добавляющий дополнительную фигуру после превращения пешки, при
																	//условии наличия основных фигур
	   bool addNumberExist(int, chessColor);                       //метод, проверяющий существует ли фигура с заданным номером, заданного цвета
	   void startDesk();
	   void sectItBegin();                                     //метод, устанавливающий итератор sectionsIt в начало отображения sectionsMap
};






#endif
