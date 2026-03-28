#version 330 core
in vec4 textColor;
in vec4 bgColor;

uniform vec2 iResolution;

out vec4 FragColor;

/*const vec4 textColor = vec4(1.0,1.0,1.0,1.0);
const vec4 bgColor = vec4(0.0,1.0,1.0,1.0);*/

const int columns = 8;
const int rows = 8;

bool getPixelValue(int index) {
    int row = index / 8;
    int col = index % 8;
    return (row + col) % 2 == 0;
}

/*bool getPixelValueOverTime(int index) {
    return ((int(iTime) % 64) & (1 << index)) != 0;
}*/

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 blockCoord = floor(uv * float(columns));
    int index = int(blockCoord.y) * columns + int(blockCoord.x);
    
    if(index >= 0 && index < columns * rows) {
        FragColor = mix(bgColor, textColor, float(getPixelValue(index)));
    } else {
        FragColor = bgColor;
    }
}