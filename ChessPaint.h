//---------------------------------------------------------------------------

#ifndef ChessPaintH
#define ChessPaintH


#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include "ChessBase.h"              //добавляем заголовочный файл базы данных шахмат
class chessDeskPaint {
Graphics::TBitmap* Field;              //спрайт доски
Graphics::TBitmap* Piece;               //спрайт фигуры на фоне поля
Graphics::TBitmap* cursorOnSect;        //спрайт выделения поля (клетки)
Graphics::TBitmap* noCursorBlack;                //спрайт для удаления предыдущего выделения черной клетки
Graphics::TBitmap* noCursorWhite;                //спрайт для удаления предыдущего выделения белой клетки
Graphics::TBitmap* selectedSect;                //спрайт полупрозрачной заливки выделенной клетки
Graphics::TBitmap* possMovesSect;               //спрайт заливки доступных для хода полей
Graphics::TBitmap* possTakesSect;               //спрайт заливки доступных для взятия полей
Graphics::TBitmap* freeWhiteSect;
Graphics::TBitmap* freeBlackSect;
TImage* deskImage;                     //визуальный объект TImage для отображения доски (передается через конструтор)
chessDesk chDsk;                    //определяем объект-базу данных шахмат
chessDesk* chDskPtr;                //объявляем указатель на объект-базу данных шахмат
public:
	chessDeskPaint();
    ~chessDeskPaint();
	chessDeskPaint(TImage*);               //конструктор класса отрисовки шахматного поля
											//аргументом конструктора является указатель объект для отображения шахматной графики
	Graphics::TBitmap* getFieldMap() const;
	Graphics::TBitmap* getPieceMap() const;
	Graphics::TBitmap* getCursorOn() const;
	Graphics::TBitmap* getNoCursorBlack() const;
	Graphics::TBitmap* getNoCursorWhite() const;
	Graphics::TBitmap* getSelectedSect() const;
	Graphics::TBitmap* getMovesSect() const;
    Graphics::TBitmap* getTakesSect() const;
	Graphics::TBitmap* getFreeWhite() const;
    Graphics::TBitmap* getFreeBlack() const;
	void setPieceMap(char*);
	void paintPieces();             //отрисока шахматных фигур на текущем этапе игры
	chessDesk* getChDsk();          //метод, возвращающий указатель на атрибут   chDsk типа chessDesk
	void setChDsk(chessDesk*);
    void setChDsk();

};
//---------------------------------------------------------------------------
#endif
