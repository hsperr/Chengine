//
//  EightyEightyBoard.h
//  Chengine
//
//  Created by Henning Sperr on 6/25/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Chengine_Board_h
#define Chengine_Board_h

#include "Chengine.h"
#include "BoardConstants.h"


typedef struct Pin{
    char from;
    char to;
    char delta;
    char enpassantpin;
}Pin;

ChError initBoard(ChessBoard* board);
ChError resetBoard(ChessBoard* board);
ChError readFENString(ChessBoard* board, char* fen);
void printBoardE(ChessBoard* board);

ChError generateMoves(ChessBoard* board,enum Color color, MoveList* moveList);
ChError generateSortedMoves(ChessBoard* board,enum Color color, MoveList* moveList);
ChError generateMoveForPosition(ChessBoard* board,const PieceInfo* pieceInfo, MoveList* moveList, int usePins, Pin* pinnedPieces);

ChError doMove(ChessBoard* board, Move* move, History* history);
ChError undoMove(ChessBoard* board, Move* move, History* history);



ChError insertPiece(ChessBoard* board, PIECE pieceType, Color color, int location);

int isCheck(ChessBoard* board, Color color);
int isAttacked(ChessBoard* board, int position, enum Color color);
ChError isLegal(ChessBoard* board, Move* move);


PieceInfo* getPieceForTile(ChessBoard* board, int tileIndex);

void indexToChar(int index, char* charArray);
void moveToChar(Move* move, char* charArray);

int compareBoards(ChessBoard* board1, ChessBoard* board2);
int equalMoves(Move* move1, Move* move2);


#endif