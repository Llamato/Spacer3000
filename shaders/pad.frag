#version 330 core
out vec4 fragColor;

in vec2 fragCoord;

uniform vec2 cameraPos;
uniform vec2 screenSize;
uniform float zoom;

void main()
{
    vec2 uv = fragCoord;
    float rw = step(1.0, mod(uv.x * 10.0, 2.0));
    fragColor = vec4(1.0, rw, rw, 1.0);
}