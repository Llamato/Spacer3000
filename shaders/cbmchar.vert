#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 tColor;
layout (location = 2) in vec4 bColor;

out vec2 fragCoord;
out vec4 textColor;
out vec4 backgroundColor;

uniform vec2 iResolution;
uniform bool pixels[16];

void main()
{
    float aspect = iResolution.x / iResolution.y;
    vec2 aaPos = aPos.xy;
    aaPos.x /= aspect;
    gl_Position = vec4(aPos.xy, aPos.z, 1.0);
    fragCoord = aPos.xy;
    textColor = tColor;
    backgroundColor = bColor;
}