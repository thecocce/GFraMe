/**
 * @file [...]
 * 
 * Declare every static variable to be used by the OpenGL wrapper
 * 
 * @author GFM
 */
#ifndef __GLW_STATIC_H_
#define __GLW_STATIC_H_

static SDL_GLContext *ctx;
static GLfloat worldMatrix[16] = 
	{1.0f, 0.0f, 0.0f,-1.0f,
	 0.0f, 1.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f};

static GLuint sprVbo;
static GLuint sprIbo;
static GLuint sprVao;
static GLuint sprTex;
static GLuint sprPrg;
static GLuint sprLocToGL;
static GLuint sprTexDimensions;
static GLuint sprTranslation;
static GLuint sprDimension;
static GLuint sprTexOffset;
static GLuint sprSampler;

static GLuint bbVbo;
static GLuint bbIbo;
static GLuint bbVao;
static GLuint bbTex;
static GLuint bbFbo;
static GLuint bbPrg;
static GLuint bbSampler;

#endif

