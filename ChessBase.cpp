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

													   //инициализируем индексы массивов
				int pb = 0, pw =  0;                   //индексы массивов-пешек
				int kb = 0, kw = 0;                     //индексы массивов-коней
				int bb = 0, bw = 0;                     //индексы массивов-слонов
				int rb = 0, rw = 0;                     //индексы массивов-ладей



			   for(int i=0; i < DESKLEN; i++)               //перебираем все эл-ты шахматного поля 8х8
					for(int j=0; j < DESKLEN; j++) {
					 //	deskSection* dSptr = new deskSection(i, j);       //создаем новый объект-ячейку
					 //	dSect[i][j] = *dSptr;                           //заносим созданный объект-ячейку в массив
						dSect[i][j] = deskSection(i,  j);                //инициализируем элементы массива шахматных полей
						deskSection* dSptr = *(dSect+i)+j;              //объявляем  локальный указатель на объект шах. поля
																		//и инициализируем его адресом эл-та двумерного массива dSect
						if(j > 1 && j < 6) {                            //для пустых ячеек (при начальной расстановке шахмат)

							pair<deskSection*, chessPiece*> p(dSptr, &noPiece);     //определяем пару из созданного объекта-ячейки и пустой шахматной фигуры
							sectionsMap.insert(p);                                   //добавляем созданную пару в отображение
						}
 //----------------------------Заносим в базу ПЕШКИ---------------------------------------------------------------------------------------
						else if(j == 1 || j == 6) {
								if(j == 1) {
										pawnBlackArr[pb] = Pawn(BLACK, pb+1);           //присваиваем эл-ту массива объектов черных пешек безымянный объект
																						//с аргументами цвета шахматной фигуры и ее номера
										chessPiece* piecePtr = (pawnBlackArr + pb);      //создаем указатель на объект-пешку, выделяем память и инициализируем

										pb++;                                            //инкрементируем номер элемента массива
										pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);   //создаем пару с объектом-ячейкой и объектом-фигурой
										sectionsMap.insert(p1);                     // заносим созданную пару в отображение
										pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);    //создаем пару с объектом-фигурой и объектом-ячейкой
										blackChess.insert(p2);                                //заносим пару в базу черных фигур, ключом является указатель на фигуру
								}
								else if(j == 6) {                                 //аналогично для ряда белых пешек
										pawnWhiteArr[pw] = Pawn(WHITE, pw+1);
										chessPiece* piecePtr = (pawnWhiteArr + pw);
										pw++;
										pair<deskSection*, chessPiece*> p1(dSptr, piecePtr);
										sectionsMap.insert(p1);
										pair<chessPiece*, deskSection*> p2(piecePtr, dSptr);
                                        whiteChess.insert(p2);
								}
						}
  //-----------------------------Заносим в базу КОНЕЙ--------------------------------------------------------------------------------------------------
						else if(j == 0 && (i == 1 || i == 6)) {         //черные кони
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
 //---------------------------Заносим в базу данных СЛОНОВ---------------------------------------------------------------------------------------
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
//-----------------------------Заносим в базу ЛАДЬИ----------------------------------------------------------------------------------------------
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
//----------------------Заносим а базу ФЕРЗЕЙ--------------------------------------------------------------------------------------
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
//--------------------------------------Заносим в базу КОРОЛЕЙ---------------------------------------------------------------------
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
			   sectionsIt = sectionsMap.begin();                //устанавливаем итератор в начало отображения
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


 chessDesk::~chessDesk() {                                                //деструктор шахматной доски
	sectionsMap.clear();                                              //очищаем множества
	whiteChess.clear();
	blackChess.clear();
	addWhitePieces.clear();
	addBlackPieces.clear();
 }


 pair<deskSection*, chessPiece*>* chessDesk::getSection() {
		static pair<deskSection*, chessPiece*> temp;             //определяем временную пару deskSection и chessPiece
		if(sectionsIt != sectionsMap.end()) {           //если итератор не указывает на облать памяти за посл. эл-ом
				temp = (*sectionsIt);                   //присваиваем значение временной паре
				sectionsIt++;                           //инкрементируем итератор
				return &temp;                            //возвращаем временное значение
		}
		else {
			sectionsIt = sectionsMap.begin();
			return NULL;
		}
 }




 //----------------------------------Реализация ФУНКЦИОНАЛЬНЫХ ОБЪЕКТОВ-------------------------------------------------------------------
 bool sectCompare::operator()(const deskSection* dS1, const deskSection* dS2) const {          //реализация функционального объекта
		if(dS1->symbol < dS2->symbol)                                                         //для сортировки объектов-ячеек
			return true;                                                                    //сортируем по возрастанию по символьной координате
		else if(dS1->symbol == dS2->symbol) {                                               //если символьные координаты равны,
			if(dS1->number < dS2->number)                                                 //сортируем по числовой
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

bool lessSectPtr::operator()(deskSection* const ptr1, deskSection* const ptr2) {      //реализация функционального объекта
																					//для сортировки указателей на deskSection
																					//по индексам ячеек
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

//в нижеследующей условной операции НЕЛЬЗЯ ИСПОЛЬЗОВАТЬ ОПЕРАТОР <=, т.к при обращении к элементу
// отображения (содержащему функтор lessPiecePtr) через ассоциативный массив, будет регистрироваться НОВЫЙ ЭЛЕМЕНТ даже при совпадении
//индекса с УЖЕ СУЩЕСТВУЮШИМИ!!!
	if(piecePtr1->getChessType() < piecePtr2->getChessType())           //если тип фигуры, на которую ссылается указатель piecePtr1,
																		//меньше типа фигуры, на которую ссылается указатель piecePtr2,
																		//(тип фигуры является перечислением), то возвращаем true,
																		//иначе - false
		return true;
	else if(piecePtr1->getChessType() == piecePtr2->getChessType()) {
		if(piecePtr1->getChessNumber() < piecePtr2->getChessNumber())       //если тип фигур одинаковый, сравниваем по номеру
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
	isCoordInSect inSect(X, Y);               //инициализируем функциональный объект
	sectionsIt = find_if(sectionsMap.begin(), sectionsMap.end(), inSect);         //выполняем алгоритм поиска с собственным
																				  //функциональным объектом inSect
	if(sectionsIt != sectionsMap.end())                       //если элемент отображения найден
		localPair = *sectionsIt;                                //присваиваем  локальной паре значение эл-та
	sectionsIt = sectionsMap.begin();                       //присваиваем итератору итератор начала отображения
	return  localPair;                                    //возвращаем пару


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

 chessPiece* chessDesk::pieceFromArr(chessColor color, chessType type, int num) {         //метод, возвр. указатель на шах. фигуру из массива
	 if(color == WHITE) {                                   //если запрашиваемая фигура белая
		 switch(type) {                                     //в зависимости от типа фигуры
		 case PAWN:
			for(int i=0; i < 8; i++) {
				if(pawnWhiteArr[i].getChessNumber() == num)        //возвращаем адрес соответств. элемента соответств. массива
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
	 else if(color == BLACK) {                                  //аналогично для черных фигур
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


 void chessDesk::addPiece(chessPiece* piecePtr) {                       //метод, вставляющий указатель на шах. фигуру а список указ.
																		//дополнительных шах. фигур
	 if(piecePtr->getChessColor() == WHITE)                          //если указатель на белую фигуру
		addWhitePieces.push_back(piecePtr);                         //вставляем в соответ. список указ. на дополнительные белые фигуры
	 else if(piecePtr->getChessColor() == BLACK)                    //если указатель на черную фигуру
		addBlackPieces.push_back(piecePtr);                         //вставляем в соответ. список указ. на дополнительные черные фигуры
	 return;
 }

 void chessDesk::clearAddPiece() {                            //метод, высвобождающий память, занимаемую дополнительными фигурами после их взятия

	 while(true) {
		 for(addPiecesIt = addWhitePieces.begin(); addPiecesIt != addWhitePieces.end(); addPiecesIt++) {       //перебираем эл-ты списка указателей на доп. белые фигуры
			piecesIt = find_if(whiteChess.begin(), whiteChess.end(), piecePairEqual(*addPiecesIt));       //если указателя на данную фигуру
			if(piecesIt == whiteChess.end()) {                                                           // в отображении белых фигур уже нет
			   addWhitePieces.erase(addPiecesIt);                                                      //удаляем указатель из списка указ. на дополнительные белые фигуры
			   delete *addPiecesIt;                                                                   //высвобождаем память, занимаемую доп. фигурой
			   (*addPiecesIt) = NULL;                                                                 //обнуляем указатель на доп. фигуру
			   break;
			}
		 }
		 if(addPiecesIt == addWhitePieces.end())
			break;
	 }

	while(true) {
		for(addPiecesIt = addBlackPieces.begin(); addPiecesIt != addBlackPieces.end(); addPiecesIt++) {    //аналогично для эл-ов списка указателей на доп. черные фигуры
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

 bool chessDesk::addNumberExist(int num, chessColor color) {                          //метод, проверяющий, существует ли фигура с заданным номером
																						//среди дополнительных фигур
	if(color == WHITE) {                                                           //если фигура белая
		for(addPiecesIt = addWhitePieces.begin(); addPiecesIt != addWhitePieces.end(); addPiecesIt++)    //перебираем все эл-ты в списке дополнительных фигур
			if((*addPiecesIt)->getChessNumber() == num)                              //если номер совпадает с заданным
				return true;                                                         //возвращаем true
	}
	else if(color == BLACK) {                                                      //аналогично для черной фигуры
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





deskSection::deskSection(char s, int n) {                   //конструктор структуры-ячейки шах. доски
		symbol = s + 'a';
		number = DESKLEN - n;                                 //численная координата на шахмкатной доске начинается с 1, притом поле A1 расположено
															//в НИЖНЕМ правом углу
		pixelX = TOPLEFTX + s * SQRSIZE;                        //инициализируем пиксельную координату Х ячейки
		pixelY = TOPLEFTY +  n * SQRSIZE;                        //инициализируем пиксельную координату Y ячейки
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

void chessPiece::checkCheck(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {     //метод, проверяющий постановку шаха после хода
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

	bool tmpChecked = ((*tmpSectionsIt).second->getCheck() == true) ? true : false;         			//объявляем и инициализируем локальную переменную для хранения признака постановки шаха

	while(true) {                                                 //необходим внешний цикл, т.к. после удаления эл-та из множества
																	//его структура меняется, а значит, продолжать далее перебор его элементов
																	//во внутреннем цикле НЕЛЬЗЯ! Внутр. цикл. необходимо запускать заново после каждого удаления
		for(locSetIt = possMoves.begin(); locSetIt != possMoves.end(); locSetIt++) {          //из множества доступных для хода полей удаляем поля,
																							  //находящиеся под ударом вражеских фигур (т.к. подставлять короля нельзя)
				map<deskSection*, chessPiece*, sectCompare> tmpMap = *currDesk;              //объявляем и инициализируем локальный объект-доску, для того чтобы сохранить
																							 //состояние шахматной доски до потенциального хода королем
				chessPiece* tmpNopiece = tmpMap[*locSetIt];                           //объявляем и инициализируем указатель на пустую фигуру, любой эл-т массива
																						//с ключом - доступным для хода полем, будет пустым
				tmpMap[*locSetIt] = this;                                            //присваиваем эл-ту массива  полей указатель на короля, вызвавшего
				tmpMap[currSect] = tmpNopiece;                                       //настояший метод, тем самым реализуем потенциальный ход королем
				(*tmpSectionsIt).second->setCheck(&tmpMap);                                                //вызываем метод проверки наличия шаха при новом состоянии доски
				if((*tmpSectionsIt).second->getCheck()) {                                                  //если после потенциального хода, король оказывается под шахом
					possMoves.erase(locSetIt);                                  //исключаем целевое поле этого хода из множество доступных для хода полей
					break;
				}
		}
		if(locSetIt == possMoves.end())
			break;
	}

	while(true) {

		for(locSetIt = possTakes.begin(); locSetIt != possTakes.end(); locSetIt++) {     //аналогично из множества доступных для взятия фигур удаляем поля,
																						//находящиеся под ударом вражеских фигур
			map<deskSection*, chessPiece*, sectCompare> tmpMap = *currDesk;
			map<deskSection*, chessPiece*, sectCompare>::iterator tmpIt;
			for(tmpIt = tmpMap.begin(); tmpIt != tmpMap.end(); tmpIt++) {            //сначала необходимо получить итератор на эл-т - пару,
																						//значением которой является указатель на пустую шах. фигуру
				if((*tmpIt).second->getChessType() == NOPIECE)
					break;
			}
			tmpMap[*locSetIt] = this;                                               //во множестве полей потенциальному для ходу полю присваиваем указатель на короля,
																					 //вызвавшего метод
			tmpMap[currSect] = (*tmpIt).second;                                    //полю, с которого совершается ход, присваиваем соответственно пустую фигуру
			(*tmpSectionsIt).second->setCheck(&tmpMap);                                                    //проверяем наличие шаха при новом состоянии доски
			if((*tmpSectionsIt).second->getCheck()) {                                                       //если после потенциального хода, король оказывается под шахом
				possTakes.erase(locSetIt);                                       //исключаем целевое поле этого хода тиз множества доступных для хода полей
				break;
			}
		}
		if(locSetIt == possTakes.end())
			break;
	}

	(*tmpSectionsIt).second->setCheck(tmpChecked);                                                 //после проверки потенциальных ходов, возвращаем флаг постановки шаха в исходное значение
	return;

}

 void Pawn::noCheckMT(deskSection* currSect,          //метод заполняет множество полей доступных для хода и для взятия пешкой
								map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	int tmpNum;
	char tmpSym;
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;            //объявляем локальный итератор отображения  currDesk (передается ч. аргум.)
	map<deskSection*, chessPiece*, sectCompare>::iterator secondIt;            //объявляем дополнительный локальный итератор отображения currDesk

    possMoves.clear();                                  //перед вставкой элементов в множества доступных для хода и взятия полей
	possTakes.clear();                                  //очищаем данные множества от эл-ов для предыдущего положения фигуры


	for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {          //ищем в отображении currDesk пару, ключ которой (deskSection*)
			if(*currSect == *(locMapIt->first)) {                                        //равен аргументу currSect
				break;

			}
	}
	if(locMapIt != currDesk->end()) {                                      //если таковая пара найдена
			if(locMapIt->second->getChessColor() == BLACK) {          //для черной пешки
 //--------------------------------------------Доступные для взятия поля------------------------------------------------------------------------------------------------
				tmpSym = currSect->symbol - 1;                     //юго-западная диагональ взятия
				tmpNum = currSect->number - 1;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));  //если есть занятое фигурой белого цвета поле,
				if(locMapIt != currDesk->end())                                                                 //т.к. черные рубят белых
					possTakes.insert(locMapIt->first);                                               //добавляем его к множеству доступных для взяти полей
			 //---------------------реализация взятия пешки на проходе-------------------------------------------------------------------------------
				if(currSect->number == 4) {                                  //если пешка расположена на 4-ой диагонали
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 4));     //итератор на пару, содержащую фигуру,
																													//которую предположительно можно взять на проходе
					secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 2));     //итератор на пару, содержащую поле,
																													//которое является предыдущим местом расположения,
																													//этой фигуры
					if(locMapIt != currDesk->end()) {                                                            //если пара с такими шахматными координатами существует
						if((*locMapIt).second->getChessType() == PAWN && (*locMapIt).second->getChessColor() == WHITE)      //если на месте предполагаемой фигуры расположена белая пешка
							if((*locMapIt).second->getPrevPos() == (*secondIt).first) {                                  //если предыдущим полем этой пешки является допустимое для взятия на проходе поле
								secondIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual(tmpSym, 3));       //указатель на поле, на которое перемещается пешка
																														//при взятии на проходе вражеской пешки
								possMoves.insert(secondIt->first);                           //помещаем поле, на которое необходимо совершить ход для взятия на проходе, во множество
							}                                                                //доступных для хода полей
					}
				}
			 //------------------------------------------------------------------------------------------------------------------------------------------------

				tmpSym = currSect->symbol + 1;
				tmpNum = currSect->number - 1;                       //аналогично для юго-восточной диагонали
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));    //проверяем доступность поля для взятия
				if(locMapIt != currDesk->end())                                                                   //при помощи функционального объекта sectTakeEnable
					possTakes.insert(locMapIt->first);
				//-----------------------реализация взятия пешки на проходе---------------------------------------------------------------------------------------
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


//------------------------------------------Доступные для хода поля-----------------------------------------------------------------------------------
				if(currSect->number != 7) {                    //для хода отличного от первого
					tmpSym = currSect->symbol;                  //символьный индекс оставляем без изменений, т.к. пешки ходят по вертикали
					tmpNum = currSect->number - 1;              //белая пешка ходит на одно поле наверх
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));     //если есть свободное поле с коорд. tNum, tSym
					if(locMapIt != currDesk->end())
						possMoves.insert(locMapIt->first);                            //добавляем указатель на него (deskSection*) к множеству  доступных
																					  //для хода полей

				}
				else {                                         //для первого хода
                    tmpSym = currSect->symbol;
					tmpNum = currSect->number-1;             //на одну
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
					if(locMapIt != currDesk->end())
						possMoves.insert(locMapIt->first);
					else
						return;                    //если на одну клетку ход не доступен, то завершаем метод, т.к. пешка
															//не может перепрыгивать фигуры
					tmpSym = currSect->symbol;
					tmpNum = currSect->number - 2;             //на две клетки
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
					if(locMapIt != currDesk->end())
						possMoves.insert(locMapIt->first);


                }


			}
			else if(locMapIt->second->getChessColor() == WHITE) {         //для белой пешки
//--------------------------------------------Доступные для взятия поля------------------------------------------------------------------------------------------------
					tmpSym = currSect->symbol - 1;                     //северо-западная диагональ взятия
					tmpNum = currSect->number + 1;
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));   //если есть занятое фигурой белого цвета поле,
																												   //т.к. черные рубят белых
					if(locMapIt != currDesk->end())
						possTakes.insert(locMapIt->first);                                               //добавляем его к множеству доступных для взяти полей
				//-----------------------реализация взятия пешки на проходе---------------------------------------------------------------------------------------
					if(currSect->number == 5) {                                                   //если пешка расположена на 5-ой горизонтали
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
					tmpNum = currSect->number + 1;                       //аналогично для северо-восточной диагонали
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));
					if(locMapIt != currDesk->end())
						possTakes.insert(locMapIt->first);
                //-----------------------реализация взятия пешки на проходе---------------------------------------------------------------------------------------
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


//--------------------------------------------Доступные для хода поля----------------------------------------------------------------------------------------
					if(currSect->number != 2) {                         //для хода отличного от первого
							tmpSym = currSect->symbol;
							tmpNum = currSect->number + 1;
							locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));      //проверяем существует ли поле с нужными координатами и свободно ли оно
							if(locMapIt != currDesk->end())                                                   //если существует
								possMoves.insert(locMapIt->first);                                               //заносим его в локальное множество доступных для хода полей
					}
					else {                                           //для первого хода
                        tmpSym = currSect->symbol;
						tmpNum = currSect->number + 1;              //на одну клетку
						locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
						if(locMapIt != currDesk->end())
							possMoves.insert(locMapIt->first);
						else
							return;
						tmpSym = currSect->symbol;
						tmpNum = currSect->number + 2;              //на две клетки
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

void Knight::noCheckMT(deskSection* currSect,                   //метод возвращает множество полей (клеток) доступных для хода конем
														map<deskSection*, chessPiece*, sectCompare>* currDesk) {         //для "пустой фигуры" (чистого виртуального метода) реализация метода не требуется
		 int tmpNum;            //временный локальный номер ячейки
		 char tmpSym;           //временный локальный символ ячейки

		 map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;   //создаем локальный итератор отображения currDesk

		 possMoves.clear();                                  //перед вставкой элементов в множества доступных для хода и взятия полей
		 possTakes.clear();                                  //очищаем данные множества от эл-ов для предыдущего положения фигуры

//-----------------------------проверяем доступность 1-го возможного ходя или взятия конем---------------------------------------
		 tmpNum = currSect->number - 1;
		 tmpSym = currSect->symbol - 2;
		 locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		 if(locMapIt != currDesk->end())                       //если подходящяя пара есть
			possMoves.insert((*locMapIt).first);                 //вставляем в локальное множество ключ пары,
															//т.е указатель на deskSection
		 if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//----------------------------проверяем доступность 2-го возможного хода или взятия конем----------------------------------
		 tmpNum = currSect->number - 2;
		 tmpSym = currSect->symbol - 1;
		 locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));            //проверяем, доступна ли ячейка для хода
		 if(locMapIt != currDesk->end())                                                 //если да,
			possMoves.insert((*locMapIt).first);                                            //то добавляем ее в множество доступных для хода полей
          if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//---------------------------проверяем доступность 3-го возможного хода или взятия конем----------------------------
		tmpNum = currSect->number - 1;
		tmpSym = currSect->symbol + 2;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//----------------------------проверяем доступность 4-го возможного хода или взятия конем----------------------------
		tmpNum = currSect->number - 2;
		tmpSym = currSect->symbol + 1;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//----------------------------проверяем доступность 5-го возможного хода или взятия конем-----------------------------
		tmpNum = currSect->number + 1;
		tmpSym = currSect->symbol - 2;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//--------------------------проверяем доступность 6-го возможного хода или взятия конем---------------------------------
		tmpNum = currSect->number + 2;
		tmpSym = currSect->symbol - 1;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));       //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//-----------------------проверяем доступность 7-го возмодного хода или взятия--------------------------------------------
		tmpNum = currSect->number + 1;
		tmpSym = currSect->symbol + 2;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
		 if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
//-------------------проверяем доступность 8-го возможного хода или взятия--------------------------------------

		tmpNum = currSect->number + 2;
		tmpSym = currSect->symbol + 1;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(tmpSym, tmpNum));
		if(locMapIt != currDesk->end())
			possMoves.insert((*locMapIt).first);
         if(chColor == BLACK) {                           //если конь черный
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, WHITE));        //проверяем, доступна ли поле для взятия
																												//(должно содержать фигуру противоположного цвета)
			if(locMapIt != currDesk->end())
				possTakes.insert((*locMapIt).first);
		 }
		 else if(chColor == WHITE) {
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(tmpSym, tmpNum, BLACK));        //проверяем, доступна ли поле для взятие
																												//(должно содержать фигуру противоположного цвета)
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


void Bishop::noCheckMT(deskSection* currSect,                 //метод возвращает множество полей доступных для хода слоном
													map<deskSection*, chessPiece*, sectCompare>* currDesk) {
		map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt = currDesk->begin();
		int tmpNum = currSect->number;                              //координаты текущего положения фигуры
		char tmpSym = currSect->symbol;
		int checkNum = tmpNum;                                     //проверяемая на доступность ячейка, начальные координаты которой равны координатам фигуры
		char checkSym = tmpSym;

        possMoves.clear();                                  //перед вставкой элементов в множества доступных для хода и взятия полей
		possTakes.clear();                                  //очищаем данные множества от эл-ов для предыдущего положения фигуры


		for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {           //если текущее поле фигуры имеется в отображении полей доски currDesk
			if(currSect == locMapIt->first)
				break;
		}
//-----------------Луч на юго-запад---------------------------------------------------------------------------
		while(locMapIt != currDesk->end()) {                                //как только на луче попадется несуществующее или занятое поле, он прервется
			checkNum--;
			checkSym--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
		}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
        locMapIt = currDesk->begin();                                    //возвращаем итератор к началу множества
		checkNum = tmpNum;                                               //координатами проверяемого поля снова назначаем координаты выбранной фигуры
		checkSym = tmpSym;
//-----------------Луч на юго-восток----------------------------------------------------------------
		while(locMapIt != currDesk->end()) {
				checkNum--;
				checkSym++;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
				if(locMapIt != currDesk->end())
					possMoves.insert(locMapIt->first);
		}
//-------------------Взятие фигуры на конце луча--------------------------------------------------------------
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
//-----------------Луч на северо-запад--------------------------------------------------------------------

		while(locMapIt != currDesk->end()) {
				checkNum++;
				checkSym--;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
				if(locMapIt != currDesk->end())
					possMoves.insert(locMapIt->first);
		}
//-----------------Взятие фигуры на конце луча--------------------------------------------------------------
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
//----------------Луч на северо-восток---------------------------------------------------------------------
		while(locMapIt != currDesk->end()) {
				checkNum++;
				checkSym++;
				locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
				if(locMapIt != currDesk->end())
					possMoves.insert(locMapIt->first);
		}
//----------------------Взятие фигуры на конце луча---------------------------------------------------------------
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

void Rook::noCheckMT(deskSection* currSect,                  //метод возвращает множество полей доступных для хода ладьей
													map<deskSection*, chessPiece*, sectCompare>* currDesk) {
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	int tmpNum = currSect->number;
	char tmpSym = currSect->symbol;
	int checkNum = tmpNum;
	char checkSym = tmpSym;

	possMoves.clear();                                  //перед вставкой элементов в множества доступных для хода и взятия полей
	possTakes.clear();                                  //очищаем данные множества от эл-ов для предыдущего положения фигуры

	for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {
		if(locMapIt->first == currSect)
			break;
	}

//---------------------Луч на север--------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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

//--------------------Луч на восток---------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);

	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//-----------------------Луч на юг------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//----------------------Луч на запад-------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
		checkSym--;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
		if(locMapIt != currDesk->end())
			possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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

    possMoves.clear();                                  //перед вставкой элементов в множества доступных для хода и взятия полей
	possTakes.clear();                                  //очищаем данные множества от эл-ов для предыдущего положения фигуры

	for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {
		if(locMapIt->first == currSect)
			break;
	}

//---------------------Луч на север--------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//--------------------Луч на восток---------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
	//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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

//-----------------------Луч на юг------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//----------------------Луч на запад-------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
		checkSym--;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
		if(locMapIt != currDesk->end())
			possMoves.insert(locMapIt->first);
}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//-----------------Луч на юго-запад---------------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {                                //как только на луче попадется несуществующее или занятое поле, он прервется
		checkNum--;
		checkSym--;
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
		if(locMapIt != currDesk->end())
			possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//---------------Луч на юго-восток----------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum--;
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//-----------------Луч на северо-запад--------------------------------------------------------------------

	while(locMapIt != currDesk->end()) {
			checkNum++;
			checkSym--;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
//----------------Луч на северо-восток---------------------------------------------------------------------
	while(locMapIt != currDesk->end()) {
			checkNum++;
			checkSym++;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
			if(locMapIt != currDesk->end())
				possMoves.insert(locMapIt->first);
	}
//---------------Взятие фигуры на конце луча---------------------------------------------------------------
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
void King::noCheckMT(deskSection* currSect,                     //метод для вставки эл-ов во множества доступных для хода и взятия фигур
													map<deskSection*, chessPiece*, sectCompare>* currDesk) {        //без проверки  шаха королю той стороны, которая выполняет ход
	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	char tmpSym = currSect->symbol;
	int tmpNum = currSect->number;
	char checkSym = tmpSym;
	int checkNum = tmpNum;

    possMoves.clear();                                  //перед вставкой элементов в множества доступных для хода и взятия полей
	possTakes.clear();                                  //очищаем данные множества от эл-ов для предыдущего положения фигуры

//--------------------Ход на юго-запад------------------------------------------------------
	checkSym--;
	checkNum--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------Взятие фигуры--------------------------------------------------------------
	if(chColor == BLACK)                                   //для черного короля
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)                              //для белого короля
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkSym = tmpSym;
	checkNum = tmpNum;
//--------------------Ход на юг--------------------------------------------------------------
	checkNum--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------Взятие фигуры--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
//-------------------Ход на юго-восток--------------------------------------------------------
	checkNum--;
	checkSym++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------Взятие фигуры--------------------------------------------------------------
    if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
	checkSym = tmpSym;
//---------------------Ход на восток-----------------------------------------------------------
	checkSym++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
	//-----------Взятие фигуры--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkSym = tmpSym;
//---------------------Ход на северо-восток--------------------------------------------------------
	checkSym++;
	checkNum++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------Взятие фигуры--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
	checkSym = tmpSym;
//---------------------Ход на север-----------------------------------------------------------------
	checkNum++;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------Взятие фигуры--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
//--------------------------Ход на северо-запад--------------------------------------------------------
	checkNum++;
	checkSym--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------Взятие фигуры--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);
	checkNum = tmpNum;
	checkSym = tmpSym;
//--------------------------Хода на запад--------------------------------------------------
	checkSym--;
	locMapIt = find_if(currDesk->begin(), currDesk->end(), sectEnable(checkSym, checkNum));
	if(locMapIt != currDesk->end())
		possMoves.insert(locMapIt->first);
    //-----------Взятие фигуры--------------------------------------------------------------
	if(chColor == BLACK)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, WHITE));
	else if(chColor == WHITE)
		locMapIt = find_if(currDesk->begin(), currDesk->end(), sectTakeEnable(checkSym, checkNum, BLACK));
	if(locMapIt != currDesk->end())
		possTakes.insert(locMapIt->first);


	return;

}

void King::setMovesTakes(deskSection* currSect, map<deskSection*, chessPiece*, sectCompare>* currDesk) {     //метод для вставки эл-ов во множества доступных
	noCheckMT(currSect, currDesk);                                                                          //для хода и взятия полей с проверкой шаха

	map<deskSection*, chessPiece*, sectCompare>::iterator locMapIt;
	set<deskSection*, lessSectPtr>* locSetPtr;                     //объявляем локальный указатель на множество полей
	set<deskSection*, lessSectPtr> tmpSet;                      //объявлдяем временное множество полей
	set<deskSection*, lessSectPtr>::iterator locSetIt;            //объявляем локальный итератор множества set<deskSection*, lessPiecePtr>


    	//----------------------Реализация рокировки--------------------------------------------------------
	if(isMoved == false && isChecked == false) {              //Если не было ходов королем
		bool freeLine = true;                  //объявляем и инициализируем флаг, принимающий значение true в случае, если линия рокировки свободна
		bool readyRook = true;              //объявляем и инициализируем флаг готовности соответств. ладьи к рокировке
		bool strikeKing = false;             //объявляем и инициализируем флаг, принимающий знач. true, если одно из полей,
											//по которым перемещается король в ходе рокировки, находится под ударом
		//-----------------в сторону королевского фланга--------------------------------------------------------
		if(chColor == WHITE) {            //для  белых фигур
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('f', 1));               //проверяем, свободны ли поля на линии рокировки
			if(locMapIt->second->getChessType() != NOPIECE)                                          //если нет, то
				freeLine = false;                                                                 //устанавливаем флаг свободы линии рокировки в положение false
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('g', 1));       //аналогично для след. поля
			if(locMapIt->second->getChessType() != NOPIECE)
				freeLine = false;
			locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('h', 1));       //устанавливаем итератор на эл-т, ключом которого является поле, на кот. расположена
																								  //ладья для рокировки
			if(locMapIt->second->getChessType() != ROOK || locMapIt->second->getIsMoved() == true)   //если ее нет на поле, или ей уже был совершен ход
				readyRook = false;                                                                  //устанавливаем флаг гтовности ладьи в положение false
			if(freeLine == true && readyRook == true) {                                         //если линия рокировки в сторону королевского флага свободна и ладья готова к рокировке
				for(locMapIt = currDesk->begin(); locMapIt != currDesk->end(); locMapIt++) {        //перебираем все фигуры
					if(locMapIt->second->getChessColor() == BLACK && locMapIt->second->getChessType() != NOPIECE) {         //для каждой из вражеских фигур
						locMapIt->second->noCheckMT(locMapIt->first, currDesk);                       //в противном случае используем метод без проверки нахождения под шахом
						if(locMapIt->second->getChessType() != PAWN)
							locSetPtr = locMapIt->second->getPossMoves();                                     //получаем множество доступных для хода полей
																											//если фигура не является пешкой, то направляения взятия совп. с напра. хода
						else {                                                                   //для вражеской пешки
							map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //итератор для поиска потенциальных для хода\взятия полей
							locSetPtr = locMapIt->second->getPossMoves();                        //возвращаем множество дост. для хода фигур
							if(!locSetPtr->empty()) {                                             //если оно непустое
								locSetIt = locSetPtr->begin();                                  //доступное для хода может всего одно поле, поэтому присваиваем итератору
																								//значение, указ. на нач. эл-т множества
								tmpSet = *locSetPtr;                //копируем множество доступных для хода полей во временное локальное множество
																		//необходимо для того, чтобы проверка потенциальных ходов не вносила изменение в реальное сост. шах .доски
								seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //проверяем, существует ли поле справа от доступного для взятия
								if(seekMapIt != currDesk->end())                   //если существует, вносим во временное множество доступных для хода полей
									tmpSet.insert(seekMapIt->first);

								seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //проверяем, существует ли поле, слева от доступного для хода
								if(seekMapIt != currDesk->end())
									tmpSet.insert(seekMapIt->first);

							}
						}

						if((*locMapIt).second->getChessType() != PAWN) {                     //если фигура не является пешкой перебираем поля из множества-атрибута класса
							for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //для каждого поля из множества доступных для взятия
									if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
									else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
							}
						}
						else if((*locMapIt).second->getChessType() == PAWN) {                    //если фигура является пешкой перебираем поля из локального множества
							for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
								if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
									else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
							}
						}

					}

				}
				if(strikeKing == false) {

					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('g', 1));       //добавляем поле, занимаемое королем после рокировки во
					possMoves.insert(locMapIt->first);                                                  //множество доступных для хода полей
				}

			}
		}
		else if(chColor == BLACK) {             //аналогично для черных фигур
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
									else {                                                                   //для вражеской пешки
											map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //итератор для поиска потенциальных для хода\взятия полей
											locSetPtr = locMapIt->second->getPossMoves();                        //возвращаем множество дост. для хода фигур
											if(!locSetPtr->empty()) {                                             //если оно непустое
												locSetIt = locSetPtr->begin();                                  //доступное для хода может всего одно поле, поэтому присваиваем итератору
																												//значение, указ. на нач. эл-т множества
												tmpSet = *locSetPtr;                //копируем множество доступных для хода полей во временное локальное множество
																						//необходимо для того, чтобы проверка потенциальных ходов не вносила изменение в реальное сост. шах .доски
												seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //проверяем, существует ли поле справа от доступного для взятия
												if(seekMapIt != currDesk->end())                   //если существует, вносим во временное множество доступных для хода полей
													tmpSet.insert(seekMapIt->first);

												seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //проверяем, существует ли поле, слева от доступного для хода
												if(seekMapIt != currDesk->end())
													tmpSet.insert(seekMapIt->first);

											}
									}

									if((*locMapIt).second->getChessType() != PAWN) {                     //если фигура не является пешкой перебираем поля из множества-атрибута класса
										for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //для каждого поля из множества доступных для взятия
												if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
													strikeKing = true;
												else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
													strikeKing = true;
										}
									}
									else if((*locMapIt).second->getChessType() == PAWN) {                    //если фигура является пешкой перебираем поля из локального множества
										for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
											if((*locSetIt)->symbol == 'f' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
													strikeKing = true;
												else if((*locSetIt)->symbol == 'g' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
													strikeKing = true;
										}
									}
                            }
						}
						if(strikeKing == false) {
							locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('g', 8));       //добавляем поле, занимаемое королем после рокировки во
							possMoves.insert(locMapIt->first);                                                  //множество доступных для хода полей
						}

				}


		}

		freeLine = true;                //локальные переменные приводим к первоначальным значениям
		readyRook = true;
		strikeKing = false;

		//-------------------------------В сторону ферзевого фланга-------------------------------------------------------
		if(chColor == WHITE) {                     //для белых фигур
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
						else {                                                                   //для вражеской пешки
								map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //итератор для поиска потенциальных для хода\взятия полей
								locSetPtr = locMapIt->second->getPossMoves();                        //возвращаем множество дост. для хода фигур
								if(!locSetPtr->empty()) {                                             //если оно непустое
									locSetIt = locSetPtr->begin();                                  //доступное для хода может всего одно поле, поэтому присваиваем итератору
																									//значение, указ. на нач. эл-т множества
									tmpSet = *locSetPtr;                //копируем множество доступных для хода полей во временное локальное множество
																			//необходимо для того, чтобы проверка потенциальных ходов не вносила изменение в реальное сост. шах .доски
									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //проверяем, существует ли поле справа от доступного для взятия
									if(seekMapIt != currDesk->end())                   //если существует, вносим во временное множество доступных для хода полей
										tmpSet.insert(seekMapIt->first);

									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //проверяем, существует ли поле, слева от доступного для хода
									if(seekMapIt != currDesk->end())
										tmpSet.insert(seekMapIt->first);

								}
						}
						if((*locMapIt).second->getChessType() != PAWN) {                     //если фигура не является пешкой перебираем поля из множества-атрибута класса
							for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //для каждого поля из множества доступных для взятия
									if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
							}
						}
						else if((*locMapIt).second->getChessType() == PAWN) {                    //если фигура является пешкой перебираем поля из локального множества
							for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
								if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 1)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
							}
						}
					}
				}
				if(strikeKing == false) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('c', 1));       //добавляем поле, занимаемое королем после рокировки во
					possMoves.insert(locMapIt->first);                                                  //множество доступных для хода полей
				}
			}

		}

		else if(chColor == BLACK) {                     //для черных фигур
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
								map<deskSection*, chessPiece*, sectCompare>::iterator seekMapIt;          //итератор для поиска потенциальных для хода\взятия полей
																								 //для вражеской пешки
								locSetPtr = locMapIt->second->getPossMoves();                        //возвращаем множество дост. для хода фигур
								if(!locSetPtr->empty()) {                                             //если оно непустое
									locSetIt = locSetPtr->begin();                                  //доступное для хода может всего одно поле, поэтому присваиваем итератору
																									//значение, указ. на нач. эл-т множества
									tmpSet = *locSetPtr;                //копируем множество доступных для хода полей во временное локальное множество
																			//необходимо для того, чтобы проверка потенциальных ходов не вносила изменение в реальное сост. шах .доски
									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol + 1, (*locSetIt)->number));   //проверяем, существует ли поле справа от доступного для взятия
									if(seekMapIt != currDesk->end())                   //если существует, вносим во временное множество доступных для хода полей
										tmpSet.insert(seekMapIt->first);

									seekMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual((*locSetIt)->symbol - 1, (*locSetIt)->number));;  //проверяем, существует ли поле, слева от доступного для хода
									if(seekMapIt != currDesk->end())
										tmpSet.insert(seekMapIt->first);

								}
						}
						if((*locMapIt).second->getChessType() != PAWN) {                     //если фигура не является пешкой перебираем поля из множества-атрибута класса
							for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++) {       //для каждого поля из множества доступных для взятия
									if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
							}
						}
						else if((*locMapIt).second->getChessType() == PAWN) {                    //если фигура является пешкой перебираем поля из локального множества
							for(locSetIt = tmpSet.begin(); locSetIt != tmpSet.end(); locSetIt++) {
								if((*locSetIt)->symbol == 'd' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
									else if((*locSetIt)->symbol == 'c' && (*locSetIt)->number == 8)                  //проверяем, не совпадает ли оно с одним из полей линии рокировки
										strikeKing = true;
							}
						}
					}
				}
				if(strikeKing == false) {
					locMapIt = find_if(currDesk->begin(), currDesk->end(), sectCoordEqual('c', 8));       //добавляем поле, занимаемое королем после рокировки во
					possMoves.insert(locMapIt->first);                                                  //множество доступных для хода полей
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

void King::setCheck(chessDesk* dskPtr) {                    //метод, устанавливающий флаг isChecked в зависимости от текущего состояния доски
	set<deskSection*, lessSectPtr>* locSetPtr;              //объявляем локальный указатель на множество полей
	set<deskSection*, lessSectPtr>::iterator locSetIt;

	if(chColor == BLACK) {
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpWhiteChess = dskPtr->getWhiteChess();                //объявляем и инициализируем локальный указатель на отображение белых фигур
		map<chessPiece*, deskSection*, lessPiecePtr>* tmpBlackChess = dskPtr->getBlackChess();
		map<chessPiece*, deskSection*, lessPiecePtr>::iterator tmpWhiteIt;                            		//объявляем итератор вышеобъявленого множ-ва

		for(tmpWhiteIt = tmpWhiteChess->begin(); tmpWhiteIt != tmpWhiteChess->end(); tmpWhiteIt++) {        //для каждой фигуры из отображения белых фигур
			(*tmpWhiteIt).first->noCheckMT((*tmpWhiteIt).second, dskPtr->getMap());
			locSetPtr = (*tmpWhiteIt).first->getPossTakes();                                             //получаем доступные для взятия поля
			for(locSetIt = locSetPtr->begin(); locSetIt != locSetPtr->end(); locSetIt++)                 //если во множестве доступных для взятия полей,
				if(**locSetIt == *(*tmpBlackChess)[this]) {                                              //есть поле на котором расположен черный король
					isChecked = true;                                                                    //устанавливаем флаг (атрибут объекта King) постановки шаха
					return;                                                                              //в положение true
				}

		}
		isChecked = false;                                                                         //если для всех враж. фигур среди доступных для взятия фигур не оказалось
																								  //ни одного поля, совпадающего с полем, на котором расположен черный король
																								  //снимаем флаг постановки шаха
	}
	else if(chColor == WHITE) {                                                                    //аналогично для белого короля
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


void King::setCheck(map<deskSection*, chessPiece*, sectCompare>* sectMap) {            //перегрузка предыдущего метода, аргументом является не указатель
		map<deskSection*, chessPiece*, sectCompare>::iterator locKingIt;               //на весь объект-шахматную доску, а указатель на его поле - отображение
		map<deskSection*, chessPiece*, sectCompare>::iterator locSectIt;               //map<deskSection*, chessPiece*, sectCompare>
		set<deskSection*, lessSectPtr>* setPtr;
		set<deskSection*, lessSectPtr>::iterator setIt;
		if(this->getChessColor() == BLACK) {
				for(locKingIt = sectMap->begin(); locKingIt != sectMap->end(); locKingIt++) {                           //локальный итератор устанавливаем
					if((*locKingIt).second->getChessColor() == BLACK && (*locKingIt).second->getChessType() == KING)     //на эл-т, ключ которого равен королю, вызв. этот метод
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



