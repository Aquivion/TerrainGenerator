#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>

using namespace std;

/**
* \brief Function for printing an error message in the command line.
* Will be implemented once in the "error.cpp" file. Marked as inline
* to avoid memory usage for the function.
*
* \param[in] pos String that should mark the position, where the error occurred.
*
* \param[in] msg Error message that should tell what went wrong.
*/
extern inline void printError(const string pos, const string msg) {
	cerr << "Error at " << pos << ": \n\n" << msg << "\n\n";
}

/**
* \brief Function for printing an critical error message in the command line
* and terminate the program with "EXIT_FAILURE". Will be implemented once in
* the "error.cpp" file. Marked as inline to avoid memory usage for the function.
*
* \param[in] pos String that should mark the position, where the error occurred.
*
* \param[in] msg Error message that should tell what went wrong.
*/
extern inline void printCriticalError(const string pos, const string msg) {
	cerr << "Critical error at " << pos << ":\n\n" << msg << "\n\nProgram terminated.\n\n";
	exit(EXIT_FAILURE);
}

/**
* \brief Function for printing Error messages that are returned by glGetError().
* Will be implemented once in the "error.cpp" file. Marked as inline to avoid
* memory usage for the function.
*
* \param[in] pos String that should mark the position, where the error occurred.
*/
extern inline void checkGLError(const string pos) {
	GLenum err(glGetError());

	if (err != GL_NO_ERROR) {
		string errorText;
		switch (err) {
			case GL_INVALID_OPERATION:
				errorText = "GL_INVALID_OPERATION"; break;
			case GL_INVALID_ENUM:
				errorText = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:
				errorText = "GL_INVALID_VALUE"; break;
			case GL_OUT_OF_MEMORY:
				errorText = "GL_OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				errorText = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			default:
				errorText = "UNKNOWN ERROR"; break;
		}
		cerr << "OpenGL Error at: " << pos << ":\n\n" << errorText << "\n\n";
	}
}
