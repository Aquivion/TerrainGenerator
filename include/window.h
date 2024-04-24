#pragma once

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "error.h"

/**
* \brief Enumeration class for the style for which the Window is constructed. Currently
* you can open a window in fullscreen, without any borders or as a resizable window.
* Because it is currently not allowed to create more than one window, there should only
* be one instance of this class in the whole application.
* 
*/
enum class Style{
	Fullscreen,
	Borderless,
	Resizable,
};

/**
* \brief The Window class.
*
* When the constructor of this class is called a window will be opened with SDL2. The window
* will have an OpenGL context so that you can draw in it.
*/
class Window
{
public:
	/**
	* \brief Just initializes member variables and the, in the .cpp file declared, global window object.
	* You can access this window with the getter declared below. This is used to have access to the window
	* data from every file this header is included.
	*
	* \param[in] width Width of the window.
	*
	* \param[in] height Height of the window.
	*/
	Window(int width, int height);

	/**
	* \brief Creates and opens a window with an active OpenGL context. The style in which the window is
	* opened can be set here.
	*
	* \param[in] style Style in which the window is opened.
	*/
	void open(Style style = Style::Resizable);

	/**
	* \brief Updates the window and handles events like closing the window, resizing it or changing from
	* flying-mode into cursor-mode by pressing "Space".
	*/
	bool update();

	/**
	* \brief Closes the window and deletes the allocated memory for it.
	*/
	void close();

	/**
	* \brief Getter for the window's width.
	*/
	int getWidth()const { return m_width; }

	/**
	* \brief Getter for the window's height.
	*/
	int getHeight()const { return m_height; }

	/**
	* \brief Getter for the boolean that tells when the window was resized.
	*/
	bool resized()const { return m_windowResized; }

	/**
	* \brief Getter for the ratio between width and height.
	*/
	float getAspect()const { return m_aspect; }

	/**
	* \brief Getter for the frames per second. To get the current Fps, "calculateFps()" needs
	* to be called in somewhere in the main loop.
	*/
	int getFps()const { return m_fps; }

	/**
	* \brief Calculates the current frames per second. Needs to be called somewhere in the main loop
	* to update the member variable m_fps.
	*/
	void calculateFPS();

private:
	/**
	* \brief The SDL window object.
	*/
	SDL_Window*	m_wnd;

	/**
	* \brief The OpenGL context object.
	*/
	SDL_GLContext m_glc;

	/**
	* \brief Frames per second.
	*/
	int m_fps;

	/**
	* \brief Window width.
	*/
	int	m_width;

	/**
	* \brief Window height.
	*/
	int m_height;

	/**
	* \brief Ratio of window width and height
	*/
	float m_aspect;

	/**
	* \brief Boolean that tells when the window was resized.
	*/
	bool m_windowResized;

	/**
	* \brief Boolean used for switching between flying-mode and cursor-mode.
	*/
	bool m_mouseModeReleased;
};

const Window* getMainWindow();

