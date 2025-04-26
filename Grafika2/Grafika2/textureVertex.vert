#version 330 core

layout(location = 0) in vec2 inPos;     // Vertex position
layout(location = 1) in vec2 inTexCoord; // Texture coordinates
out vec2 TexCoord;                     // Pass to fragment shader
uniform mat4 model;


void main() {
    gl_Position = model * vec4(inPos, 0.0, 1.0);
    TexCoord = inTexCoord; // Pass texture coordinates to the fragment shader
}
