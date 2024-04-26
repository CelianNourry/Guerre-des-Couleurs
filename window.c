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

/*!\brief opened window width */
static int _windowWidth = 1024;
/*!\brief opened window height */
static int _windowHeight = 768;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
static GLuint _quad = 0;
int a = 0;

Pion ***Plateau = nullptr;
bool initialisationPlateau = true;

GLuint _texId[2] = { 0 };
/*!\brief enum that index keyboard mapping for direction commands */
enum kyes_t {
KLEFT = 0,
KRIGHT,
KUP,
KDOWN,
KPAGEUP,
KPAGEDOWN
};

/*!\brief virtual keyboard for direction commands */
static GLuint _keys[] = {0, 0, 0, 0, 0, 0};

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
static void idle(void);
static void draw(void);
static void keydown(int keycode);
static void keyup(int keycode);

static void init(void) {
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	/* on active le test de profondeur */
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

		Info_Joueurs *Info_Joueurss = new Info_Joueurs[sizeof(Info_Joueurs)];
		Info_Plateau *Info_Plateaus = new Info_Plateau[sizeof(Info_Plateau)];

		Chateau Chateau_Bleu{true};
		Pion* Pion1 = &Chateau_Bleu;
		Chateau Chateau_Rouge{false};
		Pion* Pion2 = &Chateau_Rouge;
		Chateau Chateau_Roug{false};

		Pion1 -> SET_POS(9, 19);
		Plateau[9][19] = Pion1;

		Pion2 -> SET_POS(9, 0);
		Plateau[9][0] = Pion2;

		Plateau[9][0] -> affiche();

		bool tour = true;
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

static void idle(void) {
	static float t0 = 0.0f;
	float t, dt, dtheta = M_PI, step = 1.0f;
	dt = ((t = (float)gl4dGetElapsedTime()) - t0) / 1000.0f;
	t0 = t;
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
		case GL4DK_c:
			_center_view = !_center_view;
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
		default:
			break;
	}
}

static void draw() {
	Info_Plateau *Info_Plateaus = refresh_plateau(Plateau);

	float x_initiale = -2.5f, y_initiale = 2.5f, taille_case = 0.25f;
	GLfloat lum[4] = {0.0f, 0.0f, 5.0f, 1.0f};
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(_pId);

	glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

	gl4duBindMatrix("modelViewMatrix");
	gl4duLoadIdentityf();

	gl4duLookAtf(0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);

	float y = y_initiale;
	for (int i = 0; i < LIGNE; i++) {
		float x = x_initiale; // Réinitialisation de x pour chaque ligne
		for (int j = 0; j < COLONNE; j++) {
			if (Plateau[j][i] != nullptr){
				glActiveTexture(GL_TEXTURE0);
				/* on bind la texture _texId[0] */
				glBindTexture(GL_TEXTURE_2D, _texId[0]);
				glUniform1i(glGetUniformLocation(_pId, "myTexture"), 0);

				//Plateau[j][i] -> affiche();
				Plateau[j][i] -> RETURN_OWNER() == false
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
			gl4duScalef(taille_case / 2, taille_case / 2, taille_case / 2);
			gl4duSendMatrices();

			gl4dgDraw(_quad);

			x += taille_case;
		}
		y -= taille_case;
	}

	glUseProgram(0); // remise du programme à 0 à la fin
}

static void quit(void) {
	gl4duClean(GL4DU_ALL);
}

