#include<bits/stdc++.h>
#include <unistd.h>
#include<string>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "common/shader.hpp"
// Include AntTweakBar
#include "AntTweakBarCode/AntTweakBar/include/AntTweakBar.h"
using namespace glm;
using namespace std;

#define NUM_SHAPES 3
typedef enum { SHAPE_TEAPOT=1, SHAPE_TORUS, SHAPE_CONE } Shape;
Shape g_CurrentShape = SHAPE_TORUS;

const GLint WIDTH = 1280, HEIGHT = 768;
const GLfloat R = 1.0f, G = 1.0f, B = 1.0f, A = 0.0f;
double xposMouse, yposMouse, zoommm = 1.0f, zoomhulk = 1.0f, Person, present;
int widthWindow, heightWindow;
bool selecionamm = false, selecionahulk = false, selecionaanime = false;

const float mp = 3.1415926/180.0;
const float uqp = 3.1415926/90.0;

float XT=0,YT=0,GRAUS=0,XE=1,YE=1,TANXS=0, TANYS=0, E=1, XTM=0,YTM=0,GRAUSM=0,XEM=1,YEM=1,TANXSM=0, TANYSM=0, EM=1;
GLuint colorbuffer, vertexbuffer, MatrixIDXT, MatrixIDYT, MatrixIDGRAUS, MatrixIDXE, MatrixIDYE, MatrixIDTANXS, MatrixIDTANYS, MatrixIDE, MatrixIDXTM, 
MatrixIDYTM, MatrixIDGRAUSM, MatrixIDXEM, MatrixIDYEM, MatrixIDTANXSM, MatrixIDTANYSM, MatrixIDEM,programID, intPerson;

char * instrucoes = strdup ( "Primeiramente Selecione o personagem na Barra da esquerda clicando no desejado" ) ;
char * instrucoes2 = strdup ( "Para animacao clique na opcao correspondente na Barra da esquerda." ) ;
char * instrucoes3 = strdup ( "Para selecionar os personagens nas configuracoes de Origem pressione a tecla R " ) ;
char * itranslacao = strdup ( "Clique o botão direito do mouse, e arraste para transladar " ) ;
char * irotacao = strdup ( "Horario: Seta pra cima + Seta pra direita, Anti Horario: Seta pra cima + Seta pra esquerda" ) ;
char * iescalanuniforme = strdup ( "Pra Y: Seta pra cima + Seta pra Baixo, Para X: Seta pra Esquerda + Seta pra Direita" ) ;
char * iescalauniforme = strdup ( "Pra aumentar: Seta pra Cima, Pra diminuir: Seta Pra Baixo" ) ;
char * ishearx = strdup ( "Para X: Shift + Seta Esquerda (AntiHorario), Shift + Seta Direita (Horario)" ) ;
char * isheary = strdup ( "Para Y: Shift + Seta pra Baixo (Diminui), Shift + Seta pra Cima (Cresce)" ) ;

void adicionaBarras(){
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT-50);//Alterar tamanho da Janela

	TwBar * bar = TwNewBar("Barra de Controle");
	TwBar * bar2 = TwNewBar("Barra de Informacoes");

	TwSetParam(bar, NULL, "position", TW_PARAM_CSTRING, 1, "15 15");
	TwSetParam(bar, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
    TwAddVarRW(bar, "Select MM", TW_TYPE_BOOL8 , &selecionamm, NULL);
    TwAddVarRW(bar, "Select HULK", TW_TYPE_BOOL8 , &selecionahulk, NULL);
    TwAddVarRW(bar, "Escalar Uniforme MM", TW_TYPE_DOUBLE, &zoommm, 
        " label='Escalar Unif MM' min=0 max=20 step=0.001  help='Mostra qual a escala atual do personagem' ");

    TwAddVarRW(bar, "Escalar Uniforme Hulk", TW_TYPE_DOUBLE, &zoomhulk, 
        " label='Escalar Unif Hulk' min=0 max=20 step=0.001  help='Mostra qual a escala atual do personagem' ");

    TwAddVarRW(bar, "ANIMAR", TW_TYPE_BOOL8 , &selecionaanime, NULL);
    TwSetParam(bar2, NULL, "position", TW_PARAM_CSTRING, 1, "1050 15");
	TwSetParam(bar2, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
    TwAddVarRW(bar2, "1. ", TW_TYPE_CDSTRING , &instrucoes, NULL);
    TwAddVarRW(bar2, "2. ", TW_TYPE_CDSTRING , &instrucoes2, NULL);
    TwAddVarRW(bar2, "3. ", TW_TYPE_CDSTRING , &instrucoes3, NULL);
    TwAddVarRW(bar2, "Translacao:", TW_TYPE_CDSTRING , &itranslacao, NULL);
    TwAddVarRW(bar2, "Rotacao:", TW_TYPE_CDSTRING , &irotacao, NULL);
    TwAddVarRW(bar2, "Escala Nao Unif:", TW_TYPE_CDSTRING , &iescalanuniforme, NULL);
    TwAddVarRW(bar2, "Escala Unif:", TW_TYPE_CDSTRING , &iescalauniforme, NULL);
    TwAddVarRW(bar2, "Shear X:", TW_TYPE_CDSTRING , &ishearx, NULL);
    TwAddVarRW(bar2, "Shear Y:", TW_TYPE_CDSTRING , &isheary, NULL);
}

void shaderLoadCreat(){
	//Leitura e compilação dos Shaders em tempo de execução
	programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
	intPerson = glGetUniformLocation(programID, "Person");// PASSA A LOCALIZACAO DA VARIAVEL DA ESCOLHA DO PERSONAGEM PRO VERTEX SHADER
	MatrixIDXTM = glGetUniformLocation(programID, "XTM");
	MatrixIDYTM = glGetUniformLocation(programID, "YTM");
	MatrixIDGRAUSM = glGetUniformLocation(programID, "GRAUSM");
	MatrixIDXEM = glGetUniformLocation(programID, "XEM");
	MatrixIDYEM = glGetUniformLocation(programID, "YEM");
	MatrixIDTANXSM = glGetUniformLocation(programID, "TANXSM");
	MatrixIDTANYSM = glGetUniformLocation(programID, "TANYSM");
	MatrixIDEM = glGetUniformLocation(programID, "EM");
	MatrixIDXT = glGetUniformLocation(programID, "XT");
	MatrixIDYT = glGetUniformLocation(programID, "YT");
	MatrixIDGRAUS = glGetUniformLocation(programID, "GRAUS");
	MatrixIDXE = glGetUniformLocation(programID, "XE");
	MatrixIDYE = glGetUniformLocation(programID, "YE");
	MatrixIDTANXS = glGetUniformLocation(programID, "TANXS");
	MatrixIDTANYS = glGetUniformLocation(programID, "TANYS");
	MatrixIDE = glGetUniformLocation(programID, "E");

}

void sendTransfMM(){
	glUniform1f(MatrixIDXTM, XTM);
	glUniform1f(MatrixIDYTM, YTM);
	glUniform1f(MatrixIDGRAUSM, GRAUSM*mp);
	glUniform1f(MatrixIDEM, EM);
	glUniform1f(MatrixIDTANXSM, TANXSM*uqp);
	glUniform1f(MatrixIDTANYSM, TANYSM*uqp);
	glUniform1f(MatrixIDXEM, XEM);
	glUniform1f(MatrixIDYEM, YEM);
}

void sendTransfHulk(){
	glUniform1f(MatrixIDXT, XT);
	glUniform1f(MatrixIDYT, YT);
	glUniform1f(MatrixIDGRAUS, GRAUS*mp);
	glUniform1f(MatrixIDE, E);
	glUniform1f(MatrixIDTANXS, TANXS*uqp);
	glUniform1f(MatrixIDTANYS, TANYS*uqp);
	glUniform1f(MatrixIDXE, XE);
	glUniform1f(MatrixIDYE, YE);
}

void MouseKeyboardMovimentObject(){
	int botao;
	glfwGetCursorPos(window, &xposMouse, &yposMouse);
	glfwGetWindowSize(window,&widthWindow, &heightWindow);
	double horizontal = double(xposMouse * 2 - widthWindow)/double(widthWindow);
	double vertical = double(heightWindow - yposMouse * 2)/double(heightWindow);
	present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if(present==1){
		const unsigned char *estadobotao = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &botao);
		if (estadobotao[0] == GLFW_PRESS) 	//BUTTON_UP
		{		
			YTM += 0.01;
		}
		
		if (estadobotao[2] == GLFW_PRESS) 	//BUTTON_DOWN
		{		
			YTM += -0.01;
		}
		
		if (estadobotao[1] == GLFW_PRESS) 	//BUTTON_RIGHT
		{		
			XTM += 0.01;
		}
		
		if (estadobotao[3] == GLFW_PRESS) 	//BUTTON_LEFT
		{		
			XTM += -0.01;
		}
	}else{

		if (selecionamm && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{	
			XTM = horizontal;
			YTM = vertical-0.2;
		}
		}
	
		if (selecionahulk && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			XT = horizontal-0.84;
			YT = vertical+0.35;
		}
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{		
			GRAUS += 1;
		}
	
		if (selecionamm	&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{		
			GRAUSM += 1;
		}
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{		
			GRAUS += -1;
		}
	
		if (selecionamm	&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{		
			GRAUSM += -1;
		}
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{		
			YE += 0.001;
		}
	
		if (selecionamm && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{		
			YEM += 0.001;
		}
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{		
			XE += 0.001;
		}
	
		if (selecionamm && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{		
			XEM += 0.001;
		}
	
		if (selecionahulk 	&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{		
			E += 0.001;
		}
	
		if (selecionamm 	&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{		
			EM += 0.001;
		}
	
		if (selecionahulk 	&& glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{		
			E -= 0.001;
		}
	
		if (selecionamm 	&& glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{		
			EM -= 0.001;
		}
	
		if (selecionaanime)
		{	
			E +=-0.001;
			EM +=-0.001;
			GRAUSM+= 1.0;
			GRAUS+= -1.0;
		}
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{	
			TANXS += 0.1;
		}
	
		if (selecionamm && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{	
			TANXSM += 0.1;
		}
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{	
			TANXS += -0.1;
		}
	
		if (selecionamm && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{	
			TANXSM += -0.1;
		}
	
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{	
			TANYS += 0.1;
		}
	
		if (selecionamm && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{	
			TANYSM += 0.1;
		}
	
		if (selecionahulk && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{	
			TANYS += -0.1;
		}
	
		if (selecionamm && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{	
			TANYSM += -0.1;
		}
	
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{	
			XT=0;YT=0;GRAUS=0;XE=1;YE=1;TANXS=0;TANYS=0;E=1; XTM=0;YTM=0;GRAUSM=0;XEM=1;YEM=1;TANXSM=0;TANYSM=0; EM=0.6, zoommm = 1, zoomhulk = 1;
		}
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
	glClearColor(R, G, B, A);
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

void drawModel(int selecionaPersonagem, GLenum type, std::vector<glm::vec2> vertices, GLfloat R, GLfloat G, GLfloat B){
		if(selecionaPersonagem == 1){
			glUniform1i(intPerson, 1);
			sendTransfMM();
		}
		if(selecionaPersonagem == 2){ 	
			glUniform1i(intPerson, 2);	
			sendTransfHulk();
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
	std::vector<glm::vec2> asteroide0 = loadModel("./coordinates/asteroide0"); 
	std::vector<glm::vec2> asteroide1 = loadModel("./coordinates/asteroide1"); 
	std::vector<glm::vec2> asteroide2 = loadModel("./coordinates/asteroide2"); 
	std::vector<glm::vec2> asteroide3 = loadModel("./coordinates/asteroide3"); 
		

	do{	
		// Limpa a Tela
		glClear( GL_COLOR_BUFFER_BIT );
		// Para definir os Shaders
		glUseProgram(programID);
		// Primeiro buffer de atributo: vértices
		configLayout(vertexbuffer, colorbuffer);
		drawModel(1, GL_TRIANGLES, asteroide0, 0.0, 0.7, 0.0);
		drawModel(1, GL_TRIANGLES, asteroide1, 0.0, 0.7, 0.0);
		drawModel(1, GL_TRIANGLES, asteroide2, 0.0, 0.7, 0.0);
		drawModel(1, GL_TRIANGLES, asteroide3, 0.0, 0.7, 0.0);
		
		MouseKeyboardMovimentObject();
		if(selecionahulk){
			E = zoomhulk;
			E = zoomhulk;
		}

		if(selecionamm){
			EM = zoommm;
			EM = zoommm;
		}
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		TwDraw();	
	 	glfwSwapBuffers(window);
	 	glfwPollEvents();
}
	while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
		destroyWindows(vertexbuffer, colorbuffer, VertexArrayID, programID);
	return 0;
}
