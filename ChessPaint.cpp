//---------------------------------------------------------------------------

#pragma hdrstop

#include "ChessPaint.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//----------------���������� ��������� �����---------------------------------------------------------------------------

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
chessDeskPaint::chessDeskPaint(TImage* chessDeskImage) {                    //������������ ����� � ������������


		chDskPtr = &chDsk;
		deskImage = chessDeskImage;                 //��������� ������ ��� ����������� ������� ��� ���������� ���������
		Field = new Graphics::TBitmap;                //������ �����
		Piece = new Graphics::TBitmap;                //������ ������
		cursorOnSect = new Graphics::TBitmap;          //������ ��������� ������ ��� �������� �������
		noCursorBlack = new Graphics::TBitmap;        //������ �������� ��������� � ������ ������
		noCursorWhite = new Graphics::TBitmap;        //������ �������� ��������� � ����� ������
		selectedSect = new Graphics::TBitmap;         //������ �������������� ������� ��� ����� �� ������
		possMovesSect = new Graphics::TBitmap;
		possTakesSect = new Graphics::TBitmap;
		freeWhiteSect = new Graphics::TBitmap;
		freeBlackSect = new Graphics::TBitmap;

		Field->LoadFromFile("table.bmp");             //���������� .bmp � ������ �����
		cursorOnSect->LoadFromFile("cursor_on_sect.bmp");
		Piece->Transparent = true;                    //������ ������ ������ ����������
		cursorOnSect->Transparent = true;             //������ ������ ��������� ������ ����������
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
		chessDeskImage->Canvas->Draw(0, 0, Field);      //������������ �����
		paintPieces();                            		//�������� ����� ��������� �����
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
	pair<deskSection*, chessPiece*>* temp;         //������� ��������� �� ��������� ����
	map<deskSection*, chessPiece*, sectCompare>* tmpMap = chDskPtr->getMap();
    chDskPtr->sectItBegin();                        //�������� ����������� ���. ����� ������������� � ������
	while(true) {
			temp = chDskPtr->getSection();                //� ����� ����������� �������� ��������� ���� <deskSection, chessPiece>
			if(temp == NULL)                      //�� ��������� ������
					break;
			deskSection* tempDS = temp->first;         //�������� �� ���� ������-������
			chessPiece* tempCP = temp->second;         //�������� �� ���� ������-������

			if(tempDS->sectColor == BLACK)
					deskImage->Canvas->Draw(tempDS->pixelX, tempDS->pixelY, freeBlackSect);
			else if(tempDS->sectColor == WHITE)
					deskImage->Canvas->Draw(tempDS->pixelX, tempDS->pixelY, freeWhiteSect);

			if(tempCP->getChessType() != NOPIECE) {              //���� �� ������ ������ ����������� �������� ������
				if(tempDS->sectColor == WHITE)                   //� ����������� �� ����� ���� ��������� ����������� ��� ������ ����/��� ������� ����
					Piece->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onwhite/" + tempCP->getPicName().c_str());    //��������� ����������� ������� �� ��� ��������
																											 //ExtractFilePath ��������� ���� ���������� .exe ����� ������� � ������� AnsiString
																											 //ExtractFilePath ���������, �.�. ����� ������������� Save/OpenDialog �������� ������ ����� �������
				else if(tempDS->sectColor == BLACK)
					Piece->LoadFromFile(ExtractFilePath(Application->ExeName) + "chess_pieces_onblack/" + tempCP->getPicName().c_str());
			}

			else {

				continue;                                   //���� ����������� ������ ������, ��������� � ����. �������� �����, ������� ���������
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



