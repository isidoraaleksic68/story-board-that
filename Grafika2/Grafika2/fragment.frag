#version 330 core

// Inputs from vertex shader
in vec4 chCol;        // Color from vertex shader
in vec2 texCoords;    // Texture coordinates from vertex shader

// Outputs to framebuffer
out vec4 outCol;

// Texture sampler
uniform sampler2D texture1; 
uniform bool useTexture; // Uniform to determine if texture is used

void main()
{
    if (useTexture) {
        outCol = texture(texture1, texCoords); // Sample from texture
    } else {
        outCol = chCol; // Use vertex color
    }
}
