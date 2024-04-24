#pragma once

#include <initializer_list>
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include "error.h"

/**
* \brief Utility class for loading and linking shader files to a shader program.
*
* Currently it supports loading vertex and fragment shader.
*/
class Shader {

public:
	/**
	* \brief Constructor for loading all of the given shader source-files,
	*		 which are then compiled and linked to a shader program.
	*
	* \param[in] vsh Relative path to vertex shader file.
	*
	* \param[in] fsh Relative path to fragment shader file.
	*/
	Shader(string vsh, string fsh);

	/**
	* \brief Deletes the shader program
	*/
	~Shader();

	/**
	* \brief Uses the shader program.
	*/
	void use()const { glUseProgram(m_id); }

	/**
	* \brief Getter for the shader id.
	*/
	const GLuint getId()const { return m_id; }

private:
	/**
	* \brief Loads and compiles a single shader file. This is used by the constructor above.
	*
	* \param[in] path Path to the shader file.
	*/
	Shader(string path);

	/**
	* \brief Identification number for the shader
	*/
	GLuint m_id;

	/**
	* \brief Variable to differenciate if the shader is a shader program or a single shader
	*/
	bool m_isProg;
};
