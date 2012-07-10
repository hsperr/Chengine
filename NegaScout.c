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

int centerValue[]={1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
                 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
                 2,2,3,3,3,3,2,2,0,0,0,0,0,0,0,0,
                 2,2,3,4,4,3,2,2,0,0,0,0,0,0,0,0,
                 2,2,3,4,4,3,2,2,0,0,0,0,0,0,0,0,
                 2,2,3,3,3,3,2,2,0,0,0,0,0,0,0,0,
                 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
                 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};


typedef struct AiStatistics{
    int allMovesCalculated;
    int movesPerIterationCalculated;
    int quietNodes;
    int tableLookUpsFound;
    int cutOffs;
    MoveList* list;
    Move bestMove;
    int bestMoveScore;
}AiStatistics;


const char useAlphaBeta=1;
const char useQuiescent=1;
const char useHashTables=1;
const char usePVS=0;
const char useNullMoves=0;
const char useAspiration=1;
const int ASPIRATION_SIZE=10;

int evaluate(ChessBoard* board){
    int score=0;
    if(board->colorToPlay==BLACK)
        score=board->whitePieceScore-board->blackPieceScore;
    else
        score=board->blackPieceScore-board->whitePieceScore;
    
    for(int i=0;i<16;i++){
        if(board->colorToPlay==BLACK){
            if(board->whiteToSquare[i].location>=0)
                score-=centerValue[board->whiteToSquare[i].location];
            if(board->blackToSquare[i].location>=0)
                score+=centerValue[board->blackToSquare[i].location];
        }else{
            if(board->whiteToSquare[i].location>=0)
                score+=centerValue[board->whiteToSquare[i].location];
            if(board->blackToSquare[i].location>=0)
                score-=centerValue[board->blackToSquare[i].location];
        }
    }
    
    return score;
    
}

int quis(ChessBoard* board,int alpha, int beta, AiStatistics* stats){
    int score=evaluate(board);

    MoveList* list=stats->list;
    
    if(score>=beta)
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
        stats->quietNodes++;
        History h={0};
        Move* move =&list->array[i];
        
        doMove(board,move,&h);
        score=-quis(board,-beta,-alpha,stats);
        undoMove(board,move,&h);
        
        if(score>=beta){
            list->nextFree=offset;
            return score;
        }
        if(score>alpha)
            alpha=score;
    }
    
    list->nextFree=offset;
    return alpha;
    
}

static ChError alphaBetaRecurse(ChessBoard* board, int depth, int alpha, int beta, int* score, int allowNull, AiStatistics* stats){ 
    int offset=stats->list->nextFree;
    int localAlpha=-INIT_ALPHA;
    Move hashMove={0,0,0,0};
    ChError hr=ChError_OK;
    Move bestMove={0};
    int bounds = 2; // upper
    char usedHashMove=0;
    
    if(depth<=0){
        stats->allMovesCalculated++;
        stats->movesPerIterationCalculated++;
        if(useQuiescent)
            *score-=quis(board,alpha,beta,stats);
        else
            *score-=evaluate(board);
        
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
                localAlpha=-MATE_SCORE;
                *score-=localAlpha;
                break;
            default:
                break;
        }
        stats->list->nextFree=offset;
        return hr;
    }
    /****
     * HASH PROBING
     */
    if(useHashTables){
        if((hr=probe(board->zobrist, depth,&alpha,&beta, &localAlpha,&hashMove))){
            //not in table
        }else{
            stats->tableLookUpsFound++;
            *score-=localAlpha;
            return hr;
        }
        
        if (memcmp(&hashMove, "\0\0\0\0", 4))
        {    
            History h={0};
            hr=doMove(board,&hashMove,&h);
            if(hr){return hr; }
            
            int tempScore=0;
            hr=alphaBetaRecurse(board, depth-1, -beta,-alpha, &tempScore,useNullMoves,stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                return hr;
            }
            
            hr=undoMove(board, &hashMove,&h);
            if(hr){return hr; }
            
            if(tempScore>localAlpha){
                if(tempScore>alpha){
                    alpha=tempScore;                  
                }
                usedHashMove=1;
                localAlpha=tempScore;
                if(alpha>=beta){
                    stats->cutOffs++;
                    *score-=localAlpha;
                    return hr;
                }
            }
        }
    }
    
    if(allowNull)
    {
        if(!isCheck(board,board->colorToPlay))
        {
            board->colorToPlay=board->colorToPlay==WHITE?BLACK:WHITE;
            switchColorZobrist(&board->zobrist);
            setEnPassantZobrist(&board->zobrist, board->enPassantSquare,-5);
            int temp=board->enPassantSquare;
            board->enPassantSquare=-5;
            int eval =0;
            hr=alphaBetaRecurse(board, depth-1-2, -beta,
                                -beta+1, &eval, 0,stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw)
                return hr;
            
            board->colorToPlay=board->colorToPlay==WHITE?BLACK:WHITE;
            switchColorZobrist(&board->zobrist);
            setEnPassantZobrist(&board->zobrist, temp,-5);
            board->enPassantSquare=temp;
            if(eval >= beta){
                bounds = 1; //lower
                addKeyToTable(board->zobrist, depth, eval, bounds,bestMove);
                *score-=eval;
                return hr;
            }
        }
    }
    
    
    hr=generateSortedMoves(board, board->colorToPlay, stats->list);
    switch(hr){
        case ChError_OK:
            break;
        case ChError_StaleMate:
            stats->list->nextFree=offset;
            localAlpha=0;
            assert(stats->list->nextFree==offset);
            break;
        case ChError_CheckMate:
            assert(stats->list->nextFree==offset);
            stats->list->nextFree=offset;
            localAlpha=-MATE_SCORE;
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
    
    int b;
    if(usedHashMove){
        b=localAlpha+1;
    }else{
        b=beta;
    }
    for(int moveNumber=offset;moveNumber<stats->list->nextFree;moveNumber++){
        History h={0};
        Move* move=&stats->list->array[moveNumber];
        hr=doMove(board,move,&h);
        if(hr){return hr; }
        
        int tempScore=0;
        if(usePVS){
            hr=alphaBetaRecurse(board, depth-1, -b,-alpha,&tempScore,useNullMoves,stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                freeMoveList(stats->list);
                return hr;
            }
        }
        //PVS failed
        if((tempScore>alpha &&tempScore<beta && (moveNumber>0||usedHashMove))||!usePVS){
            tempScore=0; //research
            hr=alphaBetaRecurse(board, depth-1, -beta,-alpha,&tempScore,useNullMoves,stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                freeMoveList(stats->list);
                return hr;
            }
        }
        
        hr=undoMove(board, move,&h);
        if(hr){return hr; }
        
        if(tempScore>localAlpha){
            bounds = 0; // exact
            if(tempScore>alpha){
                alpha=tempScore;
                bestMove=*move;
            }
            localAlpha=tempScore;
            if(alpha>=beta){
                bounds = 1; //lower
                stats->cutOffs++;
                if(useAlphaBeta)
                    break;
            }
        }
        b=localAlpha+1;
    }
    
    
    stats->list->nextFree=offset;
    addKeyToTable(board->zobrist, depth, localAlpha, bounds,bestMove);
    *score-=localAlpha;
    
    return hr;
    
}

ChError searchRoot(ChessBoard* board, int depth, int alpha, int beta, AiStatistics* stats){
    ChError hr=ChError_OK;
    MoveList list={0};
    stats->list=&list;
    Move hashMove;
    char usedHashMove=0;
    
    int bounds=2;//upper
    /****
     * HASH PROBING
     */
    if(useHashTables){
        if((hr=probe(board->zobrist, depth,&alpha,&beta, &stats->bestMoveScore,&hashMove))){
            //not in table
        }else{
            //exact matches do not help us, we only want the hash move
        }
        
        if (memcmp(&hashMove, "\0\0\0\0", 4))
        {    
            History h={0};
            hr=doMove(board,&hashMove,&h);
            if(hr){return hr; }
            
            int tempScore=0;
            hr=alphaBetaRecurse(board, depth-1, -beta,-alpha, &tempScore,useNullMoves,stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                return hr;
            }
            
            hr=undoMove(board, &hashMove,&h);
            if(hr){return hr; }
            
            if(tempScore>alpha){
                stats->bestMove= hashMove;
                stats->bestMoveScore=tempScore;
                alpha=tempScore;
                bounds=0;//exact
                usedHashMove=1;
                if(alpha>(-MATE_SCORE-1000)){
                    bounds=1;//lower
                    stats->cutOffs++;
                    return hr;
                }
                
            }
        }
    }
    /****
     * Move generation
     */
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
    int b;
    if(usedHashMove)
        b=alpha+1;
    else
        b=beta;
    /****
     * Move probing
     */
    for(int moveNumber=0;moveNumber<list.nextFree;moveNumber++){
        History h={0};
        Move* move=&list.array[moveNumber];
        hr=doMove(board,move,&h);
        if(hr){
            freeMoveList(&list);
            return hr;
        }
        
        int tempScore=0;
        
        
        if(usePVS){
            hr=alphaBetaRecurse(board, depth-1, -b,-alpha,&tempScore,useNullMoves,stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                freeMoveList(&list);
                return hr;
            }
        }
        //PVS failed
        if((tempScore>alpha &&tempScore<beta && (moveNumber>0||usedHashMove))||!usePVS){
            // printf("research PVS movenumber: %d\n",moveNumber);
            tempScore=0; //research
            hr=alphaBetaRecurse(board, depth-1, -beta,-alpha,&tempScore,useNullMoves,stats);
            if(hr!=ChError_OK&&hr!=ChError_StaleMate&&hr!=ChError_CheckMate&&hr!=ChError_RepetitionDraw){
                freeMoveList(&list);
                return hr;
            }
        }
        
        hr=undoMove(board, move,&h);
        if(hr){
            freeMoveList(&list);
            return hr;
        }
        
        if(tempScore>alpha){
            stats->bestMove= list.array[moveNumber];
            stats->bestMoveScore=tempScore;
            alpha=tempScore;
            bounds=0;//exact
            if(alpha>(-MATE_SCORE-1000)){
                bounds=1;//lower
                stats->cutOffs++;
                if(useAlphaBeta)
                    break;
            }
            
        }
        b=alpha+1;
    }
    addKeyToTable(board->zobrist, depth, stats->bestMoveScore,bounds,stats->bestMove);
    freeMoveList(&list);
    return ChError_OK;
}
ChError doAiMove(ChessBoard* board, Properties* aiProperties){
   
    clearTable();
    int alpha=-INIT_ALPHA;
    int beta = INIT_ALPHA;

    ChError hr=ChError_OK;
    
    AiStatistics stats={0};

    
    long time=clock();

   

    //iterative deepening
    for(int depth=1;depth<=aiProperties->depth;depth++){
        hr=searchRoot(board, depth, alpha, beta, &stats);
        if(hr)
            return hr;
        
        //fell out of aspiration window, search again
        if(useAspiration){
            if(stats.bestMoveScore<=alpha||stats.bestMoveScore>=beta){
                alpha=-INIT_ALPHA;
                beta=INIT_ALPHA;
                printf("have to search again\n");
                depth--;//search same depth again
                continue;
            }else{
                alpha=stats.bestMoveScore-ASPIRATION_SIZE;
                beta=stats.bestMoveScore+ASPIRATION_SIZE;
            }
        }else{
            alpha=-INIT_ALPHA;
            beta=INIT_ALPHA;            
        }
        char charMove[6];    
        moveToChar(&stats.bestMove, charMove);
        printf("In Depth %d best move score %d with move %s out of %d nodes with %d qis\n",depth,stats.bestMoveScore,charMove,stats.movesPerIterationCalculated,stats.quietNodes);
        stats.allMovesCalculated+=stats.quietNodes;
        stats.quietNodes=0;
        stats.movesPerIterationCalculated=0;        
        
    }
   History h={0};
    //print move
    char charMove[6];
    moveToChar(&stats.bestMove, charMove);
    printf("#Found best move %s in %f sec out of %d nodes having %f nodes/sec.\n",charMove,((float)(clock()-time)/CLOCKS_PER_SEC),stats.allMovesCalculated,(stats.allMovesCalculated+stats.quietNodes)/((float)(clock()-time)/CLOCKS_PER_SEC));
    printf("#This run i had %d tableLookups and %d cutoffs and %d quiet nodes.\n",stats.tableLookUpsFound,stats.cutOffs,stats.quietNodes);
    
    printf("move %s\n",charMove);
    doMove(board, &stats.bestMove,&h);
    printBoardE(board);

    
    return hr;
}
