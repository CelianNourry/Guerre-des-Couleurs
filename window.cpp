/*!\file window.c
 *
 * \brief using GL4Dummies and Assimp Library to load 3D models or scenes.
 *
 * \author Farès Belhadj amsi@up8.edu
 * \date February 14 2017, modified on March 24, 2024
 */

#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include "assimp.h"
#include "jeu.cpp"
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include <iostream>

using namespace std;

#define X_INITIAL -2.5f
#define Y_INITIAL 2.5f
#define TAILLE_CASE 0.25f

/*!\brief opened window width */
static int _windowWidth = 1024;
/*!\brief opened window height */
static int _windowHeight = 768;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
static GLuint _quad = 0;
int NB_TOUR = 1;


bool tour = true;
//Le joeur veut créer un personnage depuis un château
int CC_POS_X, CC_POS_Y, CC_TYPE;
bool CC_which_one = false;
bool CC_which_character = false;
bool CC_confirmation = false;
bool did_CC_confirmation = false; //Savoir si on a déjà créer un chateau durant le tour

//Le joueur veut déplacer un personnage
int MC_POS_X, MC_POS_Y,  MC_POS_WHERE_X, MC_POS_WHERE_Y;
int* DEPLACEMENTS_RESTANTS = new int;
bool MC_which_one = false;
bool MC_where = false;
bool MC_confirmation = false;
bool did_MC_confirmation = false;

//Attaque d'autre pions
int ATK_POS_X, ATK_POS_Y, ATK_POS_WHERE_X, ATK_POS_WHERE_Y;
bool ATK_which_one = false;
bool ATK_where = false;
bool ATK_confirmation = false;
bool did_ATK_confirmation = false;

//Transformations de Seigneur à Château
int T_POS_X, T_POS_Y;
bool T_which_one = false;
bool T_confirmation = false;
bool did_T_confirmation = false;

bool mouseClickPending = false;
int mouseX = 0;
int mouseY = 0;

Pion ***Plateau = nullptr;
Info_Plateau *Info_Plateaus = nullptr;
Info_Joueurs Info_Joueurss;
bool initialisationPlateau = true;

GLuint _texId[2] = { 0 };
/*!\brief enum that index keyboard mapping for direction commands */
enum kyes_t {
	KLEFT = 0,
	KRIGHT,
	KUP,
	KDOWN,
	KPAGEUP,
	KPAGEDOWN,
	K_E, // Finir le tour
	K_C, //Veut créer un personnage depuis un château
	K_G, //Guerrier
	K_S, //Seigneur
	K_P, //Paysan
	K_M, //Déplacement personnage
	K_A, //Attaque
	K_T //Transformation du paysan en chateau
};

/*!\brief virtual keyboard for direction commands */
static GLuint _keys[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

GLfloat x = 0.0f, y = 0.0f, z = 1.0f;
GLfloat theta = 0.0f;

static GLboolean _pause = GL_TRUE;
static GLboolean _center_view = GL_FALSE;
static GLfloat rot[3] = {0, 0, 0};

static void init(void);
static void quit(void);
static void resize(int w, int h);
static void handleMouseEvents(void);
static void idle(void);
static void draw(void);
static void keydown(int keycode);
static void keyup(int keycode);
static float *caseChoisie(float x, float y);

/*!\brief identifiant de la scene générée par assimpGenScene */
static GLuint _id_scene = 0;
static GLuint _id_scene2 = 0;
static GLuint _id_scene3 = 0;
static GLuint _id_scene4 = 0;

/*!\brief the main function.*/
int main(int argc, char ** argv) {
	// Creation du plateau
	if (initialisationPlateau){
		Plateau = new Pion**[COLONNE];
		for (int i = 0; i < COLONNE; i++) {
			Plateau[i] = new Pion*[LIGNE];
			for (int j = 0; j < LIGNE; j++) {
				Plateau[i][j] = nullptr;
			}
		}

		Info_Joueurs Info_Joueurss;
		Info_Plateau *Info_Plateaus = new Info_Plateau[sizeof(Info_Plateau)];

		Chateau Chateau_Bleu{true};
		Pion* Pion1 = &Chateau_Bleu;
		Chateau Chateau_Rouge{false};
		Pion* Pion2 = &Chateau_Rouge;


		Pion1 -> SET_POS(9, 19);
		Plateau[9][19] = Pion1;

		Pion2 -> SET_POS(9, 0);
		Plateau[9][0] = Pion2;

		Info_Plateaus = refresh_plateau(Plateau);
		cout << Info_Plateaus -> Chateaux_0[0].i << endl;
	}

	initialisationPlateau = false;
	if(!gl4duwCreateWindow(argc, argv, "La Guerre des Couleurs", GL4DW_POS_UNDEFINED, GL4DW_POS_UNDEFINED,
	_windowWidth, _windowHeight, GL4DW_RESIZABLE | GL4DW_SHOWN)) return 1;

	_id_scene  = assimpGenScene("models/cfxidcuko5-Slenderman/Slenderman/Slenderman Model.obj");
	_id_scene2 = assimpGenScene("models/67-m26-tank/m26.obj");
	_id_scene3 = assimpGenScene("models/tcmi72kglyio-ft86ht/Castle grt67gh/Castle.obj");
	_id_scene4 = assimpGenScene("models/32-tractor/tractor/Tractor.obj");
	init();
	atexit(quit);
	gl4duwResizeFunc(resize);
	gl4duwKeyUpFunc(keyup);
	gl4duwKeyDownFunc(keydown);
	gl4duwDisplayFunc(draw);
	gl4duwIdleFunc(idle);
	gl4duwMainLoop();
  	return 0;
}


static void resize(int w, int h) {
  _windowWidth = w; 
  _windowHeight = h;
  glViewport(0.0f, 0.0f, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.005f, 0.005f, -0.005f * _windowHeight / _windowWidth, 0.005f * _windowHeight / _windowWidth, 0.01f, 1000.0f);
  gl4duBindMatrix("modelViewMatrix");
}

static void init(void) {
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	_quad = gl4dgGenQuadf();
	_pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);

	gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
	gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
	gl4duGenMatrix(GL_FLOAT, "model");

	glGenTextures(2, _texId);

	gl4duBindMatrix("projectionMatrix");
	gl4duLoadIdentityf();
	gl4duFrustumf(-1, 1, -0.75, 0.75, 2.5, 1000);
	resize(_windowWidth, _windowHeight);
}

// Fonction pour traiter les événements de souris
void handleMouseEvents() {
	int i, j;
    SDL_Event event;
    if (SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mouseX = event.button.x;
                    mouseY = event.button.y;
                    mouseClickPending = true;

					float *coordonnees = caseChoisie(mouseX, mouseY);

					if ((int)coordonnees[0] != -1 && (int)coordonnees[1] != -1){
						i = (int)coordonnees[0], j = (int)coordonnees[1];
						if (CC_which_one){
							if (Plateau[i][j] == nullptr || Plateau[i][j] -> type() != 'C' || Plateau[i][j] -> RETURN_OWNER() != tour) cout << "Veillez choisir un château qui vous appartient" << endl;
							else if (Plateau[i][j] -> type() == 'C'){
								CC_POS_X = i, CC_POS_Y = j;
								CC_which_character = true; //Input valide, on passe au choix du personnage à créer
								CC_which_one = false;
							}
						}
						else if (MC_which_one || ATK_which_one || T_which_one){
							if (Plateau[i][j] == nullptr || Plateau[i][j] -> type() == 'C' || Plateau[i][j] -> RETURN_OWNER() != tour) cout << "Veillez choisir un personnage à vous." << endl;
							else{
								if (MC_which_one){
									MC_POS_X = i, MC_POS_Y = j;
									MC_where = true;
									MC_which_one = false;
									cout << "La position du pion à bouger a été enregistrée" << endl;
									cout << "Veuillez choisir où ira le pion" << endl;
								}
								else if (ATK_which_one){
									if (Plateau[i][j] -> type() != 'P'){
										ATK_POS_X = i, ATK_POS_Y = j;
										ATK_where = true;
										ATK_which_one = false;
										cout << "La position du pion qui attaque a été enregistrée" << endl;
										cout << "Veuillez choisir qui attaquera le pion" << endl;
									}
									else cout << "Les paysans ne peuvent pas attaquer" << endl;
								}
								else if(T_which_one){
									if (Plateau[i][j] == nullptr) cout << "Veuillez choisir un pion" << endl;
									else if (Plateau[i][j] -> type() != 'S') cout << "Veuillez choisir un Seigneur" << endl;
									else if (Plateau[i][j] -> RETURN_OWNER() != tour) cout << "Veuillez choisir un pion qui vous appartient" << endl;
									else{
										T_POS_X = i, T_POS_Y = j;
										T_confirmation = true;
										T_which_one = false;
									}
								}
							}
						}
						else if (MC_where){
							if (Plateau[i][j] != nullptr) cout << "Veillez choisir un emplacement vide." << endl;
							else{
								cout << "Enplacement personnage reçu" << endl;
								MC_POS_WHERE_X = i, MC_POS_WHERE_Y = j;
								MC_confirmation = true;
								MC_where = false;
							}
						}
						else if (ATK_where){
							int nb_case_deplacee = 0;
							char direction;
							if (ATK_POS_X > i) direction = 'G'; //Gauche
							else if (ATK_POS_X < i) direction = 'D'; //Droite
							else if (ATK_POS_Y > j) direction = 'B'; //Bas
							else if (ATK_POS_Y < j) direction = 'H'; //Haut
							else direction = 'N'; // Valeur fallback

							if (direction == 'G') nb_case_deplacee = (ATK_POS_X + 1) - (i + 1);
							else if (direction == 'D') nb_case_deplacee = (i + 1) - (ATK_POS_X + 1);
							else if (direction == 'B') nb_case_deplacee = (ATK_POS_Y + 1) - (j + 1);
							else if (direction == 'H') nb_case_deplacee = (j + 1) - (ATK_POS_Y + 1);
							else direction = 'N'; // Valeur fallback

							//Vérifier si on ne va pas trop loin
							if (nb_case_deplacee > 1) cout << "Vous devez attaquer un pion devant vous" << endl;

							else if (i != ATK_POS_X && j != ATK_POS_Y) cout << "Vous ne pouvez pas attaquer en diagonale" << endl;
							else if (Plateau[i][j] == nullptr) cout << "Veuillez attaquer un pion" << endl;
							else if (Plateau[i][j] -> RETURN_OWNER() == tour) cout << "Le friendly fire n'est pas activé" << endl;

							else{
								cout << "Enplacement personnage reçu" << endl;
								ATK_POS_WHERE_X = i, ATK_POS_WHERE_Y = j;
								ATK_confirmation = true;
								ATK_where = false;
							}
						}
            		}
					else cout << "Veuillez choisir des coordonées sur le plateau" << endl;
                }
                break;
    		}
    	}
	}

static void idle(void) {
	static float t0 = 0.0f;
	float t, dt, dtheta = M_PI, step = 1.0f;

	dt = ((t = (float)gl4dGetElapsedTime()) - t0) / 1000.0f;
	t0 = t;

	if(_keys[KLEFT]) theta += dt * dtheta;
	if(_keys[KRIGHT]) theta -= dt * dtheta;
	if(_keys[KPAGEUP]) y += dt * 0.5f * step;
	if(_keys[KPAGEDOWN]) y -= dt * 0.5f * step;
	if(_keys[KUP]) {
		x += -dt * step * sin(theta);
		z += -dt * step * cos(theta);
	}
	if(_keys[KDOWN]) {
		x += dt * step * sin(theta);
		z += dt * step * cos(theta);
		_keys[KDOWN] = 0;
	}

	if (_keys[K_E] && !CC_which_one && !CC_which_character && !CC_confirmation && !MC_which_one && !MC_where && !MC_confirmation){
		Info_Plateaus = refresh_plateau(Plateau);
		Info_Joueurss.GAIN_PASSIF_OR(Plateau, Info_Plateaus);
		int or_joueur = Info_Joueurss.RETURN_OR(true), or_adversaire = Info_Joueurss.RETURN_OR(false);

		did_CC_confirmation = false;
		did_MC_confirmation = false;
		did_ATK_confirmation = false;
		did_T_confirmation = false;
		tour = !tour;
		cout << "Au tour de " << (tour ? "\x1b[34;1mdu joueur" : "\x1b[31;1mde l'adversaire") << endl << "\033[0mOr Joueur : " << or_joueur << endl << "Or Adversaire : " << or_adversaire << endl;
		_keys[K_E] = 0;
	}

	if(_keys[K_A]){
		if (did_ATK_confirmation){
			cout << "Vous avez déjà attaqué ce tour" << endl;
		}
		else if (did_CC_confirmation){
			cout << "Vous ne pouvez pas attaqué après avoir construit un château" << endl;
		}
		else {
			ATK_which_one = true;
			_keys[K_A] = 0; // A mettre absolument
		}
	}

	if (ATK_which_one){
		handleMouseEvents();
	}
	if (ATK_where){
		handleMouseEvents();
		did_ATK_confirmation = true;
	}
	if (ATK_confirmation){
		int resultat = Plateau[ATK_POS_X][ATK_POS_Y] -> attaque(Plateau, ATK_POS_X, ATK_POS_Y, ATK_POS_WHERE_X, ATK_POS_WHERE_Y);
		int vie_adversiare = Plateau[ATK_POS_WHERE_X][ATK_POS_WHERE_Y] -> GET_PV();
		int dmg_pion = Plateau[ATK_POS_X][ATK_POS_Y] -> GET_ATK();
		if (resultat == 1) cout << "Vous avez tué le pion" << endl;
		else if (resultat == 0) cout << "Vous avez enlevé " << dmg_pion << "PV au pion, il lui reste " << vie_adversiare << "PV" << endl;
		ATK_confirmation = false;
	}
	//Création de personnages via châteaux
	if(_keys[K_C]){
		if (!did_CC_confirmation){
			cout << "Choisissez un Château" << endl;
			CC_which_one = true;
			_keys[K_C] = 0; // A mettre absolument
		}
		else {
			cout << "Vous avez déjà placé un personnage durant ce tour, veillez le terminer" << endl;
		}
	}

	if (CC_which_one == true){
		handleMouseEvents();
	}

	if (CC_which_character && _keys[K_G]){
		cout << "Guerrier" << endl;
		CC_TYPE = 1;
		CC_which_character = false;
		CC_confirmation = true;
	}
	if (CC_which_character && _keys[K_P]){
		cout << "Paysan" << endl;
		CC_TYPE = 2;
		CC_which_character = false;
		CC_confirmation = true;
	}
	if (CC_which_character && _keys[K_S]){
		cout << "Seigneur" << endl;
		CC_TYPE = 3;
		CC_which_character = false;
		CC_confirmation = true;
	}

	if (CC_confirmation){
		Chateau nouveauChateau(tour);

    	int resultat = nouveauChateau.creer_personnage(Plateau, CC_POS_X, CC_POS_Y, &Info_Joueurss, CC_TYPE, tour);
		if (resultat == -1) cout << "Mauvais imput" << endl;
		else if (resultat == -2) cout << "Pas assez d'or" << endl;
		else if (resultat == -3) cout << "Pas de place autour du château" << endl;
		else cout << "Le châyeau a bien été créé" << endl;
		CC_confirmation = false;
		did_CC_confirmation = true;
	}

	if(_keys[K_M] && !did_CC_confirmation && !did_ATK_confirmation){ //Déplacement
		cout << "Choisissez un Pion à déplacer" << endl;
		MC_which_one = true;
		_keys[K_M] = 0; // A mettre absolument
	}

	if (MC_which_one){
		handleMouseEvents();
	}
	if (MC_where){
		if (!did_MC_confirmation) *DEPLACEMENTS_RESTANTS = Plateau[MC_POS_X][MC_POS_Y] -> GET_VITS(); // On stocke de combien de case le pion peut nse déplacer
		if (*DEPLACEMENTS_RESTANTS > 0){
			did_MC_confirmation = true;
			handleMouseEvents();
		}
		else{
			cout << "Vous ne pouvez pas aller plus loin ce tour" << endl;
			MC_where = false;
		}
	}
	if (MC_confirmation){	
		int resultat = Plateau[MC_POS_X][MC_POS_Y] -> deplacement(Plateau, MC_POS_X, MC_POS_Y, MC_POS_WHERE_X, MC_POS_WHERE_Y, DEPLACEMENTS_RESTANTS);
		if (resultat == -1) cout << "Impossible de naviguer en diagonale avec ce pion" << endl;
		else if (resultat == -2) cout << "Vous ne pas vous déplacer aussi loin" << endl;
		else if (resultat == -3) cout << "Il y a quelque chose qui vous barre la route" << endl;
		MC_confirmation = false;
	
	}
	if (_keys[K_T]){
		if (!did_T_confirmation){
			cout << "Choisissez un Seigneur à transformer" << endl;
			T_which_one = true;
			_keys[K_T] = 0; // A mettre absolument
		}
		else{
			cout << "Vous avez déjà transformé un Seigneur en Château durant ce tour" << endl;
		}

	}
	if (T_which_one){
		handleMouseEvents();
	}
	if (T_confirmation){
		Seigneur SeigneurTemp(tour);
		int resultat = SeigneurTemp.transformation(tour, &Info_Joueurss, Plateau, T_POS_X, T_POS_Y);
		if (resultat == 0) cout << "Le Seigneur s'est bien transformé en Château" << endl;
		else if (resultat == -1) cout << "Vous n'avez pas assez d'or" << endl;
		T_confirmation = false;
	}

	if(!_pause) rot[1] += 90.0f * dt;
}

static void keydown(int keycode) {
	GLint v[2];
	switch(keycode) {
		case GL4DK_LEFT:
			_keys[KLEFT] = 1;
			break;
		case GL4DK_RIGHT:
			_keys[KRIGHT] = 1;
			break;
		case GL4DK_UP:
			_keys[KUP] = 1;
			break;
		case GL4DK_DOWN:
			_keys[KDOWN] = 1;
			break;
		case GL4DK_d:
			_keys[KPAGEDOWN] = 1;
			break;
		case GL4DK_u:
			_keys[KPAGEUP] = 1;
			break;
		case GL4DK_e:
			_keys[K_E] = 1;
			break;
		case GL4DK_c:
			_keys[K_C] = 1;
			break;
		case GL4DK_g:
			_keys[K_G] = 1;
			break;
		case GL4DK_p:
			_keys[K_P] = 1;
			break;
		case GL4DK_s:
			_keys[K_S] = 1;
			break;
		case GL4DK_m:
			_keys[K_M] = 1;
			break;
		case GL4DK_a:
			_keys[K_A] = 1;
			break;
		case GL4DK_t:
			_keys[K_T] = 1;
			break;
		case GL4DK_ESCAPE:
			case 'q':
				exit(0);
			/* when 'w' pressed, toggle between line and filled mode */
			case 'w':
				glGetIntegerv(GL_POLYGON_MODE, v);
				if(v[0] == GL_FILL) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glLineWidth(3.0f);
				} else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glLineWidth(1.0f);
				}
			break;
		case GL4DK_SPACE:
			_pause = !_pause;
			break;
		default:
			break;
	}
}

static void keyup(int keycode) {
	switch(keycode) {
		case GL4DK_LEFT:
			_keys[KLEFT] = 0;
			break;
		case GL4DK_RIGHT:
			_keys[KRIGHT] = 0;
			break;
		case GL4DK_UP:
			_keys[KUP] = 0;
			break;
		case GL4DK_DOWN:
			_keys[KDOWN] = 0;
			break;
		case GL4DK_d:
			_keys[KPAGEDOWN] = 0;
			break;
		case GL4DK_u:
			_keys[KPAGEUP] = 0;
			break;
		case GL4DK_e:
			_keys[K_E] = 0;
			break;
		case GL4DK_c:
			_keys[K_C] = 0;
			break;
		case GL4DK_g:
			_keys[K_G] = 0;
			break;
		case GL4DK_p:
			_keys[K_P] = 0;
			break;
		case GL4DK_s:
			_keys[K_S] = 0;
			break;
		case GL4DK_m:
			_keys[K_M] = 0;
			break;
		case GL4DK_a:
			_keys[K_A] = 0;
			break;
		case GL4DK_t:
			_keys[K_T] = 0;
			break;
		default:
			break;
	}
}

// Case choisie par l'évenement de click de souris
static float *caseChoisie(float x, float y){
	float x_0 = 245.0f, x_19 = 755.0f;
	float y_0 = 115.0f, y_19 = 625.0f;
	static float coordonnees[2];

	if (x >= x_0 && x <= x_19){
		for (float i = 0.0f, caseActuelle = x_0; (int)i < COLONNE; i++, caseActuelle += (x_19 - x_0) / COLONNE){
			if (x > caseActuelle) coordonnees[0] = i;
		}
	}
	else coordonnees[0] = -1.0f; // Cas où le lick dépasse le tableau

	if (y >= y_0 && y <= y_19){
		for (float i = 0.0f, caseActuelle = y_0; (int)i < LIGNE; i++, caseActuelle += (y_19 - y_0) / LIGNE){
			if (y > caseActuelle) coordonnees[1] = i;
		}
	}
	else coordonnees[1] = -1.0f;

    return coordonnees;
}

static void draw(void) {
	GLfloat lum[4] = {0.0f, 0.0f, 5.0f, 1.0f};
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(_pId);

	glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);
	gl4duBindMatrix("modelViewMatrix");
	gl4duLoadIdentityf();

	gl4duLookAtf(x, y, z,
	       x - sin(theta),  _center_view ? 0.0f : y,  z - cos(theta),
	       0.0f, 1.0f, 0.0f);
  
  	gl4duRotatef(rot[1], 0.0f, 1.0f, 0.0f);
	gl4duPushMatrix();

	float y = Y_INITIAL;
	for (int i = 0; i < LIGNE; i++) {
		float x = X_INITIAL;
		for (int j = 0; j < COLONNE; j++) {
			if (Plateau[j][i] != nullptr){
				gl4duLoadIdentityf();
                gl4duTranslatef(x, y, -10.0f);
                gl4duScalef(TAILLE_CASE, TAILLE_CASE, TAILLE_CASE);
                gl4duSendMatrices();
				if(Plateau[j][i] -> type() == 'S') assimpDrawScene(_id_scene);
				else if(Plateau[j][i] -> type() == 'C') assimpDrawScene(_id_scene3);
				else if(Plateau[j][i] -> type() == 'P') assimpDrawScene(_id_scene4);
				else if(Plateau[j][i] -> type() == 'G') assimpDrawScene(_id_scene2);
				gl4duPopMatrix();

				Plateau[j][i] -> RETURN_OWNER()
				? glUniform4f(glGetUniformLocation(_pId, "diffuse_color"), 0.0f, 0.0f, 0.9f, 0.8f)
				: glUniform4f(glGetUniformLocation(_pId, "diffuse_color"), 0.9f, 0.0f, 0.0f, 0.8f);
				
			}
			else{
				((i + j) % 2 == 0)
				? glUniform4f(glGetUniformLocation(_pId, "diffuse_color"), 0.0f, 0.0f, 0.0f, 0.8f)
				: glUniform4f(glGetUniformLocation(_pId, "diffuse_color"), 1.0f, 1.0f, 1.0f, 0.8f);
			}

			gl4duLoadIdentityf();
			gl4duTranslatef(x, y, -10.0f);
			gl4duScalef(TAILLE_CASE / 2, TAILLE_CASE / 2, TAILLE_CASE / 2);
			gl4duSendMatrices();

			gl4dgDraw(_quad);

			x += TAILLE_CASE;
		}
		y -= TAILLE_CASE;
	}
}

/*!\brief function called at exit, it cleans all created GL4D objects.*/
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}

