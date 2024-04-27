#ifndef PION_HPP
#define PION_HPP

#include <iostream>
using namespace std;

#define LIGNE 20
#define COLONNE 20

struct Info_Joueurs;
// Classe de base d'un pion
class Pion {
protected:
    int x;
    int y;
    bool owned;

    int TYPE;
    int ATK;
    int PV;
    int PROD;
    int VITS;
    int COUT;

public:
    Pion(bool owner)
        : owned(owner) {}
    int deplacement(Pion*** Plateau, int MC_POS_X, int MC_POS_Y, int MC_POS_WHERE_X, int MC_POS_WHERE_Y, int *DEPLACEMENTS_RESTANTS);
    void affiche();
    char type(void);
    int production(void);
    //Retourne à qui appartient le pion
    bool RETURN_OWNER(void);
    int GET_COUT(void);
    int GET_VITS(void);
    int SET_POS(int x, int y);
};

class Chateau : public Pion {
public:
    Chateau(bool owner);
    int creer_personnage(Pion*** Plateau, int pos_x, int pos_y, Info_Joueurs *Info_Joueurs, int type, bool tour);
};

class Guerrier : public Pion {
public:
    Guerrier(bool owner);
};

class Paysan : public Pion {
public:
    Paysan(bool owner);
};

class Seigneur : public Pion {
public:
    Seigneur(bool owner);
};

struct Info_Pion {
    int i;
    int j;

    Info_Pion(void)
        : i(-1), j(-1) {}
};

// Infos générées par le plateau actuel
struct Info_Plateau {
    Info_Pion* Chateaux_0;
    Info_Pion* Guerriers_0;
    Info_Pion* Paysans_0;
    Info_Pion* Seigneurs_0;

    Info_Pion* Chateaux_1;
    Info_Pion* Guerriers_1;
    Info_Pion* Paysans_1;
    Info_Pion* Seigneurs_1;

    Info_Plateau();
    ~Info_Plateau();
};

struct Info_Joueurs {
    int OR_JOUEUR_0;
    int OR_JOUEUR_1;

    Info_Joueurs(void);
    void GAIN_PASSIF_OR(Pion ***Plateau, Info_Plateau *Info_Plateau);
    void RETIRER_OR(bool tour, int cout);
    int RETURN_OR(bool tour);
};

#endif // PION_HPP
