#Dependencias:
#http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:download ou Moodle
#copiar libAntTweakBar.so e libAntTweakBar.so.1 para /usr/local/lib
#após usar o comando ldconfig como root
CPPSOURCES = $(wildcard *.cpp)
CXX      	= g++
LIBS     	= -lAntTweakBar -lGL -lglfw -lGLEW


Target: AntTweakBar

all:	Target

AntTweakBar: $(CPPSOURCES)
	$(CXX)  common/shader.cpp common/text2D.cpp common/texture.cpp $(CPPSOURCES) $(LIBS) -o Trabalho5 -Wall
run:
	./Trabalho5