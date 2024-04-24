#include "ft2font.h"

/**
* \brief Freetype library object, needed by freetype.
*/
FT_Library lib = nullptr;

/**
* \brief Frees the freetype library object.
*/
void free() {
	FT_Error err = FT_Done_FreeType(lib);
	if (err)
		printError("'ft2font.cpp' -> free()", "Freetype library object could not be freed.");
}

/**
* \brief Initializes the freetype library object.
* Only needed to be done once after program start.
*/
void initFont() {
	FT_Error err = FT_Init_FreeType(&lib);
	if (err)
		printError("'ft2font.cpp'->initFont()", "Initialization of the Freetype library object failed.");
	atexit(free);
}

Font::Font(string path, FT_UInt size) : m_size(size) {
	// Intializes freetype library object once
	if (!lib)
		initFont();

	// Creating new freetype face object
	FT_Error err = FT_New_Face(lib, path.c_str(), 0, &m_face);

	// Error checking
	if (err == FT_Err_Unknown_File_Format)
		printError("Font::Font(path, size)", "Font format is not supported.");
	else if (err != 0)
		printError("Font::Font(path, size)", "File not found");

	// Set size for the font glyphs
	err = FT_Set_Pixel_Sizes(m_face, m_size, 0);

	// Error checking
	if (err)
		printError("Font::Font(path, size)", "Given font size is not supported");
}

void Font::setSize(FT_UInt size) {
	// Set new size for font glyps
	FT_Error err = FT_Set_Pixel_Sizes(m_face, size, 0);

	// Error checking
	if (err)
		printError("Font::setSize(size)", "Given font size is not supported");
	else
		m_size = size;
}

Font::~Font() {
	FT_Done_Face(m_face);
}

Text::Text(wstring text, Font& font): m_text(text), m_width(0), m_height(0){
	if (text.empty())
		printCriticalError("Text::Text(text, font)", "text is empty.");
	else {
		// Load text's characters one by one into the freetype face
		for (auto c : text) {
			FT_Load_Char(font.getFace(), c, FT_LOAD_ADVANCE_ONLY);

			// Calculate the text's width
			m_width += font.getFace()->glyph->advance.x;
		}

		// Divide width by 64 because of the freetype internal size format.
		m_width >>= 6;

		// Calculate height and divide by 64
		m_height = (font.getFace()->size->metrics.ascender - font.getFace()->size->metrics.descender) >> 6;

		// Allocate memory for the pixel data
		m_pixmap = new unsigned char[m_width*m_height]();

		// Starting point of the letter to be drawn next
		int penX = 0, penY = -font.getFace()->size->metrics.descender >> 6;

		// Load each character into the pixmap
		for (auto c : text) {
			// Load single char
			FT_Load_Char(font.getFace(), c, FT_LOAD_RENDER);

			// Create a freetype glyph slot
			FT_GlyphSlot glyph = font.getFace()->glyph;

			// Write into the pixmap
			for (unsigned y = 0; y < glyph->bitmap.rows; ++y) {
				for (unsigned x = 0; x < glyph->bitmap.width; ++x) {
					m_pixmap[penX + glyph->bitmap_left + x + (penY + glyph->bitmap_top - y) * m_width]
						= glyph->bitmap.buffer[x + y * glyph->bitmap.pitch];
				}
			}

			// Change starting point for next letter
			penX += font.getFace()->glyph->advance.x >> 6;
		}
	}
}

Text::~Text() {
	delete[] m_pixmap;
}
