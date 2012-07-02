//
//  EightyEightBoard.c
//  Chengine
//
//  Created by Henning Sperr on 6/25/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Board.h"
#include <assert.h>



// & with 0100 determines sliding piece
// & with 0001 piece can slide diagonally
// & with 0010 piece can slide vertical/horizontal
// & with 1000 piece is qrook/krook


//from http://mediocrechess.sourceforge.net/guides/attackedsquares.html
static const int ATTACK_NONE = 0;
static const int ATTACK_KQR = 1;
static const int ATTACK_QR = 2;
static const int ATTACK_KQBwP = 3;
static const int ATTACK_KQBbP = 4;
static const int ATTACK_QB = 5;
static const int ATTACK_N = 6;

static const int DELTA_ARRAY[] =
{  0,   0,   0,   0,   0,   0,   0,   0,   0, -17,   0,   0,   0,   0,   0,   0, -16,   0,   0,   0,
    0,   0,   0, -15,   0,   0, -17,   0,   0,   0,   0,   0, -16,   0,   0,   0,   0,   0, -15,   0,
    0,   0,   0, -17,   0,   0,   0,   0, -16,   0,   0,   0,   0, -15,   0,   0,   0,   0,   0,   0,
    -17,   0,   0,   0, -16,   0,   0,   0, -15,   0,   0,   0,   0,   0,   0,   0,   0, -17,   0,   0,
    -16,   0,   0, -15,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -17, -33, -16, -31, -15,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, -18, -17, -16, -15, -14,   0,   0,   0,   0,   0,
    0,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,
    0,   0,  14,  15,  16,  17,  18,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  15,  31,
    16,  33,  17,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,  16,   0,   0,  17,
    0,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,   0,  16,   0,   0,   0,  17,   0,   0,   0,
    0,   0,   0,  15,   0,   0,   0,   0,  16,   0,   0,   0,   0,  17,   0,   0,   0,   0,  15,   0,
    0,   0,   0,   0,  16,   0,   0,   0,   0,   0,  17,   0,   0,  15,   0,   0,   0,   0,   0,   0,
    16,   0,   0,   0,   0,   0,   0,  17,   0,   0,   0,   0,   0,   0,   0,   0,   0};

static const int ATTACK_ARRAY[] =
{0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,2,0,0,0,     //0-19
    0,0,0,5,0,0,5,0,0,0,0,0,2,0,0,0,0,0,5,0,     //20-39
    0,0,0,5,0,0,0,0,2,0,0,0,0,5,0,0,0,0,0,0,     //40-59
    5,0,0,0,2,0,0,0,5,0,0,0,0,0,0,0,0,5,0,0,     //60-79
    2,0,0,5,0,0,0,0,0,0,0,0,0,0,5,6,2,6,5,0,     //80-99
    0,0,0,0,0,0,0,0,0,0,6,4,1,4,6,0,0,0,0,0,     //100-119
    0,2,2,2,2,2,2,1,0,1,2,2,2,2,2,2,0,0,0,0,     //120-139
    0,0,6,3,1,3,6,0,0,0,0,0,0,0,0,0,0,0,5,6,     //140-159
    2,6,5,0,0,0,0,0,0,0,0,0,0,5,0,0,2,0,0,5,     //160-179
    0,0,0,0,0,0,0,0,5,0,0,0,2,0,0,0,5,0,0,0,     //180-199
    0,0,0,5,0,0,0,0,2,0,0,0,0,5,0,0,0,0,5,0,     //200-219
    0,0,0,0,2,0,0,0,0,0,5,0,0,5,0,0,0,0,0,0,     //220-239
    2,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0         }; //240-256



static inline int IS_ON_BOARD(int x)   {return (x&0x88)==0;};

static inline int COLUMN(int position)                 {return (position&0x0F);};
static inline int ROW(int position)                    {return ((position&0xF0)>>4);};
static inline int SAME_COLUMN(int position1, int position2){return COLUMN(position1)==COLUMN(position2);};

static inline int IS_PROMOTE_ROW(int position,enum Color color) {return (color==WHITE&&ROW(position)==0)||(color==BLACK&&ROW(position)==8);};



ChError initBoard(ChessBoard* board){
    if(board==NULL){
        printf("initBoard, board was NULL");
        return ChError_Arguments;
    }
    
    board->playedMoves.alloc=0;
    board->playedMoves.nextFree=0;
    board->playedMoves.array=NULL;
    
    readFENString(board,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    return ChError_OK;
    
}

ChError resetBoard(ChessBoard* board){
    if(board==NULL){
        printf("initBoard, board was NULL");
        return ChError_Arguments;
    }
    
    board->playedMoves.alloc=0;
    board->playedMoves.nextFree=0;
    board->playedMoves.array=NULL;
    
    readFENString(board,"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    return ChError_OK;
    
}

ChError readFENString(ChessBoard* board, char* fen){    
    int boardIndex=0;
    int stringIndex=0;
    ChError hr;
    
    //reset piece index
    for(int i=0;i<16;i++){
        board->whiteToSquare[i].piece=-5;
        board->blackToSquare[i].piece=-5;
        board->whiteToSquare[i].location=-5;
        board->blackToSquare[i].location=-5;
        
    }
    //board piece index
    for(int i=0;i<127;i++){
        board->tiles[i]=NULL;
    }
    //in board representation of fen string there is no " " 
    for(;fen[stringIndex]!=' ';stringIndex++){
        char current=fen[stringIndex];
        switch(current){
            case 'r':
                hr=insertPiece(board, rook,BLACK,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'n':
                hr=insertPiece(board, knight,BLACK, boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'b':
                hr=insertPiece(board, bishop,BLACK,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'q':
                hr=insertPiece(board, queen,BLACK,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'k':
                hr=insertPiece(board, king,BLACK,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'p':
                hr=insertPiece(board, pawn,BLACK,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'R':
                hr=insertPiece(board, rook,WHITE,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'N':
                hr=insertPiece(board, knight,WHITE,boardIndex);
                if(hr)
                    return hr;boardIndex++;
                break;
            case 'B':
                hr=insertPiece(board,bishop,WHITE,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'Q':
                hr=insertPiece(board, queen,WHITE,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'K':
                hr=insertPiece(board, king,WHITE,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            case 'P':
                hr=insertPiece(board, pawn,WHITE,boardIndex);
                if(hr)
                    return hr;
                boardIndex++;
                break;
            default:
                //fill with spaces or next row
                if(current=='/'){
                    boardIndex+=8;
                    continue;
                }
                if(current>'0'&&current<'9'){
                    int number=current-'0';
                    for(int j=0;j<number;j++){
                        board->tiles[boardIndex]=NULL;
                        boardIndex++;
                    }
                }
                break;
        }
        
    }
    
    //"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    if(fen[stringIndex]==' '){
        stringIndex++;
    }else{
        return ChError_BrokenFenString;
    }
    
    if(fen[stringIndex]=='w'){
        board->colorToPlay=WHITE;
    }else if(fen[stringIndex]=='b'){
        board->colorToPlay=BLACK;
    }else{
        return ChError_BrokenFenString;
    }
    stringIndex++;
    
    if(fen[stringIndex]==' '){
        stringIndex++;
    }else{
        return ChError_BrokenFenString;
    }
    
    //castling not possible
    board->rights[WHITE].kingCastlingPossible=0;
    board->rights[WHITE].queenCastlingPossible=0;
    board->rights[BLACK].kingCastlingPossible=0;
    board->rights[BLACK].queenCastlingPossible=0;
    
    if(fen[stringIndex]=='-'){
        stringIndex++;
    }else{
        if(fen[stringIndex]=='K'){
            assert(board->tiles[H1]->piece==rook);
            assert(board->tiles[E1]->piece==king);
            board->rights[WHITE].kingCastlingPossible=1;
            
            stringIndex++;
        }
        if(fen[stringIndex]=='Q'){
            assert(board->tiles[A1]->piece==rook);
            assert(board->tiles[E1]->piece==king);
            board->rights[WHITE].queenCastlingPossible=1;
            
            stringIndex++;
        }
        if(fen[stringIndex]=='k'){
            assert(board->tiles[H8]->piece==rook);
            assert(board->tiles[E8]->piece==king);
            board->rights[BLACK].kingCastlingPossible=1;
            
            stringIndex++;
        }
        if(fen[stringIndex]=='q'){
            assert(board->tiles[A8]->piece==rook);
            assert(board->tiles[E8]->piece==king);
            board->rights[BLACK].queenCastlingPossible=1;
            
            stringIndex++;
        }
    }
    
    if(fen[stringIndex]==' '){
        stringIndex++;
    }else{
        return ChError_BrokenFenString;
    }
    
    if(fen[stringIndex]=='-'){
        board->enPassantSquare=-5;
        stringIndex++;
    }else{
        if(fen[stringIndex]>='a'&&fen[stringIndex]<='h'){
            if(fen[stringIndex+1]>='0'&&fen[stringIndex+1]<='8'){
                board->enPassantSquare= (int)(fen[stringIndex]-'a')+0x10*(7-(int)(fen[stringIndex+1]-'1'));
                stringIndex+=2;
            }
        }
    }
    
    if(fen[stringIndex]=='\0'){
        return ChError_OK;
    }else{
        return ChError_BrokenFenString;
    }
    if(fen[stringIndex]==' '){
        stringIndex++;
    }else{
        return ChError_BrokenFenString;
    }
    
    board->staleMateMoves=(int)fen[stringIndex];
    stringIndex++;
    
    //handle stalematemoves
    stringIndex++;
    if(fen[stringIndex]==' '){
        stringIndex++;
    }else{
        return ChError_BrokenFenString;
    }
    //handle numMoves
    
    return ChError_OK;
}
static int getPieceScore(PIECE piece){
    int score=0;
    switch (piece) {
        case queen:
            score=900;
            break;
        case rook:
            score=500;
            break;
        case pawn:
            score=100;
            break;
        case knight:
            score=300;
            break;
        case bishop:
            score=300;
            break;
        default:
            break;
    }
    return score;
}

ChError insertPiece(ChessBoard* board, PIECE pieceType, Color color, int location){
    PieceInfo* pieceToSquareArray;
    if(color==WHITE){
        pieceToSquareArray=board->whiteToSquare;
    }else{
        pieceToSquareArray=board->blackToSquare;
    }
    
    if(pieceType==king){
        pieceToSquareArray[0].piece=pieceType;
        pieceToSquareArray[0].location=location;
        pieceToSquareArray[0].color=color;
        pieceToSquareArray[0].score=100000;
        
        board->tiles[location]=&pieceToSquareArray[0];
    }else{
        for(int i=1;i<16;i++){
            if(pieceToSquareArray[i].piece==-5){
                pieceToSquareArray[i].piece=pieceType;
                pieceToSquareArray[i].location=location;
                pieceToSquareArray[i].color=color;
                pieceToSquareArray[i].score=getPieceScore(pieceType);
                
                board->tiles[location]=&pieceToSquareArray[i];
                break;
            }
        }
    }
    
    return ChError_OK;
}


void printBoardE(ChessBoard* board){
    for(int i=0x00; i<=0x78;i+=0x01){
        if(IS_ON_BOARD(i)){
            PieceInfo* piece = getPieceForTile(board,i);
            if(!piece)
                printf("· ");
            else
                switch (piece->piece) {
                    case pawn:
                        if(piece->color==BLACK)
                            printf("p ");
                        else
                            printf("P ");
                        break;
                    case knight:
                        if(piece->color==BLACK)
                            printf("n ");
                        else
                            printf("N ");
                        break;
                    case king:
                        if(piece->color==BLACK)
                            printf("k ");
                        else
                            printf("K ");
                        break;
                    case bishop:
                        if(piece->color==BLACK)
                            printf("b ");
                        else
                            printf("B ");
                        break;
                    case rook:
                        if(piece->color==BLACK)
                            printf("r ");
                        else
                            printf("R ");
                        break;
                    case queen:
                        if(piece->color==BLACK)
                            printf("q ");
                        else
                            printf("Q ");
                        break;
                    default:
                        printf("· ");
                        break;
                }
        }else{
            if(((i & 0x0F)|0x08)==0x08){
                printf("%d\n",(int)(8-((i&0xF0)>>4)));
            }
        }
    }
    for(int j=0;j<8;j++){
        printf("%c ",'a'+j);
    }
    printf("\n");
}

int isAttacked(ChessBoard* board, int position, enum Color attackerColor){
    int direction=attackerColor==WHITE?1:-1;
    PieceInfo* attackerPieces=attackerColor==WHITE?board->whiteToSquare:board->blackToSquare;
    
    //pawns
    PieceInfo* piece=getPieceForTile(board,position+direction*0x0F);
    if(piece&&piece->color==attackerColor&&piece->piece==pawn){
        return 1;
    }
    piece=getPieceForTile(board,position+direction*0x11);
    if(piece&&piece->color==attackerColor&&piece->piece==pawn){
        return 1;
    }
    
    for(int i=0;i<16;i++){
        PieceInfo nextPiece=attackerPieces[i];
        int index=(nextPiece.location-position)+128;
        int attack=ATTACK_ARRAY[index];
        
        if(attack==0||nextPiece.piece==pawn)
            continue;
        
        if(nextPiece.piece==knight){
            if(attack==ATTACK_N){
                return 1;
            }
            continue;
        }
        if(nextPiece.piece==king){
            if(attack==ATTACK_KQR||attack==ATTACK_KQBwP||attack==ATTACK_KQBbP){
                return 1;
            }
            continue;
        }
        if(nextPiece.piece==queen){
            if(attack==ATTACK_KQR||attack==ATTACK_KQBwP||attack==ATTACK_KQBbP||attack==ATTACK_QB||attack==ATTACK_QR){
                int delta=DELTA_ARRAY[index];
                int nextPosition=position+delta;
                while(IS_ON_BOARD(nextPosition)){
                    if(nextPosition==nextPiece.location)
                        return 1;
                    if(board->tiles[nextPosition])
                        break;
                    
                    nextPosition+=delta;
                }
                
            }
            continue;
        }
        if(nextPiece.piece==rook){
            if(attack==ATTACK_KQR||attack==ATTACK_QR){
                int delta=DELTA_ARRAY[index];
                int nextPosition=position+delta;
                while(IS_ON_BOARD(nextPosition)){
                    if(nextPosition==nextPiece.location)
                        return 1;
                    if(board->tiles[nextPosition])
                        break;
                    
                    nextPosition+=delta;
                }
                
            }
            continue;
        }
        
        if(nextPiece.piece==bishop){
            if(attack==ATTACK_KQBwP||attack==ATTACK_KQBbP||attack==ATTACK_QB){
                int delta=DELTA_ARRAY[index];
                int nextPosition=position+delta;
                while(IS_ON_BOARD(nextPosition)){
                    if(nextPosition==nextPiece.location)
                        return 1;
                    if(board->tiles[nextPosition])
                        break;
                    nextPosition+=delta;
                }
                
            }
            continue;
        } 
    }
    return 0;
}

ChError doMove(ChessBoard* board, Move* move){
    if(board==NULL||move==NULL)
        return ChError_Arguments;
    
    Color myColor=board->colorToPlay;
    Rights* myRights=&board->rights[myColor];
    Rights* opponentRights=&board->rights[myColor==WHITE?BLACK:WHITE];
    //adjustCastlingRights
    
    move->whiteKingCastlingRights=board->rights[WHITE].kingCastlingPossible;
    move->whiteQueenCastlingRights=board->rights[WHITE].queenCastlingPossible;
    
    move->blackKingCastlingRights=board->rights[BLACK].kingCastlingPossible;
    move->blackQueenCastlingRights=board->rights[BLACK].queenCastlingPossible;
    
    move->previousEnPassantSquare=board->enPassantSquare;
    
    
    //moveRookForCastling
    PieceInfo* fromPiece=getPieceForTile(board, move->from);
    if(fromPiece->piece==king){
        if(move->from-move->to==0x02){
            // side
            int rookPos=myColor==WHITE?A1:A8;
            int newPosition=move->to+0x01;
            assert(board->tiles[rookPos]!=NULL);
            
            PieceInfo* rookInfo=board->tiles[rookPos];
            
            assert(rookInfo->piece==rook);
            rookInfo->location=newPosition;
            board->tiles[newPosition]=board->tiles[rookPos];
            board->tiles[rookPos]=NULL;
            
        }else if(move->from-move->to==-0x02){
            int newPosition=move->to-0x01;
            int rookPos=myColor==WHITE?H1:H8;
            
            PieceInfo* rookInfo=board->tiles[rookPos];
            
            assert(rookInfo->piece==rook);
            rookInfo->location=newPosition;
            board->tiles[newPosition]=board->tiles[rookPos];
            board->tiles[rookPos]=NULL;
            
        }
        myRights->kingCastlingPossible=0;
        myRights->queenCastlingPossible=0;
    }
    
    int qRookField=myColor==WHITE?A1:A8;
    if(move->from==qRookField){
        myRights->queenCastlingPossible=0;
    }
    
    int kRookField=myColor==WHITE?H1:H8;
    if(move->from==kRookField){
        myRights->kingCastlingPossible=0;
    }
    int enemyqRookField=myColor==WHITE?A8:A1;
    if(move->to==enemyqRookField){
        opponentRights->queenCastlingPossible=0;
    }
    
    int enemykRookField=myColor==WHITE?H8:H1;
    if(move->to==enemykRookField){
        opponentRights->kingCastlingPossible=0;
    }
    
    board->enPassantSquare=-5;
    
    move->pieceToSquareIndexTo=NULL;
    //removePiece (Enpassant/Normal)
    if(fromPiece->piece==pawn){
        if(!SAME_COLUMN(move->from, move->to)&&!board->tiles[move->to]){
            int delta=COLUMN(move->to)-COLUMN(move->from);
            
            move->pieceToSquareIndexTo=board->tiles[move->from+delta];
            board->tiles[move->from+delta]=NULL;
            move->pieceToSquareIndexTo->location=-5;
        } 
        if(move->to-move->from==0x20||
           move->to-move->from==-0x20){
            board->enPassantSquare=move->from+(move->to-move->from)/0x02;
        }
        if(move->promote>0){
            fromPiece->piece=move->promote;
            fromPiece->score=getPieceScore(move->promote);
        }
        
    }
    if(board->tiles[move->to]!=0x0){
        move->pieceToSquareIndexTo=board->tiles[move->to];
        board->tiles[move->to]=NULL;
        move->pieceToSquareIndexTo->location=-5;
        
    }
    
    board->tiles[move->to]=board->tiles[move->from];
    board->tiles[move->from]=NULL;
    board->tiles[move->to]->location=move->to;
    
    //updateColor
    board->colorToPlay=board->colorToPlay==WHITE?BLACK:WHITE;
    
    //addToMoveList(&board->playedMoves, move);
    
    return ChError_OK;
}
ChError undoLastMove(ChessBoard* board){
    return undoMove(board, &board->playedMoves.array[board->playedMoves.nextFree-1]);
}

ChError undoMove(ChessBoard* board, Move* move){
    if(board==NULL||move==NULL)
        return ChError_Arguments;
    
    //updateColor
    board->colorToPlay=board->colorToPlay==WHITE?BLACK:WHITE;
    
    //unmovePiece
    board->tiles[move->from]=board->tiles[move->to];
    board->tiles[move->to]=NULL;
    board->tiles[move->from]->location=move->from;
    
    
    
    PieceInfo* fromPiece=getPieceForTile(board, move->from);
    
    //promotion
    if(move->promote>0){
        fromPiece->piece=pawn;
    }
    
    if(fromPiece->piece==pawn&&move->to==move->previousEnPassantSquare){
        assert(move->pieceToSquareIndexTo!=NULL);
        int delta=COLUMN(move->to)-COLUMN(move->from);
        int pawnPos=move->from+delta;
        
        board->tiles[pawnPos]=move->pieceToSquareIndexTo;
        board->tiles[pawnPos]->location=pawnPos;
    }else if(move->pieceToSquareIndexTo!=NULL){
        board->tiles[move->to]=move->pieceToSquareIndexTo;
        board->tiles[move->to]->location=move->to;
    }
    
    //UpdateEnpassantSquare
    board->enPassantSquare=move->previousEnPassantSquare;
    
    //moveRookForCastling
    board->rights[WHITE].queenCastlingPossible=move->whiteQueenCastlingRights;
    board->rights[WHITE].kingCastlingPossible=move->whiteKingCastlingRights;
    board->rights[BLACK].queenCastlingPossible=move->blackQueenCastlingRights;
    board->rights[BLACK].kingCastlingPossible=move->blackKingCastlingRights;
    
    Color myColor=board->colorToPlay;
    if(fromPiece->piece==king){
        if((move->from-move->to==0x02)){
            //qrook
            int rookPos=myColor==WHITE?A1:A8;
            
            assert(board->tiles[(move->to+0x01)]->piece==rook);
            board->tiles[rookPos]=board->tiles[(move->to+0x01)];
            board->tiles[rookPos]->location=rookPos;
            board->tiles[(move->to+0x01)]=NULL;
            
        }else if((move->from-move->to==-0x02)){
            //krook
            int rookPos=myColor==WHITE?H1:H8;
            
            assert(board->tiles[(move->to-0x01)]->piece==rook);
            board->tiles[rookPos]=board->tiles[(move->to-0x01)];
            board->tiles[rookPos]->location=rookPos;
            board->tiles[(move->to-0x01)]=NULL;
        }
    }
    
    board->playedMoves.nextFree--;
    
    return ChError_OK;
}
int isCheck(ChessBoard* board, Color color){
    PieceInfo* myPieces=color==WHITE?board->whiteToSquare:board->blackToSquare;
    
    return isAttacked(board, myPieces[0].location, color==WHITE?BLACK:WHITE);
    
}
static ChError addMove(ChessBoard* board,int from, int to, PIECE promotion, MoveList* list){
    ChError hr=ChError_OK;
    Move move={-5};
    move.from=from;
    move.to=to;
    move.promote=promotion;
    
    hr=addToMoveList(list, &move);
    return hr;
}


ChError generateMoveForPosition(ChessBoard* board,const PieceInfo* pieceInfo, MoveList* moveList){
    int rookdeltas[]={-0x01,0x01,-0x10,0x10};
    int bishopdeltas[]={-0x11,-0x0F,0x0F,0x11};
    int kingdeltas[]={-0x01,-0x11,-0x10,-0x0F,0x01,0x11,0x10,0x0F};
    int knightdeltas[]={-0x1F,-0x21,-0x12,-0x0E,0x1F,0x21,0x12,0x0E};
    PieceInfo* pieceOnNewSquare=NULL;
    int nextPosition=0;
    int position = pieceInfo->location;
    Color enemyColor=pieceInfo->color==WHITE?BLACK:WHITE;
    int pawnDirectionModifier=pieceInfo->color==BLACK?1:-1;
    
    switch (pieceInfo->piece) {
        case pawn:
            nextPosition=position+0x10*pawnDirectionModifier;
            if(IS_ON_BOARD(nextPosition)){
                pieceOnNewSquare=getPieceForTile(board,nextPosition);
                if(!pieceOnNewSquare){
                    
                    if(((nextPosition&0xF0)==0x00&&pieceInfo->color==WHITE)||
                       ((nextPosition&0xF0)==0x70&&pieceInfo->color==BLACK)){
                        addMove(board,position, nextPosition,queen, moveList);
                        addMove(board,position, nextPosition,knight, moveList);
                        addMove(board,position, nextPosition,rook, moveList);
                        addMove(board,position, nextPosition,bishop, moveList);
                    }else{
                        addMove(board,position, nextPosition,0, moveList);
                    }
                    
                    int homeRow=pieceInfo->color==BLACK?(position&0xF0)==0x10:(position&0xF0)==0x60;
                    if(homeRow){
                        nextPosition+=0x10*pawnDirectionModifier;
                        pieceOnNewSquare=getPieceForTile(board,nextPosition);
                        if(!pieceOnNewSquare){
                            addMove(board,position, nextPosition,0, moveList);
                        }
                    }
                }
            }
            nextPosition=position+0x11*pawnDirectionModifier;
            if(IS_ON_BOARD(nextPosition)){
                pieceOnNewSquare=getPieceForTile(board,nextPosition);
                if(pieceOnNewSquare&&
                   pieceOnNewSquare->color!=pieceInfo->color){
                    if(((nextPosition&0xF0)==0x00&&pieceInfo->color==WHITE)||
                       ((nextPosition&0xF0)==0x70&&pieceInfo->color==BLACK)){
                        addMove(board,position, nextPosition,queen, moveList);
                        addMove(board,position, nextPosition,knight, moveList);
                        addMove(board,position, nextPosition,rook, moveList);
                        addMove(board,position, nextPosition,bishop, moveList);
                    }else{
                        addMove(board,position, nextPosition,0, moveList);               
                    }
                }else if(board->enPassantSquare==nextPosition){
                    addMove(board,position, nextPosition,0, moveList); 
                }
            }
            nextPosition=position+0x0F*pawnDirectionModifier;
            if(IS_ON_BOARD(nextPosition)){
                pieceOnNewSquare=getPieceForTile(board,nextPosition);
                if(pieceOnNewSquare&&
                   pieceOnNewSquare->color!=pieceInfo->color){
                    if(((nextPosition&0xF0)==0x00&&pieceInfo->color==WHITE)||
                       ((nextPosition&0xF0)==0x70&&pieceInfo->color==BLACK)){
                        addMove(board,position, nextPosition,queen, moveList);
                        addMove(board,position, nextPosition,knight, moveList);
                        addMove(board,position, nextPosition,rook, moveList);
                        addMove(board,position, nextPosition,bishop, moveList);
                    }else{
                        addMove(board,position, nextPosition,0, moveList);
                    }
                }else if(board->enPassantSquare==nextPosition){
                    addMove(board,position, nextPosition,0, moveList); 
                }
            }               
            
            
            break;
        case knight:
            
            for(int i=0;i<8;i++){
                nextPosition=position+knightdeltas[i];
                if(IS_ON_BOARD(nextPosition)){
                    pieceOnNewSquare=getPieceForTile(board,nextPosition);
                    if(!pieceOnNewSquare||
                       pieceOnNewSquare->color!=pieceInfo->color){
                        addMove(board,position, nextPosition,0, moveList);
                    }
                }
            }
            break;
        case king:
            
            nextPosition=position;
            for(int i=0;i<8;i++){
                nextPosition=position+kingdeltas[i];
                if(IS_ON_BOARD(nextPosition)){
                    pieceOnNewSquare=getPieceForTile(board,nextPosition);
                    if((!pieceOnNewSquare||
                        pieceOnNewSquare->color!=pieceInfo->color)&&
                       !isAttacked(board,nextPosition,enemyColor)){
                        addMove(board,position, nextPosition,0, moveList);;
                    }
                    
                }
            }
            Rights* castleRights=&board->rights[pieceInfo->color];
            if(castleRights->kingCastlingPossible==1){
                if(board->tiles[position+0x01]==NULL&&
                   board->tiles[position+0x02]==NULL&&
                   !isAttacked(board, position+0x01, enemyColor)&&
                   !isAttacked(board, position+0x02, enemyColor)){
                    addMove(board,position, position+0x02,0, moveList);
                }
            }
            if(castleRights->queenCastlingPossible==1){
                if(board->tiles[position-0x01]==NULL&&
                   board->tiles[position-0x02]==NULL&&
                   board->tiles[position-0x03]==NULL&&
                   !isAttacked(board, position-0x01, enemyColor)&&
                   !isAttacked(board, position-0x02, enemyColor)){
                    addMove(board,position, position-0x02,0, moveList);
                }
            }
            
            break;   
            
            
            
            
        case bishop:
            for(int i=0;i<4;i++){
                nextPosition=position+bishopdeltas[i];
                while(IS_ON_BOARD(nextPosition)){
                    pieceOnNewSquare=getPieceForTile(board,nextPosition);
                    if(!pieceOnNewSquare){
                        addMove(board,position, nextPosition,0, moveList);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0, moveList); 
                        }
                        break;
                    }
                    nextPosition+=bishopdeltas[i];
                }
            }
            break;
        case rook:
            for(int i=0;i<4;i++){
                nextPosition=position+rookdeltas[i];
                while(IS_ON_BOARD(nextPosition)){
                    pieceOnNewSquare=getPieceForTile(board,nextPosition);
                    if(!pieceOnNewSquare){
                        addMove(board,position, nextPosition,0, moveList);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0, moveList);
                        }
                        break;
                    }
                    nextPosition+=rookdeltas[i];
                }
            }
            break;
        case queen:
            for(int i=0;i<4;i++){
                nextPosition=position+rookdeltas[i];
                while(IS_ON_BOARD(nextPosition)){
                    pieceOnNewSquare=getPieceForTile(board,nextPosition);
                    if(!pieceOnNewSquare){
                        addMove(board,position, nextPosition,0, moveList);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0, moveList);
                        }
                        break;
                    }
                    nextPosition+=rookdeltas[i];
                }
            }
            
            for(int i=0;i<4;i++){
                nextPosition=position+bishopdeltas[i];
                while(IS_ON_BOARD(nextPosition)){
                    pieceOnNewSquare=getPieceForTile(board,nextPosition);
                    if(!pieceOnNewSquare){
                        addMove(board,position, nextPosition,0, moveList);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0, moveList); 
                        }
                        break;
                    }
                    nextPosition+=bishopdeltas[i];
                }
            }
            break;
        default:
            break;
    }
    
    return ChError_OK;
}

ChError generateMoves(ChessBoard* board,enum Color color, MoveList* moveList){
    PieceInfo* pieceArray=color==WHITE?board->whiteToSquare:board->blackToSquare;
    
    for(int i=0;i<16;i++){
        if(pieceArray[i].location==-5){
            continue;
        }
        generateMoveForPosition(board,&pieceArray[i], moveList);
    }
    return ChError_OK;
}
int equalMoves(Move* move1, Move* move2){
    return move1->from==move2->from&&move1->to==move2->to;
}
int isLegal(ChessBoard* board, Move* move){
    MoveList moveList={0};
    
    generateMoves(board, board->colorToPlay, &moveList);
    
    for(int i=0;i<moveList.nextFree;i++){
        if(equalMoves(&moveList.array[i],move)){
            freeMoveList(&moveList);
            return 1;
        }
    }
    freeMoveList(&moveList);
    return 0;
}

PieceInfo* getPieceForTile(ChessBoard* board, int tileIndex){
    if(!board->tiles[tileIndex]||!IS_ON_BOARD(tileIndex))
        return NULL;
    
    return board->tiles[tileIndex];
}

void indexToChar(int index, char* charArray){
    charArray[0]='a'+(index&0x0F);
    charArray[1]='8'-((index&0xF0)>>4);
}

void moveToChar(Move* move, char* charArray){
    
    indexToChar(move->from, charArray);
    indexToChar(move->to,&charArray[2]);
    switch (move->promote) {
        case rook:
            charArray[4]='r';
            break;
        case queen:
            charArray[4]='q';
            break;
        case knight:
            charArray[4]='n';
            break;
        case bishop:
            charArray[4]='b';
            break;
        default:
            charArray[4]=' ';
            break;
    }
    
    charArray[5]='\0';
}


int compareBoards(ChessBoard* board1, ChessBoard* board2){
    
    for(int i=0;i<128;i++){
        PieceInfo* piece1=getPieceForTile(board1,i);
        PieceInfo* piece2=getPieceForTile(board2,i);
        if(!piece1&&piece2)
            return 1;
        if(!piece2&&piece1)
            return 1;
        if(!piece1&&!piece2)
            continue;
        if(piece1->piece!=piece2->piece||
           piece1->color!=piece2->color||
           piece1->location!=piece2->location){
            return 1;
        }
    }
    if(board1->colorToPlay!=board2->colorToPlay)
        return 1;
    if(board1->enPassantSquare!=board2->enPassantSquare)
        return 1;
    for(int i=WHITE;i<=BLACK;i++){
        if(board1->rights[WHITE].kingCastlingPossible!=board2->rights[WHITE].kingCastlingPossible){
            return 1;
        }
        if(board1->rights[WHITE].queenCastlingPossible!=board2->rights[WHITE].queenCastlingPossible){
            return 1;
        }
        if(board1->rights[BLACK].kingCastlingPossible!=board2->rights[BLACK].kingCastlingPossible){
            return 1;
        }
        if(board1->rights[BLACK].queenCastlingPossible!=board2->rights[BLACK].queenCastlingPossible){
            return 1;
        }
        
    }
    
    return 0;
}