/**
 * @file main.c
 * 
 * @brief contiene la función principal junto con otras funciones de respaldo
 * 
 * @author Mariano Malouly Orozco
 * @author Ricardo Sanchéz Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @date 08/05/2024
*/

#include "header.h"

int main(int argc, char *argv[])
{
  // partida
  JUEGO juego;

  srand(time(NULL));

  // inicia gtk
  gtk_init(&argc, &argv);

  // inicializa la estructura de juego
  initJuego(&juego);

  // carga la ventana principal
  loadMainWindow(&juego);

  gtk_main();

  return 0;
}

/**
 * Inicializa la estructura de juego
 * 
 * @author Mariano Malouly Orozco
 * @author Ricardo Sanchéz Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @param *juego la estructura a inicializar
 * 
 * @return void
*/
void initJuego(JUEGO *juego) 
{
  int i = 0;

  // inicia el historial con un tablero vacío
  juego->actual = (LISTA *) malloc(sizeof(LISTA));
  juego->actual->valor.estadoPartida = 0;
  juego->actual->valor.turno = 0;
  juego->actual->valor.playable = FALSE;
  juego->actual->sig = NULL;
  juego->actual->ant = NULL;

  juego->doubleTurn = FALSE;

  // aloca memoria para la información de los botones
  for(i = 0; i < 9; i++) 
  {
    juego->gstructArr[i] = (void *) malloc(sizeof(GSTRUCT));

    juego->actual->valor.tablero[i] = ' ';
  }

  // establece los valores principales de losjugadores
  for(i = 0; i < 2; i++) 
  {
    juego->jugadores[i].esCPU = 0;
    juego->jugadores[i].nombre[0] = 0;
  }

  return;
}

/**
 * Borra toda la información de la partida, dejando la pantalla de la misma forma que al iniciar el programa
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *juego la información de la partida
 * 
 * @return void
*/
void resetGame(JUEGO *juego)
{
  LISTA *temp, *temp2;
  GSTRUCT *button;
  
  GdkColor color;

  int i = 0;

  gdk_color_parse("#DCDAD5", &color);

  juego->hardMode = FALSE;

  stopMusic();

  // reestablece todos los elementos gráficos de la ventana
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

  // libera el historial de movimientos
  if(juego->actual != NULL)
  {
    while(juego->actual->ant != NULL)
    {
      juego->actual = juego->actual->ant;
    }

    juego->actual->valor.estadoPartida = 0;
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

/**
 * Inicia una nueva partida con la información de los argumentos
 * 
 * @author Ricardo Sánchez Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @param *juego la informacion de la partida a modificar
 * @param vsAI si se va a enfrentar a una computadora
 * @param hardMode si la dificultad va a ser dificil
 * @param jug1[] el nombre del primer jugador
 * @param jug2[] el nombre del segundo jugador
 * 
 * @return void
*/
void setNewGame(JUEGO *juego, gboolean vsAI, gboolean hardMode, char jug1[], char jug2[])
{
  char *jugadores[2];

  int ran = rand() % 2;

  // reinicia el tablero
  resetGame(juego);

  juego->actual->valor.estadoPartida = 1;

  jugadores[0] = jug1;

  // si se enfrenta a una computadora
  if(vsAI)
  {
    // establece a un jugador como la computadora
    juego->jugadores[(ran + 1) % 2].esCPU = 1;
    jugadores[1] = AI_NAME;

    // si se elige modo dificil, activa los valores necesarios
    if(hardMode)
    {
      juego->hardMode = TRUE;

      displayHardMode(juego);

      startMusic();
    }
  }
  // si no, establece a ambos jugadores como usuarios
  else
  {
    jugadores[1] = jug2;
  }

  // muestra los widgets necesarios
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

  // si empieza la computadora
  if(juego->jugadores[0].esCPU)
  {
    aiTurn(juego, juego->actual->valor.turno);
    gtk_widget_set_sensitive(juego->graficos.moveButtons[0], FALSE);
  }

  return;
}

/**
 * Hace visibles los elementos del modo dificil
 * 
 * @author Ricardo Sánchez Zepeda
 * @author Luis Julián Zamora Treviño
 * 
 * @param *juego la información del juego
 * 
 * @return void
*/
void displayHardMode(JUEGO *juego)
{
  GdkColor color;

  int i = 0;
  
  // cambia el color del fondo
  gdk_color_parse("#790000", &color);

  gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_NORMAL, &color);
  gtk_widget_modify_bg(juego->graficos.window, GTK_STATE_INSENSITIVE, &color);

  // muestra las llamas
  for(i = 0; i < 2; i++)
  {
    //gtk_widget_show(juego->graficos.flames[i]);
  }

  return;
}

/**
 * Copia en el tablero el estado del arreglo de caracteres del tablero interno
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *juego la información de la aprtida
 * 
 * @return void
*/
void coppyBoardState(JUEGO *juego)
{
  GdkColor selected, unselected;

  GSTRUCT *button;

  int i = 0;

  // color de botones activos o inactivos
  gdk_color_parse("#DCDAD5", &unselected);
  gdk_color_parse("#A3A3A3", &selected);

  // para los 9 botones
  for(i = 0; i < 9; i++)
  {
    button = (GSTRUCT *)juego->gstructArr[i];

    // elimina la imagen del botón
    gtk_widget_destroy(button->image);

    // segun el tablero interno
    switch(juego->actual->valor.tablero[i])
    {
      // si el boton tiene una X
      case 'X':
      {
        // establece su imagen y su fondo  respectivamente
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

    // muestra el icono del boton
    gtk_container_add(GTK_CONTAINER(juego->botones[i]), button->image);
    gtk_widget_show(button->image);
  }

  return;
}

/**
 * Establece a los jugadores y jugador actual segun la información de la partida
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *juego la info de la partida
 * 
 * @return void
*/
void coppyPlayersState(JUEGO *juego)
{
  int i = 0;

  // muestra la simagenes y los nombres de los jugadores
  for(i = 0; i < 2; i++)
  {
    gtk_widget_show(juego->graficos.playerImg[i]);
    gtk_label_set_text(GTK_LABEL(juego->graficos.playerName[i]), juego->jugadores[i].nombre);
  }

  // cambia la imagend el jugador actual
  gtk_widget_destroy(juego->graficos.playingImg);

  if(juego->actual->valor.estadoPartida > 0)
  {
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m40[juego->actual->valor.turno]);
  }
  else
  {
    juego->graficos.playingImg = gtk_image_new_from_pixbuf(juego->graficos.m40[2]);
  }

  gtk_box_pack_start(GTK_BOX(juego->graficos.playingBox), juego->graficos.playingImg, FALSE, TRUE, 0);
  gtk_widget_show(juego->graficos.playingImg);

  // si hay jugada anterior, habilita el boton de anterior
  if(juego->actual->ant != NULL)
  {
    gtk_widget_set_sensitive(juego->graficos.moveButtons[0], TRUE);
  }

  return;
}

/**
 * Guarda la información de una partida en un archivo binario
 * 
 * @author Mariano Malouly Orozco
 * @author Ricardo Sanchéz Zepeda
 * 
 * @param fileName[] nombre del archivo
 * @param *datos los datos de la aprtida
 * @param *parent el widget que llamó a la función
 * 
 * @return void
*/
void saveFile(char fileName[], JUEGO *datos, GtkWidget *parent)
{
  GtkWidget *dialog, *label, *box;
  
  LISTA *temp;

  FILE *fp;
  
  int i = 0;

  fp = fopen(fileName, "wb");

  // guarda la información relevante de la aprtida
  fwrite(&datos->hardMode, sizeof(gboolean), 1, fp);

  for(i = 0; i < 2; i++)
  {
    fwrite(&datos->jugadores[i], sizeof(JUGADOR), 1, fp);
  }

  temp = datos->actual;

  while(temp->ant != NULL)
  {
    temp = temp->ant;
  }

  while(temp != NULL)
  {
    fwrite(&temp->valor, sizeof(ESTADO), 1, fp);
    temp = temp->sig;
  }

  fclose(fp);

  // muestra un mensaje al guardar con exito la partida
  dialog = gtk_dialog_new_with_buttons("Archivo guardado", GTK_WINDOW(parent), GTK_DIALOG_MODAL, "Cerrar", GTK_RESPONSE_CLOSE, NULL);
  
  box = gtk_vbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), box, TRUE, TRUE, 20);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);

  label = gtk_label_new("Archivo guardado con exito!");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 20);

  gtk_widget_show_all(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

  return;
}

/**
 * Carga una partida de un archivo binario, regresa:
 * -2 si no pudo acceder al archivo
 * -1 si no pudo leer un dato de la partida
 * 0 si pudo cargar correctamente la partida
 * 
 * @author Mariano Malouly Orozco
 * @author Ricardo Sanchéz Zepeda
 * 
 * @param fileName[] nombre del archivo
 * @param *datos los datos de la aprtida
 * @param *parent el widget que llamó a la función
 * 
 * @return int
*/
int loadFile(char fileName[], JUEGO *datos, GtkWidget *parent)
{
  GtkWidget *dialog, *label, *box;

  LISTA *temp;
  ESTADO turno;

  FILE *fp;

  int rv = 0;
  int i = 0;

  temp = NULL;
  datos->actual = NULL;

  // prepara un mensaje
  dialog = gtk_dialog_new_with_buttons("Cargar partida", GTK_WINDOW(parent), GTK_DIALOG_MODAL, "Cerrar", GTK_RESPONSE_CLOSE, NULL);
  
  box = gtk_vbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), box, TRUE, TRUE, 20);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);

  fp = fopen(fileName, "rb");


  // si puede abrir el archivo
  if(fp != NULL)
  {
    // intentará leer cada dato uno por uno, al no poder leer algo, marcará un error
    if(!fread(&datos->hardMode, sizeof(gboolean), 1, fp))
    {
      label = gtk_label_new("Partida corrupta.");
      rv = -1;
    }

    for(i = 0; i < 2 && !rv; i++)
    {
      if(!fread(&datos->jugadores[i], sizeof(JUGADOR), 1, fp))
      {
        label = gtk_label_new("Partida corrupta.");
        rv = -1;
      }
    }

    // aloca memoria para el historial de movimientos
    while(fread(&turno, sizeof(ESTADO), 1, fp) && !rv)
    {
      temp = (LISTA *) malloc(sizeof(LISTA));
      temp->valor = turno;
      temp->ant = NULL;
      temp->sig = NULL;

      if(datos->actual != NULL)
      {
        datos->actual->sig = temp;
        temp->ant = datos->actual;

        datos->actual = temp;
      }
      else
      {
        datos->actual = temp;
      }
    }

    if(datos->actual == NULL && !rv)
    {
      label = gtk_label_new("Partida corrupta.");
      rv = -1;
    }
    
    fclose(fp);

    if(!rv)
    {
      label = gtk_label_new("Partida cargada.");
    }
  }
  else
  {
    label = gtk_label_new("No se puede acceder al archivo.");
    rv = -2;
  }

  // muestra el mensaje que haya resultado
  gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 20);
  gtk_widget_show_all(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

  return rv;
}