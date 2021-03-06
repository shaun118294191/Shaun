#define FAST
#ifdef FAST 
    #pragma GCC optimize(2)
#endif
/**
 * 8th Dec 
 * 搜索层数过高超时的问题
 * 中期问题
*/
//#define Debug
#include<stdio.h> 
#include<math.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h> 
#define place(x, y) printf(" %d,%d", x, y), fflush(stdout)
#define DEBUG printf("DEBUG [MESSAGE]\n"), fflush(stdout)
const int Inf = 0x3f3f3f3f;
const int MAXSEC = 89000;
//CLOCKS_PER_SEC
/********************/
#define SIZE 8 
#define PIECE 12
#define EMPTY 0
#define BLACK 1
#define WHITE -1
#define CHESSVALUE 200
#define KINGVALUE 600
#define MAXNODE 1
#define MINNODE 0
#define MAXJUMP 16
#define max(a,b) (a > b ? a : b)
#define min(a,b) (a < b ? a : b)
#define HALF(x) (x >> 1)
#define QUARTER(x) (x >> 2)
/********************/
time_t timeOfTurn;
time_t currentTime;
time_t timeLimit;
time_t myTime;
int Depth;
int myColor, opponentColor;
/********step options************/
int curStep;
int myResChess, opponentResChess;
/********step options**********/
char Command[SIZE * SIZE]; //store orders
int cntJump, thisJump, longestJump; //cntJump, longestJump need to be area variable
#define printScore printf("myScore: %.4f opponentScore: %.4f\n", myScore, opponentScore), fflush(stdout)
typedef struct{
    int r[PIECE];
    int c[PIECE];
    int cntMoves;
}myCommand;
typedef struct{
    bool isKing;
    int color;
    int chessId;
}board;
typedef struct{
    int r, c;
    //bool isKing;
    int lgstJump;
    int color;
}chess;
board Board[SIZE][SIZE];
chess Chess[(PIECE << 1) + 1], moveChess[PIECE + 1], jumpChess[PIECE + 1];
myCommand deepeningCommand, moveCommand, tmpCommand, storeCommand;
/*******move*************/
int  blackDr[] = {-1, -1, 1, 1}, whiteDr[] = {1, 1, -1, -1};
int  blackDc[] = {-1, 1, -1, 1}, whiteDc[] = {1, -1, -1, 1};
/*******move*************/
//Beta is the minimum upper bound of possible solutions  最好情况最少多少分
//Alpha is the maximum lower bound of possible solutions 最坏情况最多多少分
//a <=n <= b    a >= b stop consideration
//alpha beta tree search
inline static void iterativeDeepening(int preBest);
inline static void getRestChess();
inline static void depthManager();
inline static bool isNotInBound(int  r, int  c);
inline static void judgeKing(board * pBoard, int  r, int  color);
inline static void clearBoard(board * pBoard);
inline static void boardInitialize(void);
inline static void changeChessPos(chess * pChess, int nowR, int nowC);
inline static void backTrackChessPos(chess * pChess, int preR, int preC);
inline static void cleanLgstJump(void);
inline static void printMove();
void printBoard(void);
void printBoardcolor(void);
inline static void chessInit(chess *, int);
inline static void dealOppoentPlace(char * s, int len);
inline static void dealMyPlace(void);
inline static void clearCommand(void);
inline static bool chessThreatened(int  r, int  c, int  color);
inline static bool boundaryJump(int r, int c, int color);
inline static bool canBeKing(int r, int c, int color);
inline static int evaluateScore/*Score evaluation*/(bool isMaxNode);
void jumpJudge(int  color/*should be my color*/, int  r, int  c, int  cJump); //0 can't jump
int minMax(bool isMaxNode, int  depth, int alpha, int beta);
int  getPossilbeMoves(int  limitChess, int  cntChess, int  color);
void makeJump(bool isMaxNode, int  color, int  depth, int  cJump, int  copylongestJump, int  r, int  c, int alpha, int beta, int * jumpBest);
void makeMove(bool isMaxNode, int  color, int  depth, int  r, int  c, int alpha, int beta, int * moveBest);
/*******function**********/
int main(){
    //freopen("test.in", "r", stdin);
    //freopen("test.out", "w", stdout);
    while(true){
        memset(Command, '\0', sizeof(Command));
        gets(Command);
        if(!strncmp(Command, "END ", 3)) 
            exit(0);
        if(!strncmp(Command, "START ", 5)){
            myColor = (*(Command + 6) - '0') == 1 ? 1 : -1;
            opponentColor = ~myColor + 1;
            boardInitialize(), chessInit(&Chess[1], 1)/*, moveClear()*/;
            printf("OK\n"), fflush(stdout);
            #ifdef Debug
                printBoard();
            #endif
        }
        if(!strncmp(Command, "TURN ", 4)){
            timeOfTurn = clock();
            timeLimit = min(((MAXSEC - myTime) / (60 - curStep)), 1750) - 100;
            depthManager();
            timeOfTurn = clock();
            int bestVal = minMax(true, 0, -Inf, Inf);
            deepeningCommand = moveCommand;
            if(clock() - timeOfTurn <= 500 && curStep <= 48) iterativeDeepening(bestVal);
            currentTime = clock();
            printMove();
            myTime += currentTime - timeOfTurn;
            #ifdef Debug
                printf("time cost = %.3f\n", 1.0 * (currentTime - timeOfTurn) / 1000);
            #endif
        }
        if(!strncmp(Command, "PLACE ", 5)){
            dealOppoentPlace(Command, strlen(Command));
        }
    }
    return 0;
}
inline static void printMove(){
    printf("%d", deepeningCommand.cntMoves + 1), fflush(stdout);
    for(int i = 0; i <= deepeningCommand.cntMoves; i++)
        place(deepeningCommand.r[i], deepeningCommand.c[i]);
    printf("\n"), fflush(stdout);
    dealMyPlace();
}
inline static void getRestChess(){
    myResChess = opponentResChess = 0;
    for(int i = 1; i <= PIECE; i++)
        if(Chess[i].color) myResChess++;
    for(int i = 1; i <= PIECE; i++)
        if(Chess[i].color) opponentResChess++;
}
inline static void depthManager(){ /**O2 13 not O2 12***/
    curStep++;
    getRestChess();
    #ifdef FAST
        Depth = 12;
        if(curStep > 50) Depth = 10; 
    #endif
    #ifndef FAST
        Depth = 11;
    #endif
}
inline static void iterativeDeepening(int preBest){
    for(Depth = 13; ; Depth++){
        if(clock() - timeOfTurn >= 900) return;
        int bestVal = minMax(true, 0, -Inf, Inf);
        if(clock() - timeOfTurn >= timeLimit) return;
        if(bestVal >= preBest){
            deepeningCommand = moveCommand;
            preBest = bestVal;
        } 
    }
}
inline static bool isNotInBound(int r, int c){
    return r >= SIZE || r < 0 || c >= SIZE || c < 0;
}
inline static void judgeKing(board * pBoard, int r, int color){
    if((color == WHITE && r == SIZE - 1) || (color == BLACK && r == 0)) 
        pBoard->isKing = true;
}
inline static void clearBoard(board * pBoard){
    pBoard->color = EMPTY, pBoard->isKing = false, pBoard->chessId = EMPTY;
}
inline static void chessInit(chess * pChess, int cnt){
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
    for(int i = 1; i <= (PIECE << 1); i++) Chess[i].lgstJump = 0;
}
void printBoard(void){
    putchar(' ');
    for(int i = 0; i < SIZE; i++)
        printf("%3d", i);
    putchar('\n');
    for(int i = 0; i < SIZE; i++){
        printf("%d ",i);
        for(int j = 0; j < SIZE; j++)
            printf("%2d ", Board[i][j].chessId);
        putchar('\n');
    }  
    printf("Okay, chessId printed\n");
}
void printBoardcolor(void){
    putchar(' ');
    for(int i = 0; i < SIZE; i++)
        printf("%3d", i);
    putchar('\n');
    for(int i = 0; i < SIZE; i++){
        printf("%d ",i);
        for(int j = 0; j < SIZE; j++)
            printf("%2d ", Board[i][j].color);
        putchar('\n');
    }  
    printf("Okay, color printed\n");
}
inline static void dealOppoentPlace(char * s, int len){ //finished
    int preR, preC, nowR, nowC;
    int midR, midC;
    int opponentId, myId;
    for(int i = 7; i < len; i += 4){
        if(i == 7) 
            preR = *(s + i + 1) - '0', preC = *(s + i + 3) - '0';
        else{
            nowR = *(s + i + 1) - '0', nowC = *(s + i + 3) - '0';
            midR = (preR + nowR) >> 1;
            midC = (preC + nowC) >> 1;
            /*get chess Id*/
            opponentId = Board[preR][preC].chessId;
            myId = Board[midR][midC].chessId;
            /*get chess Id*/
            Board[nowR][nowC] = Board[preR][preC];
            clearBoard(&Board[preR][preC]);
            changeChessPos(&Chess[opponentId], nowR, nowC);
            if(midR != preR && midR != nowR){
                clearBoard(&Board[midR][midC]);
                Chess[myId].color = EMPTY;
            }      
            preR = nowR, preC = nowC;
        }
    }
    judgeKing(&Board[preR][preC], preR, opponentColor);
    #ifdef Debug
        printBoardcolor();
        printBoard();
    #endif
}
inline static void dealMyPlace(void){
    int preR, preC, nowR, nowC;
    int midR, midC;
    int opponentId, myId; 
    preR = deepeningCommand.r[0], preC = deepeningCommand.c[0];
    for(int i = 1; i <= deepeningCommand.cntMoves; i++){
        nowR = deepeningCommand.r[i], nowC = deepeningCommand.c[i];
        if(Board[preR][preC].color != myColor){
            printf("error!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            exit(0);
        }
        midR = (preR + nowR) >> 1;
        midC = (preC + nowC) >> 1;
        myId = Board[preR][preC].chessId;
        opponentId = Board[midR][midC].chessId;
        /*get chess Id*/
        Board[nowR][nowC] = Board[preR][preC];
        changeChessPos(&Chess[myId], nowR, nowC);
        clearBoard(&Board[preR][preC]);
        if(midR != preR && midR != nowR){
            clearBoard(&Board[midR][midC]);
            Chess[opponentId].color = EMPTY;
        }      
        preR = nowR, preC = nowC;
    }
    judgeKing(&Board[preR][preC], preR, myColor);
    clearCommand();
    #ifdef Debug
        printBoardcolor();
        printBoard();
    #endif
}
inline static void clearCommand(){
    memset(&moveCommand, 0, sizeof(myCommand));
    memset(&tmpCommand, 0, sizeof(myCommand));
    memset(&storeCommand, 0, sizeof(myCommand));
    memset(&deepeningCommand, 0, sizeof(myCommand));
}
inline static bool chessThreatened(int r, int c, int color){
    int * pDr, * pDc;
    pDr = color == BLACK ? blackDr : whiteDr;
    pDc = color == BLACK ? blackDc : whiteDc;
    for(int i = 0; i < 2; i++){
        int nextR1 = r + *(pDr + i), nextC1 = c + *(pDc + i);
        if(isNotInBound(nextR1, nextC1)) continue;
        int nextR2 = r - *(pDr + i), nextC2 = c - *(pDc + i);
        if(isNotInBound(nextR2, nextC2)) continue;
        if(Board[nextR1][nextC1].color == (~color + 1) && !Board[nextR2][nextC2].color)
            return true;
        if(Board[nextR2][nextC2].color == (~color + 1) && !Board[nextR1][nextC1].color)
            return true;
    }
    pDr = NULL, pDc = NULL;
    return false;
}
inline static bool boundaryJump(int r, int c, int color){
    int * pDr, * pDc;
    board * pBoard = &Board[r][c];
    pDr = color == BLACK ? blackDr : whiteDr;
    pDc = color == BLACK ? blackDc : whiteDc;
    int endJ = pBoard->isKing ? 4 : 2;
    for(int i = 0; i < endJ; i++){
        int nextR = r + *(pDr + i), nextC = c + *(pDc + i);
        if(isNotInBound(nextR, nextC)) continue;
        int jumpR = nextR + *(pDr + i), jumpC = nextC + *(pDc + i);
        pBoard = &Board[nextR][nextC];
        if(pBoard->color == (~color + 1) && !Board[jumpR][jumpC].color)
            return true;
    }
    return false;
}
inline static bool canBeKing(int r, int c, int color){
    if(color == BLACK && r == 1 && c == 0 && !Board[0][1].color)
        return true;
    if(color == WHITE && r == SIZE - 2 && c == SIZE - 1 && !Board[SIZE - 1][SIZE - 2].color)
        return true;
    return false;
}
inline static int evaluateScore(bool isMaxNode){/*should be much more complex, needed to be changed*/
    int myScore = 0, opponentScore = 0;
    for(int i = 1; i <= PIECE; i++){
        chess * pChess = &Chess[i];
        if(pChess->color == EMPTY) continue;
        int r = Chess[i].r, c = Chess[i].c;
        board * pBoard = &Board[r][c];
        if(pBoard->isKing) myScore += KINGVALUE;
            else myScore += CHESSVALUE;
        if(chessThreatened(r, c, myColor)){
            if(pBoard->isKing) myScore -= (isMaxNode ? QUARTER(KINGVALUE) : HALF(KINGVALUE));
            else myScore -= (isMaxNode ? QUARTER(CHESSVALUE) : HALF(CHESSVALUE)); //上一层为minNode 敌方已做完决策
        } 
        if(c == 0 || c == SIZE - 1){
            if(isMaxNode){
                if(boundaryJump(r, c, myColor)) myScore += HALF(CHESSVALUE);
                    else if(!pBoard->isKing && canBeKing(r, c, myColor)) myScore += HALF(KINGVALUE) + QUARTER(KINGVALUE);
                else myScore -= QUARTER(CHESSVALUE) >> 1;
            }
        }
        if(myColor == BLACK && !pBoard->isKing) {
            if(r <= 2) myScore += (SIZE - 1 - r) << 3;
            if(r == 7) myScore += r;
        } 
        if(myColor == WHITE && !pBoard->isKing){
            if(r >= 6) myScore += r << 3;
            if(r == 0) myScore += SIZE - 1 - r;
        }
    } 
    for(int i = PIECE + 1; i <= PIECE << 1; i++){
        chess * pChess = &Chess[i];
        if(pChess->color == EMPTY) continue;
        int r = Chess[i].r, c = Chess[i].c;
        board * pBoard = &Board[r][c];
        if(pBoard->isKing) opponentScore += KINGVALUE;
            else opponentScore += CHESSVALUE;
        if(chessThreatened(r, c, opponentColor)){
            if(pBoard->isKing) opponentScore -= (isMaxNode ? HALF(KINGVALUE) : QUARTER(KINGVALUE)); //上一层为minNode 敌方已做完决策
            else opponentScore -= (isMaxNode ? HALF(CHESSVALUE) : QUARTER(CHESSVALUE)); //上一层为minNode 敌方已做完决策
        }       
        if(c == 0 || c == SIZE - 1) opponentScore -= QUARTER(CHESSVALUE) >> 1;
        if(opponentColor == WHITE && !pBoard->isKing) {
            if(r >= 6) opponentScore += r << 3;
            if(r == 0) opponentScore += SIZE - 1 - r;
        } 
        if(opponentColor == BLACK && !pBoard->isKing) {
            if(r <= 1) opponentScore += (SIZE - 1 - r) << 3;
            if(r == 7) opponentColor += r;
        }
    }
    return (myScore - opponentScore);
}
void jumpJudge(int color/*color of current player*/, int  r, int  c, int  cJump){
    if(cJump > thisJump) thisJump = cJump;
    int * pDr, * pDc;
    pDr = color == BLACK ? blackDr : whiteDr;
    pDc = color == BLACK ? blackDc : whiteDc;
    for(int i = 0; i < 4; i ++){
        int nextR = r + *(pDr + i), nextC = c + *(pDc + i);
        int jumpR = nextR + *(pDr + i), jumpC = nextC + *(pDc + i);
        if(isNotInBound(jumpR, jumpC)) 
            continue;
        if(Board[nextR][nextC].color == (~color + 1) && !Board[jumpR][jumpC].color){
            //紧邻棋子为敌方棋子，且敌方棋子后为空白
            board start = Board[r][c], opponent = Board[nextR][nextC];//make copy
            int startId = start.chessId, opponentId = opponent.chessId;/***/
            Board[jumpR][jumpC] = Board[r][c];//change value
            changeChessPos(&Chess[startId], jumpR, jumpC);
            Chess[opponentId].color = EMPTY;
            clearBoard(&Board[r][c]), clearBoard(&Board[nextR][nextC]);//change value   
            /*get next jump*/
            jumpJudge(color, jumpR, jumpC, cJump + 1);
            /*get next jump*/
            Board[r][c] = start, Board[nextR][nextC] = opponent;
            backTrackChessPos(&Chess[startId], r, c);
            Chess[opponentId].color = (~color + 1);
            clearBoard(&Board[jumpR][jumpC]);
            /*back track*/
        }
    }
}
int getPossilbeMoves(int limitChess, int cntChess, int color){
    //id 1~12 mychess  id 13~24 opponent chess
    int startI, endI, endJ = 2;
    int * pDr, *pDc;
    startI = color == myColor ? 1 : PIECE + 1;
    endI = color == myColor ? PIECE : PIECE << 1;
    //get chess id range
    pDr = color == BLACK ? blackDr : whiteDr;
    pDc = color == BLACK ? blackDc : whiteDc;
    //move pointer
    for(int  i = startI; i <= endI; i++){
        int  r = Chess[i].r, c = Chess[i].c;
        board * pBoard = &Board[r][c];
        if(!pBoard->color || !Chess[i].color) continue;
        thisJump = 0;
        /********jump part*****************/
        jumpJudge(color, r, c, 0);
        Chess[i].lgstJump = thisJump;
        if(thisJump && thisJump >= longestJump){
            longestJump = thisJump;
            jumpChess[cntJump++] = Chess[i];
            continue;
        }
        if(longestJump) continue; //可以跳跃 不用再判断能否走
        /********jump part*****************/
        /*just move in the following lines*/
        if(pBoard->isKing) endJ = 4;
        for(int j = 0; j < endJ; j++){//
            int nextR = r + *(pDr + j), nextC = c + *(pDc + j);
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
void makeJump(bool isMaxNode, int  color, int  depth, int  cJump, int  copylongestJump, int  r, int  c, int alpha, int beta, int * jumpBest){
    /*不再继续跳跃 继续向下搜索*/
    if(cJump == copylongestJump){
        board * pBoard = &Board[r][c];
        judgeKing(pBoard, r, color);
        int value = minMax(isMaxNode ^ 1, depth + 1, alpha, beta); 
        if(isMaxNode){
            if((*jumpBest) < value){
                *jumpBest = value;
                if(depth == 0){
                    storeCommand.cntMoves = cJump;
                    storeCommand.r[cJump] = r;
                    storeCommand.c[cJump] = c;
                    tmpCommand = storeCommand;
                }
            }
        }
        else if((*jumpBest) > value) *jumpBest = value;
        pBoard->isKing = false;
        return;
    }
    int * pDr, *pDc;
    pDr = color == BLACK ? blackDr : whiteDr;
    pDc = color == BLACK ? blackDc : whiteDc;
    for(int  i = 0; i < 4; i++){
        int  nextR = r + *(pDr + i), nextC = c + *(pDc + i);
        int  jumpR = nextR + *(pDr + i), jumpC = nextC + *(pDc + i);
        if(isNotInBound(jumpR, jumpC)) 
            continue;
        if(Board[nextR][nextC].color == (~color + 1) && !Board[jumpR][jumpC].color){
            //紧邻棋子为敌方棋子，且敌方棋子后为空白
            board start = Board[r][c], opponent = Board[nextR][nextC];//make copy
            int startId = start.chessId, opponentId = opponent.chessId;/***/
            Board[jumpR][jumpC] = Board[r][c];//change value
            changeChessPos(&Chess[startId], jumpR, jumpC);
            Chess[opponentId].color = EMPTY;
            clearBoard(&Board[r][c]), clearBoard(&Board[nextR][nextC]);
            if(depth == 0){
                storeCommand.cntMoves = cJump;
                storeCommand.r[cJump] = r;
                storeCommand.c[cJump] = c;
            }
            /*get next jump*/
            makeJump(isMaxNode, color, depth, cJump + 1, copylongestJump, jumpR, jumpC, alpha, beta, jumpBest);
            /*get next jump*/
            Board[r][c] = start, Board[nextR][nextC] = opponent;
            backTrackChessPos(&Chess[startId], r, c);
            Chess[opponentId].color = (~color + 1);
            clearBoard(&Board[jumpR][jumpC]);
            /*back track*/
        }
    }
}
void makeMove(bool isMaxNode, int  color, int  depth, int r, int c, int alpha, int beta, int * moveBest){
    int endJ = Board[r][c].isKing ? 4 : 2;
    int * pDr, *pDc;
    pDr = color == BLACK ? blackDr : whiteDr;
    pDc = color == BLACK ? blackDc : whiteDc;
    for(int i = 0; i < endJ; i++){//
        int nextR = r + *(pDr + i), nextC = c + *(pDc + i);
        if(isNotInBound(nextR, nextC)) 
            continue;
        if(Board[nextR][nextC].color == EMPTY){
            board start = Board[r][c];
            int chessId = start.chessId;
            Board[nextR][nextC] = Board[r][c];
            changeChessPos(&Chess[chessId], nextR, nextC);
            judgeKing(&Board[nextR][nextC], nextR, color);
            clearBoard(&Board[r][c]);
            int value = minMax(isMaxNode ^ 1, depth + 1, alpha, beta);
            if(isMaxNode){
                if((*moveBest) < value){
                    *moveBest = value;
                    if(depth == 0){
                        storeCommand.cntMoves = 1;
                        storeCommand.r[0] = r, storeCommand.c[0] = c;
                        storeCommand.r[1] = nextR, storeCommand.c[1] = nextC;
                        tmpCommand = storeCommand;
                    }
                }
            } 
            else if((*moveBest) > value) *moveBest = value;
            Board[r][c] = start;
            backTrackChessPos(&Chess[chessId], r, c);
            clearBoard(&Board[nextR][nextC]);   
        }   
    }
}
int minMax(bool isMaxNode, int depth, int alpha, int beta){
    if(depth >= Depth) return evaluateScore(isMaxNode);
    if(clock() - timeOfTurn >= timeLimit){
        int cutOffScore = evaluateScore(isMaxNode);
        if(isMaxNode) cutOffScore = cutOffScore > 0? cutOffScore << 1 : cutOffScore >> 1;
            else cutOffScore = cutOffScore > 0 ? cutOffScore >> 1 : cutOffScore << 1;
        return cutOffScore;  
    }
    int cntChess, copyLongestJump, copyCntJump; //使用局部变量防止递归修改值
    chess copyMoveChess[PIECE + 1], copyJumpChess[PIECE + 1];
    int value, bestVal;
    cleanLgstJump();
    if(isMaxNode){
        value = -Inf, bestVal = -Inf, longestJump = 0, cntJump = 0/*related to jump chess*/;
        cntChess = getPossilbeMoves(myResChess > 8 ? 9 : myResChess, 0, myColor);
        copyLongestJump = longestJump, copyCntJump = cntJump;
        memcpy(copyJumpChess, jumpChess, sizeof(copyJumpChess));
        memcpy(copyMoveChess, moveChess, sizeof(copyMoveChess));
        /**jumpChess moveChess copy**/
        if(copyCntJump){
            for(int i = 0; i < copyCntJump; i++){
                if(copyJumpChess[i].lgstJump < copyLongestJump) continue;
                makeJump(isMaxNode, myColor, depth, 0, copyLongestJump, copyJumpChess[i].r, copyJumpChess[i].c, alpha, beta, &value);
                bestVal = max(bestVal, value);
                if(bestVal == value && !depth){
                    //printf("Line %d %.3f\n", __LINE__, bestVal);
                    moveCommand = tmpCommand; //apply moves;
                }
                alpha = max(alpha, bestVal);
                if(beta <= alpha){
                    break;
                }
            }
        }else{
            for(int i = 0; i < cntChess; i++){
                makeMove(isMaxNode, myColor, depth, copyMoveChess[i].r, copyMoveChess[i].c, alpha, beta, &value);
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
    }
    if(!isMaxNode){
        value = Inf, bestVal = Inf, longestJump = 0, cntJump = 0/*related to jump chess*/;
        cntChess = getPossilbeMoves(opponentResChess > 8 ? 9 : opponentResChess, 0, opponentColor);
        copyLongestJump = longestJump, copyCntJump = cntJump;
        memcpy(copyJumpChess, jumpChess, sizeof(copyJumpChess));
        memcpy(copyMoveChess, moveChess, sizeof(copyMoveChess));
         /**jumpChess moveChess copy**/
        if(copyCntJump){
            for(int i = 0; i < copyCntJump; i++){
                if(copyJumpChess[i].lgstJump < copyLongestJump) continue;
                makeJump(isMaxNode, opponentColor, depth, 0, copyLongestJump, copyJumpChess[i].r, copyJumpChess[i].c, alpha, beta, &value);
                bestVal = min(bestVal, value);
                beta = min(beta, bestVal);
                if(beta <= alpha){
                    break;
                }
            }
        }else{
            for(int i = 0; i < cntChess; i++){
                makeMove(isMaxNode, opponentColor, depth, copyMoveChess[i].r, copyMoveChess[i].c, alpha, beta, &value);
                bestVal = min(bestVal, value);
                beta = min(beta, bestVal);
                if(beta <= alpha){
                    break;
                }
            }
        }
    }
    return bestVal;
}
