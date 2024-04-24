#pragma once

#include <chrono>
#include <vector>
#include <GL/glew.h>

#include "noise.h"
#include "glm.h"
#include "texture.h"
#include "error.h"

/**
* \brief Vertex class. 
*
* Just holds a 3D vector for the position and a 2D vector for the texture coordinate.
*/
struct Vertex{
	vec3 pos;
	vec2 texCoord;
};

/**
* \brief The Terrain class. 
*
* Holds all information about the terrain and all methods to calculate it's vertex
* data and it's normal map and also a method to generate pixel data for a seamless 
* noise texture. The vertices are drawn in "Triangle strip" mode so that an index
* list is also generated with the creation of an object of this class. A Vertex
* does not contain a normal. This is because the normal data is stored in a texture 
* that is applied to the terrain, which is a lot better for performance and allows
* higher detailed lighting effects.
*/
class Terrain
{
public:
	/**
	* \brief Constructs the Perlin Noise terrain with the given size and detail. The mid-
	* point of it is always at position (0.0, 0.0, 0.0)
	*
	* \param[in] noise Pointer to the noise object which is used to calculate the height
	* of each vertex.
	*
	* \param[in] surfaceWidth Width of the terrain.
	*
	* \param[in] surfaceDepth Depth of the terrain.
	*
	* \param[in] vertexDetail Determines the total vertex count. The vertex count in width
	* and height is calculated with (128*vertexDetail).
	*
	* \param[in] normalMapDetail Determines the pixel resoultion of the normal map texture.
	* Pixel in width and height are calculated with (256*normalMapDetail).
	*
	* \param[in] programId Id of the shader program for the terrain.
	*/
	Terrain(Noise &noise, float surfaceWidth, float surfaceDepth, unsigned vertexDetail, unsigned normalMapDetail, const GLuint programId);

	/**
	* \brief Destructor. Deletes the vertex-, element-list and the normal and seamless map data.
	*/
	~Terrain();

	/**
	* \brief Calculates the vertex positions of the terrain based on the given noise object.
	* Also uploads the calculated, highest Y-value of the terrain to the shader program.
	*/
	void calculateVertices();

	/**
	* \brief Calculates the indices of the vertices for drawing with triangle strip, 
	* based on the vertex count.
	*/
	void calculateElements();

	/**
	* \brief Calculates the normal map data which is used as a texture. As said this is because
	* of performance issues.
	*/
    void calculateNormalMap();

	/**
	* \brief Calculates the seamless noise texture data based on the given noise object and
	* resolution.
	*
	* \param[in] n Reference of the noise object which is used for the seamless texture generation.
	*
	* \param[in] resolution Detail of the texture date. For example a resolution of 256 make pixel
	* count of 256*256.
	*/
	void calculateSeamlessMap(const Noise &n, unsigned resolution);

	/**
	* \brief Applies a texture to terrain by linking the texture id with a sampler in the shader 
	* program for the terrain.
	*
	* \param[in] samplerName Name of the sampler in the fragment shader file.
	*
	* \param[in] texId Id of the texture.
	*/
	void applyTexture(const char* samplerName, GLint texId)const;

	/**
	* \brief Draw the terrain in triangle strip mode with the use of the element list.
	*/
	void draw()const;

	/**
	* \brief Getter for a Reference of the vertex data.
	*/
	const vector<Vertex>& getVertices()const { return *m_vertices; }

	/**
	* \brief Getter for a Reference of the element data.
	*/
	const vector<GLuint>& getElements()const { return *m_elements; }

	/**
	* \brief Getter for the width of the surface.
	*/
	float getWidth()const { return m_surfaceWidth; }

	/**
	* \brief Getter for the depth of the surface.
	*/
	float getDepth()const { return m_surfaceDepth; }

	/**
	* \brief Getter for the highest y-value of the terrain. Is implemented as (max-min) because
	* every y-position of each vertex is substracted by min to make the lowest point always 0.
	*/
	float getMax()const { return float(m_max - m_min); }

	/**
	* \brief Getter for the brightness of the terrain.
	*/
	float getBrightness()const { return m_brightness; }

	/**
	* \brief Getter for the boolean that tells whether the seamless texture is shown or hidden.
	*/
	bool getSeamlessTexEnabled()const { return m_seamlessTexEnabled; }

	/**
	* \brief Getter for the vertex detail.
	*/
	unsigned getVertexDetail()const { return m_vertexDetail; }

	/**
	* \brief Getter for the vertex count per column.
	*/
	unsigned getVPC()const { return m_vpc; }

	/**
	* \brief Getter for the vertex count per row.
	*/
	unsigned getVPR()const { return m_vpr; }

	/**
	* \brief Getter for the shader program id of the terrain
	*/
	const GLuint getProgramId()const { return m_programId; }

	/**
	* \brief Getter for normal map detail.
	*/
	unsigned getNormalMapDetail()const { return m_normalMapDetail; }

	/**
	* \brief Getter for the width of the normalmap which should always equal 256*normalMapDetail.
	*/
	unsigned getNormalMapWidth()const { return m_normalMapWidth; }

	/**
	* \brief Getter for the height of the normalmap which should always equal 256*normalMapDetail.
	*/
	unsigned getNormalMapHeight()const { return m_normalMapHeight; }

	/**
	* \brief Returns a reference oth the terrain's noise object.
	*/
	Noise& getNoise() { return *m_noise; }

	/**
	* \brief Returns a reference to the normal map. The normal map is always being calculated new
	* because the only case when you can use the old normal map is when all parameters are the same
	* as before. Then the terrain should not be generated because the result is the same.
	*
	* \param[in] detail Detail of the normalMap. Pixel count in width and height is always 256*detail.
	*/
	const vector<vec3>& getNormalMap(unsigned detail);

	/**
	* \brief Returns a reference to the seamless texture data which is calculated based on the given
	* noise object and the given resolution.
	*
	* \param[in] n Seamless noise object for generating the texture data.
	*
	* \param[in] resolution Detail of the texture created from the returned data.
	*/
	const vector<vec3>& getSeamlessMap(const Noise &n, unsigned resolution);

	/**
	* \brief Frees the allocated memory for the normal map. This is done by switching the current 
	* vector with a new empty vector. Was implemented because after the texture
	* is uploaded the data is no longer needed.
	*/
	void freeNormalMap() { m_normalMap->clear(); vector<vec3> s; m_normalMap->swap(s); }

	/**
	* \brief Frees the allocated memory for the vertex and element list. This is done by switching 
	* the current vector with a new empty vector. Was implemented because after the data 
	* is uploaded to the buffer object, it is no longer needed.
	*/
	void freeVertices() { m_vertices->clear(); vector<Vertex> sV; m_vertices->swap(sV); 
						  m_elements->clear(); vector<GLuint> sE; m_elements->swap(sE); }

	/**
	* \brief Frees the allocated memory for the seamless texture data. This is done by switching
	* the current vector with a new empty vector. Was implemented because after the texture
	* is uploaded the data is no longer needed.
	*/
	void freeSeamlessMap() { m_seamlessMap->clear(); vector<vec3> s; m_seamlessMap->swap(s); }

	/**
	* \brief Resets the parameter of the terrain's noise object.
	*
	* \param[in] seed initializes the random engine with which the random values are being generated.
	*
	* \param[in] layerCount Number of layers that are added together.
	*
	* \param[in] startFrequency Frequency of the first layer.
	*
	* \param[in] frequencyFactor Factor with which the frequency will be multiplied after each added layer.
	*
	* \param[in] weightDivisor Divisor with which the weight of each layer will be decreased.
	*
	* \param[in] amplitude Determines the overall 'height' of the noise values.
	*/
	void setNoise(int seed, unsigned layerCount, float startFrequency, float frequencyFactor, float weightDivisor, float amplitude);

	/**
	* \brief Sets a new size for the terrain.
	*
	* \param[in] surfaceWidth New size in width.
	*
	* \param[in] surfaceDepth New size in depth.
	*/
	void setSize(float surfaceWidth, float surfaceDepth){m_surfaceWidth = surfaceWidth; m_surfaceDepth = surfaceDepth;}

	/**
	* \brief Resets the number of vertices per row and per column.
	*
	* \param[in] detail Determines the number of vertices per row and column with 128*detail.
	*/
	void setVertexDetail(unsigned detail) { m_vertexDetail = detail;  m_vpr = 128*detail; m_vpc = 128*detail; }

	/**
	* \brief Resets the detail of the normal map.
	*
	* \param[in] detail Determines the number of pixel in width and height with 256*detail.
	*/
	void setNormalMapDetail(unsigned detail) { m_normalMapDetail = detail; m_normalMapWidth = 256 * detail; m_normalMapHeight = 256 * detail; }

	/**
	* \brief Setter for the brightness of the terrain. Also uploads it to the shader program
	*
	* \param[in] brightness New brightness value.
	*/
	void setBrightness(float brightness);

	/**
	* \brief Setter for the boolean that tells whether the seamless texture is shown or hidden.
	* Alos uploads that value to the shader program.
	*
	* \param[in] enabled New bool value.
	*/
	void setSeamlessTexEnabled(bool enabled);

private:
	/**
	* \brief Width of the surface
	*/
	float m_surfaceWidth;

	/**
	* \brief Depth of the surface
	*/
	float m_surfaceDepth;

	/**
	* \brief Lowest point of the terrain. Should be 0 after recalculation.
	*/
	float m_min;

	/**
	* \brief Hight point of the terrain. Should be (max-min) after recalculation.
	*/
	float m_max;

	/**
	* \brief Terrain brightness
	*/
	float m_brightness;

	/**
	* \brief Boolean that tells whether the seamless texture is shown or hidden.
	*/
	bool m_seamlessTexEnabled;

	/**
	* \brief Detail of the vertex map.
	*/
	unsigned m_vertexDetail;

	/**
	* \brief Vertex count in a row.
	*/
	unsigned m_vpr;

	/**
	* \brief Vertex count in a column.
	*/
	unsigned m_vpc;

	/**
	* \brief Stores the id of the shader program for the terrain
	*/
	const GLuint m_programId;

	/**
	* \brief Detail of the normal map.
	*/
	unsigned m_normalMapDetail;

	/**
	* \brief Pixel of the normal in width.
	*/
	unsigned m_normalMapWidth;

	/**
	* \brief Pixel of the normal in height.
	*/
	unsigned m_normalMapHeight;

	/**
	* \brief Number of elements in the index list for the vertices.
	*/
	unsigned m_elementsSize;

	/**
	* \brief Pointer to the given noise object.
	*/
    Noise* m_noise;

	/**
	* \brief List of all vertices.
	*/
	vector<Vertex>* m_vertices;

	/**
	* \brief Element list for the vertices.
	*/
	vector<GLuint>* m_elements;

	/**
	* \brief Normal map data.
	*/
	vector<vec3> *m_normalMap;

	/**
	* \brief Seamless texture data.
	*/
	vector<vec3> *m_seamlessMap;
};

