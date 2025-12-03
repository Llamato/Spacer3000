#version 330 core
out vec4 fragColor;

in vec2 fragCoord;

uniform vec2 screenSize;
uniform float fuelQuantity;

void main() {
   vec2 uv = fragCoord;
    vec3 fuelEmptyColor = vec3(1.0, 0.0, 0.0);
    vec3 fuelFullColor = vec3(0.0, 1.0, 0.0);
    fragColor = vec4(mix(fuelEmptyColor, fuelFullColor, uv.x), step(0.9, uv.y));
}