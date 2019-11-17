#include<stdio.h>
#include<math.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<stdbool.h>
#define place(x, y) printf(" %d,%d", x, y), fflush(stdout)
#define DEBUG printf("DEBUG [MESSAGE]\n"), fflush(stdout)
typedef char byte;
/********************/
#define SIZE (byte)8 
#define EMPTY (byte)0
#define BLACK (byte)1
#define WHITE (byte)2
#define chessValue (byte)1
#define kingValue (byte)3
#define LEFT (byte)-1
#define RIGHT (byte)1
#define max(a, b) (a > b ? a : b)
/********************/
byte row, column;
byte myColor;
//bool Black_side; //down_side fisrt
//bool White_side; //up_side
byte Command[SIZE * SIZE]; //store orders
byte myMarch, opponentMarch;
byte myScore, opponentScore;
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
s Board[SIZE][SIZE];
void boardInitialize(void);
void printBoard(void);
void dealOppoentPlace(char *s, byte len);
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
            if(myColor == 1) myMarch = -1;
            if(myColor == 2) myMarch = 1;
            boardInitialize();
            printf("OK\n"), fflush(stdout);
        }
        if(!strncmp(Command, "TURN ", 4)){
            /**function**/
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
    for(int i = 5; i < len; i += 4){
        if(i == 5) 
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
    calculateScore();
    printScore;
}
void calculateScore(void){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++)
            if(Board[i][j].color == myColor)
                myScore += (Board[i][j].isKing ? kingValue : chessValue);
            else if(Board[i][j].color)
                opponentScore += (Board[i][j].isKing ? kingValue : chessValue);
    }  
}