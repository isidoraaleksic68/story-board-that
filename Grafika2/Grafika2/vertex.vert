#version 330 core

// Input attributes
layout(location = 0) in vec2 inPos;   // Vertex position
layout(location = 1) in vec4 inCol;   // Vertex color
layout(location = 2) in vec2 inTex;   // Texture coordinates

// Outputs to fragment shader
out vec4 chCol;         // Color output
out vec2 texCoords;     // Texture coordinates output

uniform bool useTexture; // Uniform to determine if texture is used

void main()
{
    gl_Position = vec4(inPos.xy, 0.0, 1.0); // Convert 2D position to 4D
    
    if (useTexture) {
        texCoords = inTex; // Pass texture coordinates
    } else {
        chCol = inCol;    // Pass vertex color
    }
}
