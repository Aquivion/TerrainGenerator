#pragma once

#include "error.h"
#include <GL/glew.h>
#include <vector>

/**
* \brief This class is used to create vertex and element buffer objects with OpenGL.
*
* It is designed as a template class to use it for different types. Therefore the whole class
* is implemented in the header only. It supports to overwrite the buffer data in case the
* vertex data shall be changed.
*/
template<typename T> class Buffer {
public:
	/**
	* \brief Constructor for creating an empty vertex buffer object, to fill it afterwards.
	*
	* \param[in] size number of elements.
	*
	* \param[in] usage defines how the buffer data shall be used.
	*/
	Buffer(GLuint size, GLenum usage)
		:m_stride(sizeof(T)), m_vertexCount(size), m_eboActive(false)
	{
		if (size == 0)
			printCriticalError("Buffer(size, vertexSize, usage)", "Size equals zero.");
		else {
			// Check if any OpenGL errors happend before this code block is called. Used to clear the error buffer.
			checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before this call");

			// Create vertex array object name and store it in a member variable
			glGenVertexArrays(1, &m_vaoId);

			// Bind vertex array object
			glBindVertexArray(m_vaoId);

			// Generate buffer object name and store it in member variable
			glGenBuffers(1, &m_vboId);

			// Bind Buffer to the GL_ARRAY_BUFFER target
			glBindBuffer(GL_ARRAY_BUFFER, m_vboId);

			// Upload buffer data
			glBufferData(GL_ARRAY_BUFFER, size*m_stride, nullptr, usage);

			// Checking for OpenGL errors
			checkGLError("Buffer(size, vertexSize, usage) -> Creating VBO " + to_string(m_vboId));
		}
	}

	/**
	* \brief Constructor for creating a vertex buffer object with a defined size
	*
	* \param[in] vertices vector containing the buffer data
	*
	* \param[in] usage defines how the buffer data shall be used
	*/
	Buffer(const vector<T>& vertices, GLenum usage)
		:m_stride(sizeof(T)), m_vertexCount(vertices.size()), m_eboActive(false)
	{
		if (vertices.empty())
			printCriticalError("Buffer(vertices, vertexSize, usage)", "Vertex list is empty");
		else {
			// Check if any OpenGL errors happend before this code block is called. Used to clear the error buffer.
			checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before this call");

			// See comments in the first constructor as reference for this code block
			glGenVertexArrays(1, &m_vaoId);
			glBindVertexArray(m_vaoId);

			glGenBuffers(1, &m_vboId);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*m_stride, vertices.data(), usage);
			checkGLError("Buffer(vertices, vertexSize, usage) -> Creating VBO " + to_string(m_vboId));
		}
	}

	/**
	* \brief Constructor for creating a vertex buffer and a element buffer object.
	*        This is used for vertices which are indexed, to draw Triangle with Triangle Strip.
	*
	* \param[in] vertices vector containing buffer data
	*
	* \param[in] elements vector for the elements which are the indexes for the vertices.
	*
	* \param[in] usage defines how the buffer data shall be used
	*/
	Buffer(const vector<T>& vertices, const vector<GLuint>& elements, GLenum usage)
		:m_stride(sizeof(T)), m_vertexCount(vertices.size()), m_eboActive(true)
	{
		if (vertices.empty())
			printCriticalError("Buffer(vertices, elements, vertexSize, usage)", "Vertex list is empty");
		else if(elements.empty())
			printCriticalError("Buffer(vertices, elements, vertexSize, usage)", "Element list is empty");
		else {
			// Check if any OpenGL errors happend before this code block is called. Used to clear the error buffer.
			checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before this call");

			// See comments in the first constructor as reference for this code block
			glGenVertexArrays(1, &m_vaoId);
			glBindVertexArray(m_vaoId);

			glGenBuffers(1, &m_vboId);
			glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*m_stride, vertices.data(), usage);
			checkGLError("Buffer(vertices, elements, vertexSize, usage) -> Creating VBO " + to_string(m_vboId));

			// Same procedure with the element buffer object
			glGenBuffers(1, &m_eboId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLuint), elements.data(), usage);
			checkGLError("Buffer(vertices, elements, vertexSize, usage) -> Creating EBO " + to_string(m_eboId));
			m_eboActive = true;
		}
	}

	/**
	* \brief When this function is called, an attribute is added to the VertexAttribArray.
	*        This tells OpenGL how a Vertex shall be read. (e.g vertex position, normal, texture coordinate)
	*
	* \param[in] shaderId id of the shader the attribute is uploaded to.
	*
	* \param[in] name Name of the attribute in the given shader
	*
	* \param[in] attElementCount	Number of floats in the type T. for example an attribute for a vertex position
	* in 3D got 3 floats so the size is 3.
	*
	* \param[in] attCount Number of attributes for one vertex. If it is 1 then the stride parameter at 
	* glVertexAttribPointer is 0.
	*
	* \param[in] offset	offset of the attribute. for example, if there is already a vertex position attribute
	* with 3 floats the offset is 3.
	*/
	void attrib(GLuint shaderId, const GLchar* name, GLint attElementCount, GLint attCount, GLint offset) {
		if (name == nullptr)
			printError("Buffer::attrib(..) VBO Id " + to_string(m_vboId), "Given parameter 'name' points is null. No attribute is being set.");
		else if (name[0] == '\0')
			printError("Buffer::attrib(..) VBO Id " + to_string(m_vboId), "Given parameter 'name' is empty. No attribute is being set.");
		else if (attElementCount < 0 || offset < 0)
			printError("Buffer::attrib(..) VBO Id " + to_string(m_vboId), "Given parameter 'size' or 'offset' is less than zero. No attribute is being set.");
		else {
			// Check if any OpenGL errors happend before this code block is called. Used to clear the error buffer.
			checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before this call");

			// Set active buffer object
			use();

			// Getting location of the input variable for that attrib in the shader program
			GLint att = glGetAttribLocation(shaderId, name);

			// Telling OpenGL how the attribute is laid out in the vertex array
			unsigned stride_tmp = m_stride;
			if (attCount == 1)
				stride_tmp = 0;
			glVertexAttribPointer(att, attElementCount, GL_FLOAT, GL_FALSE, stride_tmp, (const void*)(offset*sizeof(GLfloat)));

			// Vertex attribute array need to be enabled
			glEnableVertexAttribArray(att);

			checkGLError("Buffer::attrib(..) VBO Id " + to_string(m_vboId));
		}
	}

	/**
	* \brief Overwrites the current vertex buffer with the given vertices.
	*
	* \param[in] vertices The new vertex data.
	*/
	void upload(const vector<T>& vertices) {
		if (vertices.empty())
			printError("Buffer::upload(vertices) VBO Id " + to_string(m_vboId), "Vertex list is empty. No upload made.");
		else {
			// Check if any OpenGL errors happend before this code block is called. Used to clear the error buffer.
			checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before this call");

			// Set active buffer object
			use();

			// Overwrite data with the same size as before
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*m_stride, vertices.data());
			checkGLError("Buffer::upload(vertices) VBO Id " + to_string(m_vboId));
		}
	}

	/**
	* \brief Overwrites the current vertex and element buffer with the given vertex and element data.
	*		 Only used, if the new data has the same size as the old.
	*
	* \param[in] vertices The new vertex data.
	*
	* \param[in] elements The new element data.
	*/
	void upload(const vector<T>& vertices, const vector<GLuint>& elements) {
		if (vertices.empty())
			printError("Buffer::upload(vertices, elements) VBO Id " + to_string(m_vboId), "Vertex list is empty. No upload made.");
		else if (elements.empty())
			printError("Buffer::upload(vertices, elements) VBO Id " + to_string(m_vboId), "Element list is empty. No upload made.");
		else if (m_eboActive == false)
			printError("Buffer::upload(vertices, elements) VBO Id " + to_string(m_vboId), "Buffer object does not have an element buffer. No upload made.");
		else {
			// Check if any OpenGL errors happend before this code block is called. Used to clear the error buffer.
			checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before this call");

			use();
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*m_stride, vertices.data());
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, elements.size()*sizeof(GLuint), elements.data());
			checkGLError("Buffer::upload(vertices, elements) VBO Id:" + to_string(m_vboId));
		}
	}

	/**
	* \brief Creates a new buffer with the new size, defined by the given vertex array.
	*		 This is used, if the data size differ.
	*
	* \param[in] vertices vector containing buffer data
	*
	* \param[in] elements vector for the elements which are the indexes for the vertices.
	*
	* \param[in] usage defines how the buffer data shall be used
	*/
	void upload(const vector<T>& vertices, const vector<GLuint>& elements, GLenum usage) {
		if (vertices.empty())
			printError("Buffer::upload(vertices, elements, vertexSize, usage)", "Vertex list is empty. No upload made.");
		else if (elements.empty())
			printError("Buffer::upload(vertices, elements, vertexSize, usage)", "Element list is empty. No upload made.");
		else {
			checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before call");

			// Set active buffer object
			use();

			// Set new vertex count because this function is called, when the vertex count differs
			m_vertexCount = vertices.size();

			// Call glBufferData again to create a buffer with a new size
			glBufferData(GL_ARRAY_BUFFER, vertices.size()*m_stride, vertices.data(), usage);
			checkGLError("Buffer::upload(vertices, elements, vertexSize, usage) -> Upload new VBO: " + to_string(m_vboId));
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(GLuint), elements.data(), usage);
			checkGLError("Buffer::upload(vertices, elements, vertexSize, usage) -> Upload new EBO: " + to_string(m_vboId));

			m_eboActive = true;
		}
	}

	/**
	* \brief Binds vertex or element buffer and uses them.
	*/
	void use() {
		glBindVertexArray(m_vaoId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
		if (m_eboActive)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboId);
	}

	/**
	* \brief Deletes vertex buffer object and the possible element buffer.
	*/
	~Buffer() {
		// Check if any OpenGL errors happened before this code block is called. Used to clear the error buffer.
		checkGLError("Buffer(size, vertexSize, usage) -> Error occurred before this call");

		glDeleteBuffers(1, &m_vboId);
		checkGLError("Buffer::~Buffer VBO Id " + to_string(m_vboId));
		if (m_eboActive) {
			glDeleteBuffers(1, &m_eboId);
			checkGLError("Buffer::~Buffer EBO Id " + to_string(m_vboId));
		}
	}

	/**
	* \brief Getter for the vertex count.
	*/
	unsigned getVertexCount()const { return m_vertexCount; }

private:
	/**
	* \brief Id for the vertex array object. This array stores the links between all 
	* created attributes and their belonged VBO. Then there is no need of creating new
	* attributes with glVertexAttribPointer function, when you want to use a buffer.
	* Instead you just need to bind the vertex array object again.
	*/
	GLuint m_vaoId;

	/**
	* \brief Id for the vertex buffer object
	*/
	GLuint m_vboId;

	/**
	* \brief Id for the element buffer object
	*/
	GLuint m_eboId;

	/**
	* \brief Length of a vertex in bytes.
	*/
	unsigned m_stride;

	/**
	* \brief Number of vertices stored in the buffer.
	*/
	unsigned m_vertexCount;

	/**
	* \brief Boolean that tells, when an element buffer is used.
	*/
	bool m_eboActive;
};
