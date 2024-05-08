#include "header.h"

void button_pressed(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  ESTADO *nuevo, *temp, *temp2;
  GdkColor color;

  int i = 0;

  char players[] = "XO";

  char gameEnded = 0;

  // solo actua si está vacío el espacio
  if(buttondata->juego->actual->tablero[buttondata->id] == ' ' && buttondata->juego->actual->estadoPartida > 0)
  {
    // siempre borra los turnos siguientes
    temp = buttondata->juego->actual->sig;

    while(temp != NULL)
    {
      temp2 = temp;
      temp = temp2->sig;

      free(temp2);
    }

    buttondata->juego->actual->sig = NULL;
    gtk_widget_set_sensitive(buttondata->juego->graficos.moveButtons[1], FALSE);
    gtk_widget_set_sensitive(buttondata->juego->graficos.moveButtons[0], TRUE);

    nuevo = (ESTADO*) malloc(sizeof(ESTADO)); // <-- crea un nueva instancia de historial
    nuevo->ant = buttondata->juego->actual; // <-- logicamente su anterior era el turno actual
    buttondata->juego->actual->sig = nuevo; // <-- el siguiente del actual es el nuevo elemento
    nuevo->turno = (buttondata->juego->actual->turno + 1) % 2;
    nuevo->estadoPartida = buttondata->juego->actual->estadoPartida;
    nuevo->sig = NULL; // <-- no tiene siguiente

    // copia el tablero
    for(i = 0; i < 9; i++)
    {
      nuevo->tablero[i] = buttondata->juego->actual->tablero[i];
    }

    // cambia el color (en caso de tiro de ia)
    gdk_color_parse("#A3A3A3", &color);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);

    // cambia el valor en el arreglo
    nuevo->tablero[buttondata->id] = players[buttondata->juego->actual->turno];

    gtk_widget_destroy(buttondata->image);

    buttondata->image = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[buttondata->juego->actual->turno]);
      gtk_container_add(GTK_CONTAINER(eventbox), buttondata->image);
      gtk_widget_show(buttondata->image);

    gtk_widget_destroy(buttondata->juego->graficos.playingImg);

    buttondata->juego->graficos.playingImg = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[nuevo->turno]);
      gtk_box_pack_start(GTK_BOX(buttondata->juego->graficos.playingBox), buttondata->juego->graficos.playingImg, FALSE, TRUE, 0);
      gtk_widget_show(buttondata->juego->graficos.playingImg);
    
    buttondata->juego->actual = nuevo;

    gameEnded = estadoTablero(nuevo->tablero);

    if (gameEnded) {
      gtk_widget_destroy(buttondata->juego->graficos.playingImg);

      buttondata->juego->graficos.playingImg = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[2]);
        gtk_box_pack_start(GTK_BOX(buttondata->juego->graficos.playingBox), buttondata->juego->graficos.playingImg, FALSE, TRUE, 0);
        gtk_widget_show(buttondata->juego->graficos.playingImg);

      endPopup(buttondata->juego, gameEnded);

      buttondata->juego->actual->estadoPartida = -1;
      g_print("Juego terminó. Estado tablero: %c\n", gameEnded);
    }

    // TODO: move the vairables up
    if (buttondata->juego->jugadores[nuevo->turno].esCPU && !gameEnded) {
      aiTurn(buttondata->juego, buttondata->juego->actual->turno);
    }
  }
  else if(!buttondata->juego->actual->estadoPartida)
  {
    nuevaPartida(NULL, buttondata->juego);
  }

  return;
}

void button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  GdkColor color;

  if(buttondata->juego->actual->tablero[buttondata->id] == ' ' && buttondata->juego->actual->estadoPartida >= 0)
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

  if(buttondata->juego->actual->tablero[buttondata->id] == ' ')
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
  JUEGO *juego = (JUEGO *) data;

  ESTADO *temp;

  int i = 0;

  for (i = 0; i < 9; i++) 
  {
    free(juego->gstructArr[i]);
  }

  if(juego->actual != NULL)
  {
    while(juego->actual->ant != NULL)
    {
      juego->actual = juego->actual->ant;
    }

    while(juego->actual != NULL)
    {
      temp = juego->actual;
      juego->actual = temp->sig;

      free(temp);
    }
  }

  gtk_main_quit();

  return;
}

void lastTurn(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *) data;

  if(datos->actual != NULL && datos->actual->ant != NULL)
  {
    datos->actual = datos->actual->ant;

    coppyBoardState(datos);

    gtk_widget_set_sensitive(datos->graficos.moveButtons[1], TRUE);

    if(datos->actual->ant == NULL)
    {
      gtk_widget_set_sensitive(datos->graficos.moveButtons[0], FALSE);
    }

    gtk_widget_destroy(datos->graficos.playingImg);

    if(datos->actual->estadoPartida > 0)
    {
      datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[datos->actual->turno]);
    }
    else
    {
      datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[2]);
    }

    gtk_box_pack_start(GTK_BOX(datos->graficos.playingBox), datos->graficos.playingImg, FALSE, TRUE, 0);
    gtk_widget_show(datos->graficos.playingImg);
  }

  return;
}

void nextTurn(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *) data;

  if(datos->actual != NULL && datos->actual->sig != NULL)
  {
    datos->actual = datos->actual->sig;

    coppyBoardState(datos);

    gtk_widget_set_sensitive(datos->graficos.moveButtons[0], TRUE);

    if(datos->actual->sig == NULL)
    {
      gtk_widget_set_sensitive(datos->graficos.moveButtons[1], FALSE);
    }

    gtk_widget_destroy(datos->graficos.playingImg);

    if(datos->actual->estadoPartida > 0)
    {
      datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[datos->actual->turno]);
    }
    else
    {
      datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[2]);
    }
    
    gtk_box_pack_start(GTK_BOX(datos->graficos.playingBox), datos->graficos.playingImg, FALSE, TRUE, 0);
    gtk_widget_show(datos->graficos.playingImg);
  }

  return;
}