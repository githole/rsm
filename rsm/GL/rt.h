#ifndef _RT_H_
#define _RT_H_

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <string>
#include <set>
#include <iostream>

class RenderTarget {
private:
	GLuint _frameBuffer;
	GLuint _renderBuffer;
	GLuint _texture;

	int _size;
public:
	virtual ~RenderTarget() {
		glDeleteTextures(1, &_texture);
		glDeleteRenderbuffers(1, &_renderBuffer);
		glDeleteFramebuffers(1, &_frameBuffer);
	}

	const int Size() {
		return _size;
	}

	void Bind() {
		glViewport(0, 0, _size, _size);
		glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
	}

	void Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLuint Texture() {
		return _texture;
	}

	RenderTarget(const int size, GLuint wrap = GL_CLAMP_TO_EDGE, GLuint filterParam = GL_LINEAR, GLuint internalFormat = GL_RGBA32F, GLuint type = GL_FLOAT) :
		_size(size) {
		glGenTextures(1, &_texture);
		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _size, _size, 0, GL_RGBA, type, NULL);
		static const GLfloat border[] = {1, 1, 1, 1};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);

		glGenFramebuffers(1, &_frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

		glGenRenderbuffers(1, &_renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size, _size);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _renderBuffer);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};


#endif