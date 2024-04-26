#include "pions.cpp"
#include <string.h>
#include <unistd.h>
#include <limits>

void cin_clear(){
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

void affichage_debut_jeu(Pion ***Plateau, Info_Plateau *Info_Plateaus, Info_Joueurs *Info_Joueurss, bool tour){
  Info_Plateaus = refresh_plateau(Plateau);

  cout << "Or joueur : " << Info_Joueurss -> RETURN_OR(true) << endl;
  cout << "Or adversaire : " << Info_Joueurss -> RETURN_OR(false) << endl << endl;
  cout << (
        tour
        ? "\x1b[34;1mC'est au tour du joueur"
        : "\x1b[31;1mC'est au tour de l'adversaire"
        ) << "\033[0m" << endl;
  cout << "Vous pouvez : " << endl << "1 - Ajouter un membre à l'équipe" << endl << "2 - Ne rien faire" << endl;

  sleep(1);
  cout << "Que voulez vous faire ?" << endl;
}

void choix_creer_personnage(Pion ***Plateau, Info_Plateau *Info_Plateau, Info_Joueurs *Info_Joueurs, bool tour){
  int choix_chateau, choix_type, i, resultat_creer_personnage, resultat_creer_personnage_f;
  cout << "Avec quel chateau ?" << endl;
  if (tour) for (i = 1;  Info_Plateau->Chateaux_0[i - 1].i != -1; i++) cout << i << " : Chateau en X = " << Info_Plateau->Chateaux_0[i - 1].i <<", Y = " << Info_Plateau->Chateaux_0[i - 1].j << endl;
  else for (i = 1;  Info_Plateau->Chateaux_1[i - 1].i != -1; i++) cout << i << " : Chateau en X = " << Info_Plateau->Chateaux_1[i - 1].i <<", Y = " << Info_Plateau->Chateaux_1[i - 1].j << endl;

  cin_clear();

  if ((cin >> choix_chateau)) {
    if (choix_chateau <= 0 || choix_chateau > i - 1) resultat_creer_personnage = -1;
  }
  else resultat_creer_personnage =  -1;

  cin_clear();
  cout << "Quel pion voulez-vous créer ?" << endl << "1 - Guerrier" << endl << "2 - Paysan" << endl << "3 - Seigneur" << endl;
  if ((cin >> choix_type)) {
    if (choix_type <= 0 || choix_type > 3) resultat_creer_personnage = -1;
  }
  else resultat_creer_personnage = -1;

  Chateau Chateau_Temp{tour};
  if (tour) resultat_creer_personnage_f = Chateau_Temp.creer_personnage(Plateau, Info_Plateau->Chateaux_0[choix_chateau - 1].i, Info_Plateau->Chateaux_0[choix_chateau - 1].j, Info_Joueurs, choix_type, tour);
  else resultat_creer_personnage_f = Chateau_Temp.creer_personnage(Plateau, Info_Plateau->Chateaux_1[choix_chateau - 1].i, Info_Plateau->Chateaux_1[choix_chateau - 1].j, Info_Joueurs, choix_type, tour);
  
  if (resultat_creer_personnage > 0 || resultat_creer_personnage_f > 0){
    if (resultat_creer_personnage_f == -1 || resultat_creer_personnage == -1) cout << "Veillez choisir une option valide" << endl; 
    if (resultat_creer_personnage_f == -2) cout << "Vous n'avez pas assez d'or pour acheter ce pion" << endl;
    if (resultat_creer_personnage_f == -3) cout << "Il n'a a plus de place autour de ce château" << endl;
    sleep(1);
    affichage_debut_jeu(Plateau, Info_Plateau, Info_Joueurs, tour);
  }
}
