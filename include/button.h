#pragma once

#include "block.h"

/**
 * \brief Enumeration class for the possible states a button object could be in
 */
enum class StateId {
	Mouseover, Pressed, Released, None
};

/**
 * \brief Button class which is a subclass of Block. 
 * 
 * A button object can be used to simply trigger special events. It can be in 3 possible states.
 * When it is in 'Mouseover' state, the color of the button gets highlighted. The 'Pressed' state 
 * says that it is currently pressed down. When it is released, the button is in the 'Released' state
 * until the next loop run.
 */
class Button: public Block{
    public:
		/**
		 * \brief Constructor. Gets position and size.
		 *
		 * \param[in] posX X-position of the top left corner of the button.
		 *
		 * \param[in] posY Y-position of the top left corner of the button.
		 *
		 * \param[in] width Width.
		 *
		 * \param[in] height Height.
		 */
        Button(int posX, int posY, unsigned width, unsigned height);

		/**
		 * \brief Returns the current state the button is in.
		 */
        const StateId getState()const {return m_state;}

		/**
		 * \brief Draws the button considering it's color based on the state.
		 */
        void update();

    private:
		/**
		 * \brief State the button is in based on the current context.
		 */
        StateId m_state;
};