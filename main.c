//
//  main.c
//  Chengine
//
//  Created by Henning Sperr on 6/7/12.
//  Copyright 2012 Henning Sperr. All rights reserved.
//

#include "Chengine.h"



int main (int argc, const char * argv[])
{
    
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    
    Game newGame;
    ChError hr;
    initTable(1048576, 0xFFFFF);
    
    newGame.isRunning=0;
    newGame.aiPlayer[WHITE].depth=10;
    newGame.aiPlayer[WHITE].timelimit=500;
    newGame.aiPlayer[WHITE].isAi=1;
    
    newGame.aiPlayer[BLACK].depth=7;
    newGame.aiPlayer[BLACK].timelimit=500;
    newGame.aiPlayer[BLACK].isAi=0;
    
    if((hr=initBoard(&newGame.board))){
        printf("After init board:");
        printError(hr);
    }

    //readFENString(&newGame.board, "k7/8/8/3p4/2P1P3/8/8/K7 w - - 0 1");
         printBoardE(&newGame.board);
    while(newGame.isRunning){
            
        if(newGame.aiPlayer[newGame.board.colorToPlay].isAi){
            hr=doAiMove(&newGame.board, &newGame.aiPlayer[newGame.board.colorToPlay]);
            switch (hr) {
                case ChError_OK:
                    break;
                case ChError_CheckMate:
                    if(newGame.board.colorToPlay==WHITE){
                        printf("0-1 {Black mates}\n");
                        newGame.aiPlayer[BLACK].isAi=0;
                        newGame.aiPlayer[WHITE].isAi=0;
                    }else{
                        printf("1-0 {White mates}\n");
                        newGame.aiPlayer[BLACK].isAi=0;
                        newGame.aiPlayer[WHITE].isAi=0;
                    }
                    break;
                case ChError_StaleMate:
                        printf("1/2-1/2 {Stalemate}\n");
                        newGame.aiPlayer[BLACK].isAi=0;
                        newGame.aiPlayer[WHITE].isAi=0;
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

    
    //TESTING
  
     
    printf("Perfting\n");
    
   // readFENString(&newGame.board, "rnbqkbnr/1ppppppp/p7/1B6/4P3/8/PPPP1PPP/RNBQK1NR b KQkq - 1 2");
   printBoardE(&newGame.board);
    perft(&newGame.board,1);
    perft(&newGame.board,2);
    perft(&newGame.board,3);
    perft(&newGame.board,4);
   // perft(&newGame.board,5);
   // perft(&newGame.board,6);
    
   // 

    printf("Perfting 1 2 3 4\n");
    perft_hash(&newGame.board,1);
    perft_hash(&newGame.board,2);
    perft_hash(&newGame.board,3);
    perft_hash(&newGame.board,4);
    printf("Perfting 5 6 7 8\n");

    perft_hash(&newGame.board,5);
    perft_hash(&newGame.board,5);

   // perft_hash(&newGame.board,6);
   // perft_hash(&newGame.board,7);
   // perft_hash(&newGame.board,8);
    
    
    freeTable();
    return 0;
}



