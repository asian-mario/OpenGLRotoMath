#define _USE_MATH_DEFINES

#include<iostream>
#include<stdlib.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<cmath>

#include"shaderclass.h"
#include"VAO.h"
#include"EBO.h"
#include"VBO.h"
#include <chrono>
#include <cmath>


using namespace std;

// make the decltype slightly easier to the eye
using seconds_t = std::chrono::seconds;

// return the same type as seconds.count() below does.
// note: C++14 makes this a lot easier.
decltype(seconds_t().count()) get_seconds_since_epoch()
{
	// get the current time
	const auto now = chrono::system_clock::now();

	// transform the time into a duration since the epoch
	const auto epoch = now.time_since_epoch();

	// cast the duration into seconds
	const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);

	// return the number of seconds
	return seconds.count();
}

/* 
In the background the previous frame shown is being overwritten with new information, these are called buffers
front buffer = seen/read
back buffer = written

Vertex data: Vertex Shader > Shape Assembly > Geo Shader > Rasterization (Mathematical shape into pixels) > Fragment Shader (Adds colours to pixels, depends on a lot) > tests and blending

index buffers tell open GL the oder to connect the vertices together, uses indices

#version 330 core
layout (location = 0) in vec3 aPos; uses layout with the location of 0 for the aPos. also says that there is a vec3 datatype
void main()
{
gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); asign gl_position with all needed positions
}
kind of 'outputs' gl_Position

#version 330 core
out vec4 FragColor; outputs a vec4 colour
void main()
{
   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f); what the colour is (i think its orange)
}


uniforms are like universal variables that can be accessed by other shaders and can be accessed from the main function

*/

//------------------------------ROTATION-------------------------------
inline float degToRad(float deg) {
	return deg * (M_PI / 180.0);
}

class Vec2D {
public:
	float x, y;

	Vec2D(float x, float y) {
		this->x = x;
		this->y = y;
	}

	void rotateDegrees(float theta) {
		float rotMatrix[2][2] = {
			{cos(degToRad(theta)), -sin(degToRad(theta))},
			{sin(degToRad(theta)), cos(degToRad(theta)) }
		};



		float newX = (x * rotMatrix[0][0]) + (y * rotMatrix[0][1]); // get new X
		float newY = (x * rotMatrix[1][0]) + (y * rotMatrix[1][1]); // get new Y

		x = newX;
		y = newY;
	}
};

class Mat2D {
public:
	float x[2];
	float y[2];

};

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Specifying OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Using core porfile (just modern libraries)

	GLfloat vert2d[] =
	{//               CO-ORDINATES   /          COLOURS           /   TEXTURE CO-ORDS    //
		-0.5f, -0.5f, 0.0f,              1.0f, 0.0f, 0.0f,         0.0f, 0.0f,//lower left corner
		-0.5f, 0.5f, 0.0f,               0.0f, 1.0f, 0.0f,			0.0f, 1.0f,//Upper left corner
		 0.5f, 0.5f, 0.0f,               0.0f, 0.0f, 1.0f,			1.0f, 1.0f,//Upper right corner
		 0.5f, -0.5f, 0.0f,              1.0f, 1.0f, 1.0f,		1.0f, 0.0f//lower right corner


	};

	//index buffer
	GLuint indices[] = {
		0, 2 ,1, //upper triangle
		0, 3, 2 //lower triangle

	};



	//------------------------------------------------------------------------------------------------

	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Testing", NULL, NULL); //Width, Height, Name, Fullscreen, ~
	if (window == NULL) {
		cout << "Failed to create a window" << endl; //Just checking if the window failed to create
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); //tells glfw to make the window to hold 'all' of opengl

	gladLoadGL(); //load the config for opengl

	glViewport(0, 0, 800, 800); //where we want the opengl to show stuff

	//Creates shadeprogram from default.vert and default.frag
	Shader shaderProgram("Default.vert", "default.frag");

	//Generates VAO and binds it
	VAO VAO1;
	VAO1.Bind();

	//Generates VBO links it to vertices
	VBO VBO1(vert2d, sizeof(vert2d));
	//Generates EBO and links it to indices
	EBO EBO1(indices, sizeof(indices));

	//Links attributes to VAO, like colors and co-ords
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//Unbind the VAO, EBO and VBO
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale"); //get uniform reference value (stored in uniID)
	GLuint timeID = glGetUniformLocation(shaderProgram.ID, "time");


	auto startTime = get_seconds_since_epoch();




	//----------TEXTURES-------------------------------

	int widthImg, heightImg, numColCh; //image width & height and amount of colour channels
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load("pop_cat.png", &widthImg, &heightImg, &numColCh, 0); //loc, width, height, num of colour channels and desired channels

	GLuint texture;
	glGenTextures(1, &texture); //num. of textures & reference variable
	glActiveTexture(GL_TEXTURE0); //choosing the slot you want to use for the texture
	glBindTexture(GL_TEXTURE_2D, texture); //type of texture and the var

	/*when scaled up or down we need to choose between GL_NEAREST and GL_LINEAR, 
	GL_NEAREST keeps all the pixels as is and can appear pixelated when scaled up to high (more for pixel art)
	GL_LINEAR which makes new pixels according to pixels nearby (may make image blurier
	*/

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //type of texture, setting, what we want to change inside setting
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//textures can be repeated a few ways, GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE and GL_CLAMP_TO_BORDER. you can mix and match since they only aplly on one axis. s = x, t = y, z = r

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	/*GL_CLAMP_TO_BORDER:
		float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f,};
		glTexParamterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
	*/

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes); //Texture type, ~, type of col channels, width, height, ~, color channels our image has, datatype of pixels, image info
	glGenerateMipmap(GL_TEXTURE_2D); //smaller resolution versions

	stbi_image_free(bytes); //delete data
	glBindTexture(GL_TEXTURE_2D, 0); //unbind texture

	GLuint tex0uni = glGetUniformLocation(shaderProgram.ID, "tex0");
	shaderProgram.Activate();
	glUniform1i(tex0uni, 0);
	//RGBA is for png, RGB is for jpg
	//-------------------------------------------------

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //clears the colour of current background and replaces it. also this is navy blue
	glClear(GL_COLOR_BUFFER_BIT); //le execute with the color buffer
	glfwSwapBuffers(window);

	while (!glfwWindowShouldClose(window)) {
		
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.Activate();
		//-------GLUNIFORM AFTER SHADERPROGRAM ACTIVATES------
		glUniform1f(uniID, 0.5f); //opengl has many suffixes for gluniform but we are using 1f becasue we are inputting one float (the 0.5f controls scale)
		glUniform1f(timeID, get_seconds_since_epoch() - startTime); 
		glBindTexture(GL_TEXTURE_2D, texture);
		VAO1.Bind();
		//glDrawArrays(GL_TRIANGLES, 0, 3); type of primitive, starting index, amount of vertices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //type of primitive, how many indices, indices datatype, index
		glfwSwapBuffers(window);
		glfwPollEvents(); //this is to make the window actually, respond.
	}

	//------Delete-----------------
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	glDeleteTextures(1, &texture);
	shaderProgram.Delete();
	//-----------------------------

	
	glfwDestroyWindow(window); //killing the window after we finish

	glfwTerminate(); //terminating glfw after function
	return 0;
}