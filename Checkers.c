#pragma GCC optimize (2)
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
typedef char byte;
//CLOCKS_PER_SEC
/********************/
#define SIZE (byte)8 
#define PIECE (byte)12
#define EMPTY (byte)0
#define BLACK (byte)1
#define WHITE (byte)2
#define chessValue (byte)1
#define kingValue (byte)3
#define MAXNODE (byte)1
#define MINNODE (byte)0
#define MAXJUMP (byte)16
#define max(a,b) (a > b ? a : b)
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
byte mustJump /*in function jumpJudge, remember to set it to 0*/, longestJump;
#define printScore printf("myScore: %.4f opponentScore: %.4f\n", myScore, opponentScore), fflush(stdout)
typedef struct{ 
    byte preR, preC;
}Path;
typedef struct{
    byte r, c;
}pathStore;
typedef struct{
    bool isKing;
    byte color;
}board;
typedef struct{
    byte r, c;
    bool isKing;
    bool mustAttack;
    byte color;
}chess;
board Board[SIZE][SIZE];
chess myChess[PIECE], opponentChess[PIECE];
Path prePath[SIZE][SIZE];
pathStore movePath[PIECE];
/*******move*************/
byte dr[] = {-1, -1, 1, 1};
byte dc[] = {-1, 1, -1, 1};
/*******move*************/
//Beta is the minimum upper bound of possible solutions  最好情况最少多少分
//Alpha is the maximum lower bound of possible solutions 最坏情况最多多少分
//a <=n <= b    a >= b stop consideration
int beta = Inf, alpha = - Inf;
byte minValue, maxValue;
//alpha beta tree search
void boardInitialize(void);
void printBoard(void);
void chessInit(void);
void dealOppoentPlace(char *s, byte len);
int evaluateScore/*Score evaluation*/(void);
void moveClear(void);
void jumpJudge(int color/*black side or white side*/, int r, int c, int cntJump); //0 can't jump
void evaluatePath(int);
int minMax(bool isMaxNode, int r, int c, int depth, int alpha, int beta);
/*******function**********/
int main(){
    freopen("test.in", "r", stdin);
    freopen("test.out", "w", stdout);
    while(true){
        memset(Command, '\0', sizeof(Command));
        gets(Command);
        if(!strncmp(Command, "END ", 3)) 
            break;
        if(!strncmp(Command, "START ", 5)){
            myColor = *(Command + 6) - '0';
            opponentColor = (myColor == 1 ? 2 : 1);
            boardInitialize(), chessInit()/*, moveClear()*/;
            if(myColor == 1) myMarch = -1;  //black up
            if(myColor == 2) myMarch = 1; //white
            printf("OK\n"), fflush(stdout);
        }
        if(!strncmp(Command, "TURN ", 4)){
            Depth = 6;
            timeOfTurn = clock() / (double)CLOCKS_PER_SEC;
            //moveClear()
            longestJump = 0, jumpJudge(opponentColor, 5, 2, 0);
            printf("%d\n", longestJump), fflush(stdout);
            printf("Calculating\n"), fflush(stdout);
        }
        if(!strncmp(Command, "PLACE ", 5)){
            dealOppoentPlace(Command, strlen(Command));
            /**function**/
            //printf("Okay, placed\n"), fflush(stdout);
        }
    }
    return 0;
}
void chessInit(void){
    int cnt1 = 0, cnt2 = 0;
    if(myColor == BLACK){
        for(int i = 5; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == myColor){
                    myChess[cnt1].r = i, myChess[cnt1].c = j;
                    myChess[cnt1++].color = myColor;
                }
        for(int i = 2; i >= 0; i--)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == opponentColor){
                    opponentChess[cnt2].r = i, opponentChess[cnt2].c = j;
                    opponentChess[cnt2++].color = opponentColor;
                }
    }
    if(myColor == WHITE){
        for(int i = 5; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == opponentColor){
                    opponentChess[cnt2].r = i, opponentChess[cnt2].c = j;
                    opponentChess[cnt2++].color = opponentColor;
                }
        for(int i = 2; i >= 0; i--)
            for(int j = 0; j < SIZE; j++)
                if(Board[i][j].color == myColor){
                    myChess[cnt1].r = i, myChess[cnt1].c = j;
                    myChess[cnt1++].color = myColor;
                }
    }      
}
void boardInitialize(void){
    for(int i = 0; i < SIZE; i++)
        for(int j = 0; j < SIZE; j++)
            Board[i][j].color = EMPTY;
    for(int i = 1; i < SIZE; i += 2)
        Board[0][i].color = Board[2][i].color = WHITE,
        Board[6][i].color = BLACK;
    for(int i = 0; i < SIZE; i += 2)
        Board[5][i].color = Board[7][i].color = BLACK,
        Board[1][i].color = WHITE;
}
void printBoard(void){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++)
            printf("%d ", Board[i][j].color);
        putchar('\n');
    }  
    printf("Okay, printed\n");
}
void dealOppoentPlace(char *s, byte len){ //finished
    byte preR, preC, nowR, nowC;
    byte midR, midC;
    for(int i = 7; i < len; i += 4){
        if(i == 7) 
            preR = *(s + i + 1) - '0', preC = *(s + i + 3) - '0';
        else{
            nowR = *(s + i + 1) - '0', nowC = *(s + i + 3) - '0';
            midR = preR + nowR >> 1;
            midC = preC + nowC >> 1;
            Board[nowR][nowC] = Board[preR][preC];
            memset(&Board[preR][preC], 0, sizeof(board));
            if(midR != preR && midR != nowR)
                memset(&Board[midR][midC], 0, sizeof(board));
            preR = nowR, preC = nowC;
        }
    }
    printBoard();
    //calculateScore();
    //printScore;
}
int evaluateScore(void){/*should be much more complex, needed to be changed*/
    myScore = opponentScore = false;
    for(int i = 0; i < PIECE; i++){
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
    } 
    printScore;
    return myScore - opponentScore;
}
/*jumpJudge finished*/
void jumpJudge(int color/*black or white side,should be opponent's*/, int r, int c, int cntJump){
    if(cntJump > longestJump) longestJump = cntJump;
    for(int i = 0; i < 4; i ++){
        int nextR = r + dr[i], nextC = c + dc[i];
        int jumpR = r + 2 * dr[i], jumpC = c + 2 * dc[i];
        if(jumpR >= SIZE || jumpR < 0 || jumpC >= SIZE || jumpC < 0) continue;
        if(Board[nextR][nextC].color == color && !Board[jumpR][jumpC].color){
            //紧邻棋子为敌方棋子，且敌方棋子后为空白
            board start = Board[r][c], opponent = Board[nextR][nextC];//make copy
            Board[jumpR][jumpC] = Board[r][c];//change value
            memset(&Board[r][c], 0, sizeof(board));//change value
            memset(&Board[nextR][nextC], 0, sizeof(board));//change value
            /*get next jump*/
            jumpJudge(color, jumpR, jumpC, cntJump + 1);
            /*get next jump*/
            Board[r][c] = start, Board[nextR][nextC] = opponent;
            memset(&Board[jumpR][jumpC], 0, sizeof(board));
            /*back track*/
        }
    }
}
int minMax(bool isMaxNode, int r, int c, int depth, int alpha, int beta){

}
/*int MinMax(int depth) {
 if(SideToMove() == WHITE) { // 白方是“最大”者
 return Max(depth);
} else { // 黑方是“最小”者
 return Min(depth);
 }
}

int Max(int depth) {
　int best = -INFINITY;
　if (depth <= 0) {
　　return Evaluate();
　}
  currentTime = clock() / (double)CLOCKS_PER_SEC;
　GenerateLegalMoves();
　while (MovesLeft()) {
　　MakeNextMove();
　　val = Min(depth - 1);
　　UnmakeMove();
　　if (val > best) {
　　　best = val;
　　}
　}
　return best;
}
　
int Min(int depth) {
　int best = INFINITY;　// 注意这里不同于“最大”算法
　if (depth <= 0) {
　　return Evaluate();
　}
　GenerateLegalMoves();
　while (MovesLeft()) {
　　MakeNextMove();
　　val = Max(depth - 1);
　　UnmakeMove();
　　if (val < best) { 　// 注意这里不同于“最大”算法
　　　best = val;
　　}
　}
　return best;
}*/

/*
int minimax( Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta )
{
    if ( node.childs.size() == 0 )
        return node.value;

    int value, bestVal;

    if ( isMaximizingPlayer ) {
        bestVal = -100000;
        for ( auto& child : node.childs ) {
            value = minimax(child, depth+1, false, alpha, beta);
            bestVal = std::max( bestVal, value);
            if ( bestVal == value && depth == 0 )
                node.move = child.move;
            alpha = std::max( alpha, bestVal);
            if ( beta <= alpha )
                break;
        }
        return bestVal;

    } else {
        bestVal = 100000;
        for ( auto& child : node.childs ) {
            value = minimax(child, depth+1, true, alpha, beta);
            bestVal = std::min( bestVal, value );
            if ( bestVal == value && depth == 0 )
                node.move = child.move;
            beta = std::min( beta, bestVal);
            if ( beta <= alpha )
                break;
        return bestVal;
        }
    }
}
*/











































































































































































































