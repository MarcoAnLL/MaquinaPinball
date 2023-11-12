/*
Semestre 2024-1
Pr�ctica 7: Iluminaci�n 1 
*/
//para cargar imagen
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
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture;

Model Kitt_M;
Model Llanta_M;
Model Llanta2_M;
Model Llanta3_M;
Model Llanta4_M;

Model chasis_M;

Model lampara_M;

Model caballo_M;
Model pinball_M;


Model Camino_M;
Model Blackhawk_M;
Model Dado_M;

Skybox skybox;

//Temporizador para luces
bool lightOn = false;
int lightTimer = 0;
int lightInterval = 1000;

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
PointLight pointLights2[MAX_POINT_LIGHTS];
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights2[MAX_SPOT_LIGHTS];
SpotLight spotLights3[MAX_SPOT_LIGHTS];
SpotLight spotLights4[MAX_SPOT_LIGHTS];
SpotLight spotLights5[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//funci�n de calculo de normales por promedio de v�rtices 
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

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void Crear12Caras()
{
	unsigned int caras_indices[] = {
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
	};

	GLfloat caras_vertices[] = {
		                      // S        T
		
			// numero 1
			0.0f, 0.0f, 1.0f,		0.05f,  0.7f,		1.0f,	0.0f,	0.0f,	//E
		   -1.0f, 0.0f, 0.0f,		0.33f,  0.7f,		0.0f,	0.0f,	-1.0f,	//B
			0.0f, -1.0f,0.0f,		0.20f,  0.99f,		0.0f,	-1.0f,	0.0f,	//D

			// numero 2
			0.0f, 0.0f, 1.0f,		0.67f,  0.7f,		0.0f,	0.0f,	-1.0f,	//E
			0.0f, -1.0f,0.0f,		0.5f,  0.99f,		0.0f,	-1.0f,	0.0f,	//D
			1.0f, 0.0f, 0.0f,		0.35f,  0.7f,		-1.0f,	0.0f,	0.0f,	//A

			// numero 6
			0.0f, 0.0f, 1.0f,		0.7f,  0.37f,		0.0f,	0.0f,	-1.0f,	//E
			1.0f, 0.0f, 0.0f,		0.99f,  0.37f,		-1.0f,	0.0f,	0.0f,	//A
			0.0f, 1.0f, 0.0f,		0.85f,  0.66f,		0.0f,	1.0f,	.0f,	//C

			// numero 5
			0.0f, 0.0f, 1.0f,		0.67f,  0.39f,		0.0f,	0.0f,	-1.0f,	//E
			0.0f, 1.0f, 0.0f,		0.48f,  0.66f,		0.0f,	1.0f,	0.0f,	//C
		   -1.0f, 0.0f, 0.0f,		0.35f,  0.39f,		1.0f,	0.0f,	0.0f,	//B

		   // numero 4
			0.0f, 0.0f, -1.0f,		0.05f,  0.37f,		1.0f,	0.0f,	-1.0f,	//F
		   -1.0f, 0.0f, 0.0f,		0.33f,  0.37f,		0.0f,	1.0f,	 1.0f,	//B
			0.0f, -1.0f,0.0f,		0.20f,  0.66f,		-1.0f,	0.0f,	-1.0f,	//D

			// numero 3
			0.0f, 0.0f, -1.0f,		0.7f,  0.73f,		1.0f,	0.0f,	-1.0f,	//F
			0.0f, -1.0f,0.0f,		0.85f,  0.99f,		0.0f,	-1.0f,	0.0f,	//D
			1.0f, 0.0f, 0.0f,		0.99f,  0.73f,		-1.0f,	0.0f,	0.0f,	//A

			// numero 7
			0.0f, 0.0f, -1.0f,		0.33f,  0.05f,		1.0f,	0.0f,	-1.0f,	//F
			1.0f, 0.0f, 0.0f,		0.0f,  0.05f,		0.0f,	1.0f,	0.0f,	//A
			0.0f, 1.0f, 0.0f,		0.16f,  0.33f,		-1.0f,	0.0f,	1.0f,	//C

			// numero 8
			0.0f, 0.0f, -1.0f,		0.67f,  0.05f,		1.0f,	0.0f,	-1.0f,	//F
			0.0f, 1.0f, 0.0f,		0.48f,  0.33f,		0.0f,	1.0f,	0.0f,	//C
		   -1.0f, 0.0f, 0.0f,		0.35f,  0.05f,		0.0f,	0.0f,	1.0f,	//B
	};
	Mesh* caras12 = new Mesh();
	caras12->CreateMesh(caras_vertices, caras_indices, 192, 24); //Numero de datos en los vertices 
	meshList.push_back(caras12);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// temporizar
void timer(int value) {
	lightTimer += lightInterval;
	lightOn = !lightOn;

}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	
	CreateObjects();
	CreateShaders();
	Crear12Caras();
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	dadoTexture = Texture("Textures/numerosdados.tga");
	dadoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/carro/carro.obj");

	chasis_M = Model();
	chasis_M.LoadModel("Models/carro/chasis.obj");

	Llanta_M = Model();
	Llanta_M.LoadModel("Models/carro/llanta1.obj");

	Llanta2_M = Model();
	Llanta2_M.LoadModel("Models/carro/llanta2.obj");

	Llanta3_M = Model();
	Llanta3_M.LoadModel("Models/carro/llanta3.obj");

	Llanta4_M = Model();
	Llanta4_M.LoadModel("Models/carro/llanta4.obj");;

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	lampara_M = Model();
	lampara_M.LoadModel("Models/carro/lampara.obj");

	caballo_M = Model();
	caballo_M.LoadModel("Models/carro/caballo.obj");

	pinball_M = Model();
	pinball_M.LoadModel("Models/carro/pinball.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;

	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.01f, 0.0f,
		5.0f);
	spotLightCount++;

	
	/*SpotLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat xDir, GLfloat yDir, GLfloat zDir,
		GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat edg);*/




		            // Luces Coche
	//adelante
	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		40.0f, 4.0f, 0.0f,
		0.03f, 5.0f, 0.3f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	
	//atras
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		-40.0f, 4.0f, 0.0f,
		0.03f, 5.0f, 0.3f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;


	//luz helicoptero
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f, //color
		2.0f, 4.0f, //
		-2.0f, 5.0f, 6.0f, //coordenadas
		0.0f, -5.0f, 0.0f, 
		1.0f, 0.0f, 0.0f,// con,exp
		15.0f); //grados luz
	spotLightCount++;

	/*GLfloat red, GLfloat green, GLfloat blue, 
						GLfloat aIntensity, GLfloat dIntensity, 
						GLfloat xPos, GLfloat yPos, GLfloat zPos, 
						GLfloat con, GLfloat lin, GLfloat exp) : Light(red, green, blue, aIntensity, dIntensity)
*/

	//luz caballo
	pointLights[0] = PointLight(0.01f, 0.78f, 1.0f,
		0.0f, 1.0f,
		0.0f, 5.0f, -55.0f,
		0.8f, 0.01f, 0.000001f);
	pointLightCount++;



	//luz lampara 
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 5.0f, 40.0f,
		0.8f, 0.01f, 0.000001f);
	pointLightCount++;

	pointLights2[0] = pointLights[1];
	pointLights2[1] = pointLights[0];

	//spotLights2[0] = spotLights[0];
	//spotLights2[1] = spotLights[3];
	//spotLights2[2] = spotLights[2];
	spotLights2[0] = spotLights[1];

	//spotLights3[0] = spotLights[0];
	//spotLights3[1] = spotLights[3];
	//spotLights3[2] = spotLights[1];
	spotLights3[0] = spotLights[2];



	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

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
		
		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);


		

		// luz ligada a la c�mara de tipo flash
		//sirve para que en tiempo de ejecuci�n (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		//shaderList[0].SetPointLights(pointLights, pointLightCount);
		//shaderList[0].SetSpotLights(spotLights, spotLightCount);
	
		if (mainWindow.getarregloluces() == 1)
		{
			if (mainWindow.getlinterna() == true)
			{

				shaderList[0].SetPointLights(pointLights2, pointLightCount);
			}
			else
			{

				shaderList[0].SetPointLights(pointLights2, pointLightCount - 1);
			}

		}
		else
		{
			if (mainWindow.getlinterna() == true)
			{

				shaderList[0].SetPointLights(pointLights, pointLightCount);
			}
			else
			{

				shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
			}
		}

		shaderList[0].SetSpotLights(spotLights2, spotLightCount - 1);
		shaderList[0].SetSpotLights(spotLights3, spotLightCount - 1);

		lightTimer += lightInterval;

		if (mainWindow.getcarrodos() == true)
		{
			
					shaderList[0].SetSpotLights(spotLights2, spotLightCount);
					shaderList[0].SetSpotLights(spotLights3, spotLightCount - 1);

					glm::vec3 posCoche = glm::vec3(40.0f, 4.0f, -5.0f);
					posCoche.x += mainWindow.getmueveCarro();
					spotLights[1].SetFlash(posCoche, glm::vec3(1.0f, 0.0f, 0.0f));
					
					//luz temporal para helicoptero
					if (lightTimer == 70000)
					{
						shaderList[0].SetSpotLights(spotLights3, spotLightCount - 1);
						shaderList[0].SetSpotLights(spotLights, spotLightCount);
				
						lightTimer = 0;
					
					}
					else
					{
						//lightTimer += lightInterval;
						shaderList[0].SetSpotLights(spotLights3, spotLightCount - 1);
						shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
						
					
					

					}
					glm::vec3 posHelicoptero = glm::vec3(-2.0f, 4.9f, 6.0f);
					posHelicoptero.x += mainWindow.getmueveHelicoptero();
					spotLights[3].SetFlash(posHelicoptero, glm::vec3(0.0f, -1.0f, 0.0f));

		
		}
		else if (mainWindow.getcarrodos() == false)
		{
			
					shaderList[0].SetSpotLights(spotLights3, spotLightCount);
				    shaderList[0].SetSpotLights(spotLights2, spotLightCount - 1);

					glm::vec3 posCochedos = glm::vec3(-40.0f, 4.0f, -5.0f);
					posCochedos.x += mainWindow.getmueveCarro();
					spotLights[2].SetFlash(posCochedos, glm::vec3(-1.0f, 0.0f, 0.0f));

					//luz temporal para helicoptero
					if (lightTimer == 70000)
					{
						shaderList[0].SetSpotLights(spotLights2, spotLightCount - 1);
						shaderList[0].SetSpotLights(spotLights, spotLightCount);
						
						
						lightTimer = 0;
				
					

					}
					else
					{
						shaderList[0].SetSpotLights(spotLights2, spotLightCount - 1);
						shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
					

					}
					glm::vec3 posHelicoptero = glm::vec3(-2.0f, 4.9f, 6.0f);
					posHelicoptero.x += mainWindow.getmueveHelicoptero();
					spotLights[3].SetFlash(posHelicoptero, glm::vec3(0.0f, -1.0f, 0.0f));
					
		}




	

		/*
		//luz temporal para helicoptero
		if (lightTimer == 70000)
		{

			shaderList[0].SetSpotLights(spotLights, spotLightCount);
			lightTimer = 0;
		
			
		}
		else
		{
			lightTimer += lightInterval;
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
			
			
		}
		*/

		
	
		

	
	    
		

		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmueveCarro(), -0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Instancia del chasis 
		model = modelaux;
		model = glm::translate(model, glm::vec3(-32.0f, 16.3f, 11.8f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		chasis_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-29.0f, 10.0f, 14.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(20.0f, 10.0f, 13.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta2_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-29.0f, 10.0f, -13.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta3_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(20.0f, 10.0f, -13.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta4_M.RenderModel();
		
	

		//Helicoptero

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmueveHelicoptero(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();


		//dado
		color = glm::vec3(0.7f, 0.7, 0.7f);//color blanco, multiplica a la informaci�n de color de la textura
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 40.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();

		//lampara

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 50.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara_M.RenderModel();

		//caballo

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -50.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		caballo_M.RenderModel();

		//pinball

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -70.0f));
		model = glm::scale(model, glm::vec3(1.4f, 1.4f, 1.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pinball_M.RenderModel();


		//Agave �qu� sucede si lo renderizan antes del coche y el helic�ptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f)); 
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
