/**
 * @file popups.c
 * 
 * @brief Contiene toda la parte gráfica para las diversas ventanas que el programa genera a lo largo de la ejecución
 * 
 * @author Luis Julián Zamora Treviño
 * @date 08/05/2024
*/

#include "header.h"

/**
 * La ventana para una nueva partida
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget El widget que invocó a la función
 * @param data Puntero a toda la información del juego
 * 
 * @return void
*/
void nuevaPartida(GtkWidget *widget, gpointer data)
{
  // hace un cast a la información del juego
  JUEGO *datos = (JUEGO *)data;
  
  // estrugtura con información para un juego nuevo
  NGDATA newGame;

  // widgets para la ventana
  GtkWidget *dialog, *mBox, *hBox, *vBox, *label;

  // nombre de los usuarios tanto para leerlos de un entrybox como para mandarlos directamente a la info del juego
  const gchar *names[2];
  char plrnms[2][2000];

  gint res = 0;
  gint sRes = 0;

  int s = 0;

  // si hay una partida en curso
  if(datos->actual->valor.estadoPartida)
  {
    // genera una ventana que pregunte si se desea guardar partida
    dialog = gtk_dialog_new_with_buttons("Guardar partida?", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cancelar", GTK_RESPONSE_CANCEL, "Si", GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
    
    vBox = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vBox), 10);

    label = gtk_label_new("Guardar partida actual antes de iniciar una nueva?");
      gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);

    // para guardar la partida en curso
    do
    {
      // pregunta al usuario si guardar
      res = gtk_dialog_run(GTK_DIALOG(dialog));
      sRes = GTK_RESPONSE_ACCEPT;
      
      // si se desea guardar
      if(res == GTK_RESPONSE_YES)
      {
        // ejecuta la ventana de guardado de archivos
        sRes = saveGame(datos, dialog);
      }

      // si se canceló el guardado de archivos, repite el bucle
    } while(sRes != GTK_RESPONSE_ACCEPT);
    
    gtk_widget_destroy(dialog);
  }

  // si no se canceló la creación de una nueva partida o no hay una partida en curso
  if(res != GTK_RESPONSE_CANCEL || !datos->actual->valor.estadoPartida)
  {
    // genera una ventana que solicita la información para la nueva aprtida
    dialog = gtk_dialog_new_with_buttons("Nueva partida", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
      gtk_window_set_default_size(GTK_WINDOW(dialog), 453, 316);
    
    mBox = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), mBox, TRUE, TRUE, 5);

    // selección de adversario
    label = gtk_label_new("Adversario:");
      gtk_box_pack_start(GTK_BOX(mBox), label, TRUE, TRUE, 5);
    
    hBox = gtk_hbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 5);

    // creación de radiobuttons para elegir al adversario
    newGame.radioButtons[0] = gtk_radio_button_new_with_label(NULL, "Jugador");
      gtk_box_pack_start(GTK_BOX(hBox), newGame.radioButtons[0], TRUE, FALSE, 0);
      gtk_signal_connect(GTK_OBJECT(newGame.radioButtons[0]), "toggled", G_CALLBACK(radio_changed), &newGame);

    newGame.radioButtons[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(newGame.radioButtons[0]), "Computadora");
      gtk_box_pack_start(GTK_BOX(hBox), newGame.radioButtons[1], TRUE, FALSE, 0);
    
    hBox = gtk_hbox_new(TRUE, 5);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, TRUE, 0);

    // selección de modo dificil (solo se activa si la partida es contra una computadora)
    newGame.chb = gtk_toggle_button_new_with_label("Modo dificil.");
      gtk_box_pack_start(GTK_BOX(hBox), newGame.chb, FALSE, TRUE, 0);
      gtk_widget_set_sensitive(newGame.chb, FALSE);
    
    // sección de nombres de jugadores
    label = gtk_label_new("Nombre(s):");
      gtk_box_pack_start(GTK_BOX(mBox), label, TRUE, TRUE, 0);
    
    hBox = gtk_hbox_new(FALSE, 5);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, TRUE, TRUE, 5);
    
    label = gtk_label_new("Jugador 1:");
      gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, TRUE, 5);

    // entry box para el nombre del primer jugador
    newGame.players[0] = gtk_entry_new();
      gtk_box_pack_start(GTK_BOX(hBox), newGame.players[0], TRUE, TRUE, 5);

    hBox = gtk_hbox_new(FALSE, 5);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, TRUE, TRUE, 5);

    label = gtk_label_new("Jugador 2:");
      gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, TRUE, 5);

    // entry box para el nombre del segundo jugador
    newGame.players[1] = gtk_entry_new();
      gtk_box_pack_start(GTK_BOX(hBox), newGame.players[1], TRUE, TRUE, 5);
    
    newGame.eLabel = gtk_label_new("\0");
      gtk_box_pack_start(GTK_BOX(mBox), newGame.eLabel, TRUE, TRUE, 5);
    
    gtk_widget_show_all(dialog);
    
    // ejecuta muestra la ventana y evalua ciertas validaciones
    do
    {
      s = 1;

      res = gtk_dialog_run(GTK_DIALOG(dialog));

      // si el usuario selecciona aceptar
      if(res == GTK_RESPONSE_OK)
      {
        // obtiene los nombres y los copia en cadenas de caracteres
        names[0] = gtk_entry_get_text(GTK_ENTRY(newGame.players[0]));
        names[1] = gtk_entry_get_text(GTK_ENTRY(newGame.players[1]));

        strcpy(plrnms[0], names[0]);
        strcpy(plrnms[1], names[1]);

        // si los campos de nombres están vacíos y deben tener valores
        if((gtk_widget_get_sensitive(newGame.players[1]) && !strlen(names[1])) || !strlen(names[0]))
        {
          // muestra error y valida para reiniciar el bucle
          gtk_label_set_text(GTK_LABEL(newGame.eLabel), "Debes llenar todos los campos activos.");
          
          s = 0;
        }
        // si los nombres son iguales
        else if(!strcmp(names[0], names[1]))
        {
          // muestra error y valida para reiniciar el bucle
          gtk_label_set_text(GTK_LABEL(newGame.eLabel), "Los nombres no pueden ser iguales.");
          
          s = 0;
        }
        // si el usuario juega contra computadora y tiene el mismo nombre
        else if((!gtk_widget_get_sensitive(newGame.players[1]) && !strcmp(names[0], AI_NAME)))
        {
          // muestra error y valida para reiniciar el bucle
          gtk_label_set_text(GTK_LABEL(newGame.eLabel), "Hercules.raw, solo puede haber uno.");
          
          s = 0;
        }
        // si la partida es contra otro jugador
        else if(gtk_widget_get_sensitive(newGame.players[1]))
        {
          // prepara una nueva partida con la info de los nuevos jugadores
          setNewGame(datos, FALSE, FALSE, plrnms[0], plrnms[1]);
        }
        // si la partida es contra la computadora
        else
        {
          // si es en modo difícil
          if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(newGame.chb)))
          {
            // prepara la partida contra la computadora en modo dificil
            setNewGame(datos, TRUE, TRUE, plrnms[0], NULL);
          }
          else
          {
            // prepara la partida contra la computadora
            setNewGame(datos, TRUE, FALSE, plrnms[0], NULL);
          }
        }
      }
      // si se seleccionó cancelar, cierra la ventana y no inicia una nueva partida
      else
      {
        s = 1;
      }
    } while(!s);
      
    gtk_widget_destroy(dialog);
  }

  return;
}

/**
 * Ventana de confirmación para terminar una partida
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget El widget que invocó a la función
 * @param data Puntero a toda la información del juego
 * 
 * @return void
*/
void terminarPartida(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;

  GtkWidget *dialog, *label, *box;

  gint res = 0;
  gint sRes = 0;

  // si hay una partida en curso
  if(datos->actual->valor.estadoPartida)
  {
    // pregunta si se desea guardar antes de terminar
    dialog = gtk_dialog_new_with_buttons("Guardar partida?", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cancelar", GTK_RESPONSE_CANCEL, "Si", GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
    
    box = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), box, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    label = gtk_label_new("Guardar partida actual antes de terminar?");
      gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);

    // muestra la ventana y realiza la acción que el usuario indicó
    do
    {
      res = gtk_dialog_run(GTK_DIALOG(dialog));
      sRes = GTK_RESPONSE_ACCEPT;
      
      // si se desea guardar la partida
      if(res == GTK_RESPONSE_YES)
      {
        // ejecuta la ventana de guardado
        sRes = saveGame(datos, dialog);
      }
    } while(sRes != GTK_RESPONSE_ACCEPT);

    // si no se canceló la accion de temrinar la partida
    if(res != GTK_RESPONSE_CANCEL)
    {
      // reinicia el tablero y borra la información de la partida
      resetGame(datos);
    }
    
    gtk_widget_destroy(dialog);
  }
  // si no hay una partida en curso
  else
  {
    // muestra una ventana de error
    dialog = gtk_dialog_new_with_buttons("No se puede terminar", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cerrar", GTK_RESPONSE_CLOSE, NULL);

    box = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), box, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    label = gtk_label_new("No hay una partida en curso para terminar.");
      gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);
  }

  return;
}

/**
 * Ejecuta la ventana de guardado de juego, regresa el valor de la respuesta del usuario (el botón que presiona)
 *
 * @author Luis Julián Zamora Treviño
 * 
 * @param *datos La informacion del juego
 * @param *parent el widget que llamó a la función
 * 
 * @return gint
*/
gint saveGame(JUEGO *datos, GtkWidget *parent)
{
  GtkWidget *dialog, *label, *box;

  gint result = 0;
  gchar *file;

  // si hay una partida en curso
  if(datos->actual->valor.estadoPartida)
  {
    // muestra la ventana de guardado
    dialog = gtk_file_chooser_dialog_new("Guardar Partida", GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    // si se da click en guardar
    if(result == GTK_RESPONSE_ACCEPT)
    {
      // obtiene la ruta
      file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      
      // guarda la partida en el archivo
      saveFile(file, datos, dialog);

      // libera memoria
      g_free(file);
    }

    gtk_widget_destroy(dialog);
  }
  // si no hay una partida en curso
  else
  {
    // muestra un mensaje de error
    dialog = gtk_dialog_new_with_buttons("No se puede guardar", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cerrar", GTK_RESPONSE_CLOSE, NULL);

    box = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), box, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    label = gtk_label_new("No hay una partida en curso para guardar.");
      gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);
  }

  return result;
}

/**
 * Ejecuta la ventana de carga de partida, regresa el valor de la respuesta del usuario (el botón que presiona)
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *datos La informacion del juego
 * @param *parent el widget que llamó a la función
 * 
 * @return gint
*/
gint loadGame(JUEGO *datos, GtkWidget *parent)
{
  GtkWidget *dialog, *label, *vBox;

  JUEGO nuevo;

  gint result = 0;
  gint res = 0;
  gint sRes = 0;
  gint s = 0;

  gchar *file;

  // si hay una partida en curso
  if(datos->actual->valor.estadoPartida)
  {
    // pregunta si se desea guardar
    dialog = gtk_dialog_new_with_buttons("Guardar partida?", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cancelar", GTK_RESPONSE_CANCEL, "Si", GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
    
    vBox = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vBox), 10);

    label = gtk_label_new("Guardar partida actual antes de cargar partida?");
      gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);

    // ejecuta la acción que el usuario seleccionó
    do
    {
      res = gtk_dialog_run(GTK_DIALOG(dialog));
      sRes = GTK_RESPONSE_ACCEPT;
      
      // si se desea guardar partida, la guarda
      if(res == GTK_RESPONSE_YES)
      {
        sRes = saveGame(datos, dialog);
      }

    } while(sRes != GTK_RESPONSE_ACCEPT);
    
    gtk_widget_destroy(dialog);
  }
  
  // ejecuta la ventana para cargar un archivo
  dialog = gtk_file_chooser_dialog_new("Cargar Partida", GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  do
  {
    // muestra la ventana
    result = gtk_dialog_run(GTK_DIALOG(dialog));

    // si se seleccionó un archivo
    if(result == GTK_RESPONSE_ACCEPT)
    {
      // obtiene la ruta del archivo
      file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      
      // intenta cargar la partida en una estructura de juego diferente a la original
      s = loadFile(file, &nuevo, dialog);

      g_free(file);

      // si pudo cargar la partida
      if(!s)
      {
        // borra la partida actual
        resetGame(datos);

        // copia los valores relevantes de la partida
        datos->actual = nuevo.actual;
        datos->jugadores[0] = nuevo.jugadores[0];
        datos->jugadores[1] = nuevo.jugadores[1];
        datos->hardMode = nuevo.hardMode;

        // establece la partida en el ultimo punto del historial
        coppyBoardState(datos);
        coppyPlayersState(datos);
      }
    }
    // si se cancela la acción, temina el bucle
    else
    {
      s = 0;
    }
  } while(s);

  gtk_widget_destroy(dialog);

  return result;
}

/**
 * Muestra el manual de usuario
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget El widget que invocó a la función
 * @param data Puntero a toda la información del juego
 * 
 * @return void
*/
void comoJugar(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;

  GtkWidget *assistant, *label;

  // desactiva la pantalla principal
  gtk_widget_set_sensitive(datos->graficos.window, FALSE);

  // crea una ventana tipo assistant (permite tener varias páginas) y le asocia una función para desbloquear la ventana principal
  assistant = gtk_assistant_new();
    gtk_widget_set_size_request(assistant, 500, 300);
    gtk_window_set_title(GTK_WINDOW (assistant), "Como jugar?");
    g_signal_connect(G_OBJECT(assistant), "destroy", G_CALLBACK (assistant_destroy), datos);
    g_signal_connect(G_OBJECT(assistant), "cancel", G_CALLBACK(assistant_close), NULL);
    g_signal_connect(G_OBJECT(assistant), "close", G_CALLBACK(assistant_close), NULL);

  // Modos de juego

  // funciona igual con todos, crea el texto que va a contener esa página
  label = gtk_label_new(
    "Al iniciar una partida, se desplegará una ventana de configuración\n"\
    "que permitirá seleccionar uno de dos posibles adversarios:\n"\
    "otro jugador o la computadora.\n\n"\
    "En caso de enfrentarte a otro jugador, ambos deberán colocar sus \n"\
    "nombres, que deben ser diferentes uno del otro.\n\n"\
    "Si se elige jugar contra la computadora, solo será necesario colocar\n"\
    "un nombre y adicionalmente podrá seleccionarse el modo dificíl.");
  
  // le agrega la página a la ventana con el titulo y el tipo de ventana
  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Modos de juego");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  

  // Modos de juego: contra jugador
  label = gtk_label_new(
    "Te enfrentarás a otro usuario, el primer turno se elige al azar.\n\n"\
    "Quien logre hacer una línea recta de tres fichas será el ganador.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Modos de juego:\nContra Jugador");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Modos de juego: contra computadora
  label = gtk_label_new(
    "Te enfrentarás a la computadora.\n\n"\
    "Esta será capaz de bloquear tus jugadas y de desarrollar las propias.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Modos de juego:\nContra Computadora");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Modos de juego: contra dificil
  label = gtk_label_new(
    "La computadora tendrá la oportunidad de tirar dos veces por turno.\n\n"\
    "Adicionalmente, el destino del universo estará en tus manos.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Modos de juego:\nContra Computadora (dificíl)");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Tablero
  label = gtk_label_new(
    "El tablero tiene de su lado izquierdo la imagen del jugador que está\n"\
    "jugando actualmente y de su lado derecho la ficha y nombre de ambos\n"\
    "jugadores.\n\n"\
    "Si se selecciona una casilla y no hay una partida en curso, se mostrará\n"\
    "la ventana de configuración de juego.\n\n"\
    "Si la partida ha acabado, no se podrán colocar más fichas.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Tablero");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Historial
  label = gtk_label_new(
    "Debajo del tablero se encuentran dos botones, uno para regresar y otro\n"\
    "para avanzar, estos se activan o desactivan si hay movimientos en el\n"\
    "historial en ese sentido.\n\n"\
    "Al seleccionarlos, la partida se recorrera al punto del historial deseado.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Historial");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Menu: Archivo, Guardar
  label = gtk_label_new(
    "Atajo: ctl + S\n\n"\
    "Despliega el navegador de archivos para guardar la partida actual.\n\n"\
    "Si no hay una partida en curso, una advertencia se desplegará en lugar\n"\
    "del navegador.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Menu: Archivo\nGuardar");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Menu: Archivo, Abrir
  label = gtk_label_new(
    "Atajo: ctl + O\n\n"\
    "Despliega el navegador de archivos para cargar una nueva partida.\n\n"\
    "En caso de que haya una partida en curso, se preguntará si se desea\n"\
    "guardar la partida actual.\n\n"\
    "Tras guardar la partida en caso de desearlo y cargar otra aprtida, la\n"\
    "partida cargada continuará desde su último punto en el historial.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Menu: Archivo\nAbrir");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Menu: Juego, Nuevo
  label = gtk_label_new(
    "Atajo: ctl + N\n\n"\
    "Despliega la ventana de selección de modo de juego.\n\n"\
    "Una vez configurada la partida, esta comenzará con las configuraciones\n"\
    "seleccionadas.\n\n"\
    "De haber una partida en curso, se preguntará si se desea guardar.\n");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Menu: Juego\nNuevo");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Menu: Juego, Terminar
  label = gtk_label_new(
    "Atajo: ctl + F\n\n"\
    "Finaliza la partida actual preguntando si se desea guardar.\n\n"\
    "Si no hay una partida en curso, se desplegará una advertencia.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Menu: Juego\nTerminar");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Menu: Mas, Como jugar
  label = gtk_label_new(
    "Atajo: ctl + H\n\n"\
    "Despliega esta ventana de ayuda.");

  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Menu: Mas\nComo jugar");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  
  
  // Menu: Mas, Acerca de
  label = gtk_label_new(
    "Atajo: ctl + A\n\n"\
    "Despliega esta ventana de créditod.");

  // el tipo de ventana tiene que ser de un tipo específico en la última página
  gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
    gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Menu: Mas\nAcerca de");
    gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label,  GTK_ASSISTANT_PAGE_CONFIRM);
    gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);

  gtk_widget_show_all (assistant);

  return;
}

/**
 * Muestra Información sobre los creadores del programa
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *widget El widget que invocó a la función
 * @param data Puntero a toda la información del juego
 * 
 * @return void
*/
void creditos(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;
  
  GtkWidget *dialog;

  // principales encargados (todos contribuimos en todo, pero a cada uno se le asigno la coordinación de cada parte del proyecto)

  // El principal encargado de la parte funcional
  const gchar *authors[] = {
    "Sánchez Zepeda Ricardo (ict23rsz)",
    NULL
  };

  // el principal encargado de la documentación
  const gchar *documenters[] = {
    "Malouly Orozco Mariano (msc23mmo)",
    NULL
  };

  // el principal encargado de la parte gráfica
  const gchar *artists[] = {
    "Zamora Treviño Luis Julián (ict23jzt)",
    NULL
  };

  dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Tic Tac Toe"); // nombre del proyecto
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "2.0"); // version
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Programación Aplicada y Laboratorio\nMaestro Jorge Rodríguez García"); // se usa de subtitulo
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors); // programador principal
    gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(dialog), documenters); // documentador principal
    gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), artists); // diseñador principal
    gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), datos->graficos.logo); // logo del proyecto

  // muestra la ventana
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

  return;
}

/**
 * Mensaje de fin de partida
 * 
 * @author Luis Julián Zamora Treviño
 * 
 * @param *juego La información de la partida y la ventana
 * @param endState La forma en la que terminó la partida
 * 
 * @return void
*/
void endPopup(JUEGO *juego, char endState)
{
  GtkWidget *dialog, *vBox, *label, *image;

  // mensaje final
  char end[10000];

  end[0] = 0;
  image = NULL;

  // inicia el mensaje de fin de juego
  dialog = gtk_dialog_new_with_buttons("Fin del juego", GTK_WINDOW(juego->graficos.window), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 430, 220);

  vBox = gtk_vbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 20);
    gtk_container_set_border_width(GTK_CONTAINER(vBox), 10);

  // si temrinpó en empate
  if(endState == 'g')
  {
    // si uno de los jugadores es una computadora
    if(juego->jugadores[juego->actual->valor.turno].esCPU || juego->jugadores[(juego->actual->valor.turno + 1) % 2].esCPU)
    {
      // mensajes diferentes para las dificultades
      if(juego->hardMode)
      {
        sprintf(end, "Neutral ending: hercules.raw volvera a dormir por otro siglo.");
      }
      else
      {
        sprintf(end, "Empate, hercules.raw parece sorprendido.");
      }
    }
    // si ambos jugadores eran personas
    else
    {
      sprintf(end, "El juego ha terminado en empate!");
    }
  }
  // si hubo ganador
  else
  {
    // si el ganador es una computadora
    if(juego->jugadores[(juego->actual->valor.turno + 1) % 2].esCPU)
    {
      // la imagen será la de la computadora
      image = gtk_image_new_from_pixbuf(juego->graficos.hercules);
        gtk_box_pack_start(GTK_BOX(vBox), image, TRUE, TRUE, 10);

      // mensajes diferentes para las dificultades
      if(juego->hardMode)
      {
        sprintf(end, "Bad ending: la humanidad esta condenada.");
      }
      else
      {
        sprintf(end, "Derrota, hercules.raw te mira con desprecio.");
      }
    }
    // si eljugador le ganó a la computadora
    else if(juego->jugadores[juego->actual->valor.turno].esCPU)
    {
      // mensajes diferentes para las dificultades
      if(juego->hardMode)
      {
        sprintf(end, "Good ending: has derrotado a un terror cosmico.");
      }
      else
      {
        sprintf(end, "Victoria, hercules.raw te respeta un poco mas que antes.");
      }
    }

    // si el ganador fue la X
    if(endState == 'X')
    {
      // si no hay imagen todavía, establece el X
      if(image == NULL)
      {
        image = gtk_image_new_from_pixbuf(juego->graficos.m40[0]);
          gtk_box_pack_start(GTK_BOX(vBox), image, TRUE, TRUE, 10);
      }

      // si no hay mensaje de victoria, lo establece
      if(!end[0])
      {
        sprintf(end, "La victoria es para %s", juego->jugadores[0].nombre);
      }
    }
    // si el ganador es el O
    else if(endState == 'O')
    {
      // ejecuta lo mismo que con X pero para el O
      if(image == NULL)
      {
        image = gtk_image_new_from_pixbuf(juego->graficos.m40[1]);
          gtk_box_pack_start(GTK_BOX(vBox), image, TRUE, TRUE, 10);
      }

      if(!end[0])
      {
        sprintf(end, "La victoria es para %s", juego->jugadores[0].nombre);
      }
    }
  }
  
  // establece el texto
  label = gtk_label_new(end);
    gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 10);

  // muestra la ventana
  gtk_widget_show_all(dialog);

  gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);

  return;
}