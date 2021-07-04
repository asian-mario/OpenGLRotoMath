#include"shaderclass.h"

using namespace std;

//reads textfile and outputs as string
string get_file_contents(const char* filename) {
	ifstream in(filename, ios::binary);
	if (in) {
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}

	throw(errno);
}

//Constructor program to build shader program from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	string vertexCode = get_file_contents(vertexFile);
	string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//Create shader object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(vertexShader, 1, &vertexSource, NULL); //ref value, using 1 string for whole shader, where source code is, ~
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //create fragment shader 
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	ID = glCreateProgram(); //wrap all the shaders into a shaderProgram
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader); //adding the shaders

	glLinkProgram(ID); //wrap up the program
	compileErrors(ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader); //delete these because they are already inside the program
}

//activates the shader for use
void Shader::Activate() {
	glUseProgram(ID);
}

//deletes shader
void Shader::Delete() {
	glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char* type) {
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			cout << "SHADER_COMPILATION_ERROR FOR:" << type << "\n" << endl;
		}
	}

	else {
		glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			cout << "SHADER_LINKING_ERROR FOR:" << type << "\n" << endl;
		}
	}
}