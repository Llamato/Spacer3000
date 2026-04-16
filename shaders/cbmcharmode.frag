#version 330 core
in vec4 textColor;
in vec4 bgColor;

uniform vec2 iResolution;
uniform uint chargen[2048];
uniform u8 screen[1000];

out vec4 FragColor;

const uint charColumns = 8u;
const uint charRows = 8u;

const uint screenColumns = 40u;
const uint screenRows = 25u;

bool getPixelValue(uint charScreenIndex, uint pixelCharIndex) {
    uint char = screen[charScreenIndex];
    uint column = pixelCharIndex % charColumns;
    uint row = pixelCharIndex / charRows;
    return (chargen[char * charRows + row] & (1u << column)) != 0u;
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 flippedUV = 1.0 - uv;

    vec2 screenCoord = vec2(flippedUV.x * screenColumns, flippedUV.y * screenRows); 
    uint screenIndex = floor(screenCoord.y) * screenColumns + floor(screenCoord.x)
    
    vec2 charCoord = vec2(fract(screenCoord.x) * float(charColumns), fract(screenCoord.y) * float(charRows));
    uint charIndex = floor(charCoord.y) * charColumns + floor(charCoord.x);
    
    FragColor = mix(bgColor, textColor, float(getPixelValue(screenIndex, charIndex)));
}