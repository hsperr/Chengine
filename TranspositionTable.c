//
//  TranspositionTable.c
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void initTable(){
    srand(0);
    
    long SIDE=rand();
    long W_CASTLING_RIGHTS[4];
    long B_CASTLING_RIGHTS[4];
    
    for(int i=0;i<4;i++){
        W_CASTLING_RIGHTS[i]=rand();
        B_CASTLING_RIGHTS[i]=rand();
    }
    
    //long PIECES[6][2][]
    
    int random_integer = rand();
}