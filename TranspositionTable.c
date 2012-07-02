//
//  TranspositionTable.c
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "TranspositionTable.h"

typedef struct TableEntry{
    int64_t zobrist; //collision detection
    int depth;    //depth searched to, from this keys position
    int score;    //score found from this position
//    Move move;    //best move for ordering
//    Move betaCutoff move

}TableEntry;

u_int64_t BLACKTOPLAY;
u_int64_t CASTLING_RIGHTS[4]; //KQkq
u_int64_t PIECES[6][2][64];
u_int64_t enPassant[8];

u_int64_t indexMask=0;

TableEntry* hashTable;

static unsigned long
x=123456789,
y=362436069,
z=521288629,
w=88675123,
v=886756453;
/* replace defaults with five random seed values in calling program */

static unsigned long xorshift(void)
{
    unsigned long t = x^(x>>7);
    x=y; y=z; z=w; w=v;
    v=(v^(v<<6))^(t^(t<<13));
    return (y+y+1)*v;
}
static u_int64_t rand64() {
    return ((u_int64_t)(xorshift())<<32)|xorshift();
}


ChError initTable(long size, long inMask){
    srand(17L);
    
     BLACKTOPLAY=rand64();
   
    for(int i=0;i<4;i++){
        CASTLING_RIGHTS[i]=rand64();
    }
    
    for(int i=0;i<6;i++){
        for(int j=0;j<2;j++){
            for(int k=0;k<64;k++){
                PIECES[i][j][k]=rand64();
            }
        }
    }
    
    for(int i=0;i<8;i++){
        enPassant[i]=rand64();
    }
    
    hashTable=malloc(size*sizeof(TableEntry));
    if(!hashTable)
        return ChError_Resources;
    
    for(long i=0;i<size;i++){
        hashTable[i].zobrist=0;
    }
    indexMask=inMask;
    
    return ChError_OK;

}
int locationToIndex(int location){
    return location-(((location&0xF0)>>4)*8);
}
u_int64_t getZobristHash(ChessBoard* board){
    long zobrist=0;
    if(board->colorToPlay==BLACK){
        zobrist^=BLACKTOPLAY;
    }
    for(int i=0;i<16;i++){
        if(board->whiteToSquare[i].location>-1){
            zobrist^=PIECES[board->whiteToSquare[i].piece][WHITE][locationToIndex(board->whiteToSquare[i].location)];
        }
        if(board->blackToSquare[i].location>-1){
            zobrist^=PIECES[board->blackToSquare[i].piece][WHITE][locationToIndex(board->blackToSquare[i].location)];
        }
        
    }
    if(board->rights[WHITE].queenCastlingPossible){
        zobrist^=CASTLING_RIGHTS[1];
    }
    if(board->rights[WHITE].kingCastlingPossible){
        zobrist^=CASTLING_RIGHTS[0];
    }
    if(board->rights[BLACK].queenCastlingPossible){
        zobrist^=CASTLING_RIGHTS[3];
    }
    if(board->rights[BLACK].kingCastlingPossible){
        zobrist^=CASTLING_RIGHTS[2];
    }
    
    if(board->enPassantSquare>=0){ //is hex like 0x54
        int col=(board->enPassantSquare&0x0F);
        assert(col>=0x00&&col<=0x07);
        zobrist^=enPassant[col];
    }
   // printf("zobrist is %lu\n",zobrist);
    return zobrist;
}

ChError probe(u_int64_t zobrist, int depth,int* score){
    if(depth==0)
        return ChError_NotInTable;
    
    long index=zobrist&indexMask;
    
    TableEntry* entry=&hashTable[index];
    if(!entry)
        return ChError_NotInTable;
    
    if(entry->zobrist==zobrist){
        //we found that position before
        if(entry->depth==depth){
            *score=entry->score;
            return ChError_OK;
        }else{
            return Cherror_DepthToLow;
        }
    }
    
    return ChError_NotInTable;
    
}
ChError addPieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color){
    int index=locationToIndex(location);
    *zobrist^=PIECES[piece][color][index];

    return ChError_OK;
}
ChError removePieceZobrist(u_int64_t* zobrist,int location,PIECE piece, Color color){
     *zobrist^=PIECES[piece][color][locationToIndex(location)];
    return ChError_OK;
}
ChError switchColorZobrist(u_int64_t* zobrist){
    *zobrist^=BLACKTOPLAY;
    return ChError_OK;
}
ChError updateCastleRightZobrist(u_int64_t* zobrist, int nr){//KQkq
    *zobrist^=CASTLING_RIGHTS[nr];
    return ChError_OK;
}
ChError setEnPassantZobrist(u_int64_t* zobrist, int oldEnPassant, int newEnPassant){
    if(oldEnPassant!=-5){
    *zobrist^=enPassant[(oldEnPassant&0x0F)];
    }
    if(newEnPassant!=-5){
        *zobrist^=enPassant[(newEnPassant&0x0F)];
    }
    return ChError_OK;
}

ChError addKeyToTable(u_int64_t zobrist, int depth, int score){
    if(depth==0)
        return ChError_OK;
    
    u_int64_t index=zobrist&indexMask;

    TableEntry* entry=&hashTable[index];
    
    if(entry->zobrist!=0&&entry->zobrist!=zobrist){
        //collision //always replace
        entry->zobrist=zobrist;
        entry->depth=depth;
        entry->score=score;
    }
    if(entry->zobrist==zobrist){
        //we found that position before
        if(entry->depth<depth){
            entry->depth=depth;
            entry->score=score;
        }
    }else{
        entry->zobrist=zobrist;
        entry->depth=depth;
        entry->score=score;
    }
    return ChError_OK;
}

void freeTable(void){
    if(hashTable)
        free(hashTable);
}