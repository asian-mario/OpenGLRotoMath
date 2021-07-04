#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType) {
	type = texType;

	int widthImg, heightImg, numColCh; //image width & height and amount of colour channels
	//flips image to the correct side
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0); //loc, width, height, num of colour channels and desired channels

	GLuint texture;
	glGenTextures(1, &ID); //num. of textures & reference variable
	glActiveTexture(GL_TEXTURE0); //choosing the slot you want to use for the texture
	glBindTexture(texType, ID); //type of texture and the var

	/*when scaled up or down we need to choose between GL_NEAREST and GL_LINEAR,
	GL_NEAREST keeps all the pixels as is and can appear pixelated when scaled up to high (more for pixel art)
	GL_LINEAR which makes new pixels according to pixels nearby (may make image blurier
	*/

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //type of texture, setting, what we want to change inside setting
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//textures can be repeated a few ways, GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE and GL_CLAMP_TO_BORDER. you can mix and match since they only aplly on one axis. s = x, t = y, z = r

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/*GL_CLAMP_TO_BORDER:
		float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f,};
		glTexParamterfv(texType, GL_TEXTURE_BORDER_COLOR, flatColor);
	*/

	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes); //Texture type, ~, type of col channels, width, height, ~, color channels our image has, datatype of pixels, image info
	glGenerateMipmap(texType); //smaller resolution versions

	stbi_image_free(bytes); //delete data
	glBindTexture(texType, 0); //unbind texture


}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit) {
	GLuint tex0uni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(tex0uni, unit);
}

void Texture::Bind() {
	glBindTexture(type, ID);
}

void Texture::Unbind() {
	glBindTexture(type, 0);
}

void Texture::Delete() {
	glDeleteTextures(1, &ID);
}
