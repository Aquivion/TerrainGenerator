#pragma once

#include "block.h"

/**
* \brief The Label class. Subclass of class Block.
*
* An object of this class is simply used to display text in the middle 
* of a defined area.
*/
class Label: public Block {
	public:
		/**
		* \brief Constructs the label block at the given area. Calls the constructor
		* of the base class "Block".
		*
		* \param[in] posX X-Position of the top left corner of the label block.
		*
		* \param[in] posY Y-Position of the top left corner of the label block.
		*
		* \param[in] width Width of the label block.
		*
		* \param[in] height Height of the label block.
		*/
		Label(int posX, int posY, unsigned width, unsigned height);

		/**
		* \brief Draws the label block.
		*/
		void update();
};
