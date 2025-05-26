#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform float time;

#define TAU 6.28318530718
#define MAX_ITER 5

void main()
{
    // Sample the texture
    vec4 texColor = texture(texture1, TexCoord);
    
    // Discard nearly transparent pixels
    if(texColor.a < 0.1)
        discard;

    float t = time * 0.5 + 23.0;
    // Scale TexCoord to control the size of the effect
    vec2 uv = TexCoord * 16.0; // Adjust 16.0 to match your map width in tiles for seamless tiling
    
    // Use mod to create tiling
    vec2 p = mod(uv * TAU, TAU) - 250.0;
    vec2 i = vec2(p);
    float c = 1.0;
    float inten = 0.005;

    for (int n = 0; n < MAX_ITER; n++) 
    {
        float t = time * (1.0 - (3.5 / float(n+1)));
        i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
        c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
    }
    c /= float(MAX_ITER);
    c = 1.17-pow(c, 1.4);
    vec3 waterColor = vec3(pow(abs(c), 8.0));
    waterColor = clamp(waterColor + vec3(0.0, 0.35, 0.5), 0.0, 1.0);

    // Mix the texture with the water effect
    vec3 finalColor = mix(texColor.rgb, waterColor, 0.3);
    
    FragColor = vec4(finalColor, texColor.a);
} 