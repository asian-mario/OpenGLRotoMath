#version 330 core

//Position/Co-ords
layout (location = 0) in vec3 aPos; 
//Colors
layout (location = 1) in vec3 aColor; 

//outputs color for the frag shader
out vec3 color;

//export texture coord
out vec2 texCoord;

layout (location = 2) in vec2 aTex;

//controls scale of verts
uniform float scale;
uniform float time;




void main()
{
	//outputs all positions of all vertices
	float control = 4.5;
	vec2 rawpos = vec2(aPos.x, aPos.z);
	//rotation matrix
	mat2 rotMat;
	rotMat[0] = vec2(cos(time/control), -sin(time/control)); 
	rotMat[1] = vec2(sin(time/control), cos(time/control));
	

	// apply rotation
	vec2 newPos = vec2(rotMat * rawpos);
	
	gl_Position = vec4(newPos.x, aPos.y , newPos.y ,1.0);

	//assigns colors from the vertex data to "color"
	color = aColor;
	texCoord = aTex;
}

