#version 330 core
out vec4 fragColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main(){
    fragColor = texture(texture1, TexCoord);

    // Discard highly transparent pixels to prevent depth/overlap bugs in UI rendering
    if(fragColor.a <= 0.8){
        discard;
    }
}