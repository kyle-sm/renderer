#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>


#define PI 3.14159f

struct shader {
	char* filename;
	GLenum type;
};


struct vertexbuffer {
	GLenum usage;
	int length;
	float* vertices;
};

struct elembuffer {
	GLenum usage;
	int length;
	GLuint* vertices;
};

struct model {
	vec3 position, rotation;
	float scale;
	struct vertexbuffer vb;
	struct elembuffer eb;
};

struct modelnode {
	struct model* data;
	struct modelnode* next;
};

//creates a shader program from an array of shader structs
GLuint generateprogram(struct shader* shaders, int size);
inline char* readshader(char* filename);
struct model genmodel(float *vertices, int vlen, GLuint *elems, int elen);
void addmodel(struct model *new);
void drawmodels();
void setposition(int x, int y, int z, struct model *m);