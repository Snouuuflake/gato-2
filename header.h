/**
 * Area funcional (R)
*/
#include <gtk-2.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AI_NAME "Hercules.raw"

/**
 * La ficha de cada jugador se infiere de su índice.
 */
typedef struct def_jugador {
  char nombre[256];
  char esCPU;
} JUGADOR;

/**
 * Estado o "judada"
 * Formato: "XXOXXOXOO"
 */
typedef struct def_estado {
  char tablero[9];
  int turno;
  int estadoPartida;
  struct def_estado *sig;
  struct def_estado *ant;
} ESTADO;

typedef struct def_gamewidgets // (L)
{
  GtkWidget *window;
  GtkWidget *playingImg;
  GtkWidget *playingBox;
  GtkWidget *moveButtons[2];
  GtkWidget *playerImg[2];
  GtkWidget *playerName[2];
  GtkWidget *flames[2];
  GdkPixbuf *m20[3];
  GdkPixbuf *m40[3];
  GdkPixbuf *hercules;
} GameWidgets;

typedef struct def_juego {
  GtkWidget *botones[9];
  void *gstructArr[9];
  ESTADO *actual;
  JUGADOR jugadores[2];

  gboolean hardMode;
  GameWidgets graficos;
} JUEGO;

/**
 * Argumento del callback de los botones
 * del tablero
 *
 * Cuando presionas un botón, manda esta
 * estructura, con el id del botón que
 * se presionó.
 *
 * Cuando la ia tira, cambia el valor de
 * id al botón que quiere presionar y
 * vuelve a llamar la función de cuando
 * se presiona un botón.
 */
typedef struct def_gstruct {
  JUEGO *juego;
  GtkWidget *image; // <-- L
  int id;
} GSTRUCT;

/**
 * ai.c (R)
 */
/**
 * Esencialmente una forma de mandar un
 * arreglo como variable y no apuntador
 * if that makes sense
 */
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
} SCORESTRUCT; // minimax score

// new game data (L)
typedef struct def_ngdata
{
  JUEGO *datos;
  GtkWidget *players[2];
  GtkWidget *radioButtons[2];
  GtkWidget *chb;
  GtkWidget *eLabel;
} NGDATA;

char estadoTablero(char tab[9]);

void aiTurn(JUEGO *juego, int playerIndex);

void printBoard(BOARDSTRUCT board);
char *getBoardItem(BOARDSTRUCT *board, int index);
void makeBoardArray(BOARDSTRUCT *board, char array[]);
char getPiece(int index);
SCORESTRUCT getMoveScore(BOARDSTRUCT board, int square, int piece, int ogpiece, int depth);

void loadMainWindow(JUEGO *juego);

void nuevaPartida(GtkWidget *widget, gpointer data);
void terminarPartida(GtkWidget *widget, gpointer data);

void guardarPartida(GtkWidget *widget, gpointer data);
void cargarPartida(GtkWidget *widget, gpointer data);

gint saveGame(JUEGO *datos, GtkWidget *parent);
gint loadGame(JUEGO *datos, GtkWidget *parent);

void comoJugar(GtkWidget *widget, gpointer data);
void creditos(GtkWidget *widget, gpointer data);

void endPopup(JUEGO *juego, char endState);

void displayHardMode(JUEGO *juego);

void button_pressed(GtkWidget *eventbox, GdkEventButton *event, gpointer data);
void button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data);
void button_leave(GtkWidget *eventbox, GdkEventButton *event, gpointer data);

void radio_changed(GtkWidget *widget, gpointer data);

void initJuego(JUEGO *juego);
void resetGame(JUEGO *juego);
void setNewGame(JUEGO *juego, gboolean vsAI, gboolean hardMode, char jug1[], char jug2[]);

void StopTheApp(GtkWidget *window, gpointer data);

/**
 * historial.c (M)
 */

void lastTurn(GtkWidget *widget, gpointer data);
void nextTurn(GtkWidget *widget, gpointer data);

void coppyBoardState(JUEGO *juego);