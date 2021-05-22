#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D pipeTexture;

void main(){
   FragColor = texture(pipeTexture, TexCoord);
}