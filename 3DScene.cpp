// Dwaine Brannon
// CS 330
// 3D scene

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include <vector>
// GLM Libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float PI = 3.14159f;
const int numSides = 30;
const float height = 1.0f;
const float radius = 0.5f;


// unnamed namespace
namespace {
	const char* const WINDOW_TITLE = "Dwaine's Scene";

	// Window Size
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh {
		GLuint vertexArrayObject;
		GLuint vertexBufferObject;
		GLuint elementBufferObject;
		GLuint numOfIndices; // Number of indices in mesh 
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	//Mesh Data
	GLMesh gMesh;
	// Shader Program
	GLuint gProgramId;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */

bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxhaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);



// Shaders
// --------------------------------
// Vertex Shader Program Source Code
const char* vertexShaderSource = "#version 440 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 colorFromVBO; \n"
"out vec4 colorFromVS; \n"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
"   colorFromVS = colorFromVBO; \n"
"}\0";


// Fragment Shader Program Source Code
const char* fragmentShaderSource = "#version 440 core\n"
"in vec4 colorFromVS; \n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = colorFromVS;\n"
"}\n\0";


// Main function. 
int main(int argc, char* argv[]) {
	

	if (!UInitialize(argc, argv, &gWindow)) {
		return EXIT_FAILURE;
	}

	// Create the Mesh 
	UCreateMesh(gMesh); // Calls function to create the VBO 

	// Create Shader Program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId)) {
		return EXIT_FAILURE;
}

	// Set Backgground color to black (Will be implicitelly used by glClear)
	glClearColor(0.3f, 0.0f, 1.0f, 1.0f);

	// game loop
	// ---------
	while (!glfwWindowShouldClose(gWindow)) {
		// input
		UProcessInput(gWindow);

		// call render function
		URender();

		glfwPollEvents();
	}
	// Release Mesh datta
	UDestroyMesh(gMesh);

	// release shader program
	UDestroyShaderProgram(gProgramId);

	exit(EXIT_SUCCESS); // Terminates the program successfully

}

// Initialize GLFW, GLEW and Create a window

bool UInitialize(int argc, char* argv[], GLFWwindow** window) {

	// initialize and configure GLFW window and specify openGL version)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, FL_TRUE);
#endif 
	// GLFW Window Creation
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);

	// Initialize GLEW
	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	return true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Functioned called to render a frame
void URender()
{	// enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the background
	glClearColor(0.3f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Creating transformations
	glm::mat4 scale = glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));
	glm::mat4 rotation = glm::rotate(glm::radians(45.0f), glm::vec3(0.0, 0.0f, 1.0f));
	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 model = translation * rotation * scale;

	glm::mat4 view = glm::translate(glm::vec3(0.0f, 0.0f, -5.0f));


	// Creates a orthographic projection
	glm::mat4 projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	// Set the shader to be used
	glUseProgram(gProgramId);

	// Retrieves and passes transform matrices to the Shader program
	GLint modelLoc = glGetUniformLocation(gProgramId, "model");
	GLint viewLoc = glGetUniformLocation(gProgramId, "view");
	GLint projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMesh.vertexArrayObject);

	// Draws the objects
	glDrawElements(GL_TRIANGLES, gMesh.numOfIndices, GL_UNSIGNED_INT, 0); // Draws the plane

	// Deactivate the VAO
	glBindVertexArray(0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
	// Specifies Normalized Device Coordinates for the plane that will represent the nintendo switch dock
	GLfloat verts[] = {
		-0.5f, 0.0f, -0.165f, // index 0
		0.0f, 1.0f, 1.0f, 1.0f, // White
		0.5f, 0.0f, -0.165f, // index 1
		1.0f, 1.0f, 1.0f, 1.0f, // white again
		0.5f, 0.0f, 0.165f, // index 2
		1.0f, 0.0f, 1.0f, 1.0f // white
		-0.5f, 0.0f, 0.165f,  // index 3
		1.0f, 1.0f, 0.5f, 1.0f // white
	};

	GLfloat indices[] = {
		0, 1, 2, 
		0, 2, 3
	};
	glGenVertexArrays(1, &mesh.vertexArrayObject); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vertexArrayObject);

	// Create VBO and bind it 
	glGenBuffers(1, &mesh.vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferObject); // activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	mesh.numOfIndices = sizeof(indices) / sizeof(indices[0]);

	glGenBuffers(1, &mesh.elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// Creates the Vertex Attribute Pointer for positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
	// Attribute 1: vertex colors
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	

	//unbind vao and vbo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void UDestroyMesh(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vertexArrayObject);
	glDeleteBuffers(1, &mesh.vertexBufferObject);
	glDeleteBuffers(1, &mesh.elementBufferObject);
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader

	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attatched compiled shaders to the shader program 
	glCompileShader(vertexShaderId);
	glCompileShader(fragmentShaderId);

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glLinkProgram(programId); // Use shader program

	return true;
}


void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}


/* GLSL Error Checking Definitions */
void PrintShaderCompileError(GLuint shader)
{
	int len = 0;
	int chWritten = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWritten, log);
		std::cout << "Shader Compile Error: " << log << std::endl;
		free(log);
	}
}


void PrintShaderLinkingError(int prog)
{
	int len = 0;
	int chWritten = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char*)malloc(len);
		glGetShaderInfoLog(prog, len, &chWritten, log);
		std::cout << "Shader Linking Error: " << log << std::endl;
		free(log);
	}
}


bool IsOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		std::cout << "glError: " << glErr << std::endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

/* GLSL Error Checking Definitions End Here */

