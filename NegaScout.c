//
//  NegaScout.c
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "NegaScout.h"

static int evaluate(ChessBoard* board){
    PieceInfo* myPieces=board->colorToPlay==WHITE?board->whiteToSquare:board->blackToSquare;
    PieceInfo* enemyPieces=board->colorToPlay==BLACK?board->whiteToSquare:board->blackToSquare;
    
    int score=0;
    
    for(int i=0;i<16;i++){
        if(myPieces[i].location>=0){
            score+=myPieces[i].score;
        }
        if(enemyPieces[i].location>=0){
            score-=enemyPieces[i].score;
        }
    }
    
    return score;
    
}
/*
int quis(ChessBoard* board,int alpha, int beta){
    int score=evaluate(board);
    
    if(score>beta)
        return beta;
    
    if(score>alpha)
        alpha=score;
    
    MoveList list={0};
    generateCaptures(board,&list);
    for(int i=0;i<list.nextFree;i++){
        doMove(board,&list.array[i]);
        score=-quis(board,-beta,-alpha);
        undoMove(board, &list.array[i]);
        
        if(score>beta)
            return beta;
        
        if(score>alpha)
            alpha=score;
    }
    
    return alpha;
    
}*/

static ChError alphaBetaRecurse(ChessBoard* board, int depth, int alpha, int beta, int* score, MoveList* list){
    if(depth<=0){
        *score=-evaluate(board);
        return ChError_OK;
    }
    int localAlpha=-100000;
    ChError hr=ChError_OK;
    
    int offset=list->nextFree;
    hr=generateMoves(board, board->colorToPlay, list);
    if(hr){
        freeMoveList(list);
        return hr;
    }
    
    if(list->nextFree==offset){
        *score=10000;
        return ChError_OK;
    }
    
    for(int moveNumber=offset;moveNumber<list->nextFree;moveNumber++){
        hr=doMove(board,&list->array[moveNumber]);
        if(hr){
            freeMoveList(list);
            return hr;
        }
        int tempScore;
        hr=alphaBetaRecurse(board, depth-1, -beta,-alpha, &tempScore,list);
      
        if(hr){
            freeMoveList(list);
            return hr;
        }
        hr=undoMove(board, &list->array[moveNumber]);
        if(hr){
            freeMoveList(list);
            return hr;
        }
        if(tempScore>localAlpha){
            if(tempScore>alpha){
                alpha=tempScore;                  
            }
            localAlpha=tempScore;
            if(alpha>=beta)
                break;
        }
    }
    list->nextFree=offset;
    *score=-localAlpha;
    return hr;
    
}

ChError doAiMove(ChessBoard* board, Properties* aiProperties){
   
    int alpha=-100000;
    int beta = 100000;
    MoveList list={0};
    Move bestMove;
    ChError hr=ChError_OK;
    
   
    
    hr=generateMoves(board, board->colorToPlay, &list);
    if(hr){
        freeMoveList(&list);
        return hr;
    }
        
    for(int depth=1;depth<=aiProperties->depth;depth++){
        for(int moveNumber=0;moveNumber<list.nextFree;moveNumber++){
            
            hr=doMove(board,&list.array[moveNumber]);
            if(hr){
                freeMoveList(&list);
                return hr;
            }
            int tempScore;

            hr=alphaBetaRecurse(board, depth-1, -beta,-alpha,&tempScore,&list);

            if(hr){
                freeMoveList(&list);
                return hr;
            }

            hr=undoMove(board, &list.array[moveNumber]);

            if(hr){
                freeMoveList(&list);
                return hr;
            }
            if(tempScore>alpha){
                bestMove= list.array[moveNumber];
                alpha=tempScore;
                printf("%dbest move score %d\n",depth,alpha);
                if(alpha==10000)
                    break;
            }
        }
         alpha=-100000;
         beta = 100000;
    }
    char charMove[6];
    moveToChar(&bestMove, charMove);
    printf("move %s\n",charMove);
    doMove(board, &bestMove);
    printBoardE(board);
    freeMoveList(&list);
    return hr;
}
