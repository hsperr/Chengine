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
    initTable(65536, 0xFFFF);
    
    newGame.isRunning=1;
    newGame.aiPlayer[WHITE].depth=7;
    newGame.aiPlayer[WHITE].timelimit=500;
    newGame.aiPlayer[WHITE].isAi=0;
    
    newGame.aiPlayer[BLACK].depth=7;
    newGame.aiPlayer[BLACK].timelimit=500;
    newGame.aiPlayer[BLACK].isAi=1;
    
    if((hr=initBoard(&newGame.board))){
        printError(hr);
    }

    //readFENString(&newGame.board, "1r2kb1r/1pp1pppp/2Q2n2/8/3P4/4B2P/1PP1BP2/R3R1K1 b k - 1 2");
         printBoardE(&newGame.board);
    while(newGame.isRunning){
        if(newGame.aiPlayer[newGame.board.colorToPlay].isAi){
            doAiMove(&newGame.board, &newGame.aiPlayer[newGame.board.colorToPlay]);
            
        }else{
            checkForInput(&newGame); 
        }
    }

    
    //TESTING
  
    /* 
    printf("Perfting\n");
    
   // readFENString(&newGame.board, "rnbqkbnr/1ppppppp/p7/1B6/4P3/8/PPPP1PPP/RNBQK1NR b KQkq - 1 2");
   printBoardE(&newGame.board);
    perft(&newGame.board,1);
    perft(&newGame.board,2);
    perft(&newGame.board,3);
    perft(&newGame.board,4);
    perft(&newGame.board,5);
    perft(&newGame.board,6);
    
   // 

    printf("Perfting 1 2 3 4\n");
    perft_hash(&newGame.board,1);
    perft_hash(&newGame.board,2);
    perft_hash(&newGame.board,3);
    perft_hash(&newGame.board,4);
    printf("Perfting 5 6 7 8\n");
    perft_hash(&newGame.board,5);
    perft_hash(&newGame.board,5);

    perft_hash(&newGame.board,6);
    perft_hash(&newGame.board,7);
    perft_hash(&newGame.board,8);*/
    
    
    //freeTable();
    return 0;
}



