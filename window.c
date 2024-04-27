/*!\file window.c
*
* \brief using GL4Dummies and Assimp Library to load 3D models or scenes.
*
* \author Farès Belhadj amsi@up8.edu
* \date February 14 2017, modified on March 24, 2024
*/
#include "Jeu/jeu.cpp"
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>
#include <SDL_image.h>

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
int a = 0;


bool tour = true;
//Le joeur veut créer un personnage depuis un château
int CC_POS_X, CC_POS_Y, CC_TYPE;
bool CC_which_one = false;
bool CC_which_character = false;
bool CC_confirmation = false;

//Le joueur veut déplacer un personnage
int MC_POS_X, MC_POS_Y,  MC_POS_WHERE_X, MC_POS_WHERE_Y;
int* DEPLACEMENTS_RESTANTS = new int;
bool MC_which_one = false;
bool MC_where = false;
bool MC_confirmation = false;

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
	K_E,
	K_C, //Veut créer un personnage depuis un château
	K_G, //Guerrier
	K_S, //Seigneur
	K_P, //Paysan
	K_M //Déplacement personnage
};

/*!\brief virtual keyboard for direction commands */
static GLuint _keys[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

typedef struct cam_t cam_t;
/*!\brief a data structure for storing camera position and
* orientation */
struct cam_t {
GLfloat x, y, z;
GLfloat theta;
};

/*!\brief the used camera */
static cam_t _cam = {0.0f, 0.0f, 1.0f, 0.0f};

/*!\brief toggle y-axis rotation pause */
static GLboolean _pause = GL_TRUE;

/*!\brief toggle view focused on the scene center */
static GLboolean _center_view = GL_FALSE;

/*!\brief rotation angles according to axis (0 = x, 1 = y, 2 = z) 
* \todo améliorer l'interface et ajouter rotations/zoom à la souris */
static GLfloat rot[3] = {0, 0, 0};

static void init(void);
static void quit(void);
static void resize(int w, int h);
static void handleMouseEvents(void);
static void idle(void);
static void draw(void);
static void keydown(int keycode);
static void keyup(int keycode);
static void jeu(void);
static float *caseChoisie(float x, float y);

static void init(void) {
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	_quad = gl4dgGenQuadf();
	_pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);

	gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
	gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
	gl4duGenMatrix(GL_FLOAT, "model");

	GLuint pixels[] = { RGBA(255, 255, 255, 255), 0 };
	glGenTextures(2, _texId);

	gl4duBindMatrix("projectionMatrix");
	gl4duLoadIdentityf();
	gl4duFrustumf(-1, 1, -0.75, 0.75, 2.5, 1000);
	resize(_windowWidth, _windowHeight);

	
	glBindTexture(GL_TEXTURE_2D, _texId[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_Surface * s = SDL_LoadBMP("images/A.bmp");
	assert(s);
	assert(s->format->BytesPerPixel == 3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
	SDL_FreeSurface(s);
  	glBindTexture(GL_TEXTURE_2D, 0); 
}

int main(int argc, char ** argv) {
	//Creation du plateau
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
		Paysan Paysans{true};
		Pion* Pion_P = &Paysans;
		Chateau Chateau_Rouge{false};
		Pion* Pion2 = &Chateau_Rouge;

		Pion1 -> SET_POS(9, 19);
		Plateau[9][19] = Pion1;

		Pion2 -> SET_POS(9, 0);
		Plateau[9][0] = Pion2;

		Pion_P -> SET_POS(9, 9);
		Plateau[9][9] = Pion_P;

		Info_Plateaus = refresh_plateau(Plateau);
		cout << Info_Plateaus->Chateaux_0[0].i << endl;

		Plateau[9][0] -> affiche();
	}

	initialisationPlateau = false;
	if(!gl4duwCreateWindow(argc, argv, "La Guerre des Couleurs", GL4DW_POS_UNDEFINED, GL4DW_POS_UNDEFINED,
	_windowWidth, _windowHeight, GL4DW_RESIZABLE | GL4DW_SHOWN)) return 1;


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
					//cout << (int)coordonnees[0] << ", " << (int)coordonnees[1] << endl;

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
						else if (MC_which_one){
							if (Plateau[i][j] == nullptr || Plateau[i][j] -> type() == 'C' || Plateau[i][j] -> RETURN_OWNER() != tour) cout << "Veillez choisir un personnage à vous." << endl;
							else{
								cout << "Emplacement personnage reçu" << endl;
								*DEPLACEMENTS_RESTANTS = Plateau[i][j] -> GET_VITS(); // On stocke de combien de case le pion peut nse déplacer
								cout << *DEPLACEMENTS_RESTANTS << endl;
								cout << Plateau[i][j] -> GET_VITS() << endl;
								MC_POS_X = i, MC_POS_Y = j;
								MC_where = true;
								MC_which_one = false;
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
            		}
                }
                break;
        }
    }
}
static void idle(void) {
	int i, j;
	static float t0 = 0.0f;
	float t, dt, dtheta = M_PI, step = 1.0f;
	dt = ((t = (float)gl4dGetElapsedTime()) - t0) / 1000.0f;
	t0 = t;

	if (_keys[K_E] && !CC_which_one && !CC_which_character && !CC_confirmation && !MC_which_one && !MC_where && !MC_confirmation){
		tour = !tour;
		cout << "Au tour de " << (tour ? "du joueur" : "de l'adversaire") << endl;
		_keys[K_E] = 0;
	}
	if(_keys[KLEFT]) _cam.theta += dt * dtheta;
	if(_keys[KRIGHT]) _cam.theta -= dt * dtheta;
	if(_keys[KPAGEUP]) _cam.y += dt * 0.5f * step;
	if(_keys[KPAGEDOWN]) _cam.y -= dt * 0.5f * step;
	if(_keys[KUP]) {
	_cam.x += -dt * step * sin(_cam.theta);
	_cam.z += -dt * step * cos(_cam.theta);
	}
	if(_keys[KDOWN]) {
	_cam.x += dt * step * sin(_cam.theta);
	_cam.z += dt * step * cos(_cam.theta);
	}

	//Création de personnages via châteaux
	if(_keys[K_C]){
		CC_which_one = true;
		_keys[K_C] = 0; // A mettre absolument
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
		if (resultat == 0){
			Info_Plateaus = refresh_plateau(Plateau);
		}
		else{
			if (resultat == -1) cout << "Mauvais imput" << endl;
			else if (resultat == -2) cout << "Pas assez d'or" << endl;
			else if (resultat == -3) cout << "Pas de place autour du château" << endl;
		}
		CC_confirmation = false;
	}

	if(_keys[K_M]){
		MC_which_one = true;
		_keys[K_M] = 0; // A mettre absolument
	}

	if (MC_which_one){
		handleMouseEvents();
	}
	if (MC_where){
		//cout << "Emplacement personnage reçu" << endl;
		//cout << DEPLACEMENTS_RESTANTS << endl;
		if (*DEPLACEMENTS_RESTANTS > 0) handleMouseEvents();
		else MC_confirmation = false;
	}
	if (MC_confirmation){
		
		int resultat = Plateau[MC_POS_X][MC_POS_Y] -> deplacement(Plateau, MC_POS_X, MC_POS_Y, MC_POS_WHERE_X, MC_POS_WHERE_Y, DEPLACEMENTS_RESTANTS);
		if (resultat == -1) cout << "Impossible de naviguer en diagonale avec ce pion" << endl;
		else if (resultat == -2) cout << "Vous ne pas vous déplacer aussi loin" << endl;
		else if (resultat == -3) cout << "Il y a quelque chose qui vous barre la route" << endl;
		MC_confirmation = false;
	
	}
	if(!_pause) rot[1] += 90.0f * dt;
	jeu();
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
		/*
		case GL4DK_c:
			_center_view = !_center_view;
			break;
		*/
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
		default:
			break;
	}
}

static void jeu(void) {
}

//Case choisie par l'évenement de click de souris
static float *caseChoisie(float x, float y){
	float x_0 = 245.0f, x_19 = 755.0f;
	float y_0 = 115.0f, y_19 = 625.0f;
	static float coordonnees[2];

	if (x >= x_0 && x <= x_19){
		for (float i = 0.0f, caseActuelle = x_0; (int)i < COLONNE; i++, caseActuelle += (x_19 - x_0) / COLONNE){
			if (x > caseActuelle) coordonnees[0] = i;
		}
	}
	else coordonnees[0] = -1.0f; //Cas où le lick dépasse le tableau

	if (y >= y_0 && y <= y_19){
		for (float i = 0.0f, caseActuelle = y_0; (int)i < LIGNE; i++, caseActuelle += (y_19 - y_0) / LIGNE){
			if (y > caseActuelle) coordonnees[1] = i;
		}
	}
	else coordonnees[1] = -1.0f;

    return coordonnees;
}

static void draw() {
	GLfloat lum[4] = {0.0f, 0.0f, 5.0f, 1.0f};
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(_pId);

	glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

	gl4duBindMatrix("modelViewMatrix");
	gl4duLoadIdentityf();

	gl4duLookAtf(0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);

	float y = Y_INITIAL;
	for (int i = 0; i < LIGNE; i++) {
		float x = X_INITIAL;
		for (int j = 0; j < COLONNE; j++) {
			if (Plateau[j][i] != nullptr){
				glActiveTexture(GL_TEXTURE0);
				/* on bind la texture _texId[0] */
				glBindTexture(GL_TEXTURE_2D, _texId[0]);
				glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);

				//Plateau[j][i] -> affiche();
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

	glUseProgram(0); // remise du programme à 0 à la fin
}

static void quit(void) {
	gl4duClean(GL4DU_ALL);
}

