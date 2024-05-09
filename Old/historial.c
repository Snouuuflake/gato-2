#include "header.h"

//TODO: delete this i guess
//int main(void)
//{
  //int idbuton;
  //JUEGO juego;
  //juego.jugadorActual = 0;
  //GSTRUCT gstruct;
  //int i,j, v=0;
//
  //gstruct.juego = &juego;
  //for(i=0;i<9;i++){
    //juego.tablero[i] = ' ';
  //}
  //juego.inicio = NULL;
//
  //printf("jugador 1: ");
  //gets(juego.jugadores[0].nombre);
  //printf("jugador 2: ");
  //gets(juego.jugadores[1].nombre);
//
  //for(i=0;i<9;i++){
    //printf("%d\n", i);
    //printf("dame el id del botton: ");
    //__fpurge(stdin);
    //scanf("%d",  &gstruct.id);
//
    //juego.tablero[gstruct.id] = "XO"[juego.jugadorActual];
    //juego.jugadorActual = (juego.jugadorActual + 1) % 2;
//
    //if(gstruct.juego->inicio == NULL){
      //printf("ES NULL\n\n");
    //}
//
    //anexarEstado(&juego);
//
  //}
//
    //imprimirHistorial(&juego);
//
    //liberarHistorial(&juego);
  //return 0;
//}


void anexarEstado(JUEGO *juego)
{
  ESTADO *new, *temp;
  int i;

    if(juego->inicio == NULL){
      new = (ESTADO*) malloc(sizeof(ESTADO));
      juego->inicio = new;
      new->sig= NULL;
      new->ant = NULL;

      for (i = 0; i < 9; i++) {
        new->tablero[i] = juego->tablero[i];
      }
      new->turno = juego->jugadorActual;
    }

    else if(juego->inicio != NULL){
      temp = juego->inicio;

      while(temp->sig != NULL){
        temp = temp->sig;
      }
      new = (ESTADO*) malloc(sizeof(ESTADO));
      temp->sig = new;
      new->ant = temp;
      new->sig = NULL;

      for (i = 0; i < 9; i++) {
        new->tablero[i] = juego->tablero[i];
      }
      new->turno = juego->jugadorActual;

    }

  //strcpy(juego->tablero, new->tablero); 

}




void imprimirHistorial(JUEGO *juego)
{
  ESTADO *temp;
  int i;

  temp = juego->inicio;

  while(temp != NULL){
    printf("turno jugador%d:\n\n", temp->turno +1);
    for(i=0;i<9;i=i+3){
      printf("%c,%c,%c\n", temp->tablero[i], temp->tablero[i+1], temp->tablero[i+2]);
    }
    printf("\n\n");
    temp = temp->sig;
  }
}


void liberarHistorial(JUEGO *juego)
{
  ESTADO *temp;

  while(juego->inicio != NULL){
    temp = juego->inicio;
    juego->inicio = juego->inicio->sig;
    free(temp);
  }
}
