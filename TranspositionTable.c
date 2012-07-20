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
    Bound bound;      //0=exact,1=lowerbound,2=upperbound
    Move move;    //best move for ordering
//    Move betaCutoff move

}TableEntry;


u_int64_t BLACKTOPLAY;
u_int64_t CASTLING_RIGHTS[16]; //KQkq
u_int64_t PIECES[6][2][64];
u_int64_t enPassant[8];


TableEntry* hashTable;
long hashTableSize=0;


typedef struct RepEntry{
    u_int64_t zobrist;
    int count;
}RepEntry;

RepEntry* repetitionTable;
long repetitionTableSize=0;

typedef struct EvalEntry{
    u_int64_t zobrist;
    int count;
}EvalEntry;

EvalEntry* evalTable;
long evalTableSize=0;

ChError initEvalTable(long size){
    evalTableSize=size;
    
    evalTable=malloc(size*sizeof(EvalEntry));
    if(!evalTable)
        return ChError_Resources;
    memset(repetitionTable,0,sizeof(EvalEntry)*evalTableSize);
    
    return ChError_OK;

}
ChError clearEvalTable(){
    memset(repetitionTable,0,sizeof(RepEntry)*repetitionTableSize);
    return ChError_OK;
}

ChError probeEvalTable(u_int64_t* zobrist, int* eval){
    u_int64_t index=*(zobrist)%evalTableSize;
   /* char wrappedaround=0;
    while(evalTable[index].zobrist!=*zobrist&&
          evalTable[index].zobrist!=0){
        index++;
        if(index>=repetitionTableSize){
            if(!wrappedaround){
                index=0;
                wrappedaround=1;
            }else{
                return ChError_NotInTable;                
            }
        }
    }*/
    if(evalTable[index].zobrist!=*zobrist){
        return ChError_NotInTable;
    }else{
        *eval=evalTable[index].count;
    }
    return ChError_OK;
    
}

ChError addToEvalTable(u_int64_t* zobrist, int eval){
    u_int64_t index=*(zobrist)%evalTableSize;
   /* char wrappedaround=0;
    while(evalTable[index].zobrist!=*zobrist&&
          evalTable[index].zobrist!=0){
        index++;
        if(index>=repetitionTableSize){
            if(!wrappedaround){
                index=0;
                wrappedaround=1;
            }else{
                clearEvalTable();
                return addToEvalTable(zobrist,eval);
                
            }
        }
    }*/
    
    evalTable[index].zobrist=*zobrist;
    evalTable[index].count=eval;
    return ChError_OK;
    
}


ChError incrementRepetitionTable(u_int64_t* zobrist){
    u_int64_t index=*(zobrist)%repetitionTableSize;
    RepEntry entry=repetitionTable[index];
    while(entry.zobrist!=*zobrist&&
          entry.zobrist!=0){
        index++;
        if(index>=repetitionTableSize)
            index=0;
        
        entry=repetitionTable[index];
    }

        
    if(repetitionTable[index].zobrist==*zobrist){
        repetitionTable[index].count++;
    }else{
        repetitionTable[index].zobrist=*zobrist;
        repetitionTable[index].count++;
    }
    return ChError_OK;
}
ChError decrementRepetitionTable(u_int64_t* zobrist){
    u_int64_t index=*(zobrist)%repetitionTableSize;
    while(repetitionTable[index].zobrist!=*zobrist&&
          repetitionTable[index].zobrist!=0){
        index++;
        if(index>=repetitionTableSize)
            index=0;
    }
    
    assert(repetitionTable[index].zobrist!=0);
        repetitionTable[index].count--;
    if(repetitionTable[index].count<=0){
        repetitionTable[index].zobrist=0;
    }
    
    return ChError_OK;
}
int probeRepetitionTable(u_int64_t* zobrist){
    u_int64_t index=*(zobrist)%repetitionTableSize;
    RepEntry entry=repetitionTable[index];
    while(entry.zobrist!=*zobrist&&
          entry.zobrist!=0){
        index++;
        if(index>=repetitionTableSize)
            index=0;
        
        entry=repetitionTable[index];
    }
    
    return repetitionTable[index].count;
}

void clearRepetitionTable(){
    memset(repetitionTable,0,sizeof(RepEntry)*repetitionTableSize);
}

static u_int64_t rand64(void)
{
    return rand() ^ ((u_int64_t)rand() << 15) ^ ((u_int64_t)rand() << 30) ^
    ((u_int64_t)rand() << 45) ^ ((u_int64_t)rand() << 60);
}

ChError initRepetitionTable(long size){
    repetitionTableSize=size;
    repetitionTable=malloc(size*sizeof(RepEntry));
    if(!repetitionTable)
        return ChError_Resources;
    memset(repetitionTable,0,sizeof(RepEntry)*repetitionTableSize);
    
    return ChError_OK;
}

ChError initHashTable(long sizeInMb){
    hashTableSize=sizeInMb;
    srand(17L);
    
     BLACKTOPLAY=rand64();
   
    for(int i=0;i<16;i++){
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
    
    hashTable=malloc(hashTableSize*sizeof(TableEntry));
    if(!hashTable)
        return ChError_Resources;
    
    memset(hashTable,0,sizeof(TableEntry)*hashTableSize);
    
    
    return ChError_OK;

}
ChError clearHashTable(void){
    
    memset(hashTable, 0, sizeof(TableEntry)*hashTableSize);
    
    return ChError_OK;
}

static int locationToIndex(int location){
    return location-(((location&0xF0)>>4)*8);
}
u_int64_t getZobristHash(ChessBoard* board){
    u_int64_t zobrist=0;
    if(board->colorToPlay==BLACK){
        zobrist^=BLACKTOPLAY;
    }
    for(int i=0;i<16;i++){
        if(board->whiteToSquare[i].location>-1){
            zobrist^=PIECES[board->whiteToSquare[i].piece][WHITE][locationToIndex(board->whiteToSquare[i].location)];
        }
        if(board->blackToSquare[i].location>-1){
            zobrist^=PIECES[board->blackToSquare[i].piece][BLACK][locationToIndex(board->blackToSquare[i].location)];
        }
        
    }
    
    zobrist^=CASTLING_RIGHTS[board->castlingRights];
   
    
    if(board->enPassantSquare>=0){ //is hex like 0x54
        int col=(board->enPassantSquare&0x0F);
        assert(col>=0x00&&col<=0x07);
        zobrist^=enPassant[col];
    }
   // printf("zobrist is %lu\n",zobrist);
    return zobrist;
}

ChError probe(u_int64_t zobrist, int depth, int* alpha, int* beta, int* score, Move* move){
    if(depth==0)
        return ChError_NotInTable;
    
    long index=zobrist%hashTableSize;
    
    TableEntry* entry=&hashTable[index];
    
    if(entry->zobrist==zobrist){
        *move=entry->move;
        //we found that position before
        if(entry->depth>=depth){
            switch (entry->bound){
                case 0:
                    *score=entry->score;
                    return ChError_OK;
                case 1: //lower
                    *alpha = max(*alpha, entry->score);
                    break;
                case 2: //upper
                    *beta = min(*beta, entry->score);
                    break;
            }     
            if (*alpha >= *beta) {
                *score=entry->score;
                return ChError_OK;
            }
        }else{
            return ChError_DepthToLow;
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
ChError addKeyToTable(u_int64_t zobrist, int depth, int score, Bound bound, Move move){
    if(depth==0)
        return ChError_OK;
    
    u_int64_t index=zobrist%hashTableSize;

    TableEntry* entry=&hashTable[index];
    

        //collision //always replace
        entry->zobrist=zobrist;
        entry->depth=depth;
        entry->score=score;
        entry->bound=bound;
        entry->move=move;
    
    return ChError_OK;
}

void freeTable(){
    if(hashTable)
        free(hashTable);
    
    if(repetitionTable)
        free(repetitionTable);
    
    if(evalTable)
        free(evalTable);
}