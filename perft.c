//
//  perft.c
//  Chengine
//
//  Created by Henning Sperr on 6/29/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "perft.h"
static long perft_rec(ChessBoard* board, Color sideToMove, int depth, MoveList* list){
    if(depth==0)
        return 1;
    
    ChError hr=ChError_OK;
    long moves=0;
    
    int startOffset=list->nextFree;
    hr=generateMoves(board, sideToMove, list);
    if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate){
        printf("perft rec generate move");
        printError(hr);
    }
    
    if(depth==1){
        int movesFound=list->nextFree-startOffset;
        list->nextFree=startOffset;
        return movesFound;
    }
    
    for(int i=startOffset;i<list->nextFree;i++){
        History h={0};
        Move* moveToDo=&list->array[i];
        doMove(board,moveToDo,&h);
        moves+=perft_rec(board,board->colorToPlay,depth-1,list);
        undoMove(board,moveToDo,&h);

    }
    list->nextFree=startOffset;
    return moves;
}


long perft(ChessBoard* board, int depth){
    long moved=0;
    unsigned long time=clock();
    ChError hr=ChError_OK;
    
    
    MoveList moveList = {0};
    
    hr=generateMoves(board, board->colorToPlay, &moveList);
    if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate){
         printf("perft after generate move:");
        printError(hr);
    }
    
    for(int i=0;i<moveList.nextFree;i++){
        History h={0};
        Move* move=&moveList.array[i];
        doMove(board,move,&h);
        moved+=perft_rec(board,board->colorToPlay,depth-1, &moveList);
        undoMove(board,move,&h);
        
    }
    freeMoveList(&moveList);
    double timeNeeded=((double)(clock()-time)/CLOCKS_PER_SEC);
    printf("Found %ld moves in %f sec makes %f Million Moves/sec\n",moved,timeNeeded,(float)(moved/timeNeeded)/1000000);
    
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
        printError(hr);
    for(int i=0;i<moveList.nextFree;i++){
        History h={0};
        Move* move=&moveList.array[i];
        doMove(board, move, &h);
        iterationMoves=0;
        iterationMoves+=perft_rec(board,board->colorToPlay,depth-1, &moveList);
        moved+=iterationMoves;
        undoMove(board,move,&h);
        
        moveToChar(&moveList.array[i],moveAsChar);
        printf("%s %ld %s in %f sec .\n",moveAsChar,iterationMoves,move->moveType==NORMAL?"N":"S",(float)((clock()-time)/CLOCKS_PER_SEC));
        
    }
    freeMoveList(&moveList);
    float timeNeeded=(float)((clock()-time)/CLOCKS_PER_SEC);
    printf("Found %ld moves in %f sec makes %f moves/sec\n",moved,timeNeeded,(float)(moved/timeNeeded));
    
}