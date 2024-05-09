#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 

typedef struct def_boardstruct {
  char a;
  char b;
  char c;
  char d;
  char e;
  char f;
  char g;
  char h;
  char i;
} BOARDSTRUCT;

typedef struct def_scorestruct {
  int score; // win: 1, nothing: 0, tie: -1, loss: -2 (this is all multiplied by a big power of 10)
  int depth;
  char exists; // boolean
} SCORESTRUCT;


// !!!!!!! estado tablero no esta declarado porque esta realmente en otro archivo
void printBoard(BOARDSTRUCT board);
char *getBoardItem(BOARDSTRUCT *board, int index);
void makeBoardArray(BOARDSTRUCT *board, char array[]);
char getPiece(int index);
SCORESTRUCT getMoveScore(BOARDSTRUCT board, int square, int piece, int ogpiece, int depth);


void printBoard(BOARDSTRUCT board) {
  int i, j;
  char boardArr[9];
  makeBoardArray(&board, boardArr);
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      printf("%d:%c ",(i*3)+j, boardArr[(i*3)+j]);
    }
    printf("\n");
  }
}

/**
 *@fn función que revisa en que estado esta el tablero de juego; si contunua, es empate o hay algún ganador
 *@param recive la una cadena de caracteres que tiene la información de que tiene cada posición del tablero
 *@return en caso de que haya un ganador, regresa el símbolo de quien ganó, si es empate regresa 'g' de gato, si no es gato ni hay ganador regresa ' '
 */
char estadoTablero(char tab[9]){
  int i, cont=0;

  // 0 1 2 
       // 3 4 5
       // 6 7 8

       for(i=0; i < 9; i=i+3){
	 if(tab[i] == tab[i+1] && tab[i] == tab[i+2]){
	   if (tab[i] != ' ') {
	     return tab[i];
	   }
	 }
       }

  for(i = 0; i < 3; i++){
    //printf("%d:%c %d:%c %d:%c",i,tab[i], i+3,tab[i+3], i+6, tab[i+6]);
    if(tab[i] == tab[i+3] && tab[i] == tab[i+6]){
      if (tab[i] != ' ') {
	return tab[i];
      }
    }
  }

  // busca diagonales
  if(tab[0] == tab[4] && tab[0] == tab[8]){
    if (tab[0] != ' ') {
      return tab[0];
    }
  }                                                      
  if(tab[2] == tab[4] && tab[2] == tab[6]){   
    if (tab[2] != ' ') {
      return tab[2];                                   
    }
  }                                             

  // busca gato
  for(i = 0; i < 9; i++){
    if(tab[i] == ' '){
      cont++;
    }
  }
  if(cont == 0){
    return 'g';
  }
  else if(cont != 0){
    return 0;
  }
}


char *getBoardItem(BOARDSTRUCT *board, int index) {
  switch (index) {
  case 0:
    return &(board->a);
    break;
  case 1:
    return &(board->b);
    break;
  case 2:
    return &(board->c);
    break;
  case 3:
    return &(board->d);
    break;
  case 4:
    return &(board->e);
    break;
  case 5:
    return &(board->f);
    break;
  case 6:
    return &(board->g);
    break;
  case 7:
    return &(board->h);
    break;
  case 8:
    return &(board->i);
    break;
  default:
    printf("Error: attempted to access index (%d)\n", index);
    return NULL; // causes segfault so i know where I messed up. Can also be validted as a boolean
    break;
  }
}

void makeBoardArray(BOARDSTRUCT *board, char array[]) {
  int i;
  for (i = 0; i < 9; i++) {
    array[i] = (*getBoardItem(board, i));
  }
}

char getPiece(int index) {
  char arr[] = {'X','O'};
  return arr[index];
}



/**
 * for real this time
 */
SCORESTRUCT getMoveScore(BOARDSTRUCT board, int square, int piece, int ogpiece, int depth) {
  int i = 0;
  char c_piece;
  char c_ogpiece;
  SCORESTRUCT tmpscore;
  SCORESTRUCT res;
  char local_board[9];
  char estado;

  c_piece = getPiece(piece);
  c_ogpiece = getPiece(ogpiece);

  // jugando en el tablero local
  (*getBoardItem(&board, square)) = c_piece;
  makeBoardArray(&board, local_board);

  // evaluando el tablero
  estado = estadoTablero(local_board);

  if (estado) {
    res.exists = 1;

    if (estado == c_piece || estado == c_ogpiece) { // someone won
      if (c_piece == c_ogpiece) { // win
        res.score = 100000; // big numbers are important for int division to not be truncated to 0
        // tenemos la ventaja de que depth siempre va a ser < 8
      } else { // loss
        res.score = -200000;
      }
    } else { // gato
      res.score = -100000;
    } 
  } else {
    res.score = 0;
    res.exists = 0;
  }
  res.depth = depth;


  if (!res.exists) {
    for (i = 0; i < 9; i++) {
      if ((*getBoardItem(&board, i)) == ' ') {
        tmpscore = getMoveScore(board, i, (piece + 1) % 2, ogpiece, depth + 1);
        res.score += tmpscore.score / (tmpscore.depth + 1);
      }
    }
  }

  res.exists = 1;
  return res;
}


// this is very much for testing and should be ignored
int main(void) {
  BOARDSTRUCT realBoard;
  char boardArr[9];
  int i;
  int j;
  int input;
  int cpuInput;
  int tmpScore;
  int highestScore;
  char b;
  
  srand(time(NULL)); 

  for (i = 0; i < 9; i++) {
    *(getBoardItem(&realBoard, i)) = ' ';
  }
  // 0 1 2
  // 3 4 5
  // 6 7 8

  makeBoardArray(&realBoard, boardArr);

  while (!estadoTablero(boardArr)) {
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        printf("%d:%c ",(i*3)+j, boardArr[(i*3)+j]);
      }
      printf("\n");
    }

    b = 0;
    while (!b) {
      __fpurge(stdin);
      scanf("%d", &input);
      if (*getBoardItem(&realBoard, input) == ' ') {
        b = 1;
        (*getBoardItem(&realBoard, input)) = getPiece(0);
      }
    }

    makeBoardArray(&realBoard, boardArr);
    b = 0;

    if (!estadoTablero(boardArr)) {
      for (i = 0; i < 9; i++) {
        if (*getBoardItem(&realBoard, i) == ' ') {
          tmpScore = getMoveScore(realBoard, i, 1, 1, 0).score;
          printf("%d: %d\n", i, tmpScore);


          if (!b) {
            b = 1;
            cpuInput = i;
            highestScore = tmpScore;
          } else if (tmpScore > highestScore) {
            cpuInput = i;
            highestScore = tmpScore;
          }
          // we get the highest score and if theres multiple, we pick a random one
          // or we just use the last one because im lazy
        }
      }
      (*getBoardItem(&realBoard, cpuInput)) = getPiece(1);
      makeBoardArray(&realBoard, boardArr);
    }
  }

  printf("final gametsate:\n");
  printBoard(realBoard);

  return 0;
}


