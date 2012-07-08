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


typedef struct AiStatistics{
    int allMovesCalculated;
    int movesPerIterationCalculated;
    int quietNodes;
    int tableLookUpsFound;
    int cutOffs;
    MoveList* list;
}AiStatistics;

 int evaluate(ChessBoard* board){
    int score=0;
    if(board->colorToPlay==BLACK)
        score=board->whitePieceScore-board->blackPieceScore;
    else
        score=board->blackPieceScore-board->whitePieceScore;
    
    for(int i=0;i<16;i++){
        if(board->colorToPlay==WHITE){
            score+=(119-board->whiteToSquare[i].location)/10;
            int column=board->whiteToSquare[i].location&0x0F;
            if(column>3&&column<7){
                score+=1;
            }
            score-=board->blackToSquare[i].location/10;
            column=board->blackToSquare[i].location&0x0F;
            if(column>3&&column<7){
                score-=1;
            }
        }else{
            score-=(119-board->whiteToSquare[i].location)/10;
            int column=board->whiteToSquare[i].location&0x0F;
            if(column>3&&column<7){
                score-=1;
            }
            score+=board->blackToSquare[i].location/10;
            column=board->blackToSquare[i].location&0x0F;
            if(column>3&&column<7){
                score+=1;
            }
        }
    }
    
    return score;
    
}

int quis(ChessBoard* board,int alpha, int beta, int depth, AiStatistics* stats){
    int score=evaluate(board);
    stats->quietNodes++;
    if(depth==0)
        return score;

    MoveList* list=stats->list;
    
    if(score>beta)
        return beta;
    
    
    if(score>alpha)
        alpha=score;
    
    int offset=list->nextFree;
    
    ChError hr;
    hr=generateCaptures(board, board->colorToPlay,list);
    switch(hr){
        case ChError_OK:
            break;
        case ChError_StaleMate:
            list->nextFree=offset;
            return 0;
            break;
        case ChError_CheckMate:
            list->nextFree=offset;
            return -MATE_SCORE;
            break;
        default:
            printError(hr);
            break;
    }


    for(int i=offset;i<list->nextFree;i++){
        History h={0};
        Move* move =&list->array[i];
        
        doMove(board,move,&h);
        score=-quis(board,-beta,-alpha,depth-1,stats);
        undoMove(board,move,&h);
        
        if(score>beta){
            list->nextFree=offset;
            return beta;
        }
        if(score>alpha)
            alpha=score;
    }
    
    list->nextFree=offset;
    return alpha;
    
}

static ChError alphaBetaRecurse(ChessBoard* board, int depth, int alpha, int beta, int* score, AiStatistics* stats){ 
    int offset=stats->list->nextFree;
    int localAlpha=-INIT_ALPHA;
    ChError hr=ChError_OK;
    
    if(depth<=0){
        stats->allMovesCalculated++;
        stats->movesPerIterationCalculated++;
        *score-=quis(board,alpha,beta,2,stats);
//TODO move code into evaluate function
        hr=generateMoves(board, board->colorToPlay, stats->list);
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
        stats->list->nextFree=offset;
        return hr;
    }
  
    
    if((hr=probe(board->zobrist, depth, &localAlpha))){
        //not in table
    }else{
        stats->tableLookUpsFound++;
       *score-=localAlpha;
       return hr;
    }
   
    
    hr=generateSortedMoves(board, board->colorToPlay, stats->list);
    switch(hr){
        case ChError_OK:
            break;
        case ChError_StaleMate:
            stats->list->nextFree=offset;
            localAlpha=0;
            *score-=localAlpha;
            break;
        case ChError_CheckMate:
            
            stats->list->nextFree=offset;
            localAlpha=MATE_SCORE;
            *score-=localAlpha;
            break;
        default:
            return hr;
            break;
    }

    if(board->repetitionMoves>=50){
        stats->list->nextFree=offset;
        localAlpha=0;
        *score-=localAlpha;
        return hr;
    }
    
    for(int moveNumber=offset;moveNumber<stats->list->nextFree;moveNumber++){
        History h={0};
        Move* move=&stats->list->array[moveNumber];
        hr=doMove(board,move,&h);
        if(hr){return hr; }
        
        int tempScore=0;
        hr=alphaBetaRecurse(board, depth-1, -beta,-alpha, &tempScore,stats);
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
            if(alpha>=beta){
                stats->cutOffs++;
                break;
            }
        }
    }


    stats->list->nextFree=offset;
    addKeyToTable(board->zobrist, depth, localAlpha);
    *score-=localAlpha;
    
    return hr;
    
}

ChError doAiMove(ChessBoard* board, Properties* aiProperties){
   
    int alpha=-INIT_ALPHA;
    int beta = INIT_ALPHA;
    MoveList list={0};
    Move bestMove={-5};
    bestMove.from=-5;
    ChError hr=ChError_OK;
    
    AiStatistics stats={0};
    stats.list=&list;
    
    long time=clock();

    hr=generateSortedMoves(board, board->colorToPlay, &list);
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
    
    if(board->whitePieceScore+board->blackPieceScore<=200900)
        aiProperties->depth=15;

    //iterative deepening
    for(int depth=1;depth<=aiProperties->depth;depth++){
        for(int moveNumber=0;moveNumber<list.nextFree;moveNumber++){
            History h={0};
            Move* move=&list.array[moveNumber];
            hr=doMove(board,move,&h);
            if(hr){
                freeMoveList(&list);
                return hr;
            }
            
            int tempScore=0;
            hr=alphaBetaRecurse(board, depth-1, -beta,-alpha,&tempScore,&stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                freeMoveList(&list);
                return hr;
            }

            hr=undoMove(board, move,&h);

            if(hr){
                freeMoveList(&list);
                return hr;
            }
            
            if(tempScore>alpha){
                bestMove= list.array[moveNumber];
                alpha=tempScore;
                if(alpha>(-MATE_SCORE-1000)){
                    stats.cutOffs++;
                    break;
                }
                
            }
        }
        
        char charMove[6];    
        moveToChar(&bestMove, charMove);
        printf("In Depth %d best move score %d with move %s out of %d nodes\n",depth,alpha,charMove,stats.movesPerIterationCalculated);
        stats.movesPerIterationCalculated=0;
        
        if(alpha>(-MATE_SCORE-1000))
            break;
        
         alpha=-INIT_ALPHA;
         beta =INIT_ALPHA;
    }
   History h={0};
    //print move
    char charMove[6];
    moveToChar(&bestMove, charMove);
    printf("#Found best move %s in %f sec out of %d nodes having %f nodes/sec.\n",charMove,((float)(clock()-time)/CLOCKS_PER_SEC),stats.allMovesCalculated,(stats.allMovesCalculated+stats.quietNodes)/((float)(clock()-time)/CLOCKS_PER_SEC));
    printf("#This run i had %d tableLookups and %d cutoffs and %d quiet nodes.\n",stats.tableLookUpsFound,stats.cutOffs,stats.quietNodes);
    
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
