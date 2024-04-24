#include "panel.h"

Panel::Panel(int x, int y, unsigned w, unsigned h): Block(x, y, w, h) {}

Panel::~Panel(){
    for(auto p : m_panels)
        delete p.second;
    for(auto b : m_buttons)
        delete b.second;
    for(auto l : m_labels)
        delete l.second;

    m_panels.clear();
    m_buttons.clear();
    m_labels.clear();
}

void Panel::text(wstring t, Font& font){
	// Error checking
	if (t.empty())
		printError("Panel::text(..)", "String is empty. No text created.");
	else {
		// See comments in text-method of the base class as reference for this code block
		Text text(t, font);

		// Check if the text fits into the panel
		if (text.getWidth() > m_width)
			printError("Panel::text(..)", "String does not fit into the panel. No text created.");
		else {
			vector<float> textureData;
			textureData.reserve(m_width*m_height * 4);
			for (unsigned i = 0; i < m_width*m_height; i++)
				for (unsigned j = 0; j < 4; j++)
					textureData.push_back(m_rgba[j]);

			int x_off = (m_width - text.getWidth()) / 2;
			int y_off = m_height - text.getHeight() - 5;

			for (unsigned y = 0; y < text.getHeight(); y++) {
				for (unsigned x = 0; x < text.getWidth(); x++) {
					unsigned char p = text.getPixmap()[x + y*text.getWidth()];
					if (p > 0) {
						float text_col = float(p) / 255.0f;
						for (int i = 0; i < 4; ++i)
							textureData[((x + x_off) + (y + y_off)*m_width) * 4 + i] += m_rgba[i] + (1.0f - m_rgba[i])*text_col;
					}
				}
			}

			// If texture already exists, overwrite the texture data
			if (m_tex)
				m_tex->sub(textureData.data(), GL_RGBA, GL_FLOAT, m_width, m_height);
			else {
				checkGLError("Panel::text(..) -> Error occured before this call");
				// Otherwise create a new texture
				m_tex = new Texture(textureData.data(), m_width, m_height, GL_RGBA, GL_NONE, GL_NEAREST, 4);

				// Attach texture to a unit in the gui shader program
				glUniform1i(glGetUniformLocation(getGuiShader()->getId(), "tex"), m_tex->getUnit());
				checkGLError("Panel::text(..) -> Attach texture to texture unit in the gui shader");
			}
		}
	}
}

void Panel::update(){
	// See comments in update method of Label or Button as reference for this code block
    glCullFace(GL_BACK);

	// Use vertex buffer
	m_vbuf->use();

	if (getMainWindow()->resized())
		calculateVertices();

	if (m_tex){
		m_tex->use();
		glUniform1ui(getUseTexLoc(), GL_TRUE);
	}
	else
		glUniform1ui(getUseTexLoc(), GL_FALSE);

    glUniform4fv(getColorLoc(), 1, m_rgba);
    glUniform1f(getBrightnessLoc(), 1.0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw all sub panels, buttons and labels
	for (auto p : m_panels)
		p.second->update();
    for (auto l : m_labels)
		l.second->update();
	for (auto b : m_buttons)
		b.second->update();
}

void Panel::addPanel(Panel *p, string index){
	if (!p)
		printError("Panel::addPanel(..)", "Given panel object is null. No panel added.");
	else if(index.empty())
		printError("Panel::addPanel(..)", "Index string is empty. No panel added.");
	else if(!(m_panels.find(index) == m_panels.end()))
		printError("Panel::addPanel(..)", "Index string: " + index + "is already in list. No panel added.");
	else
		m_panels.insert({ index, p });
}

void Panel::addButton(Button *b, string index){
	if (!b)
		printError("Panel::addButton(..)", "Given button object is null. No button added.");
	else if (index.empty())
		printError("Panel::addButton(..)", "Index string is empty. No button added.");
	else if (!(m_buttons.find(index) == m_buttons.end()))
		printError("Panel::addButton(..)", "Index string: " + index + "is already in list. No button added.");
	else
		m_buttons.insert({ index, b });
}

void Panel::addLabel(Label *l, string index){
	if (!l)
		printError("Panel::addLabel(..)", "Given label object is null. No label added.");
	else if (index.empty())
		printError("Panel::addLabel(..)", "Index string is empty. No label added.");
	else if (!(m_labels.find(index) == m_labels.end()))
		printError("Panel::addLabel(..)", "Index string: " + index + "is already in list. No label added.");
	else
		m_labels.insert({ index, l });
}

Panel* Panel::getPanelAt(string index)const{
	if (index.empty())
		printError("Panel::addPanel(..)", "Index string is empty. Null returned.");
	else if (m_panels.find(index) == m_panels.end())
		printError("Panel::addPanel(..)", "Index: " + index + " not found. Null returned.");
	else
		return m_panels.at(index);
	return nullptr;
}

Button* Panel::getButtonAt(string index)const{
	if (index.empty())
		printError("Panel::addButton(..)", "Index string is empty. Null returned.");
	else if (m_buttons.find(index) == m_buttons.end())
		printError("Panel::addButton(..)", "Index: " + index + " not found. Null returned");
	else
		return m_buttons.at(index);
	return nullptr;
}

Label* Panel::getLabelAt(string index)const{
	if (index.empty())
		printError("Panel::addLabel(..)", "Index string is empty. Null returned.");
	else if (m_labels.find(index) == m_labels.end())
		printError("Panel::addLabel(..)", "Index: " + index + " not found. Null returned");
	else
		return m_labels.at(index);
	return nullptr;
}

