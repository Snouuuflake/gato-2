#include "header.h"

// no he cambiado la imagen del titulo porque no me gusta como se ve :P

void initJuego(JUEGO *juego);

void aiTurn(JUEGO *juego, int playerIndex);

int main(int argc, char *argv[])
{
  // partida
  JUEGO juego;

  srand(time(NULL));

  // inicia gtk
  gtk_init(&argc, &argv);

  initJuego(&juego);

  loadMainWindow(&juego);

  gtk_main();

  return 0;
}

void button_pressed(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  GdkColor color;

  char players[] = "XO";

  char gameEnded = 0;

  g_print("5\n");
  // solo actua si está vacío el espacio
  if(buttondata->juego->tablero[buttondata->id] == ' ' && buttondata->juego->estadoPartida > 0)
  {
    // cambia el color (en caso de tiro de ia)
    gdk_color_parse("#A3A3A3", &color);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);

    // cambia el valor en el arreglo
    buttondata->juego->tablero[buttondata->id] = players[buttondata->juego->jugadorActual];

    gtk_widget_destroy(buttondata->image);

    buttondata->image = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[buttondata->juego->jugadorActual]);
      gtk_container_add(GTK_CONTAINER(eventbox), buttondata->image);
      gtk_widget_show(buttondata->image);

  g_print("6\n");
    // cambia el jugador
    buttondata->juego->jugadorActual = (buttondata->juego->jugadorActual + 1) % 2;

    gtk_widget_destroy(buttondata->juego->graficos.playingImg);

    buttondata->juego->graficos.playingImg = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[buttondata->juego->jugadorActual]);
      gtk_box_pack_start(GTK_BOX(buttondata->juego->graficos.playingBox), buttondata->juego->graficos.playingImg, FALSE, TRUE, 0);
      gtk_widget_show(buttondata->juego->graficos.playingImg);

  g_print("7\n");
    gameEnded = estadoTablero(buttondata->juego->tablero);
    if (gameEnded) {
      // TODO: hacer algo interesante si alguien ganó
      // @luis 
      // Me pongo a hacerlo en lo que sigues con el programa 
      gtk_widget_destroy(buttondata->juego->graficos.playingImg);

      buttondata->juego->graficos.playingImg = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[2]);
        gtk_box_pack_start(GTK_BOX(buttondata->juego->graficos.playingBox), buttondata->juego->graficos.playingImg, FALSE, TRUE, 0);
        gtk_widget_show(buttondata->juego->graficos.playingImg);

      buttondata->juego->estadoPartida = -1;
      g_print("Juego terminó. Estado tablero: %c\n", gameEnded);
    }

    // TODO: move the vairables up
    if (buttondata->juego->jugadores[ buttondata->juego->jugadorActual ].esCPU && !gameEnded) {
      aiTurn(buttondata->juego, buttondata->juego->jugadorActual);
    }
  }
  else if(!buttondata->juego->estadoPartida)
  {
    nuevaPartida(NULL, buttondata->juego);
  }

  g_print("8\n");
  return;
}

void button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  GdkColor color;

  if(buttondata->juego->tablero[buttondata->id] == ' ' && buttondata->juego->estadoPartida >= 0)
  {
    gdk_color_parse("#A3A3A3", &color);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
  }

  return;
}

void button_leave(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  GdkColor color;

  if(buttondata->juego->tablero[buttondata->id] == ' ')
  {
    gdk_color_parse("#DCDAD5", &color);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
  }

  return;
}

void radio_changed(GtkWidget *widget, gpointer data)
{
  NGDATA *datos = (NGDATA *)data;

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(datos->chb), FALSE);
    gtk_widget_set_sensitive(datos->chb, FALSE);
    gtk_widget_set_sensitive(datos->players[1], TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(datos->chb, TRUE);
    gtk_widget_set_sensitive(datos->players[1], FALSE);
    gtk_entry_set_text(GTK_ENTRY(datos->players[1]), "\0");
  }

  return;
}

void StopTheApp(GtkWidget *window, gpointer data)
{
  JUEGO *juego;
  int i = 0;

  juego = (JUEGO *) data;

  for (i = 0; i < 9; i++) 
  {
    free(juego->gstructArr[i]);
  }

  gtk_main_quit();

  return;
}

void aiTurn(JUEGO *juego, int playerIndex) {
  BOARDSTRUCT board;
  int i;
  int chosenMove;
  int greatestScore;
  char chosenMoveExists; // boolean in spirit
  SCORESTRUCT tmpScore;

  // @richie -> si la ia empieza la partida, te deja tirar por ella xd
  // comedia

  chosenMove = 0; // just in case
  chosenMoveExists = 0;

  g_print("1\n");
  for (i = 0; i < 9; i++) {
    *getBoardItem(&board, i) = juego->tablero[i];
  }

  g_print("2\n");
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

  g_print("3\n");

  (*getBoardItem(&board, chosenMove)) = getPiece(playerIndex);
  printBoard(board);
  g_print("chosenMove: %d\n", chosenMove);

  button_pressed(juego->botones[ chosenMove ], NULL, juego->gstructArr[ chosenMove ]);

  // this is for debugging and does not affect the real game in any way:
  g_print("4\n");
}

void initJuego(JUEGO *juego) 
{
  int i = 0;

  for(i = 0; i < 9; i++) 
  {
    juego->gstructArr[i] = (void *) malloc(sizeof(GSTRUCT));
  }

  for(i = 0; i < 9; i++) 
  {
    juego->tablero[i] = ' ';
  }

  juego->inicio = NULL;
  juego->estadoPartida = 0;

  for(i = 0; i < 2; i++) 
  {
    juego->jugadores[i].esCPU = 0;
  }

  juego->jugadorActual = 0;

  return;
}

void restartJuego(JUEGO *juego, gboolean vsAI, gboolean hardMode, char jug1[], char jug2[])
{
  GSTRUCT *temp;
  ESTADO *movil;

  GdkColor color;

  char *jugadores[2];

  int ran = rand() % 2;
  int i = 0;

  gdk_color_parse("#DCDAD5", &color);

  juego->jugadorActual = 0;
  juego->estadoPartida = 1;
  juego->hardMode = FALSE;

  for(i = 0; i < 9; i++)
  {
    temp = (GSTRUCT *)juego->gstructArr[i];
    
    gtk_widget_modify_bg(juego->botones[i], GTK_STATE_NORMAL, &color);

    gtk_widget_destroy(temp->image);

    temp->image = gtk_image_new_from_pixbuf(juego->graficos.m40[1]);
      gtk_widget_show(temp->image);

    juego->tablero[i] = ' ';
  }

  juego->jugadores[ran].esCPU = 0;
  juego->jugadores[(ran + 1) % 2].esCPU = 0;
  jugadores[0] = jug1;

  if(vsAI)
  {
    juego->jugadores[(ran + 1) % 2].esCPU = 1;
    jugadores[1] = AI_NAME;

    if(hardMode)
    {
      juego->hardMode = TRUE;
    }
  }
  else
  {
    jugadores[1] = jug2;
  }
  
  gtk_widget_destroy(juego->graficos.playingImg);
  
  juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m40[0]);
    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 0);
    gtk_widget_show(juego->graficos.playingImg);

  gtk_label_set_text(GTK_LABEL(juego->graficos.playerName[ran]), jugadores[0]);
  gtk_label_set_text(GTK_LABEL(juego->graficos.playerName[(ran + 1) % 2]), jugadores[1]);

  while(juego->inicio != NULL)
  {
    movil = juego->inicio;
    juego->inicio = movil->sig;
    
    free(movil);
  }

  if(juego->jugadores[0].esCPU)
  {
    aiTurn(juego, juego->jugadorActual);
  }

  return;
}


/**
 * Hacer funcionar las opciones del menu
 * modo de juego dificil
 * guardar archivos
 * cargar archivos
 * historial de movimientos
*/
