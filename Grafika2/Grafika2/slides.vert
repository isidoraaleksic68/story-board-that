#version 330 core

layout(location = 0) in vec2 inPos;   
layout(location = 1) in vec4 inCol;   
layout(location = 2) in vec2 inTex;   

out vec4 chCol;         
out vec2 texCoords;     

uniform bool useTexture; 
uniform mat4 model;  // Transformation matrix

void main()
{
    gl_Position = model * vec4(inPos.xy, 0.0, 1.0); // Apply transformation

    if (useTexture) {
        texCoords = inTex; 
    } else {
        chCol = inCol;
    }
}
