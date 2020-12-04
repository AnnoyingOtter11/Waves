//
//  WaterPlane.cpp
//
//   Sets up and renders 
//     - the ground plane, and
//


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR4.h"		// Adjust path as needed.

#include "WaterPlane.h"
#include "WavePhongData.h"

extern int meshRes;
extern int modelviewMatLocation;       // Defined in GlslWaves.cpp
extern bool check_for_opengl_errors();

unsigned int vertPos_loc = 0;    // Corresponds to "location = 0" in the verter shader definitions
unsigned int vertNormal_loc = 1; // Corresponds to "location = 1" in the verter shader definitions

// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************
const int NumObjects = 1;
const int iFloor = 0;

unsigned int myVBO[NumObjects];  // a Vertex Buffer Object holds an array of data
unsigned int myVAO[NumObjects];  // a Vertex Array Object - holds info about an array of vertex data;
unsigned int myEBO[NumObjects];  // a Element Array Buffer Object - holds an array of elements (vertex indices)

// **********************
// This sets up geometries needed for the "Initial" (the 3-D alphabet letter)
//  It is called only once.
// **********************
void MySetupSurfaces() {
    // Initialize the VAO's, VBO's and EBO's for the ground plane,
    // No data is loaded into the VBO's or EBO's until the "Remesh"
    //   routines are called.

    // For the floor:
    // Allocate the needed Vertex Array Objects (VAO's),
    //      Vertex Buffer Objects (VBO's) and Element Array Buffer Objects (EBO's)
    glGenVertexArrays(NumObjects, &myVAO[0]);
    glGenBuffers(NumObjects, &myVBO[0]);
    glGenBuffers(NumObjects, &myEBO[0]);

    glBindVertexArray(myVAO[iFloor]);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iFloor]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iFloor]);

    glVertexAttribPointer(vertPos_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Store vertices in the VBO
    glEnableVertexAttribArray(vertPos_loc);									// Enable the stored vertices
 
    // No data has been loaded into the VBO yet.
    // This is done next by the "Remesh" routine.

    RemeshFloor();
    
    check_for_opengl_errors();      // Watch the console window for error messages!
}

// **********************************************
// MODIFY THIS ROUTINE TO CALL YOUR OWN CODE IN
//   MyRemeshFloor AND MyRemeshCircularSurf
// INSTEAD OF THE "DEMO" VERSIONS.
// **********************************************

void MyRenderSurfaces() {
    RenderFloor();
    check_for_opengl_errors();      // Watch the console window for error messages!
}

// *********************************************
// A water plane gridded as an array of rectangles (triangulated)
// x and z values are in the range [-5,5].
// All points stored here with y value equal to zero.
// **** Heights are changed by the vertex shader.****
// *********************************************************

void RemeshFloor()
{

    // Floor (water plane) vertices.
    int numFloorVerts = (meshRes + 1)*(meshRes + 1);    
    float* floorVerts = new float[3 * numFloorVerts];
    // Floor elements (indices to vertices in a triangle strip)
    int numFloorElts = meshRes * 2 * (meshRes + 1);     
    unsigned int* floorElements = new unsigned int[numFloorElts];

    for (int i = 0; i <= meshRes; i++) {
        float z = (-5.0f*(float)(meshRes - i) + 5.0f*(float)i) / (float)meshRes;
        for (int j = 0; j <= meshRes; j++) {
            float x = (-5.0f*(float)(meshRes - j) + 5.0f*(float)j) / (float)meshRes;
            int vrtIdx = 3 * i * (meshRes + 1) + 3 * j;
            floorVerts[vrtIdx] = x;
            floorVerts[vrtIdx + 1] = 0.0f;
            floorVerts[vrtIdx + 2] = z;
        }
    }
    for (int i = 0; i < meshRes; i++) {
        for (int j = 0; j <= meshRes; j++) {
            int elt = 2 * i * (meshRes + 1) + 2 * j;
            floorElements[elt] = i * (meshRes + 1) + j;
            floorElements[elt + 1] = (i + 1) * (meshRes + 1) + j;
        }
    }

    // Load data into the VBO and EBO using glBindBuffer and glBufferData commands
    glBindVertexArray(myVAO[iFloor]);
    glBindBuffer(GL_ARRAY_BUFFER, myVBO[iFloor]);
    glBufferData(GL_ARRAY_BUFFER, 3 * numFloorVerts * sizeof(float), floorVerts, GL_STATIC_DRAW);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myEBO[iFloor]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFloorElts * sizeof(unsigned int), floorElements, GL_STATIC_DRAW); 

    // Avoid a memory leak by deleting the arrays obtained with "new" above
    delete[] floorVerts;
    delete[] floorElements;
}

void RenderFloor() {

    glBindVertexArray(myVAO[iFloor]);

    // Set the uniform values (they are not stored with the VAO and thus must be set again everytime
    glVertexAttrib3f(vertNormal_loc, 0.0, 1.0, 0.0);    // Generic vertex attribute: Normal is (0,1,0) for the floor.
    myMaterials[0].LoadIntoShaders();                   // materials[0] defined in DemoPhongData.h
    glUniformMatrix4fv(modelviewMatLocation, 1, false, viewMatrix.DumpByColumns());

    // Draw the the triangle strips
    for (int i = 0; i < meshRes; i++) {
        glDrawElements(GL_TRIANGLE_STRIP, 2 * (meshRes + 1), GL_UNSIGNED_INT, 
            (void*)(i * 2* (meshRes + 1) * sizeof(unsigned int)));
    }
}

