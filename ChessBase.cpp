//---------------------------------------------------------------------------

//#pragma hdrstop

#include "ChessBase.h"
//---------------------------------------------------------------------------
//#pragma package(smart_init)

 extern const int DESKLEN = 8;
 extern const int SQRSIZE = 100;
 extern const int TOPLEFTX = 50;
 extern const int TOPLEFTY = 50;
 extern const int ADDFORMWIDTH = 300;


 chessDesk::chessDesk() {

													   //�������������� ������� ��������
				int pb = 0, pw =  0;                   //������� ��������-�����
				int kb = 0, kw = 0;                     //������� ��������-�����
				int bb = 0, bw = 0;                     //������� ��������-������
				int rb = 0, rw = 0;                     //������� ��������-�����



			   for(int i=0; i < DESKLEN; i++)               //���������� ��� ��-�� ���������� ���� 8�8
					for(int j=0; j < DESKLEN; j++) {
					 //	deskSection* dSptr = new deskSection(i, j);       //������� ����� ������-������
					 //	dSect[i][j] = *dSptr;                           //������� ��������� ������-������ � ������
						dSect[i][j] = deskSection(i,  j);                //�������������� �������� ������� ��������� �����
						deskSection* dSptr = *(dSect+i)+j;              //���������  ��������� ��������� �� ������ ���. ����
																		//� �������������� ��� ������� ��-�� ���������� ������� dSect
						if(j > 1 && j < 6) {                            //��� ������ ����� (��� ��������� ����������� ������)

							pair<deskSection*, chessPiece*> p(dSptr, &noPiece);     //���������� ���� �� ���������� �������-������ � ������ ��������� ������
							sectionsMap.insert(p);                                   //��������� ��������� ���� � �����������
						}
 //----------------------------������� � ���� �����---------------------------------------------------------------------------------------
						else if(j == 1 || j == 6) {
								if(j == 1) {
										pawnBlackArr[pb] = Pawn(BLACK, pb+1);           //����������� ��-�� ������� �������� ������ ����� ���������� ������
																						//� ����������� ����� ��������� ������ � �� ������
										chessPiece* piecePtr = (pawnBlackArr + pb);      //������� ��������� �� ������-�����, �������� ������ � ��������������

										pb++;                                            //�������������� ����� �������� �������
										pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);   //������� ���� � ��������-������� � ��������-�������
										sectionsMap.insert(p1);                     // ������� ��������� ���� � �����������
										pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);    //������� ���� � ��������-������� � ��������-�������
										blackChess.insert(p2);                                //������� ���� � ���� ������ �����, ������ �������� ��������� �� ������
								}
								else if(j == 6) {                                 //���������� ��� ���� ����� �����
										pawnWhiteArr[pw] = Pawn(WHITE, pw+1);
										chessPiece* piecePtr = (pawnWhiteArr + pw);
										pw++;
										pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
										sectionsMap.insert(p1);
										pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
                                        whiteChess.insert(p2);
								}
						}
  //-----------------------------������� � ���� �����--------------------------------------------------------------------------------------------------
						else if(j == 0 && (i == 1 || i == 6)) {         //������ ����
							knightBlackArr[kb] = Knight(BLACK, kb+1);
							chessPiece* piecePtr = (knightBlackArr + kb);
							kb++;
							pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
							sectionsMap.insert(p1);
							pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
							blackChess.insert(p2);
						}

						else if(j  == 7 && (i == 1 || i == 6)) {
							knightWhiteArr[kw] = Knight(WHITE, kw+1);
							chessPiece* piecePtr = (knightWhiteArr + kw);
							kw++;
							pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
							sectionsMap.insert(p1);
							pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
							whiteChess.insert(p2);
						}
 //---------------------------������� � ���� ������ ������---------------------------------------------------------------------------------------
						else if(j == 0 && (i == 2 || i == 5)) {
							bishopBlackArr[bb] = Bishop(BLACK, bb+1);
							chessPiece* piecePtr = (bishopBlackArr + bb);
							bb++;
							pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
							sectionsMap.insert(p1);
							pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
							blackChess.insert(p2);
						}
						else if(j == 7 && (i == 2 || i == 5)) {
								bishopWhiteArr[bw] = Bishop(WHITE, bw+1);
								chessPiece* piecePtr = (bishopWhiteArr + bw);
								bw++;
								pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
								sectionsMap.insert(p1);
								pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
								whiteChess.insert(p2);
						}
//-----------------------------������� � ���� �����----------------------------------------------------------------------------------------------
						else if(j == 0 && (i == 0 || i == 7)) {
								rookBlackArr[rb] = Rook(BLACK, rb+1);
								chessPiece* piecePtr = (rookBlackArr + rb);
								rb++;
								pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
								sectionsMap.insert(p1);
								pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
								blackChess.insert(p2);
						}
						else if(j == 7 && (i == 0 || i == 7)) {
								rookWhiteArr[rw] = Rook(WHITE, rw+1);
								chessPiece* piecePtr = (rookWhiteArr + rw);
								rw++;
                                pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
								sectionsMap.insert(p1);
								pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
								whiteChess.insert(p2);
						}
//----------------------������� � ���� ������--------------------------------------------------------------------------------------
						else if(j == 0 && i == 3) {
							queenBlack = Queen(BLACK, 1);
							chessPiece* piecePtr = &queenBlack;
							pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
							sectionsMap.insert(p1);
							pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
							blackChess.insert(p2);
						}

						else if(j == 7 && i == 3) {
								queenWhite = Queen(WHITE, 1);
								chessPiece* piecePtr = &queenWhite;
								pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
								sectionsMap.insert(p1);
								pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
								whiteChess.insert(p2);
						}
//--------------------------------------������� � ���� �������---------------------------------------------------------------------
						else if(j == 0 && i == 4) {
								kingBlack = King(BLACK, 1);
								chessPiece* piecePtr = &kingBlack;
								pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
								sectionsMap.insert(p1);
								pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
								blackChess.insert(p2);
						}

						else if(j == 7 && i == 4) {
								kingWhite = King(WHITE, 1);
								chessPiece* piecePtr = &kingWhite;
								pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
								sectionsMap.insert(p1);
								pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
								whiteChess.insert(p2);
						}


					}
			   sectionsIt = sectionsMap.begin();                //������������� �������� � ������ �����������
 }

  void chessDesk::startDesk() {

	sectionsMap.clear();
	sectionsIt = sectionsMap.begin();
	blackChess.clear();
	whiteChess.clear();
	for(addPiecesIt = addWhitePieces.begin(); addPiecesIt != addWhitePieces.end(); addPiecesIt++)
		delete (*addPiecesIt);
	for(addPiecesIt = addBlackPieces.begin(); addPiecesIt != addBlackPieces.end(); addPiecesIt++)
		delete (*addPiecesIt);
	addWhitePieces.clear();
	addBlackPieces.clear();
	int pw = 0, pb = 0, kw = 0, kb = 0, bw = 0, bb = 0, rw = 0, rb = 0;


	for(int i = 0; i < DESKLEN; i++) {
			for(int j = 0; j  < DESKLEN; j++) {
                (*(dSect + i) + j)->selected = false;
				deskSection* dSptr = *(dSect + i) + j;
				if(dSptr->number > 2 && dSptr->number < 7) {
						pair<deskSection*, chessPiece*> p(dSptr, &noPiece);
						sectionsMap.insert(p);
				}
				else if(dSptr->number == 2 || dSptr->number == 7) {
					if(dSptr->number == 2) {

						chessPiece* piecePtr = (pawnWhiteArr + pw++);
						pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
						sectionsMap.insert(p1);
						pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
						whiteChess.insert(p2);
					}
					else if(dSptr->number == 7) {
						chessPiece* piecePtr = (pawnBlackArr + pb++);
						pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
						sectionsMap.insert(p1);
						pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
						blackChess.insert(p2);
                    }
				}
				else if(dSptr->number == 1 && (dSptr->symbol == 'b' || dSptr->symbol == 'g')) {
					chessPiece* piecePtr = (knightWhiteArr + kw++);
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					whiteChess.insert(p2);
				}
				else if(dSptr->number == 8 && (dSptr->symbol == 'b' || dSptr->symbol == 'g')) {
					chessPiece* piecePtr = (knightBlackArr + kb++);
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					blackChess.insert(p2);
				}
				else if(dSptr->number == 1 && (dSptr->symbol == 'c' || dSptr->symbol == 'f')) {
					chessPiece* piecePtr = (bishopWhiteArr + bw++);
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					whiteChess.insert(p2);
				}
				else if(dSptr->number == 8 && (dSptr->symbol == 'c' || dSptr->symbol == 'f')) {
					chessPiece* piecePtr = (bishopBlackArr + bb++);
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					blackChess.insert(p2);
				}
				else if(dSptr->number == 1 && (dSptr->symbol == 'a' || dSptr->symbol == 'h')) {
					chessPiece* piecePtr = (rookWhiteArr + rw++);
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					whiteChess.insert(p2);
				}
				else if(dSptr->number == 8 && (dSptr->symbol == 'a' || dSptr->symbol == 'h')) {
					chessPiece* piecePtr = (rookBlackArr + rb++);
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					blackChess.insert(p2);
				}
				else if(dSptr->number == 1 && dSptr->symbol == 'd') {
					chessPiece* piecePtr = &queenWhite;
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					whiteChess.insert(p2);

				}
				else if(dSptr->number == 8 && dSptr->symbol == 'd') {
					chessPiece* piecePtr = &queenBlack;
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					blackChess.insert(p2);
				}
				else if(dSptr->number == 1 && dSptr->symbol == 'e') {
					chessPiece* piecePtr = &kingWhite;
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					whiteChess.insert(p2);
				}
				else if(dSptr->number == 8 && dSptr->symbol == 'e') {
					chessPiece* piecePtr = &kingBlack;
					pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
					sectionsMap.insert(p1);
					pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
					blackChess.insert(p2);
				}

			}
	}


 }


 chessDesk::~chessDesk() {                                                //���������� ��������� �����
	sectionsMap.clear();                                              //������� ���������
	whiteChess.clear();
	blackChess.clear();
	addWhitePieces.clear();
	addBlackPieces.clear();
 }


 pair<deskSection*, chessPiece*>* chessDesk::getSection() {
		static pair<deskSection*, chessPiece*> temp;             //���������� ��������� ���� deskSection � chessPiece
		if(sectionsIt != sectionsMap.end()) {           //���� �������� �� ��������� �� ������ ������ �� ����. ��-��
				temp = (*sectionsIt);                   //����������� �������� ��������� ����
				sectionsIt++;                           //�������������� ��������
				return &temp;                            //���������� ��������� ��������
		}
		else {
			sectionsIt = sectionsMap.begin();
			return NULL;
		}
 }




 //----------------------------------���������� �������������� ��������-------------------------------------------------------------------
 bool sectCompare::operator()(const deskSection* dS1, const deskSection* dS2) const {          //���������� ��������������� �������
		if(dS1->symbol < dS2->symbol)                                                         //��� ���������� ��������-�����
			return true;                                                                    //��������� �� ����������� �� ���������� ����������
		else if(dS1->symbol == dS2->symbol) {                                               //���� ���������� ���������� �����,
			if(dS1->number < dS2->number)                                                 //��������� �� ��������
				return true;
			else
				return false;
        }
		else
		  return false;
 }

bool isCoordInSect::operator()(const pair<deskSection*, chessPiece*>  pR) const {
		if(coordX >= pR.first->pixelX && coordX < pR.first->pixelX + SQRSIZE
		&& coordY >= pR.first->pixelY && coordY < pR.first->pixelY + SQRSIZE)
				return true;

		else
			return false;

}

bool sectEnable::operator() (const pair<deskSection*, chessPiece*> pR) const {
		if(pR.first->number == num && pR.first->symbol == sym && pR.second->getChessType() == NOPIECE)
			return true;
		else
			return false;
}

bool sectTakeEnable::operator()(const pair<deskSection*, chessPiece*> pR) const {
	if(pR.first->number == num && pR.first->symbol == sym
		&& pR.second->getChessType() != NOPIECE && pR.second->getChessColor() == chColor)
			return true;
		else
			return false;

}

bool lessSectPtr::operator()(deskSection* const ptr1, deskSection* const ptr2) {      //���������� ��������������� �������
																					//��� ���������� ���������� �� deskSection
																					//�� �������� �����
		if(ptr1->symbol < ptr2->symbol)
			return true;
		else  if(ptr1->symbol == ptr2->symbol) {
				if(ptr1->number < ptr2->number)
					return true;
				else
					return false;
		}
		else
			return false;
}

bool equalSectPtr::operator()(const deskSection* ptr) const {
	if(ptr->symbol == sectPtr->symbol && ptr->number == sectPtr->number)
		return true;
	else
		return false;
}

bool lessPiecePtr::operator() (chessPiece* const piecePtr1, chessPiece* const piecePtr2) const  {

//� ������������� �������� �������� ������ ������������ �������� <=, �.� ��� ��������� � ��������
// ����������� (����������� ������� lessPiecePtr) ����� ������������� ������, ����� ���������������� ����� ������� ���� ��� ����������
//������� � ��� �������������!!!
	if(piecePtr1->getChessType() < piecePtr2->getChessType())           //���� ��� ������, �� ������� ��������� ��������� piecePtr1,
																		//������ ���� ������, �� ������� ��������� ��������� piecePtr2,
																		//(��� ������ �������� �������������), �� ���������� true,
																		//����� - false
		return true;
	else if(piecePtr1->getChessType() == piecePtr2->getChessType()) {
		if(piecePtr1->getChessNumber() < piecePtr2->getChessNumber())       //���� ��� ����� ����������, ���������� �� ������
			return true;
		else
			return false;
	}
	else
		return false;

}


bool piecePairEqual::operator()(const pair<chessPiece*, deskSection*> piecePair) const {
		if(piecePair.first == piecePtr)
			return true;
		else
			return false;
}


bool sectCoordEqual::operator()(const pair<deskSection*, chessPiece*> deskPair) const {
		if(deskPair.first->symbol == sym && deskPair.first->number == num)
			return true;
		else
			return false;
}

bool pieceCoordEqual::operator()(const pair<chessPiece*, deskSection*> piecePair) const {
		if(piecePair.second->symbol == sym && piecePair.second->number == num)
			return true;
		else
			return false;
}

bool coordExistSet::operator()(const deskSection* dS) const {
		if(dS->symbol == sym && dS->number == num)
			return true;
		else
			return false;
}
//-------------------------------------------------------------------------------------------------------------------------------------

 pair<deskSection*, chessPiece*> chessDesk::findSection(int X, int Y) {

	static deskSection locSect;
	static Nopiece locPiece;
	pair<deskSection*, chessPiece*> localPair = make_pair(&locSect, &locPiece);
	isCoordInSect inSect(X, Y);               //�������������� �������������� ������
	sectionsIt = find_if(sectionsMap.begin(), sectionsMap.end(), inSect);         //��������� �������� ������ � �����������
																				  //�������������� �������� inSect
	if(sectionsIt != sectionsMap.end())                       //���� ������� ����������� ������
		localPair = *sectionsIt;                                //�����������  ��������� ���� �������� ��-��
	sectionsIt = sectionsMap.begin();                       //����������� ��������� �������� ������ �����������
	return  localPair;                                    //���������� ����


 }

 pair<deskSection*, chessPiece*> chessDesk::getSection(char sym, int num) {
	 static deskSection locSect;
	 static Nopiece locPiece;
	 pair<deskSection*, chessPiece*> localPair =  make_pair(&locSect, &locPiece);
	 for(sectionsIt = sectionsMap.begin(); sectionsIt != sectionsMap.end(); sectionsIt++)
		 if(sectionsIt->first->symbol == sym && sectionsIt->first->number == num)
			localPair = *sectionsIt;
	 sectionsIt = sectionsMap.begin();
	 return localPair;
 }

 map<deskSection*, chessPiece*, sectCompare>* chessDesk::getMap() {
		return &sectionsMap;
 }

 map<chessPiece*, deskSection*, lessPiecePtr>* chessDesk::getWhiteChess() {
	 return &whiteChess;
 }

 map<chessPiece*, deskSection*, lessPiecePtr>* chessDesk::getBlackChess() {
	 return &blackChess;
 }


 Nopiece* chessDesk::getNopiece() {
     return &noPiece;
 }

 chessPiece* chessDesk::pieceFromArr(chessColor color, chessType type, int num) {         //�����, �����. ��������� �� ���. ������ �� �������
	 if(color == WHITE) {                                   //���� ������������� ������ �����
		 switch(type) {                                     //� ����������� �� ���� ������
		 case PAWN:
			for(int i=0; i < 8; i++) {
				if(pawnWhiteArr[i].getChessNumber() == num)        //���������� ����� ����������. �������� ����������. �������
					return (pawnWhiteArr + i);

			}
		 case KNIGHT:
			for(int i=0; i < 2; i++) {
				if(knightWhiteArr[i].getChessNumber() == num)
					return (knightWhiteArr + i);
            }


		 case BISHOP:
			for(int i=0; i < 2; i++) {
				if(bishopWhiteArr[i].getChessNumber() == num)
					return (bishopWhiteArr + i);
			}

		 case ROOK:
			for(int i=0; i < 2; i++) {
				if(rookWhiteArr[i].getChessNumber() == num)
					return (rookWhiteArr + i);
			}
		 case QUEEN:
			if(queenWhite.getChessNumber() == num)
				return &queenWhite;
		 case KING:
			if(kingWhite.getChessNumber() == num)
				return &kingWhite;

		 default:
				return &noPiece;
		 }
	 }
	 else if(color == BLACK) {                                  //���������� ��� ������ �����
			switch(type) {
			case PAWN:
				for(int i=0; i < 8; i++) {
					if(pawnBlackArr[i].getChessNumber() == num)
						return (pawnBlackArr + i);
				}
			case KNIGHT:
				for(int i=0; i < 2; i++) {
						if(knightBlackArr[i].getChessNumber() == num)
							return (knightBlackArr + i);

				}
			case BISHOP:
				for(int i =0; i < 2; i++) {
						if(bishopBlackArr[i].getChessNumber() == num)
							return (bishopBlackArr + i);
				}
			case ROOK:
				for(int i = 0; i < 2; i++) {
						if(rookBlackArr[i].getChessNumber() == num)
							return (rookBlackArr + i);

				}
			case QUEEN:
				if(queenBlack.getChessNumber() == num)
					return &queenBlack;
			case KING:
				if(kingBlack.getChessNumber() == num)
					return &kingBlack;
			default:
				return &noPiece;


			}
	 }
 }

 deskSection* chessDesk::sectFromArr(char sym, int num) {
	 int x = sym - 'a';
	 int y = DESKLEN - num;
	 return &dSect[x][y];
 }


 void chessDesk::addPiece(chessPiece* piecePtr) {                       //�����, ����������� ��������� �� ���. ������ � ������ ����.
																		//�������������� ���. �����
	 if(piecePtr->getChessColor() == WHITE)                          //���� ��������� �� ����� ������
		addWhitePieces.push_back(piecePtr);                         //��������� � �������. ������ ����. �� �������������� ����� ������
	 else if(piecePtr->getChessColor() == BLACK)                    //���� ��������� �� ������ ������
		addBlackPieces.push_back(piecePtr);                         //��������� � �������. ������ ����. �� �������������� ������ ������
	 return;
 }

 void chessDesk::clearAddPiece() {                            //�����, �������������� ������, ���������� ��������������� �������� ����� �� ������

	 while(true) {
		 for(addPiecesIt = addWhitePieces.begin(); addPiecesIt != addWhitePieces.end(); addPiecesIt++) {       //���������� ��-�� ������ ���������� �� ���. ����� ������
			piecesIt = find_if(whiteChess.begin(), whiteChess.end(), piecePairEqual(*addPiecesIt));       //���� ��������� �� ������ ������
			if(piecesIt == whiteChess.end()) {                                                           // � ����������� ����� ����� ��� ���
			   addWhitePieces.erase(addPiecesIt);                                                      //������� ��������� �� ������ ����. �� �������������� ����� ������
			   delete *addPiecesIt;                                                                   //������������ ������, ���������� ���. �������
			   (*addPiecesIt) = NULL;                                                                 //�������� ��������� �� ���. ������
			   break;
			}
		 }
		 if(addPiecesIt == addWhitePieces.end())
			break;
	 }

	while(true) {
		for(addPiecesIt = addBlackPieces.begin(); addPiecesIt != addBlackPieces.end(); addPiecesIt++) {    //���������� ��� ��-�� ������ ���������� �� ���. ������ ������
			piecesIt = find_if(blackChess.begin(), blackChess.end(), piecePairEqual(*addPiecesIt));
			if(piecesIt == blackChess.end()) {
				addBlackPieces.erase(addPiecesIt);
				delete *addPiecesIt;
				(*addPiecesIt) = NULL;
				break;
			}
		}
		if(addPiecesIt == addBlackPieces.end())
            break;
	}

	 return;
 }

 bool chessDesk::addNumberExist(int num, chessColor color) {                          //�����, �����������, ���������� �� ������ � �������� �������
																						//����� �������������� �����
	if(color == WHITE) {                                                           //���� ������ �����
		for(addPiecesIt = addWhitePieces.begin(); addPiecesIt != addWhitePieces.end(); addPiecesIt++)    //���������� ��� ��-�� � ������ �������������� �����
			if((*addPiecesIt)->getChessNumber() == num)                              //���� ����� ��������� � ��������
				return true;                                                         //���������� true
	}
	else if(color == BLACK) {                                                      //���������� ��� ������ ������
			for(addPiecesIt = addBlackPieces.begin(); addPiecesIt != addBlackPieces.end(); addPiecesIt++)
				if((*addPiecesIt)->getChessNumber() == num)
					 return true;
	}

	return false;


 }

 void chessDesk::sectItBegin() {
	 sectionsIt = sectionsMap.begin();
     return;
 }





deskSection::deskSection(char s, int n) {                   //����������� ���������-������ ���. �����
		symbol = s + 'a';
		number = DESKLEN - n;                                 //��������� ���������� �� ���������� ����� ���������� � 1, ������ ���� A1 �����������
															//� ������ ������ ����
		pixelX = TOPLEFTX + s * SQRSIZE;                        //�������������� ���������� ���������� � ������
		pixelY = TOPLEFTY +  n * SQRSIZE;                        //�������������� ���������� ���������� Y ������
		if(((s+1) % 2 == 0) && ((n+1) % 2 == 1))
			sectColor = BLACK;
		else if(((s+1) % 2 == 0) && ((n+1) % 2 == 0))
			sectColor = WHITE;
		else if(((s+1) % 2 == 1) && ((n+1) % 2 == 0))
			sectColor = BLACK;
		else if(((s+1) % 2 == 1) && ((n+1) % 2 == 1))
			sectColor = WHITE;
        selected = false;
}

bool operator==(const deskSection deskSect1, const deskSection deskSect2) {
		if(deskSect1.symbol == deskSect2.symbol && deskSect1.number == deskSect2.number)
			return true;
		else
			return false;
}


string chessPiece::getPicName() const {
		return picName;
}

chessType chessPiece::getChessType() const {
		return chType;
}


chessColor chessPiece::getChessColor() const {
		return chColor;
}

char chessPiece::getChessNumber() const {
		return chNumber;

}

void chessPiece::setParameters(chessType cT, chessColor cC, char num, string name) {
	chType = cT;
	chColor = cC;
	chNumber = num;
	picName = name;

}

set<deskSection*, lessSectPtr>* chessPiece::getPossMoves() {
	return &possMoves;
}

set<deskSection*, lessSectPtr>* chessPiece::getPossTakes() {
   	return &possTakes;
}

bool operator==(const chessPiece &Piece1, const chessPiece  &Piece2) {
	if(Piece1.chType == Piece2.chType && Piece1.chColor == Piece2.chColor && Piece1.chNumber == Piece2.chNumber)
		return true;
	else
		return false;

}

void chessPiece::checkCheck(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {     //�����, ����������� ���������� ���� ����� ����
    	set<deskSection*, lessSectPtr>::iterator locSetIt;

	map<deskSection*, chessPiece*, sectCompare>::iterator tmpSectionsIt;
	if(chColor == WHITE) {
		for(tmpSectionsIt = currDesk->begin(); tmpSectionsIt != currDesk->end(); tmpSectionsIt++) {
			if((*tmpSectionsIt).second->getChessType() == KING && (*tmpSectionsIt).second->getChessColor() == WHITE)
				break;

		}
	}
	else if(chColor == BLACK) {
			for(tmpSectionsIt = currDesk->begin(); tmpSectionsIt != currDesk->end(); tmpSectionsIt++) {
					if((*tmpSectionsIt).second->getChessType() == KING && (*tmpSectionsIt).second->getChessColor() == BLACK)
						break;
			}
	}

	bool tmpChecked = ((*tmpSectionsIt).second->getCheck() == true) ? true : false;         			//��������� � �������������� ��������� ���������� ��� �������� �������� ���������� ����

	while(true) {                                                 //��������� ������� ����, �.�. ����� �������� ��-�� �� ���������
																	//��� ��������� ��������, � ������, ���������� ����� ������� ��� ���������
																	//�� ���������� ����� ������! �����. ����. ���������� ��������� ������ ����� ������� ��������
		for(locSetIt = possMoves.begin(); locSetIt != possMoves.end(); locSetIt++) {          //�� ��������� ��������� ��� ���� ����� ������� ����,
																							  //����������� ��� ������ ��������� ����� (�.�. ����������� ������ ������)
				map<deskSection*, chessPiece*, sectCompare> tmpMap = *currDesk;              //��������� � �������������� ��������� ������-�����, ��� ���� ����� ���������
																							 //��������� ��������� ����� �� �������������� ���� �������
				chessPiece* tmpNopiece = tmpMap[*locSetIt];                           //��������� � �������������� ��������� �� ������ ������, ����� ��-� �������
																						//� ������ - ��������� ��� ���� �����, ����� ������
				tmpMap[*locSetIt] = this;                                            //����������� ��-�� �������  ����� ��������� �� ������, ����������
				tmpMap[currSect] = tmpNopiece;                                       //��������� �����, ��� ����� ��������� ������������� ��� �������
				(*tmpSectionsIt).second->setCheck(&tmpMap);                                                //�������� ����� �������� ������� ���� ��� ����� ��������� �����
				if((*tmpSectionsIt).second->getCheck()) {                                                  //���� ����� �������������� ����, ������ ����������� ��� �����
					possMoves.erase(locSetIt);                                  //��������� ������� ���� ����� ���� �� ��������� ��������� ��� ���� �����
					break;
				}
		}
		if(locSetIt == possMoves.end())
			break;
	}

	while(true) {

		for(locSetIt = possTakes.begin(); locSetIt != possTakes.end(); locSetIt++) {     //���������� �� ��������� ��������� ��� ������ ����� ������� ����,
																						//����������� ��� ������ ��������� �����
			map<deskSection*, chessPiece*, sectCompare> tmpMap = *currDesk;
			map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;
			for(tmpIt = tmpMap.begin(); tmpIt != tmpMap.end(); tmpIt++) {            //������� ���������� �������� �������� �� ��-� - ����,
																						//��������� ������� �������� ��������� �� ������ ���. ������
				if((*tmpIt).second->getChessType() == NOPIECE)
					break;
			}
			tmpMap[*locSetIt] = this;                                               //�� ��������� ����� �������������� ��� ���� ���� ����������� ��������� �� ������,
																					 //���������� �����
			tmpMap[currSect] = (*tmpIt).second;                                    //����, � �������� ����������� ���, ����������� �������������� ������ ������
			(*tmpSectionsIt).second->setCheck(&tmpMap);                                                    //��������� ������� ���� ��� ����� ��������� �����
			if((*tmpSectionsIt).second->getCheck()) {                                                       //���� ����� �������������� ����, ������ ����������� ��� �����
				possTakes.erase(locSetIt);                                       //��������� ������� ���� ����� ���� ��� ��������� ��������� ��� ���� �����
				break;
			}
		}
		if(locSetIt == possTakes.end())
			break;
	}

	(*tmpSectionsIt).second->setCheck(tmpChecked);                                                 //����� �������� ������������� �����, ���������� ���� ���������� ���� � �������� ��������
	return;

}

 void Pawn::noCheckMT(deskSection* currSect,          //����� ��������� ��������� ����� ��������� ��� ���� � ��� ������ ������
								map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	int tmpNum;
	char tmpSym;
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;            //��������� ��������� �������� �����������  currDesk (���������� �. �����.)
	map<deskSection*, chessPiece*, sectCompare>::iterator secondIt;            //��������� �������������� ��������� �������� ����������� currDesk

    possMoves.clear();                                  //����� �������� ��������� � ��������� ��������� ��� ���� � ������ �����
	possTakes.clear();                                  //������� ������ ��������� �� ��-�� ��� ����������� ��������� ������


	for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {          //���� � ����������� currDesk ����, ���� ������� (deskSection*)
			if(*currSect == *(locMapIt->first)) {                                        //����� ��������� currSect
				break;

			}
	}
	if(locMapIt != currDesk->end()) {                                      //���� ������� ���� �������
			if(locMapIt->second->getChessColor() == BLACK) {          //��� ������ �����
 //--------------------------------------------��������� ��� ������ ����------------------------------------------------------------------------------------------------
				tmpSym = currSect->symbol - 1;                     //���-�������� ��������� ������
				tmpNum = currSect->number - 1;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));  //���� ���� ������� ������� ������ ����� ����,
				if(locMapIt != currDesk->end())                                                                 //�.�. ������ ����� �����
					possTakes.insert(locMapIt->first);                                               //��������� ��� � ��������� ��������� ��� ����� �����
			 //---------------------���������� ������ ����� �� �������-------------------------------------------------------------------------------
				if(currSect->number == 4) {                                  //���� ����� ����������� �� 4-�� ���������
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 4));     //�������� �� ����, ���������� ������,
																													//������� ���������������� ����� ����� �� �������
					secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 2));     //�������� �� ����, ���������� ����,
																													//������� �������� ���������� ������ ������������,
																													//���� ������
					if(locMapIt != currDesk->end()) {                                                            //���� ���� � ������ ���������� ������������ ����������
						if((*locMapIt).second->getChessType() == PAWN && (*locMapIt).second->getChessColor() == WHITE)      //���� �� ����� �������������� ������ ����������� ����� �����
							if((*locMapIt).second->getPrevPos() == (*secondIt).first) {                                  //���� ���������� ����� ���� ����� �������� ���������� ��� ������ �� ������� ����
								secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 3));       //��������� �� ����, �� ������� ������������ �����
																														//��� ������ �� ������� ��������� �����
								possMoves.insert(secondIt->first);                           //�������� ����, �� ������� ���������� ��������� ��� ��� ������ �� �������, �� ���������
							}                                                                //��������� ��� ���� �����
					}
				}
			 //------------------------------------------------------------------------------------------------------------------------------------------------

				tmpSym = currSect->symbol + 1;
				tmpNum = currSect->number - 1;                       //���������� ��� ���-��������� ���������
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));    //��������� ����������� ���� ��� ������
				if(locMapIt != currDesk->end())                                                                   //��� ������ ��������������� ������� sectTakeEnable
					possTakes.insert(locMapIt->first);
				//-----------------------���������� ������ ����� �� �������---------------------------------------------------------------------------------------
				if(currSect->number == 4) {

					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 4));
					secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 2));
					if(locMapIt != currDesk->end()) {
						if((*locMapIt).second->getChessType() == PAWN && (*locMapIt).second->getChessColor() == WHITE)
							if((*locMapIt).second->getPrevPos() == (*secondIt).first) {
									secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 3));
									possMoves.insert(secondIt->first);
							}
					}
				}
				//---------------------------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------��������� ��� ���� ����-----------------------------------------------------------------------------------
				if(currSect->number != 7) {                    //��� ���� ��������� �� �������
					tmpSym = currSect->symbol;                  //���������� ������ ��������� ��� ���������, �.�. ����� ����� �� ���������
					tmpNum = currSect->number - 1;              //����� ����� ����� �� ���� ���� ������
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));     //���� ���� ��������� ���� � �����. tNum, tSym
					if(locMapIt != currDesk->end())
						possMoves.insert(locMapIt->first);                            //��������� ��������� �� ���� (deskSection*) � ���������  ���������
																					  //��� ���� �����

				}
				else {                                         //��� ������� ����
                    tmpSym = currSect->symbol;
					tmpNum = currSect->number-1;             //�� ����
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
					if(locMapIt != currDesk->end())
						possMoves.insert(locMapIt->first);
					else
						return;                    //���� �� ���� ������ ��� �� ��������, �� ��������� �����, �.�. �����
															//�� ����� ������������� ������
					tmpSym = currSect->symbol;
					tmpNum = currSect->number - 2;             //�� ��� ������
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
					if(locMapIt != currDesk->end())
						possMoves.insert(locMapIt->first);


                }


			}
			else if(locMapIt->second->getChessColor() == WHITE) {         //��� ����� �����
//--------------------------------------------��������� ��� ������ ����------------------------------------------------------------------------------------------------
					tmpSym = currSect->symbol - 1;                     //������-�������� ��������� ������
					tmpNum = currSect->number + 1;
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));   //���� ���� ������� ������� ������ ����� ����,
																												   //�.�. ������ ����� �����
					if(locMapIt != currDesk->end())
						possTakes.insert(locMapIt->first);                                               //��������� ��� � ��������� ��������� ��� ����� �����
				//-----------------------���������� ������ ����� �� �������---------------------------------------------------------------------------------------
					if(currSect->number == 5) {                                                   //���� ����� ����������� �� 5-�� �����������
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 5));
						secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 7));
						if(locMapIt != currDesk->end()) {
							if((*locMapIt).second->getChessType() == PAWN && (*locMapIt).second->getChessColor() == BLACK)
								if((*locMapIt).second->getPrevPos() == (*secondIt).first) {
										secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 6));
										possMoves.insert(secondIt->first);
								}
						}
					}
				//---------------------------------------------------------------------------------------------------------------------------------------------------


					tmpSym = currSect->symbol + 1;
					tmpNum = currSect->number + 1;                       //���������� ��� ������-��������� ���������
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));
					if(locMapIt != currDesk->end())
						possTakes.insert(locMapIt->first);
                //-----------------------���������� ������ ����� �� �������---------------------------------------------------------------------------------------
					if(currSect->number == 5) {
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 5));
						secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 7));
						if(locMapIt != currDesk->end()) {
							if((*locMapIt).second->getChessType() == PAWN && (*locMapIt).second->getChessColor() == BLACK)
								if((*locMapIt).second->getPrevPos() == (*secondIt).first) {
										secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 6));
										possMoves.insert(secondIt->first);
								}
						}
					}
				//---------------------------------------------------------------------------------------------------------------------------------------------------


//--------------------------------------------��������� ��� ���� ����----------------------------------------------------------------------------------------
					if(currSect->number != 2) {                         //��� ���� ��������� �� �������
							tmpSym = currSect->symbol;
							tmpNum = currSect->number + 1;
							locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));      //��������� ���������� �� ���� � ������� ������������ � �������� �� ���
							if(locMapIt != currDesk->end())                                                   //���� ����������
								possMoves.insert(locMapIt->first);                                               //������� ��� � ��������� ��������� ��������� ��� ���� �����
					}
					else {                                           //��� ������� ����
                        tmpSym = currSect->symbol;
						tmpNum = currSect->number + 1;              //�� ���� ������
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
						if(locMapIt != currDesk->end())
							possMoves.insert(locMapIt->first);
						else
							return;
						tmpSym = currSect->symbol;
						tmpNum = currSect->number + 2;              //�� ��� ������
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
						if(locMapIt != currDesk->end())
							possMoves.insert(locMapIt->first);

					}

			}
	}


	return;
}

void Pawn::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	noCheckMT(currSect, currDesk);
	checkCheck(currSect, currDesk);
	return;
}

void Knight::noCheckMT(deskSection* currSect,                   //����� ���������� ��������� ����� (������) ��������� ��� ���� �����
														map<deskSection*, chessPiece*, sectCompare>* currDesk) {         //��� "������ ������" (������� ������������ ������) ���������� ������ �� ���������
		 int tmpNum;            //��������� ��������� ����� ������
		 char tmpSym;           //��������� ��������� ������ ������

		 map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;   //������� ��������� �������� ����������� currDesk

		 possMoves.clear();                                  //����� �������� ��������� � ��������� ��������� ��� ���� � ������ �����
		 possTakes.clear();                                  //������� ������ ��������� �� ��-�� ��� ����������� ��������� ������

//-----------------------------��������� ����������� 1-�� ���������� ���� ��� ������ �����---------------------------------------
		 tmpNum = currSect->number - 1;
		 tmpSym = currSect->symbol - 2;
		 locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		 if(locMapIt != currDesk->end())                       //���� ���������� ���� ����
			possMoves.insert((*locMapIt).first);                 //��������� � ��������� ��������� ���� ����,
															//�.� ��������� �� deskSection
		 if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//----------------------------��������� ����������� 2-�� ���������� ���� ��� ������ �����----------------------------------
		 tmpNum = currSect->number - 2;
		 tmpSym = currSect->symbol - 1;
		 locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));            //���������, �������� �� ������ ��� ����
		 if(locMapIt != currDesk->end())                                                 //���� ��,
			possMoves.insert((*locMapIt).first);                                            //�� ��������� �� � ��������� ��������� ��� ���� �����
          if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//---------------------------��������� ����������� 3-�� ���������� ���� ��� ������ �����----------------------------
		tmpNum = currSect->number - 1;
		tmpSym = currSect->symbol + 2;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//----------------------------��������� ����������� 4-�� ���������� ���� ��� ������ �����----------------------------
		tmpNum = currSect->number - 2;
		tmpSym = currSect->symbol + 1;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//----------------------------��������� ����������� 5-�� ���������� ���� ��� ������ �����-----------------------------
		tmpNum = currSect->number + 1;
		tmpSym = currSect->symbol - 2;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//--------------------------��������� ����������� 6-�� ���������� ���� ��� ������ �����---------------------------------
		tmpNum = currSect->number + 2;
		tmpSym = currSect->symbol - 1;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));       //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//-----------------------��������� ����������� 7-�� ���������� ���� ��� ������--------------------------------------------
		tmpNum = currSect->number + 1;
		tmpSym = currSect->symbol + 2;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
		 if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//-------------------��������� ����������� 8-�� ���������� ���� ��� ������--------------------------------------

		tmpNum = currSect->number + 2;
		tmpSym = currSect->symbol + 1;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //���� ���� ������
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //���������, �������� �� ���� ��� ������
																												//(������ ��������� ������ ���������������� �����)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }

		return;
}

void Knight::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	noCheckMT(currSect, currDesk);
	checkCheck(currSect, currDesk);
	return;
}


void Bishop::noCheckMT(deskSection* currSect,                 //����� ���������� ��������� ����� ��������� ��� ���� ������
													map<deskSection*, chessPiece*, sectCompare>* currDesk) {
		map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt = currDesk->begin();
		int tmpNum = currSect->number;                              //���������� �������� ��������� ������
		char tmpSym = currSect->symbol;
		int checkNum = tmpNum;                                     //����������� �� ����������� ������, ��������� ���������� ������� ����� ����������� ������
		char checkSym = tmpSym;

        possMoves.clear();                                  //����� �������� ��������� � ��������� ��������� ��� ���� � ������ �����
		possTakes.clear();                                  //������� ������ ��������� �� ��-�� ��� ����������� ��������� ������


		for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {           //���� ������� ���� ������ ������� � ����������� ����� ����� currDesk
			if(currSect == locMapIt->first)
				break;
		}
//-----------------��� �� ���-�����---------------------------------------------------------------------------
		while(locMapIt != currDesk->end()) {                                //��� ������ �� ���� ��������� �������������� ��� ������� ����, �� ���������
			checkNum--;
			checkSym--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
		}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
        if(chColor == BLACK) {
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
						if(locMapIt != currDesk->end())
							possTakes.insert((*locMapIt).first);
		}
		else if(chColor == WHITE) {
					locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);

		}
        locMapIt = currDesk->begin();                                    //���������� �������� � ������ ���������
		checkNum = tmpNum;                                               //������������ ������������ ���� ����� ��������� ���������� ��������� ������
		checkSym = tmpSym;
//-----------------��� �� ���-������----------------------------------------------------------------
		while(locMapIt != currDesk->end()) {
				checkNum--;
				checkSym++;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
				if(locMapIt != currDesk->end())
					possMoves.insert(locMapIt->first);
		}
//-------------------������ ������ �� ����� ����--------------------------------------------------------------
        if(chColor == BLACK) {
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
						if(locMapIt != currDesk->end())
							possTakes.insert((*locMapIt).first);
		}
		else if(chColor == WHITE) {
					locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);

		}
		locMapIt = currDesk->begin();
		checkNum = tmpNum;
		checkSym = tmpSym;
//-----------------��� �� ������-�����--------------------------------------------------------------------

		while(locMapIt != currDesk->end()) {
				checkNum++;
				checkSym--;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
				if(locMapIt != currDesk->end())
					possMoves.insert(locMapIt->first);
		}
//-----------------������ ������ �� ����� ����--------------------------------------------------------------
        if(chColor == BLACK) {
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
						if(locMapIt != currDesk->end())
							possTakes.insert((*locMapIt).first);
		}
		else if(chColor == WHITE) {
					locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);

		}
        locMapIt = currDesk->begin();
		checkNum = tmpNum;
		checkSym = tmpSym;
//----------------��� �� ������-������---------------------------------------------------------------------
		while(locMapIt != currDesk->end()) {
				checkNum++;
				checkSym++;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
				if(locMapIt != currDesk->end())
					possMoves.insert(locMapIt->first);
		}
//----------------------������ ������ �� ����� ����---------------------------------------------------------------
        if(chColor == BLACK) {
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
						if(locMapIt != currDesk->end())
							possTakes.insert((*locMapIt).first);
		}
		else if(chColor == WHITE) {
					locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);

		}
		return;

}

void Bishop::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	noCheckMT(currSect, currDesk);
	checkCheck(currSect, currDesk);
	return;
}

void Rook::noCheckMT(deskSection* currSect,                  //����� ���������� ��������� ����� ��������� ��� ���� ������
													map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	int tmpNum = currSect->number;
	char tmpSym = currSect->symbol;
	int checkNum = tmpNum;
	char checkSym = tmpSym;

	possMoves.clear();                                  //����� �������� ��������� � ��������� ��������� ��� ���� � ������ �����
	possTakes.clear();                                  //������� ������ ��������� �� ��-�� ��� ����������� ��������� ������

	for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {
		if(locMapIt->first == currSect)
			break;
	}

//---------------------��� �� �����--------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkNum = tmpNum;

//--------------------��� �� ������---------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);

	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkSym = tmpSym;
//-----------------------��� �� ��------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkNum = tmpNum;
//----------------------��� �� �����-------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
		checkSym--;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
		if(locMapIt != currDesk->end())
			possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	return;
}

void Rook::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	noCheckMT(currSect, currDesk);
	checkCheck(currSect, currDesk);
	return;
}

void Queen::noCheckMT(deskSection* currSect,
													map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	int tmpNum = currSect->number;
	char tmpSym = currSect->symbol;
	int checkNum = tmpNum;
	char checkSym = tmpSym;

    possMoves.clear();                                  //����� �������� ��������� � ��������� ��������� ��� ���� � ������ �����
	possTakes.clear();                                  //������� ������ ��������� �� ��-�� ��� ����������� ��������� ������

	for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {
		if(locMapIt->first == currSect)
			break;
	}

//---------------------��� �� �����--------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkNum = tmpNum;
//--------------------��� �� ������---------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
	//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkSym = tmpSym;

//-----------------------��� �� ��------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkNum = tmpNum;
//----------------------��� �� �����-------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
		checkSym--;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
		if(locMapIt != currDesk->end())
			possMoves.insert(locMapIt->first);
}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkSym = tmpSym;
//-----------------��� �� ���-�����---------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {                                //��� ������ �� ���� ��������� �������������� ��� ������� ����, �� ���������
		checkNum--;
		checkSym--;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
		if(locMapIt != currDesk->end())
			possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkNum = tmpNum;
	checkSym = tmpSym;
//---------------��� �� ���-������----------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum--;
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkNum = tmpNum;
	checkSym = tmpSym;
//-----------------��� �� ������-�����--------------------------------------------------------------------

	while(locMapIt != currDesk->end()) {
			checkNum++;
			checkSym--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	locMapIt = currDesk->begin();
	checkNum = tmpNum;
	checkSym = tmpSym;
//----------------��� �� ������-������---------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum++;
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------������ ������ �� ����� ����---------------------------------------------------------------
	if(chColor == BLACK) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
					if(locMapIt != currDesk->end())
						possTakes.insert((*locMapIt).first);
	}
	else if(chColor == WHITE) {
				locMapIt = find_if(currDesk-> begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
				if(locMapIt != currDesk->end())
					possTakes.insert((*locMapIt).first);

	}
	return;

}

void Queen::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	noCheckMT(currSect, currDesk);
	checkCheck(currSect, currDesk);
	return;
}
void King::noCheckMT(deskSection* currSect,                     //����� ��� ������� ��-�� �� ��������� ��������� ��� ���� � ������ �����
													map<deskSection*, chessPiece*, sectCompare>* currDesk) {        //��� ��������  ���� ������ ��� �������, ������� ��������� ���
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	char tmpSym = currSect->symbol;
	int tmpNum = currSect->number;
	char checkSym = tmpSym;
	int checkNum = tmpNum;

    possMoves.clear();                                  //����� �������� ��������� � ��������� ��������� ��� ���� � ������ �����
	possTakes.clear();                                  //������� ������ ��������� �� ��-�� ��� ����������� ��������� ������

//--------------------��� �� ���-�����------------------------------------------------------
	checkSym--;
	checkNum--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------������ ������--------------------------------------------------------------
	if(chColor == BLACK)                                   //��� ������� ������
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)                              //��� ������ ������
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkSym = tmpSym;
	checkNum = tmpNum;
//--------------------��� �� ��--------------------------------------------------------------
	checkNum--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------������ ������--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
//-------------------��� �� ���-������--------------------------------------------------------
	checkNum--;
	checkSym++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------������ ������--------------------------------------------------------------
    if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
	checkSym = tmpSym;
//---------------------��� �� ������-----------------------------------------------------------
	checkSym++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------������ ������--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkSym = tmpSym;
//---------------------��� �� ������-������--------------------------------------------------------
	checkSym++;
	checkNum++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------������ ������--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
	checkSym = tmpSym;
//---------------------��� �� �����-----------------------------------------------------------------
	checkNum++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------������ ������--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
//--------------------------��� �� ������-�����--------------------------------------------------------
	checkNum++;
	checkSym--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------������ ������--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
	checkSym = tmpSym;
//--------------------------���� �� �����--------------------------------------------------
	checkSym--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------������ ������--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);


	return;

}

void King::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {     //����� ��� ������� ��-�� �� ��������� ���������
	noCheckMT(currSect, currDesk);                                                                          //��� ���� � ������ ����� � ��������� ����

	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	set<deskSection*, lessSectPtr>* locSetPtr;                     //��������� ��������� ��������� �� ��������� �����
	set<deskSection*, lessSectPtr> tmpSet;                      //���������� ��������� ��������� �����
	set<deskSection*, lessSectPtr>::iterator locSetIt;            //��������� ��������� �������� ��������� set<deskSection*, lessPiecePtr>


    	//----------------------���������� ���������--------------------------------------------------------
	if(isMoved == false && isChecked == false) {              //���� �� ���� ����� �������
		bool freeLine = true;                  //��������� � �������������� ����, ����������� �������� true � ������, ���� ����� ��������� ��������
		bool readyRook = true;              //��������� � �������������� ���� ���������� ����������. ����� � ���������
		bool strikeKing = false;             //��������� � �������������� ����, ����������� ����. true, ���� ���� �� �����,
											//�� ������� ������������ ������ � ���� ���������, ��������� ��� ������
		//-----------------� ������� ������������ ������--------------------------------------------------------
		if(chColor == WHITE) {            //���  ����� �����
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('f', 1));               //���������, �������� �� ���� �� ����� ���������
			if(locMapIt->second->getChessType() != NOPIECE)                                          //���� ���, ��
				freeLine = false;                                                                 //������������� ���� ������� ����� ��������� � ��������� false
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('g', 1));       //���������� ��� ����. ����
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('h', 1));       //������������� �������� �� ��-�, ������ �������� �������� ����, �� ���. �����������
																								  //����� ��� ���������
			if(locMapIt->second->getChessType() != ROOK || locMapIt->second->getIsMoved() == true)   //���� �� ��� �� ����, ��� �� ��� ��� �������� ���
				readyRook = false;                                                                  //������������� ���� ��������� ����� � ��������� false
			if(freeLine == true && readyRook == true) {                                         //���� ����� ��������� � ������� ������������ ����� �������� � ����� ������ � ���������
				for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {        //���������� ��� ������
					if(locMapIt->second->getChessColor() == BLACK && locMapIt->second->getChessType() != NOPIECE) {         //��� ������ �� ��������� �����
						locMapIt->second->noCheckMT(locMapIt->first, currDesk);                       //� ��������� ������ ���������� ����� ��� �������� ���������� ��� �����
						if(locMapIt->second->getChessType() != PAWN)
							locSetPtr = locMapIt->second->getPossMoves();                                     //�������� ��������� ��������� ��� ���� �����
																											//���� ������ �� �������� ������, �� ������������ ������ ����. � �����. ����
						else {                                                                   //��� ��������� �����
							map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //�������� ��� ������ ������������� ��� ����\������ �����
							locSetPtr = locMapIt->second->getPossMoves();                        //���������� ��������� ����. ��� ���� �����
							if(!locSetPtr->empty()) {                                             //���� ��� ��������
								locSetIt = locSetPtr->begin();                                  //��������� ��� ���� ����� ����� ���� ����, ������� ����������� ���������
																								//��������, ����. �� ���. ��-� ���������
								tmpSet = *locSetPtr;                //�������� ��������� ��������� ��� ���� ����� �� ��������� ��������� ���������
																		//���������� ��� ����, ����� �������� ������������� ����� �� ������� ��������� � �������� ����. ��� .�����
								seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //���������, ���������� �� ���� ������ �� ���������� ��� ������
								if(seekMapIt != currDesk->end())                   //���� ����������, ������ �� ��������� ��������� ��������� ��� ���� �����
									tmpSet.insert(seekMapIt->first);

								seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //���������, ���������� �� ����, ����� �� ���������� ��� ����
								if(seekMapIt != currDesk->end())
									tmpSet.insert(seekMapIt->first);

							}
						}

						if((*locMapIt).second->getChessType() != PAWN) {                     //���� ������ �� �������� ������ ���������� ���� �� ���������-�������� ������
							for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //��� ������� ���� �� ��������� ��������� ��� ������
									if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
									else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
							}
						}
						else if((*locMapIt).second->getChessType() == PAWN) {                    //���� ������ �������� ������ ���������� ���� �� ���������� ���������
							for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
								if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
									else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
							}
						}

					}

				}
				if(strikeKing == false) {

					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('g', 1));       //��������� ����, ���������� ������� ����� ��������� ��
					possMoves.insert(locMapIt->first);                                                  //��������� ��������� ��� ���� �����
				}

			}
		}
		else if(chColor == BLACK) {             //���������� ��� ������ �����
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('f', 8));
				if(locMapIt->second->getChessType() != NOPIECE)
					freeLine = false;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('g', 8));
				if(locMapIt->second->getChessType() != NOPIECE)
					freeLine = false;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('h', 8));
				if(locMapIt->second->getChessType() != ROOK || locMapIt->second->getIsMoved() == true)
					readyRook = false;
				if(freeLine == true && readyRook == true) {
						for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {
							if(locMapIt->second->getChessColor() == WHITE && locMapIt->second->getChessType() != NOPIECE) {
									locMapIt->second->noCheckMT(locMapIt->first, currDesk);
									if(locMapIt->second->getChessType() != PAWN)
										locSetPtr = locMapIt->second->getPossMoves();
									else {                                                                   //��� ��������� �����
											map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //�������� ��� ������ ������������� ��� ����\������ �����
											locSetPtr = locMapIt->second->getPossMoves();                        //���������� ��������� ����. ��� ���� �����
											if(!locSetPtr->empty()) {                                             //���� ��� ��������
												locSetIt = locSetPtr->begin();                                  //��������� ��� ���� ����� ����� ���� ����, ������� ����������� ���������
																												//��������, ����. �� ���. ��-� ���������
												tmpSet = *locSetPtr;                //�������� ��������� ��������� ��� ���� ����� �� ��������� ��������� ���������
																						//���������� ��� ����, ����� �������� ������������� ����� �� ������� ��������� � �������� ����. ��� .�����
												seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //���������, ���������� �� ���� ������ �� ���������� ��� ������
												if(seekMapIt != currDesk->end())                   //���� ����������, ������ �� ��������� ��������� ��������� ��� ���� �����
													tmpSet.insert(seekMapIt->first);

												seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //���������, ���������� �� ����, ����� �� ���������� ��� ����
												if(seekMapIt != currDesk->end())
													tmpSet.insert(seekMapIt->first);

											}
									}

									if((*locMapIt).second->getChessType() != PAWN) {                     //���� ������ �� �������� ������ ���������� ���� �� ���������-�������� ������
										for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //��� ������� ���� �� ��������� ��������� ��� ������
												if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
													strikeKing = true;
												else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
													strikeKing = true;
										}
									}
									else if((*locMapIt).second->getChessType() == PAWN) {                    //���� ������ �������� ������ ���������� ���� �� ���������� ���������
										for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
											if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
													strikeKing = true;
												else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
													strikeKing = true;
										}
									}
                            }
						}
						if(strikeKing == false) {
							locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('g', 8));       //��������� ����, ���������� ������� ����� ��������� ��
							possMoves.insert(locMapIt->first);                                                  //��������� ��������� ��� ���� �����
						}

				}


		}

		freeLine = true;                //��������� ���������� �������� � �������������� ���������
		readyRook = true;
		strikeKing = false;

		//-------------------------------� ������� ��������� ������-------------------------------------------------------
		if(chColor == WHITE) {                     //��� ����� �����
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('b', 1));
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('c', 1));
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('d', 1));
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('a', 1));
			if(locMapIt->second->getChessType() != ROOK || locMapIt->second->getIsMoved() == true)
				readyRook = false;
			if(freeLine == true && readyRook == true) {
				for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {
					if(locMapIt->second->getChessType() != NOPIECE && locMapIt->second->getChessColor() == BLACK) {
						locMapIt->second->noCheckMT(locMapIt->first, currDesk);
						if(locMapIt->second->getChessType() != PAWN)
							locSetPtr = locMapIt->second->getPossMoves();
						else {                                                                   //��� ��������� �����
								map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //�������� ��� ������ ������������� ��� ����\������ �����
								locSetPtr = locMapIt->second->getPossMoves();                        //���������� ��������� ����. ��� ���� �����
								if(!locSetPtr->empty()) {                                             //���� ��� ��������
									locSetIt = locSetPtr->begin();                                  //��������� ��� ���� ����� ����� ���� ����, ������� ����������� ���������
																									//��������, ����. �� ���. ��-� ���������
									tmpSet = *locSetPtr;                //�������� ��������� ��������� ��� ���� ����� �� ��������� ��������� ���������
																			//���������� ��� ����, ����� �������� ������������� ����� �� ������� ��������� � �������� ����. ��� .�����
									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //���������, ���������� �� ���� ������ �� ���������� ��� ������
									if(seekMapIt != currDesk->end())                   //���� ����������, ������ �� ��������� ��������� ��������� ��� ���� �����
										tmpSet.insert(seekMapIt->first);

									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //���������, ���������� �� ����, ����� �� ���������� ��� ����
									if(seekMapIt != currDesk->end())
										tmpSet.insert(seekMapIt->first);

								}
						}
						if((*locMapIt).second->getChessType() != PAWN) {                     //���� ������ �� �������� ������ ���������� ���� �� ���������-�������� ������
							for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //��� ������� ���� �� ��������� ��������� ��� ������
									if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
							}
						}
						else if((*locMapIt).second->getChessType() == PAWN) {                    //���� ������ �������� ������ ���������� ���� �� ���������� ���������
							for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
								if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 1)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
							}
						}
					}
				}
				if(strikeKing == false) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('c', 1));       //��������� ����, ���������� ������� ����� ��������� ��
					possMoves.insert(locMapIt->first);                                                  //��������� ��������� ��� ���� �����
				}
			}

		}

		else if(chColor == BLACK) {                     //��� ������ �����
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('b', 8));
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('c', 8));
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('d', 8));
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('a', 8));
			if(locMapIt->second->getChessType() != ROOK || locMapIt->second->getIsMoved() == true)
				readyRook = false;
			if(freeLine == true && readyRook == true) {
				for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {
					if(locMapIt->second->getChessType() != NOPIECE && locMapIt->second->getChessColor() == WHITE) {
						locMapIt->second->noCheckMT(locMapIt->first, currDesk);
						if(locMapIt->second->getChessType() == PAWN)
							locSetPtr = locMapIt->second->getPossMoves();
						else {
								map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //�������� ��� ������ ������������� ��� ����\������ �����
																								 //��� ��������� �����
								locSetPtr = locMapIt->second->getPossMoves();                        //���������� ��������� ����. ��� ���� �����
								if(!locSetPtr->empty()) {                                             //���� ��� ��������
									locSetIt = locSetPtr->begin();                                  //��������� ��� ���� ����� ����� ���� ����, ������� ����������� ���������
																									//��������, ����. �� ���. ��-� ���������
									tmpSet = *locSetPtr;                //�������� ��������� ��������� ��� ���� ����� �� ��������� ��������� ���������
																			//���������� ��� ����, ����� �������� ������������� ����� �� ������� ��������� � �������� ����. ��� .�����
									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //���������, ���������� �� ���� ������ �� ���������� ��� ������
									if(seekMapIt != currDesk->end())                   //���� ����������, ������ �� ��������� ��������� ��������� ��� ���� �����
										tmpSet.insert(seekMapIt->first);

									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //���������, ���������� �� ����, ����� �� ���������� ��� ����
									if(seekMapIt != currDesk->end())
										tmpSet.insert(seekMapIt->first);

								}
						}
						if((*locMapIt).second->getChessType() != PAWN) {                     //���� ������ �� �������� ������ ���������� ���� �� ���������-�������� ������
							for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //��� ������� ���� �� ��������� ��������� ��� ������
									if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
							}
						}
						else if((*locMapIt).second->getChessType() == PAWN) {                    //���� ������ �������� ������ ���������� ���� �� ���������� ���������
							for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
								if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 8)                  //���������, �� ��������� �� ��� � ����� �� ����� ����� ���������
										strikeKing = true;
							}
						}
					}
				}
				if(strikeKing == false) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('c', 8));       //��������� ����, ���������� ������� ����� ��������� ��
					possMoves.insert(locMapIt->first);                                                  //��������� ��������� ��� ���� �����
				}
			}


		}
        //----------------------------------------------------------------------------------------------------------------------------------------------


	}

	checkCheck(currSect, currDesk);

	return;

}


void Nopiece::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	;
}

void Pawn::setPrevPos(deskSection* dsPrev) {
	prevPosPtr = dsPrev;
	return;
}

deskSection* Pawn::getPrevPos() const {
		return prevPosPtr;
}

void Rook::setIsMoved(bool f) {
	isMoved = f;
	return;
}

void King::setIsMoved(bool f) {
	isMoved = f;
	return;
}

void King::setCheck(chessDesk* dskPtr) {                    //�����, ��������������� ���� isChecked � ����������� �� �������� ��������� �����
	set<deskSection*, lessSectPtr>* locSetPtr;              //��������� ��������� ��������� �� ��������� �����
	set<deskSection*, lessSectPtr>::iterator locSetIt;

	if(chColor == BLACK) {
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = dskPtr->getWhiteChess();                //��������� � �������������� ��������� ��������� �� ����������� ����� �����
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = dskPtr->getBlackChess();
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;                            		//��������� �������� ��������������� ����-��

		for(tmpWhiteIt = tmpWhiteChess->begin(); tmpWhiteIt != tmpWhiteChess->end(); tmpWhiteIt++) {        //��� ������ ������ �� ����������� ����� �����
			(*tmpWhiteIt).first->noCheckMT((*tmpWhiteIt).second, dskPtr->getMap());
			locSetPtr = (*tmpWhiteIt).first->getPossTakes();                                             //�������� ��������� ��� ������ ����
			for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++)                 //���� �� ��������� ��������� ��� ������ �����,
				if(**locSetIt == *(*tmpBlackChess)[this]) {                                              //���� ���� �� ������� ���������� ������ ������
					isChecked = true;                                                                    //������������� ���� (������� ������� King) ���������� ����
					return;                                                                              //� ��������� true
				}

		}
		isChecked = false;                                                                         //���� ��� ���� ����. ����� ����� ��������� ��� ������ ����� �� ���������
																								  //�� ������ ����, ������������ � �����, �� ������� ���������� ������ ������
																								  //������� ���� ���������� ����
	}
	else if(chColor == WHITE) {                                                                    //���������� ��� ������ ������
			map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = dskPtr->getBlackChess();
			map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = dskPtr->getWhiteChess();
			map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpBlackIt;
			for(tmpBlackIt = tmpBlackChess->begin(); tmpBlackIt != tmpBlackChess->end(); tmpBlackIt++) {
				(*tmpBlackIt).first->noCheckMT((*tmpBlackIt).second, dskPtr->getMap());
				locSetPtr = (*tmpBlackIt).first->getPossTakes();
				for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {
						if(**locSetIt == *(*tmpWhiteChess)[this]) {
							isChecked = true;
							return;
                        }
                }
			}
			isChecked = false;

    }
}


void King::setCheck(map<deskSection*, chessPiece*, sectCompare>* sectMap) {            //���������� ����������� ������, ���������� �������� �� ���������
		map<deskSection*, chessPiece*, sectCompare>::iterator locKingIt;               //�� ���� ������-��������� �����, � ��������� �� ��� ���� - �����������
		map<deskSection*, chessPiece*, sectCompare>::iterator locSectIt;               //map<deskSection*, chessPiece*, sectCompare>
		set<deskSection*, lessSectPtr>* setPtr;
		set<deskSection*, lessSectPtr>::iterator setIt;
		if(this->getChessColor() == BLACK) {
				for(locKingIt = sectMap->begin(); locKingIt != sectMap->end(); locKingIt++) {                           //��������� �������� �������������
					if((*locKingIt).second->getChessColor() == BLACK && (*locKingIt).second->getChessType() == KING)     //�� ��-�, ���� �������� ����� ������, ����. ���� �����
						break;
				}
				for(locSectIt = sectMap->begin(); locSectIt != sectMap->end(); locSectIt++) {
					if((*locSectIt).second->getChessColor() == WHITE && (*locSectIt).second->getChessType() != NOPIECE) {
						 (*locSectIt).second->noCheckMT((*locSectIt).first, sectMap);
						 setPtr = (*locSectIt).second->getPossTakes();
						 for(setIt = setPtr->begin(); setIt != setPtr->end(); setIt++)
							 if(**setIt == *(*locKingIt).first) {
								isChecked = true;
								return;
							 }


					}

				}
		}
		else if(this->getChessColor() == WHITE) {
				for(locKingIt = sectMap->begin(); locKingIt != sectMap->end(); locKingIt++) {
						if((*locKingIt).second->getChessColor() == WHITE &&  (*locKingIt).second->getChessType() == KING)
							break;
				}
				for(locSectIt = sectMap->begin(); locSectIt != sectMap->end(); locSectIt++) {
					if((*locSectIt).second->getChessColor() == BLACK && (*locSectIt).second->getChessType() != NOPIECE) {
						if((*locSectIt).second->getChessType() != KING)
						(*locSectIt).second->noCheckMT((*locSectIt).first, sectMap);
						setPtr = (*locSectIt).second->getPossTakes();
						for(setIt = setPtr->begin(); setIt != setPtr->end(); setIt++) {
								if(**setIt ==  *(*locKingIt).first) {
										isChecked = true;
										return;
								}
                        }
                    }
				}
		}
		isChecked = false;

}

void King::setCheck(bool check) {
	isChecked = check;
	return;
}

bool King::getCheck() const {
		return isChecked;
}

bool Rook::getIsMoved() const {
		return isMoved;

}

bool King::getIsMoved() const {
		return isMoved;
}



