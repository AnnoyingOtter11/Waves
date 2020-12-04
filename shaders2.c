// **************
// The vertex shader for Phong lighting with Phong shading.
//   Mostly this copies material values, modelview position,
//   and modelview surface normal to the fragment shader
// **************
#beginglsl vertexshader vertexShader_PhongPhong2
#version 330 core
layout (location = 0) in vec3 vertPos;         // Position in attribute location 0
layout (location = 1) in vec3 vertNormal;      // Surface normal in attribute location 1
layout (location = 2) in vec2 vertTexCoords;   // Texture coordinates in attribute location 2
layout (location = 3) in vec3 EmissiveColor;   // Surface material properties 
layout (location = 4) in vec3 AmbientColor; 
layout (location = 5) in vec3 DiffuseColor; 
layout (location = 6) in vec3 SpecularColor; 
layout (location = 7) in float SpecularExponent; 
layout (location = 8) in float UseFresnel;		// Shold be 1.0 (for Fresnel) or 0.0 (for no Fresnel)

out vec3 mvPos;         // Vertex position in modelview coordinates
out vec3 mvNormalFront; // Normal vector to vertex in modelview coordinates
out vec3 matEmissive;
out vec3 matAmbient;
out vec3 matDiffuse; 
out vec3 matSpecular;
out float matSpecExponent;
out vec2 theTexCoords;
out float useFresnel;
out vec3 vertPosNew;

uniform mat4 projectionMatrix;        // The projection matrix
uniform mat4 modelviewMatrix;         // The modelview matrix
uniform float curTime;
uniform int waveMode;
uniform float x;
uniform float z;
uniform float amp;
uniform float rx;
uniform float rz;
uniform float rxp;
uniform float rzp;
uniform float rebound1;
uniform float rebound2;


void main()
{
		float pi = 3.1415926535;
		float lambda = 2.0;
		float freq = 0.25;

		vertPosNew.x = vertPos.x;
		vertPosNew.z = vertPos.z;

		

		if (waveMode == 1) {
			float d = sqrt(pow(vertPosNew.x - x, 2) + pow(vertPosNew.z - z, 2));
			float d31 = sqrt(pow(vertPosNew.x - (-5 - 2*x), 2) + pow(vertPosNew.z - (-5-2*z), 2));
			float d32 = sqrt(pow(vertPosNew.x - x, 2) + pow(vertPosNew.z -(-5 - 2*z), 2));
			float d33 = sqrt(pow(vertPosNew.x - (10 - 2*x), 2) + pow(vertPosNew.z -(-5 - 2*z), 2));
			float d21 = sqrt(pow(vertPosNew.x - (-5 - 2*x), 2) + pow(vertPosNew.z - z, 2));
			float d23 = sqrt(pow(vertPosNew.x - (10-2*x), 2) + pow(vertPosNew.z - z, 2));
			float d11 = sqrt(pow(vertPosNew.x - (-5 - 2*x), 2) + pow(vertPosNew.z -(10 - 2*z), 2));
			float d12 = sqrt(pow(vertPosNew.x - x, 2) + pow(vertPosNew.z - (10 - 2*z), 2));
			float d13 = sqrt(pow(vertPosNew.x -(10 - 2*x), 2) + pow(vertPosNew.z - (10 - 2*z), 2));

			//float e = sqrt(pow(vertPosNew.x, 2) + pow(vertPosNew.z + 8, 2));
			/**
			if ((vertPosNew.x > rxp && vertPosNew.x < rx || (-vertPosNew.x) > rxp && (vertPosNew.x) < rx)
				 && (vertPosNew.z > rzp && vertPosNew.z < rz || (-vertPosNew.z) > rzp && (vertPosNew.z) < rz)) {
				vertPosNew.y = amp * cos(2 * pi*(d / lambda - freq * curTime));
			}
			*/
				
			if (rx > sqrt(50)) {
				if (d < rebound1 && d > rebound2) {
					vertPosNew.y = amp * pow(exp(1.0), -rz) * sin(2 * pi*(d / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d21 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d12 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d23 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d32 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d11 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d13 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d31 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d33 / lambda - freq * curTime));
				}
				if (d < rebound2) {
					vertPosNew.y = amp * pow(exp(1.0), -rz) * sin(2 * pi*(d / lambda - freq * curTime));
					//vertPosNew.y = 0;
				}
				
				if (d > rebound1 && d < 2 * (sqrt(50)) - d) {
					vertPosNew.y = amp * pow(exp(1.0), -rz) * sin(2 * pi*(d / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d21 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d12 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d23 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d32 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d11 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d13 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d31 / lambda - freq * curTime)) +
						amp * pow(exp(1.0), -rz) * sin(2 * pi*(d33 / lambda - freq * curTime));
				}
				if (d > 2 * (sqrt(50)) - d) {
					vertPosNew.y = 0;
				}
			}

			else if (d > rxp && d < rx) {
				vertPosNew.y = amp * pow(exp(1.0), -rz) * sin(2 * pi*(d / lambda - freq * curTime));
			}
			
			else if (d < rxp) {
				vertPosNew.y = amp * pow(exp(1.0), -rz) * sin(2 * pi*(d / lambda - freq * curTime));
			}
			
			
			else {
				vertPosNew.y = 0;
			}

			vec4 mvPos4 = modelviewMatrix * vec4(vertPosNew.x, vertPosNew.y, vertPosNew.z, 1.0);
			gl_Position = projectionMatrix * mvPos4;
			mvPos = vec3(mvPos4.x, mvPos4.y, mvPos4.z) / mvPos4.w;
			mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal); // Unit normal from the surface 
			matEmissive = EmissiveColor;
			matAmbient = AmbientColor;
			matDiffuse = DiffuseColor;
			matSpecular = SpecularColor;
			matSpecExponent = SpecularExponent;
			theTexCoords = vertTexCoords;
			useFresnel = UseFresnel;
			
		}

		else {
			vec4 mvPos4 = modelviewMatrix * vec4(vertPos.x, vertPos.y, vertPos.z, 1.0);
			gl_Position = projectionMatrix * mvPos4;
			mvPos = vec3(mvPos4.x, mvPos4.y, mvPos4.z) / mvPos4.w;
			mvNormalFront = normalize(inverse(transpose(mat3(modelviewMatrix)))*vertNormal); // Unit normal from the surface 
			matEmissive = EmissiveColor;
			matAmbient = AmbientColor;
			matDiffuse = DiffuseColor;
			matSpecular = SpecularColor;
			matSpecExponent = SpecularExponent;
			theTexCoords = vertTexCoords;
			useFresnel = UseFresnel;
		}
	
}
#endglsl






// **************
// The base code for the fragment shader for Phong lighting with Phong shading.
//   This does all the hard work of the Phong lighting by calling CalculatePhongLighting()
// **************
#beginglsl fragmentshader fragmentShader_PhongPhong2
#version 330 core

in vec3 mvPos;         // Vertex position in modelview coordinates
in vec3 mvNormalFront; // Normal vector to vertex (front facing) in modelview coordinates
in vec3 matEmissive;
in vec3 matAmbient;
in vec3 matDiffuse;
in vec3 matSpecular;
in vec3 vertPosNew;
in float matSpecExponent;
in float useFresnel;

layout (std140) uniform phGlobal { 
    vec3 GlobalAmbientColor;        // Global ambient light color 
    int NumLights;                  // Number of lights 
    bool LocalViewer;               // true for local viewer; false for directional viewer 
    bool EnableEmissive;            // Control whether emissive colors are rendered 
    bool EnableDiffuse;             // Control whether diffuse colors are rendered 
    bool EnableAmbient;             // Control whether ambient colors are rendered 
    bool EnableSpecular;            // Control whether specular colors are rendered 
	bool UseHalfwayVector;			// Control whether halfway vector method is used
};

const int MaxLights = 8;        // The maximum number of lights (must match value in C++ code)
struct phLight { 
    bool IsEnabled;             // True if light is turned on 
    bool IsAttenuated;          // True if attenuation is active 
    bool IsSpotLight;           // True if spotlight 
    bool IsDirectional;         // True if directional 
    vec3 Position; 
    vec3 AmbientColor; 
    vec3 DiffuseColor; 
    vec3 SpecularColor; 
    vec3 SpotDirection;         // Should be unit vector! 
    float SpotCosCutoff;        // Cosine of cutoff angle 
    float SpotExponent; 
    float ConstantAttenuation; 
    float LinearAttenuation; 
    float QuadraticAttenuation; 
};
layout (std140) uniform phLightArray { 
    phLight Lights[MaxLights];
};

vec3 mvNormal; 
in vec2 theTexCoords;          // Texture coordinates (interpolated from vertex shader) 
uniform sampler2D theTextureMap;
uniform bool applyTexture;
uniform float curTime;
uniform int waveMode;
uniform float amp;
uniform float x;
uniform float z;
uniform mat4 modelviewMatrix;         // The modelview matrix
uniform float rx;
uniform float rz;
uniform float rxp;
uniform float rzp;
uniform float rebound1;
uniform float rebound2;

vec3 nonspecColor;
vec3 specularColor;  
out vec4 fragmentColor;         // Color that will be used for the fragment

void CalculatePhongLighting();  // Calculates: nonspecColor and specularColor. 
vec4 applyTextureFunction();

void main() { 
float pi = 3.1415926535;
float lambda = 2.0;
float freq = 0.25;
float dx = 0.0;
float dz = 0.0;
float d = sqrt(pow(vertPosNew.x - x, 2) + pow(vertPosNew.z - z, 2));
float d31 = sqrt(pow(vertPosNew.x - (-5 - 2 * x), 2) + pow(vertPosNew.z - (-5 - 2 * z), 2));
float d32 = sqrt(pow(vertPosNew.x - x, 2) + pow(vertPosNew.z - (-5 - 2 * z), 2));
float d33 = sqrt(pow(vertPosNew.x - (10 - 2 * x), 2) + pow(vertPosNew.z - (-5 - 2 * z), 2));
float d21 = sqrt(pow(vertPosNew.x - (-5 - 2 * x), 2) + pow(vertPosNew.z - z, 2));
float d23 = sqrt(pow(vertPosNew.x - (10 - 2 * x), 2) + pow(vertPosNew.z - z, 2));
float d11 = sqrt(pow(vertPosNew.x - (-5 - 2 * x), 2) + pow(vertPosNew.z - (10 - 2 * z), 2));
float d12 = sqrt(pow(vertPosNew.x - x, 2) + pow(vertPosNew.z - (10 - 2 * z), 2));
float d13 = sqrt(pow(vertPosNew.x - (10 - 2 * x), 2) + pow(vertPosNew.z - (10 - 2 * z), 2));


if (rx > sqrt(50)  && waveMode == 1) {
	if (d < rebound1 && d > rebound2) {

		dx = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d21 / lambda - freq * curTime))) / (lambda*d21) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d12 / lambda - freq * curTime))) / (lambda*d12) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d23 / lambda - freq * curTime))) / (lambda*d23) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d32 / lambda - freq * curTime))) / (lambda*d32) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d11 / lambda - freq * curTime))) / (lambda*d11) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d13 / lambda - freq * curTime))) / (lambda*d13) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d31 / lambda - freq * curTime))) / (lambda*d31) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d33 / lambda - freq * curTime))) / (lambda*d33);

		dz = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d21 / lambda - freq * curTime))) / (lambda*d21) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d12 / lambda - freq * curTime))) / (lambda*d12) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d23 / lambda - freq * curTime))) / (lambda*d23) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d32 / lambda - freq * curTime))) / (lambda*d32) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d11 / lambda - freq * curTime))) / (lambda*d11) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d13 / lambda - freq * curTime))) / (lambda*d13) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d31 / lambda - freq * curTime))) / (lambda*d31) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d33 / lambda - freq * curTime))) / (lambda*d33);
	}
	if (d < rebound2) {
		
		dx = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);
		dz = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);
		
	}

	if (d > rebound1 && d < 2 * (sqrt(50)) - d) {

		dx = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d21 / lambda - freq * curTime))) / (lambda*d21) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d12 / lambda - freq * curTime))) / (lambda*d12) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d23 / lambda - freq * curTime))) / (lambda*d23) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d32 / lambda - freq * curTime))) / (lambda*d32) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d11 / lambda - freq * curTime))) / (lambda*d11) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d13 / lambda - freq * curTime))) / (lambda*d13) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d31 / lambda - freq * curTime))) / (lambda*d31) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d33 / lambda - freq * curTime))) / (lambda*d33);

		dz = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d21 / lambda - freq * curTime))) / (lambda*d21) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d12 / lambda - freq * curTime))) / (lambda*d12) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d23 / lambda - freq * curTime))) / (lambda*d23) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d32 / lambda - freq * curTime))) / (lambda*d32) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d11 / lambda - freq * curTime))) / (lambda*d11) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d13 / lambda - freq * curTime))) / (lambda*d13) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d31 / lambda - freq * curTime))) / (lambda*d31) +
			(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d33 / lambda - freq * curTime))) / (lambda*d33);
		
	}
	if (d > 2 * (sqrt(50)) - d) {
		dz = 0;
		dx = 0;
	}

}

else if ((d > rxp && d < rx) && waveMode == 1) {
	//dx = (-2*pi*amp*(vertPosNew.x) * sin(2*pi * (d/lambda - freq*curTime))) / (lambda*d);
	//dz = (-2*pi*amp*(vertPosNew.z) * sin(2*pi * (d/lambda - freq*curTime))) / (lambda*d);
	dx =(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);
	dz =(amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);

}

else if (d < rxp && waveMode == 1) {
	//dx = (-2 * pi*amp/2*(vertPosNew.x) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);
	//dz = (-2 * pi*amp/2*(vertPosNew.z) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);
	dx = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.x) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);
	dz = (amp * pow(exp(1.0), -rz))*(-2 * pi*(vertPosNew.z) * sin(2 * pi * (d / lambda - freq * curTime))) / (lambda*d);
}


mvNormal.x = -dx;
mvNormal.z = -dz;
mvNormal.y = 1;



vec3 normalVec = normalize(inverse(transpose(mat3(modelviewMatrix)))*mvNormal);

    if ( gl_FrontFacing ) {
        mvNormal = normalVec;
    }
    else {
        mvNormal = -normalVec;
    }

    CalculatePhongLighting();       // Calculates: nonspecColor and specularColor. 
    fragmentColor = vec4(nonspecColor+specularColor, 1.0f);   // Add alpha value of 1.0.
    if ( applyTexture ) { 
        fragmentColor = applyTextureFunction();
    }
}
#endglsl

