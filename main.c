//
//  main.c
//  Chengine
//
//  Created by Henning Sperr on 6/7/12.
//  Copyright 2012 Henning Sperr. All rights reserved.
//

#include "Chengine.h"
#include "Board.h"
#include "perft.h"
#include "NegaScout.h"
#include "Input.h"


int main (int argc, const char * argv[])
{
    
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    
    Game newGame;
    ChError hr;
    
    newGame.isRunning=1;
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
    //readFENString(&board, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1R1K b kq - 1 1");
    //readFENString(&newGame.board, "2k5/8/8/8/p7/8/8/4K3 b - - 0 1");
    printBoardE(&newGame.board);
    
   // printBoardE(&board);
   // divide(&board,2);
    
    /*Properties aiProp;
    aiProp.depth=8;
    aiProp.timelimit=100;
    long time=clock();
    doAiMove(&newGame.board, &aiProp);
    printf("Took %f \n",((double)clock()-time)/CLOCKS_PER_SEC);*/
  
    
    printf("Perfting\n");
    
    perft(&newGame.board,1);
    perft(&newGame.board,2);
    perft(&newGame.board,3);
    perft(&newGame.board,4);
    perft(&newGame.board,5);
    perft(&newGame.board,6);
        
    
    

    return 0;
}



