//---------------------------------------------------------------------------

#pragma hdrstop

#include "ChessPaint.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//----------------–еализаци€ ќ“–»—ќ¬ » ƒќ— »---------------------------------------------------------------------------

Graphics::TBitmap* chessDeskPaint::getFieldMap() const {
		return Field;
}
Graphics::TBitmap* chessDeskPaint::getPieceMap() const {
		return Piece;
}

Graphics::TBitmap* chessDeskPaint::getCursorOn() const {
		return cursorOnSect;
}

Graphics::TBitmap* chessDeskPaint::getNoCursorBlack() const {
	return noCursorBlack;
}

Graphics::TBitmap* chessDeskPaint::getNoCursorWhite() const {
	return noCursorWhite;
}

Graphics::TBitmap* chessDeskPaint::getSelectedSect() const {
		return selectedSect;
}
Graphics::TBitmap* chessDeskPaint::getMovesSect() const {
		return possMovesSect;
}

Graphics::TBitmap* chessDeskPaint::getTakesSect() const {
		return possTakesSect;
}

Graphics::TBitmap* chessDeskPaint::getFreeWhite() const {
		return freeWhiteSect;
}

Graphics::TBitmap* chessDeskPaint::getFreeBlack() const {
		return freeBlackSect;
}
chessDeskPaint::chessDeskPaint(TImage* chessDeskImage) {                    //отрисовываем доску в конструкторе


		chDskPtr = &chDsk;
		deskImage = chessDeskImage;                 //сохран€ем объект дл€ отображени€ графики дл€ дальнейших отрисовок
		Field = new Graphics::TBitmap;                //спрайт доски
		Piece = new Graphics::TBitmap;                //спрайт фигуры
		cursorOnSect = new Graphics::TBitmap;          //спрайт выделени€ €чейкт при движении курсора
		noCursorBlack = new Graphics::TBitmap;        //спрайт удалени€ выделени€ с черной клетки
		noCursorWhite = new Graphics::TBitmap;        //спрайт удалени€ выделени€ с белой клетки
		selectedSect = new Graphics::TBitmap;         //спрайт полупрозрачной заливки при клике по клетке
		possMovesSect = new Graphics::TBitmap;
		possTakesSect = new Graphics::TBitmap;
		freeWhiteSect = new Graphics::TBitmap;
		freeBlackSect = new Graphics::TBitmap;

		Field->LoadFromFile("table.bmp");             //подгружаем .bmp в спрайт доски
		cursorOnSect->LoadFromFile("cursor_on_sect.bmp");
		Piece->Transparent = true;                    //делаем спрайт фигуры прозрачным
		cursorOnSect->Transparent = true;             //делаем спрайт выделени€ клетки прозрачным
		cursorOnSect->TransparentColor = clWhite;
		noCursorBlack->LoadFromFile("no_cursor_on_black.bmp");
		noCursorBlack->Transparent = true;
		noCursorBlack->TransparentColor = clWhite;
		noCursorWhite->LoadFromFile("no_cursor_on_white.bmp");
		noCursorWhite->Transparent = true;
		noCursorWhite->TransparentColor = clBlack;
		selectedSect->LoadFromFile("selected_sect.bmp");
		possMovesSect->LoadFromFile("moves_sect.bmp");
		possTakesSect->LoadFromFile("takes_sect.bmp");
		freeWhiteSect->LoadFromFile("free_white_sect.bmp");
		freeBlackSect->LoadFromFile("free_black_sect.bmp");
		chessDeskImage->Canvas->Draw(0, 0, Field);      //отрисовываем доску
		paintPieces();                            		//вызываем метод отрисовки фигур
}

chessDeskPaint::~chessDeskPaint() {
	delete Field;
	delete Piece;
	delete cursorOnSect;
	delete noCursorBlack;
	delete noCursorWhite;
	delete selectedSect;
	delete freeWhiteSect;
	delete freeBlackSect;
	delete possMovesSect;
	delete possTakesSect;
}

void chessDeskPaint::paintPieces() {
	pair<deskSection*, chessPiece*>* temp;         //создаем указатель на временную пару
	map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPtr->getMap();
    chDskPtr->sectItBegin();                        //итератор отображени€ шах. полей устанавливаем в начало
	while(true) {
			temp = chDskPtr->getSection();                //в цикле присваиваем значени€ временной паре <deskSection, chessPiece>
			if(temp == NULL)                      //до последней €чейки
					break;
			deskSection* tempDS = temp->first;         //выдел€ем из пары объект-€чейку
			chessPiece* tempCP = temp->second;         //выдел€ем из пары объект-фигуру

			if(tempDS->sectColor == BLACK)
					deskImage->Canvas->Draw(tempDS->pixelX, tempDS->pixelY, freeBlackSect);
			else if(tempDS->sectColor == WHITE)
					deskImage->Canvas->Draw(tempDS->pixelX, tempDS->pixelY, freeWhiteSect);

			if(tempCP->getChessType() != NOPIECE) {              //если на данной клетке расположена непуста€ фигура
				if(tempDS->sectColor == WHITE)                   //в зависимости от цвета пол€ загружаем изображение дл€ белого пол€/дл€ черного пол€
					Piece->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/" + tempCP->getPicName().c_str());    //загружаем изображение спрайта по его названию
																											 //ExtractFilePath извлекает путь нахождени€ .exe файла проекта в формате AnsiString
																											 //ExtractFilePath необходим, т.к. после использовани€ Save/OpenDialog мен€етс€ текуща папка проекта
				else if(tempDS->sectColor == BLACK)
					Piece->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/" + tempCP->getPicName().c_str());
			}

			else {

				continue;                                   //если расположена пуста€ фигура, переходим к след. итерации цикла, упуска€ отрисовку
			}

			deskImage->Canvas->Draw(tempDS->pixelX, tempDS->pixelY, Piece);
		}
}

void chessDeskPaint::setPieceMap(char* name) {
		Piece->LoadFromFile(name);

}

chessDesk* chessDeskPaint::getChDsk() {
		return chDskPtr;
}

void chessDeskPaint::setChDsk(chessDesk* ptr) {
	chDskPtr = ptr;
	return;
}

void chessDeskPaint::setChDsk() {
	chDskPtr = &chDsk;
	return;
}



