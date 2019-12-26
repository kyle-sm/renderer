#include "render.h"
float cubeverts[] = {
						0.5f, 0.5f, 0.5f,
						-0.5f, 0.5f, 0.5f,
						0.5f, -0.5f, 0.5f,
						-0.5f, -0.5f, 0.5f,
						0.5f, 0.5f, -0.5f,
						-0.5f, 0.5f, -0.5f,
						0.5f, -0.5f, -0.5f,
						-0.5f, -0.5f, -0.5f,
					};
GLuint cubelems[] = {
						0, 1, 2, 1, 3, 2, //front 
						6, 7, 2, 2, 7, 3, //bottom
						4, 7, 6, 7, 5, 4, //back
						4, 5, 0, 5, 1, 0, //top 
						7, 1, 5, 7, 3, 1, //left
						6, 2, 0, 6, 0, 4 //right
					};
struct modelnode *models = NULL; 
GLuint shaderProgram;
GLFWwindow *window;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	
	struct shader shaders[] = {	{"fragment", GL_FRAGMENT_SHADER},
								{"vertex", GL_VERTEX_SHADER}};

	shaderProgram = generateprogram(shaders, 2);
	glUseProgram(shaderProgram);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	for(int r = -4; r < 5; r += 2) {
		for(int c = -4; c < 5; c += 2) {
			struct model *cube = malloc(sizeof(struct model));
			*cube = genmodel(cubeverts, 24, cubelems, 36);
			setposition(r, 0, c, cube);
			addmodel(cube);
		}
	}

	mat4 perspective = GLM_MAT4_IDENTITY_INIT;
	glm_perspective(PI/2, 1.25, 0, 10, perspective);
 	GLuint uproj = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(uproj, 1, GL_TRUE, perspective[0]);
	GLuint uview = glGetUniformLocation(shaderProgram, "view");

	vec3 eye = {0.0, 0.0, 0.0};
	vec3 dir = {0.0, 0.0, -1.0};
	vec3 up = {0.0, 1.0, 0.0};

	while(!glfwWindowShouldClose(window)) {
		glm_vec3_normalize(dir);
		mat4 view = GLM_MAT4_IDENTITY_INIT;
		glm_look(eye, dir, up, view);
		glUniformMatrix4fv(uview, 1, GL_TRUE, view[0]);
		drawmodels();
		glfwPollEvents();
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			vec3 velocity;
			glm_vec3_scale(dir, 0.1f, velocity);
			glm_vec3_add(eye, velocity, eye);
		}
		if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
			vec3 velocity;
			glm_vec3_scale(dir, 0.1f, velocity);
			glm_vec3_sub(eye, velocity, eye);
		}
		if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			glm_vec3_rotate(dir, +0.05f, GLM_YUP);
		if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			glm_vec3_rotate(dir, -0.05f, GLM_YUP);
		if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			eye[1] += 0.1f;
		}
	}

	glfwTerminate();
	return 0;
}

struct model genmodel(float *vertices, int vlen, GLuint *elems, int elen) {
	struct model m = {
				.position = {0, 0, 0},
				.rotation = {0, 0, 0},
				.scale = 1.0f,
				.vb = {
						.usage = GL_STATIC_DRAW,
						.length = vlen,
						.vertices = vertices
				},
				.eb = {
						.usage = GL_STATIC_DRAW,
						.length = elen,
						.vertices = elems
				}
			};
	return m;
}

void addmodel(struct model *new){
	struct modelnode *m = malloc(sizeof(struct modelnode));
	m->next = models;
	m->data = new;
	models = m;
}

void drawmodels() {
	glClear(GL_COLOR_BUFFER_BIT);
	for(struct modelnode* m = models; m != NULL; m = m->next) {
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->data->vb.length, m->data->vb.vertices, GL_STATIC_DRAW);
		
		GLuint ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m->data->eb.length, m->data->eb.vertices, GL_STATIC_DRAW);
	
		GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(posAttrib);

		mat4 translation;
		glm_translate_make(translation, m->data->position);
		mat4 rotation = GLM_MAT4_IDENTITY_INIT;
		glm_euler(m->data->rotation, rotation);
		mat4 model = GLM_MAT4_IDENTITY_INIT;
		glm_mul(translation, rotation, model);

		GLuint umodel = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(umodel, 1, GL_TRUE, model[0]);
	
		glDrawElements(GL_TRIANGLES, m->data->eb.length, GL_UNSIGNED_INT, 0);
	}
	glfwSwapBuffers(window);
}

GLuint generateprogram(struct shader *shaders, int size) {
	GLuint program = glCreateProgram();
	for(int i = 0; i < size; i++) {
		GLchar *shaderSource = (GLchar*)readshader((shaders + i)->filename);
		GLuint shader = glCreateShader((shaders + i)->type);
		glShaderSource(shader, 1, &shaderSource, NULL);
		glCompileShader(shader);
		free(shaderSource);
		
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if(status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(shader, 512, NULL, buffer);
			//this removes the leading digit making error messages easier for acme to handle
			fprintf(stderr, "%s%s", (shaders + i)->filename, buffer+1); 
			exit(EXIT_FAILURE);
		}
		
		glAttachShader(program, shader);
	}	
	glBindFragDataLocation(program, 0, "outColor");
	glLinkProgram(program);

	return program;
}

inline char* readshader(char* filename) {
	FILE *f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	rewind(f);
	
	char *string = malloc(fsize + 1);
	fread(string, 1, fsize, f);
	fclose(f);
	
	string[fsize] = '\0';

	return string;
}

void setposition(int x, int y, int z, struct model *m) {
	m->position[0] = x;
	m->position[1] = y;
	m->position[2] = z;
}