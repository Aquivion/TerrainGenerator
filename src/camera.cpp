#include "camera.h"

Camera::Camera(vec3 pos, const GLuint programId)
	: m_position(pos), m_rotation(vec2(0.0, 0.0)), m_lightRotation(0.0f)
{
	// Get keyboard state
	m_keyboardState = SDL_GetKeyboardState(NULL);

	// Get name for projection-translation-rotation matrix
	m_mvpLoc = glGetUniformLocation(programId, "mvp");
	m_posLoc = glGetUniformLocation(programId, "cameraPos");

	// Get relative mouse position
	SDL_GetRelativeMouseState(&m_mousePos.x, &m_mousePos.y);
	m_cursorModeEnabled = false;

	// Setup viewing direction
	m_viewingDir = vec3(0.0, 0.0, -1.0);
	m_startViewingDir = m_viewingDir;
	m_camSpeed = 0.2f;

	// Initialize matrices
	m_projMatrix = perspective(radians(45.0), double(float(getMainWindow()->getWidth()) / float(getMainWindow()->getHeight())), 1.0, 1000.0);
	m_translationMatrix = translate(mat4(1.0f), -pos);
	m_rotationMatrix = rotate(mat4(1.0f), radians(0.0f), vec3(0.0, 0.0, 0.0));
	m_mvpMatrix = (m_projMatrix * m_rotationMatrix) * m_translationMatrix;

	// Create light direction
	m_lightDir = normalize(vec3(1.0, -1.0, 0.0));

	// Upload light direction
	checkGLError("Camera(..) -> Error occured before this call");
	m_lightLoc = glGetUniformLocation(programId, "lightDir");
	glUniform3fv(m_lightLoc, 1, value_ptr(-m_lightDir));
	checkGLError("Camera(..) -> Upload light direction");
}

void Camera::update(){
	// Only do calculations when user is in flying mode
	if (SDL_GetRelativeMouseMode() == SDL_TRUE) {

		// Update camera position based on the key which is pressed
		if (m_keyboardState[SDL_SCANCODE_W])
			m_position += m_viewingDir * m_camSpeed;
		if (m_keyboardState[SDL_SCANCODE_A])
			m_position -= cross(m_viewingDir, vec3(0.0, 1.0, 0.0)) * m_camSpeed;
		if (m_keyboardState[SDL_SCANCODE_S])
			m_position -= m_viewingDir * m_camSpeed;
		if (m_keyboardState[SDL_SCANCODE_D])
			m_position += cross(m_viewingDir, vec3(0.0, 1.0, 0.0)) * m_camSpeed;

		// Increase camera speed if left shift is pressed
		if (m_keyboardState[SDL_SCANCODE_LSHIFT])
			m_camSpeed = 0.8f;
		else
			m_camSpeed = 0.2f;

		// Upload camera position
		glUniform3fv(m_posLoc, 1, value_ptr(m_position));

		// Get relative mouse position
		SDL_GetRelativeMouseState(&m_mousePos.x, &m_mousePos.y);
	}

	// Save mouse position if cursor mode is enabled
	if (SDL_GetRelativeMouseMode() == SDL_FALSE && !m_cursorModeEnabled){
		m_oldMousePos = m_mousePos;
		m_cursorModeEnabled = true;
	}

	// Overwrite mouse position with the saved one if cursor mode is disabled
	if (SDL_GetRelativeMouseMode() == SDL_TRUE && m_cursorModeEnabled){
		m_mousePos = m_oldMousePos;
		m_cursorModeEnabled = false;
	}

    // Update projection matrix if the window was resized
    if (getMainWindow()->resized()) {
        m_projMatrix = perspective(radians(45.0), double(float(getMainWindow()->getWidth()) / float(getMainWindow()->getHeight())), 1.0, 1000.0);
        m_mvpMatrix = (m_projMatrix * m_rotationMatrix) * m_translationMatrix;
        glUniformMatrix4fv(m_mvpLoc, 1, GL_FALSE, value_ptr(m_mvpMatrix));
    }

	// Only do calculations when user is in flying mode
	if (SDL_GetRelativeMouseMode() == SDL_TRUE) {

		// Rotate viewing direction based on mouse movement
		m_rotation.x += float(m_mousePos.x) / 10.0f;
		m_rotation.y += float(m_mousePos.y) / 10.0f;

		// Matrix rotation for updating the viewing direction
		mat4 rotMatrix_x = rotate(mat4(1.0f), radians(m_rotation.x), vec3(0.0, 1.0, 0.0));
		mat4 rotMatrix_y = rotate(mat4(1.0f), radians(m_rotation.y), vec3(1.0, 0.0, 0.0));
		m_rotationMatrix = rotMatrix_y * rotMatrix_x;

		// Calculate new viewing direction
		m_viewingDir = vec3(vec4(m_startViewingDir.x, m_startViewingDir.y, m_startViewingDir.z, 1.0) * m_rotationMatrix);

		// Translation based on the position
		m_translationMatrix = translate(mat4(1.0f), -m_position);

		// Calculate and upload new projection-translation-rotation matrix
		m_mvpMatrix = (m_projMatrix * m_rotationMatrix) * m_translationMatrix;
		glUniformMatrix4fv(m_mvpLoc, 1, GL_FALSE, value_ptr(m_mvpMatrix));

		// Rotate light direction and upload it
		static vec4 startLightDir = vec4(m_lightDir, 1.0);
		m_lightDir = vec3(rotate(mat4(1.0f), radians(m_lightRotation), vec3(0.0, 1.0, 0.0)) * startLightDir);
		m_lightRotation += 0.1f;
		if (m_lightRotation >= 360.0f)
			m_lightRotation = 0.0;
		glUniform3fv(m_lightLoc, 1, value_ptr(-m_lightDir));
	}
}
