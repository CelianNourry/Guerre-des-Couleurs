#include "pions.hpp"
#include <unistd.h>

#define OR_INITIAL 500

int Pion::deplacement(const char orientation){
    if (orientation == 'H')
        this -> y -= VITS;
    else if (orientation == 'B')
        this -> y += VITS;
    else if (orientation == 'D')
        this -> x += VITS;
    else if (orientation == 'G')
        this -> x -= VITS;
    else {
        cout << "Erreur" << endl;
        return -1;
    }
    return 0;
}

void Pion::affiche(){
    cout << "(" << x << ", " << y << ")" << endl;
    cout << owned << endl;
    cout << TYPE << endl;
    cout << ATK << endl;
    cout << PV << endl;
    cout << PROD << endl;
    cout << VITS << endl;
    cout << COUT << endl;
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
    cout << "Le joueur a dépenser " << Pion_Resultant -> GET_COUT() << " d'or" << endl;
    sleep(2);
    return 0;
}

Guerrier::Guerrier(bool owner)
    : Pion(owner){
    TYPE = 1;
    PV = 10;
    ATK = 5;
    COUT = 10;
}

Paysan::Paysan(bool owner)
    : Pion(owner){
    TYPE = 2;
    PV = 1;
    PROD = 5;
    COUT = 20;
}

Seigneur::Seigneur(bool owner)
    : Pion(owner){
    TYPE = 3;
    PV = 5;
    ATK = 3;
    COUT = 10;
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
    tour == 1 ? OR_JOUEUR_0 -= cout : OR_JOUEUR_1 -= cout;
}

int Info_Joueurs::RETURN_OR(bool tour){
    if (tour) return OR_JOUEUR_0;
    return OR_JOUEUR_1;
}