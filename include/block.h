#pragma once

#include <vector>
#include "error.h"
#include "texture.h"
#include "buffer.h"
#include "window.h"
#include "ft2font.h"
#include "shader.h"

/**
* \brief The base class for all used gui-elements.
*
* Holds all methods and variables, that are needed by the sub classes.
* A block is always in a shape of a rectangle. A Vertex buffer object 
* is used to store it's geometry data. There is one "Gui" - Shader program
* defined as a global in the "block.cpp" file to paint the blocks.
*/
class Block {
public:
	/**
	* \brief Defines the position and it's width and height.
	*
	* \param[in] posX Position on x-axis relative to the SDL window.
	*                 Defines the top left corner of the block.
	*
	* \param[in] posY Position on y-axis relative to the SDL window.
	*                 Defines the top left corner of the block.
	*
	* \param[in] width Width of the block in the left direction.
	*
	* \param[in] height Height of the block in the down direction.
	*/
	Block(int posX, int posY, unsigned width, unsigned height);

	/**
	* \brief Virtual destructor. Can be overloaded in sub classes.
	*/
	virtual ~Block();

	/**
	* \brief Sets the background color of the block.
	*
	* \param[in] r Red value of the color.
	*
	* \param[in] g Green value of the color.
	*
	* \param[in] b Blue value of the color.
	*
	* \param[in] a Alpha value of the color.
	*/
	void color(float r, float g, float b, float a);

	/**
	* \brief Sets a new position for the top left corner of the block.
	*
	* \param[in] posX New X-position.
	*
	* \param[in] posY New Y-position.
	*/
	void reorder(int posX, int posY);

	/**
	* \brief Creates a text in form of a texture, that will be displayed in the center
	* of the block. Can be overloaded in sub classes.
	*
	* \param[in] str String that will be displayed in the block.
	*
	* \param[in] f Font for the text.
	*/
	virtual void text(wstring str, Font& f);

	/**
	* \brief The update function, which draws the block on the current context.
	* Needs to be overloaded by subclasses.
	*/
	virtual void update() = 0;

	/**
	* \brief Getter for the top left corner position on x-axis.
	*/
	int getStartX()const { return m_x; }

	/**
	* \brief Getter for the top left corner position on y-axis.
	*/
	int getStartY()const { return m_y; }

	/**
	* \brief Getter for block's width.
	*/
	unsigned getWidth()const { return m_width; }

	/**
	* \brief Getter for the block's height.
	*/
	unsigned getHeight()const { return m_height; }

	/**
	* \brief Getter for the text string in the block
	*/
	wstring getText()const { return m_textcontent; }

protected:
	/**
	* \brief Calculates the vertices and transforms them into OpenGL coordinates.
	*/
	void calculateVertices();

	/**
	* \brief Top left corner position on x-axis.
	*/
	int m_x;

	/**
	* \brief Top left corner position on y-axis.
	*/
	int m_y;

	/**
	* \brief Width of the block.
	*/
	unsigned m_width;

	/**
	* \brief Height of the block.
	*/
	unsigned m_height;

	/**
	* \brief Top left and bottom right corner of the block
	*/
	vec2 m_edges[2];

	/**
	* \brief Array for the rgba color values.
	*/
	float m_rgba[4];

	/**
	* \brief Vertex buffer object for the block.
	*/
	Buffer<vec2>* m_vbuf;

	/**
	* \brief Texture for the block.
	*/
	Texture* m_tex;

	/**
	* \brief String for the text.
	*/
	wstring m_textcontent;
};

/**
* \brief Getter for the shader program.
*/
const Shader* getGuiShader();

/**
* \brief Getter for the 'useTexture' uniform location id that was set once in the block.cpp file
*/
const GLuint getUseTexLoc();

/**
* \brief Getter for the 'color' uniform location id that was set once in the block.cpp file
*/
const GLuint getColorLoc();

/**
* \brief Getter for the 'brightness' uniform location id that was set once in the block.cpp file
*/
const GLuint getBrightnessLoc();


