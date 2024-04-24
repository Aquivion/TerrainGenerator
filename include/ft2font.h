#pragma once

#include <iostream>
#include <string>
#include <ft2build.h>
#include "error.h"
#include FT_FREETYPE_H

/**
* \brief Font class for loading fonts and render their glyphs with a specific size.
*/
class Font {
public:
	/**
	* \brief Default constructor.
	*/
	Font(){}

	/**
	* \brief Loads a ttf file and sets the size of the font.
	*
	* \param[in] path Path that points to the ttf file.
	*
	* \param[in] size Size of the font glyphs.
	*/
	Font(string path, FT_UInt size);

	/**
	* \brief Returns the size of the font glyphs.
	*/
	FT_UInt getSize() { return m_size; }

	/**
	* \brief Return the freetype face object, in which the glyphs are loaded one
	*        after the other.
	*/
	FT_Face getFace() { return m_face; }

	/**
	* \brief Sets the size of the font glyphs.
	*
	* \param[in] s Given size.
	*/
	void setSize(FT_UInt size);

	/**
	* \brief Deletes the face, in which the glyphs were loaded.
	*/
	~Font();

private:
	/**
	* \brief The freetype face object, in which the glyphs will be loaded one
	*        after the other.
	*/
	FT_Face m_face;

	/**
	* \brief Size of the font glyphs.
	*/
	FT_UInt m_size;
};

/**
* \brief Utility class to create lines of text in form of a pixmap.
*/
class Text {
public:
	/**
	* \brief Constructs the text and loads each letter one by one into the given
	*        Font object.
	*
	* \param[in] text String that will be rendered in a line.
	*
	* \param[in] font Font object, in which the text letters will be stored.
	*/
	Text(wstring text, Font& font);

	/**
	* \brief Getter for the pixel data pointer.
	*/
	unsigned char* getPixmap() { return m_pixmap; }

	/**
	* \brief Getter for the text's string.
	*/
	wstring getText() { return m_text; }

	/**
	* \brief Getter for the width of the text, defined by the sum of each
	*        letter's width.
	*/
	unsigned getWidth() { return m_width; }

	/**
	* \brief Getter for the text's height, defined by the font object.
	*/
	unsigned getHeight() { return m_height; }

	/**
	* \brief Deletes the pixel data.
	*/
	~Text();

private:
	/**
	* \brief String that holds the text.
	*/
	wstring m_text;

	/**
	* \brief Pointer to the pixel data of the text.
	*/
	unsigned char* m_pixmap;

	/**
	* \brief Width of the text, defined by the sum of each characters width.
	*
	*	A characters width in freetype is specified in 1/64th of points. A point
	*  is a physical distance, equaling 1/72th of an inch.
	*/
	unsigned m_width;

	/**
	* \brief Height of the text, defined by the font object.
	*
	* A characters height in freetype is specified in 1/64th of points. A point
	* is a physical distance, equaling 1/72th of an inch.
	*/
	unsigned m_height;
};
