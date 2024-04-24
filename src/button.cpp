#include "button.h"

Button::Button(int x, int y, unsigned w, unsigned h) : Block(x, y, w, h) {
	m_state = StateId::None;
}

void Button::update() {
	// Switch cull face
	glCullFace(GL_BACK);

	// Use buffer object
	m_vbuf->use();

	// Calculate new vertices when window was resized
	if (getMainWindow()->resized())
		calculateVertices();

	// When there is an active texture ..
	if (m_tex) {
		m_tex->use();
		// Set uniform for enabled texture to true
		glUniform1ui(getUseTexLoc(), GL_TRUE);
	}
	else
		// Otherwise set it to false
		glUniform1ui(getUseTexLoc(), GL_FALSE);

	// Set the button state tu StateId::up once, if the button was released
	if (m_state == StateId::Pressed)
		m_state = StateId::Released;

	// When the button state was up once, then in the next cycle it is set to state none
	else if (m_state == StateId::Released)
		m_state = StateId::None;

	// Get mouse positions
	int mouseX, mouseY;
	Uint32 mouseButton = SDL_GetMouseState(&mouseX, &mouseY);

	// Handle mouse over and left button click events
	if (mouseX > m_x && mouseY > m_y && mouseX < m_x + int(m_width) && mouseY < m_y + int(m_height)) {
		if (mouseButton == SDL_BUTTON(SDL_BUTTON_LEFT)) {
			glUniform1f(getBrightnessLoc(), 0.8f);
			m_state = StateId::Pressed;
		}
		else {
			glUniform1f(getBrightnessLoc(), 1.4f);
			if (m_state == StateId::None)
				m_state = StateId::Mouseover;
		}
	}
	else {
		if (m_state == StateId::Mouseover)
			m_state = StateId::None;
		glUniform4fv(getColorLoc(), 1, m_rgba);
		glUniform1f(getBrightnessLoc(), 1.0f);
	}

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
