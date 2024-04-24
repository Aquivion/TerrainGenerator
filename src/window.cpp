#include "window.h"

/**
* \brief Global window object that will point to the first constructed window.
*/
const Window* _window = nullptr;

Window::Window(int width, int height) : m_fps(0), m_width(width), m_height(height),
	m_aspect(float(width) / float(height)), m_mouseModeReleased(false)
{
	if (width <= 0 || height <= 0)
		printCriticalError("Window(..)", "Window width and / or height must not be less then or equal to 0");
	else {
		// Initialize global window object only once after Window constructor was called
		if (_window == nullptr)
			_window = this;
	}
}

void Window::open(Style s){
	// Initialize SDL for window management
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		printCriticalError("Window::open(..)", "SDL Error: " + string(SDL_GetError()));

	// Telling SDL which OpenGL version the program shall use
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Enable antialising for polygons | Note: currently disabled because of performance
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	// Which window format shall be used
	Uint32 flags = SDL_WINDOW_OPENGL;
	switch (s){
		case Style::Fullscreen:
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; break;
		case Style::Borderless:
			flags |= SDL_WINDOW_BORDERLESS; break;
		case Style::Resizable:
			flags |= SDL_WINDOW_RESIZABLE; break;
	}

	// Create SDL window
	m_wnd = SDL_CreateWindow("TerrainGenerator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, flags);
	SDL_GetWindowSize(m_wnd, &m_width, &m_height);

	// Create OpenGL Kontext
	m_glc = SDL_GL_CreateContext(m_wnd);
	if (!m_glc)
		printCriticalError("Window::open(..)", "SDL Error: " + string(SDL_GetError()));

	// Global variable from the glew library. Need to be true, to enable newer OpenGL versions
	glewExperimental = GL_TRUE;

	// Loading all OpenGL functions that are available by the graphics card
	GLenum err = glewInit();

	// Checking if any errors occured
	if (err != GLEW_OK)
		printCriticalError("Window::open(..)", "GLEW Error: " + string((const char*)glewGetErrorString(err)));
	if (!GLEW_VERSION_3_0)
		printCriticalError("Window::open(..)", "GLEW Error: Graphics card does not support OpenGL 3.0");

	// Limit frames per second to 60
	SDL_GL_SetSwapInterval(1);
}

bool Window::update(){
	// Create SDL Event variable
	SDL_Event evt;
	m_windowResized = false;

	// Getting SDL events
	while (SDL_PollEvent(&evt)){
		if (evt.type == SDL_WINDOWEVENT){
			// Return false to close application
			if (evt.window.event == SDL_WINDOWEVENT_CLOSE)
				return false;

			// Calculate new aspect and set new viewpot when window is resized
			else if (evt.window.event == SDL_WINDOWEVENT_RESIZED){
				m_windowResized = true;
				m_width = evt.window.data1;
				m_height = evt.window.data2;
				m_aspect = float(m_width) / float(m_height);
				glViewport(0, 0, m_width, m_height);
			}
		}
		else if (evt.type == SDL_KEYDOWN){

			// Close application if escape is pressed
			if (evt.key.keysym.sym == SDLK_ESCAPE)
				return false;

			// Enable and disable cursormode by pressing space
			if (evt.key.keysym.sym == SDLK_SPACE && SDL_GetRelativeMouseMode() == SDL_TRUE){
				SDL_SetRelativeMouseMode(SDL_FALSE);
				m_mouseModeReleased = true;
			}
			if (evt.key.keysym.sym == SDLK_SPACE && SDL_GetRelativeMouseMode() == SDL_FALSE && !m_mouseModeReleased)
				SDL_SetRelativeMouseMode(SDL_TRUE);
			m_mouseModeReleased = false;
		}
	}

	// Swap back buffer with front buffer and draw context
	SDL_GL_SwapWindow(m_wnd);

	return true;
}

void Window::close(){
	// Free memory
	SDL_DestroyWindow(m_wnd);
	SDL_Quit();
}

void Window::calculateFPS(){
	// Calculate fps using the standard library's "steady clock" for time measurement
	static auto t0 = chrono::steady_clock::now();
	static int frameCount = 0;
	if ((chrono::steady_clock::now() - t0).count() > chrono::steady_clock::period::den){
		m_fps = frameCount;
		cout << "FPS: " << m_fps << endl;
		t0 = chrono::steady_clock::now();
		frameCount = 0;
	}
	frameCount++;
}

const Window* getMainWindow(){
	return _window;
}
