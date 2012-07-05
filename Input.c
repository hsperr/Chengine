//
//  Input.c
//  Chengine
//
//  Created by Henning Sperr on 7/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Input.h"

void checkForInput(Game* game){
    char input[100];
    while(1){
        if(fgets(input,100,stdin)){
            printf("#XBoardSentCommand %s",input);
            if(strncmp(input,"xboard",6)==0){
                //output newline
                printf("\n");
                
            }else if(strncmp(input,"accepted",8)==0){
                //ignore
            }else if(strncmp(input,"rejected",8)==0){
                printf("Error accepting %s please fix that\n",input);
                exit(-1);
            }else if(strncmp(input,"protover",8)==0){
                printf("feature playother=1\n");
                printf("feature sigterm=0\n");
                printf("feature sigint=0\n");
                printf("feature analyze=0\n");
                printf("feature colors=0\n");
                printf("feature myname=\"Chengine\"\n");
                printf("feature reuse=1\n");
                printf("feature ping=1\n");
                printf("feature san=0\n");
                printf("feature debug=1\n");
                printf("feature time=0\n");
                printf("feature variants=\"normal\"\n");
                printf("feature done=1\n");
                //comes immidiately after xboard command if version 2 or above
                //else new command might follow from old version
                //set xboard version to N from "protover N" string
                //reply with feature string answers are "accepted" "rejected"
            }else if(strncmp(input,"fen",3)==0){
                readFENString(&game->board, &input[4]);
                //reset board white to move engine is black, reset clocks
                //do not ponder even if pondering is on
            }else if(strncmp(input,"new",3)==0){
                resetBoard(&game->board);
                //reset board white to move engine is black, reset clocks
                //do not ponder even if pondering is on
            }else if(strncmp(input,"variant",6)==0){
                
            }else if(strncmp(input,"quit",4)==0){
                //exit chess engine
                game->isRunning=0;
                return;
            }else if(strncmp(input,"random",6)==0){
                //can use it to add small random to eval function to vary play
            }else if(strncmp(input,"force",5)==0){
                //engine plays neither color, stop clocks, 
                //engine just checks that moves coming are legal and proper color is moving
                game->aiPlayer[BLACK].isAi=0;
                game->aiPlayer[WHITE].isAi=0;
            }else if(strncmp(input,"go",2)==0){
                game->aiPlayer[game->board.colorToPlay].isAi=1;
                return;
                //leave force mode, set engine to play color that is on move
                //start thinking and clock
            }else if(strncmp(input,"white",5)==0){
                //leave force mode and let engine play other color
                //begin pondering if enabled
                game->board.colorToPlay=WHITE;
                //state->isPlayerAi[WHITE]=false;
                //state->isPlayerAi[BLACK]=true;
            }else if(strncmp(input,"black",5)==0){
                //leave force mode and let engine play other color
                //begin pondering if enabled
                game->board.colorToPlay=BLACK;
                //state->isPlayerAi[WHITE]=true;
                //state->isPlayerAi[BLACK]=false;
            }else if(strncmp(input,"playother",9)==0){
                //leave force mode and let engine play other color
                //begin pondering if enabled
                game->aiPlayer[game->board.colorToPlay==WHITE?BLACK:WHITE].isAi=1;
            }else if(strncmp(input,"level",5)==0){
                // set time controls level MPS BASE INC
            }else if(strncmp(input,"st",2)==0){
                // set time controls st TIME
                game->aiPlayer[WHITE].timelimit=(int)(input[3]-'0');
                game->aiPlayer[BLACK].timelimit=(int)(input[3]-'0');
            }else if(strncmp(input,"sd",2)==0){
                game->aiPlayer[WHITE].depth=(int)(input[3]-'0');
                game->aiPlayer[BLACK].depth=(int)(input[3]-'0');
            }else if(strncmp(input,"nps",3)==0){
                /*The engine should not use wall-clock time to make its timing decisions, but an own internal time measure based on the number of nodes it has searched (and will report as "thinking output", see section 10), converted to seconds through dividing by the given NODE_RATE. Example: after receiving the commands "st 8" and "nps 10000", the engine should never use more that 80,000 nodes in the search for any move. In this mode, the engine should report user CPU time used (in its thinking output), rather than wall-clock time. This even holds if NODE_RATE is given as 0, but in that case it should also use the user CPU time for its timing decisions. The effect of an "nps" command should persist until the next "new" command.*/
                printf("Error (unkown command): nps\n");
            }else if(strncmp(input,"time",4)==0){
                // set a clock that belongs to the engine, even if color changes in 1/1000 sec
            }else if(strncmp(input,"otim",2)==0){
                // set clock that belongs to the opponent, even is color changes
            }else if(strncmp(input,"usermove",8)==0){
                // xboard version2 feature can enable form
                // usermove MOVE
                //e2e4  <-- normal move
                //e7e8q <--- pawn promotion
                //e1g1, e1c1, e8g8, e8c8 <-- castling
            }else if(strncmp(input,"?",1)==0){
                // force move if thinking, else ignore
            }else if(strncmp(input,"ping",4)==0){
                // reply to ping N with pong N only when all other commands finished
                printf("pong %d",(int)(input[5]-'0'));
            }else if(strncmp(input,"draw",4)==0){
                // opponent offers a draw
                //accept with "offer draw"
            }else if(strncmp(input,"edit",4)==0){
                //xboard wants to edit board
                //will send edit (enter edit mode) then c,pa4,+ or . meaning change color initially white, pawn a4 current color, # clear board, .leave edit mode then something like 
            }else if(strncmp(input,"hint",4)==0){
                //user wants a hint"
            }else if(strncmp(input,"undo",4)==0){
                // undo move, engine switches color
                printf("Unknown command\n");
                
            }else if(strncmp(input,"remove",6)==0){
                // 2x undo
            }else if(strncmp(input,"hard",4)==0){
                // enable pondering
            }else if(strncmp(input,"easy",4)==0){
                // disable pondering
            }else if(strncmp(input,"post",4)==0){
                // turn on thinking output
            }else if(strncmp(input,"nopost",6)==0){
                // turn off thinking output
            }else if(strncmp(input,"analyze",6)==0){
                // enter analyze mode
            }else if(strncmp(input,"print",5)==0){
                printBoardE(&game->board);
            }else if(strncmp(input,"name",4)==0){
                // name X inform engine of opponent name
            }else if(strncmp(input,"divide",6)==0){
                divide(&game->board, (int)(input[7]-'0'));
            }else{
                //try if is a move input e.g.
                //a2a3
                if(input[0]>='a'&&input[0]<='h'){
                    if(input[1]>='1'&&input[1]<='8'){
                        if(input[2]>='a'&&input[2]<='h'){
                            if(input[3]>='1'&&input[3]<='8'){
                                Move move={-5};
                                History history={0};
                                move.from=(int)(input[0]-'a')+0x10*(7-(int)(input[1]-'1'));
                                move.to=(int)(input[2]-'a')+0x10*(7-(int)(input[3]-'1'));
                                if(input[4]=='q')
                                    move.promote=queen;
                                else if(input[4]=='r')
                                    move.promote=knight;
                                else if(input[4]=='b')
                                    move.promote=bishop;
                                else if(input[4]=='n')
                                    move.promote=knight;
                                
                                if(isLegal(&game->board,&move)){
                                    doMove(&game->board, &move,&history);
                                    printBoardE(&game->board);
                                    return;
                                }else{
                                    printf("Illegal Move\n");
                                }
                            }   
                        }   
                    } 
                }
            }
            
            for(int i=0;i<100;i++)
                input[i]=' ';
        }
    }
}