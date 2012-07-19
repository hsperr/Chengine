//
//  Eval.c
//  Chengine
//
//  Created by Henning Sperr on 7/17/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Eval.h"

//EVAL MATRICES ARE FROM CRAFTY SOURCE
int pval[2][128] = {
    { 0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
        66,  66,  66,  66,  66,  66,  66,  66,0,0,0,0,0,0,0,0,
        10,  10,  10,  30,  30,  10,  10,  10,0,0,0,0,0,0,0,0,
        6,   6,   6,  16,  16,   6,   6,   6,0,0,0,0,0,0,0,0,
        3,   3,   3,  13,  13,   3,   3,   3,0,0,0,0,0,0,0,0,   /* [mg][black][64] */
        1,   1,   1,  10,  10,   1,   1,   1,0,0,0,0,0,0,0,0,
        0,   0,   0, -12, -12,   0,   0,   0,0,0,0,0,0,0,0,0,
        0,   0,   0,   0,   0,   0,   0,   0 ,0,0,0,0,0,0,0,0},
        
        { 0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
            0,   0,   0, -12, -12,   0,   0,   0,0,0,0,0,0,0,0,0,
            1,   1,   1,  10,  10,   1,   1,   1,0,0,0,0,0,0,0,0,
            3,   3,   3,  13,  13,   3,   3,   3,0,0,0,0,0,0,0,0,
            6,   6,   6,  16,  16,   6,   6,   6,0,0,0,0,0,0,0,0,   /* [mg][white][64] */
            10,  10,  10,  30,  30,  10,  10,  10,0,0,0,0,0,0,0,0,
            66,  66,  66,  66,  66,  66,  66,  66,0,0,0,0,0,0,0,0,
            0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0 }
};
int qval[2][128] = {
    { 0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
        0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
        0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
        0,   4,   6,   8,   8,   6,   4,   0,0,0,0,0,0,0,0,0,
        0,   4,   6,   8,   8,   6,   4,   0,0,0,0,0,0,0,0,0,   /* [mg][black][64] */
        0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
        0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
        0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0 },
        
        { 0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
            0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
            0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
            0,   4,   6,   8,   8,   6,   4,   0,0,0,0,0,0,0,0,0,
            0,   4,   6,   8,   8,   6,   4,   0, 0,0,0,0,0,0,0,0,  /* [mg][white][64] */
            0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
            0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
            0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0},
};
int bval[2][128] = {
    {  0,   0,   2,   4,   4,   2,   0,   0,0,0,0,0,0,0,0,0,
        0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
        2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
        4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,
        4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,   /* [mg][black][127] */
        2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
        0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
        -10, -10,  -8,  -6,  -6,  -8, -10, -10,0,0,0,0,0,0,0,0, },
    
    {-10, -10,  -8,  -6,  -6,  -8, -10, -10,0,0,0,0,0,0,0,0,
        0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
        2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
        4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,
        4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,   /* [mg][white][127] */
        2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
        0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
        0,   0,   2,   4,   4,   2,   0,   0,0,0,0,0,0,0,0,0, }
    
};
int nval[2][128] = {
    {-29, -19, -19,  -9,  -9, -19, -19, -29,0,0,0,0,0,0,0,0,
        1,  12,  18,  22,  22,  18,  12,   1,0,0,0,0,0,0,0,0,
        1,  14,  23,  27,  27,  23,  14,   1,0,0,0,0,0,0,0,0,
        1,  14,  23,  28,  28,  23,  14,   1,0,0,0,0,0,0,0,0,
        1,  12,  21,  24,  24,  21,  12,   1,0,0,0,0,0,0,0,0,  /* [mg][black][64] */
        1,   2,  19,  17,  17,  19,   2,   1,0,0,0,0,0,0,0,0,
        1,   2,   2,   2,   2,   2,   2,   1,0,0,0,0,0,0,0,0,
        -19, -19, -19, -19, -19, -19, -19, -19,0,0,0,0,0,0,0,0 },
    
    {-19, -19, -19, -19, -19, -19, -19, -19,0,0,0,0,0,0,0,0,
        1,   2,   2,   2,   2,   2,   2,   1,0,0,0,0,0,0,0,0,
        1,   2,  19,  17,  17,  19,   2,   1,0,0,0,0,0,0,0,0,
        1,  12,  21,  24,  24,  21,  12,   1,0,0,0,0,0,0,0,0,
        1,  14,  23,  28,  28,  23,  14,   1,0,0,0,0,0,0,0,0,  /* [mg][white][64] */
        1,  14,  23,  27,  27,  23,  14,   1,0,0,0,0,0,0,0,0,
        1,  12,  18,  22,  22,  18,  12,   1,0,0,0,0,0,0,0,0,
        -29, -19, -19,  -9,  -9, -19, -19, -29,0,0,0,0,0,0,0,0 }
    
};
int knight_outpost[2][128] = {
    { 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 1, 4, 4, 4, 4, 1, 0,0,0,0,0,0,0,0,0,
        0, 2, 6, 8, 8, 6, 2, 0,0,0,0,0,0,0,0,0,
        0, 1, 4, 4, 4, 4, 1, 0,0,0,0,0,0,0,0,0,   /* [black][64] */
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},
    
    { 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 1, 4, 4, 4, 4, 1, 0,0,0,0,0,0,0,0,0,
        0, 2, 6, 8, 8, 6, 2, 0,0,0,0,0,0,0,0,0,   /* [white][64] */
        0, 1, 4, 4, 4, 4, 1, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0}
};
int bishop_outpost[2][128] = {
    { 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        -1, 0, 0, 0, 0, 0, 0,-1,0,0,0,0,0,0,0,0,
        0, 0, 1, 1, 1, 1, 0, 0,0,0,0,0,0,0,0,0,
        0, 1, 3, 3, 3, 3, 1, 0,0,0,0,0,0,0,0,0,
        0, 3, 5, 5, 5, 5, 3, 0,0,0,0,0,0,0,0,0,   /* [black][127] */
        0, 1, 2, 2, 2, 2, 1, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0, },
    
    { 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,
        0, 1, 2, 2, 2, 2, 1, 0,0,0,0,0,0,0,0,0,
        0, 3, 5, 5, 5, 5, 3, 0,0,0,0,0,0,0,0,0,
        0, 1, 3, 3, 3, 3, 1, 0,0,0,0,0,0,0,0,0,   /* [white][127] */
        0, 0, 1, 1, 1, 1, 0, 0,0,0,0,0,0,0,0,0,
        -1, 0, 0, 0, 0, 0, 0,-1,0,0,0,0,0,0,0,0,
        0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0, }
};

int imbalance[9][9] = {
    /* n=-4  n=-3  n=-2  n=-1   n=0  n=+1  n=+2  n=+3 +n=+4 */
    {-126, -126, -126, -126, -126, -126, -126, -126,  -42 }, /* R=-4 */
    {-126, -126, -126, -126, -126, -126, -126,  -42,   42 }, /* R=-3 */
    {-126, -126, -126, -126, -126, -126,  -42,   42,   84 }, /* R=-2 */
    {-126, -126, -126, -126, -104,  -42,   42,   84,  126 }, /* R=-1 */
    {-126, -126, -126,  -88,    0,   88,  126,  126,  126 }, /*  R=0 */
    {-126,  -84,  -42,   42,  104,  126,  126,  126,  126 }, /* R=+1 */
    { -84,  -42,   42,  126,  126,  126,  126,  126,  126 }, /* R=+2 */
    { -42,   42,  126,  126,  126,  126,  126,  126,  126 }, /* R=+3 */
    {  42,  126,  126,  126,  126,  126,  126,  126,  126 }  /* R=+4 */
};


//my own arrays
int centralization[128]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
    0,1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,
    0,1,2,3,3,2,1,0,0,0,0,0,0,0,0,0,
    0,1,2,3,3,2,1,0,0,0,0,0,0,0,0,0,
    0,1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};




const int NO_LOCATION=-5;

int EvaluateBishops(ChessBoard* board,PieceInfo* bishopsArray, Color color){
    int score=0;
    
    int location=bishopsArray->location;
    score+=bval[color][location];
    
    score+=bishop_outpost[color][location];
    
    
    return score;
}

int EvaluateKnights(ChessBoard* board,PieceInfo* knightsArray, Color color){
    int score=0;
    
    int location=knightsArray->location;
    score+=nval[color][location];
    
    score+=knight_outpost[color][location];
    
    
    return score;
}

int EvaluateComplex(ChessBoard* board){
    int whitePawnCount=0;
    int whiteMajors=0;
    int blackPawnCount=0;
    int blackMajors=0;
    PieceInfo whitePawns[8]={0};
    PieceInfo blackPawns[8]={0};
    
    
    
    int wScore=0;
    int bScore=0;
    
    if(board->whitePieceScore!=getPieceScore(king)&&board->blackPieceScore!=getPieceScore(king)){
        for(int i=0;i<16;i++){
            if(board->whiteToSquare[i].location!=NO_LOCATION){
                if(board->whiteToSquare[i].piece==pawn){
                    whitePawns[whitePawnCount]=board->whiteToSquare[i];
                    wScore+=pval[WHITE][board->whiteToSquare[i].location];
                    whitePawnCount++;
                }else{
                    whiteMajors++;   
                    if(board->whiteToSquare[i].piece==bishop){
                        wScore+=EvaluateBishops(board, &board->whiteToSquare[i], WHITE);
                    }
                    else if(board->whiteToSquare[i].piece==knight){
                        wScore+=EvaluateKnights(board, &board->whiteToSquare[i], WHITE);
                    }
                    else if(board->whiteToSquare[i].piece==queen){
                        wScore+=qval[WHITE][board->whiteToSquare[i].location];
                    }else if(board->whiteToSquare[i].piece==rook){
                        int pawnFound=0;
                        for(int k=0;k<8;k++){
                            int index=(board->whiteToSquare[i].location&0x0F)+k*0x10;
                            if(board->tiles[index]&&board->tiles[index]->piece==pawn){
                                pawnFound++;
                                break;
                            }
                        }
                        if(pawnFound==0)
                            wScore+=35;
                    }

                }
                
            }
            if(board->blackToSquare[i].location!=NO_LOCATION){
                if(board->blackToSquare[i].piece==pawn){
                    blackPawns[blackPawnCount]=board->blackToSquare[i];
                    bScore+=pval[BLACK][board->blackToSquare[i].location];
                    blackPawnCount++;
                }else{
                    blackMajors++;
                    if(board->blackToSquare[i].piece==bishop){
                        bScore+=EvaluateBishops(board, &board->blackToSquare[i], BLACK);
                    }else if(board->blackToSquare[i].piece==knight){
                        bScore+=EvaluateKnights(board, &board->blackToSquare[i], BLACK);
                    }
                    else if(board->blackToSquare[i].piece==queen){
                        bScore+=qval[BLACK][board->blackToSquare[i].location];
                    }else if(board->whiteToSquare[i].piece==rook){
                        int pawnFound=0;
                        for(int k=0;k<8;k++){
                            int index=(board->blackToSquare[i].location&0x0F)+k*0x10;
                            if(board->tiles[index]&&board->tiles[index]->piece==pawn){
                                pawnFound++;
                                break;
                            }
                        }
                        if(pawnFound==0)
                            bScore+=35;
                    }
                }
                
            }
            
        }
        //CASTLE AND MOBILITY
        wScore+=20*board->hasCastled&0x1;
        bScore+=20*(board->hasCastled&0x2>>1);
        
        int attackMap[128]={0};
        int battackMap[128]={0};
        generateAttackMap(board, WHITE, attackMap);
        generateAttackMap(board, BLACK, battackMap);
        for(int i=0;i<128;i++){
            wScore+=attackMap[i];
            bScore+=battackMap[i];
        }
        
        //Imbalance
        int major = 4+whiteMajors-blackMajors;
        int minor = 4+whitePawnCount-blackPawnCount;
        major=max(min(major,8),0);
        minor=max(min(minor,8),0);
        
        wScore+=imbalance[major][minor];
        bScore-=imbalance[major][minor];
        
        wScore+=board->whitePieceScore;
        bScore+=board->blackPieceScore;
        
        if(board->colorToPlay==WHITE){
            wScore+=5;
            return wScore-bScore;
        }else{
            bScore+=5;
            return bScore-wScore;
        }
        
        
        
    }else{
        return 0;
    }
    
}

