#include "pions.hpp"
#include <unistd.h>

#define OR_INITIAL 500

int Pion::deplacement(Pion*** Plateau, int MC_POS_X, int MC_POS_Y, int MC_POS_WHERE_X, int MC_POS_WHERE_Y, int *DEPLACEMENTS_RESTANTS){
    char direction = 'N';
    int nb_case_deplacee = 0;
    if (MC_POS_WHERE_X != MC_POS_X && MC_POS_WHERE_Y != MC_POS_Y) return -1;

    if (MC_POS_X > MC_POS_WHERE_X) direction = 'G'; //Gauche
    else if (MC_POS_X < MC_POS_WHERE_X) direction = 'D'; //Droite
    else if (MC_POS_Y > MC_POS_WHERE_Y) direction = 'B'; //Bas
    else if (MC_POS_Y < MC_POS_WHERE_Y) direction = 'H'; //Haut

    if (direction == 'G') nb_case_deplacee = (MC_POS_X + 1) - (MC_POS_WHERE_X + 1);
    else if (direction == 'D') nb_case_deplacee = (MC_POS_WHERE_X + 1) - (MC_POS_X + 1);
    else if (direction == 'B') nb_case_deplacee = (MC_POS_Y + 1) - (MC_POS_WHERE_Y + 1);
    else if (direction == 'H') nb_case_deplacee = (MC_POS_WHERE_Y + 1) - (MC_POS_Y + 1);
    else nb_case_deplacee = 0;

    //Vérifier si on ne va pas trop loin
    if (nb_case_deplacee > *DEPLACEMENTS_RESTANTS) return -2;

    //Vérifier s'il y a des autres pions sur le chemin
    for (int i = nb_case_deplacee; i > 0; i--){
        int MC_POS_X_TEMP, MC_POS_Y_TEMP;
        MC_POS_X_TEMP = MC_POS_X, MC_POS_Y_TEMP = MC_POS_Y;
        if (direction == 'G'){
            MC_POS_X_TEMP--;
            if (Plateau[MC_POS_X_TEMP][MC_POS_Y_TEMP] != nullptr) return -3;
        }
        else if (direction == 'D'){
            MC_POS_X_TEMP++;
            if (Plateau[MC_POS_X_TEMP][MC_POS_Y_TEMP] != nullptr) return -3;
        }
        else if (direction == 'B'){
            MC_POS_Y_TEMP--;
            if (Plateau[MC_POS_X_TEMP][MC_POS_Y_TEMP] != nullptr) return -3;
        }
        else if (direction == 'H'){
            MC_POS_Y_TEMP++;
            if (Plateau[MC_POS_X_TEMP][MC_POS_Y_TEMP] != nullptr) return -3;
        }
    }
    *DEPLACEMENTS_RESTANTS -= nb_case_deplacee;
    Plateau[MC_POS_WHERE_X][MC_POS_WHERE_Y] = Plateau[MC_POS_X][MC_POS_Y];
    Plateau[MC_POS_X][MC_POS_Y] = nullptr;
    return 0;
}

int Pion::attaque(Pion*** Plateau, int ATK_POS_X, int ATK_POS_Y, int ATK_POS_WHERE_X, int ATK_POS_WHERE_Y){
    if (Plateau[ATK_POS_WHERE_X][ATK_POS_WHERE_Y] -> GET_PV() - Plateau[ATK_POS_X][ATK_POS_Y] -> GET_ATK() < 1){
        Plateau[ATK_POS_WHERE_X][ATK_POS_WHERE_Y] = nullptr;
        return 1;
    }
    else {
        Plateau[ATK_POS_WHERE_X][ATK_POS_WHERE_Y] -> REMOVE_PV(Plateau[ATK_POS_X][ATK_POS_Y] -> GET_ATK());
        return 0;
    }
}

char Pion::type(){
    if (this -> TYPE == 0) return 'C';
    else if (this -> TYPE == 1) return 'G';
    else if (this -> TYPE == 2) return 'P';
    else if (this -> TYPE == 3) return 'S';
    return 'O';
}

int Pion::production(){
    return this -> PROD;
}

bool Pion::RETURN_OWNER(){
    return this -> owned;
}

int Pion::GET_COUT(){
    return this -> COUT;
}
int Pion::GET_VITS(){
    return this -> VITS;
}
int Pion::GET_ATK(){
    return this -> ATK;
}
int Pion::GET_PV(){
    return this -> PV;
}

int Pion::REMOVE_PV(int dmg){
    this -> PV -= dmg;
    return 0;
}

int Pion::SET_POS(int x, int y){
    if (x >= 0 && x < LIGNE && y >= 0 && y < COLONNE) {
        this -> x = x;
        this -> y = y;
        return 0;
    }
    return -1;
}

Chateau::Chateau(bool owner) : Pion(owner) {
    TYPE = 0;
    PV = 20;
    PROD = 2;
    COUT = 15;
    VITS = 0;
}

int Chateau::creer_personnage(Pion*** Plateau, int pos_x, int pos_y, Info_Joueurs *Info_Joueurs, int type, bool owner) {
    Pion* Pion_Resultant;
    if (type == 1) Pion_Resultant = new Guerrier(owner);
    else if (type == 2) Pion_Resultant = new Paysan(owner);
    else if (type == 3) Pion_Resultant = new Seigneur(owner);
    else return -1; // Mauvais input

    if (Info_Joueurs -> RETURN_OR(owner) - (Pion_Resultant -> GET_COUT()) < 0)
        return -2; // Pas assez d'or
    
    // Placement du pion sur le plateau
    if (pos_y - 1 >= 0 && Plateau[pos_x][pos_y - 1] == nullptr && pos_x >= 0 && pos_x < LIGNE && pos_y >= 0 && pos_y < COLONNE) Plateau[pos_x][pos_y - 1] = Pion_Resultant;
    else if (pos_y + 1 < COLONNE && Plateau[pos_x][pos_y + 1] == nullptr && pos_x >= 0 && pos_x < LIGNE && pos_y >= 0 && pos_y < COLONNE) Plateau[pos_x][pos_y + 1] = Pion_Resultant;
    else if (pos_x - 1 >= 0 && Plateau[pos_x - 1][pos_y] == nullptr && pos_x >= 0 && pos_x < LIGNE && pos_y >= 0 && pos_y < COLONNE) Plateau[pos_x - 1][pos_y] = Pion_Resultant;
    else if (pos_x + 1 < LIGNE && Plateau[pos_x + 1][pos_y] == nullptr && pos_x >= 0 && pos_x < LIGNE && pos_y >= 0 && pos_y < COLONNE) Plateau[pos_x + 1][pos_y] = Pion_Resultant;
    else return -3;

    Info_Joueurs -> RETIRER_OR(owner, Pion_Resultant -> GET_COUT());
    sleep(2);
    return 0;
}

Guerrier::Guerrier(bool owner)
    : Pion(owner){
    TYPE = 1;
    PV = 10;
    ATK = 5;
    COUT = 10;
    VITS = 3;
}

Paysan::Paysan(bool owner)
    : Pion(owner){
    TYPE = 2;
    PV = 1;
    PROD = 5;
    COUT = 20;
    VITS = 2;
}

Seigneur::Seigneur(bool owner)
    : Pion(owner){
    TYPE = 3;
    PV = 5;
    ATK = 3;
    COUT = 10;
    VITS = 1;
}

int Seigneur::transformation(bool tour, Info_Joueurs *Info_Joueurs, Pion*** Plateau, int T_POS_X, int T_POS_Y){
    //On créer un nouveau château à la position du Seigneur si le joueur a assez d'or
    if ((Info_Joueurs -> RETURN_OR(tour) - 20) > 0){
        Chateau* Chateau_Temp = new Chateau(tour);
        if (Chateau_Temp == nullptr) return -2;

        Plateau[T_POS_X][T_POS_Y] = Chateau_Temp;
        Info_Joueurs -> RETIRER_OR(tour, 20);

        return 0;
    }
    else {
        return -1; // Pas assez d'or pour la transformation
    }
}

Info_Plateau::Info_Plateau(void){
    Chateaux_0 = new Info_Pion[COLONNE * LIGNE];
    Guerriers_0 = new Info_Pion[COLONNE * LIGNE];
    Paysans_0 = new Info_Pion[COLONNE * LIGNE];
    Seigneurs_0 = new Info_Pion[COLONNE * LIGNE];

    Chateaux_1 = new Info_Pion[COLONNE * LIGNE];
    Guerriers_1 = new Info_Pion[COLONNE * LIGNE];
    Paysans_1 = new Info_Pion[COLONNE * LIGNE];
    Seigneurs_1 = new Info_Pion[COLONNE * LIGNE];
}

Info_Plateau::~Info_Plateau(void){
    delete[] Chateaux_0;
    delete[] Guerriers_0;
    delete[] Paysans_0;
    delete[] Seigneurs_0;

    delete[] Chateaux_1;
    delete[] Guerriers_1;
    delete[] Paysans_1;
    delete[] Seigneurs_1;
}

Info_Joueurs::Info_Joueurs(void)
    : OR_JOUEUR_0(OR_INITIAL), OR_JOUEUR_1(OR_INITIAL){}

//Donne l'or au bon joueur à chaque tour
void Info_Joueurs::GAIN_PASSIF_OR(Pion ***Plateau, Info_Plateau *Info_Plateau){
    for (int i = 0;  Info_Plateau -> Chateaux_0[i].i != -1; i++){
        this -> OR_JOUEUR_0 += Plateau[Info_Plateau -> Chateaux_0[i].i][Info_Plateau -> Chateaux_0[i].j] -> production();
    }

    for (int i = 0;  Info_Plateau -> Chateaux_1[i].i != -1; i++){
        this -> OR_JOUEUR_1 += Plateau[Info_Plateau -> Chateaux_1[i].i][Info_Plateau -> Chateaux_1[i].j] -> production();
    }
}

void Info_Joueurs::RETIRER_OR(bool tour, int cout){
    tour == true ? OR_JOUEUR_0 -= cout : OR_JOUEUR_1 -= cout;
}

int Info_Joueurs::RETURN_OR(bool tour){
    if (tour) return OR_JOUEUR_0;
    return OR_JOUEUR_1;
}
