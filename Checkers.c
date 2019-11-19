#include<stdio.h>
#include<math.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<stdbool.h>
#define place(x, y) printf(" %d,%d", x, y), fflush(stdout)
#define DEBUG printf("DEBUG [MESSAGE]\n"), fflush(stdout)
const int Inf = 0x3f3f3f3f;
typedef char byte;
/********************/
#define SIZE (byte)8 
#define PIECE (byte)12
#define EMPTY (byte)0
#define BLACK (byte)1
#define WHITE (byte)2
#define chessValue (byte)1
#define kingValue (byte)3
#define LEFT (byte)-1
#define RIGHT (byte)1
#define MAXNODE (byte)1
#define MINNODE (byte)0
#define max(a, b) (a > b ? a : b)
/********************/
byte Depth;
byte row, column;
byte myColor;
//bool Black_side; //down_side fisrt
//bool White_side; //up_side
byte Command[SIZE * SIZE]; //store orders
byte myMarch, opponentMarch;
byte myScore = 12, opponentScore = 12, diff; //to make diff as big as possilbe
#define printScore printf("myScore: %d opponentScore: %d\n", myScore, opponentScore), fflush(stdout)
/*if(the_checker[1].color=="white"){
		cpy_downRight = upRight;
		cpy_downLeft = upLeft;
		cpy_upLeft = downLeft;
		cpy_upRight = downRight;
	}
	else{
		cpy_downRight = upLeft;
		cpy_downLeft = upRight;
		cpy_upLeft = downRight;
		cpy_upRight = downLeft;
}  */
typedef struct{
    int up_left_point;
    int up_right_point;
    int down_left_point;
    int down_right_point;
    bool isKing;
    bool mustAttack;
    byte color;
}s;
typedef struct{
    byte r, c;
    bool isKing;
    bool mustAttack;
    byte color;
}c;
s Board[SIZE][SIZE];
c Chess[PIECE];
int beta = Inf, alpha = - Inf;
byte minValue, maxValue;
//Beta is the minimum upper bound of possible solutions  最好情况最少多少分
//Alpha is the maximum lower bound of possible solutions 最坏情况最多多少分
//a <=n <= b    a >= b stop consideration
void boardInitialize(void);
void printBoard(void);
void chessInit(void);
void dealOppoentPlace(char *s, byte len);
void Evaluation(void);
void calculateScore(void);
int get_max(int r, int c){
    int Max = max(Board[r][c].up_left_point, Board[r][c].up_right_point);
    Max = max(Max, max(Board[r][c].down_left_point, Board[r][c].down_right_point));
    return Max;
}
inline void eliminateChess(){
    return;
}
inline void enemyMoveCoty(){
    return;
}
inline void Evaluate(){ //if two points are the same, choose the most optimal way
    
}
inline void executeMove(){
    Evaluate();
    return;
}
void clear(char *s, int len){
    memset(s, '\0', len * sizeof(char));
}
int main(){
    freopen("test.in", "r", stdin);
    freopen("test.out", "w", stdout);
    while(true){
        gets(Command);
        if(!strncmp(Command, "END ", 3)) 
            break;
        if(!strncmp(Command, "START ", 5)){
            myColor = *(Command + 6) - '0';
            chessInit();
            if(myColor == 1) myMarch = -1;  //black up
            if(myColor == 2) myMarch = 1; //white
            boardInitialize();
            printf("OK\n"), fflush(stdout);
        }
        if(!strncmp(Command, "TURN ", 4)){
            Depth = 6;
            /**function**/ //build tree
            printf("Calculating\n"), fflush(stdout);
        }
        if(!strncmp(Command, "PLACE ", 5)){
            dealOppoentPlace(Command, strlen(Command));
            /**function**/
            printf("Okay, placed\n"), fflush(stdout);
        }
    }
    return 0;
}
void chessInit(){
    int cnt = 0;
    for(int i = 0; i < SIZE; i++)
        for(int j = 0; j < SIZE; j++)
            if(Board[i][j].color == myColor){
                Chess[cnt].r = i, Chess[cnt].c = j;
                Chess[cnt].color = myColor;
                cnt++;
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
    printBoard();
}
void printBoard(void){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++)
            printf("%d ", Board[i][j].color);
        putchar('\n');
    }  
}
void dealOppoentPlace(char *s, byte len){
    byte preR, preC, nowR, nowC;
    byte midR, midC;
    for(int i = 7; i < len; i += 4){
        if(i == 7) 
            preR = *(s + i + 1) - '0', preC = *(s + i + 3) - '0';
        else{
            nowR = *(s + i + 1) - '0', nowC = *(s + i + 3) - '0';
            midR = (preR + nowR) >> 1;
            midC = (preC + nowC) >> 1;
            Board[nowR][nowC] = Board[preR][preC];
            memset(&Board[preR][preC], 0, sizeof(Board[preR][preC]));
            memset(&Board[midR][midC], 0, sizeof(Board[midR][midC]));
            preR = nowR, preC = nowC;
        }
    }
    printBoard();
    //calculateScore();
    //printScore;
}
void calculateScore(void){
    myScore = opponentScore = false;
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++)
            if(Board[i][j].color == myColor)
                myScore += (Board[i][j].isKing ? kingValue : chessValue);
            else if(Board[i][j].color)
                opponentScore += (Board[i][j].isKing ? kingValue : chessValue);
    }  
}
