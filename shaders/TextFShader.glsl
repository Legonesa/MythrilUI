#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text; // The grayscale font character bitmap
uniform vec3 textColor;

void main()
{    
    // FreeType generates grayscale bitmaps (saved in the 'red' channel). 
    // We map that intensity to the alpha channel here.
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}  