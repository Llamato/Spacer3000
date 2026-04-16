#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aTextColor;
layout (location = 2) in vec4 aBgColor;

out vec4 textColor;
out vec4 bgColor;

void main() {
    // Pass the vertex position directly to gl_Position
    // Assumes aPos is already in Normalized Device Coordinates (NDC) range [-1, 1]
    vec4 ndc = vec4(2*aPos, 0.0, 1.0);
    gl_Position = ndc;
    textColor = aTextColor;
    bgColor = aBgColor;
}