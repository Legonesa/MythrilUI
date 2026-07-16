#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection; // Orthographic projection matrix
uniform vec2 Middle;     // Layout offset pivot

void main()
{
    // Renders the font quad adjusted relative to the pivot center
    gl_Position = projection * vec4(vertex.x + Middle.x/2, Middle.y/2 + vertex.y, 0.0, 1.0);
    TexCoords = vertex.zw;
}  