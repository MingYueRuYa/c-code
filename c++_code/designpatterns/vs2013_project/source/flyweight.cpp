/****************************************************************************
**
** Copyright (C) 2017 liushixiongcpp@163.com
** All rights reserved.
**
****************************************************************************/

/*
* ��Ԫģʽ�����ù�������Ч��֧�ִ���ϸ���ȵĶ���
* ����Ԫģʽ�У��������ǳ���Ҫ�ĸ���ڲ�״̬���ⲿ״̬��
* �ڲ�״̬�洢��flyweight�У��������˶�����flyweight��������Ϣ����Щ��Ϣʹ��flyweight���Ա�����
* ���ⲿ״̬ȡ����flyweight�����������ݳ������仯����˲��ɹ����û��������ڱ�Ҫ��ʱ���ⲿ״̬���ݸ�flyweight��
*/

#ifndef flyweight_h
#define flyweight_h

#include <iostream>
#include <string>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::vector;

namespace flyweight
{

//external state
class Position
{
public:
    Position(const int &pX, const int &pY): mX(pX), mY(pY) {}

    ~Position() {}

    int GetX() const { return mX; }

    int GetY() const { return mY; }

private:
    int mX;
    int mY;
};

typedef enum PieceColor
{
    White = 0,
    Black
};

class Piece
{
public:
    Piece(const PieceColor &pPieceColor):mPieceColor(pPieceColor), mPosition(nullptr) {}

    ~Piece() {}

    void SetPoint(const Position *pPosition)
    {
        mPosition = pPosition;
    }

    void Show()
    {
        if (nullptr == mPosition) { return; }
        
        if (White == mPieceColor) {
            cout << "white piece ";
        } else if (Black == mPieceColor) {
            cout << "black piece ";
        }
        cout << "position x:"  << mPosition->GetX() << ", y:" << mPosition->GetY() << endl;
    }

private:
    const Position * mPosition;

    PieceColor mPieceColor; //internal state
};

class PieceFactory
{
public:
    PieceFactory() {}

    ~PieceFactory() 
    {
        map<PieceColor, Piece *>::iterator color_piece_map = mColorPieceMap.begin();

        for (; color_piece_map != mColorPieceMap.end(); ++color_piece_map) {
            delete color_piece_map->second;
            mColorPieceMap[color_piece_map->first] = nullptr;
        }
        mColorPieceMap.clear();
    }

    Piece* GetPiece(const PieceColor &pColor)
    {
        Piece *piece = mColorPieceMap[pColor];

        if (nullptr != piece) {  return piece; }

        piece = new Piece(pColor);
        mColorPieceMap[pColor] = piece;
        return piece;
    }

private:
    map<PieceColor, Piece *> mColorPieceMap;
};

class ChessBoard
{
public:
    ChessBoard() {}

    ~ChessBoard() 
    {
        map<Position *, Piece *>::iterator position_piece_map = mPositionPieceMap.begin();

        for (; position_piece_map != mPositionPieceMap.end(); ++position_piece_map) {
            delete position_piece_map->first;
        }
        mPositionPieceMap.clear();
    }

    bool PlayChess(const int &pX, const int &pY, Piece *pPiece)
    {
        if (nullptr == pPiece) { return false; }

        Position *position =new Position(pX, pY);

        pPiece->SetPoint(position);
        pPiece->Show();

        mPositionPieceMap.insert(pair<Position *, Piece *>(position, pPiece));
        return true;
    }

private:
    map<Position *, Piece *> mPositionPieceMap;
};

    static void test_flyweight()
    {
        cout << "######################################\n" \
                "##  this is flyweight demo...       ##\n" \
                "##                                  ##\n" \
                "######################################"   \
             << endl;

        PieceFactory *piecefactory = new PieceFactory();
        ChessBoard *board = new ChessBoard();
        board->PlayChess(1, 2, piecefactory->GetPiece(White));
        board->PlayChess(2, 3, piecefactory->GetPiece(Black));

        delete piecefactory;
        piecefactory = nullptr;
        
        delete board;
        board = nullptr;
    }

};

#endif //flyweight_h
