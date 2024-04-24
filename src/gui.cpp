#include "gui.h"

/**
 * \brief Standard library random engine
 */
default_random_engine rEng;

/**
* \brief Global font object.
*/
Font font("fonts/OpenSans.ttf", 12);

/**
* \brief Random distribution for random seed generation.
*/
uniform_int_distribution<unsigned> dist(0, 9999999);

Gui::Gui(Terrain &terrain){
	// Initialize member
	m_terrain = &terrain;
	m_showInfo = true;
	m_showNoisePanel = false;
	m_keyboardState = SDL_GetKeyboardState(NULL);

	// Shorten strings
	wstring fFstr = to_wstring(m_terrain->getNoise().getFrequencyFactor());
	wstring wDstr = to_wstring(m_terrain->getNoise().getWeightDivisor());
	wstring brightstr = to_wstring(m_terrain->getBrightness());
	wstring shortfFstr, shortwDstr, shortbrightstr;
	for (int i = 0; i < 4; i++) {
		shortfFstr += fFstr[i];
		shortwDstr += wDstr[i];
		shortbrightstr += brightstr[i];
	}
	Label *terrainLabel = new Label(10, 10, 200, 25);

	// Create Information Panel
	m_infoPanel = new Panel(getMainWindow()->getWidth() -200, 0, 200, 50);
	m_infoPanel->color(0.2f, 0.2f, 0.2f, 0.0f);

	// Create information labels
	Label *vertexCountLabel = new Label(getMainWindow()->getWidth() - 200, 10, 200, 20);
	Label *normalMapResLabel = new Label(getMainWindow()->getWidth() - 200, 30, 200, 20);

	// Setting color for info labels
	vertexCountLabel->color(0.2f, 0.2f, 0.2f, 0.0f);
	normalMapResLabel->color(0.2f, 0.2f, 0.2f, 0.0f);

	// Setting text for info labels
	vertexCountLabel->text(L"Vertices: " + to_wstring(m_terrain->getVPR()*m_terrain->getVPC()), font);
	normalMapResLabel->text(L"Normal-Map Pixel: " + to_wstring(m_terrain->getNormalMapWidth()) + L"x" + to_wstring(m_terrain->getNormalMapHeight()), font);
	
	// Add info labels to info panel
	m_infoPanel->addLabel(vertexCountLabel, "label_VertexCount");
	m_infoPanel->addLabel(normalMapResLabel, "label_NormalMapResolution");

    // Create Main Panel
    m_mainPanel = new Panel(0, 0, 220, 2000);
    m_mainPanel->color(0.1f, 0.1f, 0.1f, 0.5f);

    // Create terrain label

	font.setSize(15);
	terrainLabel->color(0.25f, 0.25f, 0.25f, 1.0f);
	terrainLabel->text(L"Landschaft", font);
	m_mainPanel->addLabel(terrainLabel, "label_Terrain");
	font.setSize(12);

    /* Creating terrain modifiers */
	layout_modifier(L"Gr\u00F6\u00DFe: " + to_wstring(int(m_terrain->getDepth())), "SurfaceSize",
        vec2(10, 40), 200, 31, 18);
    layout_modifier(L"Vertex Detail: " + to_wstring(int(m_terrain->getVertexDetail())), "VertexDetail",
        vec2(10, 75), 200, 31, 18);
    layout_modifier(L"Normal-Map Detail: " + to_wstring(int(m_terrain->getNormalMapDetail())), "NormalMapDetail",
        vec2(10, 110), 200, 31, 18);
	layout_modifier(L"Helligkeit: " + shortbrightstr, "Brightness",
		vec2(10, 145), 200, 31, 18);

	// Create button that activates and deactivates the noise texture
	Button *activateTexture = new Button(10, 180, 200, 25);
	activateTexture->color(0.2f, 0.2f, 0.8f, 0.5f);
	activateTexture->text(L"Noise Textur aktiviert", font);
	m_mainPanel->addButton(activateTexture, "button_activateTexture");

    // Create perlin noise label
    Label *perlinLabel = new Label(10, 225, 200, 25);
	perlinLabel->color(0.25f, 0.25f, 0.25f, 1.0f);
	font.setSize(15);
	perlinLabel->text(L"Perlin Noise", font);
	m_mainPanel->addLabel(perlinLabel, "label_PerlinNoise");

	// Create button that opens the panel in which the user can change the noise algorithm
	Button *openNoisePanel = new Button(187, 230, 17, 15);
	openNoisePanel->color(0.25f, 0.25f, 0.25f, 1.0f);
	openNoisePanel->text(L"v", font);
	m_mainPanel->addButton(openNoisePanel, "button_openNoisePanel");
	font.setSize(12);

	// Creating noise type panel
	m_noisePanel = new Panel(187, 245, 200, 99);
	m_noisePanel->color(0.2f, 0.2f, 0.2f, 1.0f);

	// Creating buttons for noise type panel
	Button *changeToPerlin = new Button(192, 250, 190, 20);
	Button *changeToBillowy = new Button(192, 273, 190, 20);
	Button *changeToRidgid = new Button(192, 296, 190, 20);
	Button *changeToSinus = new Button(192, 319, 190, 20);
	changeToPerlin->color(0.3f, 0.3f, 0.3f, 1.0f);
	changeToBillowy->color(0.3f, 0.3f, 0.3f, 1.0f);
	changeToRidgid->color(0.3f, 0.3f, 0.3f, 1.0f);
	changeToSinus->color(0.3f, 0.3f, 0.3f, 1.0f);
	changeToPerlin->text(L"Perlin Noise", font);
	changeToBillowy->text(L"Billowy Noise", font);
	changeToRidgid->text(L"Ridgid Noise", font);
	changeToSinus->text(L"Cosinus Noise", font);
	m_noisePanel->addButton(changeToPerlin, "button_changeToPerlin");
	m_noisePanel->addButton(changeToBillowy, "button_changeToBillowy");
	m_noisePanel->addButton(changeToRidgid, "button_changeToRidgid");
	m_noisePanel->addButton(changeToSinus, "button_changeToCosinus");

    /* Creating Perlin Noise modifier */
	int addY = 35;
	int startY = perlinLabel->getStartY() + perlinLabel->getHeight() + 5;

	// Creating label for seed
	Label *seedLabel = new Label(10, startY, 137, 31);
	seedLabel->color(0.2f, 0.2f, 0.2f, 0.5f);
	seedLabel->text(L"Seed: " + to_wstring(int(m_terrain->getNoise().getSeed())), font);
	m_mainPanel->addLabel(seedLabel, "label_Seed");

	// Creating buttons for seed label
	Button *randomizeSeed = new Button(150, startY, 60, 14);
	Button *incSeed = new Button(150, startY + 17, 18, 14);
	Button *decSeed = new Button(150 + 21, startY + 17, 18, 14);
	Button *defaultSeed = new Button(150 + 42, startY + 17, 18, 14);

	// Color buttons for seed label
	randomizeSeed->color(0.25f, 0.2f, 0.15f, 1.0f);
	incSeed->color(0.3f, 0.2f, 0.2f, 1.0f);
	decSeed->color(0.2f, 0.2f, 0.3f, 1.0f);
	defaultSeed->color(0.25f, 0.25f, 0.25f, 1.0f);

	// Setting button text's for seed label
	randomizeSeed->text(L"Random", font);
	incSeed->text(L"+", font);
	decSeed->text(L"-", font);
	defaultSeed->text(L"<", font);

	// Add buttons for seed label to main panel
	m_mainPanel->addButton(randomizeSeed, "button_randomizeSeed");
	m_mainPanel->addButton(incSeed, "button_incSeed");
	m_mainPanel->addButton(decSeed, "button_decSeed");
	m_mainPanel->addButton(defaultSeed, "button_defaultSeed");

	// Creating modifier for perlin noise parameter
    layout_modifier(L"Schichten: " + to_wstring(m_terrain->getNoise().getLayerCount()), "LayerCount", 
		vec2(10, startY + (1*addY)), 200, 31, 18);
    layout_modifier(L"Startfrequenz: " + to_wstring(int(m_terrain->getNoise().getStartFrequency())), "StartFrequency", 
		vec2(10, startY + (2*addY)), 200, 31, 18);
	layout_modifier(L"Frequenzfaktor: " + shortfFstr, "FrequencyFactor",
        vec2(10, startY + (3*addY)), 200, 31, 18);
	layout_modifier(L"Gewichtungsteiler: " + shortwDstr, "WeightDivisor", 
		vec2(10, startY + (4 * addY)), 200, 31, 18);
	layout_modifier(L"Amplitude: " + to_wstring(int(m_terrain->getNoise().getAmplitude())), "Amplitude",
		vec2(10, startY + (5 * addY)), 200, 31, 18);

	// Create generation button
	Button *generate = new Button(10, startY + (6*addY) + 10, 100, 20);
	generate->color(0.1f, 0.3f, 0.1f, 1.0f);
	font.setSize(13);
	generate->text(L"Generieren", font);
	m_mainPanel->addButton(generate, "button_generate");
	font.setSize(12);

	// Create loading label
	m_loading = new Label(int((getMainWindow()->getWidth() + 200) / 2.0f) - 50, int(getMainWindow()->getHeight() / 2.0f) - 50, 100, 20);
	m_loading->color(0.25f, 0.25f, 0.25f, 0.0f);
	font.setSize(16);
	m_loading->text(L"Laden ...", font);
	font.setSize(12);
}

Gui::~Gui(){
	delete m_loading;
    delete m_mainPanel;
	delete m_noisePanel;
	delete m_infoPanel;
}

void Gui::update(){
	// Listen "Surface Size" modifier
    if (m_mainPanel->getButtonAt("button_incSurfaceSize")->getState() == StateId::Pressed)
        modifySurfaceSize(1);
    if (m_mainPanel->getButtonAt("button_decSurfaceSize")->getState() == StateId::Pressed)
		modifySurfaceSize(-1);

	// Listen "Vertex Detail" modifier
    if (m_mainPanel->getButtonAt("button_incVertexDetail")->getState() == StateId::Released)
        modifyVertexDetail(1);
    if (m_mainPanel->getButtonAt("button_decVertexDetail")->getState() == StateId::Released)
        modifyVertexDetail(-1);

	// Listen "Normal map Resolution" modifier
    if (m_mainPanel->getButtonAt("button_incNormalMapDetail")->getState() == StateId::Released)
        modifyNormalMapDetail(1);
    if (m_mainPanel->getButtonAt("button_decNormalMapDetail")->getState() == StateId::Released)
        modifyNormalMapDetail(-1);

	// Listen "Seed" modifier
	if (m_mainPanel->getButtonAt("button_randomizeSeed")->getState() == StateId::Released)
		randomizeSeed();
	if (m_mainPanel->getButtonAt("button_defaultSeed")->getState() == StateId::Released)
		defaultSeed();
	if (m_mainPanel->getButtonAt("button_incSeed")->getState() == StateId::Released)
		modifySeed(1);
	if (m_mainPanel->getButtonAt("button_decSeed")->getState() == StateId::Released)
		modifySeed(-1);

	// Listen "Layer count" modifier
    if (m_mainPanel->getButtonAt("button_incLayerCount")->getState() == StateId::Released)
        modifyLayerCount(1);
    if (m_mainPanel->getButtonAt("button_decLayerCount")->getState() == StateId::Released)
        modifyLayerCount(-1);

	// Listen "Start Frequency" modifier
    if (m_mainPanel->getButtonAt("button_incStartFrequency")->getState() == StateId::Released)
        modifyStartFrequency(1.0f);
    if (m_mainPanel->getButtonAt("button_decStartFrequency")->getState() == StateId::Released)
        modifyStartFrequency(-1.0f);

	// Listen "Frequency Factor" modifier
	if (m_mainPanel->getButtonAt("button_incFrequencyFactor")->getState() == StateId::Released)
		modifyFrequencyFactor(0.05f);
	if (m_mainPanel->getButtonAt("button_decFrequencyFactor")->getState() == StateId::Released)
		modifyFrequencyFactor(-0.05f);

	// Listen "Weight Divisor" modifier
	if (m_mainPanel->getButtonAt("button_incWeightDivisor")->getState() == StateId::Released)
		modifyWeightDivisor(0.05f);
	if (m_mainPanel->getButtonAt("button_decWeightDivisor")->getState() == StateId::Released)
		modifyWeightDivisor(-0.05f);

	// Listen "Amplitude" modifier
	if (m_mainPanel->getButtonAt("button_incAmplitude")->getState() == StateId::Pressed)
		modifyAmplitude(1);
	if (m_mainPanel->getButtonAt("button_decAmplitude")->getState() == StateId::Pressed)
		modifyAmplitude(-1);
}

void Gui::updateBrightness() {
	// Listen "Terrain Brightness" modifier
	if (m_mainPanel->getButtonAt("button_incBrightness")->getState() == StateId::Pressed)
		modifyBrightness(0.1f);
	if (m_mainPanel->getButtonAt("button_decBrightness")->getState() == StateId::Pressed)
		modifyBrightness(-0.1f);
	
}
void Gui::updateTextureEnabling(){
	// Listen if seamless noise texture shall be activated or deactivated
	if (m_mainPanel->getButtonAt("button_activateTexture")->getState() == StateId::Released)
		activateTextureClicked();
}
void Gui::updateNoisePanelEvents() {
	static bool changeHit = false;

	// Listen "Noise Type" modifiers
	if (!changeHit) {
		if (m_noisePanel->getButtonAt("button_changeToPerlin")->getState() == StateId::Released) {
			hideNoisePanel();
			changeHit = true;
			changeToPerlinClicked();
		}
		if (m_noisePanel->getButtonAt("button_changeToBillowy")->getState() == StateId::Released) {
			hideNoisePanel();
			changeHit = true;
			changeToBillowyClicked();
		}
		if (m_noisePanel->getButtonAt("button_changeToRidgid")->getState() == StateId::Released) {
			hideNoisePanel();
			changeHit = true;
			changeToRidgidClicked();
		}
		if (m_noisePanel->getButtonAt("button_changeToCosinus")->getState() == StateId::Released) {
			hideNoisePanel();
			changeHit = true;
			changeToCosinusClicked();
		}
	}

	// Listen whether the noise panel should be open or hidden
	if (m_mainPanel->getButtonAt("button_openNoisePanel")->getState() == StateId::Released) {
		if (!m_showNoisePanel) {
			changeHit = false;
			showNoisePanel();
		}
		else
			hideNoisePanel();
	}
}
void Gui::updateInfo() {
	static bool keyPressed = false;
	
	// Hide/Show information panel when i was pressed
	if (m_keyboardState[SDL_SCANCODE_I]) {
		if (!keyPressed) {
			if (m_showInfo)
				m_showInfo = false;
			else
				m_showInfo = true;
			keyPressed = true;
		}
	}
	else
		keyPressed = false;
}

void Gui::layout_modifier(wstring labelText, string modifierName, vec2 startpoint, int w, int h, int bW){
	if (labelText.empty())
		printError("Gui::layout_modifier(..)", "Parameter labelText is empty. No gui elements created.");
	else if(modifierName.empty())
		printError("Gui::layout_modifier(..)", "Parameter modifierName is empty. No gui elements created.");
	else {
		// Create label
		Label *label = new Label((int)startpoint.x, (int)startpoint.y, w - bW - 3, h);
		label->color(0.2f, 0.2f, 0.2f, 0.5f);
		label->text(labelText, font);

		// Create increase button
		Button *incButton = new Button((int)startpoint.x + label->getWidth() + 3, (int)startpoint.y,
			bW, int((h / 2.0f) - 1.5f));
		incButton->color(0.3f, 0.2f, 0.2f, 1.0f);
		incButton->text(L"+", font);

		// Create decrease button
		Button *decButton = new Button(incButton->getStartX(), (int)startpoint.y + incButton->getHeight() + 3,
			incButton->getWidth(), incButton->getHeight());
		decButton->color(0.2f, 0.2f, 0.3f, 1.0f);
		decButton->text(L"-", font);

		// Add label and the two buttons to the main panel
		m_mainPanel->addLabel(label, "label_" + modifierName);
		m_mainPanel->addButton(incButton, "button_inc" + modifierName);
		m_mainPanel->addButton(decButton, "button_dec" + modifierName);
	}
}

void Gui::generateTerrain(Texture &normalTexture, Buffer<Vertex> &terrainBuffer){
	checkGLError("Gui::generateTerrain(..) -> Error occurred before this call");

	// Use terrain shader program 
	glUseProgram(m_terrain->getProgramId());

	// Calculate new vertices and elements
    m_terrain->calculateVertices();
	checkGLError("Gui::generateTerrain(..) -> Calculate new vertex data");

	// Create and upload new normal map texture
	int nmDetail = m_terrain->getNormalMapDetail();
	normalTexture.sub(m_terrain->getNormalMap(nmDetail).data(), GL_RGB, GL_FLOAT, 256*nmDetail, 256* nmDetail);
	m_terrain->freeNormalMap();
	checkGLError("Gui::generateTerrain(..) -> Upload new normal texture");

	// Upload new vertex data to the vertex buffer object
    if(m_terrain->getVPR()*m_terrain->getVPC() == terrainBuffer.getVertexCount())
        terrainBuffer.upload(m_terrain->getVertices());
	else {
		// Only calculate new element list when the vertex count differ.
		m_terrain->calculateElements();
		terrainBuffer.upload(m_terrain->getVertices(), m_terrain->getElements(), GL_DYNAMIC_DRAW);
	}
	m_terrain->freeVertices();

	// Switch back to gui shader
	getGuiShader()->use();

	// Update information panel
	m_infoPanel->getLabelAt("label_VertexCount")->text(L"Vertices: " + 
		to_wstring(m_terrain->getVPR()*m_terrain->getVPC()), font);
	m_infoPanel->getLabelAt("label_NormalMapResolution")->text(L"Normal-Map Pixel: " + 
		to_wstring(m_terrain->getNormalMapWidth()) + L"x" + to_wstring(m_terrain->getNormalMapHeight()), font);
	checkGLError("Gui::generateTerrain(..) -> End of the function");
}

void Gui::modifySurfaceSize(int value){
    if((m_terrain->getDepth()+value) >= 4  && (m_terrain->getDepth()+value) <= 999){
        m_terrain->setSize(m_terrain->getWidth()+value, m_terrain->getDepth()+value);
        m_mainPanel->getLabelAt("label_SurfaceSize")->text(L"Gr\u00F6\u00DFe: " + to_wstring(int(m_terrain->getDepth())), font);
    }
}

void Gui::modifyVertexDetail(int value){
    if((m_terrain->getVertexDetail()+value) <= 8 && (m_terrain->getVertexDetail()+value) > 0){
		m_terrain->setVertexDetail(m_terrain->getVertexDetail()+value);
        m_mainPanel->getLabelAt("label_VertexDetail")->text(L"Vertex Detail: " + to_wstring(int(m_terrain->getVertexDetail())), font);
    }
}

void Gui::modifyNormalMapDetail(int value){
    if((m_terrain->getNormalMapDetail()+value) <= 8 && (m_terrain->getNormalMapDetail()+value) > 0){
        m_terrain->setNormalMapDetail(m_terrain->getNormalMapDetail()+value);
        m_mainPanel->getLabelAt("label_NormalMapDetail")->text(L"Normal-Map Detail: " + to_wstring(int(m_terrain->getNormalMapDetail())), font);
    }
}

void Gui::modifyBrightness(float value) {
	if ((m_terrain->getBrightness() + value) <= 10.0f && (m_terrain->getBrightness() + value) > 0.1f) {
		m_terrain->setBrightness(m_terrain->getBrightness() + value);
		wstring str = to_wstring(m_terrain->getBrightness());
		wstring shortstr;
		for (int i = 0; i < 4; i++)
			shortstr += str[i];
		m_mainPanel->getLabelAt("label_Brightness")->text(L"Helligkeit: " + shortstr, font);	
	}
}

void Gui::randomizeSeed() {
	m_terrain->getNoise().setNewSeed(dist(rEng));
	m_mainPanel->getLabelAt("label_Seed")->text(L"Seed: " + to_wstring(int(m_terrain->getNoise().getSeed())), font);
}

void Gui::defaultSeed() {
	m_terrain->getNoise().setNewSeed(5);
	m_mainPanel->getLabelAt("label_Seed")->text(L"Seed: " + to_wstring(int(m_terrain->getNoise().getSeed())), font);
}

void Gui::modifySeed(int value) {
	if ((m_terrain->getNoise().getSeed() + value) > 0 && (m_terrain->getNoise().getSeed() + value) <= 9999999) {
		m_terrain->getNoise().setNewSeed(m_terrain->getNoise().getSeed() + value);
		m_mainPanel->getLabelAt("label_Seed")->text(L"Seed: " + to_wstring(int(m_terrain->getNoise().getSeed())), font);
	}
}

void Gui::modifyLayerCount(int value){
    if((m_terrain->getNoise().getLayerCount()+value) > 0 && (m_terrain->getNoise().getLayerCount()+value) <= 12){
        m_terrain->getNoise().setLayerCount(m_terrain->getNoise().getLayerCount() + value);
        m_mainPanel->getLabelAt("label_LayerCount")->text(L"Schichten: " + to_wstring(int(m_terrain->getNoise().getLayerCount())), font);
    }
}

void Gui::modifyStartFrequency(float value){
	if ((m_terrain->getNoise().getStartFrequency() + value) > 0 && (m_terrain->getNoise().getStartFrequency() + value) <= 1000) {
		m_terrain->getNoise().setStartFrequency(m_terrain->getNoise().getStartFrequency() + value);
		m_mainPanel->getLabelAt("label_StartFrequency")->text(L"Startfrequenz: " + to_wstring(int(m_terrain->getNoise().getStartFrequency())), font);
	}
}

void Gui::modifyFrequencyFactor(float value){
    if((m_terrain->getNoise().getFrequencyFactor()+value) > 0.05f && (m_terrain->getNoise().getFrequencyFactor() + value) <= 9.95f){
        m_terrain->getNoise().setFrequencyFactor(m_terrain->getNoise().getFrequencyFactor() + value);

        wstring str = to_wstring(m_terrain->getNoise().getFrequencyFactor());
        wstring shortstr;
        for (int i = 0; i < 4; i++)
            shortstr += str[i];

		m_mainPanel->getLabelAt("label_FrequencyFactor")->text(L"Frequenzfaktor: " + shortstr, font);
	}
}

void Gui::modifyWeightDivisor(float value){
	if ((m_terrain->getNoise().getWeightDivisor() + value) > 0.05f && (m_terrain->getNoise().getWeightDivisor() + value) <= 9.95f) {
		m_terrain->getNoise().setWeightDivisor(m_terrain->getNoise().getWeightDivisor() + value);

		wstring str = to_wstring(m_terrain->getNoise().getWeightDivisor());
		wstring shortstr;
		for (int i = 0; i < 4; i++)
			shortstr += str[i];

		m_mainPanel->getLabelAt("label_WeightDivisor")->text(L"Gewichtungsteiler: " + shortstr, font);
	}
}

void Gui::modifyAmplitude(int value) {
	if (int(m_terrain->getNoise().getAmplitude() + value) > 0 && int(m_terrain->getNoise().getAmplitude() + value) <= 9999999) {
		m_terrain->getNoise().setAmplitude(m_terrain->getNoise().getAmplitude() + value);
		m_mainPanel->getLabelAt("label_Amplitude")->text(L"Amplitude: " + to_wstring(int(m_terrain->getNoise().getAmplitude())), font);
	}
}

void Gui::changeToPerlinClicked() {
	font.setSize(15);
	m_mainPanel->getLabelAt("label_PerlinNoise")->text(L"Perlin Noise", font);
	m_terrain->getNoise().setNoiseType(NoiseType::PerlinNoise);
	font.setSize(12);
}

void Gui::changeToBillowyClicked() {
	font.setSize(15);
	m_mainPanel->getLabelAt("label_PerlinNoise")->text(L"Billowy Noise", font);
	m_terrain->getNoise().setNoiseType(NoiseType::BillowyNoise);
	font.setSize(12);
}

void Gui::changeToRidgidClicked() {
	font.setSize(15);
	m_mainPanel->getLabelAt("label_PerlinNoise")->text(L"Ridgid Noise", font);
	m_terrain->getNoise().setNoiseType(NoiseType::RidgidNoise);
	font.setSize(12);
}

void Gui::changeToCosinusClicked() {
	font.setSize(15);
	m_mainPanel->getLabelAt("label_PerlinNoise")->text(L"Cosinus Noise", font);
	m_terrain->getNoise().setNoiseType(NoiseType::CosinusNoise);
	font.setSize(12);
}

void Gui::activateTextureClicked() {
	if (m_terrain->getSeamlessTexEnabled()) {
		m_terrain->setSeamlessTexEnabled(false);
		m_mainPanel->getButtonAt("button_activateTexture")->color(0.1f, 0.1f, 0.4f, 0.5f);
		m_mainPanel->getButtonAt("button_activateTexture")->text(L"Noise Textur deaktiviert", font);
	}
	else {
		m_terrain->setSeamlessTexEnabled(true);
		m_mainPanel->getButtonAt("button_activateTexture")->color(0.2f, 0.2f, 0.8f, 0.5f);
		m_mainPanel->getButtonAt("button_activateTexture")->text(L"Noise Textur aktiviert", font);
	}
}

void Gui::reorderPanels() {
	m_loading->reorder(int((getMainWindow()->getWidth() + 200) / 2.0f) - 50, int(getMainWindow()->getHeight() / 2.0f) - 50);
	m_infoPanel->reorder(getMainWindow()->getWidth() - 200, 0);
	m_infoPanel->getLabelAt("label_VertexCount")->reorder(getMainWindow()->getWidth() - 200, 10);
	m_infoPanel->getLabelAt("label_NormalMapResolution")->reorder(getMainWindow()->getWidth() - 200, 30);
	m_noisePanel->reorder(187, 245);
	m_noisePanel->getButtonAt("button_changeToPerlin")->reorder(192, 250);
	m_noisePanel->getButtonAt("button_changeToBillowy")->reorder(192, 273);
	m_noisePanel->getButtonAt("button_changeToRidgid")->reorder(192, 296);
	m_noisePanel->getButtonAt("button_changeToCosinus")->reorder(192, 319);
}