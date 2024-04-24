#include "shader.h"

Shader::Shader(string vsh, string fsh){
	if (vsh.empty())
		printCriticalError("Shader(vsh, fsh)", "Path string to the vertex shader file is empty");
	else if(fsh.empty())
		printCriticalError("Shader(vsh, fsh)", "Path string to the fragment shader file is empty");
	else {
		// Initialize m_isProg = true because this constructor links all shader files to a shader programs
		m_isProg = true;

		// Generate shader program id
		m_id = glCreateProgram();

		// Loading and attaching vertex and fragment shader to a program
		Shader vertexShader(vsh);
		glAttachShader(m_id, vertexShader.getId());
		Shader fragmentShader(fsh);
		glAttachShader(m_id, fragmentShader.getId());

		// Linking the shader program
		glLinkProgram(m_id);

		// Used for error checking
		GLint status, len;

		// Retrieve and print linking errors.
		glGetProgramiv(m_id, GL_LINK_STATUS, &status);

		// If shader program was not linked correctly ...
		if (status != GL_TRUE) {
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
			GLchar* buf = new GLchar[len];
			glGetProgramInfoLog(m_id, len, nullptr, buf);
			cerr << buf;
			delete[] buf;
			exit(EXIT_FAILURE);
		}

		// Use the shader program
		glUseProgram(m_id);
	}
}

Shader::Shader(string path){
	if (path.empty())
		printError("Shader(path)", "Path string is empty.");
	else {
		// Initialize m_isProg = false because this constructor only compiles a shader file
		m_isProg = false;

		// Loading shader file
		ifstream file(path);

		// Checking if file exists
		if (!file) {
			cerr << "Shader Error:\n" << path << "  not existent.\n";
			exit(EXIT_FAILURE);
		}

		// Getting size of file
		GLint fileSize;
		file.seekg(0, file.end);
		fileSize = GLint(file.tellg());
		file.seekg(0, file.beg);

		// Writing the shader file content into a char array
		GLchar* buf = new GLchar[fileSize]();
		file.read(buf, fileSize);

		// Getting type of shader
		string type = path.substr(path.size() - 4, 4);

		// Generate shader id based on the type (e.g: vertex shader, fragment shader etc.)
		if (type == ".vsh")
			m_id = glCreateShader(GL_VERTEX_SHADER);
		else if (type == ".fsh")
			m_id = glCreateShader(GL_FRAGMENT_SHADER);
		else {
			cerr << "Shader Error:\n" << path << "\nWrong file extension.\n\n";
			exit(EXIT_FAILURE);
		}

		// Uploads shader source code to OpenGL
		glShaderSource(m_id, 1, (const GLchar**)&buf, (const GLint*)&fileSize);

		// Compiling shader with OpenGL
		glCompileShader(m_id);

		// Free unused memory
		delete[] buf;

		// Retrieve and print compilation errors.
		GLint status, len;
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);

		// If shader did not compiled successfully ..
		if (status != GL_TRUE) {
			glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &len);
			GLchar* buf = new GLchar[len];
			glGetShaderInfoLog(m_id, len, nullptr, buf);
			cerr << buf;
			delete[] buf;
			exit(EXIT_FAILURE);
		}
	}
}

Shader::~Shader(){
	if (m_isProg)
		glDeleteProgram(m_id);
	else
		glDeleteShader(m_id);
}
