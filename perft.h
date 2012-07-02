//
//  perft.h
//  Chengine
//
//  Created by Henning Sperr on 6/29/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Chengine_perft_h
#define Chengine_perft_h

#include "Chengine.h"
#include "Board.h"

long perft(ChessBoard* board, int depth);
long perft_hash(ChessBoard* board, int depth);
void divide(ChessBoard* board, int depth);


#endif
