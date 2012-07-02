//
//  perft.c
//  Chengine
//
//  Created by Henning Sperr on 6/29/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "perft.h"
long perft_rec(ChessBoard* board, Color sideToMove, int depth, MoveList* list){
    if(isCheck(board, board->colorToPlay==WHITE?BLACK:WHITE))
        return 0;
    
    if(depth==0)
        return 1;
    
    ChError hr=ChError_OK;
    long moves=0;
    
    int startOffset=list->nextFree;
    hr=generateMoves(board, sideToMove, list);
    if(hr)
        printf("Error in perft %d\n",hr);
    
    for(int i=startOffset;i<list->nextFree;i++){
        doMove(board,&list->array[i]);
        moves+=perft_rec(board,board->colorToPlay,depth-1,list);
        undoMove(board,&list->array[i]);
    }
    list->nextFree=startOffset;
    return moves;
}


long perft(ChessBoard* board, int depth){
    long moved=0;
    long iterationMoves=0;
    unsigned long time=clock();
    ChError hr=ChError_OK;
    
    MoveList moveList = {0};
    
    hr=generateMoves(board, board->colorToPlay, &moveList);
    if(hr)
        printf("Error in perft %d\n",hr);
    
    for(int i=0;i<moveList.nextFree;i++){
        iterationMoves=0;
        doMove(board,&moveList.array[i]);
        iterationMoves+=perft_rec(board,board->colorToPlay,depth-1, &moveList);
        moved+=iterationMoves;
        undoMove(board,&moveList.array[i]);
        
    }
    freeMoveList(&moveList);
    double timeNeeded=((double)(clock()-time)/CLOCKS_PER_SEC);
    printf("Found %ld moves in %f sec makes %f moves/sec\n",moved,timeNeeded,(float)(moved/timeNeeded));
    
    return moved;
}

long perft_hash_rec(ChessBoard* board, Color sideToMove, int depth, MoveList* list){
    if(isCheck(board, board->colorToPlay==WHITE?BLACK:WHITE))
        return 0;
    
    if(depth==0)
        return 1;

    
    ChError hr=ChError_OK;
    long moves=0;
    int iterationMoves;
    
    int startOffset=list->nextFree;
    hr=generateMoves(board, sideToMove, list);
    if(hr)
        printf("Error in perft %d\n",hr);
    
    for(int i=startOffset;i<list->nextFree;i++){
        Move* move=&list->array[i];
        doMove(board,move);
        if((hr=probe(board->zobrist, depth-1, &iterationMoves))){
            moves+=perft_hash_rec(board,board->colorToPlay,depth-1,list);
        }else{
            moves+=iterationMoves;
            iterationMoves=0;
        }
        undoMove(board,&list->array[i]);
    }
    list->nextFree=startOffset;
    addKeyToTable(board->zobrist, depth, (int)moves);
    return moves;
}

long perft_hash(ChessBoard* board, int depth){
    long moved=0;
    long iterationMoves=0;
    unsigned long time=clock();
    ChError hr=ChError_OK;
    
    MoveList moveList = {0};
    
    hr=generateMoves(board, board->colorToPlay, &moveList);
    if(hr)
        printf("Error in perft %d\n",hr);
    
    for(int i=0;i<moveList.nextFree;i++){
        iterationMoves=0;
        doMove(board,&moveList.array[i]);
        iterationMoves+=perft_hash_rec(board,board->colorToPlay,depth-1, &moveList);
        moved+=iterationMoves;
        undoMove(board,&moveList.array[i]);
    }
    freeMoveList(&moveList);
    double timeNeeded=((double)(clock()-time)/CLOCKS_PER_SEC);
    printf("Found %ld moves in %f sec makes %f moves/sec\n",moved,timeNeeded,(float)(moved/timeNeeded));
    
    return moved;
}

void divide(ChessBoard* board, int depth){
    long moved=0;
    long iterationMoves=0;
    unsigned long time=clock();
    ChError hr=ChError_OK;
    char moveAsChar[6];
    
    MoveList moveList = {0};
    
    hr=generateMoves(board, board->colorToPlay, &moveList);
    if(hr)
        printf("Error in perft %d\n",hr);
    for(int i=0;i<moveList.nextFree;i++){
        iterationMoves=0;
        iterationMoves+=perft_rec(board,board->colorToPlay,depth-1, &moveList);
        moved+=iterationMoves;
        undoMove(board,&moveList.array[i]);
        
        moveToChar(&moveList.array[i],moveAsChar);
        printf("For move %s I found %ld moves after %f sec .\n",moveAsChar,iterationMoves,(float)((clock()-time)/CLOCKS_PER_SEC));
        
    }
    freeMoveList(&moveList);
    float timeNeeded=(float)((clock()-time)/CLOCKS_PER_SEC);
    printf("Found %ld moves in %f sec makes %f moves/sec\n",moved,timeNeeded,(float)(moved/timeNeeded));
    
}