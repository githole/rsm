#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

#include "shader.h"
#include "rt.h"
#include "matrix.h"
#include "primitive.h"

static Shader shader;
static Shader lightShader;
static Shader postShader;
static Shader vplShader;
static Shader vpl_normalShader;
static Shader vpl_posShader;
static Shader giShader;
static Shader blurXShader;
static Shader blurYShader;


static RenderTarget *giRT;
static RenderTarget *lightRT;
static RenderTarget *cameraRT;

static RenderTarget *vplRT;
static RenderTarget *vplPosRT;
static RenderTarget *vplNormalRT;

static RenderTarget *blurRT;

static float myAngle, sunAngle;
const int CameraTexSize = 1024;
const int ShadowMapSize = 1024;
const int vplTexSize = 512;
const int giTexSize = 256;
const int blurTexSize = 256;


static int windowWidth = 1280;
static int windowHeight = 720;

void CompileShader() {
	shader.CompileFromFile("shader.fs", "shader.vs");
	lightShader.CompileFromFile("light.fs", "light.vs");
	postShader.CompileFromFile("post.fs", "post.vs");
	vplShader.CompileFromFile("vpl.fs", "vpl.vs");
	vpl_normalShader.CompileFromFile("vpl_normal.fs", "vpl_normal.vs");
	vpl_posShader.CompileFromFile("vpl_pos.fs", "vpl_pos.vs");
	giShader.CompileFromFile("gi.fs", "gi.vs");
	blurXShader.CompileFromFile("blurx.fs", "blur.vs");
	blurYShader.CompileFromFile("blury.fs", "blur.vs");
}

int ProcessSDLEvents() {
    SDL_Event eve;
    while (SDL_PollEvent(&eve)) {
        switch(eve.type) {
		case SDL_KEYDOWN:
			{
			Uint16 ch = eve.key.keysym.unicode;
			SDLKey key = isprint(ch) ? (SDLKey)ch : eve.key.keysym.sym;

			if (eve.key.keysym.sym == SDLK_F5) {
				CompileShader();
			}
			if (eve.key.keysym.sym == SDLK_LEFT)
				myAngle -= 0.01f;
			if (eve.key.keysym.sym == SDLK_RIGHT)
				myAngle += 0.01f;
			
			if (eve.key.keysym.sym == SDLK_a)
				sunAngle -= 0.01f;
			if (eve.key.keysym.sym == SDLK_s)
				sunAngle += 0.01f;

			if (eve.key.keysym.sym == SDLK_ESCAPE)
				return -1;
			}
			break;

        case SDL_QUIT:
            return -1;
        }
    }
	return 0;
}


int Initialize(const int width, const int height, const int SDLflags) {
    const SDL_VideoInfo* info = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << SDL_GetError() << std::endl;
		return -1;
    }
	std::cerr << "SDL_Init succeeded" << std::endl;
	
	SDL_WM_SetCaption("test", NULL);
	
    info = SDL_GetVideoInfo( );

    if (!info) {
		std::cerr << SDL_GetError() << std::endl;
		return -1;
    }
	std::cerr <<  "SDL_GetVideoInfo succeeded" << std::endl;
	
    const int bpp = info->vfmt->BitsPerPixel;
	
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_EnableUNICODE(true);
	SDL_EnableKeyRepeat(25, 1);
	
    if (SDL_SetVideoMode(width, height, bpp, SDLflags) == 0) {
		std::cerr << SDL_GetError() << std::endl;
		return -1;
    }
	std::cerr << "SDL_SetVideoMode succeeded" << std::endl;
	
	if (glewInit() != GLEW_OK) {
		std::cerr << "Error: glewInit()" << std::endl;
		return -1;
	}
	std::cerr << "glewInit succeeded" << std::endl;

	// shader
	CompileShader();

	// シャドウマップ
	lightRT = new RenderTarget(ShadowMapSize, GL_CLAMP_TO_BORDER);

	// カメラテクスチャ
	cameraRT = new RenderTarget(CameraTexSize, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_RGBA, GL_UNSIGNED_BYTE);
	
	// VPL
	vplRT = new RenderTarget(vplTexSize, GL_CLAMP_TO_EDGE, GL_NEAREST);
	
	// VPL法線
	vplNormalRT = new RenderTarget(vplTexSize, GL_CLAMP_TO_EDGE, GL_NEAREST);
	
	// VPL位置
	vplPosRT = new RenderTarget(vplTexSize, GL_CLAMP_TO_EDGE, GL_NEAREST);

	// GI
	giRT = new RenderTarget(giTexSize);

	// BluredGI
	blurRT = new RenderTarget(blurTexSize);


	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return 0;
}


void SetMatrix(Shader& s) {
	// CameraProjection
	s.SetUniformMatrix4x4("uCamProj", Matrix::PerspectiveMatrix(45.0f, (float)windowWidth / windowHeight, 0.1f, 100.0f).m);
	
	// CameraView
	s.SetUniformMatrix4x4("uCamView", 
		Matrix::LookAt(5.0f * sin(myAngle), 5.0f, 5.0f * cos(myAngle),
					   0.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 0.0f).m);
	// LightProjection
	s.SetUniformMatrix4x4("uLightProj", Matrix::OrthoMatrix(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f).m);
	
	// LightView
	Matrix uLightView = Matrix::LookAt(12.0f * sin(sunAngle), 16.0f, 10.0f * cos(sunAngle),
					   0.0f, 0.0f, 0.0f,
					   0.0f, 1.0f, 0.0f);
	s.SetUniformMatrix4x4("uLightView", uLightView.m);

	GLfloat invm[16];
	GLfloat transm[16];
	Matrix::inverse(uLightView.m, invm);
	Matrix::transpose(invm, transm);
	s.SetUniformMatrix4x4("uLightNormalMatrix", transm);
}


void DrawObjects(Shader &s) {
	// 平面描画
	s.SetUniformMatrix4x4("uModel", Matrix().m);
	glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
	DrawPlane(20.0f);

	// 立方体
	// ModelMatrix
	s.SetUniformMatrix4x4("uModel", Matrix().m);
	glColor4f(1, 0.4f, 0.5f, 1.0f);
	DrawCube(1.0f, 1.0f, 1.0f);
	
	// ModelMatrix
	s.SetUniformMatrix4x4("uModel", Matrix::TranslateMatrix(0.85f, 0.7f, 0.3f).m);
	glColor4f(0.98f, 0.98f, 0.98f, 1.0f);
	DrawCube(1.0f, 1.0f, 1.0f);
	
	// ModelMatrix
	s.SetUniformMatrix4x4("uModel", Matrix::TranslateMatrix(-0.3f, -0.5f, 2.5f).m);
	glColor4f(0.2f, 0.3f, 1, 1.0f);
	DrawCube(1.0f, 1.0f, 1.0f);
	
	// ModelMatrix
	s.SetUniformMatrix4x4("uModel", Matrix::TranslateMatrix(2.0f, 0.6f, 1.5f).m);
	glColor4f(0.2f, 1.0f, 0.3f, 1.0f);
	DrawCube(1.0f, 1.0f, 1.0f);
	
	// ModelMatrix
	s.SetUniformMatrix4x4("uModel", Matrix::TranslateMatrix(-1.85f, -0.5f, -1.3f).m);
	glColor4f(0.98f, 0.98f, 0.98f, 1.0f);
	DrawCube(1.0f, 1.0f, 1.0f);
}

static float lightIntensity[4] = {1.5f, 1.4f, 1.2f, 1.0f};
static float ambient[4] = {0.7f, 0.8f, 1.1f, 1.0f};

void RenderObject() {
	cameraRT->Bind();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);

	shader.Bind();
	shader.SetUniform("resolution", (float)cameraRT->Size(),  (float)cameraRT->Size());
	shader.SetUniform("uAmbient", ambient[0], ambient[1], ambient[2], ambient[3]);
	shader.SetUniform("uLightIntensity", lightIntensity[0], lightIntensity[1], lightIntensity[2], lightIntensity[3]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightRT->Texture());
	shader.SetUniform("shadowMap", (int)0);

	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, giRT->Texture());
	shader.SetUniform("giTexture", (int)1);


	SetMatrix(shader);
	DrawObjects(shader);

	shader.Unbind();

	// テクスチャ描画
	const float aspect = (float)windowWidth / windowHeight;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightRT->Texture());
	DrawTexture(0.9f, -0.8f, 0.1f, 0.1f * aspect);
	
	glBindTexture(GL_TEXTURE_2D, vplRT->Texture());
	DrawTexture(0.6f, -0.8f, 0.1f, 0.1f * aspect);
	
	glBindTexture(GL_TEXTURE_2D, vplPosRT->Texture());
	DrawTexture(0.3f, -0.8f, 0.1f, 0.1f * aspect);
	
	glBindTexture(GL_TEXTURE_2D, vplNormalRT->Texture());
	DrawTexture(0.0f, -0.8f, 0.1f, 0.1f * aspect);
	
	glBindTexture(GL_TEXTURE_2D, giRT->Texture());
	DrawTexture(-0.3f, -0.8f, 0.1f, 0.1f * aspect);
}

void RenderVPL() {
	vplRT->Bind();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);

	vplShader.Bind();

	vplShader.SetUniform("uLightIntensity", lightIntensity[0], lightIntensity[1], lightIntensity[2], lightIntensity[3]);

	SetMatrix(vplShader);
	DrawObjects(vplShader);

	vplShader.Unbind();
}


void RenderVPLPos() {
	vplPosRT->Bind();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	vpl_posShader.Bind();

	SetMatrix(vpl_posShader);
	DrawObjects(vpl_posShader);

	vpl_posShader.Unbind();
}

void RenderVPLNormal() {
	vplNormalRT->Bind();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);
	vpl_normalShader.Bind();

	SetMatrix(vpl_normalShader);
	DrawObjects(vpl_normalShader);

	vpl_normalShader.Unbind();
}

void RenderGI() {
	giRT->Bind();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);

	giShader.Bind();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, vplRT->Texture());
	giShader.SetUniform("vplTexture", (int)1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, vplPosRT->Texture());
	giShader.SetUniform("vplPosTexture", (int)2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, vplNormalRT->Texture());
	giShader.SetUniform("vplNormalTexture", (int)3);

	SetMatrix(giShader);
	DrawObjects(giShader);

	giShader.Unbind();
}

void RenderShadowmap() {
	lightRT->Bind();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_DEPTH_TEST);

	lightShader.Bind();

	SetMatrix(lightShader);
	 DrawObjects(lightShader);

	lightShader.Unbind();
}

void BlurGI(RenderTarget *rt) {
	blurRT->Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	blurYShader.Bind();
	blurYShader.SetUniform("resolution", (float)blurRT->Size(), (float)blurRT->Size());
	// テクスチャ描画
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rt->Texture());
	blurYShader.SetUniform("texture", (int)0);
	DrawTexture(0, 0, 1, 1);
	blurYShader.Unbind();	


	rt->Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	blurXShader.Bind();
	blurXShader.SetUniform("resolution", (float)rt->Size(), (float)rt->Size());
	// テクスチャ描画
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurRT->Texture());
	blurXShader.SetUniform("texture", (int)0);
	DrawTexture(0, 0, 1, 1);
	
	blurXShader.Unbind();	

}

int Render() {
	RenderVPL();
	RenderVPLPos();
	RenderVPLNormal();
	RenderGI();
	for (int i = 0; i < 6; i ++)
		BlurGI(giRT);

	RenderShadowmap();
	RenderObject();

	
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	postShader.Bind();
	// テクスチャ描画
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cameraRT->Texture());
	postShader.SetUniform("texture", (int)0);
	postShader.SetUniform("resolution", (float)windowWidth, (float)windowHeight);
	postShader.SetUniform("time", (float)SDL_GetTicks());
	DrawTexture(0, 0, 1, 1);
	postShader.Unbind();	

	SDL_GL_SwapBuffers();
	return 0;
}

int main(int argc, char** argv) {
	int flags = SDL_OPENGL;

	if (argc >= 2) {
	  if (strcmp(argv[1], "-f") == 0) {
		  flags = SDL_OPENGL | SDL_FULLSCREEN ;
	  }
	}
	if (argc >= 4) {
	  windowWidth = atoi(argv[2]);
	  windowHeight = atoi(argv[3]);
	}

	if (Initialize(windowWidth, windowHeight, flags) < 0)
		return 0;

	while (1) {
		if (ProcessSDLEvents() < 0)
			break;
		
		int now = SDL_GetTicks();
		Render();
		int tm = SDL_GetTicks() - now;
		char buf[256];
		sprintf(buf, "%d", tm);
		SDL_WM_SetCaption(buf, NULL);
	}

	return 0;
}
