#include "pions.cpp"
#include <string.h>
#include <unistd.h>

void clear_terminal(){
  #ifdef __unix__
     std::system("clear");
  #elif _WIN32
  std::system("cls");
  #elif defined(__APPLE__)
    std::system("cls");
  #endif

}
Info_Plateau *refresh_plateau(Pion ***Plateau){

  Info_Plateau *Info_Plateaus = new Info_Plateau[sizeof(Info_Plateau)];
  char TYPE_PION;
  bool OWNER_PION;
  int NB_Chateau_0 = 0, NB_Guerrier_0 = 0, NB_Paysans_0 = 0, NB_Seigneurs_0 = 0;
  int NB_Chateau_1 = 0, NB_Guerrier_1 = 0, NB_Paysans_1 = 0, NB_Seigneurs_1 = 0;
  for (int i = 0; i < COLONNE; i++){
  }
  for (int j = 0, a = 0; j < COLONNE; j++, a++){
    for (int i = 0, b = 0; i < LIGNE; i++, b++){
      if (Plateau[i][j] == nullptr){}
      else {
        TYPE_PION = Plateau[i][j] -> type();
        OWNER_PION = Plateau[i][j] -> RETURN_OWNER();

        if (TYPE_PION == 'C'){
          if (OWNER_PION){ Info_Plateaus -> Chateaux_0[NB_Chateau_0].i = i; Info_Plateaus -> Chateaux_0[NB_Chateau_0].j = j; NB_Chateau_0++;}
          else {Info_Plateaus -> Chateaux_1[NB_Chateau_1].i = b; Info_Plateaus -> Chateaux_1[NB_Chateau_1].j = a; NB_Chateau_1++;}
        }
        else if (TYPE_PION == 'G') {
          if (OWNER_PION){ Info_Plateaus -> Guerriers_0[NB_Chateau_0].i = i; Info_Plateaus -> Guerriers_0[NB_Chateau_0].j = j; NB_Guerrier_0++;}
          else {Info_Plateaus -> Guerriers_1[NB_Chateau_1].i = b; Info_Plateaus -> Guerriers_1[NB_Chateau_1].j = a; NB_Guerrier_1++;}
        }
        else if (TYPE_PION == 'P') {
          if (OWNER_PION){ Info_Plateaus -> Paysans_0[NB_Chateau_0].i = i; Info_Plateaus -> Paysans_0[NB_Chateau_0].j = j; NB_Paysans_0++;}
          else {Info_Plateaus -> Paysans_1[NB_Chateau_1].i = b; Info_Plateaus -> Paysans_1[NB_Chateau_1].j = a; NB_Paysans_1++;}
        }
        else if (TYPE_PION == 'S') {
          if (OWNER_PION){ Info_Plateaus -> Seigneurs_0[NB_Chateau_0].i = i; Info_Plateaus -> Seigneurs_0[NB_Chateau_0].j = j; NB_Seigneurs_0++;}
          else {Info_Plateaus -> Seigneurs_1[NB_Chateau_1].i = b; Info_Plateaus -> Seigneurs_1[NB_Chateau_1].j = a; NB_Seigneurs_1++;}
        }

      }
    }
    for (int i = 0; i < COLONNE; i++){
    }
  }
  return Info_Plateaus;
}
