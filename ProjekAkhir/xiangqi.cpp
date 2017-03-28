#include<GL\glut.h>
#include<iostream>
#define _USE_MATH_DEFINES 
#include <math.h>
#include <sapi.h>
#include <map>
#include <vector>
#include <IL\config.h>
#include <IL\devil_cpp_wrapper.hpp>
#include <IL\devil_internal_exports.h>
#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilu_region.h>
#include <IL\ilut.h>
#include <IL\ilut_config.h>

/* Global variables */
char title[] = "XiangQi";   
int refreshMills = 1,rotate = 0, coorx = 0, coory = 4,fontindex = 0, lastfps = 0, frames = 0,camerax = 0, cameray = 5, temp = 1;
ISpVoice * pVoice;
bool open = false;
float chipSize = 0.17;
FILE *File;
HRESULT hr;
struct chessPosition {
	float x;
	float y;
	float z;
};
std::vector<float> mappos;
std::vector<GLuint> texture;
std::map<float, chessPosition*> position;
int location = 0;
int chessMap[10][9] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,28,-1,-1,-1,-1,-1,29,-1},
	{23,-1,24,-1,25,-1,26,-1,27},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{18,-1,19,-1,20,-1,21,-1,22},
	{-1,30,-1,-1,-1,-1,-1,31,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{ 9,10,11,12,13,14,15,16,17}
};

/* Initialize OpenGL Graphics */
void speak(wchar_t const * text);
void generateBoard();
void DrawCircle(float cx, float cy, float r, int num_segments, float depth);
void drawChip(int total,float chesspositionx, float chesspositiony);
bool enter = false;
GLfloat positionlight[] = { 0.0, 5.0, 1.5, 1.0 };

void initTexture() {
	texture.push_back(ilutGLLoadImage("wood.jpg"));
	texture.push_back(ilutGLLoadImage("Board.png"));
	texture.push_back(ilutGLLoadImage("piece/blackking.bmp"));
	texture.push_back(ilutGLLoadImage("piece/blackbishop.bmp"));
	texture.push_back(ilutGLLoadImage("piece/blackcart.bmp"));
	texture.push_back(ilutGLLoadImage("piece/blackhorse.bmp"));
	texture.push_back(ilutGLLoadImage("piece/blackscholar.bmp"));
	texture.push_back(ilutGLLoadImage("piece/redking.png"));
	texture.push_back(ilutGLLoadImage("piece/redbishop.bmp"));
	texture.push_back(ilutGLLoadImage("piece/redcart.bmp"));
	texture.push_back(ilutGLLoadImage("piece/redhorse.bmp"));
	texture.push_back(ilutGLLoadImage("piece/redscholar.bmp"));
	texture.push_back(ilutGLLoadImage("piece/redprivate.bmp"));
	texture.push_back(ilutGLLoadImage("piece/blackprivate.bmp"));
	texture.push_back(ilutGLLoadImage("piece/blackcannon.bmp"));
	texture.push_back(ilutGLLoadImage("piece/redcannon.bmp"));
}

void speak(wchar_t const * text) {
	if (FAILED(::CoInitialize(NULL)))
		std::cout << "Failed to Initialize voice" << std::endl;
	hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	if (SUCCEEDED(hr)){
		pVoice->Speak(text, 0, NULL);
		pVoice->Release();
	}
	::CoUninitialize();
}

void initGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	ilutRenderer(ILUT_OPENGL);
	ilutEnable(ILUT_OPENGL_CONV);
	initTexture();
}

void display() {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(camerax, cameray, 1, coorx, coory, 0, 0, 1, 0);
	glTranslatef(0.0, 2.0, -5.0);
	glRotated(-70, 1.0, 0.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, positionlight);
	glTranslated(0.0, 0.0, 1.5);
	glDisable(GL_LIGHTING);
	glColor3f(0.0, 1.0, 1.0);
	glutWireCube(0.1);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(camerax, cameray,1, coorx, coory, 0, 0, 1, 0);
	glTranslatef(0.0, 0.0, -5.0);
	glColor3f(0.9, 0.3, 0.2);
	glRotatef(90, 0, 0, 1);
	glScalef(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	generateBoard();
	glDisable(GL_TEXTURE_2D);
	glTranslatef(0.0, 0.0, 5.0);
	glPopMatrix();
	glPushMatrix();
	gluLookAt(camerax, cameray, 1, coorx, coory, 0, 0, 1, 0);
	glTranslatef(0.0, 0.001, -5.0);
	glColor3f(0.9, 0.3, 0.2);
	glRotatef(90, 0, 0, 1);
	glScalef(1.0, -1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -2.0f, -2.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 2.0f, -2.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 2.0f, 2.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -2.0f, 2.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glTranslatef(0.0, 0.0, 5.0);
	glPopMatrix();
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(camerax, cameray, 1, coorx, coory, 0, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(1.0, -1.0, 1.0);
	glTranslatef(-4.5, 3.1, -1.2);
	glColor3f(1.0f, 1.0f, 1.0f);
	drawChip(9,5, 0);
	temp = 1;
	for (float k = 0;k <= 0;k += 1)
		for (float j = 0;j <= 5;j += 0.42) {
			for (float i = 0;i < chipSize;i += 0.01) {
				glEnable(GL_TEXTURE_2D);
				if (temp == 5)
					glBindTexture(GL_TEXTURE_2D, texture[7]);
				else if (temp == 3 || temp == 7)
					glBindTexture(GL_TEXTURE_2D, texture[8]);
				else if (temp == 1 || temp == 9)
					glBindTexture(GL_TEXTURE_2D, texture[9]);
				else if (temp == 2 || temp == 8)
					glBindTexture(GL_TEXTURE_2D, texture[10]);
				else if (temp == 4 || temp == 6)
					glBindTexture(GL_TEXTURE_2D, texture[11]);
				DrawCircle(2.8 + j, 0.08 + k, i, 50, 0.1);
				glDisable(GL_TEXTURE_2D);
			}
			if (temp != 9) temp++; else break;
		}
	temp = 1;				
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[12]);
	for (float k = 0;k <= 0;k += 1)
		for (float j = 0;j <= 5;j += 0.85) {
			for (float i = 0;i <chipSize;i += 0.01) {
				DrawCircle(2.8 + j, 1.3 + k, i, 50, 0.1);
			}
			if (temp != 5) temp++; else break;
		}
	glDisable(GL_TEXTURE_2D);
	temp = 1;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[13]);
	for (float k = 0;k <= 0;k += 1)
		for (float j = 0;j <= 5;j += 0.85) {
			for (float i = 0;i < chipSize;i += 0.01) {
				DrawCircle(2.8 + j, 2.5 + k, i, 50, 0.1);
			}
			if (temp != 5) temp++; else break;
		}
	glDisable(GL_TEXTURE_2D);
	temp = 1;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[14]);
	for (float k = 0;k <= 0;k += 1)
		for (float j = 0;j <= 5;j += 2.53) {
			for (float i = 0;i < chipSize;i += 0.01) {
				DrawCircle(3.2 + j, 2.85 + k, i, 50, 0.1);
			}
			if (temp != 2) temp++; else break;
		}
	glDisable(GL_TEXTURE_2D);
	temp = 1;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[15]);
	for (float k = 0;k <= 0;k += 1)
		for (float j = 0;j <= 5;j += 2.53) {
			for (float i = 0;i < chipSize;i += 0.01) {
				DrawCircle(3.2 + j, 0.95 + k, i, 50, 0.1);
			}
			if (temp != 2) temp++; else	break;
		}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	ilClearImage();
	glFlush();
	glutSwapBuffers();
	if (open == false) {
		speak(L"This is a guide speaker");
		speak(L"Welcome to Chinese Chess Board!");
		open = true;
	}
}

void drawChip(int total,float chesspositionx,float chesspositiony) {
	int temp = 1;
	for (float k = 0;k <= chesspositiony;k += 1)
		for (float j = 0;j <= chesspositionx;j += 0.42) {
			for (float i = 0;i < chipSize;i += 0.01) {
				glEnable(GL_TEXTURE_2D);
				if (temp == 5)
					glBindTexture(GL_TEXTURE_2D, texture[2]);
				else if (temp == 3 || temp == 7)
					glBindTexture(GL_TEXTURE_2D, texture[3]);
				else if (temp == 1 || temp == 9)
					glBindTexture(GL_TEXTURE_2D, texture[4]);
				else if (temp == 2 || temp == 8)
					glBindTexture(GL_TEXTURE_2D, texture[5]);
				else if (temp == 4 || temp == 6)
					glBindTexture(GL_TEXTURE_2D, texture[6]);
				DrawCircle(2.8 + j, 3.58 + k, i, 50, 0.1);
				glDisable(GL_TEXTURE_2D);
			}
			if (temp != total) temp++; else break;
		}
}

void DrawCircle(float cx, float cy, float r, int num_segments,float depth)
{
	float theta, x, y;
	if (position.find(cx+cy) == position.end()) {
		position[cx + cy] = new chessPosition();
		position[cx + cy]->x = 0;
		position[cx + cy]->y = 0;
		position[cx + cy]->z = 0;
		mappos.push_back(cx + cy);
	}
	else {
		glTranslatef(position[cx + cy]->x, position[cx + cy]->y, position[cx + cy]->z);
	}
	for (float j = 0;j < depth;j += 0.01) {
		glBegin(GL_TRIANGLE_FAN);
		for (float ii = 0; ii < num_segments; ii += 1)
		{
			theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle
			x = r * cosf(theta);//calculate the x component
			y = r * sinf(theta);//calculate the y component
			if (j == 0) glTexCoord3f((x / r + 1)*0.5, (y / r + 1)*0.5, j);
			glVertex3f(x + cx, y + cy, j);//output 
		}
		glEnd();
	}
	if (position.find(cx + cy) != position.end()) {
		glTranslatef(-1*position[cx + cy]->x, -1*position[cx + cy]->y, -1*position[cx + cy]->z);
	}
}
void generateBoard() {
	glBegin(GL_QUADS);
	// Front Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -1.0f, -1.0f);
	// Top Face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	// Bottom Face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -2.0f, -2.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 2.0f, -2.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 2.0f, 2.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -2.0f, 2.0f);
	// Left Face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
}

void timer(int value) {
	glutPostRedisplay();      // Post re-paint request to activate display()
	glutTimerFunc(refreshMills, timer, 0); // next timer call milliseconds later
}

void reshape(GLsizei width, GLsizei height) {  
	if (height == 0) height = 1;                // To prevent divide by 0
	glViewport(0, 0, width, height);
	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset 
	gluPerspective(45.0f, (GLfloat)((GLfloat)width / (GLfloat)height), 0.1f, 3000.0f); //Enable perspective projection with fovy, aspect, zNear and zFar
}


void handleKeypress(unsigned char key, int, int) {
	if (enter == false) {
		if (key == 'n') {
			enter = true;
			position[mappos.at(location)]->z = 0;
		}
		if (key == '6') {
			if (mappos.at(location) != mappos.back()) {
				position[mappos.at(location + 1)]->z = -0.42;
				position[mappos.at(location)]->z = 0;
				location++;
			}
		}
		if (key == '4') {
			if (mappos.at(location) != mappos.front()) {
				position[mappos.at(location - 1)]->z = -0.42;
				position[mappos.at(location)]->z = 0;
				location--;
			}
		}
	}
	else if (enter == true) {
					if (key == '6')
						position[mappos.at(location)]->x += 0.42;
					else if (key == '4')
						position[mappos.at(location)]->x -= 0.42;
					else if (key == '8')
						position[mappos.at(location)]->y += 0.42;
					else if (key == '2')
						position[mappos.at(location)]->y -= 0.42;
		if (key == 'n' ) {
			enter = false;
			for(int i=0;i<10;i++)
				for (int j = 0;j < 9;j++) {
					if (chessMap[i][j] == location) {
						int x = position[mappos.at(location)]->x / (float)(0.42);
						int y = position[mappos.at(location)]->y / (float)(0.42);
						if (chessMap[i -y][j + x] != -1) {
							position[mappos.at(chessMap[i - y][j + x])]->x = 100;
							position[mappos.at(chessMap[i - y][j + x])]->y = 100;
							chessMap[i - y][j + x] = -1;
						}
						break;
					}
				}
			position[mappos.at(location)]->z = -0.42;
		}
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);           
	glutInitDisplayMode(GLUT_DOUBLE); 
	glutInitWindowSize(840, 680);   
	glutInitWindowPosition(50, 50); 
	glutCreateWindow(title);          
	glutDisplayFunc(display);       
	glutReshapeFunc(reshape);       
	glutKeyboardFunc(handleKeypress);
	initGL();   
	glutTimerFunc(0, timer, 0);     
	glutMainLoop();                 
	return EXIT_SUCCESS;
}

