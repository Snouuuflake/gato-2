#include "header.h"

// no he cambiado la imagen del titulo porque no me gusta como se ve :P

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
    *getBoardItem(&board, i) = juego->actual->tablero[i];
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

  juego->actual = (ESTADO *) malloc(sizeof(ESTADO));
  juego->actual->estadoPartida = 0;
  juego->actual->turno = 0;
  juego->actual->sig = NULL;
  juego->actual->ant = NULL;

  for(i = 0; i < 9; i++) 
  {
    juego->gstructArr[i] = (void *) malloc(sizeof(GSTRUCT));

    juego->actual->tablero[i] = ' ';
  }

  for(i = 0; i < 2; i++) 
  {
    juego->jugadores[i].esCPU = 0;
    juego->jugadores[i].nombre[0] = 0;
  }

  return;
}

void setNewGame(JUEGO *juego, gboolean vsAI, gboolean hardMode, char jug1[], char jug2[])
{
  char *jugadores[2];

  int ran = rand() % 2;

  resetGame(juego);

  juego->actual->estadoPartida = 1;


  jugadores[0] = jug1;

  if(vsAI)
  {
    juego->jugadores[(ran + 1) % 2].esCPU = 1;
    jugadores[1] = AI_NAME;

    if(hardMode)
    {
      juego->hardMode = TRUE;

      displayHardMode(juego);
    }
  }
  else
  {
    jugadores[1] = jug2;
  }

  gtk_widget_show(juego->graficos.playerImg[0]);
  gtk_widget_show(juego->graficos.playerImg[1]);
  
  gtk_widget_destroy(juego->graficos.playingImg);
  
  juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m40[0]);
    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 0);
    gtk_widget_show(juego->graficos.playingImg);

  gtk_label_set_text(GTK_LABEL(juego->graficos.playerName[ran]), jugadores[0]);
  strcpy(juego->jugadores[ran].nombre, jugadores[0]);

  gtk_label_set_text(GTK_LABEL(juego->graficos.playerName[(ran + 1) % 2]), jugadores[1]);
  strcpy(juego->jugadores[(ran + 1) % 2].nombre, jugadores[1]);

  if(juego->jugadores[0].esCPU)
  {
    aiTurn(juego, juego->actual->turno);
    gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);
  }

  return;
}

void resetGame(JUEGO *juego)
{
  ESTADO *temp, *temp2;
  GSTRUCT *button;
  
  GdkColor color;

  int i = 0;

  gdk_color_parse("#DCDAD5", &color);

  juego->hardMode = FALSE;

  gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
  gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

  for(i = 0; i < 9; i++)
  {
    button = (GSTRUCT *)juego->gstructArr[i];
    
    gtk_widget_modify_bg(juego->botones[i], GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(juego->botones[i], GTK_STATE_INSENSITIVE, &color);

    gtk_widget_destroy(button->image);

    button->image = gtk_image_new_from_pixbuf(juego->graficos.m40[2]);
      gtk_container_add(GTK_CONTAINER(juego->botones[i]), button->image);
      gtk_widget_show(button->image);
  }

  for(i = 0; i < 2; i++)
  {
    gtk_widget_set_sensitive(juego->graficos.moveButtons[i], FALSE);

    gtk_widget_hide(juego->graficos.flames[i]);
    
    gtk_widget_hide(juego->graficos.playerImg[i]);
    gtk_label_set_text(GTK_LABEL(juego->graficos.playerName[i]), "\0");

    juego->jugadores[i].esCPU = 0;
  }

  gtk_widget_destroy(juego->graficos.playingImg);
  
  juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m40[2]);
    gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 0);
    gtk_widget_show(juego->graficos.playingImg);

  if(juego->actual != NULL)
  {
    while(juego->actual->ant != NULL)
    {
      juego->actual = juego->actual->ant;
    }

    juego->actual->estadoPartida = 0;
    temp = juego->actual->sig;

    while(temp != NULL)
    {
      temp2 = temp;
      temp = temp2->sig;

      free(temp2);
    }

    juego->actual->sig = NULL;
  }
  
  return;
}

void displayHardMode(JUEGO *juego)
{
  GdkColor color;

  int i = 0;
  
  gdk_color_parse("#790000", &color);

  gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
  gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

  for(i = 0; i < 2; i++)
  {
    gtk_widget_show(juego->graficos.flames[i]);
  }

  return;
}

void coppyBoardState(JUEGO *juego)
{
  GdkColor selected, unselected;

  GSTRUCT *button;

  int i = 0;

  gdk_color_parse("#DCDAD5", &unselected);
  gdk_color_parse("#A3A3A3", &selected);

  for(i = 0; i < 9; i++)
  {
    button = (GSTRUCT *)juego->gstructArr[i];

    gtk_widget_destroy(button->image);

    switch(juego->actual->tablero[i])
    {
      case 'X':
      {
        button->image = gtk_image_new_from_pixbuf(juego->graficos.m40[0]);
    
        gtk_widget_modify_bg(juego->botones[i], GTK_STATE_NORMAL, &selected);
        gtk_widget_modify_bg(juego->botones[i], GTK_STATE_INSENSITIVE, &selected);
        break;
      }
      case 'O':
      {
        button->image = gtk_image_new_from_pixbuf(juego->graficos.m40[1]);
        
        gtk_widget_modify_bg(juego->botones[i], GTK_STATE_NORMAL, &selected);
        gtk_widget_modify_bg(juego->botones[i], GTK_STATE_INSENSITIVE, &selected);
        break;
      }
      default:
      {
        button->image = gtk_image_new_from_pixbuf(juego->graficos.m40[2]);

        gtk_widget_modify_bg(juego->botones[i], GTK_STATE_NORMAL, &unselected);
        gtk_widget_modify_bg(juego->botones[i], GTK_STATE_INSENSITIVE, &unselected);
        break;
      }
    }

    gtk_container_add(GTK_CONTAINER(juego->botones[i]), button->image);
    gtk_widget_show(button->image);
  }

  return;
}


/**
 * Nueva IA
 * Que la ia tire 2 veces en modo dificil
 * Musica del modo dificil
 * Manejar historial para partidas contra ia
 * 
 * Pantalla de victoria
 * Pantalla de guardado
 * Pantalla al presionar un botón sin partida
 * Pantalla de créditos
 * PAntalla de como jugar
 * 
 * Cargar y guardar archivos
 * 
*/