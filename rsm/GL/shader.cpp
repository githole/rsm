#include "shader.h"


enum ShaderGLType {
	GLSL_VS,
	GLSL_FS
};
	
void getErrorLog(GLuint shader, std::set<int>* errorLines = NULL)
{
	GLsizei bufSize = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1) {
		GLchar *infoLog;
		GLsizei length;

		infoLog = new GLchar[bufSize];

		glGetShaderInfoLog(shader, bufSize, &length, infoLog);
//		Logger::Instance()->OutputString("Compile Status: " + std::string(infoLog));
		std::cerr << "Compile Status: " + std::string(infoLog) << std::endl;

		// 適当な解析
		if (errorLines != NULL) {
			std::string tmpStr;
			for (int i = 0; i <  bufSize; i ++) {
				if (infoLog[i] == '\n') {
					int num = 0;
					int numcnt = 0;
					bool inNum = false;
					for (unsigned int j = 0; j < tmpStr.length(); j ++) {
						if (inNum) {
							if (isdigit(tmpStr[j])) {
								num = num * 10 + (tmpStr[j] - '0');
							} else {
								inNum = false;
								numcnt ++;

								// 二番目の数字
								if (numcnt == 2) {
									errorLines->insert(num);
								}
							}
						} else {
							if (isdigit(tmpStr[j])) {
								inNum = true;
								num = num * 10 + (tmpStr[j] - '0');
							}
						}
					}
					tmpStr = "";
				} else {
					tmpStr += infoLog[i];
				}
			}
		}

		delete[] infoLog;
	}
}

GLuint CompileShader(ShaderGLType type, const GLchar* source, std::set<int>* errorLines = NULL)
{
	GLint status;
	unsigned int prog = 0;
	switch (type) {
	case GLSL_VS:
		{
 			prog = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(prog, 1, &source, 0);
			glCompileShader(prog);
			glGetShaderiv(prog, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE) {
				getErrorLog(prog, errorLines);
				std::cerr << "Compile error in vertex shader." << std::endl;
				glDeleteShader(prog);
				prog = 0;
			}

			return prog;
		}break;
	case GLSL_FS:
		{
			prog = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(prog, 1, &source, 0);
			glCompileShader(prog);
			glGetShaderiv(prog, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE) {
				getErrorLog(prog, errorLines);
				std::cerr << "Compile error in fragment shader." << std::endl;
				glDeleteShader(prog);
				prog = 0;
			}

			return prog;
		}break;
	}
	return 0;
}


GLuint LinkShader(GLuint vsh, GLuint fsh)
{
	GLuint program = 0;
	if (vsh != 0 && fsh != 0) {
		program = glCreateProgram();
		glAttachShader(program, vsh);
		glAttachShader(program, fsh);
		// リンク
		glLinkProgram(program);
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			std::cerr << "Link Error." << std::endl;
			glDeleteProgram(program);
			program = 0;
		}
	}

	return program;
}

Shader::Shader() {
	OK = false;
	shaderProgram = 0;
}

GLuint Shader::CompileFromFile(const std::string& fsfilename, const std::string& vsfilename) {
	std::string fsshader, vsshader;
	FILE* fp = fopen(fsfilename.c_str(), "rt");
	if (fp != NULL) {
		char buf[1024];
		while (fgets(buf, 1024, fp) != NULL) {
			fsshader += buf;
		}
		fclose(fp);
	}
	fp = fopen(vsfilename.c_str(), "rt");
	if (fp != NULL) {
		char buf[1024];
		while (fgets(buf, 1024, fp) != NULL) {
			vsshader += buf;
		}
		fclose(fp);
	}

	return Compile(fsshader, vsshader);
}

GLuint Shader::Compile(const std::string& fsshader, const std::string& vsshader) {
	errorLinesVS.clear();
	GLuint vsh = CompileShader(GLSL_VS, vsshader.c_str(), &errorLinesVS); 
	if (vsh == 0) {
		std::cerr << "Vertex Shader Error." << std::endl;
		return 0;
	}

	errorLinesFS.clear();
	GLuint fsh = CompileShader(GLSL_FS, fsshader.c_str(), &errorLinesFS);
	if (fsh == 0) {
		std::cerr << "Fragment Shader Error." << std::endl;
		glDeleteShader(vsh);
		return 0;
	}

	GLuint program = LinkShader(vsh, fsh);
	if (program != 0) {
		OK = true;
		if (shaderProgram != 0)
			glDeleteProgram(shaderProgram);
		shaderProgram = program;

		glDeleteShader(vsh);
		glDeleteShader(fsh);
	} else
		return 0;

	// 新しくシェーダープログラムセットされたらここにくる
	return shaderProgram;
}

Shader::~Shader() {
	if (shaderProgram != 0) {
		glDeleteProgram(shaderProgram);
	}
}

bool Shader::Valid() {
	return OK;
}

void Shader::Bind() {
	glUseProgram(shaderProgram);
}

void Shader::Unbind() {
	glUseProgram(0);
}

void Shader::SetUniform(const GLchar* name, int i) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform1i(id, i);
	}
}

void Shader:: SetUniform(const GLchar* name, float v) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform1f(id, v);
	}
}

void Shader:: SetUniform(const GLchar* name, float* fv, int size) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform1fv(id, size, fv);
	}
}

void Shader:: SetUniform(const GLchar* name, float x, float y) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform2f(id, x, y);
	}
}

void Shader::SetUniform(const GLchar* name, float x, float y, float z, float w) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniform4f(id, x, y, z, w);
	}
}


void Shader::SetUniformMatrix4x4(const GLchar* name, float *fv) {
	if (shaderProgram) {
		GLuint id = glGetUniformLocation(shaderProgram, name);
		if (id != -1)
			glUniformMatrix4fv(id, 1, false, fv);
	}
}