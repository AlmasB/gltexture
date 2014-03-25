#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <GL/glu.h>

#include <string.h>

static const char* vertexShaderCode = "           \n\
#version 120                                      \n\
                                                  \n\
attribute vec3 Position;                          \n\
attribute vec2 UV;                                \n\
varying vec2 outUV;                               \n\
                                                  \n\
void main() {                                     \n\
	vec2 size = Position.xy - vec2(400, 300); \n\
	size /= vec2(400, -300);                  \n\
	gl_Position = vec4(size, 0, 1);           \n\
                                                  \n\
	outUV = UV;                               \n\
}";

static const char* fragmentShaderCode = "         \n\
#version 120                                      \n\
                                                  \n\
varying vec2 outUV;                               \n\
uniform sampler2D sampler;                        \n\
                                                  \n\
void main() {                                     \n\
	gl_FragColor = texture2D(sampler, outUV); \n\
}";

SDL_Window * window;
SDL_GLContext glContext;

void init() {
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Texture Example in OpenGL",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	glContext = SDL_GL_CreateContext(window);

	IMG_Init(IMG_INIT_PNG);

	glewInit();
}

GLuint createShader(const char * shaderCode, GLenum shaderType) {
	GLuint shader = glCreateShader(shaderType);

	const GLchar* strings[] = { shaderCode };	// shader code strings
	GLint lengths[] = { (GLint)strlen(shaderCode) };	// shader code string length

	glShaderSource(shader, 1, strings, lengths);
	glCompileShader(shader);

	return shader;
}

GLuint createProgram(const char * vertexShaderCode, const char * fragmentShaderCode) {
	GLuint program = glCreateProgram();

	glAttachShader(program, createShader(vertexShaderCode, GL_VERTEX_SHADER));
	glAttachShader(program, createShader(fragmentShaderCode, GL_FRAGMENT_SHADER));
	glLinkProgram(program);
	glValidateProgram(program);

	return program;
}

GLuint createGLTextureFromSurface(SDL_Surface * surface) {
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void drawGLTexture(GLuint textureID, float x, float y, int textureW, int textureH) {
	GLuint textureVBO;
	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);

	GLfloat textureVertexData[] = {
		x, y, 0, 0,
		x, y + textureH, 0, 1.0f,
		x + textureW, y, 1.0f, 0,

		x, y + textureH, 0, 1.0f,
		x + textureW, y + textureH, 1.0f, 1.0f,
		x + textureW, y, 1.0f, 0
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertexData), textureVertexData, GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);	// x,y repeat every 4 values

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(GLfloat), (const GLvoid*)(2 * sizeof(GLfloat)));	// u,v start at 2, repeat 4

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteBuffers(1, &textureVBO);
}

int main(int argc, char * args[]) {
	init();

	GLuint program = createProgram(vertexShaderCode, fragmentShaderCode);
	glUseProgram(program);

	SDL_Surface * texture = IMG_Load("texture.png");
	GLuint textureID = createGLTextureFromSurface(texture);
	SDL_FreeSurface(texture);

	drawGLTexture(textureID, 50, 100, 512, 512);	
	SDL_GL_SwapWindow(window);

	SDL_Delay(5000);

	glDeleteTextures(1, &textureID);

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
