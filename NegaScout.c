//
//  NegaScout.c
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "NegaScout.h"

#define MATE_SCORE 300000
#define INIT_ALPHA -1000000
#define INIT_BETA  1000000
#define DRAW_SCORE 0

int centerWhiteValue[]={1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
    3,4,4,5,5,4,4,3,0,0,0,0,0,0,0,0,
    3,4,4,4,4,4,4,3,0,0,0,0,0,0,0,0,
    2,2,4,4,4,4,2,2,0,0,0,0,0,0,0,0,
    2,2,3,4,4,3,2,2,0,0,0,0,0,0,0,0,
    2,2,3,3,3,3,2,2,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};
int centerBlackValue[]={1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
    2,2,3,3,3,3,2,2,0,0,0,0,0,0,0,0,
    2,2,3,4,4,3,2,2,0,0,0,0,0,0,0,0,
    2,2,3,4,4,3,2,2,0,0,0,0,0,0,0,0,
    2,2,4,4,4,4,2,2,0,0,0,0,0,0,0,0,
    3,4,4,5,5,4,4,3,0,0,0,0,0,0,0,0,
    3,4,4,4,4,4,4,3,0,0,0,0,0,0,0,0};
/*

 {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0}
 */


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
const char usePVS=1;
const char useNullMoves=0;
const char NULL_DEPTH=4;
const char useAspiration=1;
const int ASPIRATION_SIZE=10;
const int MAX_DEPTH=16;

const int TIME_CHECK_INTERVAL=50000;
int nextTimeCheck=1000;
long startTime=0;
float timeForThisMove=0;
int stopSearch=0;


int evaluate(ChessBoard* board){
    int score=0;
    if(board->colorToPlay==WHITE)
        score=board->whitePieceScore-board->blackPieceScore;
    else
        score=board->blackPieceScore-board->whitePieceScore;
    

    
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
    int localAlpha=INIT_ALPHA;
    Move hashMove={0,0,0,0};
    ChError hr=ChError_OK;
    Move bestMove={0};
    int bounds = 2; // upper
    char usedHashMove=0;
    
    /**
     * Time control
     */ 
    nextTimeCheck--;
    if(nextTimeCheck==0){
        nextTimeCheck=TIME_CHECK_INTERVAL;
        if((float)(clock()-startTime)/CLOCKS_PER_SEC>=timeForThisMove){
            stopSearch=1;
        }
        return ChError_OK;
    }
    
    if(probeRepetitionTable(&board->zobrist)>=3){
        localAlpha=0;
        *score=localAlpha;
        return hr;
    }
   
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
                *score-=beta;
                return hr;
            }
        }
    }
    
    
    hr=generateSortedMoves(board, board->colorToPlay, stats->list,NULL);
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
    for(int moveNumber=offset;moveNumber<stats->list->nextFree&&!stopSearch;moveNumber++){
        History h={0};
        Move* move=&stats->list->array[moveNumber];
        
        if(move->promote!=0){
            if(move->promote!=queen)
                continue;
        }
        
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
                if(alpha>(MATE_SCORE+1000)){
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
    hr=generateSortedMoves(board, board->colorToPlay, &list,NULL);
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
    for(int moveNumber=0;moveNumber<list.nextFree&&!stopSearch;moveNumber++){
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
            
        }
        b=alpha+1;
    }
    addKeyToTable(board->zobrist, depth, stats->bestMoveScore,bounds,stats->bestMove);
    freeMoveList(&list);
    return ChError_OK;
}
ChError doAiMove(ChessBoard* board, Properties* aiProperties){
   
    clearTable();
    int alpha=INIT_ALPHA;
    int beta = INIT_BETA;
    ChError hr=ChError_OK;
    AiStatistics stats={0};

    timeForThisMove=(float)aiProperties->timelimit/40000;
    printf("I want to take %f sec for this move.\n", timeForThisMove);
    startTime=clock();
    stopSearch=0;
    nextTimeCheck=TIME_CHECK_INTERVAL;
    
    long time=clock();

    //iterative deepening
    for(int depth=1;depth<=7&&!stopSearch;depth++){
        hr=searchRoot(board, depth, alpha, beta, &stats);
        if(hr)
            return hr;
        
        //fell out of aspiration window, search again
        if(useAspiration){
            if(stats.bestMoveScore<=alpha||stats.bestMoveScore>=beta){
                alpha=INIT_ALPHA;
                beta=INIT_BETA;
                printf("have to search again\n");
                depth--;//search same depth again
                continue;
            }else{
                alpha=stats.bestMoveScore-ASPIRATION_SIZE;
                beta=stats.bestMoveScore+ASPIRATION_SIZE;
            }
        }else{
            alpha=INIT_ALPHA;
            beta=INIT_BETA;            
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
    
    
    aiProperties->timelimit-=(clock()-startTime)/CLOCKS_PER_SEC*1000;
    printf("move %s\n",charMove);
    doMove(board, &stats.bestMove,&h);
    printBoardE(board);
    
    
    if(probeRepetitionTable(&board->zobrist)>=3){
        hr=ChError_RepetitionDraw;
    }
    
    return hr;
}

typedef struct SerachInformation{
    ChessBoard* board;
    //statistics
    int allMovesCalculated;
    int movesPerIterationCalculated;
    int quietNodes;
    int tableLookUpsFound;
    int cutOffs;
    int globalDepth;
    
    
    MoveList* list; //global move list 
    
    
    Move* bestMoves;     //principal variation
    int* bestMoveScores;
}SearchInformation;

int quiscent(ChessBoard* board,int alpha, int beta, SearchInformation* info){
    int score=evaluate(board);
    
    MoveList* list=info->list;
    
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
    
    
    for(int i=offset;i<list->nextFree&&!stopSearch;i++){
        info->quietNodes++;
        History h={0};
        Move* move =&list->array[i];
        
        doMove(board,move,&h);
        score=-quiscent(board,-beta,-alpha,info);
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
int NegaScoutRoot(int alpha, int beta, int depth, int allowNull, SearchInformation* info){
    int myGlobOffset = info->list->nextFree;
    int pvsBeta=beta;
    int localAlpha=0;
    ChError hr=ChError_OK;
    Bound bound=BOUND_LOWER;
    //ADD TIMING HERE
    nextTimeCheck--;
    if(nextTimeCheck==0){
        nextTimeCheck=TIME_CHECK_INTERVAL;
        if((float)(clock()-startTime)/CLOCKS_PER_SEC>=timeForThisMove){
            stopSearch=1;
            return 0;
        }
    }
    //ADD REPETITION DRAW AND 50 MOVE RULE HERE
    
    if(probeRepetitionTable(&info->board->zobrist)>=3||info->board->repetitionMoves>=50){
        return 0;
    }

    if(depth<=0){
        info->movesPerIterationCalculated++;
        if(useQuiescent)
            return quiscent(info->board, alpha, beta, info);
        else
            return evaluate(info->board);
    }
    
    if(allowNull&&depth>NULL_DEPTH)
    {
        if(!isCheck(info->board,info->board->colorToPlay))
        {
            info->board->colorToPlay=info->board->colorToPlay==WHITE?BLACK:WHITE;
            switchColorZobrist(&info->board->zobrist);
            setEnPassantZobrist(&info->board->zobrist, info->board->enPassantSquare,-5);
            int temp=info->board->enPassantSquare;
            info->board->enPassantSquare=-5;
            
            int eval =0;
            eval=-NegaScoutRoot(-beta, -beta+1, depth-1-2, 0, info);
            
            info->board->colorToPlay=info->board->colorToPlay==WHITE?BLACK:WHITE;
            switchColorZobrist(&info->board->zobrist);
            setEnPassantZobrist(&info->board->zobrist, temp,-5);
            info->board->enPassantSquare=temp;
                    
            if(eval >= beta){
                bound = BOUND_UPPER; //lower
                Move bestMove={0};
                addKeyToTable(info->board->zobrist, depth, eval, bound,bestMove);
                return beta;
            }
        }
    }
    
    //PROBE HASHTABLE HERE
    
    if(useHashTables){
        Move hashMove={0};
        if((hr=probe(info->board->zobrist, depth,&alpha,&beta, &localAlpha,&hashMove))){
            //not in table
        }else{
            info->tableLookUpsFound++;//exact hit
            info->bestMoves[depth]=hashMove;
            info->bestMoveScores[depth]=localAlpha;

            return localAlpha;
        }
        //try hashmove first
        if(hashMove.from!=0||hashMove.to!=0){
            addToMoveList(info->list, &hashMove);
        }
    }
    
    hr=generateSortedMoves(info->board, info->board->colorToPlay, info->list,NULL);
    switch (hr){
        case ChError_OK:
            break;
        case ChError_CheckMate:
            //we know that we found no moves
            alpha=-MATE_SCORE-depth; //evaluate situation as bad
            break;
        case ChError_StaleMate:
            alpha=DRAW_SCORE;
            break;
        default:
            printError(hr);
            break;
    }
    Move localPV[depth+1];
    
    Move* globalPV=info->bestMoves;
    info->bestMoves=localPV;
    Move localBestMove={0};
    for(int moveNr=myGlobOffset; moveNr<info->list->nextFree&&!stopSearch;moveNr++){
        History h={0};
        Move* move = &info->list->array[moveNr];
        doMove(info->board, move, &h);
        
        
        if(usePVS){
            localAlpha=-NegaScoutRoot(-pvsBeta, -alpha, depth-1,0, info);
            if(localAlpha>alpha&&localAlpha<beta&&moveNr>myGlobOffset){
                localAlpha=-NegaScoutRoot(-beta, -alpha, depth-1,useNullMoves, info);
            }
        }else{
           localAlpha=-NegaScoutRoot(-beta, -alpha, depth-1,useNullMoves, info); 
        }
        
        undoMove(info->board, move, &h);
        
        if(alpha<localAlpha){
            bound=BOUND_EXACT;
            alpha=localAlpha;
            info->bestMoves[depth]=*move;
            info->bestMoveScores[depth]=localAlpha;
            localBestMove=*move;
            
            for(int i=1;i<=depth;i++){
                globalPV[i]=localPV[i];
            }
            
            if(alpha>=beta&&useAlphaBeta){
                info->cutOffs++;
                bound=BOUND_UPPER;
                break;
            }
        
        }
        pvsBeta=alpha+1;
        localAlpha=INIT_ALPHA;
        
    }
    
    info->bestMoves=globalPV;
    info->list->nextFree=myGlobOffset;
    addKeyToTable(info->board->zobrist, depth, alpha, bound,localBestMove);
    
    return alpha;
}


ChError doAi(ChessBoard* board){
    clearTable();
    int alpha=INIT_ALPHA;
    int beta = INIT_BETA;
    ChError hr=ChError_OK;
    SearchInformation info ={0};
    MoveList list={0};
    Move lastSerachesBestMove;
    int lastIterationScore=INIT_ALPHA;
    Move globalPV[100];
    int globalBestScores[100];
    info.board=board;
    info.list=&list;
    info.bestMoves=globalPV;
    info.bestMoveScores=globalBestScores;
    
    
    timeForThisMove=(float)3800000/40000;
    printf("I want to take %f sec for this move.\n", timeForThisMove);
    startTime=clock();
    stopSearch=0;
    nextTimeCheck=TIME_CHECK_INTERVAL;
    
    long time=clock();
    int depth =1;
    for(;!stopSearch&&depth<=MAX_DEPTH;depth++){
        info.globalDepth=depth;
        lastSerachesBestMove=globalPV[depth-1];
        int bestScore=NegaScoutRoot(alpha, beta, depth,0, &info);
        
        //fell out of aspiration window, search again
        if(useAspiration){
            if(bestScore<=alpha||bestScore>=beta){
                alpha=INIT_ALPHA;
                beta=INIT_BETA;
                printf("Aspiration Window Fail:have to search again\n");
                depth--;//search same depth again
                continue;
            }else{
                alpha=bestScore-ASPIRATION_SIZE;
                beta=bestScore+ASPIRATION_SIZE;
            }
        }else{
            alpha=INIT_ALPHA;
            beta=INIT_BETA;            
        }
    
        
        /***********
        *STATISTICS
        ************/
        char charMove[6];    
        moveToChar(&info.bestMoves[depth], charMove);
        printf("In Depth %d best move score %d with move %s out of %d nodes with %d qis\n",depth,info.bestMoveScores[depth],charMove,info.movesPerIterationCalculated,info.quietNodes);
        info.allMovesCalculated+=info.quietNodes;
        info.allMovesCalculated+=info.movesPerIterationCalculated;
        info.quietNodes=0;
        info.movesPerIterationCalculated=0; 
        
        if((float)(clock()-startTime)/CLOCKS_PER_SEC*2>timeForThisMove){
            depth++;
            break;
        }
        
    }
    History h={0};
    //print move
    char charMove[6];
    
    
    printf("Principal Variation: ");
    for(int i=depth-1;i>=1;i--){
        moveToChar(&info.bestMoves[i], charMove);
        printf("%s - ",charMove);
    }
    printf("\n");
    
    if(!stopSearch)
        moveToChar(&info.bestMoves[depth-1], charMove);
    else
        moveToChar(&lastSerachesBestMove, charMove);
    
    printf("#Found best move %s in %f sec out of %d nodes having %f nodes/sec.\n",charMove,((float)(clock()-time)/CLOCKS_PER_SEC),info.allMovesCalculated,(info.allMovesCalculated+info.quietNodes)/((float)(clock()-time)/CLOCKS_PER_SEC));
    printf("#This run i had %d tableLookups and %d cutoffs and %d quiet nodes.\n",info.tableLookUpsFound,info.cutOffs,info.quietNodes);
    
    printf("move %s\n",charMove);
    
    if(!stopSearch)
        doMove(board, &info.bestMoves[depth-1],&h);
    else
        doMove(board, &lastSerachesBestMove,&h);
    
    printBoardE(board);
    
    
    hr=generateMoves(board, board->colorToPlay, &list);
    switch (hr){
        case ChError_OK:
            break;
        case ChError_CheckMate:
            return hr;
            break;
        case ChError_StaleMate:
            return hr;
            break;
        default:
            printError(hr);
            break;
    }
    
    
    if(probeRepetitionTable(&board->zobrist)>=3){
        hr=ChError_RepetitionDraw;
    }
    
    return hr;
}