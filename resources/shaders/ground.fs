#version 330 core

out vec4 FragColor;

in vec2 outTexCoords;

uniform sampler2D t0;
uniform sampler2D t1;

void main(){
    FragColor = mix(texture(t0, outTexCoords), texture(t1, outTexCoords), 0.3);
}


