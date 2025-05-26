#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform float time;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    // Invert colors
    FragColor = vec4(1.0 - texColor.rgb, texColor.a);
} 