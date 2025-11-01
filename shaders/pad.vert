#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 cameraPos;
uniform vec2 screenSize;
uniform float zoom;

out vec2 fragCoord;

void main()
{
    float aspect = screenSize.x / screenSize.y;
    vec2 viewPos = (aPos.xy - cameraPos) * zoom;
    viewPos.x /= aspect;
    gl_Position = vec4(viewPos, aPos.z, 1.0);
    fragCoord = aPos.xy;
}