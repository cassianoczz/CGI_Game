#include <bits/stdc++.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
 
// Include GLM
#include <glm/glm.hpp>
#include </usr/include/glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "common/shader.hpp"
// Include AntTweakBar
#include "AntTweakBar/include/AntTweakBar.h"

#define NUM_SHAPES 3
typedef enum { SHAPE_TEAPOT=1, SHAPE_TORUS, SHAPE_CONE } Shape;
Shape g_CurrentShape = SHAPE_TORUS;
const GLint WIDTH = 1280, HEIGHT = 768;
const GLfloat R = 0.0f, G = 0.0f, B = 0.3f, A = 0.0f;

bool gLookAtOther = true;
double zoom = 1.0f, g_Rotation = 0, g_LightDirection[] = { -0.57735f, -0.57735f, -0.57735f };
double g_MatAmbient[] = { 0.5f, 0.0f, 0.0f, 1.0f };
double xposMouse, yposMouse;
int widthWindow, heightWindow;


glm::mat3 translation = glm::mat3(1.0f);
glm::mat3 escala = glm::mat3(1.0f);

void adicionaBarras(){
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT-50);//Alterar tamanho da Janela

	TwBar * bar = TwNewBar("TweakBar");
	TwSetParam(bar, NULL, "position", TW_PARAM_CSTRING, 1, "10 10");
	TwSetParam(bar, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.

    TwAddVarRW(bar, "Habilita Zoom", TW_TYPE_BOOL8 , &gLookAtOther, NULL);
    TwAddVarRW(bar, "zoom", TW_TYPE_DOUBLE, &zoom, 
               " label='zoom' min=0 max=20 step=0.001  help='Incrementa e decrementa o triangulo' ");

    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation, 
               " label='Object rotation' opened=true help='Change the object orientation.' ");
   	TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &g_LightDirection, 
               " label='Light direction' opened=true help='Change the light direction.' ");
   	TwAddVarRW(bar, "Ambient", TW_TYPE_COLOR3F, &g_MatAmbient, " group='Material' ");

   {
        // ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
        TwEnumVal shapeEV[NUM_SHAPES] = { {SHAPE_TEAPOT, "Teapot"}, {SHAPE_TORUS, "Torus"}, {SHAPE_CONE, "Cone"} };
        // Create a type for the enum shapeEV
        TwType shapeType = TwDefineEnum("ShapeType", shapeEV, NUM_SHAPES);
        // add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
        TwAddVarRW(bar, "Shape", shapeType, &g_CurrentShape, " keyIncr='<' keyDecr='>' help='Change object shape.' ");
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

 	window = glfwCreateWindow( WIDTH, HEIGHT, "Color triangle", NULL, NULL);
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
	//Habilita a captura das teclas retornando GLFW_PRESS pela funcao glfwGetKey()
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//  Mostra o fragmento que estiver mais próximo da câmera Z-Buffer
	glDepthFunc(GL_LESS);
	return 1;
}


void destroyWindows(GLuint vertexbuffer, GLuint colorbuffer, GLuint VertexArrayID, GLuint programID){
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	TwTerminate();
	glfwTerminate();
}

void MouseKeyboardMovimentObject(){
	glfwGetCursorPos(window, &xposMouse, &yposMouse);
	glfwGetWindowSize(window,&widthWindow, &heightWindow);
	double horizontal = double(xposMouse * 2 - widthWindow)/double(widthWindow);
	double vertical = double(heightWindow - yposMouse * 2)/double(heightWindow);
	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{		
		escala[0][0] += 0.001;
		escala[1][1] += 0.001;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{		
		escala[0][0] -= 0.001;
		escala[1][1] -= 0.001;
	}
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

int main( void )
{
 	initWindow();

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );


	static const GLfloat g_vertex_buffer_data[] = { 
		-0.1f, 0.0f,
		0.1f, 0.0f,
		0.0f, 0.1f,
	};

	static const GLfloat g_color_buffer_data[] = { 
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);

	GLuint MatrixID = glGetUniformLocation(programID, "MatrizCombinada");
	glm::mat3 MatrizCombinada = glm::mat3(1.0f);
	
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		
		configLayout(vertexbuffer, colorbuffer);
		
		MouseKeyboardMovimentObject();

		if(gLookAtOther){
			escala[0][0] = zoom;
			escala[1][1] = zoom;
		}
		MatrizCombinada = translation * escala;
		glUniformMatrix3fv(MatrixID, 1, GL_TRUE, &MatrizCombinada[0][0]);
		//Triangulo 1
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		//color RGB
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
		//vertexs
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data)); 
			

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		TwDraw();

		glfwSwapBuffers(window);
		glfwPollEvents();

	} 
	while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS
		&& glfwWindowShouldClose(window) == 0);

	destroyWindows(vertexbuffer, colorbuffer, VertexArrayID, programID);

	return 0;
}

