#version 330 core

uniform vec2 iResolution;
uniform uint chargen[2048];
uniform vec4 colorPallet[16];

uniform uint screen[1000];
uniform uint colors[1000];


out vec4 FragColor;

const uint charColumns = 8u;
const uint charRows = 8u;

const uint screenColumns = 40u;
const uint screenRows = 25u;

const vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);

bool getPixel(uint charScreenIndex, uint pixelCharIndex) {
    uint char = screen[charScreenIndex];
    uint column = pixelCharIndex % charColumns;
    uint oppositeColumn = (charColumns - 1u) - column;
    uint row = pixelCharIndex / charRows;
    return (chargen[char * charRows + row] & (1u << oppositeColumn)) != 0u;
}

vec4 getColor(uint charScreenIndex) {
    uint colorPalletIndex = colors[charScreenIndex];
    return colorPallet[colorPalletIndex];
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 mirroredUV = vec2(uv.x, 1.0 - uv.y);

    vec2 screenCoord = vec2(mirroredUV.x * screenColumns, mirroredUV.y * screenRows); 
    uint screenIndex = uint(floor(screenCoord.y) * screenColumns + floor(screenCoord.x));
    
    vec2 charCoord = vec2(fract(screenCoord.x) * float(charColumns), fract(screenCoord.y) * float(charRows));
    uint charIndex = uint(floor(charCoord.y) * float(charColumns) + floor(charCoord.x));
    vec4 fragColor = mix(bgColor, getColor(screenIndex), float(getPixel(screenIndex, charIndex)));
    if (fragColor.a < 0.1) discard;
    FragColor = fragColor;
}