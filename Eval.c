//
//  Eval.c
//  Chengine
//
//  Created by Henning Sperr on 7/17/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Eval.h"


static inline int IS_ON_BOARD(int x)   {return (x&0x88)==0;};

static  int KING_POS[2][128] =
{{
    -50, -50, -50, -50, -50, -50, -50, -50,    0,0,0,0,0,0,0,0,
    -50, -50, -50, -50, -50, -50, -50, -50,    0,0,0,0,0,0,0,0,
    -40, -50, -50, -50, -50, -50, -50, -40,    0,0,0,0,0,0,0,0,
    -30, -40, -40, -40, -40, -40, -40, -30,    0,0,0,0,0,0,0,0,
    -15, -25, -40, -40, -40, -40, -25, -15,    0,0,0,0,0,0,0,0,
    -10, -20, -20, -25, -25, -20, -20, -10,    0,0,0,0,0,0,0,0,
    10,  15,   0,   0,   0,   0,  15,  10,    0,0,0,0,0,0,0,0,
    10,  20,   0,   0,   0,   0,  20,  10,    0,0,0,0,0,0,0,0
},{
    10,  20,   0,   0,   0,   0,  20,  10,    0,0,0,0,0,0,0,0,
    10,  15,   0,   0,   0,   0,  15,  10,    0,0,0,0,0,0,0,0,
    -10, -20, -20, -25, -25, -20, -20, -10,    0,0,0,0,0,0,0,0,
    -15, -25, -40, -40, -40, -40, -25, -15,    0,0,0,0,0,0,0,0,
    -30, -40, -40, -40, -40, -40, -40, -30,    0,0,0,0,0,0,0,0,
    -40, -50, -50, -50, -50, -50, -50, -40,    0,0,0,0,0,0,0,0,
    -50, -50, -50, -50, -50, -50, -50, -50,    0,0,0,0,0,0,0,0,
    -50, -50, -50, -50, -50, -50, -50, -50,    0,0,0,0,0,0,0,0
}
};	


static int QUEEN_POS[2][128] =
{{
    0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
    0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
    0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
    0,   4,   6,   8,   8,   6,   4,   0,0,0,0,0,0,0,0,0,
    0,   4,   6,   8,   8,   6,   4,   0,0,0,0,0,0,0,0,0,   /* [mg][black][64] */
    0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
    0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0
},{
    0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
    0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
    0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
    0,   4,   6,   8,   8,   6,   4,   0,0,0,0,0,0,0,0,0,
    0,   4,   6,   8,   8,   6,   4,   0,0,0,0,0,0,0,0,0,   /* [mg][white][64] */
    0,   4,   4,   6,   6,   4,   4,   0,0,0,0,0,0,0,0,0,
    0,   0,   4,   4,   4,   4,   0,   0,0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0
}};

static int ROOK_POS[2][128] =
{{
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0
},{
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0,
    -6,   -3,   0,   3,   3,   0,   -3,   -6,    0,0,0,0,0,0,0,0
}};

static int QUEEN_POS_ENDING[128] =
{
    -24, -16, -12,  -8,  -8, -12, -16, -24,    0,0,0,0,0,0,0,0,
    -16,  -8,  -4,   0,   0,  -4,  -8, -16,    0,0,0,0,0,0,0,0,
    -12,  -4,   0,   4,   4,   0,  -4, -12,    0,0,0,0,0,0,0,0,
    -8,   0,   4,   8,   8,   4,   0,  -8,    0,0,0,0,0,0,0,0,
    -8,   0,   4,   8,   8,   4,   0,  -8,    0,0,0,0,0,0,0,0,
    -12,  -4,   0,   4,   4,   0,  -4, -12,    0,0,0,0,0,0,0,0,
    -16,  -8,  -4,   0,   0,  -4,  -8, -16,    0,0,0,0,0,0,0,0,
    -24, -16, -12,  -8,  -8, -12, -16, -24,    0,0,0,0,0,0,0,0
};

static  int BISHOP_POS[2][128] =
{{
    0,   0,   2,   4,   4,   2,   0,   0,0,0,0,0,0,0,0,0,
    0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
    2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
    4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,
    4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,   
    2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
    0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
    -10, -10,  -8,  -6,  -6,  -8, -10, -10,0,0,0,0,0,0,0,0
},{
    -10, -10,  -8,  -6,  -6,  -8, -10, -10,0,0,0,0,0,0,0,0,
    0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
    2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
    4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,
    4,   8,  10,  16,  16,  10,   8,   4,0,0,0,0,0,0,0,0,   
    2,   6,  12,  10,  10,  12,   6,   2,0,0,0,0,0,0,0,0,
    0,   8,   6,   8,   8,   6,   8,   0,0,0,0,0,0,0,0,0,
    0,   0,   2,   4,   4,   2,   0,   0,0,0,0,0,0,0,0,0
}};
static  int KNIGHT_POS_ENDING[128] =
{
    -10,  -5,  -5,  -5,  -5,  -5,  -5, -10,    0,0,0,0,0,0,0,0,
    -5,   0,   0,   0,   0,   0,   0,  -5,    0,0,0,0,0,0,0,0,
    -5,   0,   5,   5,   5,   5,   0,  -5,    0,0,0,0,0,0,0,0,
    -5,   0,   5,  10,  10,   5,   0,  -5,    0,0,0,0,0,0,0,0,
    -5,   0,   5,  10,  10,   5,   0,  -5,    0,0,0,0,0,0,0,0,
    -5,   0,   5,   5,   5,   5,   0,  -5,    0,0,0,0,0,0,0,0,
    -5,   0,   0,   0,   0,   0,   0,  -5,    0,0,0,0,0,0,0,0,
    -10,  -5,  -5,  -5,  -5,  -5,  -5, -10,    0,0,0,0,0,0,0,0
};
static int KNIGHT_POS[2][128] =//[color][index]
{{		
    -19, -8, -19, -19, -19, -19, -8, -19,0,0,0,0,0,0,0,0,
    1,   2,   2,   2,   2,   2,   2,   1,0,0,0,0,0,0,0,0,
    1,   2,  10,  17,  17,  10,   2,   1,0,0,0,0,0,0,0,0,
    1,  12,  21,  24,  24,  21,  12,   1,0,0,0,0,0,0,0,0,
    1,  14,  23,  28,  28,  23,  14,   1, 0,0,0,0,0,0,0,0, 
    1,  14,  23,  27,  27,  23,  14,   1,0,0,0,0,0,0,0,0,
    1,  12,  18,  22,  22,  18,  12,   1,0,0,0,0,0,0,0,0,
    -29, -19, -19,  -9,  -9, -19, -19, -29,0,0,0,0,0,0,0,0
},{-29, -19, -19,  -9,  -9, -19, -19, -29,   0,0,0,0,0,0,0,0,
    1,  12,  18,  22,  22,  18,  12,   1,   0,0,0,0,0,0,0,0,
    1,  14,  23,  27,  27,  23,  14,   1,   0,0,0,0,0,0,0,0,
    1,  14,  23,  28,  28,  23,  14,   1,   0,0,0,0,0,0,0,0,
    1,  12,  21,  24,  24,  21,  12,   1,   0,0,0,0,0,0,0,0,
    1,   2,  19,  17,  17,  19,   2,   1,   0,0,0,0,0,0,0,0,
    1,   2,   2,   2,   2,   2,   2,   1,   0,0,0,0,0,0,0,0,
    -19, -18, -19, -19, -19, -19, -18 -19 ,0,0,0,0,0,0,0,0
}
};

static  int BISHOP_POS_ENDING[128] =
{
    -18, -12,  -9,  -6,  -6,  -9, -12, -18,   0,0,0,0,0,0,0,0,
    -12,  -6,  -3,   0,   0,  -3,  -6, -12,   0,0,0,0,0,0,0,0,
    -9,  -3,   0,   3,   3,   0,  -3,  -9,   0,0,0,0,0,0,0,0,
    -6,   0,   3,   6,   6,   3,   0,  -6,   0,0,0,0,0,0,0,0,
    -6,   0,   3,   6,   6,   3,   0,  -6,   0,0,0,0,0,0,0,0,
    -9,  -3,   0,   3,   3,   0,  -3,  -9,   0,0,0,0,0,0,0,0,
    -12,  -6,  -3,   0,   0,  -3,  -6, -12,   0,0,0,0,0,0,0,0,
    -18, -12,  -9,  -6,  -6,  -9, -12, -18,   0,0,0,0,0,0,0,0
};
static  int KNIGHT_OUTPOST[2][128] =//[color][index]
{{0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   1,   4,   5,   5,   4,   1,   0,    0,0,0,0,0,0,0,0,
    0,   2,   6,  10,  10,   6,   2,   0,    0,0,0,0,0,0,0,0,
    0,   2,   5,  10,  10,   5,   2,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0
},{
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   2,   5,  10,  10,   5,   2,   0,    0,0,0,0,0,0,0,0,
    0,   2,   6,  10,  10,   6,   2,   0,    0,0,0,0,0,0,0,0,
    0,   1,   4,   5,   5,   4,   1,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0
}};

/* Positioning of the pawns in the endgame*/
static int PAWN_POS_ENDING[2][128] =
{{
    140,   140,   140,   140,   140,   140,   140,   140,    0,0,0,0,0,0,0,0,
    130,   130,   130,   130,   130,   130,   130,   130,    0,0,0,0,0,0,0,0,
    70,     70,    70,    70,    70,    70,    70,    70,    0,0,0,0,0,0,0,0,
    60,     60,    60,    60,    60,    60,    60,    60,    0,0,0,0,0,0,0,0,
    50,     50,    50,    50,    50,    50,    50,    50,    0,0,0,0,0,0,0,0,
    40,     40,    40,    40,    40,    40,    40,    40,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,                    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,                    0,0,0,0,0,0,0,0
},
    {
        0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
        0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
        40,   40,   40,   40,   40,   40,   40,   40,    0,0,0,0,0,0,0,0,
        50,   50,   50,   50,   50,   50,   50,   50,    0,0,0,0,0,0,0,0,
        60,   60,   60,   60,   60,   60,   60,   60,    0,0,0,0,0,0,0,0,
        70,   70,   70,   70,   70,   70,   70,   70,    0,0,0,0,0,0,0,0,
        130,   130,   130,   130,   130,   130,   130,   130,    0,0,0,0,0,0,0,0,
        140,   140,   140,   140,   140,   140,   140,   140,    0,0,0,0,0,0,0,0
    }};


static int ROOK_POS_ENDING[128] =
{
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0,
    0,   0,   0,   0,   0,   0,   0,   0,    0,0,0,0,0,0,0,0
};

static int KING_POS_ENDING[128] =
{
    -20, -15, -10, -10, -10, -10, -15, -20,    0,0,0,0,0,0,0,0,
    -15,  -5,   0,   0,   0,   0,  -5, -15,    0,0,0,0,0,0,0,0,
    -10,   0,   5,   5,   5,   5,   0, -10,    0,0,0,0,0,0,0,0,
    -10,   0,   5,  10,  10,   5,   0, -10,    0,0,0,0,0,0,0,0,
    -10,   0,   5,  10,  10,   5,   0, -10,    0,0,0,0,0,0,0,0,
    -10,   0,   5,   5,   5,   5,   0, -10,    0,0,0,0,0,0,0,0,
    -15,  -5,   0,   0,   0,   0,  -5, -15,    0,0,0,0,0,0,0,0,
    -20, -15, -10, -10, -10, -10, -15, -20,    0,0,0,0,0,0,0,0
};	
static int PAWN_POS[2][128] = //[color][index]
{
  { 0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
        66,  66,  66,  66,  66,  66,  66,  66,0,0,0,0,0,0,0,0,
        10,  10,  10,  30,  30,  10,  10,  10,0,0,0,0,0,0,0,0,
        6,   6,   6,  16,  16,   6,   6,   6,0,0,0,0,0,0,0,0,
        3,   3,   3,  13,  13,   3,   3,   3,  0,0,0,0,0,0,0,0, 
        1,   1,   1,  10,  10,   1,   1,   1,0,0,0,0,0,0,0,0,
        0,   0,   0, -12, -12,   0,   0,   0,0,0,0,0,0,0,0,0,
        0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0 }
    ,{
        
        0,   0,   0,   0,   0,   0,   0,   0,0,0,0,0,0,0,0,0,
        0,   0,   0, -12, -12,   0,   0,   0,0,0,0,0,0,0,0,0,
        1,   1,   1,  10,  10,   1,   1,   1,0,0,0,0,0,0,0,0,
        3,   3,   3,  13,  13,   3,   3,   3,0,0,0,0,0,0,0,0,
        6,   6,   6,  16,  16,   6,   6,   6,0,0,0,0,0,0,0,0,  
        10,  10,  10,  30,  30,  10,  10,  10,0,0,0,0,0,0,0,0,
        66,  66,  66,  66,  66,  66,  66,  66,0,0,0,0,0,0,0,0,
        0,   0,   0,   0,   0,   0,   0,   0 ,0,0,0,0,0,0,0,0,}

};
static int KING_ATTACK_EVAL[] =
{   0,  2,  3,  6, 12, 18, 25, 37, 50, 75,
    100,125,150,175,200,225,250,275,300,325,
    350,375,400,425,450,475,500,525,550,575, 
    600,600,600,600,600,600,600,600,600,600,
    600,600,600,600,600,600,600,600,600,600,
    600,600,600,600,600,600,600,600,600,600,
    600,600,600,600,600,600,600,600,600,600,
    600,600,600,600,600,600,600,600,600,600
};

int pawnsOnFile[2][8];


//piece is closer to king gets better scores
static int TROPISM_KNIGHT[] = {0, 30, 30, 20, 10, 0, 0, 0};
static int TROPISM_BISHOP[] = {0, 20, 20, 10, 0, 0, 0, 0};
static int TROPISM_ROOK[] =   {0, 40, 30, 20, 10, 10, 10, 10};
static int TROPISM_QUEEN[] =  {0, 60, 50, 40, 30, 20, 20, 20};
static int TROPISM_KING[] =  {0, 100, 70, 50, 0, 0, 0, 0};



const int DOUBLED_PAWN=20;
const int ISOLATED_PAWN=40;
const int PASSED_PAWN_RANK_VALUE=20;
const int BISHOP_PAIR=50;
const int ROOK_OPEN=20;
const int ROOK_SEMI_OPEN=15;
const int NO_LOCATION=-5;

int gamePhase=0;

int evaluate(ChessBoard* board){
    
    return board->playerScores[board->colorToPlay].totalScores-board->playerScores[board->colorToPlay==WHITE?BLACK:WHITE].totalScores;
    
}

int eval_trapped(ChessBoard* board, Color color){
    return 0;
}

static int eval_pawn(ChessBoard* board, int index, Color color){
    
    int score=0;
    if(gamePhase!=EndGame)
        score+=PAWN_POS[color][index];
    else
        score+=PAWN_POS_ENDING[color][index];
    
    if(pawnsOnFile[color][boardFile(index)]>1){
        score-=DOUBLED_PAWN;
    }
    
    pawnsOnFile[color][boardFile(index)]++;
    
    
    return score;
}
static int eval_knight(ChessBoard* board, int index, Color color){
    
    int score=0;
    if(gamePhase!=EndGame)
       score+=KNIGHT_POS[color][index];
    else
       score+=KNIGHT_POS_ENDING[index];
    
    int outPost=KNIGHT_OUTPOST[color][index];
    if(outPost!=0){
        int direction=color==WHITE?-1:1;
        //defendet twice
        int defendedLeft=board->tiles[index-0x11*direction]&&(board->tiles[index-0x11*direction]->color==color&&
                                                              board->tiles[index-0x11*direction]->piece==pawn);
        int defendedRight=board->tiles[index-0x0F*direction]&&(board->tiles[index-0x0F*direction]->color==color&&
                                                               board->tiles[index-0x0F*direction]->piece==pawn);
        if(defendedLeft&&defendedRight){
            score+=2*outPost;
        }else if(defendedLeft){
            score+=outPost;
        }else if(defendedRight){
            score+=outPost;
        }
    }
    
    if(color==WHITE){
        score+=TROPISM_KNIGHT[distance(board->blackToSquare[0].location, index)];
    }else{
        score+=TROPISM_KNIGHT[distance(board->whiteToSquare[0].location, index)];
    }
    return score;
}

static int eval_bishop(ChessBoard* board, int index, Color color){
    int score=0;
    if(gamePhase!=EndGame)
        score+=BISHOP_POS[color][index];
    else
        score+=BISHOP_POS_ENDING[index];
    
    if(color==WHITE){
        score+=TROPISM_BISHOP[distance(board->blackToSquare[0].location, index)];
    }else{
        score+=TROPISM_BISHOP[distance(board->whiteToSquare[0].location, index)];
    }
    
    if(board->playerScores[color].pieceCounts[bishop]>=2){
        score+=BISHOP_PAIR;
    }
    return score;
}

static int eval_rook(ChessBoard* board, int index, Color color){
    int score=0;
    if(gamePhase!=EndGame)
        score+=ROOK_POS[color][index];
    else
        score+=ROOK_POS_ENDING[index];
    
    if(color==WHITE){
        score+=TROPISM_ROOK[distance(board->blackToSquare[0].location, index)];
    }else{
        score+=TROPISM_ROOK[distance(board->whiteToSquare[0].location, index)];
    }
    
    
    //semi open file only enemy
    if(pawnsOnFile[color][boardFile(index)]==0 && pawnsOnFile[color==WHITE?BLACK:WHITE][boardFile(index)]!=0){
        score+=ROOK_SEMI_OPEN;
    }else if(pawnsOnFile[color][boardFile(index)]==0 && pawnsOnFile[color==WHITE?BLACK:WHITE][boardFile(index)]==0){
        score+=ROOK_OPEN;
    }
    
    return score;
}
static int eval_queen(ChessBoard* board, int index, Color color){
    int score=0;
    
    
    if(color==WHITE){
        score+=TROPISM_QUEEN[distance(board->blackToSquare[0].location, index)];
    }else{
        score+=TROPISM_QUEEN[distance(board->whiteToSquare[0].location, index)];
    }
    if(gamePhase!=EndGame)
    score+=QUEEN_POS[color][index];
    else
        score+=QUEEN_POS_ENDING[index];
    return score;
}
static int eval_king(ChessBoard* board, int index, Color color){
    int score=0;
    if(gamePhase!=EndGame)
        score+=KING_POS[color][index];
    else{
        score+=KING_POS_ENDING[index];
        int dist=0;
        if(color==WHITE){
            dist=distance(board->blackToSquare[0].location, index);
            score+=TROPISM_KING[dist];
        }else{
            dist=distance(board->whiteToSquare[0].location, index);
            score+=TROPISM_KING[dist];
        }
    }
    return score;
}   
static int evaluatePiece(ChessBoard* board, PieceInfo* piece, Color color){
    int score=0;
    switch (piece->piece) {
        case pawn:
            score+=eval_pawn(board, piece->location, color);
            //printf("Pawn Color: %d Location: %d score: %d\n", color, piece->location, score);
            break;
        case queen:
            score+=eval_queen(board, piece->location, color);
            //printf("queen Color: %d Location: %d score: %d\n", color, piece->location, score);
            break;
        case knight:
            score+=eval_knight(board,piece->location,color);
            //printf("knight Color: %d Location: %d score: %d\n", color, piece->location, score);
            break;
        case rook:
            score+=eval_rook(board, piece->location,color);
            //printf("rook Color: %d Location: %d score: %d\n", color, piece->location, score);
            break;
        case bishop:
            score+=eval_bishop(board, piece->location, color);
            //printf("bishop Color: %d Location: %d score: %d\n", color, piece->location, score);
            break;
        case king:
            score+=eval_king(board,piece->location,color);
            //printf("king Color: %d Location: %d score: %d\n", color, piece->location, score);
            break;
            
        default:
            break;
    }
    
    return score;
}
//counts bits set
static int NumberOfSetBits(int i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
static int kingAttacked(ChessBoard* board,PieceInfo* piece,int enemyAttackMap[], int myAttackMap[]){
    int score=0;
    int direction=piece->color==WHITE?-1:1;
    
    int attackedIndex=0;
    int attackedCount=0;
    int kingIndex=piece->location;
    
    attackedIndex=kingIndex+31*direction;
    if(IS_ON_BOARD(attackedIndex)){
        
    }
    attackedIndex=kingIndex+32*direction;
    if(IS_ON_BOARD(attackedIndex)){
        
    }
    attackedIndex=kingIndex+33*direction;
    if(IS_ON_BOARD(attackedIndex)){
        
    }
    
    
    attackedIndex=kingIndex+1*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        
    }
    attackedIndex=kingIndex-1*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        
    }
    attackedIndex=kingIndex-15*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        
    }
    attackedIndex=kingIndex-16*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        
    }
    attackedIndex=kingIndex-17*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        
    }
    //directly in front
    attackedIndex=kingIndex+15*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        //no own piece there
        if(!board->tiles[attackedIndex]||board->tiles[attackedIndex]->color!=piece->color){
            attackedCount++;
        }
    }
    attackedIndex=kingIndex+16*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        //no own piece there
        if(!board->tiles[attackedIndex]||board->tiles[attackedIndex]->color!=piece->color){
            attackedCount++;
        }
    }
    attackedIndex=kingIndex+17*direction;
    if(IS_ON_BOARD(attackedIndex)&&enemyAttackMap[attackedIndex]!=0){
        attackedCount++;
        //only king defending
        if((myAttackMap[attackedIndex]&0x3FFF)==0x00){
            attackedCount++;
        }
        //no own piece there
        if(!board->tiles[attackedIndex]||board->tiles[attackedIndex]->color!=piece->color){
            attackedCount++;
        }
    }
    
    score=KING_ATTACK_EVAL[attackedCount];
    return -score;
}
static int kingDefense(ChessBoard* board,PieceInfo* piece,int enemyAttackMap[], int myAttackMap[]){
    int score=0;
    int kingIndex=piece->location;
    int direction=piece->color==WHITE?-1:1;
    
    //pawn left of king
    if(IS_ON_BOARD(kingIndex+direction*0x11)&&
       board->tiles[kingIndex+direction*0x11]&&
       board->tiles[kingIndex+direction*0x11]->piece==pawn){
        score+=5;    
    }else if(IS_ON_BOARD(kingIndex+direction*0x21)&&
             board->tiles[kingIndex+direction*0x21]&&
             board->tiles[kingIndex+direction*0x21]->piece==pawn){
        score-=10;
    }else{
        score-=20;
    }
    //front
    if(IS_ON_BOARD(kingIndex+direction*0x10)&&
       board->tiles[kingIndex+direction*0x10]&&
       board->tiles[kingIndex+direction*0x10]->piece==pawn){
        score+=5;    
    }else if(IS_ON_BOARD(kingIndex+direction*0x20)&&
             board->tiles[kingIndex+direction*0x20]&&
             board->tiles[kingIndex+direction*0x20]->piece==pawn){
        score-=5;
    }else{
        score-=20;
    }
    if(IS_ON_BOARD(kingIndex+direction*0x0F)&&
       board->tiles[kingIndex+direction*0x0F]&&
       board->tiles[kingIndex+direction*0x0F]->piece==pawn){
        score+=5;    
    }else if(IS_ON_BOARD(kingIndex+direction*0x1F)&&
             board->tiles[kingIndex+direction*0x1F]&&
             board->tiles[kingIndex+direction*0x1F]->piece==pawn){
        score-=5;
    }else{
        score-=20;
    }
    
    //TODO: king on queenside check for fianchetto bishop
    
    return score;
}

int drawByMaterial(ChessBoard* board, Color color){
    
    if(board->playerScores[color].pieceCounts[pawn]!=0||
       board->playerScores[color].pieceCounts[rook]!=0||
       board->playerScores[color].pieceCounts[queen]!=0||
       board->playerScores[color].pieceCounts[bishop]>1||
       board->playerScores[color].pieceCounts[knight]>2){
        return 0;
    }
    return 1;
}


int EvaluateComplex(ChessBoard* board){
    if(drawByMaterial(board, BLACK)&&drawByMaterial(board, WHITE)){
        return 0;
    }
    int score=0;
    gamePhase=getGamePhase(board);
    //if probing is not an error (NOT IN TABLE)
    if(!probeEvalTable(&board->zobrist, &score)){
        return score;
    }
    
    int wscore=0;
    int bscore=0;
    
    memset(pawnsOnFile,0,2*sizeof(int)*8);
    
    wscore=eval_trapped(board,WHITE);
    bscore=eval_trapped(board,BLACK);
    
    for(int i=0;i<16;i++){
        if(board->whiteToSquare[i].location!=NO_LOCATION)
            wscore+=evaluatePiece(board, &board->whiteToSquare[i], WHITE);
        if(board->blackToSquare[i].location!=NO_LOCATION)
            bscore+=evaluatePiece(board, &board->blackToSquare[i], BLACK);
        
    }
    for(int i=0;i<8;i++){
        
        if(i==0){
            if(pawnsOnFile[BLACK][i]!=0&&pawnsOnFile[BLACK][i+1]==0)
                bscore-=ISOLATED_PAWN;
            
            if(pawnsOnFile[WHITE][i]!=0&&pawnsOnFile[WHITE][i+1]==0)
                wscore-=ISOLATED_PAWN;
            
            //free pawn worth two isolated because it can run
            if(pawnsOnFile[BLACK][i]!=0&&
               pawnsOnFile[WHITE][i]==0&&
               pawnsOnFile[WHITE][i+1]==0){
                bscore+=ISOLATED_PAWN*2;
            }
            
            //free pawn worth two isolated because it can run
            if(pawnsOnFile[WHITE][i]!=0&&
               pawnsOnFile[BLACK][i]==0&&
               pawnsOnFile[BLACK][i+1]==0){
                wscore+=ISOLATED_PAWN*2;
            }
            continue; 
        }
            
        if(i==7){
            if(pawnsOnFile[BLACK][i]!=0&&pawnsOnFile[BLACK][i-1]==0)
                bscore-=ISOLATED_PAWN;
            if(pawnsOnFile[WHITE][i]!=0&&pawnsOnFile[WHITE][i-1]==0)
                wscore-=ISOLATED_PAWN;
            
            //free pawn worth two isolated because it can run
            if(pawnsOnFile[BLACK][i]!=0&&
               pawnsOnFile[WHITE][i]==0&&
               pawnsOnFile[WHITE][i-1]==0){
                bscore+=ISOLATED_PAWN*2;
            }
            
            //free pawn worth two isolated because it can run
            if(pawnsOnFile[WHITE][i]!=0&&
               pawnsOnFile[BLACK][i]==0&&
               pawnsOnFile[BLACK][i-1]==0){
                wscore+=ISOLATED_PAWN*2;
            }
            continue;
        }
            
        if(pawnsOnFile[BLACK][i]!=0&&pawnsOnFile[BLACK][i-1]==0&&pawnsOnFile[BLACK][i+1]==0)
            bscore-=ISOLATED_PAWN;
        
        //free pawn worth two isolated because it can run
        if(pawnsOnFile[BLACK][i]!=0&&
           pawnsOnFile[WHITE][i]==0&&
           pawnsOnFile[WHITE][i+1]==0&&
           pawnsOnFile[WHITE][i-1]==0){
            bscore+=PASSED_PAWN_RANK_VALUE;
        }
        
        //free pawn worth two isolated because it can run
        if(pawnsOnFile[WHITE][i]!=0&&
           pawnsOnFile[BLACK][i]==0&&
           pawnsOnFile[BLACK][i+1]==0&&
           pawnsOnFile[BLACK][i-1]==0){
            wscore+=PASSED_PAWN_RANK_VALUE;
        }
        
        if(pawnsOnFile[WHITE][i]!=0&&pawnsOnFile[WHITE][i-1]==0&&pawnsOnFile[WHITE][i+1]==0)
            wscore-=ISOLATED_PAWN;
    }
  
    
     int wAttackMap[128];
     int bAttackMap[128];
     memset(wAttackMap,0,sizeof(int)*128);
     memset(bAttackMap,0,sizeof(int)*128);
     generateAttackMap(board, WHITE, wAttackMap);
     generateAttackMap(board, BLACK, bAttackMap);
     
     int wSafe=0;
     int wUnsafe=0;
     int bSafe=0;
     int bUnsafe=0;
     for(int i=0;i<128;i++){
     int wBitsSet=NumberOfSetBits(wAttackMap[i]);
     int bBitsSet=NumberOfSetBits(bAttackMap[i]);
     if(wBitsSet>0&&bBitsSet==0){
     //safe to go there
     wSafe++;
     }else if(wBitsSet==0&&bBitsSet>0){
     bSafe++;
     }else{
     wUnsafe++;
     bUnsafe++;
     }
     }
     wscore+=(2*wSafe+wUnsafe);
     bscore+=(2*bSafe+bUnsafe);
     
     wscore+=kingAttacked(board,&board->whiteToSquare[0],bAttackMap,wAttackMap);
     bscore+=kingAttacked(board,&board->blackToSquare[0],wAttackMap,bAttackMap);
     
     wscore+=kingDefense(board,&board->whiteToSquare[0],bAttackMap,wAttackMap);
     bscore+=kingDefense(board,&board->blackToSquare[0],wAttackMap,bAttackMap);
     
     
     //endgame pawns are more worth
     if(board->phase==EndGame){
        bscore+=board->playerScores[BLACK].pieceCounts[pawn]*30;
         wscore+=board->playerScores[WHITE].pieceCounts[pawn]*30;
     }
    
    
    
    wscore+=board->playerScores[WHITE].totalScores;
    bscore+=board->playerScores[BLACK].totalScores;
    
    
    if(board->colorToPlay==WHITE){
        int endscore=wscore-bscore;
        if(board->repetitionMoves>20)
            endscore=(120-board->repetitionMoves)*endscore/100;
        
        addToEvalTable(&board->zobrist, endscore);
        return endscore;
    }else{
        int endscore=bscore-wscore;
        if(board->repetitionMoves>20)
            endscore=(120-board->repetitionMoves)*endscore/100;
        
        addToEvalTable(&board->zobrist, endscore);
        return endscore;
    }
    return 0;    
}
int getExpectedMoveScoreChange(ChessBoard* board, Move* move){
    int score=0;
    switch(board->tiles[move->from]->piece){
        case pawn:
            score+=PAWN_POS[board->colorToPlay][move->to];
            score-=PAWN_POS[board->colorToPlay][move->from];
            break;
        case rook:
            score+=ROOK_POS[board->colorToPlay][move->to];
            score-=ROOK_POS[board->colorToPlay][move->from];

            break;
        case knight:
            score+=KNIGHT_POS[board->colorToPlay][move->to];
            score-=KNIGHT_POS[board->colorToPlay][move->from];

            break;
        case bishop:
            score+=BISHOP_POS[board->colorToPlay][move->to];
            score-=BISHOP_POS[board->colorToPlay][move->from];

            break;
        case king:
            score+=KING_POS[board->colorToPlay][move->to];
            score-=KING_POS[board->colorToPlay][move->from];

            break;
        case queen:
            score+=QUEEN_POS[board->colorToPlay][move->to];
            score-=QUEEN_POS[board->colorToPlay][move->from];

            break;
    }
    
    return score;
}
