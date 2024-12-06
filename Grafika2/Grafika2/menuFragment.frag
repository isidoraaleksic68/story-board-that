#version 330 core

in vec2 TexCoord;              // Received from the vertex shader
out vec4 FragColor;            // Final fragment color
uniform sampler2D texture1;    // The texture sampler
uniform sampler2D texture2;

void main() {
    FragColor = texture(texture1, TexCoord); // Sample the texture
    FragColor = texture(texture2, TexCoord); // Sample the texture
}
