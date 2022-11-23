#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureT;

void main() {
    FragColor = texture(textureT, TexCoord);
}
