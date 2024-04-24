#include "block.h"

/**
* \brief Shader program for a block object
*/
Shader* guiShader = nullptr;

/**
* \brief Location id for the 'useTexture' uniform in the gui shader program. Tells
* whether a texture for this block is used or not.
*/
GLuint useTexLoc;

/**
* \brief Location id for color uniform in the gui shader program.
*/
GLuint colorLoc;

/**
* \brief Location id for the brightness uniform in the gui shader program.
*/
GLuint brightnessLoc;

/**
* \brief Width and Height of the main window. Used to let the button size the same after window resize.
*/
float startWidth, startHeight;

/**
* \brief Deletes the shader program.
*/
void quit() {
	delete guiShader;
}

/**
* \brief Initializes the shader program and stores the width and height of the SDL window.
* Also gets the location of all uniforms in the gui shader program and stores them into
* the global defines variables.
*/
void init() {
	checkGLError("'block.cpp' init() -> Error occured before this call");

	guiShader = new Shader{ "shader/gui.vsh", "shader/gui.fsh" };
	startWidth = float(getMainWindow()->getWidth()); 
	startHeight = float(getMainWindow()->getHeight());

	useTexLoc = glGetUniformLocation(guiShader->getId(), "useTexture");
	colorLoc = glGetUniformLocation(guiShader->getId(), "color");
	brightnessLoc = glGetUniformLocation(guiShader->getId(), "brightness");

	atexit(quit);

	checkGLError("'block.cpp' init()");
}

Block::Block(int x, int y, unsigned w, unsigned h) : m_x(x), m_y(y), m_width(w), m_height(h) {
	if (x < 0 || y < 0)
		printCriticalError("Block(x, y, w, h)", "x or y must not be less than 0");
	else {
		// Init texture with null
		m_tex = nullptr;

		// Initialize the shader program once after program start
		if (!guiShader)
			init();

		// Use the block shader program
		guiShader->use();

		// Initialize text content
		m_textcontent = L"";

		// Normalize upper left corner and lower right corner coordinate (between 0.0 and 1.0) for easier calculation of vertexdata
		m_edges[0] = vec2((float(m_x) / startWidth), (float(m_y) / startHeight));
		m_edges[1] = vec2((float(m_x + w) / startWidth), (float(m_y + h) / startHeight));

		// Create buffer object with 8 float values (4 vertices with x and y)
		m_vbuf = new Buffer<vec2>(4, GL_STATIC_DRAW);

		// Tell the buffer how the vertex data should be read
		m_vbuf->attrib(guiShader->getId(), "position", 2, 1, 0);

		// Calculate the 4 corners of the block and transform their position to OpenGL coordinates (from -1.0 to 1.0).
		calculateVertices();

		// Setting default color
		color(0.25f, 0.25f, 0.25f, 1.0f);
	}
}

void Block::calculateVertices(){
    // Factors to preserve the button size after resize
	float scaleX = startWidth / getMainWindow()->getWidth();
	float scaleY = startHeight / getMainWindow()->getHeight();

    // Array for the vertex data
	vector<vec2> vertexData;

	// Reserve 8 float values (4 vertices with x and y)
	vertexData.reserve(4);

    // Calculate the vertex data and transform it to OpenGL coordinates
	vertexData.push_back(vec2((m_edges[0].x * scaleX)*2.0f - 1.0f, -(m_edges[0].y * scaleY)*2.0f + 1.0f));
	vertexData.push_back(vec2((m_edges[0].x * scaleX)*2.0f - 1.0f, -(m_edges[1].y * scaleY)*2.0f + 1.0f));
	vertexData.push_back(vec2((m_edges[1].x * scaleX)*2.0f - 1.0f, -(m_edges[0].y * scaleY)*2.0f + 1.0f));
	vertexData.push_back(vec2((m_edges[1].x * scaleX)*2.0f - 1.0f, -(m_edges[1].y * scaleY)*2.0f + 1.0f));

    // Upload the vertex data to the buffer object
	m_vbuf->upload(vertexData);
}

void Block::color(float r, float g, float b, float a){
	if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f || a < 0.0f || a > 1.0f){
		printError("Block::color(..)","Some color values are invalid. Color is set to black.");
		r = 0.0f; g = 0.0f; b = 0.0f; a = 1.0f;
	}
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
}

void Block::reorder(int posX, int posY) {
	m_edges[0] = vec2((float(posX) / startWidth), (float(posY) / startHeight));
	m_edges[1] = vec2((float(posX + m_width) / startWidth), (float(posY + m_height) / startHeight));
	calculateVertices();
}

void Block::text(wstring str, Font& ft) {
	// Error checking
	if (str.empty())
		printError("Block::text(..)", "String is empty. No text created.");
	else {
		// Copy string into member variable
		m_textcontent = str;

		// Create text object
		Text text(str, ft);

		// Check if the text fits into the block
		if (text.getWidth() > m_width)
			printError("Block::text(..)", "String does not fit into the block. No text created");
		else {
			// Allocate memory for the texture pixel-data
			vector<float> textureData;
			textureData.reserve(m_width*m_height * 4);

			// Fill texture pixel data with background color
			for (unsigned i = 0; i < m_width*m_height; i++)
				for (unsigned j = 0; j < 4; j++)
					textureData.push_back(m_rgba[j]);

			// Calculate offset to center the text within the block
			int x_off = int(((m_width - text.getWidth()) / 2.0f));
			int y_off = int(((m_height - text.getHeight()) / 2.0f))-1;

			// Write the pixel data of the text into the pixel data of the texture
			// Only write if the text fits into the block
			if (text.getWidth() <= m_width) {
				for (unsigned y = 0; y < text.getHeight(); y++) {
					for (unsigned x = 0; x < text.getWidth(); x++) {

						// p is the grey scale color of one pixel of the antialised text
						unsigned char p = text.getPixmap()[x + y*text.getWidth()];

						// If the grey color of the pixel is not pure black
						if (p > 0) {
							float textColor = float(p) / 255.0f;

							// Then write the the pixel data into the texture data
							for (int i = 0; i < 4; ++i)

								// Consider that, the darker a pixel is the more transparent it will be.
								// Then overwrite the background color pixel-data.
								textureData[((x + x_off) + (y + y_off)*m_width) * 4 + i] += m_rgba[i] + (1.0f - m_rgba[i])*textColor;
						}
					}
				}
			}

			// If texture already exists, overwrite the texture data
			if (m_tex)
				m_tex->sub(textureData.data(), GL_RGBA, GL_FLOAT, m_width, m_height);
			else {
				checkGLError("Block::text(..) -> Error occured before this call");
				// Otherwise create a new texture
				m_tex = new Texture(textureData.data(), m_width, m_height, GL_RGBA, GL_NONE, GL_LINEAR, 4);
				// Attach texture to a unit in the gui shader program
				glUniform1i(glGetUniformLocation(guiShader->getId(), "tex"), m_tex->getUnit());
				checkGLError("Block::text(..) -> Attach texture to texture unit in the gui shader");
			}
		}
	}
}

Block::~Block(){
	delete m_vbuf;
	if (m_tex)
		delete m_tex;
}

/**
 * \brief Getter for the shader program
 */
const Shader* getGuiShader(){
    return guiShader;
}

/**
* \brief Getter for the 'useTexture' uniform location id that was set once in the block.cpp file
*/
const GLuint getUseTexLoc() {
	return useTexLoc;
}

/**
* \brief Getter for the 'color' uniform location id that was set once in the block.cpp file
*/
const GLuint getColorLoc() {
	return colorLoc;
}

/**
* \brief Getter for the 'brightness' uniform location id that was set once in the block.cpp file
*/
const GLuint getBrightnessLoc() {
	return brightnessLoc;
}

