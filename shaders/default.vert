#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform vec2 cameraPos;
uniform vec2 screenSize;
uniform float zoom;
out vec3 color;

void main()
{
    float aspect = screenSize.x / screenSize.y;
    vec2 viewPos = (aPos.xy - cameraPos) * zoom;
    viewPos.x /= aspect;
    gl_Position = vec4(viewPos, aPos.z, 1.0);
    color = aColor;
}