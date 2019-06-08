//---------------------------------------------------------------------------

#ifndef ChessPaintH
#define ChessPaintH


#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include "ChessBase.h"              //��������� ������������ ���� ���� ������ ������
class chessDeskPaint {
Graphics::TBitmap* Field;              //������ �����
Graphics::TBitmap* Piece;               //������ ������ �� ���� ����
Graphics::TBitmap* cursorOnSect;        //������ ��������� ���� (������)
Graphics::TBitmap* noCursorBlack;                //������ ��� �������� ����������� ��������� ������ ������
Graphics::TBitmap* noCursorWhite;                //������ ��� �������� ����������� ��������� ����� ������
Graphics::TBitmap* selectedSect;                //������ �������������� ������� ���������� ������
Graphics::TBitmap* possMovesSect;               //������ ������� ��������� ��� ���� �����
Graphics::TBitmap* possTakesSect;               //������ ������� ��������� ��� ������ �����
Graphics::TBitmap* freeWhiteSect;
Graphics::TBitmap* freeBlackSect;
TImage* deskImage;                     //���������� ������ TImage ��� ����������� ����� (���������� ����� ����������)
chessDesk chDsk;                    //���������� ������-���� ������ ������
chessDesk* chDskPtr;                //��������� ��������� �� ������-���� ������ ������
public:
	chessDeskPaint();
    ~chessDeskPaint();
	chessDeskPaint(TImage*);               //����������� ������ ��������� ���������� ����
											//���������� ������������ �������� ��������� ������ ��� ����������� ��������� �������
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
	void paintPieces();             //�������� ��������� ����� �� ������� ����� ����
	chessDesk* getChDsk();          //�����, ������������ ��������� �� �������   chDsk ���� chessDesk
	void setChDsk(chessDesk*);
    void setChDsk();

};
//---------------------------------------------------------------------------
#endif
