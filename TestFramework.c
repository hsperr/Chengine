//
//  TestFramework.c
//  Chengine
//
//  Created by Henning Sperr on 7/23/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "TestFramework.h"

long testPositions (ChessBoard* board)
{
    static const char input[] = "/Users/henningsperr/Desktop/Chess/chen/Chengine/ChessPositions.txt";
    
    FILE *file   = fopen ( input, "r" );
    long time=clock();
    SearchInformation info={0};
    if ( file != NULL )
    {
        char line [ 256 ];
        while ( fgets ( line, sizeof line, file ) != NULL) 
        {
             fputs ( line, stdout );
            readFENString(board, line);
            //printBoardE(board);
            
            doAiTest(board, 7,&info);
            
            
            
        }
        fclose ( file );
    }
    else
    {
        perror ( input ); /* why didn't the file open? */
    }
    
    printf("\n");
    printf("Nodes Searched: %lu (%lu)\n",info.allMovesCalculated, (info.allMovesCalculated)*100/(info.allMovesCalculated+info.globalQuietNodes));
    printf("Quiescent Nodes: %lu (%lu)\n",info.globalQuietNodes, (info.globalQuietNodes)*100/(info.allMovesCalculated+info.globalQuietNodes));
    printf("PVS failed: %d\n",info.pvsFail);
    printf("\n");
    int cutOffSum=info.cutOffs+info.hashMoveCutOffs+info.hashExactCutoffs+info.nullCutOffs+1;
    printf("Normal BetaCutOffs: %d (%d)\n",info.cutOffs,((info.cutOffs*100)+1)/cutOffSum);
    printf("Hash Exact Cutoffs: %d (%d)\n",info.hashExactCutoffs,((info.hashExactCutoffs*100)+1)/cutOffSum);
    printf("Hash Move Cutoffs: %d (%d)\n",info.hashMoveCutOffs,((info.hashMoveCutOffs*100)+1)/cutOffSum);
    printf("Null Move Cutoffs: %d (%d)\n",info.nullCutOffs,((info.nullCutOffs*100)+1)/cutOffSum);
    printf("\n");       
    printf("Total time: %f\n",(float)(clock()-time)/CLOCKS_PER_SEC);
    printf("\n");
    return info.allMovesCalculated+info.globalQuietNodes;
}

void optimizeParameters(ChessBoard* board){
    
    int* sortWeights=getSortWeights();
    int originalWeights[]={100000,100,14,18,17,15,15,20,40,90};
    long bestResult=testPositions(board);
    for(int k=0;k<10;k++)
        printf("%d ",sortWeights[k]);
    
    printf("%lu\n",bestResult);
    srand(clock());
    for(int j=0;j<100;j++){
        
        for(int w1=-100;w1<110;w1+=10){
            for(int w2=-10;w2<10;w2+=10){
                for(int w3=-10;w3<10;w3+=2){
                    for(int w4=-10;w4<10;w4+=2){
                        for(int w5=-10;w5<10;w5+=2){
                            for(int w6=-10;w6<10;w6+=2){
                                for(int w7=-10;w7<10;w7+=2){
                                    for(int w8=-20;w8<20;w8+=10){
                                        
                                        for(int w9=-20;w9<20;w9+=10){
                                            sortWeights[0]=originalWeights[0]+w1;
                                            sortWeights[1]=originalWeights[1]+w2;
                                            sortWeights[2]=originalWeights[2]+w3;
                                            sortWeights[3]=originalWeights[3]+w4;
                                            sortWeights[4]=originalWeights[4]+w5;
                                            sortWeights[5]=originalWeights[5]+w6;
                                            sortWeights[6]=originalWeights[6]+w7;
                                            sortWeights[7]=originalWeights[7]+w8;
                                            sortWeights[8]=originalWeights[8]+w9;
                                            
                                            long result=testPositions(board);
                                            if(result<bestResult){
                                                bestResult=result;
                                                for(int k=0;k<10;k++)
                                                    printf("%d ",sortWeights[k]);
                                                
                                                printf("%lu\n",bestResult);
                                            }
                                            
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
}