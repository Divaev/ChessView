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

extern const int DESKLEN;                   //���������, �������� ������ ��������� �����
extern const int SQRSIZE;                   //���������, �������� ������ ���� (������)
extern const int TOPLEFTX;                 //���������� X ������ ������
extern const int TOPLEFTY;                 //���������� Y ������ ������
const int SECTCOUNT = 64;             //���������� ����� ���. �����
extern const int ADDFORMWIDTH;


struct deskSection;
class sectCompare;
class chessDesk;
class coordExistSet;
class lessSectPtr {                        //�������������� ������ ��� ���������� ���������� �� �������-������
											//�� �������� ������ � ���������� �������
public:
	bool operator()(deskSection* const, deskSection* const);
};


class chessPiece {                                //����������� ����� ��������� ������
protected:
	chessType chType;                             //��� ������
	chessColor chColor;                             //���� ������
	char chNumber;                               //����� ������ (��� ������������� �����)
	string picName;
	set<deskSection*, lessSectPtr> possMoves;         //��������� ��������� ��� ���� ������� ������
	set<deskSection*, lessSectPtr> possTakes;       //��������� ��������� ��� ������ ������� �����

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
	friend bool operator==(const chessPiece&, const chessPiece&);     //����������� �������� ��������� � ���������
	friend bool operator<(const chessPiece&, const chessPiece&);
	set<deskSection*, lessSectPtr>* getPossMoves();   // ������ ����������� �����, ������������ ��������� ��� ������
																				//������ ������ ��� ���� � ����������� �� �� �������� ���������
	set<deskSection*, lessSectPtr>* getPossTakes();
	virtual void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*) = 0;    //�����, ����������� ��������� ��� ���� � ��� ������ ����

	virtual void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*) {
	}
	void checkCheck(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);            //�����, ��������������� ������� �� ������� ��� ���, ���� ������� ���
																							//������������ ������� (���������� �����), � ������������� ���� (���������� ����� ��������),
																							//��� ������� ��������� ����� (���������� ����� ��������)
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
	deskSection* prevPosPtr;              //��������� �� ���������� ������� ����� (����������� ������ � ������� 1 ����),
											//��������� ��� ���������� ������ �� �������
	char prevPosX;                      //���������� ���������� ���������� ������� �����
	int prevPosY;                      //��������� ���������� ���������� ������� �����
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

	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);            //�����, ������������ ��������� ��� ���� � ������
																						//���� ��� ������� ��������� ������ � �����. ���������� �����
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);         //����� ����������� �����������, �� ��� ��������
																					//������� ���� ����� ����
	void setPrevPos(deskSection*);                              //�����, ����������� ���������� ������� �����, ��������� ��� ����������
																//������ ����� ������
	deskSection* getPrevPos() const;                            //�����, ������������ ���������� ������� �����, ��������� ��� ����������
																//������ ����� ������

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
	bool isMoved;                       //����, ����������� �������� false, ���� �� ���� �� ������ ���� ������
public:                                 //� true  � ��������� ������ (���� ��������� ��� ���������� ���������)
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
	bool isMoved;                           //����, ����������� �������� false, ���� �� ���� �� ������ ���� �������,
											//� true � ��������� ������ (���� ��������� ��� ���������� ���������)
	bool isChecked;                         //����, ����������� �������� true, ���� ������ ��������� ���
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
	void noCheckMT(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);          //�����, ����������� ��������� ��������� ��� ����
																						//� ������ ����� ��� �������� �� ��� � ���������
	void setIsMoved(bool);
	bool getIsMoved() const;
	void setCheck(chessDesk*);      //�����, ��������������� ���� isChecked � ����������� �� �������� ��������� ��������� �����
												   //��������� �� ��������� ����� chessDesk* ���������� � ������� � �������� ���������
	void setCheck(map<deskSection*, chessPiece*, sectCompare>*);         //���������� ����������� ������, ������ ��������� �������� ����������� �����
	void setCheck(bool);                                    //���������� ����������� ������ � ����� ���������� ����� isChecked � ���������, ������������ ����� ���.
	bool getCheck() const;


};


class Nopiece : public chessPiece {
public:
	Nopiece() : chessPiece() {

	}
	void setMovesTakes(deskSection*, map<deskSection*, chessPiece*, sectCompare>*);

};





struct deskSection {                   //��������� ����� ������ ����

		char symbol;                  //���������� ����������
		int number;                   //��������� ����������
		int pixelX;                   //���������� ���������� X � ������ ������ � ������� ����� ���� �����
		int pixelY;                   //���������� ���������� Y � ������ ������ � ������� ����� ���� �����
		chessColor sectColor;               //���� ������
		bool selected;                //���� ������ ������ (����� �����)
		static const int lenX = 50;           //�������������� ����������� ��������� ����� � ������ ����� (��� ���� ����������)
		static const int lenY = 50;          //����������� ��������� ����� ���������������� ������ ������!

		deskSection(char, int);
		deskSection() : symbol(0), number(0), sectColor(NOCOLOR), selected(false)  {

		}
 		friend bool operator ==(const deskSection, const deskSection);           //����������� �������� ���������

};




class lessPiecePtr {                            //�������������� ������ ��� ���������� �� ����������� ��������� �����
public:
	bool operator()(chessPiece* const, chessPiece* const) const;       //������ ����� ������ ���� ����������� ����������� (��� ����������� ���������������� �������������� �������)!

};


 class sectCompare {                                 //������� �������������� ������ ��� ���������� ��������� ����������� sectionsMAp �� ������
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
	bool operator()(const pair<deskSection*, chessPiece*>) const;              //� ������� �� �������������� ��������, ������������ ��� ������������� �����������,
};                                                                       //����������� �������������� �������, ������������ � ��������,
																		// ��������� � �������� ��������� ��� ����, � �� ������ ����!


class sectEnable {                                                    //�������������� ������ ��� ������ ���� deskSection*, chessPiece*
																	//�� ���������� � �������� ����������� deskSection, � �����
																	//�� ����� ���������� �� ������ ������ ������ ChessPiece
		char sym;
		int num;
public:
	sectEnable(char s, int n) : sym(s), num(n) {

	}
	bool operator()(const pair<deskSection*, chessPiece*>) const;
};

class sectTakeEnable {                                           //�������������� ������ ��� ����� ���� pair<deskSection*, chessPiece*>
																//�� ����������� ������ deskSection* �� ������� ������  chessPiece � ��
	char sym;                                                   //�� �����
	int num;
	chessColor chColor;
public:
	sectTakeEnable(char s, int n, chessColor c) : sym(s), num(n), chColor(c) {

	}
	bool operator()(const pair<deskSection*, chessPiece*>) const;
};

class piecePairEqual {                                          //�������������� ������ ������ ���� pair<chessPiece*, deskSection*>,
	chessPiece* piecePtr;                                       //���� ������� ����� ��������� ��������� *piecePtr
public:
	piecePairEqual(chessPiece* pPtr) : piecePtr(pPtr) {

    }
	bool operator()(const pair<chessPiece*, deskSection*>) const;
};

class sectCoordEqual {                                      //�������������� ������ ��� ������ ���� pair<deskSection*, chessPiece*>
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
		Pawn pawnBlackArr[8];                           //������ ��������- ������ �����
		Knight knightBlackArr[2];                       //� �.�.
		Bishop bishopBlackArr[2];                       //������ ������� ����������, �.�. � ����������� SectionsMap ��������
		Rook rookBlackArr[2];                           //����� � ����������� � ����������� �� �������, � �� � ������ ���������,
		Queen queenBlack;                               //� �����, ��������� �� ���-�� �������
		King kingBlack;

		Pawn pawnWhiteArr[8];
		Knight knightWhiteArr[2];
		Bishop bishopWhiteArr[2];
		Rook rookWhiteArr[2];
		Queen queenWhite;
		King kingWhite;

		deskSection dSect[8][8];                    //������ �����
		Nopiece noPiece;                            //������ "������" ������
		map<deskSection*, chessPiece*, sectCompare> sectionsMap;   //�����������, � ������� ������� �������� �������-������,
															   // ���������� - ��������� �������-�������
		map<deskSection*, chessPiece*, sectCompare>::iterator sectionsIt;	//�������� �����������
		map<chessPiece*, deskSection*, lessPiecePtr> whiteChess;         //����� ����� �����, ����� - ��� ������, ����������� �� �����������
		map<chessPiece*, deskSection*, lessPiecePtr> blackChess;        //����� ������ �����
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator piecesIt;
		list<chessPiece*> addWhitePieces;                  //������ ���������� �� �������������� ����� ������ (����� �����. �����)
		list<chessPiece*> addBlackPieces;                 //������ ���������� �� �������������� ������ ������
		list<chessPiece*>::iterator addPiecesIt;          //�������� ������� ���������� �� �������������� ������

public:
		chessDesk();
	 	~chessDesk();
		pair<deskSection*, chessPiece*>* getSection();           //������� ���������� ���� �� ��������� �� ������-��������� ������
															//� ��������� �� ������ ������


		pair<deskSection*, chessPiece*> findSection(int, int);         //����� ������ ������ � ��������� ������������ (�����������)
																	// ���������� ��������� �� ���� �� �����������
		pair<deskSection*, chessPiece*> getSection(char, int);      // ����� ���������� ���� �� ����������� ���������� ����

	   map<deskSection*, chessPiece*, sectCompare>* getMap();                  //����� ��� �������� ��������� �� �����������  sectionsMap
	   map<chessPiece*, deskSection*, lessPiecePtr>* getWhiteChess();         //����� ���������� ��������� �� ����������� ����� �����
	   map<chessPiece*, deskSection*, lessPiecePtr>* getBlackChess();         //����� ���������� ��������� �� ����������� ������ �����


	   Nopiece* getNopiece();                                          //����� ���������� ��������� �� ������-������ ������
	   chessPiece* pieceFromArr(chessColor, chessType, int);                       //����� ������������ ��������� �� ��������� ������ � �������
	   deskSection* sectFromArr(char, int);
	   void clearAddPiece();                                          //�����, �������������� ������, ���������� ��������������� ��������
																	//����� �� ������
	   void addPiece(chessPiece*);                                 //����� ����������� �������������� ������ ����� ����������� �����, ���
																	//������� ������� �������� �����
	   bool addNumberExist(int, chessColor);                       //�����, ����������� ���������� �� ������ � �������� �������, ��������� �����
	   void startDesk();
	   void sectItBegin();                                     //�����, ��������������� �������� sectionsIt � ������ ����������� sectionsMap
};






#endif
