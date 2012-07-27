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
}Pin;

ChError initBoard(ChessBoard* board);
ChError resetBoard(ChessBoard* board);
ChError readFENString(ChessBoard* board, char* fen);
void printBoardE(ChessBoard* board);

ChError generateMoves(ChessBoard* board,enum Color color, MoveList* moveList,SearchInformation* info);
ChError generateSortedMoves(ChessBoard* board,enum Color color, MoveList* moveList, SearchInformation* info);
ChError generateCaptures(ChessBoard* board,enum Color color, MoveList* moveList, SearchInformation* info);
ChError generateMoveForPosition(ChessBoard* board,const PieceInfo* pieceInfo, MoveList* moveList, int usePins, Pin* pinnedPieces,SearchInformation* info);
void generateAttackMap(ChessBoard* board, enum Color attackerColor, int* attackMap);

ChError doMove(ChessBoard* board, Move* move);
ChError undoMove(ChessBoard* board, Move* move, History* history);
ChError undoLastMove(ChessBoard* board);
int* getSortWeights(void);



ChError insertPiece(ChessBoard* board, PIECE pieceType, Color color, int location);

int isCheck(ChessBoard* board, Color color);
int isAttacked(ChessBoard* board, int position, enum Color color);
ChError isLegal(ChessBoard* board, Move* move);

int getPieceScore(PIECE piece);

PieceInfo* getPieceForTile(ChessBoard* board, int tileIndex);

void indexToChar(int index, char* charArray);
void moveToChar(Move* move, char* charArray);

int compareBoards(ChessBoard* board1, ChessBoard* board2);
int equalMoves(Move* move1, Move* move2);

ChError getFenString(ChessBoard* board, char* fen);

int rank(int index); 
int boardFile(int index);
int distance(int squareA, int squareB);

void clearBoard();


#endif