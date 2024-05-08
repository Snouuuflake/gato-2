#include "header.h"

void nuevaPartida(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;
  
  NGDATA newGame;

  GtkWidget *dialog, *mBox, *hBox, *vBox, *label;

  const gchar *names[2];
  char plrnms[2][2000];

  gint res = 0;
  gint sRes = 0;

  int s = 0;

  if(datos->actual->estadoPartida)
  {
    dialog = gtk_dialog_new_with_buttons("Guardar partida?", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cancelar", GTK_RESPONSE_CANCEL, "Si", GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
    
    vBox = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vBox), 10);

    label = gtk_label_new("Guardar partida actual antes de iniciar una nueva?");
      gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);

    do
    {
      res = gtk_dialog_run(GTK_DIALOG(dialog));
      sRes = GTK_RESPONSE_ACCEPT;
      
      if(res == GTK_RESPONSE_YES)
      {
        sRes = saveGame(datos, dialog);
      }

    } while(sRes != GTK_RESPONSE_ACCEPT);
    
    gtk_widget_destroy(dialog);
  }

  if(res != GTK_RESPONSE_CANCEL || !datos->actual->estadoPartida)
  {
    dialog = gtk_dialog_new_with_buttons("Nueva partida", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
      gtk_window_set_default_size(GTK_WINDOW(dialog), 453, 316);
    
    mBox = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), mBox, TRUE, TRUE, 5);

    label = gtk_label_new("Adversario:");
      gtk_box_pack_start(GTK_BOX(mBox), label, TRUE, TRUE, 5);
    
    hBox = gtk_hbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, FALSE, 5);

    newGame.radioButtons[0] = gtk_radio_button_new_with_label(NULL, "Jugador");
      gtk_box_pack_start(GTK_BOX(hBox), newGame.radioButtons[0], TRUE, FALSE, 0);
      gtk_signal_connect(GTK_OBJECT(newGame.radioButtons[0]), "toggled", G_CALLBACK(radio_changed), &newGame);

    newGame.radioButtons[1] = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(newGame.radioButtons[0]), "Computadora");
      gtk_box_pack_start(GTK_BOX(hBox), newGame.radioButtons[1], TRUE, FALSE, 0);
    
    hBox = gtk_hbox_new(TRUE, 5);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, FALSE, TRUE, 0);

    newGame.chb = gtk_toggle_button_new_with_label("Modo dificil.");
      gtk_box_pack_start(GTK_BOX(hBox), newGame.chb, FALSE, TRUE, 0);
      gtk_widget_set_sensitive(newGame.chb, FALSE);
    
    label = gtk_label_new("Nombre(s):");
      gtk_box_pack_start(GTK_BOX(mBox), label, TRUE, TRUE, 0);
    
    hBox = gtk_hbox_new(FALSE, 5);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, TRUE, TRUE, 5);
    
    label = gtk_label_new("Jugador 1:");
      gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, TRUE, 5);

    newGame.players[0] = gtk_entry_new();
      gtk_box_pack_start(GTK_BOX(hBox), newGame.players[0], TRUE, TRUE, 5);

    hBox = gtk_hbox_new(FALSE, 5);
      gtk_box_pack_start(GTK_BOX(mBox), hBox, TRUE, TRUE, 5);

    label = gtk_label_new("Jugador 2:");
      gtk_box_pack_start(GTK_BOX(hBox), label, FALSE, TRUE, 5);

    newGame.players[1] = gtk_entry_new();
      gtk_box_pack_start(GTK_BOX(hBox), newGame.players[1], TRUE, TRUE, 5);
    
    newGame.eLabel = gtk_label_new("\0");
      gtk_box_pack_start(GTK_BOX(mBox), newGame.eLabel, TRUE, TRUE, 5);
    
    gtk_widget_show_all(dialog);
    
    do
    {
      s = 1;

      res = gtk_dialog_run(GTK_DIALOG(dialog));

      if(res == GTK_RESPONSE_OK)
      {
        names[0] = gtk_entry_get_text(GTK_ENTRY(newGame.players[0]));
        names[1] = gtk_entry_get_text(GTK_ENTRY(newGame.players[1]));

        strcpy(plrnms[0], names[0]);
        strcpy(plrnms[1], names[1]);

        if((gtk_widget_get_sensitive(newGame.players[1]) && !strlen(names[1])) || !strlen(names[0]))
        {
          gtk_label_set_text(GTK_LABEL(newGame.eLabel), "Debes llenar todos los campos activos.");
          
          s = 0;
        }
        else if(!strcmp(names[0], names[1]))
        {
          gtk_label_set_text(GTK_LABEL(newGame.eLabel), "Los nombres no pueden ser iguales.");
          
          s = 0;
        }
        else if((!gtk_widget_get_sensitive(newGame.players[1]) && !strcmp(names[0], AI_NAME)))
        {
          gtk_label_set_text(GTK_LABEL(newGame.eLabel), "Hercules.raw, solo puede haber uno.");
          
          s = 0;
        }
        else if(gtk_widget_get_sensitive(newGame.players[1]))
        {
          setNewGame(datos, FALSE, FALSE, plrnms[0], plrnms[1]);
        }
        else
        {
          if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(newGame.chb)))
          {
            setNewGame(datos, TRUE, TRUE, plrnms[0], NULL);
          }
          else
          {
            setNewGame(datos, TRUE, FALSE, plrnms[0], NULL);
          }
        }
      }
      else
      {
        s = 1;
      }
    } while(!s);
      
    gtk_widget_destroy(dialog);
  }

  return;
}

void terminarPartida(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;

  GtkWidget *dialog, *label, *box;

  gint res = 0;
  gint sRes = 0;

  if(datos->actual->estadoPartida)
  {
    dialog = gtk_dialog_new_with_buttons("Guardar partida?", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cancelar", GTK_RESPONSE_CANCEL, "Si", GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
    
    box = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), box, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    label = gtk_label_new("Guardar partida actual antes de terminar?");
      gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);

    do
    {
      res = gtk_dialog_run(GTK_DIALOG(dialog));
      sRes = GTK_RESPONSE_ACCEPT;
      
      if(res == GTK_RESPONSE_YES)
      {
        sRes = saveGame(datos, dialog);
      }

    } while(sRes != GTK_RESPONSE_ACCEPT);

    if(res != GTK_RESPONSE_CANCEL)
    {
      resetGame(datos);
    }
    
    gtk_widget_destroy(dialog);
  }
  else
  {
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

void guardarPartida(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;

  saveGame(datos, datos->graficos.window);

  return;
}

gint saveGame(JUEGO *datos, GtkWidget *parent)
{
  GtkWidget *dialog, *label, *box;

  gint result = 0;
  gchar *file;

  if(datos->actual->estadoPartida)
  {
    dialog = gtk_file_chooser_dialog_new("Guardar Partida", GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if(result == GTK_RESPONSE_ACCEPT)
    {
      file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      
      // random save function goes here
      g_print("%s\n", file);

      g_free(file);
    }

    gtk_widget_destroy(dialog);
  }
  else
  {
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

void cargarPartida(GtkWidget *widget, gpointer data)
{
  JUEGO *datos = (JUEGO *)data;

  loadGame(datos, datos->graficos.window);

  return;
}

gint loadGame(JUEGO *datos, GtkWidget *parent)
{
  GtkWidget *dialog, *label, *vBox;

  gint result = 0;
  gint res = 0;
  gint sRes = 0;
  gchar *file;

  if(datos->actual->estadoPartida)
  {
    dialog = gtk_dialog_new_with_buttons("Guardar partida?", GTK_WINDOW(datos->graficos.window), GTK_DIALOG_MODAL, "Cancelar", GTK_RESPONSE_CANCEL, "Si", GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
    
    vBox = gtk_vbox_new(TRUE, 10);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 20);
      gtk_container_set_border_width(GTK_CONTAINER(vBox), 10);

    label = gtk_label_new("Guardar partida actual antes de cargar partida?");
      gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 20);
    
    gtk_widget_show_all(dialog);

    do
    {
      res = gtk_dialog_run(GTK_DIALOG(dialog));
      sRes = GTK_RESPONSE_ACCEPT;
      
      if(res == GTK_RESPONSE_YES)
      {
        sRes = saveGame(datos, dialog);
      }

    } while(sRes != GTK_RESPONSE_ACCEPT);
    
    gtk_widget_destroy(dialog);
  }
  
  dialog = gtk_file_chooser_dialog_new("Cargar Partida", GTK_WINDOW(parent), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  result = gtk_dialog_run(GTK_DIALOG(dialog));

  if(result == GTK_RESPONSE_ACCEPT)
  {
    file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    
    // random load function goes here
    g_print("Remember to load the file from here:\n%s\n", file);

    g_free(file);
  }

  gtk_widget_destroy(dialog);

  return result;
}

void comoJugar(GtkWidget *widget, gpointer data)
{
  // JUEGO *datos = (JUEGO *)data;

  g_print("Instructivo\n");

  return;
}

void creditos(GtkWidget *widget, gpointer data)
{
  // JUEGO *datos = (JUEGO *)data;

  g_print("Creditos\n");

  return;
}

void endPopup(JUEGO *juego, char endState)
{
  GtkWidget *dialog, *vBox, *label, *image;

  GdkPixbuf *pix;

  char end[10000];

  end[0] = 0;
  image = NULL;

  dialog = gtk_dialog_new_with_buttons("Fin del juego", GTK_WINDOW(juego->graficos.window), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 430, 220);

  vBox = gtk_vbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vBox, TRUE, TRUE, 20);
    gtk_container_set_border_width(GTK_CONTAINER(vBox), 10);

    if(endState == 'g')
    {
      if(juego->jugadores[juego->actual->turno].esCPU || juego->jugadores[(juego->actual->turno + 1) % 2].esCPU)
      {
        if(juego->hardMode)
        {
          sprintf(end, "Neutral ending: hercules.raw volvera a dormir por otro siglo.");
        }
        else
        {
          sprintf(end, "Empate, hercules.raw parece sorprendido.");
        }
      }
      else
      {
        sprintf(end, "El juego ha terminado en empate!");
      }
    }
    else
    {
      if(juego->jugadores[(juego->actual->turno + 1) % 2].esCPU)
      {
        pix = gdk_pixbuf_new_from_file_at_scale("./MEDIA/hercules.raw", 200, 200, TRUE, NULL);
        image = gtk_image_new_from_pixbuf(pix);
          gtk_box_pack_start(GTK_BOX(vBox), image, TRUE, TRUE, 10);

        if(juego->hardMode)
        {
          sprintf(end, "Bad ending: la humanidad esta condenada.");
        }
        else
        {
          sprintf(end, "Derrota, hercules.raw te mira con desprecio.");
        }
      }
      else if(juego->jugadores[juego->actual->turno].esCPU)
      {
        if(juego->hardMode)
        {
          sprintf(end, "Good ending: has derrotado a un terror cosmico.");
        }
        else
        {
          sprintf(end, "Victoria, hercules.raw te respeta un poco mas que antes.");
        }
      }

      if(endState == 'X')
      {
        if(image == NULL)
        {
          image = gtk_image_new_from_pixbuf(juego->graficos.m40[0]);
            gtk_box_pack_start(GTK_BOX(vBox), image, TRUE, TRUE, 10);
        }

        if(!end[0])
        {

          sprintf(end, "La victoria es para %s", juego->jugadores[0].nombre);
        }
      }
      else if(endState == 'O')
      {
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
  
  label = gtk_label_new(end);
    gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 10);

  gtk_widget_show_all(dialog);

  gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);

  return;
}