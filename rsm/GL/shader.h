#ifndef _SHADER_H_
#define _SHADER_H_

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <string>
#include <set>
#include <iostream>


class Shader {
private:
	bool OK;
	GLuint shaderProgram;

	std::set<int> errorLinesFS;
	std::set<int> errorLinesVS;
public:
	Shader();
	virtual ~Shader();

	const std::set<int>& GetErrorLinesFS() { return errorLinesFS; }
	const std::set<int>& GetErrorLinesVS() { return errorLinesVS; }

	GLuint Compile(const std::string& fsshader, const std::string& vsshader);

	GLuint CompileFromFile(const std::string& fsfilename, const std::string& vsfilename);

	bool Valid();
	void Bind();
	void Unbind();
	void SetUniform(const GLchar* name, int i);
	void SetUniform(const GLchar* name, float v);
	void SetUniform(const GLchar* name, float* fv, int size);
	void SetUniform(const GLchar* name, float x, float y);
	void SetUniform(const GLchar* name, float x, float y, float z, float w);
	void SetUniformMatrix4x4(const GLchar* name, float *fv);
};


#endif
