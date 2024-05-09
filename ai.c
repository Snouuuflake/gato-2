/**
 * @file ai.c
 * 
 * @brief contiene todas las funciones relacionadas a las jugadas de la computadora
 * 
 * @author Ricardo Sánchez Zepeda
 * 
 * @date 08/05/2024
*/

#include "header.h"

/**
 *@fn Para debugear- imprime un BOARDSTRUCT a la terminal
 *@param board BOARDSTRUCT que se va a imprimir
 *@return void
 */
void printBoard(BOARDSTRUCT board) {
  int i, j;
  char boardArr[9];
  makeBoardArray(&board, boardArr);
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      g_print("%d:%c ",(i*3)+j, boardArr[(i*3)+j]);
    }
    printf("\n");
  }
}


/**
 *@fn Función que regresa un apuntador a uno de los elementos de
 * un BOARDSTRUCT como si fuera un arreglo
 *@example *getBoardItem(b, 3) es similar a b[3] con un arreglo
 *@param board Apundaor al BOARDSTRUCT a acceder
 *@index index Índice (tal como en un arreglo) del elemento que
 * se desea acceder
 *@return Un apundaor a un elemento de board, o en caso de que
 * index < 0 || index > 8, NULL
 */
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

/**
 *@fn Copia cada elemento de un BOARDSTRUCT a un arreglo
 *@param board Apuntador a un BOARDSTRUCT
 *@param array Un arreglo
 *@return void
 */
void makeBoardArray(BOARDSTRUCT *board, char array[]) {
  int i;
  for (i = 0; i < 9; i++) {
    array[i] = (*getBoardItem(board, i));
  }
}

/**
 *@fn Regresa la pieza correspondiente al número de jugador
 *@param index Número de jugador
 *@return 'X' si index==0 y 'O' si index==1
 */
char getPiece(int index) {
  char arr[] = {'X','O'};
  return arr[index];
}


/**
 * for real this time
 * TODO: write this
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

void aiTurn(JUEGO *juego, int playerIndex) {
  BOARDSTRUCT board;
  int i;
  int chosenMove;
  int greatestScore;
  char chosenMoveExists; // boolean in spirit
  SCORESTRUCT tmpScore;

  chosenMove = 0; // just in case
  chosenMoveExists = 0;

  for (i = 0; i < 9; i++) {
    *getBoardItem(&board, i) = juego->actual->valor.tablero[i];
  }

  for (i = 0; i < 9; i++) {
    if (*getBoardItem(&board, i) == ' ') {
      tmpScore = getMoveScore(board, i, playerIndex, playerIndex, 0);

      if (!chosenMoveExists) {
        greatestScore = tmpScore.score;
        chosenMove = i;
        chosenMoveExists = 1;
      } else {
        if (tmpScore.score > greatestScore) {
          greatestScore = tmpScore.score;
          chosenMove = i;
        } else if (tmpScore.score == greatestScore) {
          if (rand() % 2) { 
            // si hay varios espacios con el valor maximo, aleatoriamente elije uno
            chosenMove = i;
          }
        }
      }
    }
  }


  // TODO: remove this maybe? its for debugging.
  (*getBoardItem(&board, chosenMove)) = getPiece(playerIndex);
  printBoard(board);
  g_print("chosenMove: %d\n", chosenMove);

  button_pressed(juego->botones[ chosenMove ], NULL, juego->gstructArr[ chosenMove ]);

  // this is for debugging and does not affect the real game in any way:
}