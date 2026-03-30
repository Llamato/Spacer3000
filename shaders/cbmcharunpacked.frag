#version 330 core
in vec4 textColor;
in vec4 bgColor;

uniform vec2 iResolution;
uniform uint chargen[2048];
uniform uint petsciicode;

out vec4 FragColor;

const uint columns = 8u;
const uint rows = 8u;

bool getPixelValue(uint index) {
    uint char = petsciicode;
    uint column = index % columns;
    uint row = index / rows;
    return (chargen[char * rows + row] & (1u << column)) != 0u;
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 flippedUV = 1.0 - uv;
    vec2 blockCoord = floor(flippedUV * float(columns));
    uint index = uint(blockCoord.y) * columns + uint(blockCoord.x);
    FragColor = mix(bgColor, textColor, float(getPixelValue(index)));
}