#pragma once

#include "button.h"
#include "panel.h"
#include "label.h"
#include "ft2font.h"
#include "terrain.h"
#include "noise.h"
#include "error.h"

/**
* \brief The graphical user interface class.
*
* All buttons labels and panels that are seen in the application are created
* when an object of it is made. So there should be only one instance of this
* class in the whole application. Through the 'update'-functions all gui events
* are handled. The methods to handle these events are also implemented here.
*/
class Gui {
public:
	/**
	* \brief Gui-constructor. Creates all buttons and labels and adds them to the member panels.
	*
	* \param[in] terrain Reference to the terrain to have access to it's data.
	*/
	Gui(Terrain &terrain);

	/**
	* \brief Destructs all button-, label- and panel objects and the font object.
	*/
	~Gui();

	/**
	* \brief Update function to handle the most of the gui events, specially the terrain and noise modifier.
	*/
	void update();

	/**
	* \brief Update function to handle the brightness changing event.
	*/
	void updateBrightness();

	/**
	* \brief Update function to handle enabling/disabling event for the seamless noise texture.
	*/
	void updateTextureEnabling();

	/**
	* \brief Update function to handle events, happening in the panel from
	* which the user can change the noise type. The user can change between
	* billowy, ridgid, cosinus and normal perlin noise.
	*/
	void updateNoisePanelEvents();

	/**
	* \brief Update function to handle events, happening in the information
	* panel in the top right corner of the window. Updates the FPS everytime
	* they change. By pressing 'I' the user can hide or show the information
	* panel.
	*/
	void updateInfo();

	/**
	* \brief Reorders the information- and 'noise-type'-panel when the window was resized.
	*/
	void reorderPanels();

	/**
	* \brief Generates a new terrain based on the input the user made. It is called
	* when the button 'Generieren' was clicked.
	*
	* \param[in] normalTexture Reference of the normal map texture for overwriting it.
	*
	* \param[in] terrainBuffer Reference of the vertex buffer object of the terrain for overwriting it.
	*/
	void generateTerrain(Texture &normalTexture, Buffer<Vertex> &terrainBuffer);

	/**
	* \brief Getter for the main panel on the left side of the window.
	*/
	Panel* getMainPanel()const { return m_mainPanel; }

	/**
	* \brief Getter for the information panel in the top right corner of the window.
	*/
	Panel* getInfoPanel()const { return m_infoPanel; }

	/**
	* \brief Getter for the 'noise-type' panel that can be opened.
	*/
	Panel* getNoisePanel()const { return m_noisePanel; }

	/**
	* \brief Getter for the loading label that indicates a new terrain is being generated.
	*/
	Label* getLoadingLabel()const { return m_loading; }

	/**
	* \brief Getter for the boolean that tells whether the information panel is hidden or shown.
	*/
	bool getShowInfo()const { return m_showInfo; }

	/**
	* \brief Getter for the boolean that tells whether the 'noise-type' panel is hidden or shown.
	*/
	bool getShowNoisePanel()const { return m_showNoisePanel; }

private:

	/**
	* \brief Utility function to create the basic modifier layout of 2 buttons and a label.
	* One button increases the value that wants to be modified and one decreases it.
	*
	* \param[in] labelText Text that will be displayed in the label block.
	*
	* \param[in] modifierName Name for indicating the label and the two buttons. The indicating
	* label name will have the prefix 'label_' and the two buttons 'inc_' and 'dec_'.
	*
	* \param[in] startpoint Top left corner position of the label block.
	*
	* \param[in] width Width of the area including the two buttons.
	*
	* \param[in] height Height of the label block and the space the two buttons will take in height.
	*
	* \param[in] buttonWidth Width of both button blocks.
	*/
	void layout_modifier(wstring labelText, string modifierName, vec2 startpoint, int width, int height, int buttonWidth);

	/**
	* \brief Function for modifying the size of the surface.
	*
	* \param[in] value Value by which the size of the surface will be inc- or decreased.
	*/
	void modifySurfaceSize(int value);

	/**
	* \brief Function for modifying vertex detail of the surface. The vertex count in
	* width and in height is calculated with 128*vertexDetail.
	*
	* \param[in] value Value by which the vertex detail will be inc- or decreased.
	*/
	void modifyVertexDetail(int value);

	/**
	* \brief Function for modifying the resolution of the normal map. Pixel count in width
	* and in height is calculated with 256*normalMapDetail.
	*
	* \param[in] value Value by which the normal map detail will be inc- or decreased.
	*/
	void modifyNormalMapDetail(int value);

	/**
	* \brief Function for modifying the brightness of the terrain. This was implemented
	* because for some noise-types the terrain is darker than usual, because of a lower
	* height.
	*
	* \param[in] value Value by which the brightness of the terrain will be inc- or decreased.
	*/
	void modifyBrightness(float value);

	/**
	* \brief Function to randomize the seed that will used in the noise object of the terrain.
	* Output is in the range of 0 to 9999999.
	*/
	void randomizeSeed();

	/**
	* \brief Function to set the noise object's seed of the terrain to the default value.
	* This value currently is 5.
	*/
	void defaultSeed();

	/**
	* \brief Function for modifying the noise object's seed of the terrain.
	*
	* \param[in] value Value by which the seed's value will be inc- or decreased.
	*/
	void modifySeed(int value);

	/**
	* \brief Function for modifying the number of layers that will add up for the noise value.
	*
	* \param[in] value Value by which the number of layers will be inc- or decreased.
	*/
	void modifyLayerCount(int value);

	/**
	* \brief Function for modifying frequency value the first layer will have.
	*
	* \param[in] value Value by which the start frequency will be inc- or decreased.
	*/
	void modifyStartFrequency(float value);

	/**
	* \brief Function for modifying factor by which the frequency will be increased
	* with each added layer.
	*
	* \param[in] value Value by which the frequency factor will be inc- or decreased.
	*/
	void modifyFrequencyFactor(float value);

	/**
	* \brief Function for modifying divisor by which the weight of each added layer
	* will be decreased.
	*
	* \param[in] value Value by which the weight divisor will be inc- or decreased.
	*/
	void modifyWeightDivisor(float value);

	/**
	* \brief Function for modifying amplitude which determines the overall height of the terrain.
	*
	* \param[in] value Value by which the amplitude will be inc- or decreased.
	*/
	void modifyAmplitude(int value);

	/**
	* \brief Sets the member that determines whether the noise panel is displayed or not to 'true'.
	*/
	void showNoisePanel() { m_showNoisePanel = true; }

	/**
	* \brief Sets the member that determines whether the noise panel is displayed or not to 'false'.
	*/
	void hideNoisePanel() { m_showNoisePanel = false; }

	/**
	* \brief Handles the event when the button is clicked that changes the noise type to 'Perlin Noise'.
	*/
	void changeToPerlinClicked();

	/**
	* \brief Handles the event when the button is clicked that changes the noise type to 'Billowy Noise'.
	*/
	void changeToBillowyClicked();

	/**
	* \brief Handles the event when the button is clicked that changes the noise type to 'Ridgid Noise'.
	*/
	void changeToRidgidClicked();

	/**
	* \brief Handles the event when the button is clicked that changes the noise type to 'Cosinus Noise'.
	*/
	void changeToCosinusClicked();

	/**
	* \brief Handles the event when the button is clicked that enables/disables the seamless noise texture.
	*/
	void activateTextureClicked();

	/**
	* \brief Pointer to the terrain to access it's data
	*/
	Terrain *m_terrain;

	/**
	* \brief Label for the loading block that indicates when a terrain is being generated.
	* Will be displayed near to the middle of the window.
	*/
	Label *m_loading;

	/**
	* \brief Main Panel for most of the gui elements.
	*/
	Panel *m_mainPanel;

	/**
	* \brief Panel to display information about the current context, like the frames per second,
	* the number of vertices and the resolution of the normal map.
	*/
	Panel *m_infoPanel;

	/**
	* \brief Panel for the buttons witch which the user can change to the different noise types.
	*/
	Panel *m_noisePanel;

	/**
	* \brief Array of keyboard scan-codes. When pressing 'I' the information panel will be hidden
	* or shown.
	*/
	const Uint8 *m_keyboardState;

	/**
	* \brief Boolean that tells whether the information panel is shown or hidden.
	*/
	bool m_showInfo;

	/**
	* \brief Boolean that tells whether the noise panel with it's buttons is shown or hidden.
	*/
	bool m_showNoisePanel;
};
