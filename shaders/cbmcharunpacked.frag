#version 330 core
out vec4 FragColor;

uniform vec2 iResolution;

const vec4 textColor = vec4(1.0,1.0,1.0,1.0);
const vec4 bgColor = vec4(0.0,1.0,1.0,1.0);

bool getPixelValue(int index) {
    int row = index / 8;
    int col = index % 8;
    return (row + col) % 2 == 0;
}

void main() {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    
    int blocksPerRow = 8;
    vec2 blockCoord = floor(uv * float(blocksPerRow));
    int index = int(blockCoord.y) * blocksPerRow + int(blockCoord.x);
    
    if(index >= 0 && index < 64) {
        FragColor = mix(bgColor, textColor, float(getPixelValue(index)));
    } else {
        FragColor = bgColor;
    }
}