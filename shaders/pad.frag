#version 330 core
out vec4 fragColor;

in vec2 fragCoord;

uniform vec2 cameraPos;
uniform vec2 screenSize;
uniform float zoom;

void main()
{
    // Convert to world coordinates and normalize
    vec2 worldCoord = fragCoord + cameraPos;
    vec2 uv = worldCoord / screenSize * zoom;
    
    // Your original Shadertoy pattern
    float bw = step(1.0, mod(uv.x * 10.0, 2.0));
    fragColor = vec4(1.0, bw, bw, 1.0); // Red or white
}