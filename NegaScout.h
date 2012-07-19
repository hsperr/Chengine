//
//  NegaScout.h
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Chengine_NegaScout_h
#define Chengine_NegaScout_h

#include "Chengine.h"
#include "Board.h"
#include "OpeningBook.h"
#include "Eval.h"

ChError doAiMove(ChessBoard* board, Properties* aiProperties);
ChError doAi(ChessBoard* board, Properties* player);
int evaluate(ChessBoard* board);

#endif
