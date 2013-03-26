#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>


void DrawTexture(const float x, const float y, const float w, const float h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_TEXTURE_2D);
	
	glColor4f(1, 1, 1, 1);
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(x+w, y+h, 0);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(x-w, y+h, 0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(x-w, y-h, 0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(x+w, y-h, 0);
	glEnd();
}

void DrawPlane(const GLfloat size) {
	static GLfloat vertices[4][3] = {
		{-size, 0.0f,  size},
		{ size, 0.0f,  size},
		{ size, 0.0f, -size},
		{-size, 0.0f, -size},
	};
		
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[3]);
	glEnd();
}

void DrawCube(const GLfloat xsize, const GLfloat ysize, const GLfloat zsize) {
	const GLfloat x = xsize;
	const GLfloat y = ysize;
	const GLfloat z = zsize;
	GLfloat vertices[8][3] = {
		{ x, y, z},
		{ x, y,-z},
		{-x, y,-z},
		{-x, y, z},
		{ x,-y, z},
		{ x,-y,-z},
		{-x,-y,-z},
		{-x,-y, z},
	};

	// +Y
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[3]);
	glEnd();

	// -Y
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0,-1.0, 0.0);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[5]);
	glEnd();

	// +X
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(1.0,0.0, 0.0);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[4]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[1]);
	glEnd();
	
	// -X
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(-1.0,0.0, 0.0);
		glVertex3fv(vertices[2]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[3]);
	glEnd();
	
	// +Z
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0,0.0,1.0);
		glVertex3fv(vertices[0]);
		glVertex3fv(vertices[3]);
		glVertex3fv(vertices[7]);
		glVertex3fv(vertices[4]);
	glEnd();

	// -Z
	glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0,0.0,-1.0);
		glVertex3fv(vertices[1]);
		glVertex3fv(vertices[5]);
		glVertex3fv(vertices[6]);
		glVertex3fv(vertices[2]);
	glEnd();

}

#endif