//
//  NegaScout.c
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "NegaScout.h"

#include "assert.h"

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

int cutOff[10];

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
const char useNullMoves=1;
const char useAspiration=1;
const char useOpeningTable=0;
const char useComplexEvaluation=1;

const int ASPIRATION_SIZE=18;
const int MAX_DEPTH=100;
const char NULL_DEPTH=0;

const int TIME_CHECK_INTERVAL=40000;
int nextTimeCheck=1000;
long startTime=0;
float timeForThisMove=0;
int stopSearch=0;

static int quiscent(ChessBoard* board,int alpha, int beta, SearchInformation* info){
    info->currentDepth++;
    int score=0;    
    
    
    if(useComplexEvaluation)
        score=EvaluateComplex(board);
    else
        score=evaluate(board);
    
    MoveList* list=info->list;
    
    if(score>=beta)
        return beta;
    
    if(score>alpha)
        alpha=score;
    
    if(score<alpha-900)
        return alpha;
    
    int offset=list->nextFree;
    
    ChError hr;
    hr=generateCaptures(board, board->colorToPlay,list, info);
    switch(hr){
        case ChError_OK:
            break;
        case ChError_StaleMate:
            list->nextFree=offset;
            info->currentDepth--;
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
        
        if(board->tiles[move->to]&&(alpha>score+getPieceScore(board->tiles[move->to]->piece)-getPieceScore(board->tiles[move->from]->piece)+900)){
            list->nextFree=offset;
            info->currentDepth--;
            return alpha;
        }
       
        
        doMove(board,move,&h);
        score=-quiscent(board,-beta,-alpha,info);
        undoMove(board,move,&h);
        
        if(score>=beta){
            list->nextFree=offset;
            info->currentDepth--;
            return score;
        }
        if(score>alpha)
            alpha=score;
    }
    
    list->nextFree=offset;
    info->currentDepth--;
    return alpha;
    
}
static int NegaScoutRoot(int alpha, int beta, int depth, int allowNull, SearchInformation* info){
    int myGlobOffset = info->list->nextFree;
    info->currentDepth=depth;
    int pvsBeta=beta;
    int localAlpha=0;
    ChError hr=ChError_OK;
    Bound bound=HASH_BETA;
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
            else{
                if(useComplexEvaluation)
                    return EvaluateComplex(info->board);
                else
                    return evaluate(info->board);
            }
    }
    
    if(allowNull&& //no two nullmoves in a row
       beta-alpha<=1&& //not a pv search
       depth>NULL_DEPTH)
    {
        if(!isCheck(info->board,info->board->colorToPlay))
        {
            info->board->colorToPlay=info->board->colorToPlay==WHITE?BLACK:WHITE;
            switchColorZobrist(&info->board->zobrist);
            setEnPassantZobrist(&info->board->zobrist, info->board->enPassantSquare,-5);
            int temp=info->board->enPassantSquare;
            info->board->enPassantSquare=-5;
            
            int eval = 0;
            eval=-NegaScoutRoot(-beta, -beta+1, depth-1-2, 0, info);
            info->currentDepth=depth;
            info->board->colorToPlay=info->board->colorToPlay==WHITE?BLACK:WHITE;
            switchColorZobrist(&info->board->zobrist);
            setEnPassantZobrist(&info->board->zobrist, temp,-5);
            info->board->enPassantSquare=temp;
            
            if(eval >= beta){
                // bound = BOUND_UPPER; //lower
                // Move bestMove={0};
                // addKeyToTable(info->board->zobrist, depth, eval, bound,bestMove);
                // depth-=4;
                // if(depth<=0)
                //return quiscent(info->board, alpha, beta, info);
                return eval;
            }
        }
    }
    
    //PROBE HASHTABLE HERE
    Move hashMove={0};
    if(useHashTables){
        
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
    
    Move localPV[200]={0};
    Move* globalPV=info->bestMoves;
    info->bestMoves=localPV;
    
    hr=generateSortedMoves(info->board, info->board->colorToPlay, info->list,info);
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
    if(hashMove.from!=0||hashMove.to!=0){
        assert(equalMoves(&info->list->array[myGlobOffset],&hashMove));
    }
    Move localBestMove={0};
    for(int moveNr=myGlobOffset; moveNr<info->list->nextFree&&!stopSearch;moveNr++){
        History h={0};
        Move* move = &info->list->array[moveNr];
        /*  if((hashMove.from!=0||hashMove.to!=0)&&moveNr>myGlobOffset&&equalMoves(move, &hashMove)){
         continue;
         }*/
        doMove(info->board, move, &h);
        
        
        if(usePVS){
            localAlpha=-NegaScoutRoot(-pvsBeta, -alpha, depth-1,useNullMoves, info);
            if(localAlpha>alpha&&localAlpha<beta&&moveNr>myGlobOffset){
                localAlpha=-NegaScoutRoot(-beta, -alpha, depth-1,useNullMoves, info);
            }
        }else{
            localAlpha=-NegaScoutRoot(-beta, -alpha, depth-1,useNullMoves, info); 
        }
        
        undoMove(info->board, move, &h);
        info->currentDepth=depth;
        if(alpha<localAlpha){
            bound=HASH_EXACT;
            alpha=localAlpha;
            info->bestMoves[depth]=*move;
            info->bestMoveScores[depth]=localAlpha;
            localBestMove=*move;
            
            for(int i=1;i<=depth;i++){
                globalPV[i]=localPV[i];
            }
            
            if(alpha>=beta&&useAlphaBeta||alpha>=MATE_SCORE){
                info->cutOffs++;
                bound=HASH_ALPHA;
                
                if(!info->board->tiles[move->to]){
                    info->history[move->from][move->to]+=depth*depth;
                    //noncapture add killer
                    if(info->killerMoves[depth][0].from==0
                       &&info->killerMoves[depth][0].to==0){
                        info->killerMoves[depth][0]=*move;
                    }else{
                        if(!equalMoves(&info->killerMoves[depth][0], move)){
                            info->killerMoves[depth][1]=info->killerMoves[depth][0];
                            info->killerMoves[depth][0]=*move;
                        }
                    }
                }
                break;
            }
            
        }
        pvsBeta=alpha+1;
        localAlpha=INIT_ALPHA;
        
    }
    
    info->bestMoves=globalPV;
    info->list->nextFree=myGlobOffset;
    addKeyToTable(info->board->zobrist, depth, alpha, bound,localBestMove);
    if(info->isProlonged)
        info->isProlonged=0;
    return alpha;
}


ChError doAi(ChessBoard* board, Properties* player){
    History h={0};
    //print move
    char charMove[6];
    
    if(player->useOpeningTable&&useOpeningTable){
        char fenString[70];
        getFenString(board, fenString);
        uint64 openingHash=OpeningBookHash(fenString);
        Move openingMove;
        openingMove=openBookAndGetNextMove("/Users/henningsperr/Desktop/Chess/Chengine88/Chengine/Chengine/Book.bin", &openingHash);
        if(openingMove.from!=openingMove.to){
            //valid move
            //move type
            if(board->tiles[openingMove.from]->piece==king){
                if(openingMove.from-openingMove.to==0x02){
                    openingMove.moveType=board->colorToPlay==WHITE?WQUEENCASTLE:BQUEENCASTLE;
                }
                if(openingMove.from-openingMove.to==-0x02){
                    openingMove.moveType=board->colorToPlay==WHITE?WKINGCASTLE:BKINGCASTLE;
                }
                
                
            }else if(board->tiles[openingMove.from]->piece==pawn){
                if(openingMove.promote!=0)
                    openingMove.moveType=PROMOTION;
                else if(openingMove.from-openingMove.to==0x20||
                        openingMove.from-openingMove.to==-0x20){
                    openingMove.moveType=PAWNDOUBLE;
                }else if(openingMove.to==board->enPassantSquare){
                    openingMove.moveType=ENPASSANT;
                }
            }    
            moveToChar(&openingMove, charMove);
            printf("move %s\n",charMove);
            
            doMove(board, &openingMove,&h);
            printBoardE(board);
            return ChError_OK;
        }else{
            player->useOpeningTable=0;
        }
        
    }
    //NO OPENINGTABLE MOVE FOUND DO NORMAL SEARCH
    clearHashTable();
    int alpha=INIT_ALPHA;
    int beta = INIT_BETA;
    ChError hr=ChError_OK;
    SearchInformation info ={0};
    MoveList list={0};
    Move lastSerachesBestMove={0};
    Move globalPV[100];
    memcpy(&globalPV, "\0", sizeof(Move)*100);
    int globalBestScores[100];
    info.board=board;
    info.list=&list;
    info.bestMoves=globalPV;
    info.bestMoveScores=globalBestScores;
    
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
    
    
    timeForThisMove=max(0.1,(float)player->timelimit/(30*100-(min(10,(board->playedMoves.nextFree/2))*100)));
    printf("#I want to take %f sec for this move.\n", timeForThisMove);
    startTime=clock();
    stopSearch=0;
    nextTimeCheck=TIME_CHECK_INTERVAL;
    
    long time=clock();
    int depth =1;
    for(;!stopSearch&&depth<=player->depth&&depth<MAX_DEPTH;depth++){
        info.globalDepth=depth;
        lastSerachesBestMove=globalPV[depth-1];
        int bestScore=NegaScoutRoot(alpha, beta, depth,0, &info);
        
        
        //fell out of aspiration window, search again
        if(useAspiration){
            if(bestScore<=alpha||bestScore>=beta){
                alpha=INIT_ALPHA;
                beta=INIT_BETA;
                printf("#Aspiration Window Fail:have to search again\n");
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
        
        /*for(int a=0;a<10;a++){
         printf("%d ",cutOff[a]);
         }
         printf("\n");*/
        
        /***********
         *STATISTICS
         ************/
        char charMove[6];    
        
        //ply score time nodes pv
        printf("%d %d %f %d ",depth,info.bestMoveScores[depth],(float)(clock()-startTime)/CLOCKS_PER_SEC,info.movesPerIterationCalculated+info.quietNodes);
        for(int i=depth;i>=max(1,depth-20);i--){
            moveToChar(&info.bestMoves[i], charMove);
            printf("%s ",charMove);
        }
        printf("\n");
        moveToChar(&info.bestMoves[depth], charMove);
        // printf("#In Depth %d best move score %d with move %s out of %d nodes with %d qis\n",depth,info.bestMoveScores[depth],charMove,info.movesPerIterationCalculated,info.quietNodes);
        
        
        info.allMovesCalculated+=info.quietNodes;
        info.allMovesCalculated+=info.movesPerIterationCalculated;
        info.quietNodes=0;
        info.movesPerIterationCalculated=0; 
        
        if((float)(clock()-startTime)/CLOCKS_PER_SEC*2>timeForThisMove ){
            depth++;
            break;
        }
        
    }
    
    
    long rep=probeRepetitionTable(&board->zobrist);
    if((rep>=3||board->repetitionMoves>=50)&&info.bestMoveScores<0){
        hr=ChError_RepetitionDraw;
        return hr;
    }
    
    if(!stopSearch){
        moveToChar(&info.bestMoves[depth-1], charMove);
#ifdef DEBUG
        assert(isLegal(board,&info.bestMoves[depth-1]));
#endif
    }else{
        moveToChar(&lastSerachesBestMove, charMove);
#ifdef DEBUG
        assert(isLegal(board,&lastSerachesBestMove ));
#endif
    }
    
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
    
    return hr;
}