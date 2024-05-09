/**
 * @file events.c
 * 
 * @brief Contiene todas las funciones que son llamadas en eventos de los widgets
 * 
 * @author Ricardo Sánchez Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @date 08/05/2024
*/

#include "header.h"

/**
 * Al presionar un botón del tablero
 * 
 * @author Ricardo Sánchez Zepeda
 * @author Luis Julián Zamora Treiño
 * 
 * @param *eventbox La caja de eventos de la que se llama
 * @param *event El tipo de evento que se llama
 * @param data La información del botón
 * 
 * @return void
*/
void button_pressed(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  LISTA *nuevo, *temp, *temp2;
  GdkColor color;

  char players[] = "XO";

  char gameEnded = 0;

  // solo actua si está vacío el espacio y la partida está en cursp
  if(buttondata->juego->actual->valor.tablero[buttondata->id] == ' ' && buttondata->juego->actual->valor.estadoPartida > 0)
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

    // desactiva el bottón de siguiente y activa el de anterior
    gtk_widget_set_sensitive(buttondata->juego->graficos.moveButtons[1], FALSE);
    gtk_widget_set_sensitive(buttondata->juego->graficos.moveButtons[0], TRUE);

    nuevo = (LISTA*) malloc(sizeof(LISTA)); // <-- crea un nueva instancia de historial
    nuevo->ant = buttondata->juego->actual; // <-- logicamente su anterior era el turno actual
    nuevo->sig = NULL; // <-- no tiene siguiente
    buttondata->juego->actual->sig = nuevo; // <-- el siguiente del actual es el nuevo elemento
    
    nuevo->valor = buttondata->juego->actual->valor;


    if (buttondata->juego->doubleTurn == TRUE) { // permite que se repita el jugador para el modo dificil
      nuevo->valor.turno = buttondata->juego->actual->valor.turno;
    } else {
      nuevo->valor.turno = (buttondata->juego->actual->valor.turno + 1) % 2;
    }

    if (buttondata->juego->jugadores[nuevo->valor.turno].esCPU || buttondata->juego->hardMode) { 
      // stops user from changing the cpu's moves
      // hercules is a just god and does not allow cheating in a true challenge
      nuevo->valor.playable = FALSE;
    } else {
      nuevo->valor.playable = TRUE;
    }

    // cambia el color (en caso de tiro de ia)
    gdk_color_parse("#A3A3A3", &color);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);

    // cambia el valor en el arreglo
    nuevo->valor.tablero[buttondata->id] = players[buttondata->juego->actual->valor.turno];

    // cambia la imagen del botón seleccionado
    gtk_widget_destroy(buttondata->image);

    buttondata->image = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[buttondata->juego->actual->valor.turno]);
      gtk_container_add(GTK_CONTAINER(eventbox), buttondata->image);
      gtk_widget_show(buttondata->image);

    // cambia la imagen del jugador actual
    gtk_widget_destroy(buttondata->juego->graficos.playingImg);

    buttondata->juego->graficos.playingImg = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[nuevo->valor.turno]);
    gtk_box_pack_start(GTK_BOX(buttondata->juego->graficos.playingBox), buttondata->juego->graficos.playingImg, FALSE, TRUE, 0);
    gtk_widget_show(buttondata->juego->graficos.playingImg);

    buttondata->juego->actual = nuevo;

    // determina el estado del juego
    gameEnded = estadoTablero(nuevo->valor.tablero);

    // si el juego ha temrinado
    if (gameEnded) {
      if (buttondata->juego->doubleTurn == TRUE) {
        buttondata->juego->actual->valor.turno =  (buttondata->juego->actual->valor.turno + 1) % 2;
      }

      // elimina la imagen del jugador actual
      gtk_widget_destroy(buttondata->juego->graficos.playingImg);

      buttondata->juego->graficos.playingImg = gtk_image_new_from_pixbuf(buttondata->juego->graficos.m40[2]);
      gtk_box_pack_start(GTK_BOX(buttondata->juego->graficos.playingBox), buttondata->juego->graficos.playingImg, FALSE, TRUE, 0);
      gtk_widget_show(buttondata->juego->graficos.playingImg);

      // muestra la pandalla final y cambia el estado de la partida
      endPopup(buttondata->juego, gameEnded);

      buttondata->juego->actual->valor.estadoPartida = -1;
    }

    // si el siguiente jufador es la computadora, hace que la computadora realice un turno
    if (buttondata->juego->jugadores[nuevo->valor.turno].esCPU && !gameEnded && !buttondata->juego->doubleTurn) {
      if (buttondata->juego->hardMode) {
        buttondata->juego->doubleTurn = TRUE;
        aiTurn(buttondata->juego, buttondata->juego->actual->valor.turno);

        if (buttondata->juego->actual->valor.estadoPartida != -1) {
          aiTurn(buttondata->juego, buttondata->juego->actual->valor.turno);
        }
      } else {
        aiTurn(buttondata->juego, buttondata->juego->actual->valor.turno);
      }

    }
  }
  // si no hay una partida en curso, muestra la ventana de preparación de partida
  else if(!buttondata->juego->actual->valor.estadoPartida)
  {
    nuevaPartida(NULL, buttondata->juego);
  }

  buttondata->juego->doubleTurn = FALSE;
  return;
}

/**
 * Al pasar el cursor sobre el botón
 * 
 * @author Luis Julián Zamora Treiño
 * 
 * @param *eventbox La caja de eventos de la que se llama
 * @param *event El tipo de evento que se llama
 * @param data La información del botón
 * 
 * @return void
*/
void button_hover(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  GdkColor color;

  // cambia su color a un gris oscuro si este está vacío y la partida no ha terminado
  if(buttondata->juego->actual->valor.tablero[buttondata->id] == ' ' && buttondata->juego->actual->valor.estadoPartida >= 0)
  {
    gdk_color_parse("#A3A3A3", &color);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
  }

  return;
}

/**
 * Al quitar el cursor del botón
 * 
 * @author Luis Julián Zamora Treiño
 * 
 * @param *eventbox La caja de eventos de la que se llama
 * @param *event El tipo de evento que se llama
 * @param data La información del botón
 * 
 * @return void
*/
void button_leave(GtkWidget *eventbox, GdkEventButton *event, gpointer data)
{
  GSTRUCT *buttondata = (GSTRUCT *) data;
  GdkColor color;

  // cambia el color a un gris claro si es que está vacío
  if(buttondata->juego->actual->valor.tablero[buttondata->id] == ' ')
  {
    gdk_color_parse("#DCDAD5", &color);
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &color);
  }

  return;
}

/**
 * Al cambiar la selección de los radio buttons de la configuración de juego nuevo
 * Este evento solo está sobre el botón de jugar contra jugador
 * 
 * @author Luis Julián Zamora Treiño
 * 
 * @param *widget El widget que genera el evento
 * @param data La información del juego que se está creando
 * 
 * @return void
*/
void radio_changed(GtkWidget *widget, gpointer data)
{
  NGDATA *datos = (NGDATA *)data;

  // si el botón pasó a estar activo
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
  {
    // desactiva el botón del modo dificil de la computadora y no permite presionarlo y activa la entrybox del usuario2
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(datos->chb), FALSE);
    gtk_widget_set_sensitive(datos->chb, FALSE);
    gtk_widget_set_sensitive(datos->players[1], TRUE);
  }
  // si se desactivó el botón
  else
  {
    // habilita la activación del modo dificil y desactiva la entrybox del usuario 2, vaciandola en el proceso
    gtk_widget_set_sensitive(datos->chb, TRUE);
    gtk_widget_set_sensitive(datos->players[1], FALSE);
    gtk_entry_set_text(GTK_ENTRY(datos->players[1]), "\0");
  }

  return;
}

/**
 * Al presionar el botón para regresar un turno
 * 
 * @author Ricardo Sánchez Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget el botón
 * @param data la info de la partida
 * 
 * @return void
*/
void lastTurn(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *) data;
  gboolean canSwitch; 

  // si se puede recorrer
  if(datos->actual != NULL && datos->actual->ant != NULL)
  {
    // si el anterior no es una casilla jugable
    if ( !(datos->actual->ant->valor.playable) ) {
      // si el anterior anterior existe
      if (datos->actual->ant->ant != NULL ) {
        // recorre la partida
        datos->actual = datos->actual->ant->ant;
        canSwitch = TRUE;
      } else {
        canSwitch = FALSE;
      }
    } else {
      // si es una casilla jugable
      datos->actual = datos->actual->ant;
      canSwitch = TRUE;
    }

    // si puede recorrerse y la aprtida no está en modo dificil    
    if (canSwitch && !(datos->hardMode)) {
      // establece la partida en el nuevo punto
      coppyBoardState(datos);

      // activa o desactiva los botones si es necesario
      gtk_widget_set_sensitive(datos->graficos.moveButtons[1], TRUE);

      if(datos->actual->ant == NULL)
      {
        gtk_widget_set_sensitive(datos->graficos.moveButtons[0], FALSE);
      }

      // cambia la imagend el jugador actual
      gtk_widget_destroy(datos->graficos.playingImg);

      if(datos->actual->valor.estadoPartida > 0)
      {
        datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[datos->actual->valor.turno]);
      }
      else
      {
        datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[2]);
      }

      gtk_box_pack_start(GTK_BOX(datos->graficos.playingBox), datos->graficos.playingImg, FALSE, TRUE, 0);
      gtk_widget_show(datos->graficos.playingImg);
    }
  }

  return;
}

/**
 * Al presionar el botón para avanzar un turno
 * 
 * @author Ricardo Sánchez Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget el botón
 * @param data la info de la partida
 * 
 * @return void
*/
void nextTurn(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *) data;
  gboolean canSwitch;

  // si puede avanzar
  if(datos->actual != NULL && datos->actual->sig != NULL)
  {
    // si el siguiente turno no es jugable
    if ( !(datos->actual->sig->valor.playable) ) {
        // si el siguiente siguiente turno existe
        if (datos->actual->sig->sig != NULL ) {
          datos->actual = datos->actual->sig->sig;
          canSwitch = TRUE;
        } else {
          canSwitch = FALSE;
        }
    } else {
      datos->actual = datos->actual->sig;
      canSwitch = TRUE;
    }

    // si se puede cambiar
    if (canSwitch && !(datos->hardMode)) {
      // copia el nuevo tablero y actualiza el estado de los botones del historial
      coppyBoardState(datos);

      gtk_widget_set_sensitive(datos->graficos.moveButtons[0], TRUE);

      if(datos->actual->sig == NULL)
      {
        gtk_widget_set_sensitive(datos->graficos.moveButtons[1], FALSE);
      }

      // cambia la imagen del jugador actual
      gtk_widget_destroy(datos->graficos.playingImg);

      if(datos->actual->valor.estadoPartida > 0)
      {
        datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[datos->actual->valor.turno]);
      }
      else
      {
        datos->graficos.playingImg = gtk_image_new_from_pixbuf(datos->graficos.m40[2]);
      }

      gtk_box_pack_start(GTK_BOX(datos->graficos.playingBox), datos->graficos.playingImg, FALSE, TRUE, 0);
      gtk_widget_show(datos->graficos.playingImg);
    }
  }

  return;
}

/**
 * Si se cierra la ventana de como jugar
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget el widget que generó el evento
 * @param data NULL
 * 
 * @return void
*/
void assistant_close(GtkWidget *widget, gpointer data)
{
  // destruye la ventana
  gtk_widget_destroy(widget);

  return;
}

/**
 * Si se destruye la ventana de como jugar
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget el widget que generó el evento
 * @param data información de la aprtida
 * 
 * @return void
*/
void assistant_destroy(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *) data;

  // activa la ventana principal
  gtk_widget_set_sensitive(datos->graficos.window, TRUE);

  return;
}

/**
 * Al seleccionar cargar partida
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget El widget que invocó a la función
 * @param data Puntero a toda la información del juego
 * 
 * @return void
*/
void cargarPartida(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;

  // ejecuta la ventana de carga de partida
  loadGame(datos, datos->graficos.window);

  return;
}

/**
 * Al seleccionar guardar partida
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget El widget que invocó a la función
 * @param data Puntero a toda la información del juego
 * 
 * @return void
*/
void guardarPartida(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;

  // ejecuta la ventana de guardado de juego
  saveGame(datos, datos->graficos.window);

  return;
}

/**
 * Al cerrar la ventana principal
 * 
 * @author Ricardo Sánchez Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @param *window la ventana principal
 * @param data la info de la aprtida
 * 
 * @return void
*/
void StopTheApp(GtkWidget *window, gpointer data)
{
  JUEGO *juego = (JUEGO *) data;

  LISTA *temp;

  int i = 0;

  // libera la memoria con la info de los botones
  for (i = 0; i < 9; i++) 
  {
    free(juego->gstructArr[i]);
  }

  // libera la memoria del historial de ser necesario
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

  // finaliza el bucle de gtk
  gtk_main_quit();

  return;
}