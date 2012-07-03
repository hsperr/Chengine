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
    
    newGame.isRunning=0;
    newGame.aiPlayer[WHITE].depth=6;
    newGame.aiPlayer[WHITE].timelimit=500;
    newGame.aiPlayer[WHITE].isAi=0;
    
    newGame.aiPlayer[BLACK].depth=6;
    newGame.aiPlayer[BLACK].timelimit=500;
    newGame.aiPlayer[BLACK].isAi=1;
    
    if((hr=initBoard(&newGame.board))){
        printError(hr);
    }
     printBoardE(&newGame.board);
    
    while(newGame.isRunning){
        if(newGame.aiPlayer[newGame.board.colorToPlay].isAi){
            doAiMove(&newGame.board, &newGame.aiPlayer[newGame.board.colorToPlay]);
             printBoardE(&newGame.board);
        }else{
            checkForInput(&newGame); 
        }
    }

    
    //TESTING
    //readFENString(&board, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
    //readFENString(&newGame.board, "2k5/8/8/8/p7/8/8/4K3 b - - 0 1");

   // printBoardE(&board);
   // divide(&board,2);
    
    /*Properties aiProp;
    aiProp.depth=8;
    aiProp.timelimit=100;
    long time=clock();
    doAiMove(&newGame.board, &aiProp);
    printf("Took %f \n",((double)clock()-time)/CLOCKS_PER_SEC);*/
  
    
    printf("Perfting\n");
    initTable(67108864, 0x3FFFFFF);
   // readFENString(&newGame.board, "rnbqkbnr/1ppppppp/p7/1B6/4P3/8/PPPP1PPP/RNBQK1NR b KQkq - 1 2");
    printBoardE(&newGame.board);
    perft(&newGame.board,1);
    perft(&newGame.board,2);
    perft(&newGame.board,3);
    perft(&newGame.board,4);
    perft(&newGame.board,5);
    //perft(&newGame.board,6);
    
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
    perft_hash(&newGame.board,8);
    
    
    //freeTable();
    return 0;
}



