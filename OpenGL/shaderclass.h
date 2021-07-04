#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H


#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

using namespace std;

string get_file_contents(const char* filename);

class Shader {
	public:
		//Ref ID of the shader program
		GLuint ID;
		//constructor that builds shader program from 2 different shaders
		Shader(const char* vertexFile, const char* fragmentFile);

		//Activates Shader Program
		void Activate();
		//Deletes shader program
		void Delete();

	private:	
		void compileErrors(unsigned int shader, const char* type);

};

#endif