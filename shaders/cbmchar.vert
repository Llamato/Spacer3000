#version 330 core
layout (location = 0) in vec2 aPos;

void main() {
    // Pass the vertex position directly to gl_Position
    // Assumes aPos is already in Normalized Device Coordinates (NDC) range [-1, 1]
    gl_Position = vec4(2*aPos, 0.0, 1.0);
}