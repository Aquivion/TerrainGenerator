#include "terrain.h"

Terrain::Terrain(Noise &n, float sW, float sD, unsigned vD, unsigned nmD, const GLuint progId)
	: m_surfaceWidth(sW), m_surfaceDepth(sD), m_min(0), m_max(0), m_vertexDetail(vD),
	  m_vpr(128*vD), m_vpc(128*vD), m_programId(progId)
{
	if (sW <= 0.0f || sD <= 0.0f)
		printCriticalError("Terrain(..)", "Width or depth of the surface is less then or equal to 0.0");
	else if (vD == 0)
		printCriticalError("Terrain(..)", "Vertex detail must be 1 or higher");
	else if (nmD == 0)
		printCriticalError("Terrain(..)", "Normal map detail must be 1 or higher");
	else {
		// Initialize normal map detail member
		m_normalMapDetail = nmD;
		m_normalMapWidth = 256 * nmD;
		m_normalMapHeight = 256 * nmD;

		// Allocate memory
		m_elements = new vector<GLuint>();
		m_vertices = new vector<Vertex>();
		m_normalMap = new vector<vec3>();
		m_seamlessMap = new vector<vec3>();

		// Copy noise reference
		m_noise = &n;

		// Initialize other member
		m_brightness = 1.0f;

		// Calculate vertices and element list.
		calculateVertices();
		calculateElements();

		// Set and upload terrain brightness
		setBrightness(1.0f);

		// Upload that the seamless noise texture is shown at start
		setSeamlessTexEnabled(true);
	}
}

Terrain::~Terrain()
{
	m_vertices->clear();
	delete m_vertices;
	m_elements->clear();
	delete m_elements;
	m_seamlessMap->clear();
	delete m_seamlessMap;
	m_normalMap->clear();
	delete m_normalMap;
}

void Terrain::calculateVertices(){
	// Freeing and / or allocating memory
	if (m_vertices->size() > 0) {
		m_vertices->clear();
		vector<Vertex> s;
		m_vertices->swap(s);
	}
	m_vertices->resize(m_vpr * m_vpc);

	// Declaration
	m_min = m_noise->n2_layered(0, 0);
	m_max = m_noise->n2_layered(0, 0);
	float addWidth = m_surfaceWidth / float(m_vpr);
	float subDepth = m_surfaceDepth / float(m_vpc);
	float surfaceMidX = m_surfaceWidth / 2.0f;
	float surfaceMidZ = m_surfaceDepth / 2.0f;
	float textureCoordAddX = 1.0f / (m_vpr - 1.0f);
	float textureCoordAddY = 1.0f / (m_vpc - 1.0f);

	// Calculate Positions
	int index = 0;
	for (unsigned int z = 0; z < m_vpc; z++){
		for (unsigned int x = 0; x < m_vpr; x++){

			// Calculate noiseValue
			float noiseValue = (m_noise->n2_layered(x * addWidth, z * subDepth));

			// Vertex positions and texture coordinate
			m_vertices->at(index).pos = vec3(x * addWidth - surfaceMidX, noiseValue, z * (-subDepth) + surfaceMidZ);
            m_vertices->at(index).texCoord = vec2(x*textureCoordAddX, z*textureCoordAddY);

			// Calculate minimum and maximum noise value
			if (noiseValue > m_max)
				m_max = noiseValue;
			if (noiseValue < m_min)
				m_min = noiseValue;

			// Increase index
            index++;
		}
	}

	// Recalculate height values so that the terrain's lowest point is always y = 0;
	index = 0;
	for (unsigned int z = 0; z < m_vpc; z++) {
		for (unsigned int x = 0; x < m_vpr; x++) {
			m_vertices->at(index).pos.y -= m_min;
			index++;
		}
	}

	checkGLError("Terrain::calculateVertices(..) -> Upload new highpoint. Error occured before this call.");
	const GLuint maxLoc = glGetUniformLocation(m_programId, "max");
	glUniform1f(maxLoc, m_max - m_min);
	checkGLError("Terrain::calculateVertices(..) -> Upload new highpoint");
}

void Terrain::calculateNormalMap(){
	// Memory allcoation for noise values
	vector<float> noise_values;
	noise_values.reserve(m_normalMapWidth * m_normalMapHeight);

	// Freeing normal map and / or allocating memory for it
	if (m_normalMap->size() > 0) {
		m_normalMap->clear();
		vector<vec3> s;
		m_normalMap->swap(s);
	}
    m_normalMap->reserve(m_normalMapWidth * m_normalMapHeight);

	float addWidth = m_surfaceWidth / m_vpr;
	float subDepth = m_surfaceDepth / m_vpc;

	// Both sides minus 1 because of we're running from 0-(max-1)
	float widthDivisor = float(m_normalMapWidth - 1) / (m_vpr - 1);
	float heightDivisor = float(m_normalMapHeight - 1) / (m_vpc - 1);

	if (m_normalMapWidth != m_vpr || m_normalMapHeight != m_vpc) {
		for (unsigned int y = 0; y < m_normalMapHeight; y++)
			for (unsigned int x = 0; x < m_normalMapWidth; x++)
				noise_values.push_back((m_noise->n2_layered((x / widthDivisor)*addWidth, (y / heightDivisor) *subDepth)));
	}
	else {
		int index = 0;
		for (unsigned int y = 0; y < m_normalMapHeight; y++){
			for (unsigned int x = 0; x < m_normalMapWidth; x++) {
				noise_values.push_back(m_vertices->at(index).pos.y);
				index++;
			}
		}
	}

	// Calculate Normals
	int index = 0;
	bool left_tmp, bottom_tmp, right_tmp, top_tmp;
	vec3 left(0.0,0.0,0.0), bottom(0.0, 0.0, 0.0), right(0.0, 0.0, 0.0), top(0.0, 0.0, 0.0);
	for (unsigned int z = 0; z < m_normalMapHeight; z++){
		for (unsigned int x = 0; x < m_normalMapWidth; x++){
			left_tmp = true; bottom_tmp = true; right_tmp = true; top_tmp = true;
			vec3 current = { (x / widthDivisor)*addWidth, noise_values[index], (z / heightDivisor)*subDepth };
			vec3 n1(0.0, 0.0, 0.0), n2(0.0, 0.0, 0.0), n3(0.0, 0.0, 0.0), n4(0.0, 0.0, 0.0);

			// Get all 4 surrounding vertices
			if (x > 0)
				left = vec3(((x - 1) / widthDivisor)* addWidth, noise_values[index - 1], (z / heightDivisor)*(subDepth));
			else
				left_tmp = false;
			if (z > 0)
				bottom = vec3((x / widthDivisor)*addWidth, noise_values[index - m_normalMapWidth], ((z - 1) / heightDivisor)*(subDepth));
			else
				bottom_tmp = false;
			if (x < (m_normalMapWidth - 1))
				right = vec3(((x + 1) / widthDivisor)*addWidth, noise_values[index + 1], (z / heightDivisor)*(subDepth));
			else
				right_tmp = false;
			if (z < (m_normalMapHeight - 1))
				top = vec3((x / widthDivisor)*addWidth, noise_values[index + m_normalMapWidth], ((z + 1) / heightDivisor)*(subDepth));
			else
				top_tmp = false;

			if (left_tmp && bottom_tmp)
				n1 = -cross(left - current, bottom - current);
			if (bottom_tmp && right_tmp)
				n2 = -cross(bottom - current, right - current);
			if (right_tmp && top_tmp)
				n3 = -cross(right - current, top - current);
			if (top_tmp && left_tmp)
				n4 = -cross(top - current, left - current);

			// Getting the average normal
			vec3 normal = normalize(n1 + n2 + n3 + n4);

			// Pushing normal
			m_normalMap->push_back(normal);

			// Increase index
			index++;
		}
	}
}

void Terrain::calculateSeamlessMap(const Noise &n, unsigned resolution){
	// See comments in function "calculateNormalMap" as reference for this code block
	vector<float> noise_values;
	noise_values.reserve(resolution * resolution);

	if (m_seamlessMap->size() > 0) {
		m_seamlessMap->clear();
		vector<vec3> s;
		m_seamlessMap->swap(s);
	}
    m_seamlessMap->reserve(resolution * resolution);

	for (unsigned int y = 0; y < resolution; y++)
		for (unsigned int x = 0; x < resolution; x++)
			noise_values.push_back(n.n2_seamless_layered(float(x), float(y)));

	// Calculate Normals
	int index = 0;
	bool left_tmp, bottom_tmp, right_tmp, top_tmp;
	vec3 left(0.0, 0.0, 0.0), bottom(0.0, 0.0, 0.0), right(0.0, 0.0, 0.0), top(0.0, 0.0, 0.0);
	for (unsigned int z = 0; z < resolution; z++) {
		for (unsigned int x = 0; x < resolution; x++) {
			left_tmp = true; bottom_tmp = true; right_tmp = true; top_tmp = true;
			vec3 current = { x, noise_values[index], z };
			vec3 n1(0.0, 0.0, 0.0), n2(0.0, 0.0, 0.0), n3(0.0, 0.0, 0.0), n4(0.0, 0.0, 0.0);

			if (x > 0)
				left = vec3((x - 1), noise_values[index - 1], z);
			else
				left_tmp = false;
			if (z > 0)
				bottom = vec3(x, noise_values[index - resolution], (z - 1));
			else
				bottom_tmp = false;
			if (x < (resolution - 1))
				right = vec3((x + 1), noise_values[index + 1], z);
			else
				right_tmp = false;
			if (z < (resolution - 1))
				top = vec3(x, noise_values[index + resolution], (z + 1));
			else
				top_tmp = false;

			if (left_tmp && bottom_tmp)
				n1 = -cross(left - current, bottom - current);
			if (bottom_tmp && right_tmp)
				n2 = -cross(bottom - current, right - current);
			if (right_tmp && top_tmp)
				n3 = -cross(right - current, top - current);
			if (top_tmp && left_tmp)
				n4 = -cross(top - current, left - current);

			vec3 normal = normalize(n1 + n2 + n3 + n4);
			m_seamlessMap->push_back(normal);

			index++;
		}
	}
}

void Terrain::calculateElements(){
	// Calculation of the number of elements needed for the vertices
    int elementSize = ((2 * m_vpr) * (m_vpc - 2)) + (2 * m_vpr) - (m_vpc - 2);

	// Memory de-/allcotion
	if (m_elements->size() > 0) {
		m_elements->clear();
		vector<GLuint> s;
		m_elements->swap(s);
	}
    m_elements->reserve(elementSize);

	// Calculate actual elements
	GLuint bot = 0;
	GLuint top = m_vpr;
	for (unsigned int y = 0; y < m_vpc - 1; y++){
		for (unsigned int x = 0; x < m_vpr; x++){
			m_elements->push_back(bot++);
			m_elements->push_back(top++);
		}
		m_elements->push_back(~0);
	}
	m_elementsSize = elementSize;
}

void Terrain::applyTexture(const char* samplerName, GLint texUnit)const{
	checkGLError("Terrain::applyTexture(..) -> Error occured before this call.");
	glUniform1i(glGetUniformLocation(m_programId, samplerName), texUnit);
	checkGLError("Terrain::applyTexture(..)");
}

void Terrain::draw()const{
    glCullFace(GL_FRONT);
    glDrawElements(GL_TRIANGLE_STRIP, m_elementsSize, GL_UNSIGNED_INT, nullptr);
}

void Terrain::setNoise(int seed, unsigned lC, float sF, float fF, float wD, float a){
    m_noise->setNewSeed(seed);
    m_noise->setLayerCount(lC);
    m_noise->setStartFrequency(sF);
    m_noise->setFrequencyFactor(fF);
    m_noise->setWeightDivisor(wD);
    m_noise->setAmplitude(a);
}


void Terrain::setBrightness(float brightness){
	checkGLError("Terrain::setBrightness(..) -> Error occured before this call.");
	m_brightness = brightness;
	static GLuint bightnessLoc = glGetUniformLocation(m_programId, "brightness");
	glUniform1f(bightnessLoc, m_brightness);
	checkGLError("Terrain::setBrightness(..)");
}

void Terrain::setSeamlessTexEnabled(bool enabled){
	checkGLError("Terrain::setSeamlessTexEnabled(..) -> Error occured before this call.");
	m_seamlessTexEnabled = enabled;
	static GLuint seamlessEnabledLoc = glGetUniformLocation(m_programId, "seamlessTexEnabled");
	glUniform1ui(seamlessEnabledLoc, m_seamlessTexEnabled);
	checkGLError("Terrain::setSeamlessTexEnabled(..)");
}

const vector<vec3>&
Terrain::getNormalMap(unsigned detail){
	if (detail == 0)
		printCriticalError("Terrain::getNormalMap(..)", "Detail must be 1 or higher.");
	else {
		m_normalMapDetail = detail;
		m_normalMapWidth = 256 * m_normalMapDetail;
		m_normalMapHeight = 256 * m_normalMapDetail;
		calculateNormalMap();
	}
	return *m_normalMap;
}

const vector<vec3>&
Terrain::getSeamlessMap(const Noise &n, unsigned resolution){
	if (resolution == 0)
		printCriticalError("Terrain::getSeamlessMap(..)", "Resolution cannot be 0.");
	else
		calculateSeamlessMap(n, resolution);
	return *m_seamlessMap;
}


