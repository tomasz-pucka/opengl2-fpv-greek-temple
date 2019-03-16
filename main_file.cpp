#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <lodepng.h>
#include "constants.h"
#include "objloader.hpp"
#include <iostream>

using namespace glm;
using namespace std;

glm::vec3 cameraPos   = glm::vec3(0.0f, 3.0f,  40.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.2f;

int flag = 0; //0 - inside, 1 - outside

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last fra
float lastX = 700, lastY = 350;
float yaw1, pitch1;
bool firstMouse = true;
float aspect=2; //Aktualny stosunek szerokości do wysokości okna
GLuint vertexbuffer[6]; //0 - floor; 1 - roof; 2 - column; 3 - ground, 4 - background, 5 - statue
GLuint uvbuffer[6];
GLuint normalsbuffer[6];
GLuint texture[3]; //0 - marble; 1 - rock ground, 2 - background
struct obj {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> vertex_normals;
};
unsigned short obj_count=6;
struct obj obj_floor;
struct obj obj_column;
struct obj obj_roof;
struct obj obj_ground;
struct obj obj_background;
struct obj obj_statue;

float rgba_ambient[]={1,1,1,1};
float rgba_diffuse[]={1,1,1,1};
float rgba_statue_ambient[]={0.95,0.9,0.8,1};
float rgba_statue_diffuse[]={0.8,0.8,0.8,1};
float rgba_specular[]={0,0,0,1};
float rgba_emission[]={0,0,0,1};
float rgba_sun_emission[]={1,1,1,1};
unsigned short shininess=0;

float sun_light_pos[]={15,140,160,1};
float temple_light_pos[]={8,8,-10,1};
float sun_light_ambient[]={0.4,0.4,0.4,1};
float sun_light_diffuse[]={0.5,0.5,0.5,1};
float temple_light_ambient[]={0.1,0.1,0.1,1};
float temple_light_diffuse[]={0.3,0.3,0.3,1};
float light_specular[]={1,1,1,1};
//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura obługi zmiany rozmiaru bufora ramki
void windowResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); //Obraz ma być generowany w oknie o tej rozdzielczości
    if (height!=0) {
	aspect=(float)width/(float)height; //Stosunek szerokości do wysokości okna
    } else {
	aspect=1;
    }
}

void collisionDetector () {
        switch(flag) {
    case 0:
        if(cameraPos.x >= 7)
            cameraPos.x = 7;
        if(cameraPos.x <= -7)
            cameraPos.x = -7;
        if(cameraPos.z >= 6.7 && (cameraPos.x >= 0.7 || cameraPos.x <= -0.7))
            cameraPos.z = 6.7;
        if(cameraPos.z <= -0.5 && cameraPos.x >= -5.5 && cameraPos.x <= 5.5)
            cameraPos.z = -0.5;
        if(cameraPos.z <= -0.5 && cameraPos.x >= 5.5) {
            cout << flag << endl;
            flag = 17;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z <= -0.5 && cameraPos.x <= -5.5) {
            cout << flag << endl;
            flag = 18;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= 6.8){
            cout << flag << endl;
            flag = 1;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 1:
        if(cameraPos.z <= 6.8 && (cameraPos.x <= 0.7 && cameraPos.x >= -0.7)){
            cout << flag << endl;
            flag = 0;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z <= 11.8 && cameraPos.x >= 0.7 && cameraPos.x <= 5.2)
            cameraPos.x = 0.7;
        if(cameraPos.z <= 11.8 && cameraPos.x <= -0.7 && cameraPos.x >= -5.2)
            cameraPos.x = -0.7;
        if(cameraPos.z >= 11.8){
            cout << flag << endl;
            flag = 2;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 2:
        if(cameraPos.z <= 11.7 && cameraPos.x <= 0.7 && cameraPos.x >= -0.7){
            cout << flag << endl;
            flag = 1;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z <= 11.8 && cameraPos.x >= 0.7 && cameraPos.x <= 5.2)
            cameraPos.z = 11.8;
        if(cameraPos.z <= 11.8 && cameraPos.x <= -0.7 && cameraPos.x >= -5.2)
            cameraPos.z = 11.8;
        if(cameraPos.z <= 10.3 && cameraPos.x >= 5.2 && cameraPos.x <= 11)
            cameraPos.z = 10.3;
        if(cameraPos.z <= 10.3 && cameraPos.x <= -5.2 && cameraPos.x >= -11)
            cameraPos.z = 10.3;
        if(cameraPos.z >= 17.7 && cameraPos.x >= 0.7 && cameraPos.x <= 5.2)
            cameraPos.z = 17.7;
        if(cameraPos.z >= 17.7 && cameraPos.x <= -0.7 && cameraPos.x >= -5.2)
            cameraPos.z = 17.7;
        if(cameraPos.z >= 17.7 && cameraPos.x >= 6.7 && cameraPos.x <= 11)
            cameraPos.z = 17.7;
        if(cameraPos.z >= 17.7 && cameraPos.x <= -6.7 && cameraPos.x >= -11)
            cameraPos.z = 17.7;
        if(cameraPos.x >= 12.7 && cameraPos.z <= 16.3 && cameraPos.z >= 11.7)
            cameraPos.x = 12.7;
        if(cameraPos.x <= -12.7 && cameraPos.z <= 16.3 && cameraPos.z >= 11.7)
            cameraPos.x = -12.7;
        if(cameraPos.x >= 12.7 && ((cameraPos.z <= 17.7 && cameraPos.z >= 16.3) || (cameraPos.z <= 11.7 && cameraPos.z >= 11.3))) {
            cout << flag << endl;
            flag = 8;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x <= -12.7 && ((cameraPos.z <= 17.7 && cameraPos.z >= 16.3) || (cameraPos.z <= 11.7 && cameraPos.z >= 11.3))) {
            cout << flag << endl;
            flag = 9;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= 17.7 && ((cameraPos.x <= 0.7 && cameraPos.x >= -0.7) || (cameraPos.x <= 6.7 && cameraPos.x >= 5.3) || (cameraPos.x <= 12.7 && cameraPos.x >= 11) ||
                                   (cameraPos.x >= -6.7 && cameraPos.x <= -5.3) || (cameraPos.x >= -12.7 && cameraPos.x <= -11))){
            cout << flag << endl;
            flag = 5;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z <= 10.3 && cameraPos.x >= 11.3){
            cout << flag << endl;
            flag = 3;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z <= 10.3 && cameraPos.x <= -11.3){
            cout << flag << endl;
            flag = 4;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 3:
        if(cameraPos.z >= 10.3 && cameraPos.x >= 11){
            cout << flag << endl;
            flag = 2;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x <= 11)
            cameraPos.x = 11;
        if(cameraPos.x >= 12.7 && cameraPos.z <=10.3 && cameraPos.z >= 5.7)
            cameraPos.x = 12.7;
        if(cameraPos.x >= 12.7 && cameraPos.z <= 4.3 && cameraPos.z >= -0.3)
            cameraPos.x = 12.7;
        if(cameraPos.x >= 12.7 && cameraPos.z <= -1.7 && cameraPos.z >= -6.3)
            cameraPos.x = 12.7;
        if(cameraPos.x >= 12.7 && cameraPos.z <= -7.7 && cameraPos.z >= -12.3)
            cameraPos.x = 12.7;
        if(cameraPos.x >= 12.7 && cameraPos.z <= -13.7 && cameraPos.z >= -18.3)
            cameraPos.x = 12.7;
        if(cameraPos.x >= 12.7 && cameraPos.z <= -19.7 && cameraPos.z >= -24.3)
            cameraPos.x = 12.7;
        if(cameraPos.x >= 12.7 && ((cameraPos.z <= 5.7 && cameraPos.z >= 4.3) || (cameraPos.z <= -0.3 && cameraPos.z >= -1.7) ||
                                   (cameraPos.z <= -6.3 && cameraPos.z >= -7.7) || (cameraPos.z <= -12.3 && cameraPos.z >= -13.7) || (cameraPos.z <= -18.3 && cameraPos.z >= -19.7))) {
            cout << flag << endl;
            flag = 8;
            cout << "zmiana na " << flag << endl;
        }
        if(cameraPos.z <= -24.3) {
            cout << flag << endl;
            flag = 7;
            cout << "zmiana na " << flag << endl;
        }
        break;

    case 4:
        if(cameraPos.z >= 10.3 && cameraPos.x <= -11){
            cout << flag << endl;
            flag = 2;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x >= -11)
            cameraPos.x = -11;
        if(cameraPos.x <= -12.7 && cameraPos.z <=10.3 && cameraPos.z >= 5.7)
            cameraPos.x = -12.7;
        if(cameraPos.x <= -12.7 && cameraPos.z <= 4.3 && cameraPos.z >= -0.3)
            cameraPos.x = -12.7;
        if(cameraPos.x <= -12.7 && cameraPos.z <= -1.7 && cameraPos.z >= -6.3)
            cameraPos.x = -12.7;
        if(cameraPos.x <= -12.7 && cameraPos.z <= -7.7 && cameraPos.z >= -12.3)
            cameraPos.x = -12.7;
        if(cameraPos.x <= -12.7 && cameraPos.z <= -13.7 && cameraPos.z >= -18.3)
            cameraPos.x = -12.7;
        if(cameraPos.x <= -12.7 && cameraPos.z <= -19.7 && cameraPos.z >= -24.3)
            cameraPos.x = -12.7;
        if(cameraPos.x <= -12.7 && ((cameraPos.z <= 5.7 && cameraPos.z >= 4.3) || (cameraPos.z <= -0.3 && cameraPos.z >= -1.7) ||
                                   (cameraPos.z <= -6.3 && cameraPos.z >= -7.7) || (cameraPos.z <= -12.3 && cameraPos.z >= -13.7) || (cameraPos.z <= -18.3 && cameraPos.z >= -19.7))) {
            cout << flag << endl;
            flag = 9;
            cout << "zmiana na " << flag << endl;
        }
        if(cameraPos.z <= -24.3) {
            cout << flag << endl;
            flag = 7;
            cout << "zmiana na " << flag << endl;
        }
        break;

    case 5:
        if(cameraPos.z <= 17.5) {
            cout << flag << endl;
            flag = 2;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x <= -12.7)
            cameraPos.x = -12.7;
        if(cameraPos.x >= -11.3 && cameraPos.x <= -8.5)
            cameraPos.x = -11.3;
        if(cameraPos.x <= -6.7 && cameraPos.x >= -8.5)
            cameraPos.x = -6.7;
        if(cameraPos.x >= -5.3 && cameraPos.x <= -2.5)
            cameraPos.x = -5.3;
        if(cameraPos.x <= -0.7 && cameraPos.x >= -2.5)
            cameraPos.x = -0.7;
        if(cameraPos.x >= 12.7)
            cameraPos.x = 12.7;
        if(cameraPos.x <= 11.3 && cameraPos.x >= 8.5)
            cameraPos.x = 11.3;
        if(cameraPos.x >= 6.7 && cameraPos.x <= 8.5)
            cameraPos.x = 6.7;
        if(cameraPos.x <= 5.3 && cameraPos.x >= 2.5)
            cameraPos.x = 5.3;
        if(cameraPos.x >= 0.7 && cameraPos.x <= 2.5)
            cameraPos.x = 0.7;
        if(cameraPos.z >= 22.3){
            cout << flag << endl;
            flag = 6;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 6:
        cameraPos.y = 3.0f;
        if(cameraPos.z <= 22.3 && ((cameraPos.x <= 0.7 && cameraPos.x >= -0.7) || (cameraPos.x <= 6.7 && cameraPos.x >= 5.3) || (cameraPos.x <= 12.7 && cameraPos.x >= 11) ||
                                   (cameraPos.x >= -6.7 && cameraPos.x <= -5.3) || (cameraPos.x >= -12.7 && cameraPos.x <= -11))){
            cout << flag << endl;
            flag = 5;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z <= 22.3 && cameraPos.x >= 0.7 && cameraPos.x <= 5.2)
            cameraPos.z = 22.3;
        if(cameraPos.z <= 22.3 && cameraPos.x <= -0.7 && cameraPos.x >= -5.2)
            cameraPos.z = 22.3;
        if(cameraPos.z <= 22.3 && cameraPos.x >= 6.7 && cameraPos.x <= 11)
            cameraPos.z = 22.3;
        if(cameraPos.z <= 22.3 && cameraPos.x <= -6.7 && cameraPos.x >= -11)
            cameraPos.z = 22.3;
        if(cameraPos.z <= 22.3 && cameraPos.x >= 12.7 && cameraPos.x <= 17.2)
            cameraPos.z = 22.3;
        if(cameraPos.z <= 22.3 && cameraPos.x <= -12.7 && cameraPos.x >= -17.2)
            cameraPos.z = 22.3;
        if(cameraPos.z >= 25.3 && cameraPos.x >= -20 && cameraPos.x <= -4)
            cameraPos.z = 25.3;
        if(cameraPos.z >= 25.3 && cameraPos.x <= 20 && cameraPos.x >= 4)
            cameraPos.z = 25.3;
        if(cameraPos.x >= 18)
            cameraPos.x = 18;
        if(cameraPos.x <= -18)
            cameraPos.x = -18;
        if(cameraPos.z <= 24.3 && cameraPos.x >= 17.2) {
            cout << flag << endl;
            flag = 12;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z <= 24.3 && cameraPos.x <= -17.2) {
            cout << flag << endl;
            flag = 13;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= 25.3 && cameraPos.x >= -4.5 && cameraPos.x <= 4.5) {
            cout << flag << endl;
            flag = 15;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 7:
        if(cameraPos.x <= -17.3) {
            cout << flag << endl;
            flag = 13;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x >= 17.3) {
            cout << flag << endl;
            flag = 12;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= -24.3 && cameraPos.x <= 12.7 && cameraPos.x >= 11) {
            cout << flag << endl;
            flag = 3;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= -24.3 && cameraPos.x >= -12.7 && cameraPos.x <= -11) {
            cout << flag << endl;
            flag = 4;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= -24.3 && cameraPos.x >= -17.3 && cameraPos.x <= -12.7)
            cameraPos.z = -24.3;
        if(cameraPos.z >= -24.3 && cameraPos.x >= -11.2 && cameraPos.x <= -6.7)
            cameraPos.z = -24.3;
        if(cameraPos.z >= -24.3 && cameraPos.x >= -5.3 && cameraPos.x <= -0.7)
            cameraPos.z = -24.3;
        if(cameraPos.z >= -24.3 && cameraPos.x <= 5.3 && cameraPos.x >= 0.7)
            cameraPos.z = -24.3;
        if(cameraPos.z >= -24.3 && cameraPos.x <= 11.2 && cameraPos.x >= 6.7)
            cameraPos.z = -24.3;
        if(cameraPos.z >= -24.3 && cameraPos.x <= 17.3 && cameraPos.x >= 12.7)
            cameraPos.z = -24.3;
        if(cameraPos.z <= -25) {
            cameraPos.z = -25;
        }
        if(cameraPos.z >= -24.3 && ((cameraPos.x >= -6.7 && cameraPos.x <= -5.3) || (cameraPos.x >= -0.7 && cameraPos.x <= 0.7) || (cameraPos.x >= 5.3 && cameraPos.x <= 6.7))){
            cout << flag << endl;
            flag = 16;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 8:
        if(cameraPos.x <= 12.7 && cameraPos.z <= 10.3) {
            cout << flag << endl;
            flag = 3;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x <= 12.7 && cameraPos.z >= 10.3) {
            cout << flag << endl;
            flag = 2;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= 17.7)
            cameraPos.z = 17.7;
        if(cameraPos.z <= 16.3 && cameraPos.z >= 13)
            cameraPos.z = 16.3;
        if(cameraPos.z >= 11.7 && cameraPos.z <= 13)
            cameraPos.z = 11.7;
        if(cameraPos.z <= 10.3 && cameraPos.z >= 7.5)
            cameraPos.z = 10.3;
        if(cameraPos.z >= 5.7 && cameraPos.z <= 7.5)
            cameraPos.z = 5.7;
        if(cameraPos.z <= 4.3 && cameraPos.z >= 2)
            cameraPos.z = 4.3;
        if(cameraPos.z >= -0.3 && cameraPos.z <= 2)
            cameraPos.z = -0.3;
        if(cameraPos.z <= -1.7 && cameraPos.z >= -4)
            cameraPos.z = -1.7;
        if(cameraPos.z >= -6.3 && cameraPos.z <= -4)
            cameraPos.z = -6.3;
        if(cameraPos.z <= -7.7 && cameraPos.z >= -10)
            cameraPos.z = -7.7;
        if(cameraPos.z >= -12.3 && cameraPos.z <= -10)
            cameraPos.z = -12.3;
        if(cameraPos.z <= -13.7 && cameraPos.z >= -15)
            cameraPos.z = -13.7;
        if(cameraPos.z >= -18.3 && cameraPos.z <= -15)
            cameraPos.z = -18.3;
        if(cameraPos.z <= -19.7)
            cameraPos.z = -19.7;
        if(cameraPos.x >= 17.3) {
            cout << flag << endl;
            flag = 12;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 9:
        if(cameraPos.x >= -12.7 && cameraPos.z <= 10.3) {
            cout << flag << endl;
            flag = 4;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x >= -12.7 && cameraPos.z >= 10.3) {
            cout << flag << endl;
            flag = 2;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= 17.7)
            cameraPos.z = 17.7;
        if(cameraPos.z <= 16.3 && cameraPos.z >= 13)
            cameraPos.z = 16.3;
        if(cameraPos.z >= 11.7 && cameraPos.z <= 13)
            cameraPos.z = 11.7;
        if(cameraPos.z <= 10.3 && cameraPos.z >= 7.5)
            cameraPos.z = 10.3;
        if(cameraPos.z >= 5.7 && cameraPos.z <= 7.5)
            cameraPos.z = 5.7;
        if(cameraPos.z <= 4.3 && cameraPos.z >= 2)
            cameraPos.z = 4.3;
        if(cameraPos.z >= -0.3 && cameraPos.z <= 2)
            cameraPos.z = -0.3;
        if(cameraPos.z <= -1.7 && cameraPos.z >= -4)
            cameraPos.z = -1.7;
        if(cameraPos.z >= -6.3 && cameraPos.z <= -4)
            cameraPos.z = -6.3;
        if(cameraPos.z <= -7.7 && cameraPos.z >= -10)
            cameraPos.z = -7.7;
        if(cameraPos.z >= -12.3 && cameraPos.z <= -10)
            cameraPos.z = -12.3;
        if(cameraPos.z <= -13.7 && cameraPos.z >= -15)
            cameraPos.z = -13.7;
        if(cameraPos.z >= -18.3 && cameraPos.z <= -15)
            cameraPos.z = -18.3;
        if(cameraPos.z <= -19.7)
            cameraPos.z = -19.7;
        if(cameraPos.x <= -17.3) {
            cout << flag << endl;
            flag = 13;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 12:
        if(cameraPos.x <= 17.3 && ((cameraPos.z <= 11.7 && cameraPos.z >= 10.3) || (cameraPos.z <= 5.7 && cameraPos.z >= 4.3) || (cameraPos.z <= -0.3 && cameraPos.z >= -1.7) ||
                                   (cameraPos.z <= -6.3 && cameraPos.z >= -7.7) || (cameraPos.z <= -12.3 && cameraPos.z >= -13.7) || (cameraPos.z <= -18.3 && cameraPos.z >= -19.7) ||
                                   (cameraPos.z <= 17.7 && cameraPos.z >= 16.3))) {
            cout << flag << endl;
            flag = 8;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x <= 17.3 && cameraPos.z <= 22.3 && cameraPos.z >= 17.7)
            cameraPos.x = 17.3;
        if(cameraPos.x <= 17.3 && cameraPos.z <= 16.3 && cameraPos.z >= 11.7)
            cameraPos.x = 17.3;
        if(cameraPos.x <= 17.3 && cameraPos.z <= 10.3 && cameraPos.z >= 5.7)
            cameraPos.x = 17.3;
        if(cameraPos.x <= 17.3 && cameraPos.z <= 4.3 && cameraPos.z >= -0.3)
            cameraPos.x = 17.3;
        if(cameraPos.x <= 17.3 && cameraPos.z <= -1.7 && cameraPos.z >= -6.3)
            cameraPos.x = 17.3;
        if(cameraPos.x <= 17.3 && cameraPos.z <= -7.7 && cameraPos.z >= -12.3)
            cameraPos.x = 17.3;
        if(cameraPos.x <= 17.3 && cameraPos.z <= -13.7 && cameraPos.z >= -18.3)
            cameraPos.x = 17.3;
        if(cameraPos.x <= 17.3 && cameraPos.z <= -19.7 && cameraPos.z >= -24.3)
            cameraPos.x = 17.3;
        if(cameraPos.x >= 18)
            cameraPos.x = 18;
        if(cameraPos.z <= -25)
            cameraPos.z = -25;
        if(cameraPos.x <= 17.3 && cameraPos.z <= -24.3) {
            cout << flag << endl;
            flag = 7;
            cout <<"zmiana na " << flag << endl;
        }

        if(cameraPos.z >= 24.3) {
            cout << flag << endl;
            flag = 6;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 13:
        if(cameraPos.x >= -17.3 && ((cameraPos.z <= 11.7 && cameraPos.z >= 10.3) || (cameraPos.z <= 5.7 && cameraPos.z >= 4.3) || (cameraPos.z <= -0.3 && cameraPos.z >= -1.7) ||
                                   (cameraPos.z <= -6.3 && cameraPos.z >= -7.7) || (cameraPos.z <= -12.3 && cameraPos.z >= -13.7) || (cameraPos.z <= -18.3 && cameraPos.z >= -19.7) ||
                                   (cameraPos.z <= 17.7 && cameraPos.z >= 16.3))) {
            cout << flag << endl;
            flag = 9;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x >= -17.3 && cameraPos.z <= 22.3 && cameraPos.z >= 17.7)
            cameraPos.x = -17.3;
        if(cameraPos.x >= -17.3 && cameraPos.z <= 16.3 && cameraPos.z >= 11.7)
            cameraPos.x = -17.3;
        if(cameraPos.x >= -17.3 && cameraPos.z <= 10.3 && cameraPos.z >= 5.7)
            cameraPos.x = -17.3;
        if(cameraPos.x >= -17.3 && cameraPos.z <= 4.3 && cameraPos.z >= -0.3)
            cameraPos.x = -17.3;
        if(cameraPos.x >= -17.3 && cameraPos.z <= -1.7 && cameraPos.z >= -6.3)
            cameraPos.x = -17.3;
        if(cameraPos.x >= -17.3 && cameraPos.z <= -7.7 && cameraPos.z >= -12.3)
            cameraPos.x = -17.3;
        if(cameraPos.x >= -17.3 && cameraPos.z <= -13.7 && cameraPos.z >= -18.3)
            cameraPos.x = -17.3;
        if(cameraPos.x >= -17.3 && cameraPos.z <= -19.7 && cameraPos.z >= -24.3)
            cameraPos.x = -17.3;
        if(cameraPos.x <= -18)
            cameraPos.x = -18;
        if(cameraPos.z <= -25)
            cameraPos.z = -25;
        if(cameraPos.x >= -17.3 && cameraPos.z <= -24.3) {
            cout << flag << endl;
            flag = 7;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= 24.3) {
            cout << flag << endl;
            flag = 6;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 14:
        if(cameraPos.z <= -19.7 && ((cameraPos.x >= -6.7 && cameraPos.x <= -5.3) || (cameraPos.x >= -0.7 && cameraPos.x <= 0.7) || (cameraPos.x >= 5.3 && cameraPos.x <= 6.7))) {
            cout << flag << endl;
            flag = 16;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= -19.2)
            cameraPos.z = -19.2;
        if(cameraPos.x <= -6.7)
            cameraPos.x = -6.7;
        if(cameraPos.x >= 6.7)
            cameraPos.x = 6.7;
        if(cameraPos.z <= -19.7 && cameraPos.x <= 5.3 && cameraPos.x >= 0.7)
            cameraPos.z = -19.7;
        if(cameraPos.z <= -19.7 && cameraPos.x >= -5.3 && cameraPos.x <= -0.7)
            cameraPos.z = -19.7;
        break;

    case 16:
        if(cameraPos.z <= -24.3 && ((cameraPos.x >= -6.7 && cameraPos.x <= -5.3) || (cameraPos.x >= -0.7 && cameraPos.x <= 0.7) || (cameraPos.x >= 5.3 && cameraPos.x <= 6.7))) {
            cout << flag << endl;
            flag = 7;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x <= -6.7)
            cameraPos.x = -6.7;
        if(cameraPos.x >= 6.7)
            cameraPos.x = 6.7;
        if(cameraPos.x >= 0.7 && cameraPos.x <= 3.5)
            cameraPos.x = 0.7;
        if(cameraPos.x >= -5.3 && cameraPos.x <= -3.5)
            cameraPos.x = -5.3;
        if(cameraPos.x <= -0.7 && cameraPos.x >= -3.5)
            cameraPos.x = -0.7;
        if(cameraPos.x <= 5.3 && cameraPos.x >= 3.5)
            cameraPos.x = 5.3;
        if(cameraPos.z >= -19.7) {
            cout << flag << endl;
            flag = 14;
            cout <<"zmiana na " << flag << endl;
        }
        break;


    case 15: // outside
        cameraPos.y = 1.0f;

        if(cameraPos.z <= 25.3 && cameraPos.x >= -4.5 && cameraPos.x <= 4.5 && cameraPos.z >= 24.0) {
            cout << flag << endl;
            flag = 6;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= 77.0)
            cameraPos.z = 77.0;
        if(cameraPos.z <= -77.0)
            cameraPos.z = -77.0;
        if(cameraPos.x >= 77.0)
            cameraPos.x = 77.0;
        if(cameraPos.x <= -77.0)
            cameraPos.x = -77.0;
        if(cameraPos.x <= -4.5 && cameraPos.z >= 25.3 && cameraPos.z <= 28 && cameraPos.x >= -5)
            cameraPos.x = -4.5;
        if(cameraPos.x >= 4.5 && cameraPos.z >= 25.3 && cameraPos.z <= 28 && cameraPos.x <= 5)
            cameraPos.x = 4.5;
        if(cameraPos.x >= -21 && cameraPos.x <= -20.5 && cameraPos.z >= -28 && cameraPos.z <= 28)
            cameraPos.x = -21;
        if(cameraPos.x <= 21 && cameraPos.x >= 20.5 && cameraPos.z >= -28 && cameraPos.z <= 28)
            cameraPos.x = 21;
        if(cameraPos.x >= -21 && cameraPos.x <= -4.5 && cameraPos.z <= 28 && cameraPos.z >= 27)
            cameraPos.z = 28;
        if(cameraPos.x <= 21 && cameraPos.x >= 4.5 && cameraPos.z <= 28 && cameraPos.z >= 27)
            cameraPos.z = 28;
        if(cameraPos.x >= -21 && cameraPos.x <= 21 && cameraPos.z >= -28 && cameraPos.z <= -27)
            cameraPos.z = -28;
        break;
    case 17:
        if(cameraPos.z >= -0.5) {
            cout << flag << endl;
            flag = 0;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x <= 5.5)
            cameraPos.x = 5.5;
        if(cameraPos.x >= 7)
            cameraPos.x = 7;
        if(cameraPos.z <= -8) {
            cout << flag << endl;
            flag = 19;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 18:
        if(cameraPos.z >= -0.5) {
            cout << flag << endl;
            flag = 0;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.x >= -5.5)
            cameraPos.x = -5.5;
        if(cameraPos.x <= -7)
            cameraPos.x = -7;
        if(cameraPos.z <= -8) {
            cout << flag << endl;
            flag = 19;
            cout <<"zmiana na " << flag << endl;
        }
        break;

    case 19:
        if(cameraPos.z >= -8 && cameraPos.x <= -5.5) {
            cout << flag << endl;
            flag = 18;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= -8 && cameraPos.x >= 5.5) {
            cout << flag << endl;
            flag = 17;
            cout <<"zmiana na " << flag << endl;
        }
        if(cameraPos.z >= -8 && cameraPos.x <= 5.5 && cameraPos.x >= -5.5)
            cameraPos.z = -8;
        if(cameraPos.x >= 7)
            cameraPos.x = 7;
        if(cameraPos.x <= -7)
            cameraPos.x = -7;
        if(cameraPos.z <= -16.3)
            cameraPos.z = -16.3;
        break;
    }


}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw1   += xoffset;
    pitch1 += yoffset;

    if(pitch1 > 89.0f)
        pitch1 = 89.0f;
    if(pitch1 < -89.0f)
        pitch1 = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw1)) * cos(glm::radians(pitch1));
    front.y = sin(glm::radians(pitch1));
    front.z = sin(glm::radians(yaw1)) * cos(glm::radians(pitch1));
    cameraFront = glm::normalize(front);
}
//Procedura obsługi klawiatury
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    cameraSpeed = 0.4f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        //cameraPos.y = 4.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        //cameraPos.y = 4.0f;
    }

}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
    glfwSetFramebufferSizeCallback(window, windowResize); //Zarejestruj procedurę obsługi zmiany rozdzielczości bufora ramki
    glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurę obsługi klawiatury
    glfwSetCursorPosCallback(window, mouse_callback);

	loadOBJ("./objects/floor_texture.obj", obj_floor.vertices, obj_floor.uvs, obj_floor.normals);
	loadOBJ("./objects/column_texture.obj", obj_column.vertices, obj_column.uvs, obj_column.normals);
	loadOBJ("./objects/roof_texture.obj", obj_roof.vertices, obj_roof.uvs, obj_roof.normals);
	loadOBJ("./objects/ground_texture.obj", obj_ground.vertices, obj_ground.uvs, obj_ground.normals);
	loadOBJ("./objects/background.obj", obj_background.vertices, obj_background.uvs, obj_background.normals);
	loadOBJ("./objects/statue.obj", obj_statue.vertices, obj_statue.uvs, obj_statue.normals);

    std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
    unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
    unsigned error = lodepng::decode(image, width, height, "./textures/marble_texture.png");

	glClearColor(0.34,0.8,1,1); //Ustaw kolor czyszczenia ekranu

	glGenBuffers(obj_count, vertexbuffer);
    glGenBuffers(obj_count, normalsbuffer);
	glGenBuffers(obj_count, uvbuffer);

    glGenTextures(3,texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, obj_floor.vertices.size() * sizeof(glm::vec3), &obj_floor.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, obj_floor.normals.size() * sizeof(glm::vec3), &obj_floor.normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, obj_floor.uvs.size() * sizeof(glm::vec2), &obj_floor.uvs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, obj_column.vertices.size() * sizeof(glm::vec3), &obj_column.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, obj_column.normals.size() * sizeof(glm::vec3), &obj_column.normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, obj_column.uvs.size() * sizeof(glm::vec2), &obj_column.uvs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, obj_roof.vertices.size() * sizeof(glm::vec3), &obj_roof.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, obj_roof.normals.size() * sizeof(glm::vec3), &obj_roof.normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, obj_roof.uvs.size() * sizeof(glm::vec2), &obj_roof.uvs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[5]);
	glBufferData(GL_ARRAY_BUFFER, obj_statue.vertices.size() * sizeof(glm::vec3), &obj_statue.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[5]);
	glBufferData(GL_ARRAY_BUFFER, obj_statue.normals.size() * sizeof(glm::vec3), &obj_statue.normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[5]);
	glBufferData(GL_ARRAY_BUFFER, obj_statue.uvs.size() * sizeof(glm::vec2), &obj_statue.uvs[0], GL_STATIC_DRAW);

    std::vector<unsigned char> image2;   //Alokuj wektor do wczytania obrazka
    unsigned width2, height2;   //Zmienne do których wczytamy wymiary obrazka
    unsigned error2 = lodepng::decode(image2, width2, height2, "./textures/ground_texture2.png");

    glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image2.data());

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[3]);
	glBufferData(GL_ARRAY_BUFFER, obj_ground.vertices.size() * sizeof(glm::vec3), &obj_ground.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[3]);
	glBufferData(GL_ARRAY_BUFFER, obj_ground.normals.size() * sizeof(glm::vec3), &obj_ground.normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[3]);
	glBufferData(GL_ARRAY_BUFFER, obj_ground.uvs.size() * sizeof(glm::vec2), &obj_ground.uvs[0], GL_STATIC_DRAW);

    std::vector<unsigned char> image4;   //Alokuj wektor do wczytania obrazka
    unsigned width4, height4;   //Zmienne do których wczytamy wymiary obrazka
    unsigned error4 = lodepng::decode(image4, width4, height4, "./textures/background.png");
    glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width4, height4, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image4.data());

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[4]);
	glBufferData(GL_ARRAY_BUFFER, obj_background.vertices.size() * sizeof(glm::vec3), &obj_background.vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[4]);
	glBufferData(GL_ARRAY_BUFFER, obj_background.normals.size() * sizeof(glm::vec3), &obj_background.normals[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[4]);
	glBufferData(GL_ARRAY_BUFFER, obj_background.uvs.size() * sizeof(glm::vec2), &obj_background.uvs[0], GL_STATIC_DRAW);

	glEnable(GL_RESCALE_NORMAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgba_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgba_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, rgba_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, rgba_emission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
    mat4 P=perspective(65.0f*PI/180.0f,aspect,1.0f,1000.0f); //Wylicz macierz rzutowania P

    cameraPos.y = 3.0f; //postac na 1 poziomie

    collisionDetector();

    mat4 V=lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    mat4 M=mat4(1);
    glMatrixMode(GL_PROJECTION); //Włącz tryb modyfikacji macierzy rzutowania
    glLoadMatrixf(value_ptr(P)); //Załaduj macierz rzutowania
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Wyczyść bufor kolorów (czyli przygotuj "płótno" do rysowania)

    glMatrixMode(GL_MODELVIEW);
    M=rotate(M,angle_x,vec3(1.0f,0.0f,0.0f));
    M=rotate(M,angle_y,vec3(0.0f,1.0f,0.0f));
    glLoadMatrixf(value_ptr(V*M));

    glLightfv(GL_LIGHT0,GL_AMBIENT,sun_light_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,sun_light_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);
    glLightfv(GL_LIGHT0,GL_POSITION,sun_light_pos);

    glLightfv(GL_LIGHT1,GL_AMBIENT,temple_light_ambient);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,temple_light_diffuse);
    glLightfv(GL_LIGHT1,GL_SPECULAR,light_specular);
    glLightfv(GL_LIGHT1,GL_POSITION,temple_light_pos);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
    glVertexPointer(3,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[0]);
    glNormalPointer(GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
    glTexCoordPointer(2,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES,0,obj_floor.vertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
    glVertexPointer(3,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[2]);
    glNormalPointer(GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
    glTexCoordPointer(2,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES,0,obj_roof.vertices.size());

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgba_statue_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgba_statue_diffuse);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[5]);
    glVertexPointer(3,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[5]);
    glNormalPointer(GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[5]);
    glTexCoordPointer(2,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES,0,obj_statue.vertices.size());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgba_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgba_diffuse);

    glBindTexture(GL_TEXTURE_2D,texture[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[3]);
    glVertexPointer(3,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[3]);
    glNormalPointer(GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[3]);
    glTexCoordPointer(2,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES,0,obj_ground.vertices.size());

    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, rgba_sun_emission);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[4]);
    glVertexPointer(3,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[4]);
    glNormalPointer(GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[4]);
    glTexCoordPointer(2,GL_FLOAT,0,NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES,0,obj_background.vertices.size());
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, rgba_emission);

    glBindTexture(GL_TEXTURE_2D,texture[0]);
    M=translate(M,vec3(15,0,26));
    for(unsigned int i=0;i<8;i++){
        M=translate(M,vec3(0,0,-6));
        glLoadMatrixf(value_ptr(V*M));
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexPointer(3,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
        glNormalPointer(GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES,0,obj_column.vertices.size());
        M=translate(M,vec3(-30,0,0));
        glLoadMatrixf(value_ptr(V*M));
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexPointer(3,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
        glNormalPointer(GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES,0,obj_column.vertices.size());
        M=translate(M,vec3(30,0,0));
    }
    for(unsigned int i=0;i<4;i++){
        M=translate(M,vec3(-6,0,0));
        glLoadMatrixf(value_ptr(V*M));
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexPointer(3,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
        glNormalPointer(GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES,0,obj_column.vertices.size());
        M=translate(M,vec3(0,0,42));
        glLoadMatrixf(value_ptr(V*M));
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexPointer(3,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
        glNormalPointer(GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES,0,obj_column.vertices.size());
        M=translate(M,vec3(0,0,-42));
    }
    M=translate(M, vec3(18,0,36));
    for(unsigned int i=0;i<5;i++){
        M=translate(M,vec3(0,0,-6.4));
        glLoadMatrixf(value_ptr(V*M));
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexPointer(3,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
        glNormalPointer(GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES,0,obj_column.vertices.size());
        M=translate(M,vec3(-18,0,0));
        glLoadMatrixf(value_ptr(V*M));
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexPointer(3,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
        glNormalPointer(GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES,0,obj_column.vertices.size());
        M=translate(M,vec3(18,0,0));
    }
    M=translate(M,vec3(0,0,27.5));
    for(unsigned int i=0;i<2;i++){
        M=translate(M,vec3(-6,0,0));
        glLoadMatrixf(value_ptr(V*M));
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
        glVertexPointer(3,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer[1]);
        glNormalPointer(GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
        glTexCoordPointer(2,GL_FLOAT,0,NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES,0,obj_column.vertices.size());
    }

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno
	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów
	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(1400, 700, "OpenGL", NULL, NULL);
	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora
	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	initOpenGLProgram(window); //Operacje inicjujące
	float angle_x=0.0f; //Aktualny kąt obrotu obiektu wokół osi x
	float angle_y=0.0f; //Aktualny kąt obrotu obiektu wokół osi y
	glfwSetTime(0); //Wyzeruj timer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
	    float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeed = 0.1f * deltaTime;
	    glfwSetTime(0); //Wyzeruj timer
		drawScene(window,angle_x,angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}
	glDeleteBuffers(obj_count, vertexbuffer);
	glDeleteBuffers(obj_count, normalsbuffer);
	glDeleteTextures(3,texture);
	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
