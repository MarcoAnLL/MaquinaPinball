/* Semestre 2024-1
"Proyecto Pinball Cult of the lamb"*/


#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movOffset;
bool avanza;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;

//Variables de angulo de las paletas
int anguloPaleta1 = 0;
int anguloPaleta2 = 0;
int anguloPaleta3 = 0;

//Variables lamb lloron 
GLboolean animacionLambLlorra = false;

//*/*/*******************************************************************************************/

//Encendido y apagado de luces
GLboolean luz1 = true;
GLboolean luz2 = true;
GLboolean luz3 = true;

//Animacion canica simple
int animacionSimplex = 0;
int animacionSimpley = 0;
int animacionSimplez = 0;
GLboolean animacionCanicaSimple = false;



//Movimiento de palanca
GLboolean banderaPalanca = false;
int movimientoPalanca = 0;

float palancaCrece = 0;
float contadorPalanca = 0.0f;
//

int selectorCamara = 1;


GLboolean incrementaPaleta1 = true;
GLboolean incrementaPaleta2 = true;
GLboolean incrementaPaleta3 = true;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Camera camera2;

Texture brickTexture;
Texture plainTexture;
Texture pisoTexture;
Texture escudo;

Texture rojo;
Texture plata;
Texture negro;

/// 
/// / contador de puntos
/// 
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;
Texture Numero3Texture;
Texture Numero4Texture;
Texture Numero5Texture;
Texture Numero6Texture;
Texture Numero7Texture;
Texture Numero8Texture;
Texture Numero9Texture;


///apuntadores
Texture flecha1;
Texture flecha2;
Texture flecha3;
Texture flecha4;
Texture flecha5;

int contadordecimales = 0;
int contadorUnidad = 0;

float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;


////////////Declaracion de modelos///////////////////
Model espada_M;
Model cuerpo_M;
Model pierna1_M;
Model brazo1_M;
Model brazo2_M;
Model puerta_M;
Model conjuro_M;
Model bola_M;
Model paloLanzaPelota_M;
Model baseLanzaPelota_M;
Model estatuaLamb_M;
Model resorte_M;
Model resorteS_M;
Model lagrima_M;
Model canica_M;
Model canica2_M;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights2[MAX_SPOT_LIGHTS];
SpotLight spotLights3[MAX_SPOT_LIGHTS];
SpotLight spotLights4[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void crearPiramide() {
	unsigned int indices[] = {
		0, 2, 4,
		1, 2, 4,
		1, 3, 4,
		0, 3, 4,
		0, 3, 1,
		0, 2, 1
	};

	GLfloat piramide_vertices[] = {
		-1.0f, 0.0f,  0.0f,			0.5f, 1.0f,		0.0f, 0.0f, 0.0f,//A
		0.0f, -1.0f,  0.0f,			0.5f, 1.0f,		0.0f, 0.0f, 0.0f,//B
		0.0f,  0.0f,  0.0f,			0.5f, 1.0f,		0.0f, 0.0f, 0.0f,//C
		-1.0f,  -1.0f,  0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f,//D
		-0.5f, -0.5f, 1.0f,			0.5f, 1.0f,		0.0f, 0.0f, 0.0f//E
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMesh(piramide_vertices, indices, 40, 18);
	meshList.push_back(piramide);
}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 64, 36);
	meshList.push_back(cubo);
}


void CreateObjects()
{
	

	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
//variables de animacion 
float posXavion = 30.0, posYavion = 15.0, posZavion = -2.0;
float	movAvion_x = 0.0f, movAvion_y = 0.0f, movAvion_z = 0.0;
float giroAvion = 0;

//Maximo de cuadros que se van a guardar
#define MAX_FRAMES 100

//Interpolar 90 veces ?
int i_max_steps = 90;
int i_curr_steps = 15;


typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_z;		//Variable para PosicionZ
	float movGiro;		//Variable para PosicionZ
	float movAvion_xInc;		//Variable para IncrementoX guarda valores de interpolaciones
	float movAvion_yInc;		//Variable para IncrementoY guarda valores de interpolaciones
	float movAvion_zInc;		//Variable para IncrementoY guarda valores de interpolaciones
	float giroAvion;
	float giroAvionInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
//int FrameIndex = 7;			//introducir datos
int FrameIndex = 15; 
bool play = false; 
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].movAvion_z = movAvion_z;
	KeyFrame[FrameIndex].giroAvion = giroAvion;//completar
	//no volatil, agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}

void resetElements(void) //Tecla 0 Devulve a cuadro inicial de la animacion 
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	movAvion_z = KeyFrame[0].movAvion_z;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].movAvion_zInc = (KeyFrame[playIndex + 1].movAvion_z - KeyFrame[playIndex].movAvion_z) / i_max_steps;

	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;

}
/* //////////////////////////////////////////////////////////////

	Cada nueva variable se agregan esos 6 pasos
	Declararla
	Agregarla a la estructura (con incrementala) 
	Salvarla en save
	Resetearla
	Interpolarla 
	Reproducirla 

 ///////////////////////////////////////////////////////////////// 
 
 */


void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?  Hay un frame siguiente
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame? //saber penultimo o antepenultimo 
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolación del próximo cuadro
			{
				
				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			//reproduce la animacion
			//Dibujar Animación
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			movAvion_z += KeyFrame[playIndex].movAvion_zInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////


GLboolean rotaPaleta1 = false;
GLboolean rotaPaleta2 = false;
GLboolean rotaPaleta3 = false;
float contadorPaleta1 = 0.0f;
float contadorPaleta2 = 0.0f;
float contadorPaleta3 = 0.0f;

//Variables animacion lamb
GLboolean incrementaLambBrazos = true , incrementaLambPiernas = true;
float contadorLamb = 0.0f;
int anguloLambBrazos = 0, anguloLambPiernas = 0;

/////*Contador de ciclos*/
int contadorCiclos = 0;


int main()
{
	//Variables para la puntuacion
		int cambiaUnidades = 0;
		int cambiaDecenas = 0;
		int cambiaCentenas = 0;
		int cambiaMiles = 0;
		int cambiaDiezmiles = 0;
		int cambiaCienmiles = 0;
		int timepoPuntuacion = 0;


	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	
	CreateObjects();
	CreateShaders();
	crearPiramide();
	CrearCubo();

	camera = Camera(glm::vec3(-19.0f, 46.0f, 238.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);
	camera2 = Camera(glm::vec3(350.0f, 520.0f, 480.0f), glm::vec3(0.0f, 1.0f, 0.0f), -120.0f, -30.0f, 25.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/pasto.tga");
	pisoTexture.LoadTextureA();

	NumerosTexture = Texture("Textures/Puntero/numeros-0.tga");
	NumerosTexture.LoadTextureA();

	Numero1Texture = Texture("Textures/Puntero/numeros-1.tga");
	Numero1Texture.LoadTextureA();

	Numero2Texture = Texture("Textures/Puntero/numeros-2.tga");
	Numero2Texture.LoadTextureA();

	Numero3Texture = Texture("Textures/Puntero/numeros-3.tga");
	Numero3Texture.LoadTextureA();

	Numero4Texture = Texture("Textures/Puntero/numeros-4.tga");
	Numero4Texture.LoadTextureA();

	Numero5Texture = Texture("Textures/Puntero/numeros-5.tga");
	Numero5Texture.LoadTextureA();

	Numero6Texture = Texture("Textures/Puntero/numeros-6.tga");
	Numero6Texture.LoadTextureA();

	Numero7Texture = Texture("Textures/Puntero/numeros-7.tga");
	Numero7Texture.LoadTextureA();

	Numero8Texture = Texture("Textures/Puntero/numeros-8.tga");
	Numero8Texture.LoadTextureA();

	Numero9Texture = Texture("Textures/Puntero/numeros-9.tga");
	Numero9Texture.LoadTextureA();

	escudo = Texture("Textures/escudo.tga");
	escudo.LoadTextureA();

	flecha1 = Texture("Textures/apuntadores/flecha1.tga");
	flecha1.LoadTextureA();

	flecha2 = Texture("Textures/apuntadores/flecha2.tga");
	flecha2.LoadTextureA();

	flecha3 = Texture("Textures/apuntadores/flecha3.tga");
	flecha3.LoadTextureA();

	flecha4 = Texture("Textures/apuntadores/flecha4.tga");
	flecha4.LoadTextureA();

	flecha5 = Texture("Textures/apuntadores/flecha5.tga");
	flecha5.LoadTextureA();

	
	///////////////////Ruta de los modelos ///////////////////////////////

	espada_M = Model();
	espada_M.LoadModel("Models/espaada-b.obj");

	cuerpo_M = Model();
	cuerpo_M.LoadModel("Models/cuerpo.obj");

	brazo1_M = Model();
	brazo1_M.LoadModel("Models/brazo1.obj");

	brazo2_M = Model();
	brazo2_M.LoadModel("Models/brazo2.obj");

	pierna1_M = Model();
	pierna1_M.LoadModel("Models/pierna1.obj");

	
	puerta_M = Model();
	puerta_M.LoadModel("Models/pinball.obj");

	conjuro_M = Model();
	conjuro_M.LoadModel("Models/baseConjuro.obj");

	paloLanzaPelota_M = Model();
	paloLanzaPelota_M.LoadModel("Models/paloPelota.obj");

	bola_M = Model();
	bola_M.LoadModel("Models/lanzaPelotaBola.obj");

	baseLanzaPelota_M = Model();
	baseLanzaPelota_M.LoadModel("Models/basepaloPelota.obj");


	estatuaLamb_M = Model();
	estatuaLamb_M.LoadModel("Models/lamb.obj");

	lagrima_M = Model();
	lagrima_M.LoadModel("Models/lagrima.obj");
	
	resorte_M = Model();
	resorte_M.LoadModel("Models/resorte.obj");

	resorteS_M = Model();
	resorteS_M.LoadModel("Models/superriorResorte.obj");

	canica_M = Model();
	canica_M.LoadModel("Models/cascabel.obj");

	canica2_M = Model();
	canica2_M.LoadModel("Models/canica-madera.obj");


	//std::vector<std::string> skyboxFaces;
	
	

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 6);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual

	unsigned int spotLightCount = 0;
	
	
	//////////////////////////////////////////Luces proyecto
	//Luz Verde
	spotLights[2] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 210.0f, //25
		0.0f, 1.0f, 1.000f,
		1.0f, 0.0f, 0.0f,
		55.0f);
	spotLightCount++;

	//Luz roja
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 10.0f, //25
		0.0f, 0.4f, -1.000f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz blanca
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 45.0f, 280.0f, //25
		0.0f, 3.0f, -1.000f,
		1.0f, 0.0f, 0.0f,
		110.0f);
	spotLightCount++;

	int  spotLightCount2 = 0;
	//Luz Verde
	spotLights2[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 210.0f, //25
		0.0f, 1.0f, 1.000f,
		1.0f, 0.0f, 0.0f,
		55.0f);
	spotLightCount2++;

	//Luz roja
	spotLights2[0] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 10.0f, //25
		0.0f, 0.4f, -1.000f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount2++;

	//luz blanca
	spotLights2[2] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 45.0f, 280.0f, //25
		0.0f, 3.0f, -1.000f,
		1.0f, 0.0f, 0.0f,
		110.0f);
	spotLightCount2++;

	int  spotLightCount3 = 0;
	//Luz Verde
	spotLights3[0] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 210.0f, //25
		0.0f, 1.0f, 1.000f,
		1.0f, 0.0f, 0.0f,
		55.0f);
	spotLightCount3++;

	//Luz roja
	spotLights3[2] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 10.0f, //25
		0.0f, 0.4f, -1.000f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount3++;

	//luz blanca
	spotLights3[1] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 45.0f, 280.0f, //25
		0.0f, 3.0f, -1.000f,
		1.0f, 0.0f, 0.0f,
		110.0f);
	spotLightCount3++;




	
	/*Key frames no volatiles*/
	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].movAvion_z = 0.0f;
	KeyFrame[0].giroAvion = 0;

	KeyFrame[1].movAvion_x = 0.0f;
	KeyFrame[1].movAvion_y = 56.0f;
	KeyFrame[1].movAvion_z = -250.0f;
	KeyFrame[1].giroAvion = 0;

	KeyFrame[2].movAvion_x = -4.0f;
	KeyFrame[2].movAvion_y = 62.0f;
	KeyFrame[2].movAvion_z = -275.0f;
	KeyFrame[2].giroAvion = 0;

	KeyFrame[3].movAvion_x = -10.0f;
	KeyFrame[3].movAvion_y = 68.0f;
	KeyFrame[3].movAvion_z = -300.0f;
	KeyFrame[3].giroAvion = 0;

	KeyFrame[4].movAvion_x = -22.0f;
	KeyFrame[4].movAvion_y = 73.0f;
	KeyFrame[4].movAvion_z = -325.0f;
	KeyFrame[4].giroAvion = 0;

	KeyFrame[5].movAvion_x = -43.0f;
	KeyFrame[5].movAvion_y = 79.0f;
	KeyFrame[5].movAvion_z = -350.0f;
	KeyFrame[5].giroAvion = 0;

	KeyFrame[6].movAvion_x = -96.0f;
	KeyFrame[6].movAvion_y = 84.0f;
	KeyFrame[6].movAvion_z = -375.0f;
	KeyFrame[6].giroAvion = 0;

	KeyFrame[7].movAvion_x = -157.0f;
	KeyFrame[7].movAvion_y = 70.0f;
	KeyFrame[7].movAvion_z = -314.0f;
	KeyFrame[7].giroAvion = 0;

	KeyFrame[8].movAvion_x = -141.0f;
	KeyFrame[8].movAvion_y = 67.0f;
	KeyFrame[8].movAvion_z = -297.0f;
	KeyFrame[8].giroAvion = 0;

	KeyFrame[9].movAvion_x = -164.0f;
	KeyFrame[9].movAvion_y = 54.0f;
	KeyFrame[9].movAvion_z = -239.0f;
	KeyFrame[9].giroAvion = 0;

	KeyFrame[10].movAvion_x = -106.0f;
	KeyFrame[10].movAvion_y = 30.0f;
	KeyFrame[10].movAvion_z = -131.0f;
	KeyFrame[10].giroAvion = 0;

	KeyFrame[11].movAvion_x = -94.0f;
	KeyFrame[11].movAvion_y = 29.0f;
	KeyFrame[11].movAvion_z = -131.0f;
	KeyFrame[11].giroAvion = 0;

	KeyFrame[12].movAvion_x = -89.0f;
	KeyFrame[12].movAvion_y = 28.0f;
	KeyFrame[12].movAvion_z = -126.0f;
	KeyFrame[12].giroAvion = 0;

	KeyFrame[13].movAvion_x = -68.0f;
	KeyFrame[13].movAvion_y = 16.0f;
	KeyFrame[13].movAvion_z = -71.0f;
	KeyFrame[13].giroAvion = 0;

	KeyFrame[14].movAvion_x = -122.0f;
	KeyFrame[14].movAvion_y = -6.0f;
	KeyFrame[14].movAvion_z = 35.0f;
	KeyFrame[14].giroAvion = 0;
	
	//animacion de lamb lloron
	GLboolean seleccionaLagrima = true;
	float contadorLambLlora = 0.0;
	float incrementoLagrima1 = 0.0;
	float incrementoLagrima2 = 0.0;

	float incrementoLagrima3 = 0.0f;
	GLboolean seleccionaLagrima2 = true;
	float incrementoLagrima4 = 0.0f;
	float contadorLambLlora2 = 0.0f;

	//Variables dia y noche
	//////////////////////////////////////
	int ciclos = 0;
	GLboolean dia = true;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	
		printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
		printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");

		GLboolean incrementaPalanca = true;
		
	//Animacion canica sencilla
	GLboolean animacionCanicaSimple2 = false;
	GLboolean animacionCanicaSimple3 = false;
	GLboolean animacionCanicaSimple4 = false;
	/*
	std::vector<std::string> skyboxFaces;
		skyboxFaces.clear();
		skyboxFaces.push_back("Textures/Skybox/paisage.tga");
		skyboxFaces.push_back("Textures/Skybox/paisage.tga");
		skyboxFaces.push_back("Textures/Skybox/paisage.tga");
		skyboxFaces.push_back("Textures/Skybox/paisage.tga");
		skyboxFaces.push_back("Textures/Skybox/paisaje.tga");
		skyboxFaces.push_back("Textures/Skybox/paisage.tga");
		skybox = Skybox(skyboxFaces);
	*/

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		
		if (animacionCanicaSimple) {
			
			if (animacionSimplex < 53) animacionSimplex++;
			else {
				animacionCanicaSimple = false;
				animacionCanicaSimple2 = true;
			}
			if (animacionSimpley < 6 && (animacionSimplex % 5) == 0) animacionSimpley++;
			if (animacionSimplez > -28) animacionSimplez--;
		}
		if (animacionCanicaSimple2) {
			if (animacionSimplex < 102) animacionSimplex++;
			if (animacionSimpley > -15 && (animacionSimplez % 4) == 0) animacionSimpley--;
			if (animacionSimplez < 74) animacionSimplez++;
			else {
				animacionCanicaSimple2 = false;
				animacionCanicaSimple3 = true;
			}
		}
		if (animacionCanicaSimple3) {
			
			if (animacionSimpley > -77 &&(animacionSimplez % 4) == 0) animacionSimpley--;
			if (animacionSimplez < 356) animacionSimplez++;
			else {
				animacionCanicaSimple3 = false;
				animacionCanicaSimple4 = true;
			}
		}

		if (animacionCanicaSimple4) {
			if (animacionSimplex > 63 ) animacionSimplex -=2;
			if (animacionSimpley > -90 && (animacionSimplez % 2) == 0) animacionSimpley--;
			if (animacionSimplez < 395) animacionSimplez++;
			else {
				animacionCanicaSimple4 = false;
			}
		}


		if (ciclos >= 0) dia = true;
		if (ciclos >= 100) dia = false;
		if (ciclos >= 200) ciclos = 0;
		else ciclos++;
		//SkyBox
		
		std::vector<std::string> skyboxFaces;
			if (dia) {
				skyboxFaces.clear();
				skyboxFaces.push_back("Textures/Skybox/paisage.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage.tga");
				skyboxFaces.push_back("Textures/Skybox/paisaje.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage.tga");
				//skybox = Skybox(skyboxFaces);
			}
			else {
				//std::vector<std::string> skyboxFaces;
				skyboxFaces.clear();
				skyboxFaces.push_back("Textures/Skybox/paisage-noche.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage-noche.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage-noche.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage-noche.tga");
				skyboxFaces.push_back("Textures/Skybox/paisaje-noche.tga");
				skyboxFaces.push_back("Textures/Skybox/paisage-noche.tga");
				//skybox = Skybox(skyboxFaces);

			}
			skybox = Skybox(skyboxFaces);
		

		if (contadorCiclos >= 63.0) {
			if (reproduciranimacion < 1)
			{
				if (play == false && (FrameIndex > 1))
				{
					resetElements();
					//First Interpolation				
					interpolation();
					play = true;
					playIndex = 0;
					i_curr_steps = 0;
					reproduciranimacion++;
					printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
					habilitaranimacion = 0;

				}
				else
				{
					play = false;

				}
			}
		}
		

		if (animacionLambLlorra) {
			//Hay que indicar el maximo que debe de alcanzar
			if (incrementoLagrima1 >= 26.0) seleccionaLagrima = false;
			if (incrementoLagrima2 >= 26.0) seleccionaLagrima = true;
			if (contadorLambLlora >= 0.5) {
				if (seleccionaLagrima) incrementoLagrima1 += 0.1;
				else incrementoLagrima2 += 0.1;
				if (incrementoLagrima2 >= 26.0 && incrementoLagrima1 >= 26.0) {
					//animacionLambLlorra = false;
					incrementoLagrima1 = 0.0f;
					incrementoLagrima2 = 0.0f;
				}
				contadorLambLlora = 0.0;
			}
			contadorLambLlora += deltaTime * 1.0;

			if (incrementoLagrima3 >= 26.0) seleccionaLagrima2 = false;
			if (incrementoLagrima4 >= 26.0) seleccionaLagrima2 = true;
			if (contadorLambLlora2 >= 0.5) {
				if (seleccionaLagrima2) incrementoLagrima3 += 0.1;
				else incrementoLagrima4 += 0.1;
				if (incrementoLagrima3 >= 26.0 && incrementoLagrima4 >= 26.0) {
					animacionLambLlorra = false;
					incrementoLagrima3 = 0.0f;
					incrementoLagrima4 = 0.0f;
				}
				contadorLambLlora2 = 0.0;
			}
			contadorLambLlora2 += deltaTime * 1.0;
			
		}

		if (contadorLamb < 5) {
			//Brazos
			if (anguloLambBrazos == -45) incrementaLambBrazos = true;
			if (anguloLambBrazos == 45) incrementaLambBrazos = false;
			if (incrementaLambBrazos) anguloLambBrazos += 3;
			else anguloLambBrazos -= 3;
			//Piernas
			if (anguloLambPiernas == 0) incrementaLambPiernas = true;
			if (anguloLambPiernas == 44) incrementaLambPiernas = false;
			if (incrementaLambPiernas) anguloLambPiernas += 2;
			else anguloLambPiernas -= 2;
			contadorLamb = 0;
		}

		contadorLamb +=  1;

		if (banderaPalanca == true) {
			//Verifica si al angulo limite de la paleta, el angulo limite es de  45 Grados
			if (palancaCrece <= 0) incrementaPalanca = true;
			if (palancaCrece >= 3) incrementaPalanca = false;
			//Cada que el contador tenga un valor mayor a 1 incrementa o decrementa el angulo de la paleta 3 grados
			if (contadorPalanca > 1) {
				if (incrementaPalanca) palancaCrece += 0.1;
				else palancaCrece -= 0.1;
				//Al volver a 0 le asigna a la variable rotaPaleta un valor de false para indicar que a terminado la anumacion 
				if (palancaCrece <= 0) {
					banderaPalanca = false;
				}
				//Reinicia el contador para volver contar el tiempo
				contadorPalanca = 0;
			}
			//Avanza el contador s
			contadorPalanca += deltaTime * 1;
			contadorCiclos += deltaTime * 1.0;

			printf("numero de ciclos %d\n", contadorCiclos); //Checarsadsakldjasllllllllllllllllllllllllllllllllllllllllllllllll
		}
		else{
			contadorCiclos = 0;
		}

		//Animacion de paleta 1
		if (rotaPaleta1 == true) {
			//Verifica si al angulo limite de la paleta, el angulo limite es de  45 Grados
			if (anguloPaleta1 == 0) incrementaPaleta1 = true;
			if (anguloPaleta1 == 45) incrementaPaleta1 = false;
			//Cada que el contador tenga un valor mayor a 1 incrementa o decrementa el angulo de la paleta 3 grados
			if (contadorPaleta1 > 1) {
				if (incrementaPaleta1) anguloPaleta1 += 3;
				else anguloPaleta1 -= 3;
				//Al volver a 0 le asigna a la variable rotaPaleta un valor de false para indicar que a terminado la anumacion 
				if (anguloPaleta1 == 0) {
					rotaPaleta1 = false;
				}
				//Reinicia el contador para volver contar el tiempo
				contadorPaleta1 = 0;
			}
			//Avanza el contador s
			contadorPaleta1 += deltaTime * 1;
		}

		//Animacion de paleta 2s
		if (rotaPaleta2 == true) {
			//Verifica si al angulo limite de la paleta, el angulo limite es de  45 Grados
			if (anguloPaleta2 == 0) incrementaPaleta2 = true;
			if (anguloPaleta2 == 45) incrementaPaleta2 = false;
			//Cada que el contador tenga un valor mayor a 1 incrementa o decrementa el angulo de la paleta 3 grados
			if (contadorPaleta2 > 1) {
				if (incrementaPaleta2) anguloPaleta2 += 3;
				else anguloPaleta2 -= 3;
				//Al volver a 0 le asigna a la variable rotaPaleta un valor de false para indicar que a terminado la anumacion 
				if (anguloPaleta2 == 0) {
					rotaPaleta2 = false;
				}
				//Reinicia el contador para volver contar el tiempo
				contadorPaleta2 = 0;
			}
			//Avanza el contador s
			contadorPaleta2 += deltaTime * 1;
		}

		//Animacion de paleta 2s
		if (rotaPaleta3 == true) {
			//Verifica si al angulo limite de la paleta, el angulo limite es de  45 Grados
			if (anguloPaleta3 == 0) incrementaPaleta3 = true;
			if (anguloPaleta3 == 45) incrementaPaleta3 = false;
			//Cada que el contador tenga un valor mayor a 1 incrementa o decrementa el angulo de la paleta 3 grados
			if (contadorPaleta3 > 1) {
				if (incrementaPaleta3) anguloPaleta3 += 3;
				else anguloPaleta3 -= 3;
				//Al volver a 0 le asigna a la variable rotaPaleta un valor de false para indicar que a terminado la anumacion 
				if (anguloPaleta3 == 0) {
					rotaPaleta3 = false;
				}
				//Reinicia el contador para volver contar el tiempo
				contadorPaleta3 = 0;
			}
			//Avanza el contador s
			contadorPaleta3 += deltaTime * 1;
		}

		//Funcion que en el tiempo incremente una variable y decremente otra y 


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(selectorCamara==1){
			skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		if(selectorCamara==2){
			skybox.DrawSkybox(camera2.calculateViewMatrix(), projection);
		}
		
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		if (selectorCamara == 2) {
			camera2.keyControl(mainWindow.getsKeys(), deltaTime * 0.5f);
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera2.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera2.getCameraPosition().x, camera2.getCameraPosition().y, camera2.getCameraPosition().z);
		}
		if (selectorCamara == 1) {
			camera.keyControl(mainWindow.getsKeys(), deltaTime * 5);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			
		}

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		


		//Manejo de luces

		shaderList[0].SetSpotLights(spotLights, spotLightCount );

		if (!luz1) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}

		if (!luz2) {
			shaderList[0].SetSpotLights(spotLights2, spotLightCount2 - 1);
		}

		if (!luz3) {
			shaderList[0].SetSpotLights(spotLights3, spotLightCount3 - 1);
		}

		if (!luz1 && !luz3) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 2);
		}

		if (!luz1 && !luz2) {
			shaderList[0].SetSpotLights(spotLights2, spotLightCount2 - 2);
		}

		if (!luz2 && !luz3) {
			shaderList[0].SetSpotLights(spotLights3, spotLightCount3 - 2);
		}

		if (!luz3 && !luz2 && !luz1) {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 3);
		}


		////Manejo de luces
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		//shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -420.0f, 0.0f));
		model = glm::scale(model, glm::vec3(40.0f, 1.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f , 70.0f , 69.0f ));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puerta_M.RenderModel();

		

		/*Modelo espada 1*/
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-55.0f + movAvion_x, 23.0f + movAvion_y, 246.0 + movAvion_z));
		model = glm::translate(model, glm::vec3(-60.0f , 25.0f , 243.0 ));
		model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
		/*
		model = glm::rotate(model, 0.0f + toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model,334.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 274.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//*/
		///*
		model = glm::rotate(model, (-6.0f ) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, ( 334.0f + anguloPaleta1) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, ( 274.0f )* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//*/

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		espada_M.RenderModel();


		/*Modelo espada 2s*/
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(28.0f + movAvion_x, 25.0f + movAvion_y, 243.0 + movAvion_z));
		model = glm::translate(model, glm::vec3(28.0f, 25.0f, 245.0f));
		model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
		/*
		model = glm::rotate(model, 0.0f + toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model,202.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//*/
		///*
		model = glm::rotate(model, (4.0f ) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (202.0f- anguloPaleta2) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (90.0f ) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//*/

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		espada_M.RenderModel();

		/*Modelo espada 3*/
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-106.0f + movAvion_x, 60.0f + movAvion_y, 78.0 + movAvion_z));
		model = glm::translate(model, glm::vec3(-106.0f, 60.0f, 78.0));
		model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
		/*
		model = glm::rotate(model, 0.0f + toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model,(334.0f + anguloPaleta3) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 274.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//*/
		///*
		model = glm::rotate(model, (-6.0f) * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (330.0f + anguloPaleta3) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (274.0f) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//*/

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		espada_M.RenderModel();



		/*Modelo jerarquico de la LAMB*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-51.0f , 27.0f , -94.0 ));
		//model = glm::translate(model, glm::vec3(-51.0f , 93.0f , -94.0 ));
		model = glm::rotate(model, (290.0f ) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cuerpo_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(28.0f, 0.0f, 68.0f));
		//model = glm::rotate(model, * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, anguloLambBrazos * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo1_M.RenderModel();


		model = modelaux;
		model = glm::translate(model, glm::vec3(39.0f , 0.0f , 68.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, anguloLambBrazos * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo2_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(32.0f , -2.0f , 69.0f ));
		model = glm::rotate(model, -anguloLambPiernas * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna1_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(37.0f , -2.0f , 69.0f ));
		model = glm::rotate(model, anguloLambPiernas * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna1_M.RenderModel();

		//// termina instancia 1 de lamb jerarquico 


		//Inicia instancia 2 de lamb jerarquico

		/*Modelo jerarquico de la LAMB*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(19.0, -18.0f, 114.0f));
		//model = glm::translate(model, glm::vec3(-51.0f , 93.0f , -94.0 ));
		model = glm::rotate(model, (290.0f) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cuerpo_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(28.0f, 0.0f, 68.0f));
		//model = glm::rotate(model, * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, anguloLambBrazos * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo1_M.RenderModel();


		model = modelaux;
		model = glm::translate(model, glm::vec3(39.0f, 0.0f, 68.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, anguloLambBrazos * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo2_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(32.0f, -2.0f, 69.0f));
		model = glm::rotate(model, -anguloLambPiernas * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna1_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(37.0f, -2.0f, 69.0f));
		model = glm::rotate(model, anguloLambPiernas * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna1_M.RenderModel();

		//// termina instancia 2 de lamb jerarquico 



		//Inicia instancia 3 de lamb jerarquico

		/*Modelo jerarquico de la LAMB*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(17.0f, 8.0f, 27.0));
		//model = glm::translate(model, glm::vec3(-51.0f , 93.0f , -94.0 ));
		model = glm::rotate(model, (290.0f) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cuerpo_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(28.0f, 0.0f, 68.0f));
		//model = glm::rotate(model, * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, anguloLambBrazos * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo1_M.RenderModel();


		model = modelaux;
		model = glm::translate(model, glm::vec3(39.0f, 0.0f, 68.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, anguloLambBrazos * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo2_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(32.0f, -2.0f, 64.0f));
		model = glm::rotate(model, -anguloLambPiernas * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna1_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(37.0f, -2.0f, 64.0f));
		model = glm::rotate(model, anguloLambPiernas * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna1_M.RenderModel();

		//// termina instancia 3 de lamb jerarquico 



		
		//Modelo jerarquico de la activador de pinball
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f , 25.0f, 356.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		//Cilindro Lanzapelota
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f + palancaCrece, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		paloLanzaPelota_M.RenderModel();

		//Base Lanzapelota
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseLanzaPelota_M.RenderModel();

		//Bola lanzapelota
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -3.9f + (palancaCrece *9), 0.0));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		bola_M.RenderModel();

		
		model = glm::mat4(1.0); 
		//model = glm::translate(model, glm::vec3(-80.0f + movAvion_x, 75.0f + movAvion_y, 0.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-80.0f , 75.0f , 0.0f ));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		estatuaLamb_M.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(106.7f + movAvion_x, 23.0f + movAvion_y, 223.0f + movAvion_z));
		//model = glm::translate(model, glm::vec3(105.7f, 23.0f, 220.0f));
		model = glm::rotate(model, giroAvion * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.47f , 0.47f , 0.47f ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica_M.RenderModel();
		
		//Resorte
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(103.0f + movAvion_x, 18.0f + movAvion_y, 272.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(103.0f, 18.0f, 272.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, gl91229129m::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (283) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(4.3f , 4.3 - palancaCrece, 4.3));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resorte_M.RenderModel();



		//Superior Resorte
		model = modelaux;
		//model = glm::translate(model, glm::vec3(103.0f + movAvion_x, 18.0f + movAvion_y, 272.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(0.0f , -1.0f - ( palancaCrece * 5), 0.0f ));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.3f, 4.3 , 4.3));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resorteS_M.RenderModel();
		


		//Lagrima derecha 1
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-74.5f , 90.0f - incrementoLagrima1, 6.0f ));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();

		//Lagrima derecha 2
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-76.5f , 90.0f - incrementoLagrima2, 6.0f  ));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f,.2f,.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();


		//Lagrima ojo izquierdo 1
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-82.5f, 90.0f - incrementoLagrima3, 6.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();

		//Lagrima ojo izquierdo 2
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-84.5f, 90.0f - incrementoLagrima4, 6.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();

		color = glm::vec3(0.0f, 1.0f, 0.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.5f, 83.0f , -3.0f ));
		model = glm::rotate(model, 270  * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(14.0f, 14.0f, 10.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		meshList[7]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.5f, 89.0f, -3.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(14.0f, 14.0f, 10.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		meshList[7]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.5f, 95.0f, -3.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(14.0f, 14.0f, 10.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		meshList[7]->RenderMesh();


		color = glm::vec3(0.66f, 0.33f, 0.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(48.5f , 78.0f , 4.0f ));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		meshList[8]->RenderMesh();


		///////////////////////////escudos

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-162.0f+movAvion_x, 19.0f + movAvion_y, 42.0f + movAvion_z));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(150.0f, 150.0f, 150.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		escudo.UseTexture();
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();

		///////////////////////////////////////////// Apuntadores

		// textura con movimiento
			//Importantes porque la variable uniform no podemos modificarla directamente
			toffsetflechau += 0.001;
		toffsetflechav += 0.0;
		//para que no se desborde la variable
		if (toffsetflechau > 1.0)
			toffsetflechau = 0.0;
		//if (toffsetv > 1.0)
		//	toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetflechau, toffsetflechav);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(103.0f, 46.0f , 133.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f ));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 13 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f+movAvion_x, 0.0f + movAvion_y, 0.0f + movAvion_z));
		model = glm::scale(model, glm::vec3(40.0f, 4550.0f, 13.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		flecha1.UseTexture();
		
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-70.0f, 43.0f, 186.0));
		model = glm::rotate(model, 95 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); 
		model = glm::rotate(model, 59 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); 
		model = glm::scale(model, glm::vec3(40.0f, 570.0f, 13.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		flecha2.UseTexture();
		
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-90.0f, 71.0f, 42.0));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 80 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -10 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(40.0f, 50.0f, 10.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		flecha3.UseTexture();
		
		meshList[4]->RenderMesh();

		

		//Segunda canica
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-77.5f + animacionSimplex, 99.0f + animacionSimpley, -118.0f+ animacionSimplez));
		model = glm::scale(model, glm::vec3(.47f, .47f, .47f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica2_M.RenderModel();







		//////////////////////////////////////////////////////////
		// //////////////////////////////////////////////////////////
		// //////////////////////////////////////////////////////////
		// //////////////////////////////////////////////////////////
		// //////////////////////////////////////////////////////////
		//plano con todos los números



		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(130.0f, 230.0f, -235.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//Antesde cade switch va la declaracion del objeto ya nada mas lo unico que cambia es la textura
		switch (cambiaUnidades)
		{
		case 0:
			NumerosTexture.UseTexture();
			break;
		case 1:
			Numero1Texture.UseTexture();
			break;

		case 2:
			Numero2Texture.UseTexture();
			break;

		case 3:
			Numero3Texture.UseTexture();
			break;

		case 4:
			Numero4Texture.UseTexture();
			break;

		case 5:
			Numero5Texture.UseTexture();
			break;

		case 6:
			Numero6Texture.UseTexture();
			break;

		case 7:
			Numero7Texture.UseTexture();
			break;

		case 8:
			Numero8Texture.UseTexture();
			break;

		case 9:
			Numero9Texture.UseTexture();
			break;
		default:
			break;
		}
		meshList[5]->RenderMesh();
		if (timepoPuntuacion % 100 == 0 && timepoPuntuacion != 0) {
			cambiaUnidades++;
		}
		if (cambiaUnidades == 10) {
			cambiaUnidades = 0;
			cambiaDecenas += 1;
		}

		timepoPuntuacion++;

		//Aqui va el codigo para crear la primitiva, lo acomodas jeje
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(70.0f, 230.0f, -235.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		switch (cambiaDecenas)
		{
		case 0:
			NumerosTexture.UseTexture();
			break;
		case 1:
			Numero1Texture.UseTexture();
			break;

		case 2:
			Numero2Texture.UseTexture();
			break;

		case 3:
			Numero3Texture.UseTexture();
			break;

		case 4:
			Numero4Texture.UseTexture();
			break;

		case 5:
			Numero5Texture.UseTexture();
			break;

		case 6:
			Numero6Texture.UseTexture();
			break;

		case 7:
			Numero7Texture.UseTexture();
			break;

		case 8:
			Numero8Texture.UseTexture();
			break;

		case 9:
			Numero9Texture.UseTexture();
			break;
		default:
			break;
		}
		meshList[5]->RenderMesh();

		if (cambiaDecenas == 10) {
			cambiaDecenas = 0;
			cambiaCentenas += 1;
		}

		//Aqui va el codigo para crear la primitiva, lo acomodas jeje
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, 230.0f, -235.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));


		switch (cambiaCentenas)
		{
		case 0:
			NumerosTexture.UseTexture();
			break;
		case 1:
			Numero1Texture.UseTexture();
			break;

		case 2:
			Numero2Texture.UseTexture();
			break;

		case 3:
			Numero3Texture.UseTexture();
			break;

		case 4:
			Numero4Texture.UseTexture();
			break;

		case 5:
			Numero5Texture.UseTexture();
			break;

		case 6:
			Numero6Texture.UseTexture();
			break;

		case 7:
			Numero7Texture.UseTexture();
			break;

		case 8:
			Numero8Texture.UseTexture();
			break;

		case 9:
			Numero9Texture.UseTexture();
			break;
		default:
			break;
		}
		meshList[5]->RenderMesh();

		if (cambiaCentenas == 10) {
			cambiaCentenas = 0;
			cambiaMiles += 1;
		}
		//Aqui va el codigo para crear la primitiva, lo acomodas jeje
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 230.0f, -235.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		switch (cambiaMiles)
		{
		case 0:
			NumerosTexture.UseTexture();
			break;
		case 1:
			Numero1Texture.UseTexture();
			break;

		case 2:
			Numero2Texture.UseTexture();
			break;

		case 3:
			Numero3Texture.UseTexture();
			break;

		case 4:
			Numero4Texture.UseTexture();
			break;

		case 5:
			Numero5Texture.UseTexture();
			break;

		case 6:
			Numero6Texture.UseTexture();
			break;

		case 7:
			Numero7Texture.UseTexture();
			break;

		case 8:
			Numero8Texture.UseTexture();
			break;

		case 9:
			Numero9Texture.UseTexture();
			break;
		default:
			break;
		}
		meshList[5]->RenderMesh();
		if (cambiaMiles == 10) {
			cambiaMiles = 0;
			cambiaDiezmiles += 1;
		}

		//Aqui va el codigo para crear la primitiva, lo acomodas jeje
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-85.0f, 230.0f, -235.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		switch (cambiaDiezmiles)
		{
		case 0:
			NumerosTexture.UseTexture();
			break;
		case 1:
			Numero1Texture.UseTexture();
			break;

		case 2:
			Numero2Texture.UseTexture();
			break;

		case 3:
			Numero3Texture.UseTexture();
			break;

		case 4:
			Numero4Texture.UseTexture();
			break;

		case 5:
			Numero5Texture.UseTexture();
			break;

		case 6:
			Numero6Texture.UseTexture();
			break;

		case 7:
			Numero7Texture.UseTexture();
			break;

		case 8:
			Numero8Texture.UseTexture();
			break;

		case 9:
			Numero9Texture.UseTexture();
			break;
		default:
			break;
		}
		meshList[5]->RenderMesh();
		if (cambiaDiezmiles == 10) {
			cambiaDiezmiles = 0;
			cambiaCienmiles += 1;
		}

		//Aqui va el codigo para crear la primitiva, lo acomodas jeje
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-140.0f, 230.0f, -235.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		switch (cambiaCienmiles)
		{
		case 0:
			NumerosTexture.UseTexture();
			break;
		case 1:
			Numero1Texture.UseTexture();
			break;

		case 2:
			Numero2Texture.UseTexture();
			break;

		case 3:
			Numero3Texture.UseTexture();
			break;

		case 4:
			Numero4Texture.UseTexture();
			break;

		case 5:
			Numero5Texture.UseTexture();
			break;

		case 6:
			Numero6Texture.UseTexture();
			break;

		case 7:
			Numero7Texture.UseTexture();
			break;

		case 8:
			Numero8Texture.UseTexture();
			break;

		case 9:
			Numero9Texture.UseTexture();
			break;
		default:
			break;
		}
		meshList[5]->RenderMesh();
		if (cambiaCienmiles == 10) {
			cambiaCienmiles = 0;
		}

		//Modelo de adepto
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f + camera.getCameraPosition().x , -20.0f + camera.getCameraPosition().y, -80.0f + camera.getCameraPosition().z));
		model = glm::rotate(model, -camera.getCameraYaw() + (toRadians * 90), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		bola_M.RenderModel();
		
		glDisable(GL_BLEND);
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}


void inputKeyframes(bool* keys)
{
		
	//Entra tantas veces como ciclos de reloj se su procesador

	if (keys[GLFW_KEY_0])
	{
		//printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
		reproduciranimacion = 0;
		
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("movAvion_y es: %f\n", movAvion_z);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 9 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_2])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x -= 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 9 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_3])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y += 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 9 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_4])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y -= 1.0f;
			printf("\n movAvion_y es: %f\n", movAvion_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 9 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_5])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_z += 1.0f;
			printf("\n movAvion_z es: %f\n", movAvion_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 9 para poder habilitar la variable\n");
		}

	}
	
	if (keys[GLFW_KEY_6])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_z -= 1.0f;
			printf("\n movAvion_z es: %f\n", movAvion_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 9 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_7])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			giroAvion += 15.0f;
			printf("\n Giro de objeto: %f\n", giroAvion);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 9 para poder habilitar la variable\n");
		}

	}

	if (keys[GLFW_KEY_9])
	{
		if (ciclo2 < 1 && ciclo > 0)
		{
			ciclo = 0;
			printf("\n Ya puedes modificar la posicion del objeto, presiona culaquier tecla del 1 al 6\n");
		}
	}

	if (keys[GLFW_KEY_Z])
	{
		rotaPaleta1 = true;

	}
	if (keys[GLFW_KEY_X])
	{
		rotaPaleta2 = true;

	}

	if (keys[GLFW_KEY_C])
	{
		//contadorCiclos = 0;
		banderaPalanca = true;
		reproduciranimacion = 0;
	}
	if (keys[GLFW_KEY_V])
	{
		rotaPaleta3 = true;
		//animacionLambLlorra = true

	}
	
	
	if (keys[GLFW_KEY_B])
	{
		animacionLambLlorra = true;

	}

	if (keys[GLFW_KEY_F])
	{
		luz1 = false;
		
	}

	if (keys[GLFW_KEY_G])
	{		
		luz1 = true;
	}

	if (keys[GLFW_KEY_H])
	{
		luz2 = false;
	}

	if (keys[GLFW_KEY_J])
	{
		luz2 = true;
	}

	if (keys[GLFW_KEY_K])
	{
		luz3 = false;
	}

	if (keys[GLFW_KEY_L])
	{
		luz3 = true;
	}

	if (keys[GLFW_KEY_Q])
{
	animacionCanicaSimple = true;
	animacionSimplex = 0;
	animacionSimpley = 0;
	animacionSimplez = 0;
}

	if (keys[GLFW_KEY_E])
	{
		selectorCamara = 1;
	}

	if (keys[GLFW_KEY_R])
	{
		selectorCamara = 2;
	}
}
