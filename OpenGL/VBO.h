#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO {
	public:
		//ID reference for VBO
		GLuint ID;
		//makes the VBO
		VBO(GLfloat* vertices, GLsizeiptr size);

		void Bind();
		void Unbind();
		void Delete();
};
#endif