//
//  OpeningBook.h
//  Chengine
//
//  Created by Henning Sperr on 7/14/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Chengine_OpeningBook_h
#define Chengine_OpeningBook_h


#include <stdio.h>
#include <string.h>
#include "Chengine.h"

#ifdef _MSC_VER
typedef unsigned __int64 uint64;
#else
typedef unsigned long long int uint64;
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef struct {
    uint64 key;	
    uint16 move;
    uint16 weight;
    uint32 learn;
} entry_t;


uint64 OpeningBookHash(char *fen);
int find_key(FILE *f, uint64 key, entry_t *entry);
Move openBookAndGetNextMove(char* file_name, uint64* key);


#endif
