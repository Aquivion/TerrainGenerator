#include "label.h"

Label::Label(int x, int y, unsigned w, unsigned h): Block(x, y, w, h) {}

void Label::update(){
	// Change culled face site because of different triangle strip route.
    glCullFace(GL_BACK);

	// Use vertex buffer
	m_vbuf->use();

	// Calculate new vertices when window was resized
	if (getMainWindow()->resized())
		calculateVertices();

	// Use texture if there is one
	if (m_tex){
		m_tex->use();
		glUniform1ui(getUseTexLoc(), GL_TRUE);
	}
	else
		glUniform1ui(getUseTexLoc(), GL_FALSE);

	// Upload color and brightness
    glUniform4fv(getColorLoc(), 1, m_rgba);
    glUniform1f(getBrightnessLoc(), 1.0);

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
