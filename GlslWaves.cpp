/*
 * LetterProj.cpp - Version 0.3 - April 13, 2019

 *
 * Software accompanying POSSIBLE SECOND EDITION TO the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG2
 */

// These libraries are needed to link the program.
// First five are usually provided by the system.
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR2.h"
#include "LinearR3.h"		// Adjust path as needed.
#include "LinearR4.h"		// Adjust path as needed.
#include "EduPhong.h"
#include "WavePhongData.h"
#include "GlGeomSphere.h"
#include "GlShaderMgr.h"
#include "GlslWaves.h"
#include "WaterPlane.h"

// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFW!
#include <stdio.h>




// ********************
// Animation controls and state infornation
// ********************

// These variables control the view direction.
//    The arrow keys are used to change these values.
double viewAzimuth = 0.25;	// Angle of view up/down (in radians)
double viewDirection = 0.0; // Rotation of view around y-axis (in radians)
double deltaAngle = 0.01;	// Change in view angle for each up/down/left/right arrow key press
LinearMapR4 viewMatrix;		// The current view matrix, based on viewAzimuth and viewDirection.

// Control Phong lighting modes
phGlobal globalPhongData;

// These two variables control how triangles are rendered.
bool wireframeMode = false;	// Equals true for polygon GL_FILL mode. False for polygon GL_LINE mode.
bool cullBackFaces = true;
bool timer = true;
bool rtime = true;
bool rebound = false;
float tTime = 0;

// The next variable controls the resoluton of the meshes for the water plane.
int meshRes=4;

// Mode==0 for one wave. Mode==1 for one wave and smaller ripple.
int waveMode = 0;    
int mouseClick = 0;

//Window Size
const int initWidth = 800;
const int initHeight = 600;

// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************

unsigned int myWaveShader;                  // The shader program!
int projMatLocation;						// Location of the projectionMatrix in the currently active shader program
int modelviewMatLocation;					// Location of the modelviewMatrix in the currently active shader program

// These two locations are not in the supplied shader code.
// You will add them to your shader (GLSL) code)
int timeLocation;
int waveModeLocation;
float amp = 0.5;
float rxp, rzp = 0.0;
float rebound1, rebound2 = 0.0;
float rx, rz = 5.5;
double xpos, ypos, newx, newz;
int xloc, zloc, ampLoc, rxLoc, rzLoc, rxpLoc, rzpLoc, rebound1Loc, rebound2Loc;

//  The Projection matrix: Controls the "camera view/field-of-view" transformation
//     Geneally is the same for all objects in the scene.
LinearMapR4 theProjectionMatrix;		//  The Projection matrix: Controls the "camera/view" transformation

// A ModelView matrix controls the placement of a particular object in 3-space.
//     It is generally different for each object.
// The array matEntries holds the matrix values as floats to be loaded into the shader program. 
float matEntries[16];		// Holds 16 floats (since cannot load doubles into a shader that uses floats)

// *****************************
// These variables set the dimensions of the perspective region we wish to view.
// They are used to help form the projection matrix and the view matrix
// All rendered objects lie in the rectangular prism centered on the z-axis
//     equal to (-Xmax,Xmax) x (-Ymax,Ymax) x (Zmin,Zmax)
// Be sure to leave some slack in these values, to allow for rotations, etc.
// The model/view matrix can be used to move objects to this position
// THESE VALUES MAY NEED AD-HOC ADJUSTMENT TO GET THE SCENE TO BE VISIBLE.
const double Xmax = 6.0;                // Control x dimensions of viewable scene
const double Ymax = 4.0;                // Control y dimensions of viewable scene
const double Zmin = -8.0, Zmax = 8.0;   // Control z dimensions of the viewable scene
// zNear equals the distance from the camera to the z = Zmax plane
const double zNear = 15.0;              // Make this value larger or smaller to affect field of view.


// *************************
// mySetupGeometries defines the scene data, especially vertex  positions and colors.
//    - It also loads all the data into the VAO's (Vertex Array Objects) and
//      into the VBO's (Vertex Buffer Objects).
// This routine is only called once to initialize the data.
// *************************
void mySetupGeometries() {
 
    MySetupSurfaces();

     mySetViewMatrix();

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void mySetViewMatrix() {
    // Set the view matrix. Sets view distance, and view direction.
    // The final translation is done because the ground plane lies in the xz-plane,
    // YOU MAY NEED TO ADJUST THE FINAL TRANSLATION.
    viewMatrix.Set_glTranslate(0.0, 0.0, -(Zmax + zNear));      // Translate to be in front of the camera
    viewMatrix.Mult_glRotate(viewAzimuth, 1.0, 0.0, 0.0);	    // Rotate viewAzimuth radians around x-axis
    viewMatrix.Mult_glRotate(-viewDirection, 0.0, 1.0, 0.0);    // Rotate -viewDirection radians around y-axis
    viewMatrix.Mult_glTranslate(0.0, -2.5, 0.0);                // Translate the view is above the water plane
}

// *************************************
// Main routine for rendering the scene
// myRenderScene() is called every time the scene needs to be redrawn.
// mySetupGeometries() has already created the vertex and buffer objects
//    and the model view matrices.
// The EduPhong shaders should already be setup.
// *************************************
void myRenderScene() {
    float currentTime = (float)glfwGetTime();
	if (timer) {
		tTime = (float)glfwGetTime();
		timer = false;
	}
	//printf("currentTime: %f\n", currentTime);

	if (currentTime > 30.0) {
		waveMode = 0;
	}

	if (waveMode == 0) {
		amp = 0.5;
		rx = 2.00;
		rxp = 0.0;
		rz = 0.0;
		currentTime = 0.0;
		tTime = 0.0;
		timer = true;
		rebound = false;
		rebound1 = sqrt(50)+2;
		rebound2 = sqrt(50);
	}

	if (rx > sqrt(50)) {
		rebound = true;
	}

	if (currentTime - tTime > 0.01) {
			rx = rx + 0.0166666;
			rxp = rxp + 0.0166666;
			rz = rz + 0.005;
			timer = true;
			if (rebound) {
				rebound1 = rebound1 - 0.0166666;
				rebound2 = rebound2 - 0.0166666;
				//printf("rebounds: %f %f\n", rebound1, rebound2);
			}
	}

    // Clear the rendering window
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const float clearDepth = 1.0f;
    glClearBufferfv(GL_COLOR, 0, black);
    glClearBufferfv(GL_DEPTH, 0, &clearDepth);	// Must pass in a *pointer* to the depth

    glUseProgram(myWaveShader);

    // Should really check if timeLocation and waveModeLocation are -1 before calling these,
    //    but it seems to be OK to just call them anyway.
    glUniform1f(timeLocation, currentTime);     // Set the current time uniform variable in the shader
    glUniform1i(waveModeLocation, waveMode);    // Set the uniform variable in the shader
	glUniform1f(ampLoc, amp);
	glUniform1f(rxLoc, rx);
	glUniform1f(rzLoc, rz);
	glUniform1f(rzpLoc, rzp);
	glUniform1f(rxpLoc, rxp);
	glUniform1f(rebound1Loc, rebound1);
	glUniform1f(rebound2Loc, rebound2);
	
    MyRenderSurfaces();
    MyRenderSpheresForLights();

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_SceneData() {
	mySetupGeometries();


    // **** You should add "LoadShaderSource" commands if you make more GLSL files. **** 
    GlShaderMgr::LoadShaderSource("EduPhong.glsl");
	GlShaderMgr::LoadShaderSource("shaders2.c");

    unsigned int shader_vert = GlShaderMgr::CompileShader("vertexShader_PhongPhong2");
    unsigned int shader_frag = GlShaderMgr::CompileShader("fragmentShader_PhongPhong2", "calcPhongLighting", "applyTextureMap");
    unsigned int shaders_progs[2] = { shader_vert, shader_frag };
    myWaveShader = GlShaderMgr::LinkShaderProgram(2, shaders_progs);
    phRegisterShaderProgram(myWaveShader);
    check_for_opengl_errors();

    projMatLocation = glGetUniformLocation(myWaveShader, phProjMatName);
    modelviewMatLocation = glGetUniformLocation(myWaveShader, phModelviewMatName);
    // Similar commands to the next line are used to get the locations 
    //    for Uniform variables that you add to the shader programs.
    // glGetUniformLocation returns "-1" if the variable is not in the shader program.
    timeLocation = glGetUniformLocation(myWaveShader, "curTime");
    waveModeLocation = glGetUniformLocation(myWaveShader, "waveMode");
	xloc = glGetUniformLocation(myWaveShader, "x");
	zloc = glGetUniformLocation(myWaveShader, "z");
	ampLoc = glGetUniformLocation(myWaveShader, "amp");
	rxLoc = glGetUniformLocation(myWaveShader, "rx");
	rzLoc = glGetUniformLocation(myWaveShader, "rz");
	rxpLoc = glGetUniformLocation(myWaveShader, "rxp");
	rzpLoc = glGetUniformLocation(myWaveShader, "rzp");
	rebound1Loc = glGetUniformLocation(myWaveShader, "rebound1");
	rebound2Loc = glGetUniformLocation(myWaveShader, "rebound2");
    check_for_opengl_errors();

    MySetupGlobalLight();
    MySetupLights();
    LoadAllLights();
    MySetupMaterials();

    //glfwSetTime(0.0);

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

// *******************************************************
// Process all key press events.
// This routine is called each time a key is pressed or released.
// *******************************************************
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static const double Pi = 3.1415926535f;
    if (action == GLFW_RELEASE) {
        return;			// Ignore key up (key release) events
    }
    bool viewChanged = false;
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        return;
    case '4':
    case '1':
    case '2':
    case '3':
    {
        phLight& theLight = myLights[key - '1'];
        theLight.IsEnabled = !theLight.IsEnabled;   // Toggle whether the light is enabled.
        LoadAllLights();
        return;
    }
    case 'B':
        //waveMode = (waveMode + 1) % 2;
        return;
    case 'W':		// Toggle wireframe mode
        if (wireframeMode) {
            wireframeMode = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            wireframeMode = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        return;
    case 'C':		// Toggle backface culling
        cullBackFaces = !cullBackFaces;     // Negate truth value of cullBackFaces
        if (cullBackFaces) {
            glEnable(GL_CULL_FACE);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        return;
    case 'M':
        if (mods & GLFW_MOD_SHIFT) {
            meshRes = meshRes < 79 ? meshRes + 1 : 80;  // Uppercase 'M'
        }
        else {
            meshRes = meshRes > 4 ? meshRes - 1 : 3;    // Lowercase 'm'
        }
        RemeshFloor();
        return;
    case GLFW_KEY_UP:
        viewAzimuth = Min(viewAzimuth + 0.01, PIfourths - 0.05);
        viewChanged = true;
        break;
    case GLFW_KEY_DOWN:
        viewAzimuth = Max(viewAzimuth - 0.01, -PIfourths + 0.05);
        viewChanged = true;
        break;
    case GLFW_KEY_RIGHT:
        viewDirection += 0.01;
        if (viewDirection > PI) {
            viewDirection -= PI2;
        }
        viewChanged = true;
        break;
    case GLFW_KEY_LEFT:
        viewDirection -= 0.01;
        if (viewDirection < -PI) {
            viewDirection += PI2;
        }
        viewChanged = true;
        break;
    case GLFW_KEY_A:
        globalPhongData.EnableAmbient = !globalPhongData.EnableAmbient;
        break;
    case GLFW_KEY_E:
        globalPhongData.EnableEmissive = !globalPhongData.EnableEmissive;
        break;
    case GLFW_KEY_D:
        globalPhongData.EnableDiffuse = !globalPhongData.EnableDiffuse;
        break;
    case GLFW_KEY_S:
        globalPhongData.EnableSpecular = !globalPhongData.EnableSpecular;
        break;
    case GLFW_KEY_V:
        globalPhongData.LocalViewer = !globalPhongData.LocalViewer;
        break;

    }
    // Might have updated the global phong data above: upload it to the shader program.
    globalPhongData.LoadIntoShaders();

    if (viewChanged) {
        mySetViewMatrix();
        LoadAllLights();        // Have to call this since it affects the position of the lights!
    }
    else {
        // Updated the global phong data above: upload it to the shader program.
        globalPhongData.LoadIntoShaders();
    }
}

// *******************************************************
// Process all mouse button events.  - Mostly for debugging
// *******************************************************
void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwSetTime(0.0);
		waveMode = (waveMode + 1) % 2;
		glfwGetCursorPos(window, &xpos, &ypos);
		printf("Mouse click at: %d, %d.\n", (int)xpos, (int)ypos);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float cxpos = (xpos - width / 2) / (width / 2);
		float cypos = (-ypos + height/2) / (height / 2);

		// Solving for x,z coordinates
		VectorR3 xy = VectorR3(cxpos, cypos, 1);
		LinearMapR4 multiMat = theProjectionMatrix * viewMatrix;
		LinearMapR3 threeMat = LinearMapR3(multiMat.m11, multiMat.m21, multiMat.m41, multiMat.m13, multiMat.m23, multiMat.m43, multiMat.m14, multiMat.m24, multiMat.m44 );
		VectorR3 xz = threeMat.Inverse() * xy;
		newx = xz.x / xz.z;
		newz = xz.y / xz.z;
		glUniform1f(xloc, newx);
		glUniform1f(zloc, newz);
		printf("Model position at: %f, %f.\n", newx, newz);
	}
}


// *************************************************
// This function is called with the graphics window is first created,
//    and again whenever it is resized.
// The Projection View Matrix is typically set here.
//    But this program does not use any transformations or matrices.
// *************************************************
void window_size_callback(GLFWwindow* window, int width, int height) {
	// Define the portion of the window used for OpenGL rendering.
	glViewport(0, 0, width, height);

	// Setup the projection matrix as a perspective view.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
	double w = (width == 0) ? 1.0 : (double)width;
	double h = (height == 0) ? 1.0 : (double)height;
	double windowXmax, windowYmax;
    double aspectFactor = w * Ymax / (h * Xmax);   // == (w/h)/(Xmax/Ymax), ratio of aspect ratios
	if (aspectFactor>1) {
		windowXmax = Xmax * aspectFactor;
		windowYmax = Ymax;
	}
	else {
		windowYmax = Ymax / aspectFactor;
		windowXmax = Xmax;
	}

	// Using the max & min values for x & y & z that should be visible in the window,
	//		we set up the orthographic projection.
    double zFar = zNear + Zmax - Zmin;
	theProjectionMatrix.Set_glFrustum(-windowXmax, windowXmax, -windowYmax, windowYmax, zNear, zFar);

    if (glIsProgram(myWaveShader)) {
        glUseProgram(myWaveShader);
        theProjectionMatrix.DumpByColumns(matEntries);
        glUniformMatrix4fv(projMatLocation, 1, false, matEntries);
    }
    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_OpenGL() {
	
	glEnable(GL_DEPTH_TEST);	// Enable depth buffering
	glDepthFunc(GL_LEQUAL);		// Useful for multipass shaders

	// Set polygon drawing mode for front and back of each polygon
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_CULL_FACE);

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void error_callback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void setup_callbacks(GLFWwindow* window) {
	// Set callback function for resizing the window
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	// Set callback for key up/down/repeat events
	glfwSetKeyCallback(window, key_callback);

	// Set callbacks for mouse movement (cursor position) and mouse botton up/down events.
	// glfwSetCursorPosCallback(WindowID, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Set callbacks for mouse movement (cursor position) and mouse botton up/down events.
	// glfwSetCursorPosCallback(window, cursor_pos_callback);
	// glfwSetMouseButtonCallback(window, mouse_button_callback);
}

int main() {
	glfwSetErrorCallback(error_callback);	// Supposed to be called in event of errors. (doesn't work?)
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

	GLFWwindow* window = glfwCreateWindow(initWidth, initHeight, "Phong Demo", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window!\n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit()) {
		printf("Failed to initialize GLEW!.\n");
		return -1;
	}

	// Print info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
#ifdef GL_SHADING_LANGUAGE_VERSION
	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
    printf("Using GLEW version %s.\n", glewGetString(GLEW_VERSION));

	printf("------------------------------\n");
    printf("Press arrow keys to adjust the view direction.\n   ");
    printf("Press 'B' (both) to toggle whether one wave or both waves are generated.\n");
    printf("Press 'w' or 'W' (wireframe) to toggle whether wireframe or fill mode.\n");
    printf("Press 'M' (mesh) to increase the mesh resolution.\n");
    printf("Press 'm' (mesh) to decrease the mesh resolution.\n");
    printf("Press 'F'(faster) or 'f' (slower) to speed up or slow down the animation.\n");
    printf("Press '1', '2', '3' to toggle the three lights.\n");
    printf("Press '4' to toggle the spotlight.\n");
    printf("Press ESCAPE or 'X' or 'x' to exit.\n");
    printf("Press 'E' key (Emissive) to toggle rendering Emissive light.\n");
    printf("Press 'A' key (Ambient) to toggle rendering Ambient light.\n");
    printf("Press 'D' key (Diffuse) to toggle rendering Diffuse light.\n");
    printf("Press 'S' key (Specular) to toggle rendering Specular light.\n");
    printf("Press 'V' key (Viewer) to toggle using a local viewer.\n");
    printf("Press ESCAPE to exit.\n");
	
    setup_callbacks(window);
   
	// Initialize OpenGL, the scene and the shaders
    my_setup_OpenGL();
	my_setup_SceneData();
 	window_size_callback(window, initWidth, initHeight);

    // Loop while program is not terminated.
	while (!glfwWindowShouldClose(window)) {
	
		myRenderScene();				// Render into the current buffer
		glfwSwapBuffers(window);		// Displays what was just rendered (using double buffering).

		// Poll events (key presses, mouse events)
		glfwWaitEventsTimeout(1.0/60.0);	    // Use this to animate at 60 frames/sec (timing is NOT reliable)
		// glfwWaitEvents();					// Or, Use this instead if no animation.
		// glfwPollEvents();					// Use this version when animating as fast as possible
	}

	glfwTerminate();
	return 0;
}

// If an error is found, it could have been caused by any command since the
//   previous call to check_for_opengl_errors()
// To find what generated the error, you can try adding more calls to
//   check_for_opengl_errors().
char errNames[8][36] = {
	"Unknown OpenGL error",
	"GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION",
	"GL_INVALID_FRAMEBUFFER_OPERATION", "GL_OUT_OF_MEMORY",
	"GL_STACK_UNDERFLOW", "GL_STACK_OVERFLOW" };
bool check_for_opengl_errors() {
	int numErrors = 0;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		numErrors++;
		int errNum = 0;
		switch (err) {
		case GL_INVALID_ENUM:
			errNum = 1;
			break;
		case GL_INVALID_VALUE:
			errNum = 2;
			break;
		case GL_INVALID_OPERATION:
			errNum = 3;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errNum = 4;
			break;
		case GL_OUT_OF_MEMORY:
			errNum = 5;
			break;
		case GL_STACK_UNDERFLOW:
			errNum = 6;
			break;
		case GL_STACK_OVERFLOW:
			errNum = 7;
			break;
		}
		printf("OpenGL ERROR: %s.\n", errNames[errNum]);
	}
	return (numErrors != 0);
}