#include<bits/stdc++.h>
#include <unistd.h>
#include<string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "common/text2D.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "common/shader.hpp"
#include "AntTweakBarCode/AntTweakBar/include/AntTweakBar.h"
using namespace glm;
using namespace std;
#define NUM_SHAPES 3
GLFWwindow* window;

typedef enum { SHAPE_TEAPOT=1, SHAPE_TORUS, SHAPE_CONE } Shape;
Shape g_CurrentShape = SHAPE_TORUS;

const GLint WIDTH = 1366, HEIGHT = 768;
const GLfloat R = 0.0f, G = 0.0f, B = 0.0f, A = 1.0f;
double xposMouse, yposMouse, zoommm = 1.0f, zoomhulk = 1.0f, Person, present;
int widthWindow, heightWindow, selecionamm, selecionahulk, selecionaanime;

const float mp = 3.1415926/180.0;
const float uqp = 3.1415926/90.0;

float escalaAsteroide1=1,escalaAsteroide2=1,escalaAsteroide3=1,escalaAsteroide4=1,xTranslacaoNave=0,yTranslacaoNave=0;
GLuint colorbuffer, vertexbuffer, MatrixIDescalaAsteroide1,MatrixIDescalaAsteroide2,MatrixIDescalaAsteroide3,MatrixIDescalaAsteroide4, 
MatrixIDxTranslacaoNave,MatrixIDyTranslacaoNave ,programID, Object;
char * isheary = strdup ( "Para Y: Shift + Seta pra Baixo (Diminui), Shift + Seta pra Cima (Cresce)" ) ;

void TW_CALL RunCB(void *)
{ 
  TwTerminate();
}

void adicionaBarras(){
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT-50);//Alterar area que cobre o mouse
	
	TwBar * bar = TwNewBar("Space_Invaders");
	TwDefine("Space_Invaders position='0 0'  size='1366 768' refresh=0.5");
	TwDefine(" Space_Invaders color='0 0 0' text=white ");
	TwDefine(" Space_Invaders alpha=255 "); 
    TwAddVarRW(bar, "Select MM", TW_TYPE_BOOL8 , &selecionamm, NULL);
    TwAddVarRW(bar, "Select HULK", TW_TYPE_BOOL8 , &selecionahulk, NULL);
    TwDefine(" Space_Invaders help='Para reiniciar o a saga precione R' ");
	TwAddVarRW(bar, "Shear Y:", TW_TYPE_CDSTRING , &isheary, NULL);
	TwAddButton(bar, " run ", RunCB, NULL, " Label='Start Game' ");
}

void shaderLoadCreat(){
	//Leitura e compilação dos Shaders em tempo de execução
	programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
	Object = glGetUniformLocation(programID, "Object");// PASSA A LOCALIZACAO DA VARIAVEL DA ESCOLHA DO PERSONAGEM PRO VERTEX SHADER
	MatrixIDescalaAsteroide1 = glGetUniformLocation(programID, "escalaAsteroide1");
	MatrixIDescalaAsteroide2 = glGetUniformLocation(programID, "escalaAsteroide2");
	MatrixIDescalaAsteroide3 = glGetUniformLocation(programID, "escalaAsteroide3");
	MatrixIDescalaAsteroide4 = glGetUniformLocation(programID, "escalaAsteroide4");
	MatrixIDxTranslacaoNave = glGetUniformLocation(programID, "xTranslacaoNave");
	MatrixIDyTranslacaoNave = glGetUniformLocation(programID, "yTranslacaoNave");
}

void sendTransfNaveValdir(){
	glUniform1f(MatrixIDxTranslacaoNave, xTranslacaoNave);
	glUniform1f(MatrixIDyTranslacaoNave, yTranslacaoNave);
}

void sendTransfAsteroide1(){
	glUniform1f(MatrixIDescalaAsteroide1, escalaAsteroide1);	
}

void sendTransfAsteroide2(){
	glUniform1f(MatrixIDescalaAsteroide2, escalaAsteroide2);	
}

void sendTransfAsteroide3(){
	glUniform1f(MatrixIDescalaAsteroide3, escalaAsteroide3);	
}

void sendTransfAsteroide4(){
	glUniform1f(MatrixIDescalaAsteroide4, escalaAsteroide4);	
}

void MouseKeyboardMovimentObject(){
	glfwGetCursorPos(window, &xposMouse, &yposMouse);
	glfwGetWindowSize(window,&widthWindow, &heightWindow);
	double horizontal = double(xposMouse * 2 - widthWindow)/double(widthWindow);
	double vertical = double(heightWindow - yposMouse * 2)/double(heightWindow);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{	
		xTranslacaoNave = horizontal;
		yTranslacaoNave = vertical;
	}

	}

glm::vec4 limitesBoundingBox(std::vector<glm::vec2> objeto){
	glm::vec4 maiormenor = glm::vec4(objeto[0].x, objeto[0].y, objeto[0].x, objeto[0].y);

	for(int i=1 ;i < objeto.size() ;i++){
		if(objeto[i].x > maiormenor.x){
			maiormenor.x = objeto[i].x; //maiormenor[maiorX][maiorY][menorX][MenorY] 
		}if(objeto[i].y > maiormenor.y){
			maiormenor.y = objeto[i].y; //maiormenor[maiorX][maiorY][menorX][MenorY]
		}if(objeto[i].x < maiormenor.z){
			maiormenor.z = objeto[i].x; //maiormenor[maiorX][maiorY][menorX][MenorY]
		}if(objeto[i].y < maiormenor.w){
			maiormenor.w = objeto[i].y; //maiormenor[maiorX][maiorY][menorX][MenorY]
		}
	}
	return maiormenor;
}

int colisaoEntreCaixas(glm::vec4 a, glm::vec4 b){
   if((a.z <= b.x && a.x >= b.z) && (a.w <= b.y && a.y >= b.w)){
	   return 1;
   }else
   return 0;
}

int initWindow(){
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( WIDTH, HEIGHT, "Heros", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	// Aplica o contexto atual da máquina de estados para Janela Atual
	glfwMakeContextCurrent(window);

	glewExperimental = true; // Necessário para core profile
	//deve ativar antes da chamada glewInit()
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	adicionaBarras();
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
 	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//Alterado
	// Set the mouse at the center of the screen
	glfwPollEvents();
	glClearColor(R,G,B,A);
	glDepthFunc(GL_LESS);
	return 1;
}

std::vector<glm::vec2> loadModel(const char *path){
	FILE * file = fopen(path, "r");
	std::vector<glm::vec2> vertices;
	if(file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		fclose(file);
		getchar();
		return vertices;
	}

	while(1){
		glm::vec2 vertex;
		int res = fscanf(file, "%f %f\n", &vertex.x,&vertex.y);
		if(res == EOF)
			break;
		vertices.push_back(vertex);
	}
	fclose(file);
	return vertices;
}

std::vector<glm::vec3> colorir(int tam, float R, float G, float B){
	int i;
	std::vector<glm::vec3> vertexColor;
	
	for(i=0;i<tam;i++){
		glm::vec3 color;
		color.r = R;
		color.g = G;
		color.b = B;
		vertexColor.push_back(color);		
	}
	return vertexColor;
}

void drawModel(int selecionaObject, GLenum type, std::vector<glm::vec2> vertices, GLfloat R, GLfloat G, GLfloat B){
		if(selecionaObject == 0){
			glUniform1i(Object, 0);
			sendTransfNaveValdir();
		}
		else if(selecionaObject == 1){ 	
			glUniform1i(Object, 1);	
			sendTransfAsteroide1();
		}
		else if(selecionaObject == 2){ 	
			glUniform1i(Object, 2);	
			sendTransfAsteroide2();
		}
		else if(selecionaObject == 3){ 	
			glUniform1i(Object, 3);	
			sendTransfAsteroide3();
		}
		else if(selecionaObject == 4){ 	
			glUniform1i(Object, 4);	
			sendTransfAsteroide4();
		}
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	std::vector<glm::vec3> color = colorir(vertices.size(),R,G,B);
	glBufferData(GL_ARRAY_BUFFER, color.size()* sizeof(glm::vec3), &color[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()* sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glDrawArrays(type, 0, vertices.size());
}

void configLayout(GLuint vertexbuffer, GLuint colorbuffer){
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
}

void destroyWindows(GLuint vertexbuffer, GLuint colorbuffer, GLuint VertexArrayID, GLuint programID){
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	TwTerminate();
	glfwTerminate();
}


int main( void ){
	initWindow();
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	//variaveis para o vertexshaders
	shaderLoadCreat();
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &colorbuffer);
	std::vector<glm::vec2> asteroide1 = loadModel("./coordinates/asteroide1");
	std::vector<glm::vec2> asteroide2 = loadModel("./coordinates/asteroide2");
	std::vector<glm::vec2> asteroide3 = loadModel("./coordinates/asteroide3");
	std::vector<glm::vec2> asteroide4 = loadModel("./coordinates/asteroide4"); 
	std::vector<glm::vec2> nave_valdir = loadModel("./coordinates/nave_valdir");
	initText2D( "Holstein.DDS" );
	do{	
		// Limpa a Tela
		glClear( GL_COLOR_BUFFER_BIT );
		// Para definir os Shaders
		glUseProgram(programID);
		// Primeiro buffer de atributo: vértices
		configLayout(vertexbuffer, colorbuffer);
		drawModel(1, GL_TRIANGLES, asteroide1, 0.0, 1.0, 0.0);
		drawModel(2, GL_TRIANGLES, asteroide2, 0.0, 1.0, 0.0);
		drawModel(3, GL_TRIANGLES, asteroide3, 0.0, 1.0, 0.0);
		drawModel(4, GL_TRIANGLES, asteroide4, 0.0, 1.0, 0.0);
		drawModel(0, GL_TRIANGLES, nave_valdir, 0.0, 1.0, 0.0);
		MouseKeyboardMovimentObject();
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		TwDraw();	
		
		char text[256];
		sprintf(text,"%.2f sec", glfwGetTime() );
		printText2D(text, 10, 50, 30);

	 	glfwSwapBuffers(window);
		glfwPollEvents();

		glm::vec4 teste = glm::vec4(10,10,-10,-10);
		glm::vec4 teste2 = glm::vec4(60,60,30,30);
		//printf("%f\n %f\n %f\n %f\n",teste.x,teste.y,teste.z,teste.w);
		if(colisaoEntreCaixas(teste,teste2)){
			printf("Tem Colisao\n");
		}
}
	while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
		destroyWindows(vertexbuffer, colorbuffer, VertexArrayID, programID);
	return 0;
}
