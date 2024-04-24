#pragma once

#include "noise.h"
#include "shader.h"
#include "terrain.h"
#include "window.h"
#include "glm.h"

/**
* \brief The Texture class.
*
* Implements the functunality to upload texture images to OpenGL. It is possible
* to load a texture directly from a file or from the pixel data itself. Currently 
* it is only possible to load '.bmp' files. The generated texture needs to be applied 
* to a sampler in a shader program afterwards. If you want to reduce memory consumption 
* disable mipmapping or lower the number of generated mipmaps.
*/
class Texture
{
public:
	/**
	* \brief Constructor that generates a texture from a '.bmp' file. 
	*
	* \param[in] file Char array for the file's name.
	*
	* \param[in] wrapper Defines how the texture is repeated when drawn multiple times.
	* Can be (for example) one  GL_MIRRORED_REPEAT or GL_REPEAT. For a detailed desciption 
	* look at the OpenGL Reference page for the 'glTexParameteri' function and parameter
	* GL_TEXTURE_WRAP_(S / T).
	*
	* \param[in] filter Determines the transition between colors of the texture. Can be GL_NEAREST 
	* for maximum performance but without smoothed color transition or GL_LINEAR for a smoothed one
	* but less perfomance. For For a detailed desciption look at the OpenGL Reference page 
	* for the 'glTexParameteri' function and parameter GL_TEXTURE_(MIN / MAG)_FILTER.
	*
	* \param[in] texUnit Unit of the texture, used for the glActiveTexture method.
	*/
	Texture(const char* file, GLint wrapper, GLint filter, GLuint texUnit);

	/**
	* \brief Constructor that generates a texture from a '.bmp' file.
	*
	* \param[in] data Pointer to the pixel data of the texture.
	*
	* \param[in] texWidth Number of color values in width.
	*
	* \param[in] texHeight Number of color values in height.
	*
	* \param[in] format Format that determines how the values are read from the given data
	* Pointer. Could be for example GL_RGB or GL_RGBA for textures with alpha value.
	*
	* \param[in] wrapper See desciption of constructor 1.
	*
	* \param[in] filter See descrittion of constructor 1.
	*
	* \param[in] texUnit Unit of the texture, used for the glActiveTexture method.
	*/
	Texture(const void* data, unsigned texWidth, unsigned texHeight, GLenum format, GLint wrapper, GLint filter, GLuint texUnit);

	/**
	* \brief Deletes the texture.
	*/
	~Texture() { glDeleteTextures(1, &id); }

	/**
	* \brief Uploads new texture data to the already created OpenGL texture. Need to call  
	* glTexImage2D again if the texture sizes differ.
	*
	* \param[in] format Format that determines how the values are read from the given data
	* Pointer. Could be for example GL_RGB or GL_RGBA for textures with alpha value.
	*
	* \param[in] type Type of one color element. Could be GL_FLOAT.
	*
	* \param[in] filter See descrittion of constructor 1.
	*
	* \param[in] texUnit Unit of the texture, used for the glActiveTexture method.
	*
	* \param[in] texWidth Number of color values in width.
	*
	* \param[in] texHeight Number of color values in height.
	*/
	void sub(const void* data, GLenum format, GLenum type, unsigned texWidth, unsigned texHeight);

	/**
	* \brief Uses the texture by setting the active texture unit with glActiveTexture and then bind
	* the texture id to GL_TEXTURE_2D. Need to be called when another texture unit was used before.
	*/
	void use();

	/**
	* \brief Unbinds the texture unit.
	*/
	void unbind();

	/**
	* \brief Getter for the texture id.
	*/
	GLuint getId()const { return id; }

	/**
	* \brief Getter for the texture unit id.
	*/
	GLuint getUnit()const { return m_unit; }


private:
	/**
	* \brief Pixel count in width.
	*/
	unsigned m_texWidth;

	/**
	* \brief Pixel count in height.
	*/
	unsigned m_texHeight;

	/**
	* \brief Texture unit id.
	*/
	GLuint m_unit;

	/**
	* \brief Texture id.
	*/
	GLuint id;
};

