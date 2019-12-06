/**
 * 6th Dec 
 * minNode的记录 debug
 * **/
#include<stdio.h> 
#include<math.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h> 
#define place(x, y) printf(" %d,%d", x, y), fflush(stdout)
#define DEBUG printf("DEBUG [MESSAGE]\n"), fflush(stdout)
const double Inf = 1e3;
typedef char byte;
//CLOCKS_PER_SEC
/********************/
#define SIZE (byte)8 
#define PIECE (byte)12
#define EMPTY (byte)0
#define BLACK (byte)1
#define WHITE (byte)-1
#define chessValue (byte)1
#define kingValue (byte)3
#define MAXNODE (byte)1
#define MINNODE (byte)0
#define MAXJUMP (byte)16
#define MAXMCNT (byte)20
#define max(a,b) (a > b ? a : b)
#define min(a,b) (a < b ? a : b)
/********************/
double timeOfTurn;
double currentTime;
byte Depth = 6;
byte row, column;
byte myColor, opponentColor;
//bool Black_side; //down_side fisrt
//bool White_side; //up_side
byte Command[SIZE * SIZE]; //store orders
byte myMarch, opponentMarch;
double myScore, opponentScore; //to make diff as big as possilbe
byte cntJump, thisJump, longestJump;
#define printScore printf("myScore: %.4f opponentScore: %.4f\n", myScore, opponentScore), fflush(stdout)
typedef struct{
    byte r[PIECE];
    byte c[PIECE];
    byte cntMoves;
}myCommand;
typedef struct{
    bool isKing;
    byte color;
    byte chessId;
}board;
typedef struct{
    byte r, c;
    //bool isKing;
    byte lgstJump;
    byte color;
}chess;
board Board[SIZE][SIZE];
chess Chess[PIECE << 1], moveChess[MAXMCNT], jumpChess[MAXMCNT];
myCommand moveCommand, tmpCommand, storeCommand;
/*******move*************/
byte blackDr[] = {-1, -1, 1, 1}, whiteDr[] = {1, 1, -1, -1};
byte blackDc[] = {-1, 1, -1, 1}, whiteDc[] = {1, -1, -1, 1};
byte * pDr = NULL, *pDc = NULL;
/*******move*************/
//Beta is the minimum upper bound of possible solutions  最好情况最少多少分
//Alpha is the maximum lower bound of possible solutions 最坏情况最多多少分
//a <=n <= b    a >= b stop consideration
//alpha beta tree search
inline static bool isNotInBound(byte r, byte c);
inline static void judgeKing(board * pBoard, byte r, byte color);
inline static void clearBoard(board * pBoard);
inline static void boardInitialize(void);
inline static void changeChessPos(chess * pChess, int nowR, int nowC);
inline static void backTrackChessPos(chess * pChess, int preR, int preC);
inline static void cleanLgstJump(void);
void printBoard(void);
inline static void chessInit(chess *, byte);
void dealOppoentPlace(char * s, byte len);
void dealMyPlace(void);
void clearCommand(void);
double evaluateScore/*Score evaluation*/(void);
void jumpJudge(byte color/*should be my color*/, byte r, byte c, byte cJump); //0 can't jump
double minMax(bool isMaxNode, byte depth, double alpha, double beta);
byte getPossilbeMoves(byte limitChess, byte cntChess, byte color);
void makeJump(bool isMaxNode, byte color, byte depth, byte cJump, byte r, byte c, double alpha, double beta, double * jumpBest);
void makeMove(bool isMaxNode, byte color, byte depth, byte r, byte c, double alpha, double beta, double * moveBest);
/*******function**********/
int main(){
    //freopen("test.in", "r", stdin);
    //freopen("test.out", "w", stdout);
    while(true){
        memset(Command, '\0', sizeof(Command));
        gets(Command);
        if(!strncmp(Command, "END ", 3)) 
            return 0;
        if(!strncmp(Command, "START ", 5)){
            myColor = (*(Command + 6) - '0') == 1 ? 1 : -1;
            opponentColor = -myColor;
            boardInitialize(), chessInit(&Chess[0], 0)/*, moveClear()*/;
            if(myColor == 1) myMarch = -1;  //black up
                else myMarch = 1; //white
            printf("OK\n"), fflush(stdout);
        }
        if(!strncmp(Command, "TURN ", 4)){
            Depth = 4;
            timeOfTurn = clock() / (double)CLOCKS_PER_SEC;
            int ans = minMax(true, 0, -Inf, Inf);
            printf("%d", moveCommand.cntMoves), fflush(stdout);
            for(int i = 0; i <= moveCommand.cntMoves; i++)
                place(moveCommand.r[i], moveCommand.c[i]);
            printf("\n"), fflush(stdout);
            dealMyPlace();
            pDr = NULL, pDc = NULL;
            //printf("Calculating\n"), fflush(stdout);
        }
        if(!strncmp(Command, "PLACE ", 5)){
            dealOppoentPlace(Command, strlen(Command));
        }
    }
    return 0;
}
inline static bool isNotInBound(byte r, byte c){
    return r >= SIZE || r < 0 || c >= SIZE || c < 0;
}
inline static void judgeKing(board * pBoard, byte r, byte color){
    if((color == WHITE && r == SIZE - 1) || (color == BLACK && r == 0)) 
        pBoard->isKing = true;
}
inline static void clearBoard(board * pBoard){
    pBoard->color = EMPTY, pBoard->isKing = false;
}
inline static void chessInit(chess * pChess, byte cnt){
    //id 0~11 mychess  id 12~23 opponent chess
    if(myColor == BLACK){
        for(int i = 5; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == myColor){
                    Board[i][j].chessId = cnt;
                    pChess->r = i, pChess->c = j, pChess->color = myColor;
                    pChess = &Chess[++cnt];
                }
        for(int i = 2; i >= 0; i--)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == opponentColor){
                    Board[i][j].chessId = cnt;
                    pChess->r = i, pChess->c = j, pChess->color = opponentColor;
                    pChess = &Chess[++cnt];
                }
    }
    if(myColor == WHITE){
        //id 0~11 mychess  id 12~23 opponent chess
        for(int i = 2; i >= 0; i--)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == myColor){
                    Board[i][j].chessId = cnt;
                    pChess->r = i, pChess->c = j, pChess->color = myColor;
                    pChess = &Chess[++cnt];
                }
        for(int i = 5; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == opponentColor){
                    Board[i][j].chessId = cnt;
                    pChess->r = i, pChess->c = j, pChess->color = opponentColor;
                    pChess = &Chess[++cnt];
                } 
    }    
    pChess = NULL;
}
inline static void boardInitialize(void){
    for(int i = 0; i < SIZE; i++)
        for(int j = 0; j < SIZE; j++)
            Board[i][j].color = EMPTY;
    for(int i = 1; i < SIZE; i += 2){
        Board[0][i].color = Board[2][i].color = WHITE;
        Board[6][i].color = BLACK;
    } 
    for(int i = 0; i < SIZE; i += 2){
        Board[5][i].color = Board[7][i].color = BLACK;
        Board[1][i].color = WHITE;
    }      
}
inline static void changeChessPos(chess * pChess, int nowR, int nowC){
    pChess->r = nowR, pChess->c = nowC;
}
inline static void backTrackChessPos(chess * pChess, int preR, int preC){
    pChess->r = preR, pChess->c = preC;
}
inline static void cleanLgstJump(void){
    for(int i = 0; i < PIECE; i++) Chess[i].lgstJump = 0;
}
void printBoard(void){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++)
            printf("%2d ", Board[i][j].color);
        putchar('\n');
    }  
    printf("Okay, printed\n");
}
void dealOppoentPlace(char * s, byte len){ //finished
    byte preR, preC, nowR, nowC;
    byte midR, midC;
    byte opponentId, myId;
    for(int i = 7; i < len; i += 4){
        if(i == 7) 
            preR = *(s + i + 1) - '0', preC = *(s + i + 3) - '0';
        else{
            nowR = *(s + i + 1) - '0', nowC = *(s + i + 3) - '0';
            midR = preR + nowR >> 1;
            midC = preC + nowC >> 1;
            opponentId = Board[preR][preC].chessId;
            myId = Board[midR][midC].chessId;
            /*get chess Id*/
            Board[nowR][nowC] = Board[preR][preC];
            clearBoard(&Board[preR][preC]);
            changeChessPos(&Chess[opponentId], nowR, nowC);
            if(midR != preR && midR != nowR){
                clearBoard(&Board[midR][midC]);
                //Chess[myId].color = EMPTY;
            }      
            preR = nowR, preC = nowC;
        }
    }
    judgeKing(&Board[preR][preC], preR, opponentColor);
    printBoard();
}
void dealMyPlace(void){
    byte preR, preC, nowR, nowC;
    byte midR, midC;
    byte opponentId, myId; 
    preR = moveCommand.r[0], preC = moveCommand.c[0];
    for(int i = 1; i <= moveCommand.cntMoves; i++){
        nowR = moveCommand.r[i], nowC = moveCommand.c[i];
        midR = preR + nowR >> 1;
        midC = preC + nowC >> 1;
        myId = Board[preR][preC].chessId;
        opponentId = Board[midR][midC].chessId;
        /*get chess Id*/
        Board[nowR][nowC] = Board[preR][preC];
        clearBoard(&Board[preR][preC]);
        changeChessPos(&Chess[myId], nowR, nowC);
        if(midR != preR && midR != nowR){
            clearBoard(&Board[midR][midC]);
            //Chess[opponentId].color = EMPTY;
        }      
        preR = nowR, preC = nowC;
    }
    judgeKing(&Board[preR][preC], preR, myColor);
    clearCommand();
    printBoard();
}
void clearCommand(){
    memset(&moveCommand, 0, sizeof(myCommand));
    memset(&tmpCommand, 0, sizeof(myCommand));
    memset(&storeCommand, 0, sizeof(myCommand));
}
double evaluateScore(void){/*should be much more complex, needed to be changed*/
    myScore = opponentScore = false;
    /*for(int i = 0; i < PIECE; i++){
        if(myChess[i].color != EMPTY && myChess[i].isKing) myScore += 3;
        if(opponentChess[i].color != EMPTY && opponentChess[i].isKing) opponentScore += 3;
        if(myChess[i].color != EMPTY && !myChess[i].isKing){
            if(myMarch == -1)
                myScore = myScore + SIZE - myChess[i].r;
            else myScore = myScore + myChess[i].r + 1;
        } 
        if(opponentChess[i].color != EMPTY && !opponentChess[i].isKing){
            if(opponentMarch == 1)
                opponentScore = opponentScore + opponentChess[i].r + 1;
            else opponentScore = opponentScore + SIZE - opponentChess[i].r;
        } 
    } */
    //printScore;
    return myScore - opponentScore;
}
void jumpJudge(byte color/*should be my color*/, byte r, byte c, byte cJump){
    if(cJump > thisJump) thisJump = cJump;
    for(byte i = 0; i < 4; i ++){
        byte nextR = r + *(pDr + i), nextC = c + *(pDc + i);
        byte jumpR = nextR + *(pDr + i), jumpC = nextC + *(pDc + i);
        if(isNotInBound(jumpR, jumpC)) 
            continue;
        if(Board[nextR][nextC].color == (~color + 1) && !Board[jumpR][jumpC].color){
            //紧邻棋子为敌方棋子，且敌方棋子后为空白
            board start = Board[r][c], opponent = Board[nextR][nextC];//make copy
            Board[jumpR][jumpC] = Board[r][c];//change value
            changeChessPos(&Chess[Board[r][c].chessId], jumpR, jumpC);
            clearBoard(&Board[r][c]), clearBoard(&Board[nextR][nextC]);//change value   
            /*get next jump*/
            jumpJudge(color, jumpR, jumpC, cJump + 1);
            /*get next jump*/
            Board[r][c] = start, Board[nextR][nextC] = opponent;
            backTrackChessPos(&Chess[Board[jumpR][jumpC].chessId], r, c);
            clearBoard(&Board[jumpR][jumpC]);
            /*back track*/
        }
    }
}
byte getPossilbeMoves(byte limitChess, byte cntChess, byte color){
    //id 0~11 mychess  id 12~23 opponent chess
    byte startI, endI, endJ = 2;
    if(color == myColor) startI = 0, endI = PIECE;
        else startI = PIECE, endI = PIECE << 1;
    if(color == BLACK) pDr = blackDr, pDc = blackDc;
        else pDr = whiteDr, pDc = whiteDc;
        //move pointer
    for(byte i = startI; i < endI; i++){
        byte r = Chess[i].r, c = Chess[i].c;
        if(Board[r][c].color == EMPTY) continue;
        thisJump = 0;
        /********jump part*****************/
        jumpJudge(color, r, c, 0);
        if(thisJump && thisJump >= longestJump){
            Chess[i].lgstJump = thisJump;
            longestJump = thisJump;
            jumpChess[cntJump++] = Chess[i];
            cntJump++;
            continue;
        }
        /********jump part*****************/
        /*just move in the following lines*/
        if(Board[r][c].isKing) endJ = 4;
        for(byte j = 0; j < endJ; j++){//
            byte nextR = r + *(pDr + j), nextC = c + *(pDc + j);
            if(isNotInBound(nextR, nextC)) 
                continue;
            if(Board[nextR][nextC].color == EMPTY){
                moveChess[cntChess++] = Chess[i];
                break;//棋子能走 不需继续判断;
            }   
            //canMoveChess line 64
        }
        if(cntChess >= limitChess) return cntChess;
    }
    return cntChess;
}
void makeJump(bool isMaxNode, byte color, byte depth, byte cJump, byte r, byte c, double alpha, double beta, double * jumpBest){
    for(byte i = 0; i < 4; i ++){
        byte nextR = r + *(pDr + i), nextC = c + *(pDc + i);
        byte jumpR = nextR + *(pDr + i), jumpC = nextC + *(pDc + i);
        if(isNotInBound(jumpR, jumpC)) 
            continue;
        if(Board[nextR][nextC].color == (~color + 1) && !Board[jumpR][jumpC].color){
            //紧邻棋子为敌方棋子，且敌方棋子后为空白
            board start = Board[r][c], opponent = Board[nextR][nextC];//make copy
            Board[jumpR][jumpC] = Board[r][c];//change value
            changeChessPos(&Chess[Board[r][c].chessId], jumpR, jumpC);
            clearBoard(&Board[r][c]), clearBoard(&Board[nextR][nextC]);
            if(depth == 0){
                storeCommand.cntMoves = cJump;
                storeCommand.r[cJump] = r;
                storeCommand.c[cJump] = c;
            }
            /*get next jump*/
            makeJump(isMaxNode, color, depth, cJump + 1, jumpR, jumpC, alpha, beta, jumpBest);
            /*get next jump*/
            Board[r][c] = start, Board[nextR][nextC] = opponent;
            backTrackChessPos(&Chess[Board[jumpR][jumpC].chessId], r, c);
            clearBoard(&Board[jumpR][jumpC]);
            /*back track*/
        }
    }
    if(cJump == longestJump){
        judgeKing(&Board[r][c], r, color);
        double value = minMax(isMaxNode ^ 1, depth + 1, alpha, beta);
        if(isMaxNode)
            if(*jumpBest > value){
                *jumpBest = value;
                if(depth == 0){
                    storeCommand.cntMoves = cJump;
                    storeCommand.r[cJump] = r;
                    storeCommand.c[cJump] = c;
                    tmpCommand = storeCommand;
                }
            }
        else if(*jumpBest < value) *jumpBest = value;
        Board[r][c].isKing = false;
    }
}
void makeMove(bool isMaxNode, byte color, byte depth, byte r, byte c, double alpha, double beta, double * moveBest){
    byte endJ = Board[r][c].isKing ? 4 : 2;
    for(byte i = 0; i < endJ; i++){//
        byte nextR = r + *(pDr + i), nextC = c + *(pDc + i);
        if(isNotInBound(nextR, nextC)) 
            continue;
        if(Board[nextR][nextC].color == EMPTY){
            board start = Board[r][c];
            Board[nextR][nextC] = Board[r][c];
            changeChessPos(&Chess[Board[r][c].chessId], nextR, nextC);
            judgeKing(&Board[nextR][nextC], nextR, color);
            clearBoard(&Board[r][c]);
            double value = minMax(isMaxNode ^ 1, depth + 1, alpha, beta);
            if(isMaxNode)
                if(*moveBest > value){
                    *moveBest = value;
                    if(depth == 0){
                        storeCommand.cntMoves = 1;
                        storeCommand.r[0] = r, storeCommand.c[0] = c;
                        storeCommand.r[1] = nextR, storeCommand.c[0] = nextC;
                        tmpCommand = storeCommand;
                    }
                }
            else if(*moveBest < value) *moveBest = value;
            Board[r][c] = start;
            backTrackChessPos(&Chess[Board[nextR][nextC].chessId], r, c);
            clearBoard(&Board[nextR][nextC]);    
        }   
    }
}
double minMax(bool isMaxNode, byte depth, double alpha, double beta){
    /*remember to add chessId*/
   // printf("now In %d\n", __LINE__);
    if(depth == Depth) return evaluateScore();
    byte cntChess;
    double value, bestVal;
    cleanLgstJump();
    if(isMaxNode){
        bestVal = -Inf, longestJump = 0, cntJump = 0/*related to jump chess*/;
        cntChess = getPossilbeMoves(6, 0, myColor);
        //for(int i = 0; i < cntJump; i++)
            //printf("%d %d\n", jumpChess[i].r, jumpChess[i].c);
        //for(int i = 0; i < cntChess; i++)
            //printf("%d %d\n", moveChess[i].r, moveChess[i].c);
        if(cntJump){
            for(int i = 0; i < cntJump; i++){
                if(jumpChess[i].lgstJump < longestJump) continue;
                makeJump(isMaxNode, myColor, depth, 0, jumpChess[i].r, jumpChess[i].c, alpha, beta, &value);
                bestVal = max(bestVal, value);
                if(bestVal == value && !depth){
                    moveCommand = tmpCommand; //apply moves;
                }
                alpha = max(alpha, bestVal);
                if(beta <= alpha){
                    break;
                }
            }
        }else{
            for(int i = 0; i < cntChess; i++){
                makeMove(isMaxNode, myColor, depth, moveChess[i].r, moveChess[i].c, alpha, beta, &value);
                bestVal = max(bestVal, value);
                if(bestVal == value && !depth){
                    moveCommand = tmpCommand; //apply moves;
                }
                alpha = max(alpha, bestVal);
                if(beta <= alpha){
                    break;
                }
            }
        }
        //printf("%d\n", longestJump), fflush(stdout);
        //printf("now In %d\n", __LINE__);
        //id 0~11 mychess  id 12~23 opponent chess
    }
    if(!isMaxNode){
        bestVal = -Inf, longestJump = 0, cntJump = 0/*related to jump chess*/;
        cntChess = getPossilbeMoves(5, 0, opponentColor);
        //for(int i = 0; i < cntJump; i++)
            //printf("%d %d\n", jumpChess[i].r, jumpChess[i].c);
        //for(int i = 0; i < cntChess; i++)
            //printf("%d %d\n", moveChess[i].r, moveChess[i].c);
        if(cntJump){
            for(int i = 0; i < cntJump; i++){
                if(jumpChess[i].lgstJump < longestJump) continue;
                makeJump(isMaxNode, opponentColor, depth, 0, jumpChess[i].r, jumpChess[i].c, alpha, beta, &value);
                bestVal = min(bestVal, value);
                beta = min(beta, bestVal);
                if(beta <= alpha){
                    break;
                }
            }
        }else{
            for(int i = 0; i < cntChess; i++){
                makeMove(isMaxNode, myColor, depth, moveChess[i].r, moveChess[i].c, alpha, beta, &value);
                bestVal = min(bestVal, value);
                beta = min(beta, bestVal);
                if(beta <= alpha){
                    break;
                }
            }
        }
    }
    return 0;
}












































































































































































































