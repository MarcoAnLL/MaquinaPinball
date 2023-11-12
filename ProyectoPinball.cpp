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

//Movimiento de palanca
GLboolean banderaPalanca = false;
int movimientoPalanca = 0;

float palancaCrece = 0;
float contadorPalanca = 0.0f;
//


GLboolean incrementaPaleta1 = true;
GLboolean incrementaPaleta2 = true;
GLboolean incrementaPaleta3 = true;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture plainTexture;
Texture pisoTexture;


Texture rojo;
Texture plata;
Texture negro;

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
float	movAvion_x = 0.0f, movAvion_y = 0.0f, movAvion_z = 0.0, movGiro;
float giroAvion = 0;

//Maximo de cuadros que se van a guardar
#define MAX_FRAMES 100

//Interpolar 90 veces ?
int i_max_steps = 90;
int i_curr_steps = 4;


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
int FrameIndex = 0;
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
GLboolean linterna = false;

GLboolean rotaPaleta2 = false;
GLboolean rotaPaleta3 = false;
float contadorPaleta1 = 0.0f;
float contadorPaleta2 = 0.0f;
float contadorPaleta3 = 0.0f;
float arreglo_luces = 0.0f;

//Variables animacion lamb
GLboolean incrementaLambBrazos = true , incrementaLambPiernas = true;
float contadorLamb = 0.0f;
int anguloLambBrazos = 0, anguloLambPiernas = 0;


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/pasto.tga");
	pisoTexture.LoadTextureA();
	
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
	resorteS_M.LoadModel("Models/superiorResorte.obj");

	canica_M = Model();
	canica_M.LoadModel("Models/cascabel.obj");

	std::vector<std::string> skyboxFaces;
	
	skyboxFaces.push_back("Textures/Skybox/paisage.tga");
	skyboxFaces.push_back("Textures/Skybox/paisage.tga");
	skyboxFaces.push_back("Textures/Skybox/paisage.tga");
	skyboxFaces.push_back("Textures/Skybox/techo.tga");
	skyboxFaces.push_back("Textures/Skybox/paisaje.tga");
	skyboxFaces.push_back("Textures/Skybox/paisage.tga");

	skybox = Skybox(skyboxFaces);

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
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	/*
	//luz AMBIENTE
	pointLights[1] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-60.0f, 25.0f, 243.0f,
		0.8f, 0.01f, 0.0000000001f);
	pointLightCount++;
	*/
	
	//espada
	
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 210.0f, //25
		0.0f, 1.0f, 1.000f,
		1.0f, 0.0f, 0.0f,
		55.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		-20.0f, 25.0f, 10.0f, //25
		0.0f, 0.4f, -1.000f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 45.0f, 280.0f, //25
		0.0f, 3.0f, -1.000f,
		1.0f, 0.0f, 0.0f,
		110.0f);
	spotLightCount++;

	spotLights2[0] = spotLights[0];
	spotLights2[1] = spotLights[2];
	spotLights2[2] = spotLights[3];
	spotLights2[3] = spotLights[1];

	spotLights3[0] = spotLights[0];
	spotLights3[1] = spotLights[1];
	spotLights3[2] = spotLights[3];
	spotLights3[3] = spotLights[2];


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	
		printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
		printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X");

		GLboolean incrementaPalanca = true;
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		
		if (contadorLamb < 1) {
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

		contadorLamb = deltaTime * 1;

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
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
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
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//apagar y prender luces
		GLboolean banderaun = false;
		GLboolean banderaDS = false;
		GLboolean banderaTRE = false;

		if (mainWindow.getarregloluces() == 1)
		{
			if (inputKeyframes.linterna() == true)
			{

				shaderList[0].SetSpotLights(spotLights, spotLightCount);
			}
			else
			{

				shaderList[0].SetSpotLights(spotLights, spotLightCount-1);
			}

		}
		else
		{
			if (inputKeyframes.linterna() == true)
			{

				shaderList[0].SetSpotLights(spotLights2, spotLightCount);
			}
			else
			{
				shaderList[0].SetSpotLights(spotLights2, spotLightCount-1);
			}
		}

/*
		if (mainWindow.getlinterna() == true)
		{

			shaderList[0].SetSpotLights(spotLights3, spotLightCount);
		}
		else
		{
			shaderList[0].SetSpotLights(spotLights3, spotLightCount - 1);
		}

		*/


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
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



		
		//Modelo jerarquico de la activador de pinball
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f , 25.0f, 356.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

		modelaux = model;
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f + palancaCrece, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		paloLanzaPelota_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseLanzaPelota_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -3.9f + (palancaCrece *9), 0.0));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
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
		model = glm::translate(model, glm::vec3(103.7f , 18.0f , 225.0f ));
		//model = glm::translate(model, glm::vec3(-80.0f, 75.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica_M.RenderModel();

		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(103.0f + movAvion_x, 18.0f + movAvion_y, 272.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(103.0f, 18.0f, 272.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (283) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.3f , 4.3 - palancaCrece, 4.3));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resorte_M.RenderModel();


		//Lagrima derecha 1
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-74.5f , 90.0f + movAvion_y, 6.0f ));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();

		//Lagrima derecha 2
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-76.5f , 90.0f  , 6.0f  ));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f,.2f,.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();


		//Lagrima ojo izquierdo 1
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-82.5f, 90.0f, 6.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();

		//Lagrima ojo izquierdo 2
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-82.5.0f + movAvion_x, 82.0f + movAvion_y, 6.0f + movAvion_z));
		model = glm::translate(model, glm::vec3(-84.5f, 90.0f, 6.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (283 + movAvion_x) * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.2f, .2f, .2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lagrima_M.RenderModel();

		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}


void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
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
	//Entra tantas veces como ciclos de reloj se su procesador

	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;
			
		}
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
		banderaPalanca = true;

	}
	if (keys[GLFW_KEY_V])
	{
		rotaPaleta3 = true;
		//animacionLambLlorra = true

	}

	if (keys[GLFW_KEY_O])
	{
		linterna = false;
	}

	if (keys[GLFW_KEY_P])
	{
		linterna = true;
	}

	if (keys[GLFW_KEY_N])
	{

		arreglo_luces = 1.0f;
	}
	if (keys[GLFW_KEY_M])
	{
		arreglo_luces = 2.0f;
	}

}
