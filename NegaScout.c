//
//  NegaScout.c
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "NegaScout.h"

#define MATE_SCORE -300000
#define INIT_ALPHA 1000000

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
    int offset=list->nextFree;
    int localAlpha=-INIT_ALPHA;
    ChError hr=ChError_OK;
    
    if(depth<=0){
        *score-=evaluate(board);
        hr=generateMoves(board, board->colorToPlay, list);
        switch(hr){
            case ChError_OK:
                break;
            case ChError_StaleMate:
                localAlpha=0;
                *score=localAlpha;
                break;
            case ChError_CheckMate:
                localAlpha=MATE_SCORE;
                *score-=localAlpha;
                break;
            default:
                break;
        }
        list->nextFree=offset;
        return hr;
    }
  
    
    if((hr=probe(board->zobrist, depth, &localAlpha))){
        //not in table
    }else{
       *score-=localAlpha;
       return hr;
    }
   
    
    hr=generateMoves(board, board->colorToPlay, list);
    switch(hr){
        case ChError_OK:
            break;
        case ChError_StaleMate:
            list->nextFree=offset;
            localAlpha=0;
            *score-=localAlpha;
            break;
        case ChError_CheckMate:
            
            list->nextFree=offset;
            localAlpha=MATE_SCORE;
            *score-=localAlpha;
            break;
        default:
            return hr;
            break;
    }

    if(board->repetitionMoves>=50){
        list->nextFree=offset;
        localAlpha=0;
        *score-=localAlpha;
        return hr;
    }
    
    for(int moveNumber=offset;moveNumber<list->nextFree;moveNumber++){
        History h={0};
        Move* move=&list->array[moveNumber];
        hr=doMove(board,move,&h);
        if(hr){return hr; }
        
        int tempScore=0;
        hr=alphaBetaRecurse(board, depth-1, -beta,-alpha, &tempScore,list);
        if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
            return hr;
        }

        hr=undoMove(board, move,&h);
        if(hr){return hr; }
        
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
    addKeyToTable(board->zobrist, depth, localAlpha);
    *score-=localAlpha;
    
    return hr;
    
}

ChError doAiMove(ChessBoard* board, Properties* aiProperties){
   
    int alpha=-INIT_ALPHA;
    int beta = INIT_ALPHA;
    MoveList list={0};
    Move bestMove;
    bestMove.from=-5;
    ChError hr=ChError_OK;

    hr=generateMoves(board, board->colorToPlay, &list);
    //if checkmate or stalemate return
    switch(hr){
        case ChError_OK:
            break;
        case ChError_StaleMate:
        case ChError_CheckMate:
        default:
            freeMoveList(&list);
            return hr;
            break;
    }

    //iterative deepening
    for(int depth=1;depth<=aiProperties->depth;depth++){
        
        for(int moveNumber=0;moveNumber<list.nextFree;moveNumber++){
            History h={0};
            hr=doMove(board,&list.array[moveNumber],&h);
            if(hr){
                freeMoveList(&list);
                return hr;
            }
            
            int tempScore=0;
            hr=alphaBetaRecurse(board, depth-1, -beta,-alpha,&tempScore,&list);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                freeMoveList(&list);
                return hr;
            }

            hr=undoMove(board, &list.array[moveNumber],&h);

            if(hr){
                freeMoveList(&list);
                return hr;
            }
            if(tempScore>alpha){
                bestMove= list.array[moveNumber];
                alpha=tempScore;
                char charMove[6];    
                moveToChar(&bestMove, charMove);
                printf("%dbest move score %d with move %s\n",depth,alpha,charMove);
                if(alpha>(-MATE_SCORE-1000))
                    break;
                
            }
        }
        if(alpha>(-MATE_SCORE-1000))
            break;
         alpha=-INIT_ALPHA;
         beta = INIT_ALPHA;
    }
   History h={0};
    //print move
    char charMove[6];
    moveToChar(&bestMove, charMove);
    printf("move %s\n",charMove);
    doMove(board, &bestMove,&h);
    printBoardE(board);
    
    //Check detection for user
    hr=generateMoves(board, board->colorToPlay, &list);
    //if checkmate or stalemate return
    switch(hr){
        case ChError_OK:
            break;
        case ChError_StaleMate:
        case ChError_CheckMate:
        default:
            freeMoveList(&list);
            return hr;
            break;
    }
    
    
    freeMoveList(&list);
    return hr;
}
