#version 330 core
layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 TextureCoord;

out vec2 TexCoord;
uniform mat4 wideScreen; // Aspect ratio correction matrix

void main(){
    // Transform raw vertices against our aspect-ratio logic
    gl_Position = wideScreen * vec4(Pos, 1.0f);
    TexCoord = TextureCoord;
}