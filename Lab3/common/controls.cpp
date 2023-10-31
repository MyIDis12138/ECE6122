// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
// Initial position : on +Z
glm::vec3 position = glm::vec3( 0.0f, 10.0f, 0.0f); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 0.35*3.14f;
// Initial vertical angle : none
float verticalAngle = 0.25*3.14f;
// Initial Field of View
float initialFoV = 45.0f;
float radius = 10.0f;

float speed = 2.0f; // 3 units / second
float rotationSpeed = 0.1*3.14f;


void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		verticalAngle += rotationSpeed * deltaTime;
	}

	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		verticalAngle -= rotationSpeed * deltaTime;
	}

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		radius -= deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		radius += deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		horizontalAngle -= rotationSpeed * deltaTime;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		horizontalAngle += rotationSpeed * deltaTime;
	}
	verticalAngle = glm::clamp(verticalAngle, -1.57f, 1.57f);


	position = glm::vec3(
		radius * sin(verticalAngle) * sin(horizontalAngle),
		radius * sin(verticalAngle) * cos(horizontalAngle),
		radius * cos(verticalAngle)
	);

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		cos(verticalAngle) * cos(horizontalAngle),
		sin(verticalAngle)
	);

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position + direction,           // Camera is here
								origin, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}