#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform float time;

void main()
{
    // Calculate wave effect that expands and contracts
    float wave = (sin(time * 2.0 + aPos.x * 0.1) + 1.0) * 0.02; // Range: 0 to 0.04
    
    // Calculate direction from center
    vec2 center = vec2(0.0, 0.0);
    vec2 dir = normalize(aPos.xy - center);
    
    // Apply wave in the direction from center
    vec3 pos = aPos;
    pos.xy += dir * wave;
    
    // Scale texture coordinates inversely to the vertex movement
    vec2 texCenter = vec2(0.5, 0.5);
    vec2 texDir = aTexCoord - texCenter;
    float scale = 1.0 / (1.0 + wave * 4.0);
    TexCoord = texCenter + texDir * scale;
    
    // Ensure texture coordinates stay within [0,1] range
    TexCoord = clamp(TexCoord, vec2(0.0), vec2(1.0));
    
    gl_Position = vec4(pos, 1.0);
} 