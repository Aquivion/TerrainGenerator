#define GLM_FORCE_INLINE

#include <vector>
#include <random>
#include <chrono>

#include "error.h"
#include "window.h"
#include "terrain.h"
#include "texture.h"
#include "camera.h"
#include "buffer.h"
#include "shader.h"
#include "glm.h"
#include "gui.h"

int main(int argc, char** argv)
{
	// Create and open a resizable window
	Window wnd(900, 650);
	wnd.open(Style::Resizable);

	// Loading shader
	Shader terrainShader("shader/terrain.vsh", "shader/terrain.fsh");

	/* GL CONFIGURATIONS */
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(~0);

	// Create camera and setup view
	Camera cam(vec3(0.0f, 50.0f, 30.0f), terrainShader.getId());

	// Noise start up parameter
	int seed = 5;
	int layerCount = 1;
	float startFrequency = 20.0f;
	float frequencyDivisor = 2.0f;
	float weightDivisor = 2.0f;
	float amplitude = 42.0f;
	int seamRes = 2048;

	// Create noise objects
	Noise terrainNoise(seed, layerCount, startFrequency, frequencyDivisor, weightDivisor, amplitude);
	Noise seamlessNoise(20340, 10, 2, 10, seamRes, 2.2f, 100.0f);

	// Generate terrain
	Terrain terrain(terrainNoise, 128, 128, 1, 1, terrainShader.getId());

	/* Texturing */
	// Create normal map for the terrain
	Texture normalTexture(terrain.getNormalMap(terrain.getNormalMapDetail()).data(),
		terrain.getNormalMapWidth(), terrain.getNormalMapHeight(), GL_RGB, GL_REPEAT, GL_LINEAR, 0);
	terrain.applyTexture("normalTex", normalTexture.getUnit());
	terrain.freeNormalMap();

	// Create Seamless noise texture for the terrain
    Texture seamlessTexture(terrain.getSeamlessMap(seamlessNoise, seamRes).data(),
		seamRes, seamRes, GL_RGB, GL_REPEAT, GL_LINEAR_MIPMAP_NEAREST, 1);
	terrain.applyTexture("seamlessTex", seamlessTexture.getUnit());
	terrain.freeSeamlessMap();

    // Load textures from files and apply them to the terrain
	Texture stoneSmoothTex("textures/smooth_rock_01.bmp", GL_REPEAT, GL_LINEAR_MIPMAP_NEAREST, 2);
	terrain.applyTexture("stoneDetailTex", stoneSmoothTex.getUnit());
	Texture stoneTex("textures/stone_big_01.bmp", GL_REPEAT, GL_LINEAR_MIPMAP_NEAREST, 3);
	terrain.applyTexture("stoneTex", stoneTex.getUnit());

	// Upload terrain data
	Buffer<Vertex> *terrainBuffer = new Buffer<Vertex>(terrain.getVertices(), terrain.getElements(), GL_DYNAMIC_DRAW);
	terrainBuffer->attrib(terrain.getProgramId(), "position", 3, 2, 0);
	terrainBuffer->attrib(terrain.getProgramId(), "texCoord", 2, 2, 3);
	terrain.freeVertices();

    // Create user interface
    Gui *gui = new Gui(terrain);

	// Set relative mouse mode on
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Main loop
	while (wnd.update()){

		// Clear window
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* TERRAIN */
		// Use terrain shader program
		terrainShader.use();

		// Use terrain buffer
		terrainBuffer->use();

		// Use terrain textures
		normalTexture.use();
		seamlessTexture.use();
		stoneSmoothTex.use();
		stoneTex.use();

		// Update camera position and viewing direction
		cam.update();

		// Enable depth values and draw terrain
		glEnable(GL_DEPTH_TEST);
        terrain.draw();

		// Handle inc- or decrease events for the terrain's brightness
		gui->updateBrightness();

		// Update whether seamless texture shall be enabled or not
		gui->updateTextureEnabling();

		/* GRAPHICAL USER INTERFACE */
		// Use the gui shader program for drawing the gui
		getGuiShader()->use();
		if (!SDL_GetRelativeMouseMode()) {
			// Disable depth values when drawing gui
			glDisable(GL_DEPTH_TEST);

			// Draw main panel
			gui->getMainPanel()->update();

			// Handle events for the changing noise panel
			gui->updateNoisePanelEvents();

			// Draw noise type panel when it should be shown
			if (gui->getShowNoisePanel())
				gui->getNoisePanel()->update();
			else {
				// Handle GUI events for the main panel
				gui->update();

				// Generate terrain when the correspondent button was clicked
				if (gui->getMainPanel()->getButtonAt("button_generate")->getState() == StateId::Released)
					gui->generateTerrain(normalTexture, *terrainBuffer);
			}

			// Draw loading label when the terrain is being generated
			if (gui->getMainPanel()->getButtonAt("button_generate")->getState() == StateId::Pressed)
				gui->getLoadingLabel()->update();
		}

		// Draw info panel
		if (gui->getShowInfo())
			gui->getInfoPanel()->update();
		gui->updateInfo();

		// Reorder info panel when window was resized
		if (wnd.resized())
			gui->reorderPanels();


	}

	// Free all allocated memory at the end
	delete terrainBuffer;
	delete gui;

	// Close window and delete the existing OpenGL context
	wnd.close();

	// Return
	return 0;
}
