#include "texture.h"

Texture::Texture(const char* file, GLint wrapper, GLint filter, GLuint texUnit) : m_unit(texUnit){
	// Load bmp image
	SDL_Surface* texture = SDL_LoadBMP(file);
	
	if (texture){
		// Clean error buffer
		checkGLError("Texture(..) 1 -> Error occured before this call.");

		// Generate texture id
		glGenTextures(1, &id);

		// Bind texture
		use();

		// Texture parameter
		if (wrapper != GL_NONE){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapper);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapper);
			checkGLError("Texture(..) 1 -> Texture parameter for GL_TEXTURE_WRAP_(S / T)");
		}
		if (filter != GL_NONE) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			checkGLError("Texture(..) 1 -> Texture parameter for GL_TEXTURE_MIN_FILTER");
		}
		if (filter == GL_NEAREST || filter == GL_LINEAR) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			checkGLError("Texture(..) 1 -> Texture parameter for GL_TEXTURE_MAG_FILTER");
		}

		// Mipmapping
		bool mipmapping = false;
		if (filter == GL_LINEAR_MIPMAP_LINEAR || filter == GL_LINEAR_MIPMAP_NEAREST ||
			filter == GL_NEAREST_MIPMAP_LINEAR || filter == GL_NEAREST_MIPMAP_NEAREST){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0);
			mipmapping = true;
			checkGLError("Texture(..) 1 -> Texture parameter for mipmaps");
		}
	
		// Save texture width and height
		m_texWidth = texture->w;
		m_texHeight = texture->h;

		// Upload texture
		//glTexImage2D(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *pixels);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, texture->pixels);
		checkGLError("Texture(..) 1 -> Upload texture data");

		// Generate mip map textures
		if (mipmapping) {
			glGenerateMipmap(GL_TEXTURE_2D);
			checkGLError("Texture(..) 1 -> Generate mipmaps");
		}

		//Cleanup texture binds.
		unbind();
	}
	else
		printError("Texture(..) 1", "Failed to load image -> " + string(SDL_GetError()));

	// Free texture
	SDL_FreeSurface(texture);
}

Texture::Texture(const void* data, unsigned tW, unsigned tH, GLenum format, GLint wrapper, GLint filter, GLuint texUnit)
	: m_texWidth(tW), m_texHeight(tH), m_unit(texUnit) 
{
	if (tW == 0 || tH == 0)
		printCriticalError("Texture(..) 2", "Texture width and / or height must not be 0.");
	else if (data){
		// Clean error buffer
		checkGLError("Texture(..) 1 -> Error occured before this call.");

		// Generate tex id
		glGenTextures(1, &id);

		// Bind texture
		use();

		// Texture parameter
		if (wrapper != GL_NONE){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapper);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapper);
			checkGLError("Texture(..) 1 -> Texture parameter for GL_TEXTURE_WRAP_(S / T)");
		}
		if (filter != GL_NONE) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			checkGLError("Texture(..) 1 -> Texture parameter for GL_TEXTURE_MIN_FILTER");
		}
		if (filter == GL_NEAREST || filter == GL_LINEAR) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			checkGLError("Texture(..) 1 -> Texture parameter for GL_TEXTURE_MAG_FILTER");
		}

		// Setting parameter for mip map textures
		bool mipmapping = false;
		if (filter == GL_LINEAR_MIPMAP_LINEAR || filter == GL_LINEAR_MIPMAP_NEAREST ||
			filter == GL_NEAREST_MIPMAP_LINEAR || filter == GL_NEAREST_MIPMAP_NEAREST){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0);
			checkGLError("Texture(..) 1 -> Texture parameter for mipmaps");
			mipmapping = true;
		}

		// Upload texture
		if(format == GL_RGB)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_texWidth, m_texHeight, 0, format, GL_FLOAT, data);
		else if (format == GL_RGBA)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_texWidth, m_texHeight, 0, format, GL_FLOAT, data);
		else
			printError("Texture(..) 2", "Given format is not supported.");
		checkGLError("Texture(..) 2 -> Upload texture data");

		// Generate mipmap textures
		if (mipmapping) {
			glGenerateMipmap(GL_TEXTURE_2D);
			checkGLError("Texture(..) 1 -> Generate mipmaps");
		}

		//Cleanup texture binds.
		unbind();
	}
	else
		printError("Texture(..) 2", "Given texture data is invalid");
}

void Texture::sub(const void* data, GLenum format, GLenum type, unsigned texWidth, unsigned texHeight){
	if (texWidth == 0 || texHeight == 0)
		printError("Texture::sub(..)", "Texture width and / or height must not be 0. No texture data uploaded");
	else {
		// Clean error buffer
		checkGLError("Texture(..) 1 -> Error occured before this call.");

		// Bind texture
		use();

		// If size is the same ..
		if (texWidth == m_texWidth && texHeight == m_texHeight) {
			// just overwrite data with glTexSubImage2D
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texWidth, m_texHeight, format, type, data);
			checkGLError("Texture::sub(..) -> glSubImage2D()");
		}
		else {
			// Otherwise create a new texture and overwrite the old one.
			m_texWidth = texWidth;
			m_texHeight = texHeight;
			if (format == GL_RGB)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_texWidth, m_texHeight, 0, format, GL_FLOAT, data);
			else if (format == GL_RGBA)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_texWidth, m_texHeight, 0, format, GL_FLOAT, data);
			checkGLError("Texture::sub(..) -> glTexImage2D()");
		}

		//Cleanup texture binds.
		unbind();
	}
}

void Texture::use() {
	glActiveTexture(GL_TEXTURE0 + m_unit);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() {
	glActiveTexture(GL_TEXTURE0 + m_unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}
