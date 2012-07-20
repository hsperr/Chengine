//
//  main.c
//  Chengine
//
//  Created by Henning Sperr on 6/7/12.
//  Copyright 2012 Henning Sperr. All rights reserved.
//

#include "Chengine.h"
#include "OpeningBook.h"



int main (int argc, const char * argv[])
{
    
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);
    
    Game newGame;
    ChError hr;
    initTable(1048576*16);
    
    newGame.isRunning=1;
    
    newGame.Player[WHITE].depth=200;
    newGame.Player[WHITE].timelimit=40000000;
    newGame.Player[WHITE].isAi=0;
    newGame.Player[WHITE].useOpeningTable=1;
    
    newGame.Player[BLACK].depth=200;
    newGame.Player[BLACK].timelimit=30000000;
    newGame.Player[BLACK].isAi=1;
    newGame.Player[BLACK].useOpeningTable=1;
    
    if((hr=initBoard(&newGame.board))){
        printf("After init board:");
        printError(hr);
    }

    readFENString(&newGame.board, "r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b -  0 0");
    //printf("Evaluate: %d\n",evaluate(&newGame.board));
    printBoardE(&newGame.board);
    while(newGame.isRunning){
        if(newGame.Player[newGame.board.colorToPlay].isAi){
            //hr=doAiMove(&newGame.board, &newGame.aiPlayer[newGame.board.colorToPlay]);
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

    
    //TESTING
    //test openingbook
    
    //assert(isLegal(&newGame.board, &openingMove));
     /*
    printf("Perfting\n");
    readFENString(&newGame.board, "3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - - 0 0");
   // readFENString(&newGame.board, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
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
   // perft_hash(&newGame.board,7);
   // perft_hash(&newGame.board,8);
    
    */
    freeTable();
    return 0;
}



