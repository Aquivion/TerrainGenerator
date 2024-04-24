#pragma once

#include "glm.h"
#include "window.h"
#include "shader.h"
#include "error.h"

/**
* \brief The camera class. Allows the user to fly around the scene.
*
* The user can fly around with the camera to look at the scene from different perspctives,
* using W,A,S,D for movement and the mouse for changing the viewing direction. The movement
* depends on the viewing direction. Pressing left shift increases the movement speed. To
* switch between cursor- and flying-mode, "Space" needs to be pressed. The should always be
* only one instance of this class in the whole program. There should be only one instance of
* this class in the whole application.
*/
class Camera
{
public:
	/**
	* \brief Constructs the camera object.
	*
	* \param[in] position 3D-Position of the camera after the application has started.
	*
	* \param[in] programId Id of the current using shader program. Used to upload the matrices and the camera position to shader.
	*/
	Camera(vec3 position, const GLuint programId);

	/**
	* \brief Updates camera position and the viewing direction based on the user's input.
	*/
	void update();

private:
	/**
	* \brief Position in 3D-space.
	*/
	vec3 m_position;

	/**
	* \brief Viewing direction.
	*/
	vec3 m_viewingDir;

	/**
	* \brief First viewing direction after program has started. Used for correct rotation calculations.
	*/
	vec3 m_startViewingDir;

	/**
	* \brief Rotation of the camera on x- and y-axis.
	*/
	vec2 m_rotation;

	/**
	* \brief Speed of the camera movement. Increases when holding down left shift.
	*/
	float m_camSpeed;

	/**
	* \brief Location of the model-view-projection matrix in the shader program. Used for uploading the matrix to the shader program.
	*/
	GLuint m_mvpLoc;

	/**
	* \brief Location of the current camera position in the shader program.
	*
	* This is uploaded to calculate the distance between the camera and each vertex.
	* The distance determines when and how intense the detail-texture (generated from
	* a normal map of a seamless noise height map) is seen by the user.
	*/
	GLuint m_posLoc;

	/**
	* \brief Mouse postion on x- and y-axis.
	*/
	ivec2 m_mousePos;
	/**
	* \brief Old mouse position on x- and y-axis. Used for when the user switches to cursor mode.
	*/
	ivec2 m_oldMousePos;

	/**
	* \brief Determines if the user is in cursor- or flying mode.
	*/
	bool m_cursorModeEnabled;

	/**
	* \brief Array of keyboard scan-codes.
	*/
	const Uint8 *m_keyboardState;

	/**
	* \brief The projection matrix.
	*/
	mat4 m_projMatrix;

	/**
	* \brief The model matrix.
	*/
	mat4 m_translationMatrix;

	/**
	* \brief A rotation matrix that the camera rotation on x- and y-axis is used as the general view matrix.
	*/
	mat4 m_rotationMatrix;

	/**
	* \brief The model view projection matrix that is uploaded to the shader program.
	*/
	mat4 m_mvpMatrix;

	/**
	* \brief Current rotation of the light direction
	*/
	float m_lightRotation;

	/**
	* \brief Light direction
	*/
	vec3 m_lightDir;

	/**
	* \brief Location id for the light direction in the shader program.
	*/
	GLuint m_lightLoc;
};

