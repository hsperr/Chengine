//
//  main.c
//  Chengine
//
//  Created by Henning Sperr on 6/7/12.
//  Copyright 2012 Henning Sperr. All rights reserved.
//

#include "Chengine.h"
#include "OpeningBook.h"
#include "TestFramework.h"



int main (int argc, const char * argv[])
{
    ChError hr;    
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    
    Game newGame;
    initHashTable(65536*2*2*2*2*2*2);
    initRepetitionTable();
    initEvalTable(65536*2*2*2*2*2*2);
  
    
    if((hr=initBoard(&newGame.board))){
        printf("After init board:");
        printError(hr);
    }
    


    
    if(argc>1&&*argv[1]=='a'){
          testPositions(&newGame.board); 
        //optimizeParameters(&newGame.board);
    }else{
    
    newGame.isRunning=1;
    
    newGame.Player[WHITE].depth=200;
    newGame.Player[WHITE].timelimit=9000;
    newGame.Player[WHITE].isAi=1;
    newGame.Player[WHITE].useOpeningTable=1;
    
    newGame.Player[BLACK].depth=200;
    newGame.Player[BLACK].timelimit=9000;
    newGame.Player[BLACK].isAi=1;
    newGame.Player[BLACK].useOpeningTable=1;
    


 
    
    //readFENString(&newGame.board, "3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - - 0 0");
    //printf("Evaluate: %d\n",evaluate(&newGame.board));
    printBoardE(&newGame.board);
    while(newGame.isRunning){
        if(newGame.Player[newGame.board.colorToPlay].isAi){
            hr=doAi(&newGame.board, &newGame.Player[newGame.board.colorToPlay]);
            switch (hr) {
                case ChError_OK:
                    break;
                case ChError_CheckMate:
                    if(newGame.board.colorToPlay==WHITE){
                        printf("0-1 {Black mates}\n");
                        newGame.Player[BLACK].isAi=0;
                        newGame.Player[WHITE].isAi=0;
                    }else{
                        printf("1-0 {White mates}\n");
                        newGame.Player[BLACK].isAi=0;
                        newGame.Player[WHITE].isAi=0;
                    }
                    break;
                case ChError_StaleMate:
                        printf("1/2-1/2 {Stalemate}\n");
                        newGame.Player[BLACK].isAi=0;
                        newGame.Player[WHITE].isAi=0;
                    break;
                case ChError_RepetitionDraw:
                    printf("1/2-1/2 {Repetition Draw}\n");
                    newGame.Player[BLACK].isAi=0;
                    newGame.Player[WHITE].isAi=0;
                   
                    break;

                default:
                     printf("After doAiMove:");
                    printError(hr);
                    break;
            }
        }else{
            checkForInput(&newGame); 
        }
    }
    }
    
    freeTable();
    clearBoard();
    return 0;
}



