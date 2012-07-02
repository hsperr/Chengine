//
//  perft.c
//  Chengine
//
//  Created by Henning Sperr on 6/29/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "perft.h"
long perft_rec(ChessBoard* board, Color sideToMove, int depth, MoveList* list){
    if(depth==0)
        return 1;
    
    if(isCheck(board, board->colorToPlay==WHITE?BLACK:WHITE))
        return 0;
    
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
    char moveAsChar[6];
    
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
        
        moveToChar(&moveList.array[i],moveAsChar);
        //printf("For move %s I found %d moves after %f sec .\n",moveAsChar,iterationMoves,(float)((clock()-time)/CLOCKS_PER_SEC));
        
    }
    freeMoveList(&moveList);
    double timeNeeded=((double)(clock()-time)/CLOCKS_PER_SEC);
    printf("Found %ld moves in %f sec makes %f moves/sec\n",moved,timeNeeded,(float)(moved/timeNeeded));
    
    return moved;
}

void divide(ChessBoard* board, int depth){
    int moved=0;
    int iterationMoves=0;
    unsigned long time=clock();
    ChError hr=ChError_OK;
    char moveAsChar[6];
    
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
        
        moveToChar(&moveList.array[i],moveAsChar);
        printf("For move %s I found %d moves after %f sec .\n",moveAsChar,iterationMoves,(float)((clock()-time)/CLOCKS_PER_SEC));
        
    }
    freeMoveList(&moveList);
    float timeNeeded=(float)((clock()-time)/CLOCKS_PER_SEC);
    printf("Found %d moves in %f sec makes %f moves/sec\n",moved,timeNeeded,(float)(moved/timeNeeded));
    
}