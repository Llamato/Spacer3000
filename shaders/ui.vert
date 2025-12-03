#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 screenSize;
uniform float fuelQuantity;

out vec2 fragCoord;

void main() {
    float aspect = screenSize.x / screenSize.y;
    gl_Position = vec4(aPos, 1.0);
    fragCoord = aPos.xy;
}