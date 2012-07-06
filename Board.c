//
//  EightyEightBoard.c
//  Chengine
//
//  Created by Henning Sperr on 6/25/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Board.h"
#include <assert.h>



static const int NO_LOCATION=-5;


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


static const char castlingRights[]={
    0xe,0xf,0xf,0xf,0xC,0xf,0xf,0xd,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,
    0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,
    0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,
    0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,
    0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,
    0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,
    0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf,
    0xb,0xf,0xf,0xf,0x3,0xf,0xf,0x7,0xf,0xf,0xf,0xf,0xf,0xf,0xf,0xf
};

static inline int IS_ON_BOARD(int x)   {return (x&0x88)==0;};

static inline int COLUMN(int position)                 {return (position&0x0F);};
static inline int ROW(int position)                    {return ((position&0xF0)>>4);};
static inline int SAME_COLUMN(int position1, int position2){return COLUMN(position1)==COLUMN(position2);};

static inline int IS_PROMOTE_ROW(int position,enum Color color) {return (color==WHITE&&ROW(position)==0)||(color==BLACK&&ROW(position)==8);};

static char attackMap[127];



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
        attackMap[i]=0;
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
    board->castlingRights=0x0;
    
    if(fen[stringIndex]=='-'){
        stringIndex++;
    }else{
        if(fen[stringIndex]=='K'){
            assert(board->tiles[H1]->piece==rook);
            assert(board->tiles[E1]->piece==king);
            board->castlingRights|=0x8;
            
            stringIndex++;
        }
        if(fen[stringIndex]=='Q'){
            assert(board->tiles[A1]->piece==rook);
            assert(board->tiles[E1]->piece==king);
            board->castlingRights|=0x4;
            
            stringIndex++;
        }
        if(fen[stringIndex]=='k'){
            assert(board->tiles[H8]->piece==rook);
            assert(board->tiles[E8]->piece==king);
            board->castlingRights|=0x2;
            
            stringIndex++;
        }
        if(fen[stringIndex]=='q'){
            assert(board->tiles[A8]->piece==rook);
            assert(board->tiles[E8]->piece==king);
            board->castlingRights|=0x1;
            
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
    } if(fen[stringIndex]==' '){
        stringIndex++;
    }else{
        return ChError_BrokenFenString;
    }
    
    board->repetitionMoves=0;
    while(fen[stringIndex]!=' '){
        char fenPos=fen[stringIndex];
        board->repetitionMoves=board->repetitionMoves*10+(int)(fenPos-'0');
        stringIndex++;
    }
    
    //handle stalematemoves
    if(fen[stringIndex]==' '){
        stringIndex++;
    }else{
        return ChError_BrokenFenString;
    }
    //handle numMoves
    
    
    board->zobrist=getZobristHash(board);
    
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

ChError getPinnedPiecePositions(ChessBoard* board, enum Color color, Pin* pieceList){
    ChError hr=ChError_OK;
    int position=color==WHITE?board->whiteToSquare[0].location:board->blackToSquare[0].location;
    PieceInfo* attackerPieces=color==WHITE?board->blackToSquare:board->whiteToSquare;
    int pinnedPieces=0;
    
    for(int i=0;i<16;i++){
        PieceInfo nextPiece=attackerPieces[i];
        int index=(nextPiece.location-position)+128;
        int attack=ATTACK_ARRAY[index];
        
        if(attack==0||nextPiece.piece==pawn||nextPiece.location==NO_LOCATION)
            continue;
        
        
        switch(nextPiece.piece){
            case queen:
                if(attack==ATTACK_KQR||attack==ATTACK_KQBwP||attack==ATTACK_KQBbP||attack==ATTACK_QB||attack==ATTACK_QR){
                    int delta=DELTA_ARRAY[index];
                    int nextPosition=nextPiece.location-delta;
                    int foundPinnedPosition=NO_LOCATION;
                    while(IS_ON_BOARD(nextPosition)){
                        if(nextPosition==position)
                            break;
                        
                        if(board->tiles[nextPosition]){
                            if(board->tiles[nextPosition]->color!=color){
                                //mypiece
                                foundPinnedPosition=NO_LOCATION;
                                break;
                            }else{
                                if(foundPinnedPosition!=NO_LOCATION){
                                    //blocked by two pieces
                                    foundPinnedPosition=NO_LOCATION;
                                    break;
                                }else{
                                    //piece is pinned
                                    foundPinnedPosition=nextPosition;
                                }
                            }
                        }
                        
                        
                        nextPosition-=delta;
                    }
                    if(foundPinnedPosition>=0){
                        pieceList[pinnedPieces].from=foundPinnedPosition;
                        pieceList[pinnedPieces].to=nextPiece.location;
                        pieceList[pinnedPieces].delta=delta;                        
                        foundPinnedPosition=NO_LOCATION;
                        pinnedPieces++;
                    }
                    
                }
                break;
            case rook:
                if(attack==ATTACK_KQR||attack==ATTACK_QR){
                    int delta=DELTA_ARRAY[index];
                    int nextPosition=nextPiece.location-delta;
                    int foundPinnedPosition=NO_LOCATION;
                    while(IS_ON_BOARD(nextPosition)){
                        if(nextPosition==position)
                            break;
                        
                        if(board->tiles[nextPosition]){
                            if(board->tiles[nextPosition]->color!=color){
                                //mypiece
                                    foundPinnedPosition=NO_LOCATION;
                                
                                break;
                            }else{
                                if(foundPinnedPosition!=NO_LOCATION){
                                    //blocked by two pieces
                                    foundPinnedPosition=NO_LOCATION;
                                    break;
                                }else{
                                    //piece is pinned
                                    foundPinnedPosition=nextPosition;
                                }
                            }
                        }
                        
                        
                        nextPosition-=delta;
                    }
                    if(foundPinnedPosition>=0){
                        pieceList[pinnedPieces].from=foundPinnedPosition;
                        pieceList[pinnedPieces].to=nextPiece.location;
                        pieceList[pinnedPieces].delta=delta;

                        foundPinnedPosition=NO_LOCATION;
                        pinnedPieces++;
                    }
                    
                }
                break;
            case bishop:
                if(attack==ATTACK_KQBwP||attack==ATTACK_KQBbP||attack==ATTACK_QB){
                    int delta=DELTA_ARRAY[index];
                    int nextPosition=nextPiece.location-delta;
                    int foundPinnedPosition=NO_LOCATION;
                    while(IS_ON_BOARD(nextPosition)){
                        if(nextPosition==position)
                            break;
                        
                        if(board->tiles[nextPosition]){
                            if(board->tiles[nextPosition]->color!=color){
                                //mypiece
                                foundPinnedPosition=NO_LOCATION;
                            
                                break;
                            }else{
                                if(foundPinnedPosition!=NO_LOCATION){
                                    //blocked by two pieces
                                    foundPinnedPosition=NO_LOCATION;
                                    break;
                                }else{
                                    //piece is pinned
                                    foundPinnedPosition=nextPosition;
                                }
                            }
                        }
                        
                        
                        nextPosition-=delta;
                    }
                    if(foundPinnedPosition>=0){
                        pieceList[pinnedPieces].from=foundPinnedPosition;
                        pieceList[pinnedPieces].to=nextPiece.location;
                        pieceList[pinnedPieces].delta=delta;

                        
                        foundPinnedPosition=NO_LOCATION;
                        pinnedPieces++;
                    }
                    
                }
                break;
        }
    }
    
    //printf("Pinned pieces %d\n",pinnedPieces);
    return hr;
}

void generateAttackMap(ChessBoard* board, enum Color attackerColor){
    for(int i=0;i<127;i++){
        attackMap[i]=0;
    }
    int rookdeltas[]={-0x01,0x01,-0x10,0x10};
    int bishopdeltas[]={-0x11,-0x0F,0x0F,0x11};
    int kingdeltas[]={-0x01,-0x11,-0x10,-0x0F,0x01,0x11,0x10,0x0F};
    int knightdeltas[]={-0x1F,-0x21,-0x12,-0x0E,0x1F,0x21,0x12,0x0E};
    int direction=attackerColor==WHITE?-1:1;
    PieceInfo* attackerPieces=attackerColor==WHITE?board->whiteToSquare:board->blackToSquare;
    
    for(int j=0;j<16;j++){
        PieceInfo nextPiece=attackerPieces[j];        
        switch(nextPiece.piece){
            case pawn:
                if(IS_ON_BOARD(nextPiece.location+direction*0x0F))
                    attackMap[nextPiece.location+direction*0x0F]+=1;
                if(IS_ON_BOARD(nextPiece.location+direction*0x11))
                    attackMap[nextPiece.location+direction*0x11]+=1;
                break;
            case knight:
                for(int i=0;i<8;i++){
                    if(IS_ON_BOARD(nextPiece.location+knightdeltas[i]))
                        attackMap[nextPiece.location+knightdeltas[i]]+=1;
                }
                break;
            case king:
                for(int i=0;i<8;i++){
                    if(IS_ON_BOARD(nextPiece.location+kingdeltas[i]))
                        attackMap[nextPiece.location+kingdeltas[i]]+=1;
                }
                break;
            case queen:
                for(int i=0;i<4;i++){
                    int delta=rookdeltas[i];
                    int nextPosition=nextPiece.location+delta;
                    while(IS_ON_BOARD(nextPosition)){
                        attackMap[nextPosition]+=1;
                        if(board->tiles[nextPosition])
                            break;
                        
                        nextPosition+=delta;
                    }
                    
                }
                for(int i=0;i<4;i++){
                    int delta=bishopdeltas[i];
                    int nextPosition=nextPiece.location+delta;
                    while(IS_ON_BOARD(nextPosition)){
                        attackMap[nextPosition]+=1;
                        if(board->tiles[nextPosition])
                            break;
                        
                        nextPosition+=delta;
                    }
                    
                }
                break;
            case rook:
                for(int i=0;i<4;i++){
                    int delta=rookdeltas[i];
                    int nextPosition=nextPiece.location+delta;
                    while(IS_ON_BOARD(nextPosition)){
                        attackMap[nextPosition]+=1;
                        if(board->tiles[nextPosition])
                            break;
                        
                        nextPosition+=delta;
                    }
                    
                }
                break;
            case bishop:
                for(int i=0;i<4;i++){
                    int delta=bishopdeltas[i];
                    int nextPosition=nextPiece.location+delta;
                    while(IS_ON_BOARD(nextPosition)){
                        attackMap[nextPosition]+=1;
                        if(board->tiles[nextPosition])
                            break;
                        
                        nextPosition+=delta;
                    }
                    
                }
                break;
            default:
                printf("Error no default case in create attack map");
                break;
        }
    }
    
    
    /*   for(int i =0;i<127;i++){
     if((i&0x0F)==0x08)
     printf("\n");
     if((i&0x0F)>=0x08)
     continue;
     printf("%d ",attackMap[i]);
     
     }
     printf("\n");*/
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
        
        if(attack==0||nextPiece.piece==pawn||nextPiece.location==NO_LOCATION)
            continue;
        
        switch(nextPiece.piece){
            case knight:
                if(attack==ATTACK_N){
                    return 1;
                }
                break;
            case king:
                if(attack==ATTACK_KQR||attack==ATTACK_KQBwP||attack==ATTACK_KQBbP){
                    return 1;
                }
                break;
            case queen:
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
                break;
            case rook:
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
                break;
            case bishop:
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
                break;
        }
    }
    return 0;
}

ChError doMove(ChessBoard* board, Move* move, History* history){
    if(board==NULL||move==NULL)
        return ChError_Arguments;
   
#ifdef DEBUG
    u_int64_t zobrist=getZobristHash(board);
    assert(board->zobrist==zobrist);
#endif
    assert(board->tiles[move->from]!=NULL);
    Color myColor=board->colorToPlay;
    PieceInfo* fromPiece=board->tiles[move->from];
   
    history->castlingRights=board->castlingRights;
    history->previousEnPassantSquare=board->enPassantSquare;
    history->oldRepetitionMoves=board->repetitionMoves;
    history->zobrist=board->zobrist;

    
    //capture piece
    if(board->tiles[move->to]!=0x0){
        history->capturedPiece=board->tiles[move->to];
        assert(history->capturedPiece->piece!=king);
        removePieceZobrist(&board->zobrist,move->to, history->capturedPiece->piece,myColor==WHITE?BLACK:WHITE);
        board->tiles[move->to]=NULL;
        history->capturedPiece->location=NO_LOCATION;
      
    }
    
    
    removePieceZobrist(&board->zobrist,move->from, board->tiles[move->from]->piece,myColor);
    board->tiles[move->to]=board->tiles[move->from];
    board->tiles[move->from]=NULL;
    board->tiles[move->to]->location=move->to;
    addPieceZobrist(&board->zobrist,move->to, board->tiles[move->to]->piece,myColor);
    //reset enpassant
    board->enPassantSquare=NO_LOCATION;
    
  
    
    board->castlingRights&=castlingRights[move->from];
    board->castlingRights&=castlingRights[move->to];
    if(board->castlingRights!=history->castlingRights){
        updateCastleRightZobrist(&board->zobrist, history->castlingRights);//remove former rights
        updateCastleRightZobrist(&board->zobrist, board->castlingRights);//add the new ones
    }
    
    
    switch (move->moveType) {
        case PAWNDOUBLE:
            board->enPassantSquare=move->from+(move->to-move->from)/0x02;
            break;
        case PROMOTION:
            removePieceZobrist(&board->zobrist,fromPiece->location, fromPiece->piece, myColor);
            fromPiece->piece=move->promote;
            fromPiece->score=getPieceScore(move->promote);
            addPieceZobrist(&board->zobrist,fromPiece->location, fromPiece->piece, myColor);
            break;
        case ENPASSANT:
            history->capturedPiece=board->tiles[move->from+COLUMN(move->to)-COLUMN(move->from)];
            board->tiles[move->from+COLUMN(move->to)-COLUMN(move->from)]=NULL;
            removePieceZobrist(&board->zobrist,move->from+COLUMN(move->to)-COLUMN(move->from), history->capturedPiece->piece,myColor==WHITE?BLACK:WHITE);
            history->capturedPiece->location=NO_LOCATION;
            break;
        case WKINGCASTLE:
            assert(board->tiles[H1]->piece==rook);
            
            removePieceZobrist(&board->zobrist,board->tiles[H1]->location,rook,myColor);
            addPieceZobrist(&board->zobrist,move->to-0x01,rook,myColor);
            
            board->tiles[H1]->location=move->to-0x01;
            board->tiles[move->to-0x01]=board->tiles[H1];
            board->tiles[H1]=NULL;
            break;
        case BKINGCASTLE:
            assert(board->tiles[H8]->piece==rook);
            
            removePieceZobrist(&board->zobrist,board->tiles[H8]->location,rook,myColor);
            addPieceZobrist(&board->zobrist,move->to-0x01,rook,myColor);
            
            board->tiles[H8]->location=move->to-0x01;
            board->tiles[move->to-0x01]=board->tiles[H8];
            board->tiles[H8]=NULL;
            break;
        case WQUEENCASTLE:
            assert(board->tiles[A1]!=NULL);  
            assert(board->tiles[A1]->piece==rook);
            
            removePieceZobrist(&board->zobrist,board->tiles[A1]->location,rook,myColor);
            addPieceZobrist(&board->zobrist,move->to+0x01,rook,myColor);
            
            board->tiles[A1]->location=move->to+0x01;
            board->tiles[move->to+0x01]=board->tiles[A1];
            board->tiles[A1]=NULL;
            break;
        case BQUEENCASTLE:
            assert(board->tiles[A8]!=NULL);  
            assert(board->tiles[A8]->piece==rook);
            assert(board->tiles[A8]->color==BLACK);
            
            removePieceZobrist(&board->zobrist,board->tiles[A8]->location,rook,myColor);
            addPieceZobrist(&board->zobrist,move->to+0x01,rook,myColor);
            
            board->tiles[A8]->location=move->to+0x01;
            board->tiles[move->to+0x01]=board->tiles[A8];
            board->tiles[A8]=NULL;
            break;
            
        default:
            break;
    }
   
    //updateColor
    board->colorToPlay=board->colorToPlay==WHITE?BLACK:WHITE;
    switchColorZobrist(&board->zobrist);
    //update enpassant
    
    setEnPassantZobrist(&board->zobrist, history->previousEnPassantSquare, board->enPassantSquare);
    
    addToMoveList(&board->playedMoves, move);
    
    if(history->capturedPiece==NULL&&board->tiles[move->to]!=pawn)
        board->repetitionMoves++;
    else
        board->repetitionMoves=0;
       
#ifdef DEBUG
    u_int64_t zobrist2=getZobristHash(board);
    assert(board->zobrist==zobrist2);
#endif
    return ChError_OK;
}

ChError undoMove(ChessBoard* board,Move* move, History* history){
    if(board==NULL||move==NULL)
        return ChError_Arguments;
#ifdef DEBUG
    u_int64_t zobrist=getZobristHash(board);
    assert(board->zobrist==zobrist);
#endif
    assert(board->tiles[move->to]!=NULL);
    board->colorToPlay=board->colorToPlay==WHITE?BLACK:WHITE;
    board->zobrist=history->zobrist;
    board->castlingRights=history->castlingRights;
    board->enPassantSquare=history->previousEnPassantSquare;
    board->repetitionMoves=history->oldRepetitionMoves;
    
    //unmovePiece
    board->tiles[move->from]=board->tiles[move->to];
    board->tiles[move->to]=NULL;
    board->tiles[move->from]->location=move->from;
    
    
    if(move->moveType!=ENPASSANT){
        if(history
           ->capturedPiece){
            board->tiles[move->to]=history->capturedPiece;
            board->tiles[move->to]->location=move->to;
        }
    }
    
    switch(move->moveType){
        case PAWNDOUBLE:
            break;
        case PROMOTION:
            board->tiles[move->from]->piece=pawn;
            board->tiles[move->from]->score=getPieceScore(pawn);
            break;
        case NORMAL:
            break;
        case ENPASSANT:
            assert(history->capturedPiece!=NULL);
            int index=move->from+COLUMN(move->to)-COLUMN(move->from);
            board->tiles[index]=history->capturedPiece;
            board->tiles[move->from+COLUMN(move->to)-COLUMN(move->from)]->location=move->from+COLUMN(move->to)-COLUMN(move->from);
            
            break;
        case WKINGCASTLE:
            assert(board->tiles[(move->to-0x01)]->piece==rook);
            
            board->tiles[H1]=board->tiles[(move->to-0x01)];
            board->tiles[H1]->location=H1;
            board->tiles[(move->to-0x01)]=NULL;            
            break;
        case BKINGCASTLE:
            assert(board->tiles[(move->to-0x01)]->piece==rook);
            
            board->tiles[H8]=board->tiles[(move->to-0x01)];
            board->tiles[H8]->location=H8;
            board->tiles[(move->to-0x01)]=NULL;            
            break;
        case WQUEENCASTLE:
            assert(board->tiles[(move->to+0x01)]->piece==rook);
            board->tiles[A1]=board->tiles[(move->to+0x01)];
            board->tiles[A1]->location=A1;
            board->tiles[(move->to+0x01)]=NULL;
            
            break;
        case BQUEENCASTLE:
            assert(board->tiles[(move->to+0x01)]->piece==rook);
            
            board->tiles[A8]=board->tiles[(move->to+0x01)];
            board->tiles[A8]->location=A8;
            board->tiles[(move->to+0x01)]=NULL;
            break;
            
        default:
            break;    
    }
    
    board->playedMoves.nextFree--;
    
#ifdef DEBUG
    u_int64_t zobrist2=getZobristHash(board);
    assert(board->zobrist==zobrist2);
#endif
    return ChError_OK;
}
int isCheck(ChessBoard* board, Color color){
    PieceInfo* myPieces=color==WHITE?board->whiteToSquare:board->blackToSquare;
    return isAttacked(board, myPieces[0].location, color==WHITE?BLACK:WHITE);
    
}
static ChError addMove(ChessBoard* board,int from, int to, PIECE promotion, enum MoveType type, MoveList* list, int usePins, Pin* pinnedPieces){
    ChError hr=ChError_OK;
#ifdef DEBUG
    u_int64_t zobrist=getZobristHash(board);
    assert(board->zobrist==zobrist);
#endif
    //if piece is pinned it can only move if it hits the pinning piece
    if(usePins&&type!=ENPASSANT){
        for(int i=0;i<9;i++){
            if(pinnedPieces[i].from!=NO_LOCATION){
                if(from==pinnedPieces[i].from){
                    if(to!=pinnedPieces[i].to){
                                               
                        int index=(from-to)+128;
                        int delta=DELTA_ARRAY[index];
                        if(delta!=pinnedPieces[i].delta&&delta!=-pinnedPieces[i].delta)
                           return hr;

                    }
                }
            }else{
                break;
            }
        }

        Move move={NO_LOCATION};
        move.from=from;
        move.to=to;
        move.moveType=type;
        move.promote=promotion;
        hr=addToMoveList(list, &move);
        
    }else{ 
        
        
        PieceInfo* temp = board->tiles[to];
        if(temp)
            temp->location=NO_LOCATION;
        if(type==ENPASSANT){
            temp=board->tiles[from+COLUMN(to)-COLUMN(from)];
            board->tiles[from+COLUMN(to)-COLUMN(from)]=NULL;
            temp->location=NO_LOCATION;
        }
     
        assert(board->tiles[from]);
        board->tiles[to]=board->tiles[from];
        board->tiles[from]->location=to;
        board->tiles[from]=NULL;
        int add=1;
        if(isCheck(board, board->colorToPlay)){
            add=0;     
        }
        
        board->tiles[from]=board->tiles[to];
        board->tiles[to]=NULL;
        if(type==ENPASSANT){
            board->tiles[from+COLUMN(to)-COLUMN(from)]=temp;
            temp->location=from+COLUMN(to)-COLUMN(from);
        }else{
            board->tiles[to]=temp;
            if(temp)
                board->tiles[to]->location=to;  
        }
        board->tiles[from]->location=from;
#ifdef DEBUG
        u_int64_t zobrist=getZobristHash(board);
        assert(board->zobrist==zobrist);
#endif    
        if(add){
            Move move={NO_LOCATION};
            move.from=from;
            move.to=to;
            move.moveType=type;
            move.promote=promotion;
            hr=addToMoveList(list, &move);
        }

    }
    
    return hr;
}


ChError generateMoveForPosition(ChessBoard* board,const PieceInfo* pieceInfo, MoveList* moveList, int usePins, Pin* pinnedPieces){
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
                        addMove(board,position, nextPosition,queen,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,knight,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,rook,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,bishop,PROMOTION, moveList,usePins,pinnedPieces);
                    }else{
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
                    }
                    
                    int homeRow=pieceInfo->color==BLACK?(position&0xF0)==0x10:(position&0xF0)==0x60;
                    if(homeRow){
                        nextPosition+=0x10*pawnDirectionModifier;
                        pieceOnNewSquare=getPieceForTile(board,nextPosition);
                        if(!pieceOnNewSquare){
                            addMove(board,position, nextPosition,0,PAWNDOUBLE, moveList,usePins,pinnedPieces);
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
                        addMove(board,position, nextPosition,queen,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,knight,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,rook,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,bishop,PROMOTION, moveList,usePins,pinnedPieces);
                    }else{
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);               
                    }
                }else if(board->enPassantSquare==nextPosition){
                    addMove(board,position, nextPosition,0,ENPASSANT, moveList,usePins,pinnedPieces); 
                }
            }
            nextPosition=position+0x0F*pawnDirectionModifier;
            if(IS_ON_BOARD(nextPosition)){
                pieceOnNewSquare=getPieceForTile(board,nextPosition);
                if(pieceOnNewSquare&&
                   pieceOnNewSquare->color!=pieceInfo->color){
                    if(((nextPosition&0xF0)==0x00&&pieceInfo->color==WHITE)||
                       ((nextPosition&0xF0)==0x70&&pieceInfo->color==BLACK)){
                        addMove(board,position, nextPosition,queen,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,knight,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,rook,PROMOTION, moveList,usePins,pinnedPieces);
                        addMove(board,position, nextPosition,bishop,PROMOTION, moveList,usePins,pinnedPieces);
                    }else{
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
                    }
                }else if(board->enPassantSquare==nextPosition){
                    addMove(board,position, nextPosition,0,ENPASSANT, moveList,usePins,pinnedPieces); 
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
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
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
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);;
                    }
                    
                }
            }
            if(board->colorToPlay==WHITE){
            if((board->castlingRights&0x8)==0x8){
                if(board->tiles[position+0x01]==NULL&&
                   board->tiles[position+0x02]==NULL&&
                   !isAttacked(board, position, enemyColor)&&
                   !isAttacked(board, position+0x01, enemyColor)&&
                   !isAttacked(board, position+0x02, enemyColor)){
                    addMove(board,position, position+0x02,0,WKINGCASTLE, moveList,usePins,pinnedPieces);
                }
            }
            if((board->castlingRights&0x4)==0x4){
                if(board->tiles[position-0x01]==NULL&&
                   board->tiles[position-0x02]==NULL&&
                   board->tiles[position-0x03]==NULL&&
                   !isAttacked(board, position, enemyColor)&&
                   !isAttacked(board, position-0x01, enemyColor)&&
                   !isAttacked(board, position-0x02, enemyColor)){
                    addMove(board,position, position-0x02,0,WQUEENCASTLE, moveList,usePins,pinnedPieces);
                }
            }
            }else{
                if((board->castlingRights&0x2)==0x2){
                    if(board->tiles[position+0x01]==NULL&&
                       board->tiles[position+0x02]==NULL&&
                       !isAttacked(board, position, enemyColor)&&
                       !isAttacked(board, position+0x01, enemyColor)&&
                       !isAttacked(board, position+0x02, enemyColor)){
                        addMove(board,position, position+0x02,0,BKINGCASTLE, moveList,usePins,pinnedPieces);
                    }
                }
                if((board->castlingRights&0x1)==0x1){
                    if(board->tiles[position-0x01]==NULL&&
                       board->tiles[position-0x02]==NULL&&
                       board->tiles[position-0x03]==NULL&&
                       !isAttacked(board, position, enemyColor)&&
                       !isAttacked(board, position-0x01, enemyColor)&&
                       !isAttacked(board, position-0x02, enemyColor)){
                        addMove(board,position, position-0x02,0,BQUEENCASTLE, moveList,usePins,pinnedPieces);
                    }
                } 
            }
            
            break;   
            
            
            
            
        case bishop:
            for(int i=0;i<4;i++){
                nextPosition=position+bishopdeltas[i];
                while(IS_ON_BOARD(nextPosition)){
                    pieceOnNewSquare=getPieceForTile(board,nextPosition);
                    if(!pieceOnNewSquare){
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces); 
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
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
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
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
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
                        addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces);
                    }else{
                        if(pieceOnNewSquare->color!=pieceInfo->color){
                            addMove(board,position, nextPosition,0,NORMAL, moveList,usePins,pinnedPieces); 
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
    
    Pin pinnedPieces[9];
    for(int i=0;i<9;i++)
        pinnedPieces[i].from=NO_LOCATION;
    
    int usePins=1;
    if(isCheck(board, color)){
        usePins=0;
    }else{
        getPinnedPiecePositions(board, color, pinnedPieces);
    }
    //generateAttackMap(board, color==WHITE?BLACK:WHITE);
    int offset=moveList->nextFree;
    for(int i=0;i<16;i++){
        if(pieceArray[i].location==-5){
            continue;
        }
        generateMoveForPosition(board,&pieceArray[i], moveList,usePins,pinnedPieces);
    }
    
    if(moveList->nextFree==offset){
        if(isCheck(board, color)){
            return ChError_CheckMate;
        }else{
            return ChError_StaleMate;
        }
    }
    
    return ChError_OK;
}
int equalMoves(Move* move1, Move* move2){
    return move1->from==move2->from&&move1->to==move2->to&&move1->promote==move2->promote;
}
ChError isLegal(ChessBoard* board, Move* move){
    MoveList moveList={0};
    
    ChError hr=generateMoves(board, board->colorToPlay, &moveList);
    if(hr)
        return hr;
    
    for(int i=0;i<moveList.nextFree;i++){
        if(equalMoves(&moveList.array[i],move)){
            freeMoveList(&moveList);
            return 1;
        }
    }
    freeMoveList(&moveList);
    return ChError_OK;
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
    if(board1->castlingRights!=board2->castlingRights)
        return 1;
    
    return 0;
}